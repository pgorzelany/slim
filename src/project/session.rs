use std::collections::{BTreeMap, BTreeSet, VecDeque};
use std::fs;
use std::path::{Path, PathBuf};

use crate::ast::{Item, Program};
use crate::codegen;
use crate::incremental::{
    self, DeclarationId, DeclarationKind, Fingerprint, LoweredDeclarationState,
    ModuleLoweringSession,
};
use crate::sema::{self, CheckedProgram};
use crate::span::Source;

use super::interface::InterfaceArtifact;
use super::manifest::{ModuleSpec, ProjectManifest};
use super::{ProjectDiagnostic, cache, load_with, resolver};

#[derive(Clone, Debug, Default, Eq, PartialEq)]
pub struct ProjectWorkStats {
    pub modules_read: usize,
    pub modules_parsed: usize,
    pub modules_lowered: usize,
    pub modules_checked: usize,
    pub modules_generated: usize,
    pub modules_reused: usize,
    pub declarations_parsed: usize,
    pub declarations_lowered: usize,
    pub declarations_checked: usize,
    pub declarations_generated: usize,
    pub declarations_reused: usize,
    pub invalidation_closure: usize,
    pub persistent_hits: usize,
    pub persistent_misses: usize,
    pub fallback_clean: bool,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct ProjectDeclarationState {
    pub id: DeclarationId,
    pub syntax: Fingerprint,
    pub interface: Fingerprint,
    pub dependencies: BTreeSet<DeclarationId>,
    pub cache_stamp: u64,
}

#[derive(Debug)]
pub struct ProjectIncrementalCompilation {
    pub source: Source,
    pub checked: Option<CheckedProgram>,
    pub diagnostics: Vec<ProjectDiagnostic>,
    pub interfaces: BTreeMap<String, InterfaceArtifact>,
    pub stats: ProjectWorkStats,
    pub entry: Option<String>,
    generated_c: Option<String>,
}

impl ProjectIncrementalCompilation {
    pub fn succeeded(&self) -> bool {
        self.generated_c.is_some() && self.diagnostics.is_empty()
    }

    pub fn emit_c(&self) -> Option<&str> {
        self.generated_c.as_deref()
    }
}

#[derive(Clone, Debug)]
struct CachedProjectDeclaration {
    syntax: Fingerprint,
    interface: Fingerprint,
    checked: Item,
    generated: String,
    cache_stamp: u64,
}

#[derive(Clone, Debug, Default, Eq, PartialEq)]
struct ProjectDependencyGraph {
    edges: BTreeMap<DeclarationId, BTreeSet<DeclarationId>>,
}

impl ProjectDependencyGraph {
    fn from_program(program: &Program) -> Self {
        let identities: BTreeMap<_, _> = program
            .items
            .iter()
            .map(|item| (item_name(item).to_owned(), resolved_id(item)))
            .collect();
        let mut edges = BTreeMap::new();
        for item in &program.items {
            let id = resolved_id(item);
            let mut names = BTreeSet::new();
            incremental::collect_item_dependencies(item, &mut names);
            edges.insert(
                id,
                names
                    .into_iter()
                    .filter_map(|name| identities.get(&name).cloned())
                    .collect(),
            );
        }
        Self { edges }
    }

