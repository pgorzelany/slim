use std::collections::{BTreeMap, BTreeSet};
use std::path::Path;

use crate::diagnostic::Diagnostic;
use crate::lexer;
use crate::sexpr::{self, SExpr, SExprKind};
use crate::span::{Source, Span};

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct SpannedName {
    pub value: String,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub struct ModuleSpec {
    pub identity: SpannedName,
    pub path: String,
    pub path_span: Span,
    pub imports: Vec<SpannedName>,
    pub exports: Vec<SpannedName>,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub struct ProjectManifest {
    pub entry: SpannedName,
    pub modules: Vec<ModuleSpec>,
    pub span: Span,
}

impl ProjectManifest {
    pub fn module(&self, identity: &str) -> Option<&ModuleSpec> {
        self.modules
            .binary_search_by_key(&identity, |module| module.identity.value.as_str())
            .ok()
            .map(|index| &self.modules[index])
    }
}

pub fn parse(source: &Source) -> Result<ProjectManifest, Vec<Diagnostic>> {
    let (tokens, mut diagnostics) = lexer::lex(&source.text);
    let (forms, parse_diagnostics) = sexpr::parse(&tokens, source.text.len());
    diagnostics.extend(parse_diagnostics);
    if forms.is_empty() {
        diagnostics.push(Diagnostic::error(
            "E0401",
            "expected one project manifest form",
            Span::default(),
        ));
        return Err(sorted(diagnostics));
    }
    if forms.len() > 1 {
        for extra in &forms[1..] {
            diagnostics.push(Diagnostic::error(
                "E0401",
                "manifest may contain only one project form",
                extra.span,
            ));
        }
    }
    let form = &forms[0];
    let Some(elements) = list(form) else {
        diagnostics.push(Diagnostic::error(
            "E0401",
            "project manifest must be a form",
            form.span,
        ));
        return Err(sorted(diagnostics));
    };
    if elements.first().and_then(atom) != Some("project") {
        diagnostics.push(
            Diagnostic::error("E0401", "manifest must start with `project`", form.span)
                .note("write (project 1 (entry MODULE) (module ...)+)"),
        );
    }
    if elements.get(1).and_then(atom) != Some("1") {
        let span = elements.get(1).map_or(form.span, |element| element.span);
        diagnostics.push(Diagnostic::error(
            "E0402",
            "unsupported project manifest version; expected `1`",
            span,
        ));
    }

    let entry = elements
        .get(2)
        .and_then(|entry| parse_entry(entry, &mut diagnostics));
    let mut modules = Vec::new();
    for module in elements.iter().skip(3) {
        if let Some(module) = parse_module(module, &mut diagnostics) {
            modules.push(module);
        }
    }
    if modules.is_empty() {
        diagnostics.push(Diagnostic::error(
            "E0404",
            "project requires at least one module clause",
            form.span,
        ));
    }

    validate_modules(&modules, entry.as_ref(), &mut diagnostics);
    let Some(entry) = entry else {
        return Err(sorted(diagnostics));
    };
    if diagnostics.is_empty() {
        Ok(ProjectManifest {
            entry,
            modules,
            span: form.span,
        })
    } else {
        Err(sorted(diagnostics))
    }
}

fn parse_entry(form: &SExpr, diagnostics: &mut Vec<Diagnostic>) -> Option<SpannedName> {
    let Some(elements) = list(form) else {
        diagnostics.push(Diagnostic::error(
            "E0403",
            "entry must be (entry MODULE)",
            form.span,
        ));
        return None;
    };
    if elements.len() != 2 || elements.first().and_then(atom) != Some("entry") {
        diagnostics.push(Diagnostic::error(
            "E0403",
            "entry must be exactly (entry MODULE)",
            form.span,
        ));
        return None;
    }
    parse_name(&elements[1], "entry module", diagnostics)
}

fn parse_module(form: &SExpr, diagnostics: &mut Vec<Diagnostic>) -> Option<ModuleSpec> {
    let Some(elements) = list(form) else {
        diagnostics.push(Diagnostic::error(
            "E0404",
            "module clause must be a form",
            form.span,
        ));
        return None;
    };
    if elements.len() != 5 || elements.first().and_then(atom) != Some("module") {
        diagnostics.push(
            Diagnostic::error(
                "E0404",
                "module must be (module NAME \"path.slim\" (imports ...) (exports ...))",
                form.span,
            )
            .note("module clause fields have one canonical order"),
        );
        return None;
    }
    let identity = parse_name(&elements[1], "module identity", diagnostics)?;
    let path = match &elements[2].kind {
        SExprKind::Bytes(bytes) => match String::from_utf8(bytes.clone()) {
            Ok(path) if valid_relative_path(&path) => path,
            Ok(path) => {
                diagnostics.push(Diagnostic::error(
                    "E0407",
                    format!("invalid confined module path `{path}`"),
                    elements[2].span,
                ));
                return None;
            }
            Err(_) => {
                diagnostics.push(Diagnostic::error(
                    "E0407",
                    "module path must be valid UTF-8",
                    elements[2].span,
                ));
                return None;
            }
        },
        _ => {
            diagnostics.push(Diagnostic::error(
                "E0407",
                "module path must be a byte string",
                elements[2].span,
            ));
            return None;
        }
    };
    let imports = parse_name_list(&elements[3], "imports", diagnostics)?;
    let exports = parse_name_list(&elements[4], "exports", diagnostics)?;
    Some(ModuleSpec {
        identity,
        path,
        path_span: elements[2].span,
        imports,
        exports,
        span: form.span,
    })
}

fn parse_name_list(
    form: &SExpr,
    head: &str,
    diagnostics: &mut Vec<Diagnostic>,
) -> Option<Vec<SpannedName>> {
    let Some(elements) = list(form) else {
        diagnostics.push(Diagnostic::error(
            "E0404",
            format!("{head} must be ({head} NAME*)"),
            form.span,
        ));
        return None;
    };
    if elements.first().and_then(atom) != Some(head) {
        diagnostics.push(Diagnostic::error(
            "E0404",
            format!("expected `{head}` clause in canonical position"),
            form.span,
        ));
        return None;
    }
    let mut names = Vec::new();
    for name in &elements[1..] {
        if let Some(name) = parse_name(name, head, diagnostics) {
            names.push(name);
        }
    }
    validate_sorted(&names, head, diagnostics);
    Some(names)
}

fn parse_name(form: &SExpr, role: &str, diagnostics: &mut Vec<Diagnostic>) -> Option<SpannedName> {
    let Some(value) = atom(form) else {
        diagnostics.push(Diagnostic::error(
            "E0405",
            format!("{role} must be an unqualified identifier"),
            form.span,
        ));
        return None;
    };
    if !valid_unqualified_name(value) {
        diagnostics.push(Diagnostic::error(
            "E0405",
            format!("invalid {role} `{value}`"),
            form.span,
        ));
        return None;
    }
    Some(SpannedName {
        value: value.to_owned(),
        span: form.span,
    })
}

fn validate_modules(
    modules: &[ModuleSpec],
    entry: Option<&SpannedName>,
    diagnostics: &mut Vec<Diagnostic>,
) {
    let module_names: Vec<_> = modules
        .iter()
        .map(|module| module.identity.clone())
        .collect();
    validate_sorted(&module_names, "module clauses", diagnostics);

    let mut identities = BTreeMap::<&str, Span>::new();
    let mut paths = BTreeMap::<&str, Span>::new();
    for module in modules {
        if let Some(previous) = identities.insert(&module.identity.value, module.identity.span) {
            diagnostics.push(
                Diagnostic::error(
                    "E0408",
                    format!("duplicate module identity `{}`", module.identity.value),
                    module.identity.span,
                )
                .label(previous, "first module identity is here"),
            );
        }
        if let Some(previous) = paths.insert(&module.path, module.path_span) {
            diagnostics.push(
                Diagnostic::error(
                    "E0408",
                    format!("module path `{}` is used more than once", module.path),
                    module.path_span,
                )
                .label(previous, "first path use is here"),
            );
        }
    }
    let known: BTreeSet<_> = identities.keys().copied().collect();
    if let Some(entry) = entry
        && !known.contains(entry.value.as_str())
    {
        diagnostics.push(Diagnostic::error(
            "E0403",
            format!("entry module `{}` is not declared", entry.value),
            entry.span,
        ));
    }
    for module in modules {
        for import in &module.imports {
            if import.value == module.identity.value {
                diagnostics.push(Diagnostic::error(
                    "E0412",
                    format!("module `{}` cannot import itself", module.identity.value),
                    import.span,
                ));
            } else if !known.contains(import.value.as_str()) {
                diagnostics.push(Diagnostic::error(
                    "E0411",
                    format!(
                        "module `{}` imports unknown module `{}`",
                        module.identity.value, import.value
                    ),
                    import.span,
                ));
            }
        }
    }
    if diagnostics.is_empty()
        && let Some(cycle) = find_cycle(modules)
    {
        let first = modules
            .iter()
            .find(|module| module.identity.value == cycle[0])
            .expect("cycle module exists");
        diagnostics.push(
            Diagnostic::error(
                "E0413",
                format!("import cycle: {}", cycle.join(" -> ")),
                first.identity.span,
            )
            .note("Core 0.2 project imports must be acyclic"),
        );
    }
}

fn validate_sorted(names: &[SpannedName], role: &str, diagnostics: &mut Vec<Diagnostic>) {
    for pair in names.windows(2) {
        if pair[0].value >= pair[1].value {
            diagnostics.push(
                Diagnostic::error(
                    "E0406",
                    format!("{role} must be strictly sorted and unique"),
                    pair[1].span,
                )
                .label(pair[0].span, "previous name is here"),
            );
        }
    }
}

fn find_cycle(modules: &[ModuleSpec]) -> Option<Vec<String>> {
    let edges: BTreeMap<_, _> = modules
        .iter()
        .map(|module| {
            (
                module.identity.value.clone(),
                module
                    .imports
                    .iter()
                    .map(|import| import.value.clone())
                    .collect::<Vec<_>>(),
            )
        })
        .collect();
    for start in edges.keys() {
        let mut path = Vec::new();
        let mut active = BTreeMap::new();
        if let Some(mut cycle) = visit_cycle(start, &edges, &mut path, &mut active) {
            cycle.pop();
            let smallest = cycle
                .iter()
                .enumerate()
                .min_by_key(|(_, name)| *name)
                .map(|(index, _)| index)
                .unwrap_or(0);
            cycle.rotate_left(smallest);
            cycle.push(cycle[0].clone());
            return Some(cycle);
        }
    }
    None
}

fn visit_cycle(
    current: &str,
    edges: &BTreeMap<String, Vec<String>>,
    path: &mut Vec<String>,
    active: &mut BTreeMap<String, usize>,
) -> Option<Vec<String>> {
    if let Some(index) = active.get(current).copied() {
        let mut cycle = path[index..].to_vec();
        cycle.push(current.to_owned());
        return Some(cycle);
    }
    active.insert(current.to_owned(), path.len());
    path.push(current.to_owned());
    for dependency in edges.get(current).into_iter().flatten() {
        if let Some(cycle) = visit_cycle(dependency, edges, path, active) {
            return Some(cycle);
        }
    }
    path.pop();
    active.remove(current);
    None
}

fn valid_unqualified_name(value: &str) -> bool {
    let mut bytes = value.bytes();
    let Some(first) = bytes.next() else {
        return false;
    };
    (first.is_ascii_alphabetic() || first == b'_')
        && bytes.all(|byte| {
            byte.is_ascii_alphanumeric() || matches!(byte, b'_' | b'-' | b'.' | b'?' | b'!')
        })
}

fn valid_relative_path(value: &str) -> bool {
    !value.is_empty()
        && value.ends_with(".slim")
        && !value.starts_with(['/', '\\'])
        && !value.contains('\\')
        && !Path::new(value).is_absolute()
        && value
            .split('/')
            .all(|segment| !segment.is_empty() && segment != "." && segment != "..")
}

fn sorted(mut diagnostics: Vec<Diagnostic>) -> Vec<Diagnostic> {
    diagnostics.sort_by_key(|diagnostic| (diagnostic.primary.start, diagnostic.code));
    diagnostics
}

fn list(form: &SExpr) -> Option<&[SExpr]> {
    match &form.kind {
        SExprKind::List(elements) => Some(elements),
        _ => None,
    }
}

fn atom(form: &SExpr) -> Option<&str> {
    match &form.kind {
        SExprKind::Atom(value) => Some(value),
        _ => None,
    }
}

#[cfg(test)]
mod tests {
    use std::path::PathBuf;

    use super::*;

    fn source(text: &str) -> Source {
        Source::new(PathBuf::from("slim.project"), text.to_owned())
    }

    #[test]
    fn parses_canonical_manifest() {
        let manifest = parse(&source(
            "(project 1 (entry app) (module app \"src/app.slim\" (imports math) (exports)) (module math \"src/math.slim\" (imports) (exports Number add)))",
        ))
        .unwrap();
        assert_eq!(manifest.entry.value, "app");
        assert_eq!(manifest.modules.len(), 2);
        assert_eq!(manifest.modules[1].exports[0].value, "Number");
    }

    #[test]
    fn rejects_noncanonical_order_and_paths() {
        let errors = parse(&source(
            "(project 1 (entry app) (module z \"../z.slim\" (imports) (exports)) (module app \"app.slim\" (imports z z) (exports)))",
        ))
        .unwrap_err();
        let codes: BTreeSet<_> = errors.iter().map(|error| error.code).collect();
        assert!(codes.contains("E0406"));
        assert!(codes.contains("E0407"));
    }

    #[test]
    fn reports_canonical_cycle() {
        let errors = parse(&source(
            "(project 1 (entry a) (module a \"a.slim\" (imports b) (exports)) (module b \"b.slim\" (imports c) (exports)) (module c \"c.slim\" (imports a) (exports)))",
        ))
        .unwrap_err();
        let cycle = errors.iter().find(|error| error.code == "E0413").unwrap();
        assert_eq!(cycle.message, "import cycle: a -> b -> c -> a");
    }
}
