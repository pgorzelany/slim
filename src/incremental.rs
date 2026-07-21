use std::collections::{BTreeMap, BTreeSet, VecDeque};
use std::fmt;

use crate::ast::*;
use crate::codegen;
use crate::compiler;
use crate::diagnostic::Diagnostic;
use crate::lexer::{self, Token, TokenKind};
use crate::parser;
use crate::sema::{self, Builtin, CheckedProgram};
use crate::sexpr;
use crate::span::{Source, Span};

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub enum DeclarationKind {
    Function,
    Record,
    Variant,
}

impl DeclarationKind {
    pub const fn name(self) -> &'static str {
        match self {
            Self::Function => "fn",
            Self::Record => "record",
            Self::Variant => "variant",
        }
    }
}

#[derive(Clone, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct DeclarationId {
    pub module: String,
    pub kind: DeclarationKind,
    pub name: String,
}

impl fmt::Display for DeclarationId {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            formatter,
            "{}::{}::{}",
            self.module,
            self.kind.name(),
            self.name
        )
    }
}

#[derive(Clone, Copy, Debug, Eq, Ord, PartialEq, PartialOrd)]
pub struct Fingerprint(pub u64);

impl fmt::Display for Fingerprint {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "{:016x}", self.0)
    }
}

#[derive(Clone, Debug, Default, Eq, PartialEq)]
pub struct DependencyGraph {
    edges: BTreeMap<DeclarationId, BTreeSet<DeclarationId>>,
}

impl DependencyGraph {
    pub fn dependencies(&self, id: &DeclarationId) -> Option<&BTreeSet<DeclarationId>> {
        self.edges.get(id)
    }

    pub fn stable_text(&self) -> String {
        let mut output = String::new();
        for (id, dependencies) in &self.edges {
            output.push_str(&id.to_string());
            output.push('\t');
            output.push_str(
                &dependencies
                    .iter()
                    .map(ToString::to_string)
                    .collect::<Vec<_>>()
                    .join(","),
            );
            output.push('\n');
        }
        output
    }

    fn from_program(program: &CheckedProgram) -> Self {
        let module = &program.program.name;
        let by_name: BTreeMap<_, _> = program
            .program
            .items
            .iter()
            .map(|item| {
                let id = item_id(module, item);
                (id.name.clone(), id)
            })
            .collect();
        let mut edges = BTreeMap::new();
        for item in &program.program.items {
            let id = item_id(module, item);
            let mut names = BTreeSet::new();
            collect_item_dependencies(item, &mut names);
            let dependencies = names
                .into_iter()
                .filter_map(|name| by_name.get(&name).cloned())
                .collect();
            edges.insert(id, dependencies);
        }
        Self { edges }
    }