    fn reverse_closure(&self, roots: &BTreeSet<DeclarationId>) -> BTreeSet<DeclarationId> {
        let mut reverse = BTreeMap::<DeclarationId, BTreeSet<DeclarationId>>::new();
        for (dependent, dependencies) in &self.edges {
            for dependency in dependencies {
                reverse
                    .entry(dependency.clone())
                    .or_default()
                    .insert(dependent.clone());
            }
        }
        let mut closure = roots.clone();
        let mut queue: VecDeque<_> = roots.iter().cloned().collect();
        while let Some(changed) = queue.pop_front() {
            for dependent in reverse.get(&changed).into_iter().flatten() {
                if closure.insert(dependent.clone()) {
                    queue.push_back(dependent.clone());
                }
            }
        }
        closure
    }
}

#[derive(Clone, Debug)]
struct ProjectState {
    manifest_shape: String,
    lowerings: BTreeMap<String, ModuleLoweringSession>,
    declarations: BTreeMap<DeclarationId, CachedProjectDeclaration>,
    graph: ProjectDependencyGraph,
    interfaces: BTreeMap<String, InterfaceArtifact>,
    next_cache_stamp: u64,
}

pub struct ProjectSession {
    state: Option<ProjectState>,
    cache_directory: Option<PathBuf>,
}

impl Default for ProjectSession {
    fn default() -> Self {
        Self::new()
    }
}

impl ProjectSession {
    pub fn new() -> Self {
        Self {
            state: None,
            cache_directory: None,
        }
    }

    pub fn with_cache_directory(directory: PathBuf) -> Self {
        Self {
            state: None,
            cache_directory: Some(directory),
        }
    }

    pub fn declarations(&self) -> Vec<ProjectDeclarationState> {
        let Some(state) = &self.state else {
            return Vec::new();
        };
        state
            .declarations
            .iter()
            .map(|(id, declaration)| ProjectDeclarationState {
                id: id.clone(),
                syntax: declaration.syntax,
                interface: declaration.interface,
                dependencies: state.graph.edges.get(id).cloned().unwrap_or_default(),
                cache_stamp: declaration.cache_stamp,
            })
            .collect()
    }

