use std::collections::{BTreeMap, BTreeSet};

use crate::ast::*;
use crate::diagnostic::Diagnostic;
use crate::sema::{self, Builtin, CheckedProgram};
use crate::span::{Source, Span};

use super::{LoadedProject, ProjectDiagnostic, ProjectRelated};

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum DeclarationClass {
    Function,
    Type,
}

#[derive(Clone, Debug)]
pub(super) struct ModuleLocation {
    module: String,
    source: Source,
    base: usize,
    end: usize,
}

#[derive(Clone, Debug)]
pub struct ResolvedProject {
    pub(super) program: Program,
    pub(super) entry: String,
    pub(super) locations: Vec<ModuleLocation>,
    layers: Vec<Vec<String>>,
    module_declarations: BTreeMap<String, BTreeSet<String>>,
}

pub fn resolve(project: &LoadedProject) -> Result<ResolvedProject, Vec<ProjectDiagnostic>> {
    let catalog: BTreeMap<_, BTreeMap<_, _>> = project
        .modules
        .iter()
        .map(|(module, loaded)| {
            let declarations = loaded
                .program
                .items
                .iter()
                .map(|item| {
                    let (name, class) = match item {
                        Item::Function(function) => {
                            (function.name.clone(), DeclarationClass::Function)
                        }
                        Item::Record(record) => (record.name.clone(), DeclarationClass::Type),
                        Item::Variant(variant) => (variant.name.clone(), DeclarationClass::Type),
                    };
                    (name, class)
                })
                .collect();
            (module.clone(), declarations)
        })
        .collect();
    let exports: BTreeMap<_, BTreeSet<_>> = project
        .manifest
        .modules
        .iter()
        .map(|module| {
            (
                module.identity.value.clone(),
                module
                    .exports
                    .iter()
                    .map(|export| export.value.clone())
                    .collect(),
            )
        })
        .collect();
    let imports: BTreeMap<_, BTreeSet<_>> = project
        .manifest
        .modules
        .iter()
        .map(|module| {
            (
                module.identity.value.clone(),
                module
                    .imports
                    .iter()
                    .map(|import| import.value.clone())
                    .collect(),
            )
        })
        .collect();

    let mut locations = Vec::new();
    let mut next_base = 0;
    for spec in &project.manifest.modules {
        let source = project
            .modules
            .get(&spec.identity.value)
            .expect("loaded module exists")
            .source
            .clone();
        let end = next_base + source.text.len();
        locations.push(ModuleLocation {
            module: spec.identity.value.clone(),
            source,
            base: next_base,
            end,
        });
        next_base = end + 1;
    }

    let mut diagnostics = Vec::new();
    let mut resolved_by_module = BTreeMap::new();
    for spec in &project.manifest.modules {
        let loaded = project
            .modules
            .get(&spec.identity.value)
            .expect("loaded module exists");
        let mut program = loaded.program.clone();
        for item in &mut program.items {
            resolve_item(
                item,
                &spec.identity.value,
                &catalog,
                &imports,
                &exports,
                &loaded.source,
                &mut diagnostics,
            );
        }
        resolved_by_module.insert(spec.identity.value.clone(), program.items);
    }
    if !diagnostics.is_empty() {
        sort_project_diagnostics(&mut diagnostics);
        return Err(diagnostics);
    }

    let layers = project.topological_layers();
    let mut items = Vec::new();
    let mut module_declarations = BTreeMap::new();
    for layer in &layers {
        for module in layer {
            let location = locations
                .iter()
                .find(|location| &location.module == module)
                .expect("module location exists");
            let module_items = resolved_by_module
                .remove(module)
                .expect("resolved module exists");
            module_declarations.insert(
                module.clone(),
                module_items
                    .iter()
                    .map(|item| resolved_item_name(item).to_owned())
                    .collect(),
            );
            for mut item in module_items {
                shift_item(&mut item, location.base as isize);
                items.push(item);
            }
        }
    }
    Ok(ResolvedProject {
        program: Program {
            name: "project".to_owned(),
            items,
            span: Span::new(0, next_base.saturating_sub(1)),
        },
        entry: format!("{}/main", project.manifest.entry.value),
        locations,
        layers,
        module_declarations,
    })
}

