use std::collections::{BTreeMap, BTreeSet};
use std::fs;
use std::path::PathBuf;

use crate::ast::{Item, Program};
use crate::codegen;
use crate::compiler;
use crate::diagnostic::Diagnostic;
use crate::sema::CheckedProgram;
use crate::span::Source;

pub mod interface;
pub mod manifest;
pub mod resolver;

use manifest::{ModuleSpec, ProjectManifest};

use self::interface::InterfaceArtifact;

#[derive(Clone, Debug)]
pub struct ProjectDiagnostic {
    pub module: Option<String>,
    pub source: Source,
    pub diagnostic: Diagnostic,
    pub related: Vec<ProjectRelated>,
}

#[derive(Clone, Debug)]
pub struct ProjectRelated {
    pub module: String,
    pub source: Source,
    pub span: crate::span::Span,
    pub message: String,
}

impl ProjectDiagnostic {
    fn local(module: Option<String>, source: Source, diagnostic: Diagnostic) -> Self {
        Self {
            module,
            source,
            diagnostic,
            related: Vec::new(),
        }
    }

    pub fn render_human(&self) -> String {
        let mut output = self.diagnostic.render_human(&self.source);
        for related in &self.related {
            let (line, column) = related.source.line_column(related.span.start);
            output.push_str(&format!(
                "   = related {}:{}:{}: {}\n",
                related.source.path().display(),
                line,
                column,
                related.message
            ));
        }
        output
    }

    pub fn render_json(&self) -> String {
        let mut output = self.diagnostic.render_json(&self.source);
        let closing = output.pop();
        debug_assert_eq!(closing, Some('}'));
        output.push_str(",\"related\":[");
        for (index, related) in self.related.iter().enumerate() {
            if index > 0 {
                output.push(',');
            }
            let (line, column) = related.source.line_column(related.span.start);
            output.push_str(&format!(
                "{{\"module\":\"{}\",\"file\":\"{}\",\"start\":{},\"end\":{},\"line\":{},\"column\":{},\"message\":\"{}\"}}",
                escape_json(&related.module),
                escape_json(&related.source.path().display().to_string()),
                related.span.start,
                related.span.end,
                line,
                column,
                escape_json(&related.message),
            ));
        }
        output.push_str("]}");
        output
    }
}

#[derive(Debug)]
pub struct ProjectCompilation {
    pub source: Source,
    pub checked: Option<CheckedProgram>,
    pub diagnostics: Vec<ProjectDiagnostic>,
    pub interfaces: BTreeMap<String, InterfaceArtifact>,
    pub entry: Option<String>,
    generated_c: Option<String>,
}

impl ProjectCompilation {
    pub fn succeeded(&self) -> bool {
        self.checked.is_some() && self.diagnostics.is_empty()
    }

    pub fn emit_c(&self) -> Option<&str> {
        self.generated_c.as_deref()
    }
}

#[derive(Clone, Debug)]
pub struct LoadedModule {
    pub spec: ModuleSpec,
    pub source: Source,
    pub program: Program,
}

#[derive(Clone, Debug)]
pub struct LoadedProject {
    pub manifest_source: Source,
    pub manifest: ProjectManifest,
    pub modules: BTreeMap<String, LoadedModule>,
}

impl LoadedProject {
    pub fn topological_layers(&self) -> Vec<Vec<String>> {
        let mut remaining: BTreeMap<_, BTreeSet<_>> = self
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
        let mut layers = Vec::new();
        while !remaining.is_empty() {
            let ready: Vec<_> = remaining
                .iter()
                .filter(|(_, dependencies)| dependencies.is_empty())
                .map(|(identity, _)| identity.clone())
                .collect();
            debug_assert!(!ready.is_empty(), "manifest parser rejects cycles");
            for identity in &ready {
                remaining.remove(identity);
            }
            for dependencies in remaining.values_mut() {
                for identity in &ready {
                    dependencies.remove(identity);
                }
            }
            layers.push(ready);
        }
        layers
    }
}