    pub fn update(&mut self, manifest_source: Source) -> ProjectIncrementalCompilation {
        let (persistent_hits, persistent_misses) = if let Some(directory) = &self.cache_directory {
            let probe = probe_persistent_cache(&manifest_source, directory);
            if let Some(compilation) = probe.compilation {
                return compilation;
            }
            (probe.hits, probe.misses)
        } else {
            (0, 0)
        };
        let source = manifest_source.clone();
        let mut lowerings = self
            .state
            .as_ref()
            .map(|state| state.lowerings.clone())
            .unwrap_or_default();
        let mut lowering_states = BTreeMap::<String, Vec<LoweredDeclarationState>>::new();
        let mut lowering_stats = BTreeMap::new();
        let loaded = match load_with(manifest_source, |spec, module_source| {
            let session = lowerings.entry(spec.identity.value.clone()).or_default();
            let (program, diagnostics, states, stats) = session.update(module_source);
            lowering_states.insert(spec.identity.value.clone(), states);
            lowering_stats.insert(spec.identity.value.clone(), stats);
            (program, diagnostics)
        }) {
            Ok(loaded) => loaded,
            Err(diagnostics) => {
                return failed(source, diagnostics, ProjectWorkStats::default());
            }
        };

        let mut stats = ProjectWorkStats {
            modules_read: loaded.modules.len(),
            persistent_hits,
            persistent_misses,
            ..ProjectWorkStats::default()
        };
        for work in lowering_stats.values() {
            stats.declarations_parsed += work.parsed;
            stats.declarations_lowered += work.lowered;
            stats.fallback_clean |= work.fallback_clean;
            stats.modules_parsed += usize::from(work.parsed > 0);
            stats.modules_lowered += usize::from(work.lowered > 0);
        }
        let interfaces = match super::interface::build(&loaded) {
            Ok(interfaces) => interfaces,
            Err(diagnostics) => return failed_with(source, diagnostics, stats, BTreeMap::new()),
        };
        let manifest_shape = manifest_shape(&loaded);
        let resolved = match resolver::resolve(&loaded) {
            Ok(resolved) => resolved,
            Err(diagnostics) => return failed_with(source, diagnostics, stats, interfaces),
        };
        let entry = resolved.entry.clone();
        let locations = resolved.locations.clone();
        let current_graph = ProjectDependencyGraph::from_program(&resolved.program);
        let syntax = syntax_map(&lowering_states);
        let current_interfaces: BTreeMap<_, _> = resolved
            .program
            .items
            .iter()
            .map(|item| (resolved_id(item), incremental::interface_fingerprint(item)))
            .collect();
        let current_ids: BTreeSet<_> = current_interfaces.keys().cloned().collect();
        let mut selected = BTreeSet::new();

        if let Some(old) = &self.state {
            if old.manifest_shape != manifest_shape {
                selected.extend(current_ids.iter().cloned());
            } else {
                let old_ids: BTreeSet<_> = old.declarations.keys().cloned().collect();
                let removed: BTreeSet<_> = old_ids.difference(&current_ids).cloned().collect();
                let mut interface_changed = removed;
                for id in &current_ids {
                    let Some(previous) = old.declarations.get(id) else {
                        selected.insert(id.clone());
                        interface_changed.insert(id.clone());
                        continue;
                    };
                    if previous.syntax != syntax[id] {
                        selected.insert(id.clone());
                    }
                    if previous.interface != current_interfaces[id] {
                        interface_changed.insert(id.clone());
                    }
                }
                selected.extend(
                    old.graph
                        .reverse_closure(&interface_changed)
                        .into_iter()
                        .filter(|id| current_ids.contains(id)),
                );
                selected.extend(
                    current_graph
                        .reverse_closure(&interface_changed)
                        .into_iter()
                        .filter(|id| current_ids.contains(id)),
                );

                let changed_public_modules: BTreeSet<_> = interfaces
                    .iter()
                    .filter(|(module, artifact)| {
                        old.interfaces
                            .get(*module)
                            .is_none_or(|previous| previous.fingerprint != artifact.fingerprint)
                    })
                    .map(|(module, _)| module.clone())
                    .collect();
                let invalidated_modules = reverse_module_closure(&loaded, &changed_public_modules);
                selected.extend(
                    current_ids
                        .iter()
                        .filter(|id| invalidated_modules.contains(&id.module))
                        .cloned(),
                );
            }
        } else {
            selected.extend(current_ids.iter().cloned());
        }
        stats.declarations_checked = selected.len();
        stats.invalidation_closure = selected.len();
        let selected_modules: BTreeSet<_> = selected.iter().map(|id| id.module.clone()).collect();
        stats.modules_checked = selected_modules.len();
        stats.modules_reused = loaded.modules.len().saturating_sub(selected_modules.len());

        let old_declarations = self.state.as_ref().map(|state| &state.declarations);
        let mut items = Vec::with_capacity(resolved.program.items.len());
        for current in resolved.program.items {
            let id = resolved_id(&current);
            let item = if selected.contains(&id) {
                current
            } else if let Some(previous) = old_declarations.and_then(|old| old.get(&id)) {
                let mut checked = previous.checked.clone();
                let delta = incremental::item_span(&current).start as isize
                    - incremental::item_span(&checked).start as isize;
                incremental::shift_item(&mut checked, delta);
                checked
            } else {
                current
            };
            items.push(item);
        }
        let program = Program {
            name: "project".to_owned(),
            items,
            span: resolved.program.span,
        };
        let selected_names = selected.iter().map(global_declaration_name).collect();
        let (checked, diagnostics) =
            sema::check_selected_with_entry(program, &selected_names, &entry);
        let mut diagnostics = diagnostics
            .into_iter()
            .map(|diagnostic| resolver::locate_diagnostic(&locations, diagnostic))
            .collect::<Vec<_>>();
        resolver::sort_project_diagnostics(&mut diagnostics);
        let Some(checked) = checked else {
            return ProjectIncrementalCompilation {
                source,
                checked: None,
                diagnostics,
                interfaces,
                stats,
                entry: Some(entry),
                generated_c: None,
            };
        };

        let mut next_stamp = self
            .state
            .as_ref()
            .map_or(1, |state| state.next_cache_stamp);
        let mut fragments = BTreeMap::new();
        let mut declarations = BTreeMap::new();
        for item in &checked.program.items {
            let id = resolved_id(item);
            let (generated, cache_stamp) = if selected.contains(&id) {
                stats.declarations_generated += 1;
                let generated = codegen::generate_item_c(&checked, item);
                let cache_stamp = next_stamp;
                next_stamp += 1;
                (generated, cache_stamp)
            } else {
                stats.declarations_reused += 1;
                let previous = old_declarations
                    .and_then(|old| old.get(&id))
                    .expect("unselected declaration has checked cache");
                (previous.generated.clone(), previous.cache_stamp)
            };
            fragments.insert(item_name(item).to_owned(), generated.clone());
            declarations.insert(
                id.clone(),
                CachedProjectDeclaration {
                    syntax: syntax[&id],
                    interface: current_interfaces[&id],
                    checked: item.clone(),
                    generated,
                    cache_stamp,
                },
            );
        }
        stats.modules_generated = selected_modules.len();
        let module_order: Vec<_> = loaded.topological_layers().into_iter().flatten().collect();
        let module_fragments: BTreeMap<_, _> = module_order
            .iter()
            .map(|module| {
                (
                    module.clone(),
                    codegen::assemble_project_module_fragment(&checked, module, &fragments),
                )
            })
            .collect();
        let generated_c = codegen::assemble_project_c(&module_fragments, &module_order, &entry);
        if let Some(directory) = &self.cache_directory {
            persist_cache(directory, &loaded, &interfaces, &module_fragments);
        }
        self.state = Some(ProjectState {
            manifest_shape,
            lowerings,
            declarations,
            graph: ProjectDependencyGraph::from_program(&checked.program),
            interfaces: interfaces.clone(),
            next_cache_stamp: next_stamp,
        });
        ProjectIncrementalCompilation {
            source,
            checked: Some(checked),
            diagnostics,
            interfaces,
            stats,
            entry: Some(entry),
            generated_c: Some(generated_c),
        }
    }
}

struct PersistentProbe {
    compilation: Option<ProjectIncrementalCompilation>,
    hits: usize,
    misses: usize,
}

pub(super) fn load_persistent(
    source: &Source,
    directory: &Path,
) -> Option<ProjectIncrementalCompilation> {
    probe_persistent_cache(source, directory).compilation
}

fn probe_persistent_cache(source: &Source, directory: &Path) -> PersistentProbe {
    let Some((manifest, module_sources)) = read_manifest_sources(source) else {
        return PersistentProbe {
            compilation: None,
            hits: 0,
            misses: 0,
        };
    };
    let mut entries = BTreeMap::new();
    let mut interfaces = BTreeMap::new();
    let mut hits = 0;
    let mut misses = 0;
    for layer in manifest_layers(&manifest) {
        for module_name in layer {
            let spec = manifest
                .module(&module_name)
                .expect("manifest layer contains module");
            let Some(module_source) = module_sources.get(&module_name) else {
                misses += 1;
                continue;
            };
            let expected_source = module_evidence_fingerprint(
                module_source,
                spec,
                manifest.entry.value == module_name,
            );
            let expected_dependencies: Option<BTreeMap<_, _>> = spec
                .imports
                .iter()
                .map(|dependency| {
                    interfaces
                        .get(&dependency.value)
                        .map(|artifact: &InterfaceArtifact| {
                            (dependency.value.clone(), artifact.fingerprint)
                        })
                })
                .collect();
            let entry = cache::read(directory, &module_name).ok().flatten();
            let valid = entry.filter(|entry| {
                entry.source == expected_source
                    && expected_dependencies
                        .as_ref()
                        .is_some_and(|dependencies| dependencies == &entry.dependencies)
            });
            if let Some(entry) = valid {
                hits += 1;
                interfaces.insert(
                    module_name.clone(),
                    InterfaceArtifact {
                        module: module_name.clone(),
                        bytes: entry.interface.clone(),
                        fingerprint: entry.interface_fingerprint,
                    },
                );
                entries.insert(module_name, entry);
            } else {
                misses += 1;
            }
        }
    }
    if misses != 0 || hits != manifest.modules.len() {
        return PersistentProbe {
            compilation: None,
            hits,
            misses,
        };
    }
    let entry_module = &manifest.entry.value;
    let module_order: Vec<_> = manifest_layers(&manifest).into_iter().flatten().collect();
    let fragments = entries
        .iter()
        .map(|(module, entry)| (module.clone(), entry.generated_c.clone()))
        .collect();
    let generated_c =
        codegen::assemble_project_c(&fragments, &module_order, &format!("{entry_module}/main"));
    let declarations_reused = entries
        .values()
        .map(|entry| entry.declaration_count as usize)
        .sum();
    PersistentProbe {
        compilation: Some(ProjectIncrementalCompilation {
            source: source.clone(),
            checked: None,
            diagnostics: Vec::new(),
            interfaces,
            stats: ProjectWorkStats {
                modules_read: manifest.modules.len(),
                modules_reused: manifest.modules.len(),
                declarations_reused,
                persistent_hits: hits,
                persistent_misses: misses,
                ..ProjectWorkStats::default()
            },
            entry: Some(format!("{entry_module}/main")),
            generated_c: Some(generated_c),
        }),
        hits,
        misses,
    }
}

pub(super) fn persist_cache(
    directory: &Path,
    project: &super::LoadedProject,
    interfaces: &BTreeMap<String, InterfaceArtifact>,
    module_fragments: &BTreeMap<String, String>,
) {
    let entry_module = &project.manifest.entry.value;
    for spec in &project.manifest.modules {
        let module = &spec.identity.value;
        let module_source = &project.modules[module].source;
        let dependencies = spec
            .imports
            .iter()
            .map(|dependency| {
                (
                    dependency.value.clone(),
                    interfaces[&dependency.value].fingerprint,
                )
            })
            .collect();
        let entry = cache::ModuleCacheEntry {
            module: module.clone(),
            source: module_evidence_fingerprint(module_source, spec, module == entry_module),
            interface_fingerprint: interfaces[module].fingerprint,
            declaration_count: u32::try_from(project.modules[module].program.items.len())
                .unwrap_or(u32::MAX),
            dependencies,
            interface: interfaces[module].bytes.clone(),
            generated_c: module_fragments[module].clone(),
        };
        let _ = cache::write_atomic(directory, &entry);
    }
}

fn module_evidence_fingerprint(source: &Source, spec: &ModuleSpec, is_entry: bool) -> Fingerprint {
    let source = incremental::normalized_source_fingerprint(source)
        .unwrap_or_else(|| cache::fingerprint(source.text.as_bytes()));
    let mut evidence = Vec::new();
    evidence.extend_from_slice(&source.0.to_be_bytes());
    evidence.extend_from_slice(spec.identity.value.as_bytes());
    evidence.push(0);
    evidence.push(u8::from(is_entry));
    for export in &spec.exports {
        evidence.extend_from_slice(export.value.as_bytes());
        evidence.push(0);
    }
    cache::fingerprint(&evidence)
}

fn read_manifest_sources(source: &Source) -> Option<(ProjectManifest, BTreeMap<String, Source>)> {
    let manifest = super::manifest::parse(source).ok()?;
    let root = fs::canonicalize(source.path().parent().unwrap_or_else(|| Path::new("."))).ok()?;
    let mut canonical_paths = BTreeSet::new();
    let mut sources = BTreeMap::new();
    for spec in &manifest.modules {
        let path = fs::canonicalize(root.join(&spec.path)).ok()?;
        if !path.starts_with(&root) || !canonical_paths.insert(path.clone()) {
            return None;
        }
        let text = fs::read_to_string(&path).ok()?;
        sources.insert(spec.identity.value.clone(), Source::new(path, text));
    }
    Some((manifest, sources))
}

fn manifest_layers(manifest: &ProjectManifest) -> Vec<Vec<String>> {
    let mut remaining: BTreeMap<_, BTreeSet<_>> = manifest
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
            .map(|(module, _)| module.clone())
            .collect();
        if ready.is_empty() {
            return Vec::new();
        }
        for module in &ready {
            remaining.remove(module);
        }
        for dependencies in remaining.values_mut() {
            for module in &ready {
                dependencies.remove(module);
            }
        }
        layers.push(ready);
    }
    layers
}