pub fn check(resolved: ResolvedProject) -> (Option<CheckedProgram>, Vec<ProjectDiagnostic>) {
    check_with_jobs(resolved, 1)
}

pub fn check_with_jobs(
    resolved: ResolvedProject,
    jobs: usize,
) -> (Option<CheckedProgram>, Vec<ProjectDiagnostic>) {
    let hardware = std::thread::available_parallelism()
        .map(usize::from)
        .unwrap_or(1);
    let jobs = jobs.max(1).min(hardware);
    let ResolvedProject {
        mut program,
        entry,
        locations,
        layers,
        module_declarations,
    } = resolved;
    if jobs == 1 {
        let (checked, diagnostics) = sema::check_with_entry(program, &entry);
        let mut diagnostics = diagnostics
            .into_iter()
            .map(|diagnostic| locate_diagnostic(&locations, diagnostic))
            .collect::<Vec<_>>();
        sort_project_diagnostics(&mut diagnostics);
        return (checked, diagnostics);
    }
    let empty = BTreeSet::new();
    let (_, entry_diagnostics) = sema::check_selected_with_entry(program.clone(), &empty, &entry);
    if !entry_diagnostics.is_empty() {
        let mut diagnostics = entry_diagnostics
            .into_iter()
            .map(|diagnostic| locate_diagnostic(&locations, diagnostic))
            .collect::<Vec<_>>();
        sort_project_diagnostics(&mut diagnostics);
        return (None, diagnostics);
    }

    let mut all_diagnostics = Vec::new();
    for layer in &layers {
        for batch in layer.chunks(jobs) {
            let results = std::thread::scope(|scope| {
                let handles = batch
                    .iter()
                    .map(|module| {
                        let selected = module_declarations[module].clone();
                        let candidate = program.clone();
                        scope.spawn(move || {
                            let result = sema::check_selected_without_entry(candidate, &selected);
                            (selected, result)
                        })
                    })
                    .collect::<Vec<_>>();
                handles
                    .into_iter()
                    .map(|handle| handle.join().expect("project checker worker panicked"))
                    .collect::<Vec<_>>()
            });
            for (selected, (checked, diagnostics)) in results {
                all_diagnostics.extend(diagnostics);
                let Some(checked) = checked else {
                    continue;
                };
                let replacements: BTreeMap<_, _> = checked
                    .program
                    .items
                    .into_iter()
                    .filter(|item| selected.contains(resolved_item_name(item)))
                    .map(|item| (resolved_item_name(&item).to_owned(), item))
                    .collect();
                for item in &mut program.items {
                    if let Some(replacement) = replacements.get(resolved_item_name(item)) {
                        *item = replacement.clone();
                    }
                }
            }
        }
    }
    if !all_diagnostics.is_empty() {
        let mut diagnostics = all_diagnostics
            .into_iter()
            .map(|diagnostic| locate_diagnostic(&locations, diagnostic))
            .collect::<Vec<_>>();
        sort_project_diagnostics(&mut diagnostics);
        return (None, diagnostics);
    }
    let (checked, diagnostics) = sema::check_selected_with_entry(program, &empty, &entry);
    let mut diagnostics = diagnostics
        .into_iter()
        .map(|diagnostic| locate_diagnostic(&locations, diagnostic))
        .collect::<Vec<_>>();
    sort_project_diagnostics(&mut diagnostics);
    (checked, diagnostics)
}