pub fn load(manifest_source: Source) -> Result<LoadedProject, Vec<ProjectDiagnostic>> {
    let manifest = match manifest::parse(&manifest_source) {
        Ok(manifest) => manifest,
        Err(diagnostics) => {
            return Err(diagnostics
                .into_iter()
                .map(|diagnostic| {
                    ProjectDiagnostic::local(None, manifest_source.clone(), diagnostic)
                })
                .collect());
        }
    };
    let parent = manifest_source
        .path()
        .parent()
        .unwrap_or_else(|| std::path::Path::new("."));
    let root = match fs::canonicalize(parent) {
        Ok(root) => root,
        Err(error) => {
            return Err(vec![ProjectDiagnostic::local(
                None,
                manifest_source.clone(),
                Diagnostic::error(
                    "E0407",
                    format!("cannot resolve project root: {error}"),
                    manifest.span,
                ),
            )]);
        }
    };

    let mut modules = BTreeMap::new();
    let mut canonical_paths = BTreeMap::<PathBuf, (&str, crate::span::Span)>::new();
    let mut diagnostics = Vec::new();
    for spec in &manifest.modules {
        let candidate = root.join(&spec.path);
        let canonical = match fs::canonicalize(&candidate) {
            Ok(path) => path,
            Err(error) => {
                diagnostics.push(ProjectDiagnostic::local(
                    Some(spec.identity.value.clone()),
                    manifest_source.clone(),
                    Diagnostic::error(
                        "E0409",
                        format!("cannot read module `{}`: {error}", spec.identity.value),
                        spec.path_span,
                    ),
                ));
                continue;
            }
        };
        if !canonical.starts_with(&root) {
            diagnostics.push(ProjectDiagnostic::local(
                Some(spec.identity.value.clone()),
                manifest_source.clone(),
                Diagnostic::error(
                    "E0407",
                    format!("module path `{}` escapes the project root", spec.path),
                    spec.path_span,
                ),
            ));
            continue;
        }
        if let Some((previous_name, previous_span)) =
            canonical_paths.insert(canonical.clone(), (&spec.identity.value, spec.path_span))
        {
            diagnostics.push(ProjectDiagnostic::local(
                Some(spec.identity.value.clone()),
                manifest_source.clone(),
                Diagnostic::error(
                    "E0408",
                    format!(
                        "modules `{previous_name}` and `{}` resolve to the same file",
                        spec.identity.value
                    ),
                    spec.path_span,
                )
                .label(previous_span, "first resolved path is here"),
            ));
            continue;
        }
        let bytes = match fs::read(&canonical) {
            Ok(bytes) => bytes,
            Err(error) => {
                diagnostics.push(ProjectDiagnostic::local(
                    Some(spec.identity.value.clone()),
                    manifest_source.clone(),
                    Diagnostic::error(
                        "E0409",
                        format!("cannot read module `{}`: {error}", spec.identity.value),
                        spec.path_span,
                    ),
                ));
                continue;
            }
        };
        let text = match String::from_utf8(bytes) {
            Ok(text) => text,
            Err(_) => {
                diagnostics.push(ProjectDiagnostic::local(
                    Some(spec.identity.value.clone()),
                    manifest_source.clone(),
                    Diagnostic::error(
                        "E0409",
                        format!("module `{}` is not valid UTF-8", spec.identity.value),
                        spec.path_span,
                    ),
                ));
                continue;
            }
        };
        let source = Source::new(&canonical, text);
        let (program, module_diagnostics) = compiler::lower_source(&source);
        diagnostics.extend(module_diagnostics.into_iter().map(|diagnostic| {
            ProjectDiagnostic::local(
                Some(spec.identity.value.clone()),
                source.clone(),
                diagnostic,
            )
        }));
        let Some(program) = program else {
            continue;
        };
        if program.name != spec.identity.value {
            diagnostics.push(ProjectDiagnostic::local(
                Some(spec.identity.value.clone()),
                manifest_source.clone(),
                Diagnostic::error(
                    "E0410",
                    format!(
                        "manifest module `{}` contains source module `{}`",
                        spec.identity.value, program.name
                    ),
                    spec.identity.span,
                )
                .note(format!("source file: {}", canonical.display())),
            ));
        }
        validate_exports(spec, &program, &manifest_source, &mut diagnostics);
        validate_main(spec, &manifest, &program, &source, &mut diagnostics);
        modules.insert(
            spec.identity.value.clone(),
            LoadedModule {
                spec: spec.clone(),
                source,
                program,
            },
        );
    }
    if diagnostics.is_empty() {
        Ok(LoadedProject {
            manifest_source,
            manifest,
            modules,
        })
    } else {
        sort_diagnostics(&mut diagnostics);
        Err(diagnostics)
    }
}