fn syntax_map(
    modules: &BTreeMap<String, Vec<LoweredDeclarationState>>,
) -> BTreeMap<DeclarationId, Fingerprint> {
    modules
        .values()
        .flatten()
        .map(|state| (state.id.clone(), state.syntax))
        .collect()
}

fn resolved_id(item: &Item) -> DeclarationId {
    let (kind, global) = match item {
        Item::Function(function) => (DeclarationKind::Function, function.name.as_str()),
        Item::Record(record) => (DeclarationKind::Record, record.name.as_str()),
        Item::Variant(variant) => (DeclarationKind::Variant, variant.name.as_str()),
    };
    let (module, name) = global
        .split_once('/')
        .expect("resolved declaration has global module/name identity");
    DeclarationId {
        module: module.to_owned(),
        kind,
        name: name.to_owned(),
    }
}

fn global_declaration_name(id: &DeclarationId) -> String {
    format!("{}/{}", id.module, id.name)
}

fn item_name(item: &Item) -> &str {
    match item {
        Item::Function(function) => &function.name,
        Item::Record(record) => &record.name,
        Item::Variant(variant) => &variant.name,
    }
}

fn manifest_shape(project: &super::LoadedProject) -> String {
    let mut shape = format!("entry\t{}\n", project.manifest.entry.value);
    for module in &project.manifest.modules {
        shape.push_str(&module.identity.value);
        shape.push('\t');
        shape.push_str(&module.path);
        shape.push('\t');
        shape.push_str(
            &module
                .imports
                .iter()
                .map(|import| import.value.as_str())
                .collect::<Vec<_>>()
                .join(","),
        );
        shape.push('\t');
        shape.push_str(
            &module
                .exports
                .iter()
                .map(|export| export.value.as_str())
                .collect::<Vec<_>>()
                .join(","),
        );
        shape.push('\n');
    }
    shape
}