    fn reverse_closure(&self, roots: &BTreeSet<DeclarationId>) -> BTreeSet<DeclarationId> {
        let mut reverse = BTreeMap::<DeclarationId, BTreeSet<DeclarationId>>::new();
        for (source, targets) in &self.edges {
            for target in targets {
                reverse
                    .entry(target.clone())
                    .or_default()
                    .insert(source.clone());
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

#[derive(Clone, Debug, Default, Eq, PartialEq)]
pub struct WorkStats {
    pub parsed: usize,
    pub lowered: usize,
    pub checked: usize,
    pub generated: usize,
    pub reused: usize,
    pub fallback_clean: bool,
}

#[derive(Clone, Debug)]
pub(crate) struct LoweredDeclarationState {
    pub id: DeclarationId,
    pub syntax: Fingerprint,
}

#[derive(Clone, Debug, Default)]
pub(crate) struct ModuleLoweringSession {
    module: Option<String>,
    declarations: BTreeMap<DeclarationId, CachedLoweredDeclaration>,
}

#[derive(Clone, Debug)]
struct CachedLoweredDeclaration {
    syntax: Fingerprint,
    layout: Fingerprint,
    item: Item,
}

impl ModuleLoweringSession {
    pub fn update(
        &mut self,
        source: &Source,
    ) -> (
        Option<Program>,
        Vec<Diagnostic>,
        Vec<LoweredDeclarationState>,
        WorkStats,
    ) {
        let indexed = match index_program(source) {
            Ok(indexed) => indexed,
            Err(()) => {
                let (program, diagnostics) = compiler::lower_source(source);
                let count = program.as_ref().map_or(0, |program| program.items.len());
                return (
                    program,
                    diagnostics,
                    Vec::new(),
                    WorkStats {
                        parsed: count,
                        lowered: count,
                        fallback_clean: true,
                        ..WorkStats::default()
                    },
                );
            }
        };
        let same_module = self.module.as_deref() == Some(indexed.module.as_str());
        let mut stats = WorkStats::default();
        let mut items = Vec::with_capacity(indexed.declarations.len());
        let mut states = Vec::with_capacity(indexed.declarations.len());
        let mut next = BTreeMap::new();
        let mut seen_names = BTreeSet::new();
        let mut diagnostics = Vec::new();
        for declaration in &indexed.declarations {
            if !seen_names.insert(declaration.id.name.clone()) {
                let (program, diagnostics) = compiler::lower_source(source);
                let count = program.as_ref().map_or(0, |program| program.items.len());
                return (
                    program,
                    diagnostics,
                    Vec::new(),
                    WorkStats {
                        parsed: count,
                        lowered: count,
                        fallback_clean: true,
                        ..WorkStats::default()
                    },
                );
            }
            let old = same_module
                .then(|| self.declarations.get(&declaration.id))
                .flatten();
            let item = if let Some(old) = old
                .filter(|old| old.syntax == declaration.syntax && old.layout == declaration.layout)
            {
                let mut item = old.item.clone();
                let delta = declaration.span.start as isize - item_span(&item).start as isize;
                shift_item(&mut item, delta);
                stats.reused += 1;
                item
            } else {
                match lower_indexed(&indexed.tokens, declaration, source.text.len()) {
                    Ok(item) => {
                        stats.parsed += 1;
                        stats.lowered += 1;
                        item
                    }
                    Err(mut errors) => {
                        diagnostics.append(&mut errors);
                        continue;
                    }
                }
            };
            states.push(LoweredDeclarationState {
                id: declaration.id.clone(),
                syntax: declaration.syntax,
            });
            next.insert(
                declaration.id.clone(),
                CachedLoweredDeclaration {
                    syntax: declaration.syntax,
                    layout: declaration.layout,
                    item: item.clone(),
                },
            );
            items.push(item);
        }
        if !diagnostics.is_empty() {
            diagnostics.sort_by_key(|diagnostic| (diagnostic.primary.start, diagnostic.code));
            return (None, diagnostics, states, stats);
        }
        self.module = Some(indexed.module.clone());
        self.declarations = next;
        (
            Some(Program {
                name: indexed.module,
                items,
                span: indexed.span,
            }),
            diagnostics,
            states,
            stats,
        )
    }
}

#[derive(Clone, Debug)]
pub struct DeclarationState {
    pub id: DeclarationId,
    pub syntax: Fingerprint,
    pub interface: Fingerprint,
    pub dependencies: BTreeSet<DeclarationId>,
    pub cache_stamp: u64,
}

#[derive(Debug)]
pub struct IncrementalCompilation {
    pub source: Source,
    pub checked: Option<CheckedProgram>,
    pub diagnostics: Vec<Diagnostic>,
    pub stats: WorkStats,
    pub graph: DependencyGraph,
    generated_c: Option<String>,
}

impl IncrementalCompilation {
    pub fn succeeded(&self) -> bool {
        self.checked.is_some() && self.diagnostics.is_empty()
    }

    pub fn emit_c(&self) -> Option<&str> {
        self.generated_c.as_deref()
    }
}

#[derive(Clone, Debug)]
struct CachedDeclaration {
    id: DeclarationId,
    syntax: Fingerprint,
    interface: Fingerprint,
    layout: Fingerprint,
    lowered: Item,
    checked: Item,
    generated: String,
    cache_stamp: u64,
}

#[derive(Default)]
pub struct IncrementalSession {
    module: Option<String>,
    cache: BTreeMap<DeclarationId, CachedDeclaration>,
    graph: DependencyGraph,
    next_cache_stamp: u64,
}

impl IncrementalSession {
    pub fn new() -> Self {
        Self {
            next_cache_stamp: 1,
            ..Self::default()
        }
    }

    pub fn declarations(&self) -> Vec<DeclarationState> {
        self.cache
            .values()
            .map(|cached| DeclarationState {
                id: cached.id.clone(),
                syntax: cached.syntax,
                interface: cached.interface,
                dependencies: self
                    .graph
                    .dependencies(&cached.id)
                    .cloned()
                    .unwrap_or_default(),
                cache_stamp: cached.cache_stamp,
            })
            .collect()
    }

    pub fn update(&mut self, source: Source) -> IncrementalCompilation {
        let indexed = match index_program(&source) {
            Ok(indexed) => indexed,
            Err(()) => return self.fallback(source),
        };
        let same_module = self.module.as_deref() == Some(indexed.module.as_str());
        let old_graph = if same_module {
            self.graph.clone()
        } else {
            DependencyGraph::default()
        };
        let mut stats = WorkStats::default();
        let mut candidates = BTreeMap::new();
        let mut diagnostics = Vec::new();
        let mut seen_names = BTreeSet::new();

        for declaration in &indexed.declarations {
            if !seen_names.insert(declaration.id.name.clone()) {
                return self.fallback(source);
            }
            let old = same_module
                .then(|| self.cache.get(&declaration.id))
                .flatten()
                .cloned();
            let candidate = if let Some(old) = old {
                if old.layout == declaration.layout && old.syntax == declaration.syntax {
                    let delta =
                        declaration.span.start as isize - item_span(&old.lowered).start as isize;
                    let mut lowered = old.lowered.clone();
                    let mut checked = old.checked.clone();
                    shift_item(&mut lowered, delta);
                    shift_item(&mut checked, delta);
                    Candidate {
                        interface: old.interface,
                        lowered,
                        checked: Some(checked),
                        old: Some(old),
                        needs_check: false,
                    }
                } else {
                    match lower_indexed(&indexed.tokens, declaration, source.text.len()) {
                        Ok(item) => {
                            stats.parsed += 1;
                            stats.lowered += 1;
                            Candidate {
                                interface: interface_fingerprint(&item),
                                lowered: item,
                                checked: None,
                                old: Some(old),
                                needs_check: true,
                            }
                        }
                        Err(mut errors) => {
                            diagnostics.append(&mut errors);
                            continue;
                        }
                    }
                }
            } else {
                match lower_indexed(&indexed.tokens, declaration, source.text.len()) {
                    Ok(item) => {
                        stats.parsed += 1;
                        stats.lowered += 1;
                        Candidate {
                            interface: interface_fingerprint(&item),
                            lowered: item,
                            checked: None,
                            old: None,
                            needs_check: true,
                        }
                    }
                    Err(mut errors) => {
                        diagnostics.append(&mut errors);
                        continue;
                    }
                }
            };
            candidates.insert(declaration.id.clone(), candidate);
        }

        if !diagnostics.is_empty() {
            diagnostics.sort_by_key(|diagnostic| (diagnostic.primary.start, diagnostic.code));
            return IncrementalCompilation {
                source,
                checked: None,
                diagnostics,
                stats,
                graph: self.graph.clone(),
                generated_c: None,
            };
        }

        let current_ids: BTreeSet<_> = candidates.keys().cloned().collect();
        let old_ids: BTreeSet<_> = if same_module {
            self.cache.keys().cloned().collect()
        } else {
            BTreeSet::new()
        };
        let removed: BTreeSet<_> = old_ids.difference(&current_ids).cloned().collect();
        let mut interface_changed = removed;
        let mut selected = BTreeSet::new();
        for (id, candidate) in &candidates {
            if candidate.needs_check {
                selected.insert(id.clone());
            }
            if candidate
                .old
                .as_ref()
                .is_none_or(|old| old.interface != candidate.interface)
            {
                interface_changed.insert(id.clone());
            }
        }
        selected.extend(
            old_graph
                .reverse_closure(&interface_changed)
                .into_iter()
                .filter(|id| current_ids.contains(id)),
        );
        stats.checked = selected.len();

        let mut items = Vec::with_capacity(indexed.declarations.len());
        for declaration in &indexed.declarations {
            let candidate = candidates
                .get(&declaration.id)
                .expect("indexed candidate exists");
            let item = if selected.contains(&declaration.id) {
                candidate.lowered.clone()
            } else {
                candidate
                    .checked
                    .clone()
                    .expect("unselected declaration has checked cache")
            };
            items.push(item);
        }
        let program = Program {
            name: indexed.module.clone(),
            items,
            span: indexed.span,
        };
        let selected_names = selected.iter().map(|id| id.name.clone()).collect();
        let (checked, mut check_diagnostics) = sema::check_selected(program, &selected_names);
        check_diagnostics.sort_by_key(|diagnostic| (diagnostic.primary.start, diagnostic.code));
        let Some(checked) = checked else {
            return IncrementalCompilation {
                source,
                checked: None,
                diagnostics: check_diagnostics,
                stats,
                graph: self.graph.clone(),
                generated_c: None,
            };
        };

        let checked_by_id: BTreeMap<_, _> = checked
            .program
            .items
            .iter()
            .cloned()
            .map(|item| (item_id(&indexed.module, &item), item))
            .collect();
        let mut fragments = BTreeMap::new();
        let mut new_cache = BTreeMap::new();
        for declaration in &indexed.declarations {
            let id = &declaration.id;
            let candidate = candidates.get(id).expect("candidate exists");
            let checked_item = checked_by_id.get(id).expect("checked item exists").clone();
            let (generated, cache_stamp) = if selected.contains(id) {
                stats.generated += 1;
                let generated = codegen::generate_item_c(&checked, &checked_item);
                let stamp = self.next_cache_stamp;
                self.next_cache_stamp += 1;
                (generated, stamp)
            } else {
                stats.reused += 1;
                let old = candidate
                    .old
                    .as_ref()
                    .expect("reused declaration has cache");
                (old.generated.clone(), old.cache_stamp)
            };
            fragments.insert(id.name.clone(), generated.clone());
            new_cache.insert(
                id.clone(),
                CachedDeclaration {
                    id: id.clone(),
                    syntax: declaration.syntax,
                    interface: candidate.interface,
                    layout: declaration.layout,
                    lowered: candidate.lowered.clone(),
                    checked: checked_item,
                    generated,
                    cache_stamp,
                },
            );
        }
        let generated_c = codegen::generate_c_from_fragments(&checked, &fragments);
        let graph = DependencyGraph::from_program(&checked);
        self.module = Some(indexed.module);
        self.cache = new_cache;
        self.graph = graph.clone();

        IncrementalCompilation {
            source,
            checked: Some(checked),
            diagnostics: check_diagnostics,
            stats,
            graph,
            generated_c: Some(generated_c),
        }
    }

    fn fallback(&self, source: Source) -> IncrementalCompilation {
        let clean = compiler::compile(source);
        let generated_c = clean.emit_c();
        let graph = clean
            .checked
            .as_ref()
            .map(DependencyGraph::from_program)
            .unwrap_or_else(|| self.graph.clone());
        IncrementalCompilation {
            source: clean.source,
            checked: clean.checked,
            diagnostics: clean.diagnostics,
            stats: WorkStats {
                fallback_clean: true,
                ..WorkStats::default()
            },
            graph,
            generated_c,
        }
    }
}

struct IndexedProgram {
    module: String,
    span: Span,
    tokens: Vec<Token>,
    declarations: Vec<IndexedDeclaration>,
}

struct IndexedDeclaration {
    id: DeclarationId,
    token_start: usize,
    token_end: usize,
    span: Span,
    syntax: Fingerprint,
    layout: Fingerprint,
}

struct Candidate {
    interface: Fingerprint,
    lowered: Item,
    checked: Option<Item>,
    old: Option<CachedDeclaration>,
    needs_check: bool,
}

fn index_program(source: &Source) -> Result<IndexedProgram, ()> {
    let (tokens, diagnostics) = lexer::lex(&source.text);
    if !diagnostics.is_empty() || tokens.len() < 4 {
        return Err(());
    }
    if tokens[0].kind != TokenKind::LeftParen
        || !matches!(&tokens[1].kind, TokenKind::Atom(head) if head == "module")
    {
        return Err(());
    }
    let TokenKind::Atom(module) = &tokens[2].kind else {
        return Err(());
    };
    let mut depth = 0_isize;
    for (index, token) in tokens.iter().enumerate() {
        match token.kind {
            TokenKind::LeftParen => depth += 1,
            TokenKind::RightParen => depth -= 1,
            TokenKind::Atom(_) | TokenKind::Bytes(_) => {}
        }
        if depth < 0 || (depth == 0 && index + 1 != tokens.len()) {
            return Err(());
        }
    }
    if depth != 0 || tokens.last().map(|token| &token.kind) != Some(&TokenKind::RightParen) {
        return Err(());
    }

    let mut declarations = Vec::new();
    let mut cursor = 3;
    while cursor + 1 < tokens.len() {
        if tokens[cursor].kind != TokenKind::LeftParen {
            return Err(());
        }
        let start = cursor;
        let mut item_depth = 0_isize;
        loop {
            let token = tokens.get(cursor).ok_or(())?;
            match token.kind {
                TokenKind::LeftParen => item_depth += 1,
                TokenKind::RightParen => item_depth -= 1,
                TokenKind::Atom(_) | TokenKind::Bytes(_) => {}
            }
            cursor += 1;
            if item_depth == 0 {
                break;
            }
        }
        let end = cursor;
        let kind = match tokens.get(start + 1).map(|token| &token.kind) {
            Some(TokenKind::Atom(kind)) if kind == "fn" => DeclarationKind::Function,
            Some(TokenKind::Atom(kind)) if kind == "record" => DeclarationKind::Record,
            Some(TokenKind::Atom(kind)) if kind == "variant" => DeclarationKind::Variant,
            _ => return Err(()),
        };
        let Some(TokenKind::Atom(name)) = tokens.get(start + 2).map(|token| &token.kind) else {
            return Err(());
        };
        let span = tokens[start].span.merge(tokens[end - 1].span);
        let text = source.text.get(span.start..span.end).ok_or(())?;
        declarations.push(IndexedDeclaration {
            id: DeclarationId {
                module: module.clone(),
                kind,
                name: name.clone(),
            },
            token_start: start,
            token_end: end,
            span,
            syntax: token_fingerprint(&tokens[start..end]),
            layout: fingerprint_bytes(text.as_bytes()),
        });
    }
    if cursor != tokens.len() - 1 {
        return Err(());
    }
    Ok(IndexedProgram {
        module: module.clone(),
        span: tokens[0]
            .span
            .merge(tokens.last().expect("tokens exist").span),
        tokens,
        declarations,
    })
}

fn lower_indexed(
    tokens: &[Token],
    declaration: &IndexedDeclaration,
    source_len: usize,
) -> Result<Item, Vec<Diagnostic>> {
    let (forms, mut diagnostics) = sexpr::parse(
        &tokens[declaration.token_start..declaration.token_end],
        source_len,
    );
    if forms.len() != 1 {
        return Err(diagnostics);
    }
    let (item, mut lower_diagnostics) = parser::lower_item(&forms[0]);
    diagnostics.append(&mut lower_diagnostics);
    if diagnostics.is_empty() {
        item.ok_or(diagnostics)
    } else {
        Err(diagnostics)
    }
}

fn token_fingerprint(tokens: &[Token]) -> Fingerprint {
    let mut hash = StableHash::new();
    for token in tokens {
        match &token.kind {
            TokenKind::LeftParen => hash.bytes(&[0]),
            TokenKind::RightParen => hash.bytes(&[1]),
            TokenKind::Atom(value) => {
                hash.bytes(&[2]);
                hash.field(value.as_bytes());
            }
            TokenKind::Bytes(value) => {
                hash.bytes(&[3]);
                hash.field(value);
            }
        }
    }
    Fingerprint(hash.finish())
}

pub(crate) fn normalized_source_fingerprint(source: &Source) -> Option<Fingerprint> {
    let (tokens, diagnostics) = lexer::lex(&source.text);
    diagnostics.is_empty().then(|| token_fingerprint(&tokens))
}

pub(crate) fn interface_fingerprint(item: &Item) -> Fingerprint {
    let mut hash = StableHash::new();
    match item {
        Item::Function(function) => {
            hash.field(b"fn");
            hash.field(function.name.as_bytes());
            for parameter in &function.params {
                hash.bytes(&[match parameter.mode {
                    ParamMode::Owned => 0,
                    ParamMode::Inout => 1,
                }]);
                hash_type(&mut hash, &parameter.ty);
            }
            hash_type(&mut hash, &function.return_type);
            for effect in function.effects.iter().copied().collect::<BTreeSet<_>>() {
                hash.field(effect.name().as_bytes());
            }
        }
        Item::Record(record) => {
            hash.field(b"record");
            hash.field(record.name.as_bytes());
            for field in &record.fields {
                hash.field(field.name.as_bytes());
                hash_type(&mut hash, &field.ty);
            }
        }
        Item::Variant(variant) => {
            hash.field(b"variant");
            hash.field(variant.name.as_bytes());
            for case in &variant.cases {
                hash.field(case.name.as_bytes());
                for ty in &case.payload {
                    hash_type(&mut hash, ty);
                }
            }
        }
    }
    Fingerprint(hash.finish())
}

fn hash_type(hash: &mut StableHash, ty: &Type) {
    match ty {
        Type::Unit => hash.field(b"Unit"),
        Type::Bool => hash.field(b"Bool"),
        Type::U8 => hash.field(b"U8"),
        Type::I64 => hash.field(b"I64"),
        Type::Bytes => hash.field(b"Bytes"),
        Type::Vec(inner) => {
            hash.field(b"Vec");
            hash_type(hash, inner);
        }
        Type::Arena(inner) => {
            hash.field(b"Arena");
            hash_type(hash, inner);
        }
        Type::Id(inner) => {
            hash.field(b"Id");
            hash_type(hash, inner);
        }
        Type::Named(name) => {
            hash.field(b"Named");
            hash.field(name.as_bytes());
        }
        Type::Error => hash.field(b"Error"),
    }
}

struct StableHash(u64);

impl StableHash {
    const fn new() -> Self {
        Self(0xcbf29ce484222325)
    }

    fn bytes(&mut self, bytes: &[u8]) {
        for byte in bytes {
            self.0 ^= u64::from(*byte);
            self.0 = self.0.wrapping_mul(0x100000001b3);
        }
    }

    fn field(&mut self, bytes: &[u8]) {
        self.bytes(&(bytes.len() as u64).to_le_bytes());
        self.bytes(bytes);
    }

    const fn finish(self) -> u64 {
        self.0
    }
}

fn fingerprint_bytes(bytes: &[u8]) -> Fingerprint {
    let mut hash = StableHash::new();
    hash.field(bytes);
    Fingerprint(hash.finish())
}

pub(crate) fn item_id(module: &str, item: &Item) -> DeclarationId {
    match item {
        Item::Function(function) => DeclarationId {
            module: module.to_owned(),
            kind: DeclarationKind::Function,
            name: function.name.clone(),
        },
        Item::Record(record) => DeclarationId {
            module: module.to_owned(),
            kind: DeclarationKind::Record,
            name: record.name.clone(),
        },
        Item::Variant(variant) => DeclarationId {
            module: module.to_owned(),
            kind: DeclarationKind::Variant,
            name: variant.name.clone(),
        },
    }
}

pub(crate) fn item_span(item: &Item) -> Span {
    match item {
        Item::Function(function) => function.span,
        Item::Record(record) => record.span,
        Item::Variant(variant) => variant.span,
    }
}

pub(crate) fn collect_item_dependencies(item: &Item, names: &mut BTreeSet<String>) {
    match item {
        Item::Function(function) => {
            for parameter in &function.params {
                collect_type_dependencies(&parameter.ty, names);
            }
            collect_type_dependencies(&function.return_type, names);
            collect_expr_dependencies(&function.body, names);
        }
        Item::Record(record) => {
            for field in &record.fields {
                collect_type_dependencies(&field.ty, names);
            }
        }
        Item::Variant(variant) => {
            for case in &variant.cases {
                for ty in &case.payload {
                    collect_type_dependencies(ty, names);
                }
            }
        }
    }
}

fn collect_type_dependencies(ty: &Type, names: &mut BTreeSet<String>) {
    match ty {
        Type::Named(name) => {
            names.insert(name.clone());
        }
        Type::Vec(inner) | Type::Arena(inner) | Type::Id(inner) => {
            collect_type_dependencies(inner, names);
        }
        Type::Unit | Type::Bool | Type::U8 | Type::I64 | Type::Bytes | Type::Error => {}
    }
}

fn collect_expr_dependencies(expr: &Expr, names: &mut BTreeSet<String>) {
    collect_type_dependencies(&expr.ty, names);
    match &expr.kind {
        ExprKind::Let {
            declared_type,
            value,
            body,
            ..
        } => {
            collect_type_dependencies(declared_type, names);
            collect_expr_dependencies(value, names);
            collect_expr_dependencies(body, names);
        }
        ExprKind::Call {
            function,
            arguments,
        } => {
            if Builtin::from_name(function).is_none() {
                names.insert(function.clone());
            }
            for argument in arguments {
                collect_expr_dependencies(argument, names);
            }
        }
        ExprKind::Recur(arguments) => {
            for argument in arguments {
                collect_expr_dependencies(argument, names);
            }
        }
        ExprKind::Set { value, body, .. } => {
            collect_expr_dependencies(value, names);
            collect_expr_dependencies(body, names);
        }
        ExprKind::Make { record, fields } => {
            names.insert(record.clone());
            for (_, value, _) in fields {
                collect_expr_dependencies(value, names);
            }
        }
        ExprKind::Get { value, .. } => collect_expr_dependencies(value, names),
        ExprKind::Case {
            variant, payload, ..
        } => {
            names.insert(variant.clone());
            for value in payload {
                collect_expr_dependencies(value, names);
            }
        }
        ExprKind::Match { value, arms } => {
            collect_expr_dependencies(value, names);
            for arm in arms {
                collect_expr_dependencies(&arm.body, names);
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

pub(crate) fn shift_item(item: &mut Item, delta: isize) {
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
    use std::path::PathBuf;

    use super::*;

    fn source(text: String) -> Source {
        Source::new(PathBuf::from("incremental.slim"), text)
    }

    fn chain(size: usize, leaf_addend: i64, leaf_inout: bool) -> String {
        let mut text = String::from("(module incremental ");
        text.push_str("(fn leaf (");
        if leaf_inout {
            text.push_str("(inout value I64)");
        } else {
            text.push_str("(value I64)");
        }
        text.push_str(") I64 (effects) (call i64.add value ");
        text.push_str(&leaf_addend.to_string());
        text.push_str(")) ");
        for index in 0..size {
            let callee = if index == 0 {
                "leaf".to_owned()
            } else {
                format!("caller-{}", index - 1)
            };
            text.push_str(&format!(
                "(fn caller-{index} ((value I64)) I64 (effects) (call {callee} value)) "
            ));
        }
        let last = if size == 0 {
            "leaf".to_owned()
        } else {
            format!("caller-{}", size - 1)
        };
        text.push_str(&format!(
            "(fn unrelated ((value I64)) I64 (effects) value) (fn main ((args (Vec Bytes))) I64 (effects) (let value I64 40 (call {last} value))))\n"
        ));
        text
    }

    fn records(reversed: bool) -> String {
        let fields = if reversed {
            "((right I64) (left I64))"
        } else {
            "((left I64) (right I64))"
        };
        format!(
            "(module records (record Pair {fields}) (fn sum ((pair Pair)) I64 (effects) (let left I64 (get pair left) (let right I64 (get pair right) (call i64.add left right)))) (fn forward ((pair Pair)) I64 (effects) (call sum pair)) (fn unrelated ((value I64)) I64 (effects) value) (fn main ((args (Vec Bytes))) I64 (effects) (let pair Pair (make Pair (left 20) (right 22)) (call forward pair))))\n"
        )
    }

    #[test]
    fn fingerprints_ignore_layout_and_separate_body_from_interface() {
        let first = index_program(&source(chain(1, 1, false))).unwrap();
        let spaced = chain(1, 1, false).replace("(fn leaf", "\n  (fn   leaf");
        let second = index_program(&source(spaced)).unwrap();
        assert_eq!(first.declarations[0].syntax, second.declarations[0].syntax);

        let first_item =
            lower_indexed(&first.tokens, &first.declarations[0], first.span.end).unwrap();
        let body = index_program(&source(chain(1, 2, false))).unwrap();
        let body_item = lower_indexed(&body.tokens, &body.declarations[0], body.span.end).unwrap();
        assert_ne!(first.declarations[0].syntax, body.declarations[0].syntax);
        assert_eq!(
            interface_fingerprint(&first_item),
            interface_fingerprint(&body_item)
        );

        let interface = index_program(&source(chain(1, 1, true))).unwrap();
        let interface_item = lower_indexed(
            &interface.tokens,
            &interface.declarations[0],
            interface.span.end,
        )
        .unwrap();
        assert_ne!(
            interface_fingerprint(&first_item),
            interface_fingerprint(&interface_item)
        );
    }

    #[test]
    fn body_edit_reuses_every_unrelated_declaration() {
        let mut session = IncrementalSession::new();
        let cold = session.update(source(chain(100, 1, false)));
        assert!(cold.succeeded(), "{:#?}", cold.diagnostics);
        assert_eq!(cold.stats.parsed, 103);
        assert_eq!(cold.stats.checked, 103);
        let unrelated_before = session
            .declarations()
            .into_iter()
            .find(|state| state.id.name == "unrelated")
            .unwrap()
            .cache_stamp;

        let unchanged = session.update(source(chain(100, 1, false)));
        assert!(unchanged.succeeded());
        assert_eq!(
            unchanged.stats,
            WorkStats {
                reused: 103,
                ..WorkStats::default()
            }
        );

        // The wider literal also moves every following declaration, proving
        // cached spans can be rebased without reparsing those declarations.
        let edited_text = chain(100, 1_000, false);
        let edited = session.update(source(edited_text.clone()));
        assert!(edited.succeeded(), "{:#?}", edited.diagnostics);
        assert_eq!(edited.stats.parsed, 1);
        assert_eq!(edited.stats.lowered, 1);
        assert_eq!(edited.stats.checked, 1);
        assert_eq!(edited.stats.generated, 1);
        assert_eq!(edited.stats.reused, 102);
        let unrelated_after = session
            .declarations()
            .into_iter()
            .find(|state| state.id.name == "unrelated")
            .unwrap()
            .cache_stamp;
        assert_eq!(unrelated_before, unrelated_after);
        let clean = compiler::compile(source(edited_text));
        assert_eq!(edited.emit_c(), clean.emit_c().as_deref());
    }

    #[test]
    fn interface_edit_checks_only_reverse_dependency_closure() {
        let mut session = IncrementalSession::new();
        let cold = session.update(source(chain(20, 1, false)));
        assert!(cold.succeeded(), "{:#?}", cold.diagnostics);
        let edited_text = chain(20, 1, true);
        let edited = session.update(source(edited_text.clone()));
        assert!(edited.succeeded(), "{:#?}", edited.diagnostics);
        // leaf, twenty callers, and main depend on the changed interface;
        // unrelated has no edge to the chain.
        assert_eq!(edited.stats.checked, 22);
        assert_eq!(edited.stats.generated, 22);
        assert_eq!(edited.stats.reused, 1);
        let clean = compiler::compile(source(edited_text));
        assert_eq!(edited.emit_c(), clean.emit_c().as_deref());
    }

    #[test]
    fn data_layout_edit_invalidates_only_named_type_users() {
        let mut session = IncrementalSession::new();
        let first = session.update(source(records(false)));
        assert!(first.succeeded(), "{:#?}", first.diagnostics);
        let pair_before = session
            .declarations()
            .into_iter()
            .find(|state| state.id.name == "Pair")
            .unwrap();

        let edited_text = records(true);
        let edited = session.update(source(edited_text.clone()));
        assert!(edited.succeeded(), "{:#?}", edited.diagnostics);
        assert_eq!(edited.stats.parsed, 1);
        assert_eq!(edited.stats.checked, 4);
        assert_eq!(edited.stats.generated, 4);
        assert_eq!(edited.stats.reused, 1);
        let pair_after = session
            .declarations()
            .into_iter()
            .find(|state| state.id.name == "Pair")
            .unwrap();
        assert_ne!(pair_before.syntax, pair_after.syntax);
        assert_ne!(pair_before.interface, pair_after.interface);
        let clean = compiler::compile(source(edited_text));
        assert_eq!(edited.emit_c(), clean.emit_c().as_deref());
    }

    #[test]
    fn dependent_diagnostics_match_clean_spans_after_source_movement() {
        let base = "(module spans (fn value ((input I64)) I64 (effects) input) (fn caller ((input I64)) I64 (effects) (call value input)) (fn main ((args (Vec Bytes))) I64 (effects) (call caller 1)))\n";
        let edited = "(module spans\n    (fn value ((input I64)) Bool (effects) true) (fn caller ((input I64)) I64 (effects) (call value input)) (fn main ((args (Vec Bytes))) I64 (effects) (call caller 1)))\n";
        let mut session = IncrementalSession::new();
        assert!(session.update(source(base.to_owned())).succeeded());
        let incremental = session.update(source(edited.to_owned()));
        let clean = compiler::compile(source(edited.to_owned()));
        assert!(!incremental.succeeded());
        let summary = |diagnostics: &[Diagnostic]| {
            diagnostics
                .iter()
                .map(|diagnostic| {
                    (
                        diagnostic.code,
                        diagnostic.primary.start,
                        diagnostic.primary.end,
                    )
                })
                .collect::<Vec<_>>()
        };
        assert_eq!(
            summary(&incremental.diagnostics),
            summary(&clean.diagnostics)
        );
    }

    #[test]
    fn invalid_edit_does_not_poison_last_good_cache() {
        let mut session = IncrementalSession::new();
        assert!(session.update(source(chain(3, 1, false))).succeeded());
        let stamps = session
            .declarations()
            .into_iter()
            .map(|state| (state.id, state.cache_stamp))
            .collect::<BTreeMap<_, _>>();
        let bad = session.update(source("(module broken (fn".to_owned()));
        assert!(!bad.succeeded());
        assert!(bad.stats.fallback_clean);
        let restored = session.update(source(chain(3, 1, false)));
        assert!(restored.succeeded());
        assert_eq!(restored.stats.generated, 0);
        let restored_stamps = session
            .declarations()
            .into_iter()
            .map(|state| (state.id, state.cache_stamp))
            .collect::<BTreeMap<_, _>>();
        assert_eq!(stamps, restored_stamps);
    }

    #[test]
    fn removing_main_is_rejected_without_poisoning_reuse() {
        let valid = chain(1, 1, false);
        let without_main = valid.replace(
            "(fn main ((args (Vec Bytes))) I64 (effects) (let value I64 40 (call caller-0 value)))",
            "",
        );
        let mut session = IncrementalSession::new();
        assert!(session.update(source(valid.clone())).succeeded());
        let removed = session.update(source(without_main));
        assert!(!removed.succeeded());
        assert!(
            removed
                .diagnostics
                .iter()
                .any(|diagnostic| diagnostic.code == "E0307")
        );
        let restored = session.update(source(valid));
        assert!(restored.succeeded());
        assert_eq!(restored.stats.generated, 0);

        let mut empty_session = IncrementalSession::new();
        let empty = empty_session.update(source("(module empty)".to_owned()));
        assert!(!empty.succeeded());
        assert!(
            empty
                .diagnostics
                .iter()
                .any(|diagnostic| diagnostic.code == "E0307")
        );
    }

    #[test]
    fn graph_text_is_deterministic() {
        let text = chain(4, 1, false);
        let mut left = IncrementalSession::new();
        let mut right = IncrementalSession::new();
        let left = left.update(source(text.clone()));
        let right = right.update(source(text));
        assert_eq!(left.graph.stable_text(), right.graph.stable_text());
    }
}