pub fn compile(manifest_source: Source) -> ProjectCompilation {
    let source = manifest_source.clone();
    let loaded = match load(manifest_source) {
        Ok(loaded) => loaded,
        Err(diagnostics) => {
            return ProjectCompilation {
                source,
                checked: None,
                diagnostics,
                interfaces: BTreeMap::new(),
                entry: None,
                generated_c: None,
            };
        }
    };
    let interfaces = match interface::build(&loaded) {
        Ok(interfaces) => interfaces,
        Err(diagnostics) => {
            return ProjectCompilation {
                source,
                checked: None,
                diagnostics,
                interfaces: BTreeMap::new(),
                entry: None,
                generated_c: None,
            };
        }
    };
    let resolved = match resolver::resolve(&loaded) {
        Ok(resolved) => resolved,
        Err(diagnostics) => {
            return ProjectCompilation {
                source,
                checked: None,
                diagnostics,
                interfaces,
                entry: None,
                generated_c: None,
            };
        }
    };
    let entry = resolved.entry.clone();
    let (checked, mut diagnostics) = resolver::check(resolved);
    sort_diagnostics(&mut diagnostics);
    let generated_c = checked
        .as_ref()
        .map(|checked| codegen::generate_c_for_entry(checked, &entry));
    ProjectCompilation {
        source,
        checked,
        diagnostics,
        interfaces,
        entry: Some(entry),
        generated_c,
    }
}

pub fn is_manifest_source(source: &Source) -> bool {
    let (tokens, _) = crate::lexer::lex(&source.text);
    tokens.first().map(|token| &token.kind) == Some(&crate::lexer::TokenKind::LeftParen)
        && matches!(
            tokens.get(1).map(|token| &token.kind),
            Some(crate::lexer::TokenKind::Atom(head)) if head == "project"
        )
}

fn validate_exports(
    spec: &ModuleSpec,
    program: &Program,
    manifest_source: &Source,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) {
    let declared: BTreeSet<_> = program.items.iter().map(item_name).collect();
    for export in &spec.exports {
        if !declared.contains(export.value.as_str()) {
            diagnostics.push(ProjectDiagnostic::local(
                Some(spec.identity.value.clone()),
                manifest_source.clone(),
                Diagnostic::error(
                    "E0414",
                    format!(
                        "module `{}` exports absent declaration `{}`",
                        spec.identity.value, export.value
                    ),
                    export.span,
                ),
            ));
        }
    }
}

fn validate_main(
    spec: &ModuleSpec,
    manifest: &ProjectManifest,
    program: &Program,
    source: &Source,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) {
    let main = program.items.iter().find(|item| item_name(item) == "main");
    if spec.identity.value == manifest.entry.value {
        if main.is_none() {
            diagnostics.push(ProjectDiagnostic::local(
                Some(spec.identity.value.clone()),
                source.clone(),
                Diagnostic::error("E0419", "entry module requires `main`", program.span),
            ));
        }
    } else if let Some(main) = main {
        diagnostics.push(ProjectDiagnostic::local(
            Some(spec.identity.value.clone()),
            source.clone(),
            Diagnostic::error(
                "E0419",
                format!(
                    "non-entry module `{}` must not declare `main`",
                    spec.identity.value
                ),
                item_span(main),
            ),
        ));
    }
}