fn reverse_module_closure(
    project: &super::LoadedProject,
    roots: &BTreeSet<String>,
) -> BTreeSet<String> {
    let mut reverse = BTreeMap::<String, BTreeSet<String>>::new();
    for module in &project.manifest.modules {
        for dependency in &module.imports {
            reverse
                .entry(dependency.value.clone())
                .or_default()
                .insert(module.identity.value.clone());
        }
    }
    let mut closure = BTreeSet::new();
    let mut queue: VecDeque<_> = roots.iter().cloned().collect();
    while let Some(changed) = queue.pop_front() {
        for dependent in reverse.get(&changed).into_iter().flatten() {
            if closure.insert(dependent.clone()) {
                queue.push_back(dependent.clone());
            }
        }
    }
    closure
}

fn failed(
    source: Source,
    diagnostics: Vec<ProjectDiagnostic>,
    stats: ProjectWorkStats,
) -> ProjectIncrementalCompilation {
    failed_with(source, diagnostics, stats, BTreeMap::new())
}

fn failed_with(
    source: Source,
    diagnostics: Vec<ProjectDiagnostic>,
    stats: ProjectWorkStats,
    interfaces: BTreeMap<String, InterfaceArtifact>,
) -> ProjectIncrementalCompilation {
    ProjectIncrementalCompilation {
        source,
        checked: None,
        diagnostics,
        interfaces,
        stats,
        entry: None,
        generated_c: None,
    }
}