#[allow(clippy::too_many_arguments)]
fn resolve_item(
    item: &mut Item,
    module: &str,
    catalog: &BTreeMap<String, BTreeMap<String, DeclarationClass>>,
    imports: &BTreeMap<String, BTreeSet<String>>,
    exports: &BTreeMap<String, BTreeSet<String>>,
    source: &Source,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) {
    match item {
        Item::Function(function) => {
            for parameter in &mut function.params {
                resolve_type(
                    &mut parameter.ty,
                    parameter.span,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
            resolve_type(
                &mut function.return_type,
                function.span,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            resolve_expr(
                &mut function.body,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            function.name = global_name(module, &function.name);
        }
        Item::Record(record) => {
            for field in &mut record.fields {
                resolve_type(
                    &mut field.ty,
                    field.span,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
            record.name = global_name(module, &record.name);
        }
        Item::Variant(variant) => {
            for case in &mut variant.cases {
                for ty in &mut case.payload {
                    resolve_type(
                        ty,
                        case.span,
                        module,
                        catalog,
                        imports,
                        exports,
                        source,
                        diagnostics,
                    );
                }
            }
            variant.name = global_name(module, &variant.name);
        }
    }
}

#[allow(clippy::too_many_arguments)]
fn resolve_type(
    ty: &mut Type,
    span: Span,
    module: &str,
    catalog: &BTreeMap<String, BTreeMap<String, DeclarationClass>>,
    imports: &BTreeMap<String, BTreeSet<String>>,
    exports: &BTreeMap<String, BTreeSet<String>>,
    source: &Source,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) {
    match ty {
        Type::Named(name) => {
            *name = resolve_reference(
                name,
                DeclarationClass::Type,
                span,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
        }
        Type::Vec(inner) | Type::Arena(inner) | Type::Id(inner) => resolve_type(
            inner,
            span,
            module,
            catalog,
            imports,
            exports,
            source,
            diagnostics,
        ),
        Type::Unit | Type::Bool | Type::U8 | Type::I64 | Type::Bytes | Type::Error => {}
    }
}

#[allow(clippy::too_many_arguments)]
fn resolve_expr(
    expr: &mut Expr,
    module: &str,
    catalog: &BTreeMap<String, BTreeMap<String, DeclarationClass>>,
    imports: &BTreeMap<String, BTreeSet<String>>,
    exports: &BTreeMap<String, BTreeSet<String>>,
    source: &Source,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) {
    match &mut expr.kind {
        ExprKind::Let {
            declared_type,
            value,
            body,
            ..
        } => {
            resolve_type(
                declared_type,
                expr.span,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            resolve_expr(
                value,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            resolve_expr(body, module, catalog, imports, exports, source, diagnostics);
        }
        ExprKind::Call {
            function,
            arguments,
        } => {
            if Builtin::from_name(function).is_none() {
                *function = resolve_reference(
                    function,
                    DeclarationClass::Function,
                    expr.span,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
            for argument in arguments {
                resolve_expr(
                    argument,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
        }
        ExprKind::Recur(arguments) => {
            for argument in arguments {
                resolve_expr(
                    argument,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
        }
        ExprKind::Set { value, body, .. } => {
            resolve_expr(
                value,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            resolve_expr(body, module, catalog, imports, exports, source, diagnostics);
        }
        ExprKind::Make { record, fields } => {
            *record = resolve_reference(
                record,
                DeclarationClass::Type,
                expr.span,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            for (_, value, _) in fields {
                resolve_expr(
                    value,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
        }
        ExprKind::Get { value, .. } => resolve_expr(
            value,
            module,
            catalog,
            imports,
            exports,
            source,
            diagnostics,
        ),
        ExprKind::Case {
            variant, payload, ..
        } => {
            *variant = resolve_reference(
                variant,
                DeclarationClass::Type,
                expr.span,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            for value in payload {
                resolve_expr(
                    value,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
        }
        ExprKind::Match { value, arms } => {
            resolve_expr(
                value,
                module,
                catalog,
                imports,
                exports,
                source,
                diagnostics,
            );
            for arm in arms {
                resolve_expr(
                    &mut arm.body,
                    module,
                    catalog,
                    imports,
                    exports,
                    source,
                    diagnostics,
                );
            }
        }
        ExprKind::Unit
        | ExprKind::Bool(_)
        | ExprKind::I64(_)
        | ExprKind::Bytes(_)
        | ExprKind::Name(_)
        | ExprKind::Error => {}
    }
}

#[allow(clippy::too_many_arguments)]
fn resolve_reference(
    reference: &str,
    expected: DeclarationClass,
    span: Span,
    module: &str,
    catalog: &BTreeMap<String, BTreeMap<String, DeclarationClass>>,
    imports: &BTreeMap<String, BTreeSet<String>>,
    exports: &BTreeMap<String, BTreeSet<String>>,
    source: &Source,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) -> String {
    let (owner, name) = if let Some((owner, name)) = reference.split_once('/') {
        if owner == module {
            diagnostics.push(ProjectDiagnostic::local(
                Some(module.to_owned()),
                source.clone(),
                Diagnostic::error(
                    "E0417",
                    format!("module `{module}` must not qualify local `{reference}`"),
                    span,
                ),
            ));
            return reference.to_owned();
        }
        if !imports[module].contains(owner) {
            diagnostics.push(ProjectDiagnostic::local(
                Some(module.to_owned()),
                source.clone(),
                Diagnostic::error(
                    "E0416",
                    format!("module `{module}` does not directly import `{owner}`"),
                    span,
                ),
            ));
            return reference.to_owned();
        }
        if !exports[owner].contains(name) {
            diagnostics.push(ProjectDiagnostic::local(
                Some(module.to_owned()),
                source.clone(),
                Diagnostic::error(
                    "E0415",
                    format!("declaration `{owner}/{name}` is not exported"),
                    span,
                ),
            ));
            return reference.to_owned();
        }
        (owner, name)
    } else {
        (module, reference)
    };
    match catalog.get(owner).and_then(|items| items.get(name)) {
        Some(actual) if *actual == expected => {}
        Some(_) => diagnostics.push(ProjectDiagnostic::local(
            Some(module.to_owned()),
            source.clone(),
            Diagnostic::error(
                "E0415",
                format!("`{owner}/{name}` has the wrong declaration kind"),
                span,
            ),
        )),
        None => diagnostics.push(ProjectDiagnostic::local(
            Some(module.to_owned()),
            source.clone(),
            Diagnostic::error(
                "E0415",
                format!("unknown declaration `{owner}/{name}`"),
                span,
            ),
        )),
    }
    global_name(owner, name)
}

fn global_name(module: &str, name: &str) -> String {
    format!("{module}/{name}")
}

fn resolved_item_name(item: &Item) -> &str {
    match item {
        Item::Function(function) => &function.name,
        Item::Record(record) => &record.name,
        Item::Variant(variant) => &variant.name,
    }
}

pub(super) fn locate_diagnostic(
    locations: &[ModuleLocation],
    mut diagnostic: Diagnostic,
) -> ProjectDiagnostic {
    let primary_location =
        find_location(locations, diagnostic.primary).unwrap_or_else(|| &locations[0]);
    rebase_span(&mut diagnostic.primary, primary_location.base);
    let mut local_labels = Vec::new();
    let mut related = Vec::new();
    for mut label in std::mem::take(&mut diagnostic.labels) {
        let location = find_location(locations, label.span).unwrap_or(primary_location);
        rebase_span(&mut label.span, location.base);
        if location.module == primary_location.module {
            local_labels.push(label);
        } else {
            related.push(ProjectRelated {
                module: location.module.clone(),
                source: location.source.clone(),
                span: label.span,
                message: label.message,
            });
        }
    }
    diagnostic.labels = local_labels;
    for fix in &mut diagnostic.fixes {
        let location = find_location(locations, fix.span).unwrap_or(primary_location);
        rebase_span(&mut fix.span, location.base);
    }
    ProjectDiagnostic {
        module: Some(primary_location.module.clone()),
        source: primary_location.source.clone(),
        diagnostic,
        related,
    }
}

fn find_location(locations: &[ModuleLocation], span: Span) -> Option<&ModuleLocation> {
    locations
        .iter()
        .find(|location| span.start >= location.base && span.start <= location.end)
}

fn rebase_span(span: &mut Span, base: usize) {
    span.start = span.start.saturating_sub(base);
    span.end = span.end.saturating_sub(base);
}

pub(super) fn sort_project_diagnostics(diagnostics: &mut [ProjectDiagnostic]) {
    diagnostics.sort_by(|left, right| {
        left.module
            .cmp(&right.module)
            .then_with(|| {
                left.diagnostic
                    .primary
                    .start
                    .cmp(&right.diagnostic.primary.start)
            })
            .then_with(|| left.diagnostic.code.cmp(right.diagnostic.code))
    });
}

fn shift_item(item: &mut Item, delta: isize) {
    match item {
        Item::Function(function) => {
            shift_span(&mut function.span, delta);
            for parameter in &mut function.params {
                shift_span(&mut parameter.span, delta);
            }
            shift_expr(&mut function.body, delta);
        }
        Item::Record(record) => {
            shift_span(&mut record.span, delta);
            for field in &mut record.fields {
                shift_span(&mut field.span, delta);
            }
        }
        Item::Variant(variant) => {
            shift_span(&mut variant.span, delta);
            for case in &mut variant.cases {
                shift_span(&mut case.span, delta);
            }
        }
    }
}

fn shift_expr(expr: &mut Expr, delta: isize) {
    shift_span(&mut expr.span, delta);
    match &mut expr.kind {
        ExprKind::Let { value, body, .. } | ExprKind::Set { value, body, .. } => {
            shift_expr(value, delta);
            shift_expr(body, delta);
        }
        ExprKind::Call { arguments, .. } | ExprKind::Recur(arguments) => {
            for argument in arguments {
                shift_expr(argument, delta);
            }
        }
        ExprKind::Make { fields, .. } => {
            for (_, value, field_span) in fields {
                shift_expr(value, delta);
                shift_span(field_span, delta);
            }
        }
        ExprKind::Get { value, .. } => shift_expr(value, delta),
        ExprKind::Case { payload, .. } => {
            for value in payload {
                shift_expr(value, delta);
            }
        }
        ExprKind::Match { value, arms } => {
            shift_expr(value, delta);
            for arm in arms {
                shift_span(&mut arm.span, delta);
                shift_expr(&mut arm.body, delta);
            }
        }
        ExprKind::Unit
        | ExprKind::Bool(_)
        | ExprKind::I64(_)
        | ExprKind::Bytes(_)
        | ExprKind::Name(_)
        | ExprKind::Error => {}
    }
}

fn shift_span(span: &mut Span, delta: isize) {
    span.start = span.start.saturating_add_signed(delta);
    span.end = span.end.saturating_add_signed(delta);
}

#[cfg(test)]
mod tests {
    use std::fs;
    use std::sync::atomic::{AtomicU64, Ordering};

    use crate::span::Source;

    use super::*;

    static NEXT_PROJECT: AtomicU64 = AtomicU64::new(0);

    fn loaded_project(app_body: &str, math_exports: &str) -> LoadedProject {
        let root = std::env::temp_dir().join(format!(
            "slim-resolver-test-{}-{}",
            std::process::id(),
            NEXT_PROJECT.fetch_add(1, Ordering::Relaxed)
        ));
        let _ = fs::remove_dir_all(&root);
        fs::create_dir(&root).unwrap();
        fs::write(
            root.join("app.slim"),
            format!("(module app (fn main ((args (Vec Bytes))) I64 (effects) {app_body}))"),
        )
        .unwrap();
        fs::write(
            root.join("math.slim"),
            "(module math (fn answer ((value I64)) I64 (effects) (call i64.add value 2)))",
        )
        .unwrap();
        let loaded = super::super::load(Source::new(
            root.join("slim.project"),
            format!("(project 1 (entry app) (module app \"app.slim\" (imports math) (exports)) (module math \"math.slim\" (imports) (exports {math_exports})))"),
        ))
        .unwrap();
        fs::remove_dir_all(root).unwrap();
        loaded
    }

    fn wide_project(app_body: &str) -> LoadedProject {
        let root = std::env::temp_dir().join(format!(
            "slim-resolver-wide-test-{}-{}",
            std::process::id(),
            NEXT_PROJECT.fetch_add(1, Ordering::Relaxed)
        ));
        let _ = fs::remove_dir_all(&root);
        fs::create_dir(&root).unwrap();
        fs::write(
            root.join("app.slim"),
            format!("(module app (fn main ((args (Vec Bytes))) I64 (effects) {app_body}))"),
        )
        .unwrap();
        for (module, value) in [("a", 1), ("b", 2), ("c", 3)] {
            fs::write(
                root.join(format!("{module}.slim")),
                format!("(module {module} (fn value () I64 (effects) {value}))"),
            )
            .unwrap();
        }
        let loaded = super::super::load(Source::new(
            root.join("slim.project"),
            "(project 1 (entry app) (module a \"a.slim\" (imports) (exports value)) (module app \"app.slim\" (imports a b c) (exports)) (module b \"b.slim\" (imports) (exports value)) (module c \"c.slim\" (imports) (exports value)))"
                .to_owned(),
        ))
        .unwrap();
        fs::remove_dir_all(root).unwrap();
        loaded
    }

    fn diagnostic_identity(diagnostics: &[ProjectDiagnostic]) -> Vec<(String, usize, String)> {
        diagnostics
            .iter()
            .map(|diagnostic| {
                (
                    diagnostic.module.clone().unwrap_or_default(),
                    diagnostic.diagnostic.primary.start,
                    diagnostic.diagnostic.code.to_owned(),
                )
            })
            .collect()
    }

    #[test]
    fn resolves_exported_qualified_call_and_checks_core_semantics() {
        let loaded = loaded_project("(call math/answer 40)", "answer");
        let resolved = resolve(&loaded).unwrap();
        assert_eq!(resolved.entry, "app/main");
        let (checked, diagnostics) = check(resolved);
        assert!(diagnostics.is_empty(), "{diagnostics:#?}");
        assert!(checked.is_some());
    }

    #[test]
    fn rejects_private_qualified_call_at_call_span() {
        let loaded = loaded_project("(call math/answer 40)", "");
        let errors = resolve(&loaded).unwrap_err();
        assert_eq!(errors[0].diagnostic.code, "E0415");
        assert_eq!(errors[0].source.path().file_name().unwrap(), "app.slim");
    }

    #[test]
    fn preserves_cross_file_declaration_label_identity() {
        let loaded = loaded_project("(call math/answer)", "answer");
        let resolved = resolve(&loaded).unwrap();
        let (_, diagnostics) = check(resolved);
        let arity = diagnostics
            .iter()
            .find(|diagnostic| diagnostic.diagnostic.code == "E0317")
            .unwrap();
        assert_eq!(arity.source.path().file_name().unwrap(), "app.slim");
        assert_eq!(arity.related.len(), 1);
        assert_eq!(
            arity.related[0].source.path().file_name().unwrap(),
            "math.slim"
        );
    }

    #[test]
    fn worker_counts_match_serial_oracle_for_output_and_errors() {
        let valid = wide_project(
            "(call i64.add (call a/value) (call i64.add (call b/value) (call c/value)))",
        );
        let mut generated = Vec::new();
        for jobs in [1, 2, 4, usize::MAX] {
            let resolved = resolve(&valid).unwrap();
            let entry = resolved.entry.clone();
            let (checked, diagnostics) = check_with_jobs(resolved, jobs);
            assert!(diagnostics.is_empty(), "jobs={jobs}: {diagnostics:#?}");
            generated.push(crate::codegen::generate_c_for_entry(
                &checked.unwrap(),
                &entry,
            ));
        }
        assert!(generated.windows(2).all(|pair| pair[0] == pair[1]));

        let invalid = wide_project("(call i64.add (call a/value) true)");
        let mut identities = Vec::new();
        for jobs in [1, 2, 4, usize::MAX] {
            let (_, diagnostics) = check_with_jobs(resolve(&invalid).unwrap(), jobs);
            assert!(!diagnostics.is_empty());
            identities.push(diagnostic_identity(&diagnostics));
        }
        assert!(identities.windows(2).all(|pair| pair[0] == pair[1]));
    }
}