fn item_name(item: &Item) -> &str {
    match item {
        Item::Function(function) => &function.name,
        Item::Record(record) => &record.name,
        Item::Variant(variant) => &variant.name,
    }
}

fn item_span(item: &Item) -> crate::span::Span {
    match item {
        Item::Function(function) => function.span,
        Item::Record(record) => record.span,
        Item::Variant(variant) => variant.span,
    }
}

fn sort_diagnostics(diagnostics: &mut [ProjectDiagnostic]) {
    diagnostics.sort_by(|left, right| {
        left.module
            .cmp(&right.module)
            .then_with(|| left.source.path().cmp(right.source.path()))
            .then_with(|| {
                left.diagnostic
                    .primary
                    .start
                    .cmp(&right.diagnostic.primary.start)
            })
            .then_with(|| left.diagnostic.code.cmp(right.diagnostic.code))
    });
}

fn escape_json(value: &str) -> String {
    value
        .chars()
        .flat_map(|character| match character {
            '"' => "\\\"".chars().collect::<Vec<_>>(),
            '\\' => "\\\\".chars().collect(),
            '\n' => "\\n".chars().collect(),
            '\r' => "\\r".chars().collect(),
            '\t' => "\\t".chars().collect(),
            control if control < ' ' => format!("\\u{:04x}", control as u32).chars().collect(),
            other => vec![other],
        })
        .collect()
}

#[cfg(test)]
mod tests {
    use std::sync::atomic::{AtomicU64, Ordering};
    use std::time::{SystemTime, UNIX_EPOCH};

    use super::*;

    struct TempProject(PathBuf);

    static NEXT_TEMP: AtomicU64 = AtomicU64::new(0);

    impl TempProject {
        fn new() -> Self {
            let nonce = SystemTime::now()
                .duration_since(UNIX_EPOCH)
                .unwrap()
                .as_nanos();
            let path = std::env::temp_dir().join(format!(
                "slim-project-unit-{}-{nonce}-{}",
                std::process::id(),
                NEXT_TEMP.fetch_add(1, Ordering::Relaxed)
            ));
            fs::create_dir(&path).unwrap();
            Self(path)
        }

        fn write(&self, path: &str, text: &str) {
            let path = self.0.join(path);
            if let Some(parent) = path.parent() {
                fs::create_dir_all(parent).unwrap();
            }
            fs::write(path, text).unwrap();
        }

        fn source(&self, text: &str) -> Source {
            Source::new(self.0.join("slim.project"), text.to_owned())
        }
    }

    impl Drop for TempProject {
        fn drop(&mut self) {
            let _ = fs::remove_dir_all(&self.0);
        }
    }

    #[test]
    fn loads_confined_modules_in_topological_layers() {
        let project = TempProject::new();
        project.write(
            "app.slim",
            "(module app (fn main ((args (Vec Bytes))) I64 (effects) 0))",
        );
        project.write(
            "math.slim",
            "(module math (fn answer ((value I64)) I64 (effects) value))",
        );
        let loaded = load(project.source(
            "(project 1 (entry app) (module app \"app.slim\" (imports math) (exports)) (module math \"math.slim\" (imports) (exports answer)))",
        ))
        .unwrap();
        assert_eq!(loaded.topological_layers(), vec![vec!["math"], vec!["app"]]);
    }

    #[test]
    fn reports_missing_source_at_manifest_path() {
        let project = TempProject::new();
        let errors =
            load(project.source(
                "(project 1 (entry app) (module app \"missing.slim\" (imports) (exports)))",
            ))
            .unwrap_err();
        assert_eq!(errors[0].diagnostic.code, "E0409");
        assert_eq!(errors[0].source.path(), project.0.join("slim.project"));
    }
}