#[cfg(test)]
mod tests {
    use std::fs;
    use std::sync::atomic::{AtomicU64, Ordering};

    use super::*;

    static NEXT_PROJECT: AtomicU64 = AtomicU64::new(0);

    struct ProjectFixture {
        root: PathBuf,
    }

    impl ProjectFixture {
        fn new() -> Self {
            let root = std::env::temp_dir().join(format!(
                "slim-project-session-test-{}-{}",
                std::process::id(),
                NEXT_PROJECT.fetch_add(1, Ordering::Relaxed)
            ));
            let _ = fs::remove_dir_all(&root);
            fs::create_dir(&root).unwrap();
            let fixture = Self { root };
            fixture.write(
                "app.slim",
                "(module app (fn main ((args (Vec Bytes))) I64 (effects) (call mid/wrap 40)))",
            );
            fixture.write(
                "base.slim",
                "(module base (record Number ((value I64))) (fn answer ((value I64)) I64 (effects) (call i64.add value 1)))",
            );
            fixture.write(
                "mid.slim",
                "(module mid (fn wrap ((value I64)) I64 (effects) (call base/answer value)))",
            );
            fixture
        }

        fn write(&self, path: &str, text: &str) {
            fs::write(self.root.join(path), text).unwrap();
        }

        fn source(&self) -> Source {
            Source::new(
                self.root.join("slim.project"),
                "(project 1 (entry app) (module app \"app.slim\" (imports mid) (exports)) (module base \"base.slim\" (imports) (exports Number answer)) (module mid \"mid.slim\" (imports base) (exports wrap)))"
                    .to_owned(),
            )
        }
    }

    impl Drop for ProjectFixture {
        fn drop(&mut self) {
            let _ = fs::remove_dir_all(&self.root);
        }
    }

    #[test]
    fn reuses_unchanged_declarations_and_matches_clean_output() {
        let fixture = ProjectFixture::new();
        let mut session = ProjectSession::new();
        let initial = session.update(fixture.source());
        assert!(initial.succeeded(), "{:#?}", initial.diagnostics);
        assert_eq!(initial.stats.declarations_checked, 4);
        assert_eq!(
            initial.emit_c(),
            super::super::compile(fixture.source()).emit_c()
        );

        let unchanged = session.update(fixture.source());
        assert!(unchanged.succeeded());
        assert_eq!(unchanged.stats.declarations_parsed, 0);
        assert_eq!(unchanged.stats.declarations_lowered, 0);
        assert_eq!(unchanged.stats.declarations_checked, 0);
        assert_eq!(unchanged.stats.declarations_generated, 0);
        assert_eq!(unchanged.stats.declarations_reused, 4);
        assert_eq!(unchanged.emit_c(), initial.emit_c());
    }

    #[test]
    fn private_body_edit_rebuilds_exactly_one_declaration() {
        let fixture = ProjectFixture::new();
        let mut session = ProjectSession::new();
        assert!(session.update(fixture.source()).succeeded());
        let before: BTreeMap<_, _> = session
            .declarations()
            .into_iter()
            .map(|state| (state.id, state.cache_stamp))
            .collect();
        fixture.write(
            "base.slim",
            "(module base (record Number ((value I64))) (fn answer ((value I64)) I64 (effects) (call i64.add value 2)))",
        );
        let updated = session.update(fixture.source());
        assert!(updated.succeeded(), "{:#?}", updated.diagnostics);
        assert_eq!(updated.stats.declarations_parsed, 1);
        assert_eq!(updated.stats.declarations_lowered, 1);
        assert_eq!(updated.stats.declarations_checked, 1);
        assert_eq!(updated.stats.declarations_generated, 1);
        assert_eq!(updated.stats.declarations_reused, 3);
        assert_eq!(
            updated.emit_c(),
            super::super::compile(fixture.source()).emit_c()
        );
        let changed: Vec<_> = session
            .declarations()
            .into_iter()
            .filter(|state| before[&state.id] != state.cache_stamp)
            .map(|state| state.id.to_string())
            .collect();
        assert_eq!(changed, vec!["base::fn::answer"]);
    }

    #[test]
    fn public_layout_edit_invalidates_reverse_module_closure() {
        let fixture = ProjectFixture::new();
        let mut session = ProjectSession::new();
        assert!(session.update(fixture.source()).succeeded());
        fixture.write(
            "base.slim",
            "(module base (record Number ((value I64) (valid Bool))) (fn answer ((value I64)) I64 (effects) (call i64.add value 1)))",
        );
        let updated = session.update(fixture.source());
        assert!(updated.succeeded(), "{:#?}", updated.diagnostics);
        assert_eq!(updated.stats.declarations_parsed, 1);
        assert_eq!(updated.stats.declarations_checked, 3);
        assert_eq!(updated.stats.declarations_generated, 3);
        assert_eq!(updated.stats.invalidation_closure, 3);
        assert_eq!(
            updated.emit_c(),
            super::super::compile(fixture.source()).emit_c()
        );
    }

    #[test]
    fn failed_edit_does_not_replace_last_good_state() {
        let fixture = ProjectFixture::new();
        let mut session = ProjectSession::new();
        assert!(session.update(fixture.source()).succeeded());
        fixture.write(
            "base.slim",
            "(module base (record Number ((value I64))) (fn answer ((value I64)) I64 (effects) true))",
        );
        assert!(!session.update(fixture.source()).succeeded());
        fixture.write(
            "base.slim",
            "(module base (record Number ((value I64))) (fn answer ((value I64)) I64 (effects) (call i64.add value 3)))",
        );
        let recovered = session.update(fixture.source());
        assert!(recovered.succeeded(), "{:#?}", recovered.diagnostics);
        assert_eq!(recovered.stats.declarations_checked, 1);
        assert_eq!(
            recovered.emit_c(),
            super::super::compile(fixture.source()).emit_c()
        );
    }

    #[test]
    fn persistent_cache_reuses_all_modules_and_recovers_from_corruption() {
        let fixture = ProjectFixture::new();
        let cache_directory = fixture.root.join("cache/v1");
        let mut writer = ProjectSession::with_cache_directory(cache_directory.clone());
        let clean = writer.update(fixture.source());
        assert!(clean.succeeded(), "{:#?}", clean.diagnostics);
        assert_eq!(clean.stats.persistent_misses, 3);

        let mut reader = ProjectSession::with_cache_directory(cache_directory.clone());
        let warm = reader.update(fixture.source());
        assert!(warm.succeeded());
        assert_eq!(warm.stats.persistent_hits, 3);
        assert_eq!(warm.stats.persistent_misses, 0);
        assert_eq!(warm.stats.declarations_reused, 4);
        assert_eq!(warm.stats.declarations_checked, 0);
        assert_eq!(warm.emit_c(), clean.emit_c());
        assert_eq!(warm.interfaces, clean.interfaces);

        fs::write(cache_directory.join(cache::file_name("base")), b"truncated").unwrap();
        let mut recovering = ProjectSession::with_cache_directory(cache_directory.clone());
        let recovered = recovering.update(fixture.source());
        assert!(recovered.succeeded(), "{:#?}", recovered.diagnostics);
        assert!(recovered.stats.persistent_misses > 0);
        assert_eq!(recovered.emit_c(), clean.emit_c());

        fixture.write(
            "base.slim",
            "(module base (record Number ((value I64))) (fn answer ((value I64)) I64 (effects) true))",
        );
        let mut invalid = ProjectSession::with_cache_directory(cache_directory);
        let rejected = invalid.update(fixture.source());
        assert!(!rejected.succeeded());
        assert!(!rejected.diagnostics.is_empty());
    }
}
