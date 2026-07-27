use std::collections::{BTreeMap, BTreeSet};
use std::env;
use std::fs;
use std::path::{Path, PathBuf};

fn semantic_pattern_tokens(text: &str) -> Vec<String> {
    let without_named_operators = [
        "i64.add", "i64.sub", "i64.mul", "i64.div", "i64.rem", "i64.eq", "i64.lt", "i64.le",
        "i64.gt", "i64.ge", "bool.not", "bool.and", "bool.or",
    ]
    .into_iter()
    .fold(text.to_owned(), |source, spelling| {
        source.replace(spelling, " ")
    });
    without_named_operators
        .split(|character: char| {
            !character.is_ascii_alphanumeric() && character != '_' && character != '-'
        })
        .filter_map(|token| {
            if token.is_empty()
                || matches!(token, "call" | "make" | "get" | "case" | "set" | "match")
            {
                return None;
            }
            Some(
                match token {
                    "record" => "struct",
                    "variant" => "enum",
                    "Unit" => "Void",
                    "unit" => "void",
                    "fork" => "parallel",
                    other => other,
                }
                .replace('-', "_"),
            )
        })
        .collect()
}

fn contains_slim_pattern(source: &str, pattern: &str) -> bool {
    if source.contains(pattern) {
        return true;
    }
    if !pattern.starts_with('(') {
        return false;
    }
    let source_tokens = semantic_pattern_tokens(source);
    let pattern_tokens = semantic_pattern_tokens(pattern);
    if pattern_tokens.is_empty() {
        return false;
    }
    let mut expected = pattern_tokens.iter();
    let Some(mut token) = expected.next() else {
        return false;
    };
    for source_token in &source_tokens {
        if source_token == token {
            let Some(next) = expected.next() else {
                return true;
            };
            token = next;
        }
    }
    false
}

const LEGACY_RFC_HEADINGS: [&str; 5] = [
    "## Need",
    "## Alternatives",
    "## Costs",
    "## Evidence",
    "## Removal",
];

const CURRENT_RFC_HEADINGS: [&str; 14] = [
    "## Summary",
    "## Motivation",
    "## Guide-level explanation",
    "## Reference-level specification",
    "## Compiler and runtime design",
    "## Compatibility and migration",
    "## Diagnostics and failure cases",
    "## Performance and complexity",
    "## Alternatives and drawbacks",
    "## Test and acceptance plan",
    "## Ratings and evidence",
    "## Decision",
    "## Implementation",
    "## Removal and supersession",
];

#[derive(Debug)]
struct Rfc {
    id: String,
    status: String,
    implementation: String,
    process: String,
    audience: String,
    kind: String,
    primitive: String,
    ratings: [i32; 6],
    score: i32,
}

fn main() {
    let command = env::args().nth(1).unwrap_or_else(|| "check".to_owned());
    if command != "check" {
        eprintln!("usage: slim-govern check");
        std::process::exit(64);
    }

    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let errors = check_repository(&root);
    if errors.is_empty() {
        println!("governance: all hard gates passed");
        return;
    }

    for error in &errors {
        eprintln!("governance error: {error}");
    }
    eprintln!("governance: {} hard gate(s) failed", errors.len());
    std::process::exit(1);
}

fn check_repository(root: &Path) -> Vec<String> {
    let mut errors = Vec::new();
    let required = [
        "AGENTS.md",
        "DESIGN.md",
        "docs/CORE.md",
        "design/FEATURE_POLICY.md",
        "design/project-semantics.tsv",
        "design/rust-budget.tsv",
        "design/surface.tsv",
    ];
    for path in required {
        if !root.join(path).is_file() {
            errors.push(format!("missing required policy file {path}"));
        }
    }

    let rfcs = load_rfcs(&root.join("design/rfcs"), &mut errors);
    check_rfcs(&rfcs, &mut errors);
    check_rfc_migration(root, &mut errors);
    check_surface(&root.join("design/surface.tsv"), &rfcs, &mut errors);
    check_semantic_ledger(
        &root.join("design/project-semantics.tsv"),
        "project-semantics.tsv",
        &rfcs,
        &mut errors,
    );
    check_conformance_coverage(root, &mut errors);
    check_dependencies(&root.join("Cargo.toml"), &mut errors);
    check_rust_safety(&root.join("src"), &mut errors);
    check_rust_budget(root, &mut errors);
    check_toolchain_cutover(root, &mut errors);
    check_ast_boundary(root, &mut errors);
    check_indented_source(root, &mut errors);
    check_selfhost_architecture(root, &mut errors);
    check_core_1d_acceptance(root, &rfcs, &mut errors);
    check_runtime_fast_paths(root, &rfcs, &mut errors);
    check_allocation_free_region_elision(root, &rfcs, &mut errors);
    check_core_1e_acceptance(root, &rfcs, &mut errors);
    check_parallelism_evidence(root, &rfcs, &mut errors);
    check_integer_proof_evidence(root, &rfcs, &mut errors);
    check_resource_evidence(root, &rfcs, &mut errors);
    check_host_boundary(root, &rfcs, &mut errors);
    check_parallelism_application_baseline(root, &rfcs, &mut errors);
    check_complete_parallel_blockers(root, &rfcs, &mut errors);
    check_total_recurrence_evidence(root, &rfcs, &mut errors);
    check_deterministic_parallel_schedule(root, &rfcs, &mut errors);
    check_total_task_failure_semantics(root, &rfcs, &mut errors);
    check_parallel_execution_boundary(root, &rfcs, &mut errors);
    check_core_1j_acceptance(root, &rfcs, &mut errors);
    check_core_1k_acceptance(root, &rfcs, &mut errors);
    check_core_1l_contracts(root, &rfcs, &mut errors);
    check_memory_architecture(root, &mut errors);
    check_direct_reduction(root, &mut errors);
    check_bounded_program_evidence(root, &mut errors);
    check_performance_architecture(root, &rfcs, &mut errors);
    errors
}

fn check_core_1l_contracts(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0082") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "compatibility"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1L requires accepted primitive-free compatibility rfc RFC-0082 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1L compatibility rfc RFC-0082 is missing".to_owned()),
    }
    match rfcs.get("RFC-0083") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1L closure requires accepted primitive-free architecture rfc RFC-0083 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1L closure rfc RFC-0083 is missing".to_owned()),
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0082") || surface.contains("RFC-0083") {
        errors.push("Core 1L rfcs must not add Core language surface".to_owned());
    }

    for required in [
        "VERSION",
        "design/release-contract.tsv",
        "docs/COMPATIBILITY.md",
        "docs/DIAGNOSTICS.md",
        "docs/RELEASE.md",
        "release/manifest.txt",
        "scripts/package-release.sh",
        "scripts/verify-0.9.sh",
        "scripts/verify-release.sh",
        "benchmarks/results/2026-07-23-core-1l-slim-1-0.md",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!("Core 1L contract artifact is missing {required}"));
        }
    }

    let version = fs::read_to_string(root.join("VERSION")).unwrap_or_default();
    let version = version.trim();
    let version_parts: Vec<_> = version.split('.').collect();
    if version_parts.len() != 3
        || version_parts
            .iter()
            .any(|part| part.is_empty() || !part.bytes().all(|byte| byte.is_ascii_digit()))
    {
        errors.push(format!(
            "VERSION must be canonical numeric x.y.z, found `{version}`"
        ));
    }
    for (path, needle) in [
        ("Cargo.toml", format!("version = \"{version}\"")),
        ("Cargo.lock", format!("version = \"{version}\"")),
        ("docs/STATUS.md", format!("Compiler version: {version}")),
    ] {
        let contents = fs::read_to_string(root.join(path)).unwrap_or_default();
        if !contents.contains(&needle) {
            errors.push(format!(
                "Core 1L version drift: {path} is missing `{needle}`"
            ));
        }
    }

    let launcher = fs::read_to_string(root.join("slimc")).unwrap_or_default();
    for required in [
        "slim_version=$(sed -n '1p' \"$slim_root/VERSION\")",
        "--version)",
        "--help)",
        "\\\"schema\\\":1",
    ] {
        if !contains_slim_pattern(&launcher, required) {
            errors.push(format!("Core 1L launcher is missing `{required}`"));
        }
    }
    for forbidden in ["0.0.1", "help|--help", "version|--version"] {
        if launcher.contains(forbidden) {
            errors.push(format!(
                "Core 1L launcher retains duplicate or stale spelling `{forbidden}`"
            ));
        }
    }

    let runtime = fs::read_to_string(root.join("runtime/slim_rt.h")).unwrap_or_default();
    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    if !runtime.contains("#define SLIM_RUNTIME_ABI_VERSION 1")
        || !codegen.contains("SLIM_RUNTIME_ABI_VERSION == 1")
        || !codegen.contains("SLIM runtime ABI mismatch")
    {
        errors.push("Core 1L generated C/runtime ABI 1 contract is incomplete".to_owned());
    }

    let expected_contracts = [
        "source-surface\t2\tpre-1.0-minor\tdesign/surface.tsv",
        "project-manifest\t1\tpre-1.0-minor\tdocs/PROJECTS.md",
        "project-interface\t2\tpre-1.0-minor\tdocs/PROJECTS.md",
        "persistent-cache\t2\trebuildable\tselfhost/cache.slim",
        "diagnostic-codes\t1\tpre-1.0-minor\tdocs/DIAGNOSTICS.md",
        "diagnostic-json\t1\tpre-1.0-minor\tdocs/DIAGNOSTICS.md",
        "analysis\t7\tversioned-tooling\tdocs/QUALITY.md",
        "cost-vector\t1\tversioned-tooling\tdocs/QUALITY.md",
        "equivalence\t2\tversioned-tooling\tdocs/QUALITY.md",
        "reduction-proof\t2\tversioned-tooling\tdocs/REDUCTION.md",
        "reduction-verification\t1\tversioned-tooling\tdocs/REDUCTION.md",
        "structural-edit\t1\tversioned-tooling\tdocs/QUALITY.md",
        "runtime-abi\t1\texact-match\truntime/slim_rt.h",
        "c-backend\t1\tpre-1.0-minor\tdocs/CORE.md",
    ];
    let contract = fs::read_to_string(root.join("design/release-contract.tsv")).unwrap_or_default();
    let rows: Vec<_> = contract
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .collect();
    if rows != expected_contracts {
        errors.push(
            "design/release-contract.tsv differs from the current SLIM release contract".to_owned(),
        );
    }
    for row in rows {
        let Some(path) = row.split('\t').nth(3) else {
            continue;
        };
        if !root.join(path).is_file() {
            errors.push(format!(
                "release contract cites missing canonical source `{path}`"
            ));
        }
    }

    let release_manifest =
        fs::read_to_string(root.join("release/manifest.txt")).unwrap_or_default();
    let release_roots: Vec<_> = release_manifest
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .collect();
    let expected_release_roots = [
        "Cargo.lock",
        "Cargo.toml",
        "DESIGN.md",
        "README.md",
        "ROADMAP.md",
        "VERSION",
        "benchmarks",
        "bootstrap",
        "bootstrap.sh",
        "conformance",
        "design",
        "docs",
        "examples",
        "release",
        "runtime",
        "scripts",
        "selfhost",
        "slimc",
        "src",
        "tests",
    ];
    if release_roots != expected_release_roots {
        errors.push("release/manifest.txt differs from the frozen source package".to_owned());
    }

    let packager = fs::read_to_string(root.join("scripts/package-release.sh")).unwrap_or_default();
    for required in [
        "status --porcelain --untracked-files=all",
        "slim_prefix=\"slim-$slim_version\"",
        "slim_archive=\"$slim_output/$slim_prefix.tar.gz\"",
        "SOURCE-MANIFEST.sha256",
        "touch -t 200001010000.00",
        "--format ustar",
        "--uid 0 --gid 0 --uname root --gname root",
        "gzip -n -9",
    ] {
        if !contains_slim_pattern(&packager, required) {
            errors.push(format!("Core 1L packager is missing `{required}`"));
        }
    }
    let release_test =
        fs::read_to_string(root.join("scripts/verify-release.sh")).unwrap_or_default();
    for required in [
        "cmp -s \"$slim_archive\" \"$slim_second\"",
        "SOURCE-MANIFEST.sha256",
        "./bootstrap.sh",
        "./slimc run benchmarks/challenges/sieve/program.slim",
        "mismatched runtime ABI compiled",
    ] {
        if !contains_slim_pattern(&release_test, required) {
            errors.push(format!(
                "Core 1L clean-package test is missing `{required}`"
            ));
        }
    }
    let release_gate = fs::read_to_string(root.join("scripts/verify-0.9.sh")).unwrap_or_default();
    for required in [
        "scripts/verify.sh",
        "scripts/verify-release.sh",
        "npm test",
        "SLIM 0.9 verification",
    ] {
        if !contains_slim_pattern(&release_gate, required) {
            errors.push(format!("Core 1L release gate is missing `{required}`"));
        }
    }

    let diagnostics = fs::read_to_string(root.join("docs/DIAGNOSTICS.md")).unwrap_or_default();
    let compatibility = fs::read_to_string(root.join("docs/COMPATIBILITY.md")).unwrap_or_default();
    for required in [
        "Schema: 1",
        "half-open primary byte span",
        "conformance/manifest.tsv",
        "conformance/projects/manifest.tsv",
    ] {
        if !contains_slim_pattern(&diagnostics, required) {
            errors.push(format!(
                "Core 1L diagnostic contract is missing `{required}`"
            ));
        }
    }
    for required in [
        "design/surface.tsv",
        "design/project-semantics.tsv",
        "0.y.0",
        "0.y.z",
        "runtime ABI 1",
        "future 1.0",
    ] {
        if !contains_slim_pattern(&compatibility, required) {
            errors.push(format!(
                "Core 1L compatibility contract is missing `{required}`"
            ));
        }
    }

    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "exposes_one_canonical_version_and_help_spelling",
        "generated_c_requires_the_exact_runtime_abi",
        "{\\\"schema\\\":1,\\\"code\\\":",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!("Core 1L permanent tests are missing `{required}`"));
        }
    }

    let status = fs::read_to_string(root.join("docs/STATUS.md")).unwrap_or_default();
    let design = fs::read_to_string(root.join("DESIGN.md")).unwrap_or_default();
    let roadmap = fs::read_to_string(root.join("ROADMAP.md")).unwrap_or_default();
    let release_evidence =
        fs::read_to_string(root.join("benchmarks/results/2026-07-23-core-1l-slim-1-0.md"))
            .unwrap_or_default();
    if !status.contains("Status: SLIM 0.9 — experimental, pre-1.0")
        || !design.contains("Status: SLIM 0.9 — experimental, pre-1.0")
        || !roadmap.contains("Status: SLIM 0.9 — experimental, pre-1.0")
        || !roadmap.contains("## Core 1L: internal stabilization milestone\n\nStatus: complete")
    {
        errors.push("Core 1L closure boundary is not canonical".to_owned());
    }
    for required in [
        "Version: 1.0.0",
        "RFCs: RFC-0082, RFC-0083",
        "2,154,365-byte compiler seed",
        "116 conformance fixtures",
        "2,000",
        "malformed-input mutations",
        "./scripts/verify-1.0.sh",
        "Darwin/arm64",
    ] {
        if !contains_slim_pattern(&release_evidence, required) {
            errors.push(format!("Core 1L evidence is missing `{required}`"));
        }
    }
}

fn check_core_1k_acceptance(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    for (id, label) in [
        ("RFC-0080", "finite byte equivalence and named cost vectors"),
        ("RFC-0081", "Core 1K closure"),
    ] {
        match rfcs.get(id) {
            Some(rfc)
                if rfc.status == "accepted"
                    && rfc.kind == "architecture"
                    && rfc.primitive == "none"
                    && rfc.score >= 60 => {}
            Some(_) => errors.push(format!(
                "Core 1K {label} requires an accepted primitive-free {id} scoring at least 60"
            )),
            None => errors.push(format!("Core 1K rfc {id} is missing")),
        }
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0080") || surface.contains("RFC-0081") {
        errors.push("Core 1K rfcs must not add Core language surface".to_owned());
    }

    for required in [
        "docs/QUALITY.md",
        "docs/REDUCTION.md",
        "benchmarks/results/2026-07-23-core-1k-semantic-quality.md",
        "conformance/evidence/u8-equivalent-left.slim",
        "conformance/evidence/u8-equivalent-right.slim",
        "conformance/evidence/u8-different.slim",
        "conformance/evidence/u8-unsupported-expression.slim",
        "conformance/evidence/reduction-nonapplicable.slim",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!("Core 1K artifact is missing {required}"));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    let quality = fs::read_to_string(root.join("selfhost/quality.slim")).unwrap_or_default();
    let equivalence =
        fs::read_to_string(root.join("selfhost/equivalence.slim")).unwrap_or_default();
    let reduce = fs::read_to_string(root.join("selfhost/reduce.slim")).unwrap_or_default();
    let proof = fs::read_to_string(root.join("selfhost/proof.slim")).unwrap_or_default();
    for (contents, label, required) in [
        (&analysis, "analysis", "(analysis 7"),
        (&quality, "quality", "(cost-vector 1"),
        (&quality, "quality", "\"dynamic-work-v1\""),
        (&quality, "quality", "\"peak-bytes-v1\""),
        (&equivalence, "equivalence", "(equivalence 2"),
        (&equivalence, "equivalence", "expression-tokens-v1"),
        (&equivalence, "equivalence", "(true 256)"),
        (&reduce, "reducer", "(true 6)"),
        (&reduce, "reducer", "(true 7)"),
        (&reduce, "reducer", "(true 8)"),
        (&proof, "proof", "(reduction-proof 2"),
        (&proof, "proof", "canonical-tokens-v1"),
        (&proof, "proof", "boolean-idempotence"),
        (&proof, "proof", "boolean-identity-match"),
        (&proof, "proof", "common-match-result"),
    ] {
        if !contains_slim_pattern(contents, required) {
            errors.push(format!("Core 1K {label} is missing `{required}`"));
        }
    }

    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "finite_equivalence_proves_or_returns_the_first_counterexample",
        "(domain-kind u8) (cases 256) (accepted-states 2)",
        "(counterexample (inputs 3)",
        "(reason unsupported-expression)",
        "boolean-idempotence",
        "boolean-identity-match",
        "common-match-result",
        "reduction-nonapplicable.slim",
        "assert_eq!(reduced_again.stdout",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!("Core 1K permanent tests are missing `{required}`"));
        }
    }

    let status = fs::read_to_string(root.join("docs/STATUS.md")).unwrap_or_default();
    let roadmap = fs::read_to_string(root.join("ROADMAP.md")).unwrap_or_default();
    if !status.contains("Status: SLIM 0.9 — experimental, pre-1.0")
        || !roadmap.contains("Current milestone: Pre-1.0 evidence-driven development")
        || !roadmap.contains("## Core 1K: semantic quality and reduction\n\nStatus: complete")
    {
        errors.push("Core 1K closure boundary is not canonical".to_owned());
    }
}

fn check_core_1j_acceptance(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0078") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "language"
                && rfc.primitive == "structured-fork"
                && rfc.score >= 40 => {}
        Some(_) => errors.push(
            "Core 1J requires accepted historical RFC-0078 ownership for `parallel` scoring at least 40"
                .to_owned(),
        ),
        None => errors.push("Core 1J `parallel` rfc RFC-0078 is missing".to_owned()),
    }
    match rfcs.get("RFC-0079") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1J closure requires accepted primitive-free RFC-0079 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1J closure rfc RFC-0079 is missing".to_owned()),
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    let parallel_rows: Vec<_> = surface
        .lines()
        .filter(|line| line.ends_with("\tRFC-0078"))
        .collect();
    if parallel_rows != ["syntax\tparallel\tlexical-two-call-parallelism\tRFC-0078"] {
        errors.push("RFC-0078 must own exactly one canonical `parallel` syntax row".to_owned());
    }

    for required in [
        "benchmarks/host/dual_fetch.slim",
        "benchmarks/host/dual_health.slim",
        "benchmarks/results/2026-07-23-core-1j-structured-concurrency.md",
        "conformance/pass/structured_parallel.slim",
        "conformance/fail/invalid_structured_parallel.slim",
        "conformance/fail/nonleading_structured_parallel.slim",
        "tests/fixtures/region_adoption.c",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!("Core 1J artifact is missing {required}"));
        }
    }

    let checker = fs::read_to_string(root.join("selfhost/check.slim")).unwrap_or_default();
    for required in [
        "(fn fork-task-valid",
        "(fn fork-leading-items",
        "\"E0356\"",
        "\"io.tcp_exchange\"",
        "\"io.monotonic_ms\"",
    ] {
        if !contains_slim_pattern(&checker, required) {
            errors.push(format!("Core 1J checker is missing `{required}`"));
        }
    }

    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    for required in [
        "(explicit Bool)",
        "(intent explicit)",
        "(profitability explicit)",
        "(race-free true) (deadlock-free true)",
    ] {
        if !contains_slim_pattern(&parallel, required) {
            errors.push(format!("Core 1J analysis is missing `{required}`"));
        }
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    for required in [
        "(fn emit_parallel_region_declarations",
        "(fn emit_parallel_region_adoption",
        "slim_parallel_first_region",
        "slim_region_adopt(slim_allocation_region",
        "(get site explicit)",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!("Core 1J lowering is missing `{required}`"));
        }
    }

    let runtime_h = fs::read_to_string(root.join("runtime/slim_rt.h")).unwrap_or_default();
    let runtime_c = fs::read_to_string(root.join("runtime/slim_rt.c")).unwrap_or_default();
    for required in ["_Atomic SlimAllocCode code", "void slim_region_adopt"] {
        if !contains_slim_pattern(&runtime_h, required) {
            errors.push(format!("Core 1J runtime interface is missing `{required}`"));
        }
    }
    for required in [
        "atomic_fetch_add",
        "void slim_region_adopt",
        "allocation->region = parent",
    ] {
        if !contains_slim_pattern(&runtime_c, required) {
            errors.push(format!(
                "Core 1J runtime implementation is missing `{required}`"
            ));
        }
    }

    let conformance = fs::read_to_string(root.join("conformance/manifest.tsv")).unwrap_or_default();
    for required in [
        "syntax:parallel",
        "diagnostic:E0356",
        "parallel:explicit-region",
    ] {
        if !contains_slim_pattern(&conformance, required) {
            errors.push(format!("Core 1J conformance is missing `{required}`"));
        }
    }

    let budgets =
        fs::read_to_string(root.join("benchmarks/performance-budgets.tsv")).unwrap_or_default();
    for required in [
        "structured-host-runtime-ratio\tdual_fetch\t0.75\tparallel-over-serial\tRFC-0078",
        "structured-host-runtime-ratio\tdual_health\t0.75\tparallel-over-serial\tRFC-0078",
    ] {
        if !budgets.contains(required) {
            errors.push(format!(
                "Core 1J performance budget is missing `{required}`"
            ));
        }
    }

    let e2e = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "explicit_structured_parallel_joins_loopback_requests_and_adopts_owned_results",
        "adopted_region_storage_remains_parent_owned_and_resizable",
        "SLIM_TASK_DISABLE",
        "SLIM_ALLOC_FAIL_AT",
    ] {
        if !contains_slim_pattern(&e2e, required) {
            errors.push(format!(
                "Core 1J integration evidence is missing `{required}`"
            ));
        }
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    if !benchmark.contains("fn run_structured_host_application")
        || !benchmark.contains("structured-host-runtime-ratio")
    {
        errors.push(
            "Core 1J loopback applications are absent from the permanent host gate".to_owned(),
        );
    }
}

fn check_parallelism_evidence(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0062") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1F evidence requires accepted primitive-free RFC-0062 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1F parallelism evidence rfc RFC-0062 is missing".to_owned()),
    }

    for required in [
        "docs/PARALLELISM.md",
        "selfhost/parallel.slim",
        "conformance/evidence/parallelism.slim",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!(
                "Core 1F parallelism artifact is missing {required}"
            ));
        }
    }

    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    for required in [
        "(record FunctionFact",
        "(function-limit 64)",
        "(edge-limit 4096)",
        "(resolution-pass-limit 64)",
        "(race-free true)",
        "(deadlock-free true)",
        "(profitability unknown)",
        "(call typing/fact_type",
        "(call typing/linked_binding_declaration",
    ] {
        if !contains_slim_pattern(&parallel, required) {
            errors.push(format!(
                "Core 1F bounded parallelism evidence is missing `{required}`"
            ));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    for required in [
        "(analysis 7",
        "(inout typed-facts (Vec typing/Fact))",
        "(call parallel/analyze source tokens typed-facts range-view)",
        "(call parallel/emit_module_facts source tokens parallel-view output)",
    ] {
        if !contains_slim_pattern(&analysis, required) {
            errors.push(format!(
                "Core 1F analysis does not consume the checked view through `{required}`"
            ));
        }
    }

    let compiler = fs::read_to_string(root.join("selfhost/slimc.slim")).unwrap_or_default();
    if !contains_slim_pattern(&compiler, "(call check/check_source input tokens)")
        || !contains_slim_pattern(
            &compiler,
            "(call analysis/emit_module input tokens typed_facts output)",
        )
        || !contains_slim_pattern(&compiler, "(fn analyze_project_path")
        || !contains_slim_pattern(&compiler, "(call project/prepare_project_path path)")
    {
        errors.push(
            "source and project analysis must consume normal checked artifacts without a second semantic path"
                .to_owned(),
        );
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0062") {
        errors
            .push("RFC-0062 has Primitive: none and must not add Core language surface".to_owned());
    }

    let budgets =
        fs::read_to_string(root.join("benchmarks/performance-budgets.tsv")).unwrap_or_default();
    if !budgets.contains("analysis-exponent\tgenerated-declarations\t1.25\texponent\tRFC-0062") {
        errors.push("Core 1F analysis scaling lacks its durable 1.25 budget".to_owned());
    }

    let e2e = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    if !e2e.contains("fn report_parentheses_are_balanced")
        || !e2e.contains("parallelism_analysis_reports_function_and_edge_bounds")
        || !e2e.contains("project_analysis_dogfoods_the_bounded_parallelism_view")
    {
        errors.push("Core 1F report structure and hard bounds lack durable tests".to_owned());
    }
}

fn check_integer_proof_evidence(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0063") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "integer proof evidence requires accepted primitive-free RFC-0063 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("integer proof evidence rfc RFC-0063 is missing".to_owned()),
    }

    for required in [
        "docs/INTEGER_PROOFS.md",
        "selfhost/ranges.slim",
        "conformance/evidence/integer_ranges.slim",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!("integer proof artifact is missing {required}"));
        }
    }

    let ranges = fs::read_to_string(root.join("selfhost/ranges.slim")).unwrap_or_default();
    for required in [
        "(record Fact ((analyzed Bool) (lower-known Bool)",
        "(call typing/linked_binding_declaration",
        "(call i64.ge value -1000000000)",
        "(call i64.le value 1000000000)",
        "(call i64.ge refinement-count 64)",
        "(domain -1000000000 1000000000)",
        "(refinement-limit 64)",
        "(checked-site-report-limit 64)",
    ] {
        if !contains_slim_pattern(&ranges, required) {
            errors.push(format!(
                "bounded integer proof implementation is missing `{required}`"
            ));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    for required in [
        "(analysis 7",
        "(call ranges/analyze source tokens typed-facts)",
        "(call ranges/emit-module-facts source tokens range-view output)",
    ] {
        if !contains_slim_pattern(&analysis, required) {
            errors.push(format!("analysis version 6 is missing `{required}`"));
        }
    }

    let quality = fs::read_to_string(root.join("selfhost/quality.slim")).unwrap_or_default();
    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    if !contains_slim_pattern(&quality, "(call ranges/fact-total range_facts body)")
        || !contains_slim_pattern(&parallel, "(call ranges/fact-total range_facts expr)")
    {
        errors.push(
            "quality and parallelism must consume the same exact-node integer totality facts"
                .to_owned(),
        );
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0063") {
        errors
            .push("RFC-0063 has Primitive: none and must not add Core language surface".to_owned());
    }

    let e2e = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "integer_ranges_prove_guarded_arithmetic_and_preserve_unknowns",
        "integer_range_refinement_limit_is_explicit_and_deterministic",
        "integer_checked_site_report_limit_is_explicit_and_deterministic",
        "(checked-site-count 65) (guarantee bounded)",
        "zero_divisor (guarantee exact) (status unavailable)",
        "domain_limit (guarantee exact) (status unavailable)",
    ] {
        if !contains_slim_pattern(&e2e, required) {
            errors.push(format!("integer proof evidence is missing `{required}`"));
        }
    }
}

fn check_resource_evidence(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0073") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1H resource evidence requires accepted primitive-free RFC-0073 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1H resource evidence rfc RFC-0073 is missing".to_owned()),
    }
    match rfcs.get("RFC-0074") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1H closure requires accepted primitive-free RFC-0074 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1H closure rfc RFC-0074 is missing".to_owned()),
    }

    for required in [
        "docs/RESOURCE_BOUNDS.md",
        "selfhost/ranges.slim",
        "conformance/evidence/resource_work.slim",
        "benchmarks/resource-baseline.tsv",
        "benchmarks/results/2026-07-23-core-1h-resource-evidence.md",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!(
                "Core 1H resource evidence artifact is missing {required}"
            ));
        }
    }

    let ranges = fs::read_to_string(root.join("selfhost/ranges.slim")).unwrap_or_default();
    for required in [
        "(record ResourceScan",
        "(profile-limit 16)",
        "(call-site-report-limit 64)",
        "(range-refinements-truncated ",
        "(reason nonliteral-controller)",
        "(false 16)",
    ] {
        if !contains_slim_pattern(&ranges, required) {
            errors.push(format!(
                "bounded resource evidence implementation is missing `{required}`"
            ));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    if !analysis.contains("(analysis 7")
        || !contains_slim_pattern(
            &analysis,
            "(call ranges/emit-module-facts source tokens range-view output)",
        )
    {
        errors.push(
            "analysis schema 7 must emit resource evidence from the shared checked range view"
                .to_owned(),
        );
    }

    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "resource_evidence_reports_exact_zero_and_unknown_recurrence_work",
        "resource_evidence_limits_profiles_and_reported_call_sites",
        "(recurrence-profile-count 17) (reported-recurrence-profiles 16)",
        "(profiled-call-site-count 65) (reported-call-site-count 64)",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!("resource evidence tests are missing `{required}`"));
        }
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "\"resources\" => run_resource_evidence()",
        "benchmarks/resource-baseline.tsv",
        "unknown_call_work_sites",
        "expression_nodes",
        "allocation_effect_functions",
        "partial_functions",
        "resource baseline line {} must have sixteen columns",
        "resource evidence changed; record the reason",
    ] {
        if !contains_slim_pattern(&benchmark, required) {
            errors.push(format!(
                "durable resource benchmark is missing `{required}`"
            ));
        }
    }

    let baseline =
        fs::read_to_string(root.join("benchmarks/resource-baseline.tsv")).unwrap_or_default();
    let baseline_challenges: BTreeSet<_> = baseline
        .lines()
        .filter(|line| !line.starts_with('#') && !line.is_empty())
        .filter_map(|line| line.split('\t').next())
        .collect();
    let manifest =
        fs::read_to_string(root.join("benchmarks/challenges/manifest.tsv")).unwrap_or_default();
    let manifest_challenges: BTreeSet<_> = manifest
        .lines()
        .filter(|line| !line.starts_with('#') && !line.is_empty())
        .filter_map(|line| line.split('\t').next())
        .collect();
    if !baseline.contains("# schema=1")
        || baseline_challenges.len() < 20
        || baseline_challenges != manifest_challenges
    {
        errors.push(
            "resource baseline must retain schema 1 and every application in the expanded corpus"
                .to_owned(),
        );
    }

    let verify = fs::read_to_string(root.join("scripts/verify.sh")).unwrap_or_default();
    if !verify.contains("slim-bench -- resources") {
        errors.push("full verification does not run the resource application baseline".to_owned());
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0073") || surface.contains("RFC-0074") {
        errors.push(
            "RFC-0073 and RFC-0074 have Primitive: none and must not add Core language surface"
                .to_owned(),
        );
    }

    let roadmap = fs::read_to_string(root.join("ROADMAP.md")).unwrap_or_default();
    if !roadmap
        .contains("## Core 1H: bounded resources and application evidence\n\nStatus: complete")
        || !roadmap.contains("## Core 1I: safe typed host boundary\n\nStatus: complete")
        || !roadmap.contains("## Core 1J: deterministic structured concurrency\n\nStatus: complete")
    {
        errors.push("Core 1H through Core 1J closure boundary is not canonical".to_owned());
    }
}

fn check_host_boundary(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0075") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "language"
                && rfc.primitive == "monotonic-clock"
                && rfc.score >= 40 => {}
        Some(_) => errors.push(
            "Core 1I clock requires accepted RFC-0075 monotonic-clock language surface scoring at least 40"
                .to_owned(),
        ),
        None => errors.push("Core 1I monotonic clock rfc RFC-0075 is missing".to_owned()),
    }
    match rfcs.get("RFC-0076") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "language"
                && rfc.primitive == "bounded-tcp-exchange"
                && rfc.score >= 40 => {}
        Some(_) => errors.push(
            "Core 1I network boundary requires accepted RFC-0076 bounded-tcp-exchange language surface scoring at least 40"
                .to_owned(),
        ),
        None => errors.push("Core 1I bounded TCP rfc RFC-0076 is missing".to_owned()),
    }
    match rfcs.get("RFC-0077") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1I closure requires accepted primitive-free RFC-0077 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1I closure rfc RFC-0077 is missing".to_owned()),
    }

    for required in [
        "docs/HOST.md",
        "benchmarks/host/needs.tsv",
        "conformance/pass/monotonic_clock.slim",
        "conformance/fail/missing_clock_effect.slim",
        "conformance/pass/tcp_exchange.slim",
        "conformance/fail/missing_tcp_effect.slim",
        "benchmarks/host/clock.slim",
        "benchmarks/host/clock.c",
        "benchmarks/host/tcp_client.slim",
        "benchmarks/results/2026-07-23-core-1i-monotonic-clock.md",
        "benchmarks/results/2026-07-23-core-1i-bounded-tcp.md",
        "tests/fixtures/unsupported_network.c",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!(
                "Core 1I host boundary artifact is missing {required}"
            ));
        }
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface
        .lines()
        .filter(|line| line.starts_with("builtin\tio.monotonic_ms\tmonotonic-clock\tRFC-0075"))
        .count()
        != 1
    {
        errors.push("io.monotonic_ms must have exactly one RFC-0075 surface row".to_owned());
    }
    if surface
        .lines()
        .filter(|line| line.starts_with("builtin\tio.tcp_exchange\tbounded-tcp-exchange\tRFC-0076"))
        .count()
        != 1
    {
        errors.push("io.tcp_exchange must have exactly one RFC-0076 surface row".to_owned());
    }
    for forbidden in ["builtin\tio.clock", "builtin\tio.now", "builtin\tio.time"] {
        if surface.contains(forbidden) {
            errors.push(format!(
                "monotonic clock alias is forbidden through `{forbidden}`"
            ));
        }
    }
    for forbidden in [
        "builtin\tio.socket",
        "builtin\tio.tcp-connect",
        "builtin\tio.network",
        "builtin\tio.http",
    ] {
        if surface.contains(forbidden) {
            errors.push(format!(
                "network or socket alias is forbidden through `{forbidden}`"
            ));
        }
    }

    for (path, required) in [
        (
            "selfhost/typing.slim",
            "(call infer_zero_scalar_builtin tokens expr arguments 3 issues)",
        ),
        (
            "selfhost/effects.slim",
            "(call syntax/ast_node_text_is source tokens callee \"io.monotonic_ms\")",
        ),
        ("selfhost/codegen.slim", "(true \"slim_monotonic_ms\")"),
        ("runtime/slim_rt.h", "int64_t slim_monotonic_ms(void);"),
        (
            "runtime/slim_rt.c",
            "static _Thread_local int64_t slim_last_monotonic_ms = 0;",
        ),
        ("selfhost/typing.slim", "(fn infer_tcp_exchange "),
        ("selfhost/effects.slim", "\"io.tcp_exchange\""),
        ("selfhost/codegen.slim", "(fn emit_tcp_exchange_call "),
        ("runtime/slim_rt.h", "bool slim_tcp_exchange("),
        ("runtime/slim_rt.c", "#if !defined(SLIM_POSIX_NETWORK)"),
    ] {
        let contents = fs::read_to_string(root.join(path)).unwrap_or_default();
        if !contains_slim_pattern(&contents, required) {
            errors.push(format!(
                "Core 1I clock implementation {path} is missing `{required}`"
            ));
        }
    }

    let manifest = fs::read_to_string(root.join("conformance/manifest.tsv")).unwrap_or_default();
    for required in [
        "monotonic-clock\trun\tconformance/pass/monotonic_clock.slim",
        "builtin:io.monotonic_ms,effect:io",
        "missing-clock-effect\tcheck-fail\tconformance/fail/missing_clock_effect.slim",
        "tcp-exchange\tcheck-pass\tconformance/pass/tcp_exchange.slim",
        "builtin:io.tcp_exchange,effect:alloc,effect:io,host:bounded-network",
        "missing-tcp-effect\tcheck-fail\tconformance/fail/missing_tcp_effect.slim",
    ] {
        if !contains_slim_pattern(&manifest, required) {
            errors.push(format!("Core 1I clock conformance is missing `{required}`"));
        }
    }

    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    if !tests.contains("monotonic_clock_is_typed_effectful_and_allocation_free")
        || !tests.contains("generated.matches(\"slim_monotonic_ms()\").count(), 2")
        || !tests.contains("(allocation-sites 0) (trap-sites 0)")
    {
        errors.push("Core 1I clock lacks its durable type/effect/runtime test".to_owned());
    }
    if !tests.contains("bounded_tcp_exchange_preserves_failure_state_and_closes_connections")
        || !tests.contains("generated.matches(\"slim_tcp_exchange(\").count(), 3")
        || !tests.contains("unsupported_network_target_returns_typed_failure")
        || !tests.contains("-DSLIM_DISABLE_NETWORK=1")
    {
        errors.push(
            "Core 1I TCP exchange lacks durable loopback and target-absence tests".to_owned(),
        );
    }

    let needs = fs::read_to_string(root.join("benchmarks/host/needs.tsv")).unwrap_or_default();
    for required in [
        "dual-endpoint-fetch\tsend and receive bounded bytes with deadline\tio.tcp-exchange\tRFC-0076",
        "child-tool-orchestration\tstart arbitrary executable and capture output\texternal launcher\tdefer",
        "filesystem-output\tpersist generated artifacts\tstdout plus external launcher\tdefer",
    ] {
        if !contains_slim_pattern(&needs, required) {
            errors.push(format!(
                "Core 1I host application matrix is missing `{required}`"
            ));
        }
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    let budgets =
        fs::read_to_string(root.join("benchmarks/performance-budgets.tsv")).unwrap_or_default();
    let verify = fs::read_to_string(root.join("scripts/verify.sh")).unwrap_or_default();
    if !benchmark.contains("\"host\" => run_host_evidence()")
        || !benchmark.contains("performance_budget(\"host-clock-runtime-ratio\", \"clock-100000\")")
        || !benchmark.contains("performance_budget(\"host-network-binary-ratio\", \"hello\")")
        || !budgets.contains("host-clock-runtime-ratio\tclock-100000\t2.00\tslim-over-c\tRFC-0075")
        || !budgets.contains(
            "host-network-binary-ratio\thello\t1.03\thost-over-network-disabled\tRFC-0076",
        )
        || !verify.contains("slim-bench -- host")
    {
        errors.push("Core 1I clock performance is not a permanent gated comparison".to_owned());
    }
}

fn check_parallelism_application_baseline(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0064") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "parallelism application baseline requires accepted primitive-free RFC-0064 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("parallelism application baseline rfc RFC-0064 is missing".to_owned()),
    }

    for required in [
        "benchmarks/parallelism-baseline.tsv",
        "benchmarks/results/2026-07-23-core-1f-application-baseline.md",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!(
                "parallelism application artifact is missing {required}"
            ));
        }
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "\"parallelism\" => run_parallelism_evidence()",
        "assert_eq!(first, second",
        "report_parentheses_are_balanced(&first)",
        "benchmarks/parallelism-baseline.tsv",
        "parallelism evidence changed; record the reason",
    ] {
        if !contains_slim_pattern(&benchmark, required) {
            errors.push(format!(
                "durable parallelism benchmark is missing `{required}`"
            ));
        }
    }

    let baseline =
        fs::read_to_string(root.join("benchmarks/parallelism-baseline.tsv")).unwrap_or_default();
    let baseline_challenges: BTreeSet<_> = baseline
        .lines()
        .filter(|line| !line.starts_with('#') && !line.is_empty())
        .filter_map(|line| line.split('\t').next())
        .collect();
    let manifest =
        fs::read_to_string(root.join("benchmarks/challenges/manifest.tsv")).unwrap_or_default();
    let manifest_challenges: BTreeSet<_> = manifest
        .lines()
        .filter(|line| !line.starts_with('#') && !line.is_empty())
        .filter_map(|line| line.split('\t').next())
        .collect();
    if !baseline.contains("# schema=5")
        || baseline_challenges.len() < 20
        || baseline_challenges != manifest_challenges
    {
        errors.push(
            "parallelism baseline must retain schema 5 and every application in the expanded corpus"
                .to_owned(),
        );
    }
    for challenge in [
        "gcd_fib",
        "sieve",
        "bfs",
        "matrix",
        "merge_sort",
        "bytefreq",
        "binary_search",
        "prefix_sum",
        "records",
        "variants",
        "state_machine",
        "signal_network",
        "arena_sum",
        "knapsack",
    ] {
        if !baseline
            .lines()
            .any(|line| line.starts_with(&format!("{challenge}\t")))
        {
            errors.push(format!("parallelism baseline is missing {challenge}"));
        }
    }

    let verify = fs::read_to_string(root.join("scripts/verify.sh")).unwrap_or_default();
    if !verify.contains("slim-bench -- parallelism") {
        errors
            .push("full verification does not run the parallelism application baseline".to_owned());
    }
    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0064") {
        errors
            .push("RFC-0064 has Primitive: none and must not add Core language surface".to_owned());
    }
}

fn check_complete_parallel_blockers(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0065") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "complete parallel blockers require accepted primitive-free RFC-0065 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("complete parallel blocker rfc RFC-0065 is missing".to_owned()),
    }

    if !root
        .join("benchmarks/results/2026-07-23-core-1f-blocker-sets.md")
        .is_file()
    {
        errors.push("complete blocker result artifact is missing".to_owned());
    }

    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    for required in [
        "(record Blockers ((declared-effects Bool) (exclusive-borrow Bool)",
        "(fn add_blocker",
        "(fn graph_blockers",
        "(fn enrich_graph_blockers",
        "(fn emit_blockers",
        "(call enrich_graph_blockers facts edges 0)",
        " (blockers",
    ] {
        if !contains_slim_pattern(&parallel, required) {
            errors.push(format!(
                "complete SLIM blocker view is missing `{required}`"
            ));
        }
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "fn parallelism_blockers(",
        "blocker_{column}",
        "every application function must have one complete blocker set",
        "parallelism baseline line {} must have thirty-five columns",
    ] {
        if !contains_slim_pattern(&benchmark, required) {
            errors.push(format!(
                "schema-2 blocker benchmark is missing `{required}`"
            ));
        }
    }

    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "safe_left (guarantee exact) (status safe) (blockers)",
        "(blockers allocation-or-io)",
        "(blockers recurrence)",
        "(blockers callee-not-safe)",
        "(blockers call-cycle)",
        "(blockers function-limit)",
        "(blockers edge-limit)",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!("complete blocker evidence is missing `{required}`"));
        }
    }

    let baseline =
        fs::read_to_string(root.join("benchmarks/parallelism-baseline.tsv")).unwrap_or_default();
    for required in [
        "# schema=5",
        "blocker_declared_effects",
        "blocker_checked_trap",
        "blocker_recurrence",
        "blocker_callee_not_safe",
    ] {
        if !contains_slim_pattern(&baseline, required) {
            errors.push(format!("schema-2 blocker baseline is missing `{required}`"));
        }
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0065") {
        errors
            .push("RFC-0065 has Primitive: none and must not add Core language surface".to_owned());
    }
}

fn check_total_recurrence_evidence(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0066") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "total recurrence evidence requires accepted primitive-free RFC-0066 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("total recurrence evidence rfc RFC-0066 is missing".to_owned()),
    }

    for required in [
        "benchmarks/challenges/state_machine/program.slim",
        "benchmarks/challenges/state_machine/program.c",
        "benchmarks/challenges/state_machine/program.rs",
        "benchmarks/results/2026-07-23-core-1f-total-recurrence.md",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!("total recurrence artifact is missing {required}"));
        }
    }

    let ranges = fs::read_to_string(root.join("selfhost/ranges.slim")).unwrap_or_default();
    for required in [
        "(fn divide-facts",
        "(fn remainder-fact",
        "(fn prove-tail-recurrence",
        "(fn decreasing-controller",
        "(call promote-total facts tail)",
        "(call promote-total facts body)",
    ] {
        if !contains_slim_pattern(&ranges, required) {
            errors.push(format!("total recurrence proof is missing `{required}`"));
        }
    }

    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    for required in [
        "(let recur_total Bool",
        "(call ranges/fact-total range_facts index)",
        "(let unsafe_recur Bool",
    ] {
        if !contains_slim_pattern(&parallel, required) {
            errors.push(format!(
                "parallel recurrence discharge is missing `{required}`"
            ));
        }
    }
    if parallel.contains("(call add_blocker scanned_blockers 1)") {
        errors.push(
            "declared capability presence must not masquerade as an observed runtime effect"
                .to_owned(),
        );
    }

    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "constant_remainder (guarantee exact) (status safe)",
        "possible_division_overflow (guarantee exact) (status unavailable)",
        "total_countdown (guarantee exact) (status safe)",
        "overdeclared (guarantee exact) (status safe)",
        "countdown_pair (guarantee exact) (status safe)",
        "(eligible-sites 4)",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!("total recurrence evidence is missing `{required}`"));
        }
    }

    let baseline =
        fs::read_to_string(root.join("benchmarks/parallelism-baseline.tsv")).unwrap_or_default();
    if !baseline
        .lines()
        .any(|line| line.starts_with("state_machine\t1172\t4\t1\t1\t2\t3\t"))
    {
        errors.push(
            "parallelism baseline must retain the positive state_machine application".to_owned(),
        );
    }

    let budgets =
        fs::read_to_string(root.join("benchmarks/performance-budgets.tsv")).unwrap_or_default();
    if !budgets.contains("native-runtime-ratio\tstate_machine\t2.50\tslim-over-c\tRFC-0066") {
        errors.push("state_machine lacks a durable native runtime budget".to_owned());
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0066") {
        errors
            .push("RFC-0066 has Primitive: none and must not add Core language surface".to_owned());
    }
}

fn check_deterministic_parallel_schedule(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0067") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "deterministic parallel scheduling requires accepted primitive-free RFC-0067 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("deterministic parallel schedule rfc RFC-0067 is missing".to_owned()),
    }

    if !root
        .join("benchmarks/results/2026-07-23-core-1f-schedule-selection.md")
        .is_file()
    {
        errors.push("deterministic schedule result artifact is missing".to_owned());
    }

    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    for required in [
        "(record Schedule ((candidates I64) (selected I64) (reported I64) (executable I64) (executed I64) (selected_until I64)))",
        "(schedule-limit 64)",
        "(call i64.ge index (get schedule selected_until))",
        "(call bool.and eligible after-previous)",
        "(policy lexical-earliest-nonoverlap)",
        "(candidate-sites ",
        "(selected-sites ",
        "(reported-sites ",
    ] {
        if !contains_slim_pattern(&parallel, required) {
            errors.push(format!(
                "deterministic SLIM schedule is missing `{required}`"
            ));
        }
    }

    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "fn parallelism_schedule_limit_is_explicit_and_deterministic()",
        "(candidate-sites 4) (selected-sites 3) (reported-sites 3)",
        "(candidate-sites 129) (selected-sites 65) (reported-sites 64)",
        "report.matches(\"(fork-site \").count(), 64",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!(
                "deterministic schedule evidence is missing `{required}`"
            ));
        }
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "candidate_sites: usize",
        "selected_sites: usize",
        "reported_sites: usize",
        "executable_sites: usize",
        "executed_sites: usize",
        "parallelism baseline line {} must have thirty-five columns",
    ] {
        if !contains_slim_pattern(&benchmark, required) {
            errors.push(format!(
                "schema-5 schedule benchmark is missing `{required}`"
            ));
        }
    }

    let baseline =
        fs::read_to_string(root.join("benchmarks/parallelism-baseline.tsv")).unwrap_or_default();
    for required in [
        "# schema=5",
        "candidate_sites\tselected_sites\treported_sites\texecutable_sites\texecuted_sites\teligible_sites",
        "state_machine\t1172\t4\t1\t1\t2\t3\t",
    ] {
        if !contains_slim_pattern(&baseline, required) {
            errors.push(format!(
                "schema-5 schedule baseline is missing `{required}`"
            ));
        }
    }
    let state = baseline
        .lines()
        .find(|line| line.starts_with("state_machine\t"))
        .unwrap_or_default();
    if !state.ends_with("\t4\t3\t3\t1\t1\t4") {
        errors.push(
            "state_machine RFC-0107 schedule baseline must remain exactly 4/3/3/1/1/4".to_owned(),
        );
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0067") {
        errors
            .push("RFC-0067 has Primitive: none and must not add Core language surface".to_owned());
    }
}

fn check_total_task_failure_semantics(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0068") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "architecture"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "total-task failure semantics require accepted primitive-free RFC-0068 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("total-task failure rfc RFC-0068 is missing".to_owned()),
    }

    let rfc = fs::read_to_string(root.join("design/rfcs/0068-total-task-failure-semantics.md"))
        .unwrap_or_default();
    for required in [
        "Worker-creation failure executes the same work inline",
        "A task cannot require\ncancellation because no accepted task can fail",
        "the parent owns the only join handle",
        "move each owned input exactly once",
    ] {
        if !contains_slim_pattern(&rfc, required) {
            errors.push(format!(
                "total-task failure contract is missing `{required}`"
            ));
        }
    }

    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    for required in [
        "(race-free true) (deadlock-free true)",
        "(let both-safe Bool",
        "(let independent Bool",
        "(call bool.and adjacent-let",
    ] {
        if !contains_slim_pattern(&parallel, required) {
            errors.push(format!("total-task proof is missing `{required}`"));
        }
    }

    let docs = fs::read_to_string(root.join("docs/PARALLELISM.md")).unwrap_or_default();
    for required in [
        "worker-creation failure",
        "cannot fail and a child",
        "cancellation and",
        "wait cycles do not exist",
    ] {
        if !contains_slim_pattern(&docs, required) {
            errors.push(format!(
                "parallel failure specification is missing `{required}`"
            ));
        }
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0068") {
        errors
            .push("RFC-0068 has Primitive: none and must not add Core language surface".to_owned());
    }
}

fn check_parallel_execution_boundary(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0069") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "runtime"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "parallel execution boundary requires accepted primitive-free RFC-0069 runtime rfc scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("parallel execution boundary rfc RFC-0069 is missing".to_owned()),
    }
    match rfcs.get("RFC-0070") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "runtime"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "tiered structured workers require accepted primitive-free RFC-0070 runtime rfc scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("tiered structured worker rfc RFC-0070 is missing".to_owned()),
    }
    match rfcs.get("RFC-0071") {
        Some(rfc)
            if rfc.status == "accepted"
                && rfc.kind == "runtime"
                && rfc.primitive == "none"
                && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "guarded automatic execution requires accepted primitive-free RFC-0071 runtime rfc scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("guarded automatic execution rfc RFC-0071 is missing".to_owned()),
    }

    for required in [
        "benchmarks/challenges/state_machine/program_parallel.c",
        "benchmarks/results/2026-07-23-core-1f-acceptance.md",
        "benchmarks/challenges/signal_network/program.slim",
        "benchmarks/challenges/signal_network/program.c",
        "benchmarks/challenges/signal_network/program.rs",
        "design/rfcs/0071-guarded-automatic-fork-join.md",
        "benchmarks/results/2026-07-23-core-1g-automatic-execution.md",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!(
                "parallel execution boundary artifact is missing {required}"
            ));
        }
    }

    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    for required in [
        "(record Site ((site I64) (first I64) (second I64) (join I64) (first-work I64) (second-work I64) (allocates Bool) (explicit Bool) (executable Bool)))",
        "(fn call-work",
        "(call ranges/recurrence-work",
        "(fn leading-let-site",
        "(minimum-task-iterations 1000000)",
        "(profitability-reason target-work-unavailable)",
        "(executable-sites ",
        "(executed-sites ",
        "(status enabled)",
    ] {
        if !contains_slim_pattern(&parallel, required) {
            errors.push(format!(
                "guarded automatic execution analysis is missing `{required}`"
            ));
        }
    }

    let reference =
        fs::read_to_string(root.join("benchmarks/challenges/state_machine/program_parallel.c"))
            .unwrap_or_default();
    for required in [
        "#include <pthread.h>",
        "pthread_create",
        "pthread_join",
        "left.output = run(left.remaining, left.input)",
    ] {
        if !contains_slim_pattern(&reference, required) {
            errors.push(format!("manual parallel reference is missing `{required}`"));
        }
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "\"parallel-runtime\" => run_parallel_runtime()",
        "fn run_parallel_runtime()",
        "manual-parallel-runtime-ratio",
        "manual parallel probe must preserve state-machine output",
        "[\"state_machine\", \"signal_network\"]",
        "generated-parallel-runtime-ratio",
        "generated parallel execution must preserve serial-fallback output",
        "SLIM_TASK_DISABLE",
    ] {
        if !contains_slim_pattern(&benchmark, required) {
            errors.push(format!("parallel runtime evidence is missing `{required}`"));
        }
    }
    let budgets =
        fs::read_to_string(root.join("benchmarks/performance-budgets.tsv")).unwrap_or_default();
    if !budgets.contains(
        "manual-parallel-runtime-ratio\tstate_machine\t2.00\tparallel-over-serial\tRFC-0069",
    ) {
        errors.push("manual parallel reference lacks its durable ratio budget".to_owned());
    }
    for required in [
        "generated-parallel-runtime-ratio\tstate_machine\t1.25\tparallel-over-serial\tRFC-0071",
        "generated-parallel-runtime-ratio\tsignal_network\t1.25\tparallel-over-serial\tRFC-0071",
        "native-runtime-ratio\tsignal_network\t2.50\tslim-over-c\tRFC-0071",
    ] {
        if !contains_slim_pattern(&budgets, required) {
            errors.push(format!(
                "guarded automatic execution budget is missing `{required}`"
            ));
        }
    }
    let baseline =
        fs::read_to_string(root.join("benchmarks/parallelism-baseline.tsv")).unwrap_or_default();
    for (challenge, schedule) in [
        ("state_machine", "\t4\t3\t3\t1\t1\t4"),
        ("signal_network", "\t8\t5\t5\t1\t1\t8"),
    ] {
        let row = baseline
            .lines()
            .find(|line| line.starts_with(&format!("{challenge}\t")))
            .unwrap_or_default();
        if !row.ends_with(schedule) {
            errors.push(format!(
                "{challenge} must retain its RFC-0107 candidate, selected, reported, executable, executed, and eligible counts"
            ));
        }
    }
    let verify = fs::read_to_string(root.join("scripts/verify.sh")).unwrap_or_default();
    if !verify.contains("slim-bench -- parallel-runtime --quick") {
        errors.push("full verification does not retain the parallel runtime probe".to_owned());
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    let driver = fs::read_to_string(root.join("slimc")).unwrap_or_default();
    let runtime_header = fs::read_to_string(root.join("runtime/slim_rt.h")).unwrap_or_default();
    let runtime_source = fs::read_to_string(root.join("runtime/slim_rt.c")).unwrap_or_default();
    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for forbidden in ["pthread_", "thrd_create", "thrd_join"] {
        if codegen.contains(forbidden) {
            errors.push(format!(
                "generated code must use only the tiered structured worker ABI: found `{forbidden}`"
            ));
        }
    }
    for required in [
        "(call parallel/analyze source tokens facts range_view)",
        "#define SLIM_PARALLEL 1",
        "slim_task_spawn",
        "slim_task_run_inline",
        "slim_task_join",
        "slim_parallel_first.slim_result",
        "slim_parallel_second.slim_result",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!(
                "guarded automatic execution code generation is missing `{required}`"
            ));
        }
    }
    for required in [
        "SLIM_WORKER_TIER",
        "-DSLIM_PARALLEL=1",
        "-DSLIM_POSIX_WORKERS=1",
        "-pthread",
    ] {
        if !contains_slim_pattern(&driver, required) {
            errors.push(format!(
                "tiered generated-program build selection is missing `{required}`"
            ));
        }
    }
    for required in [
        "#if defined(SLIM_PARALLEL)",
        "#if defined(SLIM_POSIX_WORKERS)",
        "bool slim_task_spawn",
        "void slim_task_run_inline",
        "void slim_task_join",
        "slim_task_disabled || slim_task_worker",
    ] {
        if !contains_slim_pattern(&runtime_header, required)
            && !contains_slim_pattern(&runtime_source, required)
        {
            errors.push(format!(
                "tiered structured worker ABI is missing `{required}`"
            ));
        }
    }
    for required in [
        "structured_worker_runtime_falls_back_and_prevents_nesting",
        "SLIM_TASK_FAIL_AT",
        "SLIM_TASK_DISABLE",
        "SLIM_TASK_JOIN_FAIL_AT",
        "-DSLIM_PARALLEL=1",
        "-DSLIM_POSIX_WORKERS=1",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!(
                "tiered structured worker test is missing `{required}`"
            ));
        }
    }
    for required in [
        "design/rfcs/0070-tiered-structured-worker-abi.md",
        "benchmarks/results/2026-07-23-core-1g-worker-abi.md",
        "tests/fixtures/parallel_runtime.c",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!(
                "tiered structured worker artifact is missing {required}"
            ));
        }
    }

    for required in [
        "production_codegen_executes_profitable_plan_with_serial_fallback",
        "benchmarks/challenges/signal_network/program.slim",
        "(task-work 2000000 2000000)",
        "(executable-sites 1) (executed-sites 1)",
        "(executable-sites 0) (executed-sites 0)",
        "SLIM_TASK_DISABLE",
        "SLIM_TASK_JOIN_FAIL_AT",
        "!fs::read_to_string(nested_generated)",
        "examples/hello.slim",
    ] {
        if !contains_slim_pattern(&tests, required) {
            errors.push(format!(
                "guarded automatic execution test is missing `{required}`"
            ));
        }
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    for rfc in ["RFC-0069", "RFC-0070", "RFC-0071"] {
        if surface.contains(rfc) {
            errors.push(format!(
                "{rfc} has Primitive: none and must not add Core language surface"
            ));
        }
    }
}

fn check_core_1e_acceptance(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0061") {
        Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
        Some(_) => errors
            .push("Core 1E acceptance requires accepted RFC-0061 scoring at least 60".to_owned()),
        None => errors.push("Core 1E acceptance rfc RFC-0061 is missing".to_owned()),
    }

    let roadmap = fs::read_to_string(root.join("ROADMAP.md")).unwrap_or_default();
    for required in [
        "### Core 1E: safety-preserving native efficiency\n\nStatus: complete",
        "RFC-0061 accepts Core 1E",
    ] {
        if !contains_slim_pattern(&roadmap, required) {
            errors.push(format!("Core 1E roadmap freeze is missing `{required}`"));
        }
    }

    let evidence =
        fs::read_to_string(root.join("benchmarks/results/2026-07-23-core-1e-progress.md"))
            .unwrap_or_default();
    if !evidence.contains("Status: milestone complete")
        || !evidence.contains("## Core 1E acceptance")
    {
        errors.push("Core 1E acceptance evidence is not frozen as complete".to_owned());
    }

    let budgets =
        fs::read_to_string(root.join("benchmarks/performance-budgets.tsv")).unwrap_or_default();
    for required in [
        "native-runtime-ratio\tsieve\t2.50\tslim-over-c\tRFC-0061",
        "native-runtime-ratio\tmatrix\t2.00\tslim-over-c\tRFC-0087",
        "native-runtime-ratio\tbytefreq\t2.50\tslim-over-c\tRFC-0061",
        "native-runtime-ratio\trecords\t2.50\tslim-over-c\tRFC-0061",
        "native-runtime-ratio\tvariants\t1.75\tslim-over-c\tRFC-0061",
    ] {
        if !contains_slim_pattern(&budgets, required) {
            errors.push(format!("Core 1E tightened budget is missing `{required}`"));
        }
    }
}

fn check_allocation_free_region_elision(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0060") {
        Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "allocation-free region elision requires accepted RFC-0060 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("allocation-free region rfc RFC-0060 is missing".to_owned()),
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    for required in [
        "(let allocation_effect Bool (call memory/function_plan_allocates function_plan)",
        "(let uses_child_region Bool (call bool.and local_region allocation_effect)",
        "if uses_child_region:",
        "slim_region_destroy(&slim_function_region)",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!(
                "allocation-free region elision is missing `{required}`"
            ));
        }
    }
    if codegen.contains("if local_region:") {
        errors.push("code generation restored unconditional empty child regions".to_owned());
    }
}

fn check_runtime_fast_paths(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0059") {
        Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "checked runtime fast paths require accepted RFC-0059 scoring at least 60".to_owned(),
        ),
        None => errors.push("checked runtime fast-path rfc RFC-0059 is missing".to_owned()),
    }
    match rfcs.get("RFC-0087") {
        Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "typed vector-set lowering requires accepted RFC-0087 scoring at least 60".to_owned(),
        ),
        None => errors.push("typed vector-set lowering rfc RFC-0087 is missing".to_owned()),
    }
    match rfcs.get("RFC-0098") {
        Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "versioned collection access requires accepted RFC-0098 scoring at least 60".to_owned(),
        ),
        None => errors.push("versioned collection access rfc RFC-0098 is missing".to_owned()),
    }

    let header = fs::read_to_string(root.join("runtime/slim_rt.h")).unwrap_or_default();
    for required in [
        "static inline int64_t slim_i64_add",
        "static inline int64_t slim_i64_sub",
        "static inline int64_t slim_i64_mul",
        "static inline uint8_t slim_bytes_get",
        "static inline size_t slim_vec_check_index",
        "static inline bool slim_vec_push",
        "static inline void slim_vec_set",
        "if (vector->capacity > INT64_MAX / 2)",
    ] {
        if !contains_slim_pattern(&header, required) {
            errors.push(format!("checked runtime fast path is missing `{required}`"));
        }
    }

    let runtime = fs::read_to_string(root.join("runtime/slim_rt.c")).unwrap_or_default();
    for duplicate in [
        "int64_t slim_i64_add(",
        "uint8_t slim_bytes_get(",
        "size_t slim_vec_check_index(",
        "bool slim_vec_push(",
        "void slim_vec_set(",
    ] {
        if runtime.contains(duplicate) {
            errors.push(format!(
                "runtime source duplicates header fast path `{duplicate}`"
            ));
        }
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    for required in [
        "(fn emit_vec_set_call ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact))",
        "(let vector-type I64 (call fact_type_index facts arguments)",
        "(fn emit_versioned_index ",
        "\").len > INT64_C(\"",
        "\" : slim_vec_check_index(&(\"",
        "(call emit_versioned_index source tokens params arguments index output range-facts)",
        "\"] = \"",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!(
                "typed vector-set lowering is missing checked fact use `{required}`"
            ));
        }
    }

    let manifest = fs::read_to_string(root.join("conformance/manifest.tsv")).unwrap_or_default();
    for fixture in [
        "subtraction-overflow\ttrap\t",
        "multiplication-overflow\ttrap\t",
        "division-zero\ttrap\t",
        "division-overflow\ttrap\t",
        "remainder-zero\ttrap\t",
        "u8-conversion-bounds\ttrap\t",
        "bytes-bounds\ttrap\t",
        "vector-set-bounds\ttrap\t",
    ] {
        if !manifest.contains(fixture) {
            errors.push(format!(
                "checked runtime trap fixture is missing `{fixture}`"
            ));
        }
    }
    let tests = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    if !tests.contains("typed_vector_set_preserves_aggregate_values_and_bounds_checks") {
        errors.push("typed vector-set aggregate fixture is missing".to_owned());
    }
}

fn check_core_1d_acceptance(root: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    match rfcs.get("RFC-0058") {
        Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
        Some(_) => errors
            .push("Core 1D acceptance requires accepted RFC-0058 scoring at least 60".to_owned()),
        None => errors.push("Core 1D acceptance rfc RFC-0058 is missing".to_owned()),
    }

    let roadmap = fs::read_to_string(root.join("ROADMAP.md")).unwrap_or_default();
    for required in [
        "### Core 1D: complete typed compiler view\n\nStatus: complete",
        "Core 1D is accepted by RFC-0058.",
    ] {
        if !contains_slim_pattern(&roadmap, required) {
            errors.push(format!("Core 1D roadmap freeze is missing `{required}`"));
        }
    }

    let evidence =
        fs::read_to_string(root.join("benchmarks/results/2026-07-23-core-1d-progress.md"))
            .unwrap_or_default();
    if !evidence.contains("Status: milestone complete")
        || !evidence.contains("## Core 1D acceptance audit")
    {
        errors.push("Core 1D acceptance evidence is not frozen as complete".to_owned());
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    for forbidden in [
        "link_declaration_names",
        "syntax/name_is_inout",
        "effects/",
        "(fn find_parameter_type",
        "(fn find_record_item",
        "(fn find_variant_item",
        "(fn find_variant_case",
        "(fn find_record_field",
        "(fn linked_source_type",
    ] {
        if codegen.contains(forbidden) {
            errors.push(format!(
                "Core 1D code generation restored semantic rediscovery `{forbidden}`"
            ));
        }
    }
}

fn check_performance_architecture(
    root: &Path,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    match rfcs.get("RFC-0086") {
        Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
        Some(_) => errors.push(
            "canonical O3 native builds require accepted RFC-0086 scoring at least 60".to_owned(),
        ),
        None => errors.push("canonical O3 native-build rfc RFC-0086 is missing".to_owned()),
    }
    let launcher = fs::read_to_string(root.join("slimc")).unwrap_or_default();
    if launcher.matches("-std=c11 -O3 -DNDEBUG").count() != 3
        || launcher.contains("-std=c11 -O2 -DNDEBUG")
    {
        errors.push(
            "slimc build/run must retain one canonical O3 native optimization level".to_owned(),
        );
    }

    for relative in [
        "docs/PERFORMANCE.md",
        "design/rfcs/0030-durable-performance-evidence.md",
        "benchmarks/performance-budgets.tsv",
        "benchmarks/challenges/manifest.tsv",
        "benchmarks/agent/manifest.tsv",
    ] {
        if !root.join(relative).is_file() {
            errors.push(format!("performance architecture is missing {relative}"));
        }
    }

    let budget_path = root.join("benchmarks/performance-budgets.tsv");
    let mut budgets = BTreeSet::new();
    if let Ok(text) = fs::read_to_string(&budget_path) {
        for (line_index, line) in text.lines().enumerate() {
            if line.is_empty() || line.starts_with('#') {
                continue;
            }
            let columns: Vec<_> = line.split('\t').collect();
            if columns.len() != 5 {
                errors.push(format!(
                    "performance-budgets.tsv:{} must have five columns",
                    line_index + 1
                ));
                continue;
            }
            let key = format!("{}/{}", columns[0], columns[1]);
            if !budgets.insert(key.clone()) {
                errors.push(format!("duplicate performance budget {key}"));
            }
            match columns[2].parse::<f64>() {
                Ok(limit) if limit.is_finite() && limit > 0.0 => {}
                _ => errors.push(format!(
                    "performance-budgets.tsv:{} has an invalid positive limit",
                    line_index + 1
                )),
            }
            match rfcs.get(columns[4]) {
                Some(rfc) if rfc.status == "accepted" && rfc.score >= 60 => {}
                Some(_) => errors.push(format!(
                    "performance budget {key} requires an accepted rfc scoring at least 60"
                )),
                None => errors.push(format!(
                    "performance budget {key} cites missing {}",
                    columns[4]
                )),
            }
        }
    }

    let manifest_path = root.join("benchmarks/challenges/manifest.tsv");
    let mut challenges = BTreeSet::new();
    let mut challenge_features = BTreeSet::new();
    if let Ok(text) = fs::read_to_string(&manifest_path) {
        for (line_index, line) in text.lines().enumerate() {
            if line.is_empty() || line.starts_with('#') {
                continue;
            }
            let columns: Vec<_> = line.split('\t').collect();
            if columns.len() != 9 {
                errors.push(format!(
                    "challenges/manifest.tsv:{} must have nine columns",
                    line_index + 1
                ));
                continue;
            }
            let challenge = columns[0];
            challenge_features.extend(columns[6].split(',').map(str::to_owned));
            if columns[1..7].iter().any(|field| field.is_empty()) {
                errors.push(format!(
                    "challenges/manifest.tsv:{} has empty publication metadata",
                    line_index + 1
                ));
            }
            if !matches!(columns[7], "yes" | "no") {
                errors.push(format!(
                    "challenges/manifest.tsv:{} featured must be yes or no",
                    line_index + 1
                ));
            }
            if !matches!(columns[8], "pedagogical" | "benchmark") {
                errors.push(format!(
                    "challenges/manifest.tsv:{} shape must be pedagogical or benchmark",
                    line_index + 1
                ));
            }
            if !challenges.insert(challenge.to_owned()) {
                errors.push(format!("duplicate challenge {challenge}"));
            }
            for file in ["program.slim", "program.c", "program.rs"] {
                if !root
                    .join("benchmarks/challenges")
                    .join(challenge)
                    .join(file)
                    .is_file()
                {
                    errors.push(format!("challenge {challenge} is missing {file}"));
                }
            }
            if !budgets.contains(&format!("native-runtime-ratio/{challenge}")) {
                errors.push(format!(
                    "challenge {challenge} has no native runtime performance budget"
                ));
            }
        }
    }
    if challenges.len() < 12 {
        errors.push(format!(
            "durable challenge corpus must retain at least 12 workloads, found {}",
            challenges.len()
        ));
    }
    let featured = fs::read_to_string(&manifest_path)
        .unwrap_or_default()
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .filter(|line| line.split('\t').nth(7) == Some("yes"))
        .count();
    if featured != 6 {
        errors.push(format!(
            "algorithm gallery must retain exactly six featured walkthroughs, found {featured}"
        ));
    }
    for feature in [
        "scalar",
        "vector",
        "record",
        "variant",
        "arena",
        "mutation",
        "recursion",
        "checked-arithmetic",
        "bytes",
        "graph",
    ] {
        if !challenge_features.contains(feature) {
            errors.push(format!(
                "durable challenge corpus is missing `{feature}` coverage"
            ));
        }
    }

    for required in [
        "check-exponent/generated-declarations",
        "check-exponent/generated-nested-bindings",
        "check-exponent/generated-named-type-parameters",
        "check-exponent/generated-owned-transfers",
        "emit-exponent/generated-declarations",
        "emit-exponent/generated-computed-arguments",
        "emit-exponent/generated-aggregate-temporaries",
        "emit-exponent/generated-planned-allocation-calls",
        "emit-exponent/generated-inout-binding-reads",
        "emit-check-ratio/generated-2000",
        "incremental-exponent/wide-no-change",
        "incremental-exponent/wide-private-body",
        "incremental-exponent/wide-public-interface",
        "incremental-exponent/deep-no-change",
        "incremental-exponent/deep-private-body",
        "incremental-exponent/deep-public-interface",
        "project-emit-exponent/wide",
        "project-emit-exponent/deep",
    ] {
        if !budgets.contains(required) {
            errors.push(format!("required performance budget {required} is missing"));
        }
    }

    let agent_path = root.join("benchmarks/agent/manifest.tsv");
    let mut agent_cases = BTreeSet::new();
    if let Ok(text) = fs::read_to_string(&agent_path) {
        for (line_index, line) in text.lines().enumerate() {
            if line.is_empty() || line.starts_with('#') {
                continue;
            }
            let columns: Vec<_> = line.split('\t').collect();
            if columns.len() != 2 {
                errors.push(format!(
                    "agent/manifest.tsv:{} must have two columns",
                    line_index + 1
                ));
                continue;
            }
            let case = columns[0];
            if !agent_cases.insert(case.to_owned()) {
                errors.push(format!("duplicate agent case {case}"));
            }
            for (language, extension) in [("slim", "slim"), ("c", "c"), ("rust", "rs")] {
                for role in ["broken", "fixed"] {
                    let relative =
                        format!("benchmarks/agent/cases/{case}/{language}/{role}.{extension}");
                    if !root.join(&relative).is_file() {
                        errors.push(format!("agent case {case} is missing {relative}"));
                    }
                }
            }
        }
    }
    if agent_cases.len() < 5 {
        errors.push(format!(
            "durable agent corpus must retain at least 5 cases, found {}",
            agent_cases.len()
        ));
    }

    let verify = fs::read_to_string(root.join("scripts/verify.sh")).unwrap_or_default();
    for command in [
        "performance --quick",
        "compare --quick",
        "slim-bench -- agent",
    ] {
        if !verify.contains(command) {
            errors.push(format!("release verification is missing `{command}`"));
        }
    }
    let bootstrap = fs::read_to_string(root.join("bootstrap.sh")).unwrap_or_default();
    if !bootstrap.contains("slimc.next") || !bootstrap.contains("mv -f") {
        errors.push("bootstrap compiler installation must remain atomic".to_owned());
    }
    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "enforce_scaling_series(\"incremental-exponent\"",
        "enforce_scaling_series(\"project-emit-exponent\"",
        "performance_budget(\"native-runtime-ratio\"",
        "performance_budget(\"check-exponent\", \"generated-nested-bindings\")",
        "performance_budget(\"check-exponent\", \"generated-named-type-parameters\")",
        "performance_budget(\"check-exponent\", \"generated-owned-transfers\")",
        "performance_budget(\"emit-exponent\", budget_subject)",
        "\"generated-computed-arguments\"",
        "\"generated-aggregate-temporaries\"",
        "\"generated-planned-allocation-calls\"",
        "\"generated-inout-binding-reads\"",
        "let rounds = balanced_round_count(samples);",
        "order.reverse();",
        "let elapsed = median_duration(&mut times);",
        "fn generated_computed_argument_program(calls: usize)",
        "fn generated_aggregate_temporary_program(fields: usize)",
        "fn generated_planned_allocation_call_program(calls: usize)",
        "fn generated_inout_read_program(parameters: usize)",
        "fn generated_named_type_program(functions: usize)",
        "fn generated_owned_transfer_program(transfers: usize)",
        "fn agent_manifest()",
    ] {
        if !contains_slim_pattern(&benchmark, required) {
            errors.push(format!(
                "performance benchmark implementation is missing `{required}`"
            ));
        }
    }
}

fn check_toolchain_cutover(root: &Path, errors: &mut Vec<String>) {
    let required = [
        "bootstrap/slimc-seed.c",
        "bootstrap/slimc-seed.sha256",
        "bootstrap.sh",
        "scripts/refresh-bootstrap-seed.sh",
        "selfhost/slim.project",
        "slimc",
    ];
    for path in required {
        if !root.join(path).is_file() {
            errors.push(format!("toolchain cutover artifact is missing {path}"));
        }
    }

    let forbidden_rust = [
        "src/ast.rs",
        "src/bin/slim-bootstrap.rs",
        "src/bootstrap.rs",
        "src/codegen.rs",
        "src/compiler.rs",
        "src/diagnostic.rs",
        "src/driver.rs",
        "src/formatter.rs",
        "src/incremental.rs",
        "src/lexer.rs",
        "src/lib.rs",
        "src/main.rs",
        "src/parser.rs",
        "src/project/cache.rs",
        "src/project/interface.rs",
        "src/project/manifest.rs",
        "src/project/mod.rs",
        "src/project/resolver.rs",
        "src/project/session.rs",
        "src/sema.rs",
        "src/sexpr.rs",
        "src/span.rs",
    ];
    for path in forbidden_rust {
        if root.join(path).exists() {
            errors.push(format!(
                "active Rust semantic compiler path was reintroduced: {path}"
            ));
        }
    }

    let budget = fs::read_to_string(root.join("design/rust-budget.tsv")).unwrap_or_default();
    if budget.lines().any(|line| line.contains("\tproduction\t")) {
        errors.push("toolchain cutover forbids production Rust budget entries".to_owned());
    }

    let cargo = fs::read_to_string(root.join("Cargo.toml")).unwrap_or_default();
    for forbidden in ["name = \"slimc\"", "name = \"slim-bootstrap\"", "[lib]"] {
        if cargo.contains(forbidden) {
            errors.push(format!(
                "Cargo manifest reintroduces a semantic compiler target `{forbidden}`"
            ));
        }
    }

    for script in ["bootstrap.sh", "slimc", "scripts/refresh-bootstrap-seed.sh"] {
        let source = fs::read_to_string(root.join(script)).unwrap_or_default();
        for forbidden in ["cargo", "rustc"] {
            if source.contains(forbidden) {
                errors.push(format!(
                    "production toolchain script {script} depends on forbidden `{forbidden}`"
                ));
            }
        }
    }

    let rfc = fs::read_to_string(root.join("design/rfcs/0027-portable-c-bootstrap-seed.md"))
        .unwrap_or_default();
    if !rfc.contains("Status: accepted") {
        errors.push("toolchain cutover requires accepted rfc RFC-0027".to_owned());
    }
}

fn check_conformance_coverage(root: &Path, errors: &mut Vec<String>) {
    check_ledger_coverage(
        &root.join("design/surface.tsv"),
        &root.join("conformance/manifest.tsv"),
        "accepted surface",
        errors,
    );
    check_ledger_coverage(
        &root.join("design/project-semantics.tsv"),
        &root.join("conformance/projects/manifest.tsv"),
        "accepted project behavior",
        errors,
    );
}

fn check_ledger_coverage(
    ledger_path: &Path,
    manifest_path: &Path,
    description: &str,
    errors: &mut Vec<String>,
) {
    let Ok(ledger) = fs::read_to_string(ledger_path) else {
        return;
    };
    let manifest = match fs::read_to_string(manifest_path) {
        Ok(manifest) => manifest,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", manifest_path.display()));
            return;
        }
    };
    let required: BTreeSet<_> = ledger
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .filter_map(|line| {
            let columns: Vec<_> = line.split('\t').collect();
            (columns.len() == 4).then(|| format!("{}:{}", columns[0], columns[1]))
        })
        .collect();
    let covered: BTreeSet<_> = manifest
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .filter_map(|line| line.split('\t').nth(5))
        .flat_map(|tags| tags.split(','))
        .map(str::to_owned)
        .collect();
    for missing in required.difference(&covered) {
        errors.push(format!(
            "{description} `{missing}` has no conformance coverage tag"
        ));
    }
}

fn load_rfcs(dir: &Path, errors: &mut Vec<String>) -> BTreeMap<String, Rfc> {
    let mut rfcs = BTreeMap::new();
    let entries = match fs::read_dir(dir) {
        Ok(entries) => entries,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", dir.display()));
            return rfcs;
        }
    };

    for entry in entries.flatten() {
        let path = entry.path();
        if path.extension().and_then(|value| value.to_str()) != Some("md") {
            continue;
        }
        let file_name = path
            .file_name()
            .and_then(|value| value.to_str())
            .unwrap_or("");
        let Some(number) = file_name.split('-').next() else {
            continue;
        };
        if number.len() != 4 || !number.chars().all(|ch| ch.is_ascii_digit()) {
            errors.push(format!("invalid rfc filename {file_name}"));
            continue;
        }
        let id = format!("RFC-{number}");

        let text = match fs::read_to_string(&path) {
            Ok(text) => text,
            Err(error) => {
                errors.push(format!("cannot read {}: {error}", path.display()));
                continue;
            }
        };
        let fields = parse_fields(&text);
        let process = fields.get("Process").map(String::as_str).unwrap_or("");
        let required_headings: &[&str] = match process {
            "legacy" => &LEGACY_RFC_HEADINGS,
            "1" => &CURRENT_RFC_HEADINGS,
            _ => &[],
        };
        for heading in required_headings {
            if !text.lines().any(|line| line.trim() == *heading) {
                errors.push(format!("{file_name} is missing {heading}"));
            }
        }
        if !text.starts_with(&format!("# {id}: ")) {
            errors.push(format!("{file_name} title must start with `# {id}: `"));
        }

        let required_field = |name: &str, errors: &mut Vec<String>| -> String {
            match fields.get(name) {
                Some(value) => value.clone(),
                None => {
                    errors.push(format!("{file_name} is missing {name}:"));
                    String::new()
                }
            }
        };
        let status = required_field("Status", errors);
        let implementation = required_field("Implementation", errors);
        let process = required_field("Process", errors);
        let audience = if process == "1" {
            for name in ["Author", "Created", "DecisionDate", "Approver"] {
                required_field(name, errors);
            }
            required_field("Audience", errors)
        } else {
            String::new()
        };
        let kind = required_field("Kind", errors);
        let primitive = required_field("Primitive", errors);
        let mut ratings = [0; 6];
        for (index, name) in [
            "Safety", "Compile", "Runtime", "Minimal", "Analysis", "Dogfood",
        ]
        .iter()
        .enumerate()
        {
            let value = required_field(name, errors);
            ratings[index] = parse_number(&value, file_name, name, errors);
            if !(-2..=2).contains(&ratings[index]) {
                errors.push(format!(
                    "{file_name} {name} rating must be between -2 and 2"
                ));
            }
        }
        let score_text = required_field("Score", errors);
        let score = parse_number(&score_text, file_name, "Score", errors);

        if rfcs
            .insert(
                id.clone(),
                Rfc {
                    id: id.clone(),
                    status,
                    implementation,
                    process,
                    audience,
                    kind,
                    primitive,
                    ratings,
                    score,
                },
            )
            .is_some()
        {
            errors.push(format!("duplicate RFC id {id}"));
        }
    }
    rfcs
}

fn parse_fields(text: &str) -> BTreeMap<String, String> {
    text.lines()
        .filter_map(|line| {
            let (key, value) = line.split_once(':')?;
            if key.chars().all(|ch| ch.is_ascii_alphabetic()) {
                Some((key.to_owned(), value.trim().to_owned()))
            } else {
                None
            }
        })
        .collect()
}

fn parse_number(value: &str, file: &str, field: &str, errors: &mut Vec<String>) -> i32 {
    value.parse().unwrap_or_else(|_| {
        errors.push(format!("{file} has invalid integer for {field}: {value}"));
        0
    })
}

fn check_rfcs(rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    let mut primitives = BTreeSet::new();
    let mut legacy_accepted = 0;
    let mut legacy_rejected = 0;
    for rfc in rfcs.values() {
        if !matches!(
            rfc.status.as_str(),
            "proposed" | "accepted" | "rejected" | "withdrawn" | "superseded"
        ) {
            errors.push(format!("{} has unknown status {}", rfc.id, rfc.status));
        }
        if !matches!(
            rfc.implementation.as_str(),
            "pending" | "complete" | "not-planned"
        ) {
            errors.push(format!(
                "{} has unknown implementation state {}",
                rfc.id, rfc.implementation
            ));
        }
        if !matches!(rfc.process.as_str(), "legacy" | "1") {
            errors.push(format!(
                "{} has unknown RFC process {}",
                rfc.id, rfc.process
            ));
        }
        if !matches!(
            rfc.kind.as_str(),
            "language" | "architecture" | "runtime" | "dependency" | "compatibility" | "process"
        ) {
            errors.push(format!("{} has unknown kind {}", rfc.id, rfc.kind));
        }
        if rfc.process == "1" && !matches!(rfc.audience.as_str(), "user" | "developer" | "both") {
            errors.push(format!("{} has unknown audience {}", rfc.id, rfc.audience));
        }
        match (rfc.status.as_str(), rfc.implementation.as_str()) {
            ("proposed", "pending")
            | ("accepted", "pending")
            | ("accepted", "complete")
            | ("rejected", "not-planned")
            | ("withdrawn", "not-planned")
            | ("superseded", "complete")
            | ("superseded", "not-planned") => {}
            _ => errors.push(format!(
                "{} status {} is incompatible with implementation {}",
                rfc.id, rfc.status, rfc.implementation
            )),
        }
        if rfc.process == "legacy" {
            if rfc.status == "accepted" {
                legacy_accepted += 1;
            } else if rfc.status == "rejected" {
                legacy_rejected += 1;
            }
        }
        let weights = [20, 20, 20, 20, 15, 5];
        let weighted: i32 = rfc
            .ratings
            .iter()
            .zip(weights)
            .map(|(rating, weight)| rating * weight)
            .sum();
        if weighted % 2 != 0 {
            errors.push(format!(
                "{} score is fractional; choose ratings whose weighted sum is even",
                rfc.id
            ));
        }
        let calculated = weighted / 2;
        if rfc.score != calculated {
            errors.push(format!(
                "{} declares score {} but calculated score is {}",
                rfc.id, rfc.score, calculated
            ));
        }
        if rfc.status == "accepted" && rfc.kind == "language" {
            if rfc.score < 40 {
                errors.push(format!("{} accepted language score is below 40", rfc.id));
            }
            if rfc.ratings[..4].iter().any(|rating| *rating < 0) {
                errors.push(format!(
                    "{} has a negative hard-gate rating in safety/compile/runtime/minimal",
                    rfc.id
                ));
            }
            if !rfc.ratings.contains(&2) {
                errors.push(format!("{} has no primary +2 benefit", rfc.id));
            }
        }
        if rfc.status == "accepted"
            && rfc.primitive != "none"
            && !primitives.insert(rfc.primitive.clone())
        {
            errors.push(format!(
                "{} duplicates accepted primitive {}",
                rfc.id, rfc.primitive
            ));
        }
    }
    if legacy_accepted != 98 || legacy_rejected != 8 {
        errors.push(format!(
            "legacy RFC disposition drift: expected 98 accepted and 8 rejected, found {legacy_accepted} accepted and {legacy_rejected} rejected"
        ));
    }
}

fn contains_legacy_rfc_id(text: &str) -> bool {
    text.as_bytes()
        .windows(5)
        .any(|window| window[0] == b'D' && window[1..].iter().all(u8::is_ascii_digit))
}

fn visit_repository_text_files(root: &Path, dir: &Path, action: &mut impl FnMut(&Path, &str)) {
    let Ok(entries) = fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            let name = path
                .file_name()
                .and_then(|value| value.to_str())
                .unwrap_or("");
            if matches!(name, ".git" | ".next" | "node_modules" | "out" | "target") {
                continue;
            }
            visit_repository_text_files(root, &path, action);
            continue;
        }
        let extension = path
            .extension()
            .and_then(|value| value.to_str())
            .unwrap_or("");
        let file_name = path
            .file_name()
            .and_then(|value| value.to_str())
            .unwrap_or("");
        if !matches!(
            extension,
            "c" | "json"
                | "lock"
                | "md"
                | "mjs"
                | "rs"
                | "sh"
                | "slim"
                | "toml"
                | "ts"
                | "tsv"
                | "tsx"
        ) && !matches!(file_name, "VERSION" | "slimc")
        {
            continue;
        }
        if let Ok(text) = fs::read_to_string(&path) {
            action(path.strip_prefix(root).unwrap_or(&path), &text);
        }
    }
}

fn check_rfc_migration(root: &Path, errors: &mut Vec<String>) {
    let legacy_directory = root.join("design").join("decisions");
    if legacy_directory.exists() {
        errors.push("legacy design decision directory remains after RFC migration".to_owned());
    }
    let legacy_path = ["design", "decisions"].join("/");
    visit_repository_text_files(root, root, &mut |path, text| {
        if text.contains(&legacy_path) {
            errors.push(format!(
                "{} retains the legacy decision-directory spelling",
                path.display()
            ));
        }
        if contains_legacy_rfc_id(text) {
            errors.push(format!(
                "{} retains a legacy decision identifier",
                path.display()
            ));
        }
    });
}

fn check_surface(path: &Path, rfcs: &BTreeMap<String, Rfc>, errors: &mut Vec<String>) {
    let text = match fs::read_to_string(path) {
        Ok(text) => text,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", path.display()));
            return;
        }
    };
    let mut names = BTreeSet::new();
    let mut roles = BTreeSet::new();
    let mut builtin_names = BTreeSet::new();
    for (line_index, line) in text.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() != 4 {
            errors.push(format!(
                "surface.tsv:{} must have four columns",
                line_index + 1
            ));
            continue;
        }
        let category_name = format!("{}:{}", columns[0], columns[1]);
        if !names.insert(category_name.clone()) {
            errors.push(format!("duplicate surface name {category_name}"));
        }
        let category_role = format!("{}:{}", columns[0], columns[2]);
        if !roles.insert(category_role.clone()) {
            errors.push(format!("duplicate semantic role {category_role}"));
        }
        match rfcs.get(columns[3]) {
            Some(rfc) if rfc.status == "accepted" && rfc.implementation == "complete" => {}
            Some(_) => errors.push(format!(
                "surface {category_name} cites inactive {}",
                columns[3]
            )),
            None => errors.push(format!(
                "surface {category_name} cites missing {}",
                columns[3]
            )),
        }
        if columns[0] == "builtin" {
            builtin_names.insert(columns[1].to_owned());
        }
    }
    let Some(root) = path.parent().and_then(Path::parent) else {
        errors.push("cannot resolve repository root from surface.tsv".to_owned());
        return;
    };
    let mut implementation = String::new();
    for relative in [
        "selfhost/check.slim",
        "selfhost/codegen.slim",
        "selfhost/memory.slim",
    ] {
        match fs::read_to_string(root.join(relative)) {
            Ok(source) => implementation.push_str(&source),
            Err(error) => errors.push(format!("cannot read {relative}: {error}")),
        }
    }
    for builtin in builtin_names {
        if !implementation.contains(&format!("\"{builtin}\"")) {
            errors.push(format!(
                "surface built-in `{builtin}` has no named SLIM implementation"
            ));
        }
    }
}

fn check_semantic_ledger(
    path: &Path,
    display_name: &str,
    rfcs: &BTreeMap<String, Rfc>,
    errors: &mut Vec<String>,
) {
    let text = match fs::read_to_string(path) {
        Ok(text) => text,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", path.display()));
            return;
        }
    };
    let mut names = BTreeSet::new();
    let mut roles = BTreeSet::new();
    for (line_index, line) in text.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() != 4 {
            errors.push(format!(
                "{display_name}:{} must have four columns",
                line_index + 1
            ));
            continue;
        }
        let category_name = format!("{}:{}", columns[0], columns[1]);
        if !names.insert(category_name.clone()) {
            errors.push(format!("duplicate {display_name} name {category_name}"));
        }
        let category_role = format!("{}:{}", columns[0], columns[2]);
        if !roles.insert(category_role.clone()) {
            errors.push(format!("duplicate {display_name} role {category_role}"));
        }
        match rfcs.get(columns[3]) {
            Some(rfc) if rfc.status == "accepted" && rfc.implementation == "complete" => {}
            Some(_) => errors.push(format!(
                "{display_name} {category_name} cites inactive {}",
                columns[3]
            )),
            None => errors.push(format!(
                "{display_name} {category_name} cites missing {}",
                columns[3]
            )),
        }
    }
}

fn check_dependencies(path: &Path, errors: &mut Vec<String>) {
    let Ok(text) = fs::read_to_string(path) else {
        return;
    };
    let mut in_dependencies = false;
    for line in text.lines() {
        let trimmed = line.trim();
        if trimmed.starts_with('[') {
            in_dependencies = matches!(
                trimmed,
                "[dependencies]" | "[build-dependencies]" | "[dev-dependencies]"
            );
            continue;
        }
        if in_dependencies && !trimmed.is_empty() && !trimmed.starts_with('#') {
            errors.push(format!("unapproved Cargo dependency: {trimmed}"));
        }
    }
}

fn check_rust_safety(dir: &Path, errors: &mut Vec<String>) {
    let unsafe_block = ["un", "safe {"].concat();
    let unsafe_function = ["un", "safe fn"].concat();
    visit_rs_files(dir, &mut |path, text| {
        if text.contains(&unsafe_block) || text.contains(&unsafe_function) {
            errors.push(format!(
                "unsafe Rust requires a dedicated accepted rfc: {}",
                path.display()
            ));
        }
    });
}

fn check_rust_budget(root: &Path, errors: &mut Vec<String>) {
    let path = root.join("design/rust-budget.tsv");
    let text = match fs::read_to_string(&path) {
        Ok(text) => text,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", path.display()));
            return;
        }
    };
    let mut budget = BTreeMap::<String, (String, Option<usize>)>::new();
    for (line_index, line) in text.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() != 3 {
            errors.push(format!(
                "rust-budget.tsv:{} must have three tab-separated columns",
                line_index + 1
            ));
            continue;
        }
        let maximum = match columns[1] {
            "production" => match columns[2].parse::<usize>() {
                Ok(maximum) => Some(maximum),
                Err(_) => {
                    errors.push(format!(
                        "rust-budget.tsv:{} production maximum must be bytes",
                        line_index + 1
                    ));
                    continue;
                }
            },
            "infrastructure" if columns[2] == "-" => None,
            "infrastructure" => {
                errors.push(format!(
                    "rust-budget.tsv:{} infrastructure maximum must be -",
                    line_index + 1
                ));
                continue;
            }
            role => {
                errors.push(format!(
                    "rust-budget.tsv:{} unknown Rust role {role}",
                    line_index + 1
                ));
                continue;
            }
        };
        if budget
            .insert(columns[0].to_owned(), (columns[1].to_owned(), maximum))
            .is_some()
        {
            errors.push(format!("duplicate Rust budget path {}", columns[0]));
        }
    }

    let mut seen = BTreeSet::new();
    visit_rs_files(&root.join("src"), &mut |path, source| {
        let relative = path
            .strip_prefix(root)
            .unwrap_or(path)
            .to_string_lossy()
            .replace('\\', "/");
        seen.insert(relative.clone());
        match budget.get(&relative) {
            Some((role, Some(maximum))) if source.len() > *maximum => errors.push(format!(
                "production Rust budget exceeded for {relative}: {} > {maximum} bytes; implement capability in SLIM or accept a new architecture rfc",
                source.len()
            )),
            Some((role, None)) if role == "infrastructure" => {}
            Some(_) => {}
            None => errors.push(format!(
                "unclassified Rust source {relative}; add it to rust-budget.tsv"
            )),
        }
    });
    for missing in budget.keys().filter(|path| !seen.contains(*path)) {
        errors.push(format!("Rust budget path does not exist: {missing}"));
    }
}

fn check_ast_boundary(root: &Path, errors: &mut Vec<String>) {
    let rfc_path = root.join("design/rfcs/0103-canonical-ast-boundary.md");
    let rfc = fs::read_to_string(&rfc_path).unwrap_or_default();
    for required in [
        "# RFC-0103: Canonical AST module boundary",
        "Status: accepted",
    ] {
        if !contains_slim_pattern(&rfc, required) {
            errors.push(format!(
                "canonical AST boundary is missing accepted rfc evidence `{required}`"
            ));
        }
    }

    let syntax_path = root.join("selfhost/syntax.slim");
    let syntax = fs::read_to_string(&syntax_path).unwrap_or_default();
    for required in [
        "(fn ast_node_kind",
        "(fn ast_node_start",
        "(fn ast_node_end",
        "(fn ast_node_link",
        "(fn ast_node_text_is",
        "(fn ast_next",
        "(fn ast_module_items",
        "(fn ast_item_is_function",
        "(fn ast_function_body",
        "(fn ast_binding_type",
        "(fn ast_expression_is_call",
    ] {
        if !contains_slim_pattern(&syntax, required) {
            errors.push(format!(
                "canonical AST boundary is missing semantic accessor `{required}`"
            ));
        }
    }

    let forbidden = [
        "syntax/token_kind",
        "syntax/token_start",
        "syntax/token_end",
        "syntax/token_link",
        "syntax/token_equal",
        "syntax/set_token_link",
        "syntax/skip_form",
    ];
    let selfhost = root.join("selfhost");
    let Ok(entries) = fs::read_dir(&selfhost) else {
        errors.push("cannot inspect selfhost sources for AST boundary".to_owned());
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.extension().and_then(|value| value.to_str()) != Some("slim")
            || path.file_name().and_then(|value| value.to_str()) == Some("syntax.slim")
        {
            continue;
        }
        let Ok(source) = fs::read_to_string(&path) else {
            errors.push(format!(
                "cannot inspect {} for AST boundary",
                path.display()
            ));
            continue;
        };
        for operation in forbidden {
            if source.contains(operation) {
                errors.push(format!(
                    "{} bypasses the canonical AST boundary through `{operation}`",
                    path.display()
                ));
            }
        }
    }
}

fn check_indented_source(root: &Path, errors: &mut Vec<String>) {
    let rfc = fs::read_to_string(root.join("design/rfcs/0104-indented-canonical-source.md"))
        .unwrap_or_default();
    for required in [
        "# RFC-0104: Indented canonical source",
        "Status: accepted",
        "Primitive: none",
        "two ASCII spaces per level",
        "803,456",
        "856,167",
        "885,268",
        "1,015,050",
        "820,377",
    ] {
        if !rfc.contains(required) {
            errors.push(format!(
                "indented canonical source is missing rfc evidence `{required}`"
            ));
        }
    }

    let syntax = fs::read_to_string(root.join("selfhost/syntax.slim")).unwrap_or_default();
    for required in [
        "(record ProgramParse",
        "(fn lex_modern",
        "(fn indentation_jump",
        "(fn parse_program_result",
        "\"E0102\"",
        "\"E0103\"",
        "\"E0104\"",
        "\"E0105\"",
        "\"E0106\"",
        "\"E0107\"",
    ] {
        if !contains_slim_pattern(&syntax, required) {
            errors.push(format!(
                "indented parser is missing canonical capability `{required}`"
            ));
        }
    }
    for forbidden in ["fn lex(", "fn lex_modern_recursive", "parse_legacy_program"] {
        if syntax.contains(forbidden) {
            errors.push(format!(
                "indented parser retains alternate program frontend `{forbidden}`"
            ));
        }
    }

    let formatter = fs::read_to_string(root.join("selfhost/format.slim")).unwrap_or_default();
    for required in [
        "(fn emit_module",
        "(fn emit_function",
        "(fn emit_expression",
        "(call text/append_text output \"  \")",
    ] {
        if !contains_slim_pattern(&formatter, required) {
            errors.push(format!(
                "canonical formatter is missing indented capability `{required}`"
            ));
        }
    }

    let allowed_data_parsers = ["cache.slim", "edit.slim", "project.slim", "session.slim"];
    let selfhost = root.join("selfhost");
    let Ok(entries) = fs::read_dir(&selfhost) else {
        errors.push("cannot inspect non-program data parser scope".to_owned());
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.extension().and_then(|value| value.to_str()) != Some("slim")
            || path.file_name().and_then(|value| value.to_str()) == Some("syntax.slim")
        {
            continue;
        }
        let source = fs::read_to_string(&path).unwrap_or_default();
        let name = path
            .file_name()
            .and_then(|value| value.to_str())
            .unwrap_or("");
        if source.contains("syntax/lex_data") && !allowed_data_parsers.contains(&name) {
            errors.push(format!(
                "{} uses the non-program S-expression parser outside its tooling boundary",
                path.display()
            ));
        }
    }

    let manifest = fs::read_to_string(root.join("conformance/manifest.tsv")).unwrap_or_default();
    for required in [
        "tab-indentation\tcheck-fail",
        "\tE0103@",
        "odd-indentation\tcheck-fail",
        "\tE0104@",
        "skipped-indentation\tcheck-fail",
        "\tE0105@",
        "trailing-comma\tcheck-fail",
        "\tE0109@",
        "unterminated-string\tcheck-fail",
        "\tE0107@",
    ] {
        if !manifest.contains(required) {
            errors.push(format!(
                "indented syntax diagnostic fixture is missing `{required}`"
            ));
        }
    }

    let core = fs::read_to_string(root.join("docs/CORE.md")).unwrap_or_default();
    let status = fs::read_to_string(root.join("docs/STATUS.md")).unwrap_or_default();
    let llms_generator =
        fs::read_to_string(root.join("website/scripts/generate-content.mjs")).unwrap_or_default();
    for (name, source) in [
        ("docs/CORE.md", &core),
        ("docs/STATUS.md", &status),
        ("website/scripts/generate-content.mjs", &llms_generator),
    ] {
        if source.contains("canonical S-expression source")
            || source.contains("Canonical source uses one S-expression")
        {
            errors.push(format!("{name} retains the superseded program syntax"));
        }
    }
}

fn check_selfhost_architecture(root: &Path, errors: &mut Vec<String>) {
    let directory = root.join("selfhost");
    let project_path = directory.join("slim.project");
    let project = match fs::read_to_string(&project_path) {
        Ok(project) => project,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", project_path.display()));
            return;
        }
    };
    for (module, file) in [
        ("analysis", "analysis.slim"),
        ("cache", "cache.slim"),
        ("check", "check.slim"),
        ("codegen", "codegen.slim"),
        ("compiler", "slimc.slim"),
        ("driver", "driver.slim"),
        ("edit", "edit.slim"),
        ("effects", "effects.slim"),
        ("equivalence", "equivalence.slim"),
        ("format", "format.slim"),
        ("ir", "ir.slim"),
        ("memory", "memory.slim"),
        ("project", "project.slim"),
        ("proof", "proof.slim"),
        ("quality", "quality.slim"),
        ("query", "query.slim"),
        ("reduce", "reduce.slim"),
        ("scheduler", "scheduler.slim"),
        ("session", "session.slim"),
        ("syntax", "syntax.slim"),
        ("text", "text.slim"),
        ("typing", "typing.slim"),
        ("validate", "validate.slim"),
    ] {
        let path = directory.join(file);
        if !path.is_file() {
            errors.push(format!("self-host module `{module}` is missing {file}"));
        }
        let clause = format!("(module {module} \"{file}\"");
        if !project.contains(&clause) {
            errors.push(format!(
                "selfhost/slim.project does not explicitly declare `{module}` from `{file}`"
            ));
        }
    }
    for required in [
        "(module codegen \"codegen.slim\" (imports memory parallel ranges syntax text typing) (exports emit_program))",
        "(module memory \"memory.slim\" (imports effects ir syntax) (exports AllocationPlan DestructionPlan FunctionPlan Plan ValuePlan allocation_site_region analyze empty_plan function_plan_allocates type_storage_kind))",
        "(module project \"project.slim\" (imports check codegen format memory scheduler syntax text typing validate)",
        "(module typing \"typing.slim\" (imports ir memory syntax) (exports Checked Fact Issue TypeRef View analyze append_issue builtin_known empty_view fact_type linked_binding_declaration linked_binding_is_inout))",
        "(module validate \"validate.slim\" (imports syntax) (exports executable_shape_valid module_shape_valid module_shape_valid_from))",
    ] {
        if !contains_slim_pattern(&project, required) {
            errors.push(format!(
                "selfhost/slim.project is missing compiler architecture clause `{required}`"
            ));
        }
    }

    let check_path = directory.join("check.slim");
    let Ok(check) = fs::read_to_string(&check_path) else {
        return;
    };
    for (operation, count) in [
        ("io.read_file(", check.matches("io.read_file(").count()),
        (
            "syntax.parse_program_result(",
            check.matches("syntax.parse_program_result(").count(),
        ),
    ] {
        if count != 1 {
            errors.push(format!(
                "self-host checker must perform `{operation}` exactly once, found {count}"
            ));
        }
    }
    if !check.contains("Vec[ir.Declaration]") {
        errors.push("self-host checker does not consume structured declarations".to_owned());
    }

    let typing_path = directory.join("typing.slim");
    let Ok(typing) = fs::read_to_string(&typing_path) else {
        return;
    };
    for required in [
        "(record Fact ((type TypeRef)))",
        "(record Issue ((code Bytes) (start I64) (end I64) (blocks_inference Bool)))",
        "(record Checked ((status I64) (view View) (issues (Vec Issue)) (plan memory/Plan)))",
        "(fn initialize_facts",
        "(fn fact_type",
        "(call vec.set facts expr fact)",
        "(call fact_type facts expr)",
        "(fn append_issue",
        "(make Issue (code code) (start start) (end end) (blocks_inference true))",
        "(call syntax/ast_node_set_link tokens cursor definition)",
        "(call syntax/ast_node_set_link tokens expr case_form)",
        "(call syntax/ast_node_set_link tokens cursor case_cursor)",
        "(fn check_recursive_argument_identity",
        "(call append_issue \"E0350\" argument argument issues)",
    ] {
        if !contains_slim_pattern(&typing, required) {
            errors.push(format!(
                "self-host typed issue channel is missing interval capability `{required}`"
            ));
        }
    }
    if typing.contains("(record Issue ((code Bytes) (token I64)))") {
        errors.push("self-host typed issue channel regressed to point-only spans".to_owned());
    }
    for required in ["(fn report_issue", "(get issue start)", "(get issue end)"] {
        if !contains_slim_pattern(&check, required) {
            errors.push(format!(
                "self-host checker is missing interval diagnostic consumer `{required}`"
            ));
        }
    }
    for required in [
        "(call append_form_issue \"E0336\" incomplete tokens issues)",
        "(call append_form_issue \"E0335\" cursor tokens issues)",
        "(call append_token_issue \"E0314\" body issues)",
        "(call append_token_issue \"E0344\" body issues)",
        "(call append_form_issue \"E0343\" missing tokens issues)",
        "(call append_form_issue \"E0348\" temporary tokens issues)",
        "(call append_token_issue \"E0349\" duplicate issues)",
    ] {
        if !contains_slim_pattern(&check, required) {
            errors.push(format!(
                "self-host checker is missing finalized semantic issue `{required}`"
            ));
        }
    }
    for required in [
        "(record Issue ((code Bytes) (start I64) (end I64) (blocks_inference Bool)))",
        "(record Binding ((name I64) (type I64) (borrowed Bool) (moved Bool) (parent I64)))",
        "(fn append_ownership_issue",
        "(fn issues_block_inference",
        "(fn mark_named_move",
        "(fn check_owned_argument",
        "(fn append_inout_return_issue",
        "(call i64.mul type_mode base)",
        "(call i64.rem packed base)",
        "(call append_ownership_issue \"E0315\" argument issues)",
        "(call append_ownership_issue \"E0347\" argument issues)",
        "(call append_ownership_issue \"E0347\" result issues)",
    ] {
        if !contains_slim_pattern(&typing, required) {
            errors.push(format!(
                "self-host typed ownership diagnostics are missing `{required}`"
            ));
        }
    }
    for code in [
        "E0314", "E0315", "E0335", "E0336", "E0343", "E0344", "E0347", "E0348", "E0349", "E0350",
    ] {
        let direct = format!("(call report_diagnostic \"{code}\"");
        if check.contains(&direct) {
            errors.push(format!(
                "self-host checker directly prints finalized issue {code}"
            ));
        }
    }
    for superseded in [
        "(fn report_recur_arguments",
        "(fn report_recur_span",
        "(fn report_recur_items",
        "(fn check_path_recur",
        "(fn report_boolean_match_arms",
        "(fn report_boolean_match_span",
        "(fn report_boolean_match_items",
        "(fn ownership_type_is_copy",
        "(fn ownership_parameter_type",
        "(fn moved_has",
        "(fn report_owned_arguments",
        "(fn report_builtin_move",
        "(fn report_call_move",
        "(fn report_moves_span",
        "(fn report_inout_return",
        "(fn report_move_items",
        "(fn check_path_moves",
    ] {
        if check.contains(superseded) {
            errors.push(format!(
                "self-host checker retains superseded recursive-inout pipeline `{superseded}`"
            ));
        }
    }
    let project_manifest =
        fs::read_to_string(root.join("conformance/projects/manifest.tsv")).unwrap_or_default();
    if !project_manifest.contains(
        "project-recur-rebind\tcheck-fail\tconformance/projects/recur-rebind/slim.project\tparity\tE0350@app@98:103,E0350@app@105:109",
    ) {
        errors.push("recursive-inout project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-nonexhaustive\tcheck-fail\tconformance/projects/nonexhaustive/slim.project\tparity\tE0336@app@48:77",
    ) {
        errors.push("nonexhaustive project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-boolean-recovery\tcheck-fail\tconformance/projects/boolean-recovery/slim.project\tparity\tE0336@app@48:105,E0314@app@76:83,E0335@app@88:105,E0344@app@100:105",
    ) {
        errors.push("Boolean recovery project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-ownership\tcheck-fail\tconformance/projects/ownership/slim.project\tparity\tE0315@app@172:178,E0347@app@232:238,E0315@app@359:365,E0347@app@430:436",
    ) {
        errors.push("ownership project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-malformed-module\tcheck-fail\tconformance/projects/malformed-module/slim.project\tparity\tE0102@app@67:67\tproject:module-shape,project:diagnostics",
    ) {
        errors.push("malformed project module fixture is missing".to_owned());
    }

    let project_path = directory.join("project.slim");
    let Ok(project_source) = fs::read_to_string(&project_path) else {
        return;
    };
    for required in [
        "(fn report_manifest_rules",
        "(fn find_invalid_path",
        "(fn find_unknown_import",
        "(call report_manifest_rules",
        "(record LoadedModule",
        "(record Origin",
        "(record PreparedProject",
        "(fn load_project_modules",
        "(fn flatten_loaded_project",
        "(fn prepare_project_path",
        "(fn prepare_loaded_project",
        "(fn generate_prepared_project",
        "(fn report_project_issue",
        "(fn report_project_issues",
        "(fn report_loaded_project",
        "(fn report_private_type_leaks",
        "(call validate/module_shape_valid_from source module_tokens root end)",
    ] {
        if !contains_slim_pattern(&project_source, required) {
            errors.push(format!(
                "self-host project checker is missing manifest capability `{required}`"
            ));
        }
    }
    for required in ["(get issue start)", "(get issue end)"] {
        if !contains_slim_pattern(&project_source, required) {
            errors.push(format!(
                "self-host project checker is missing interval projection `{required}`"
            ));
        }
    }
    if !contains_slim_pattern(&project_source, "(get checked issues)") {
        errors.push("self-host project checker does not consume finalized issues".to_owned());
    }

    let syntax_path = directory.join("syntax.slim");
    let Ok(syntax) = fs::read_to_string(&syntax_path) else {
        return;
    };
    for required in [
        "(fn index_names_from",
        "(let length I64 (call vec.len tokens)",
        "(let exhausted Bool (call i64.ge cursor length)",
    ] {
        if !contains_slim_pattern(&syntax, required) {
            errors.push(format!(
                "self-host name indexing is missing its token bound `{required}`"
            ));
        }
    }

    let validate_path = directory.join("validate.slim");
    let Ok(validate) = fs::read_to_string(&validate_path) else {
        return;
    };
    for required in [
        "(fn module_shape_valid_from",
        "(let length I64 (call i64.sub end root)",
        "(call item_list_valid source tokens (call syntax/ast_module_items root) closing)",
        "(call module_shape_valid_from source tokens 0 end)",
    ] {
        if !contains_slim_pattern(&validate, required) {
            errors.push(format!(
                "self-host module validation is missing bounded-slice capability `{required}`"
            ));
        }
    }

    let codegen_path = directory.join("codegen.slim");
    let Ok(codegen) = fs::read_to_string(&codegen_path) else {
        return;
    };
    let bounded_record_rfc =
        fs::read_to_string(root.join("design/rfcs/0046-bounded-record-member-lookup.md"))
            .unwrap_or_default();
    for required in [
        "# RFC-0046: Bounded record member lookup",
        "Status: accepted",
    ] {
        if !contains_slim_pattern(&bounded_record_rfc, required) {
            errors.push(format!(
                "bounded record member lookup is missing accepted rfc evidence `{required}`"
            ));
        }
    }
    if codegen.contains("link_declaration_names") {
        errors.push("self-host code generation redundantly rebuilds declaration links".to_owned());
    }
    for required in [
        "(let variant_type I64 (call fact_type_index facts value)",
        "(let item I64 (call syntax/ast_node_link tokens record)",
        "(let variant_link I64 (call syntax/ast_node_link tokens variant)",
        "(let variant_link I64 (call syntax/ast_node_link tokens variant_type)",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!(
                "self-host code generation is missing checked type/member use `{required}`"
            ));
        }
    }
    if codegen.contains("(fn find_parameter_type") {
        errors.push("self-host code generation restored parameter-only type lookup".to_owned());
    }
    if codegen.contains("(fn find_record_item") {
        errors.push("self-host code generation restored record declaration scans".to_owned());
    }
    if codegen.contains("(fn find_variant_item") {
        errors.push("self-host code generation restored variant declaration scans".to_owned());
    }
    if codegen.contains("(fn find_variant_case") {
        errors.push("self-host code generation restored variant case scans".to_owned());
    }
    if codegen.contains("(fn find_record_field") {
        errors.push("self-host code generation restored record field scans".to_owned());
    }
    if codegen.contains("syntax/name_is_inout") {
        errors.push("self-host code generation restored parameter-mode scans".to_owned());
    }
    for required in [
        "(call typing/linked_binding_is_inout tokens name)",
        "(fn emit_binding_value",
        "(fn emit_binding_address",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!(
                "self-host code generation is missing linked binding-mode use `{required}`"
            ));
        }
    }
    let typing_path = directory.join("typing.slim");
    let Ok(typing) = fs::read_to_string(&typing_path) else {
        return;
    };
    for required in [
        "(fn linked_binding_is_inout",
        "(let type_scaled I64 (call i64.mul type_value 2)",
        "(let type_mode I64 (call i64.add type_scaled borrowed_value)",
        "(call i64.div type_mode 2)",
        "(call linked_binding_is_inout tokens result)",
    ] {
        if !contains_slim_pattern(&typing, required) {
            errors.push(format!(
                "self-host typing is missing linked binding-mode capability `{required}`"
            ));
        }
    }
    if typing.contains("syntax/name_is_inout source tokens params result") {
        errors.push("self-host typing restored borrowed-return parameter scans".to_owned());
    }
    for required in [
        "(let case_link I64 (call syntax/ast_node_link tokens expr)",
        "(let case_link I64 (call syntax/ast_node_link tokens cursor)",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!(
                "self-host code generation is missing bounded variant member query `{required}`"
            ));
        }
    }
    for required in [
        "(fn checked_record_field_link",
        "(call syntax/ast_node_link tokens field_form)",
        "(call checked_record_field_link source tokens cursor definition name_start name_end)",
    ] {
        if !contains_slim_pattern(&codegen, required) {
            errors.push(format!(
                "self-host code generation is missing bounded record member query `{required}`"
            ));
        }
    }
    for required in [
        "record-wide\trun\tconformance/pass/record_wide.slim",
        "variant-wide\trun\tconformance/pass/variant_wide.slim",
        "backend:aggregate-links",
    ] {
        let manifest =
            fs::read_to_string(root.join("conformance/manifest.tsv")).unwrap_or_default();
        if !contains_slim_pattern(&manifest, required) {
            errors.push(format!(
                "aggregate-link conformance evidence is missing `{required}`"
            ));
        }
    }
    for superseded in [
        "(fn report_private_modules",
        "(fn append_project_modules",
        "(fn flatten_project",
        "(fn check_flattened_project",
        "(fn generate_project_path",
    ] {
        if project_source.contains(superseded) {
            errors.push(format!(
                "self-host project checker retains superseded pipeline `{superseded}`"
            ));
        }
    }

    let query_path = directory.join("query.slim");
    let Ok(query) = fs::read_to_string(&query_path) else {
        return;
    };
    for required in [
        "(record Snapshot",
        "(record Work",
        "(record Dependency",
        "(record State",
        "(fn build_dependencies",
        "(fn build_state",
        "(fn build_snapshots",
        "(fn measure_update",
    ] {
        if !contains_slim_pattern(&query, required) {
            errors.push(format!(
                "self-host query engine is missing typed capability `{required}`"
            ));
        }
    }

    let cache_path = directory.join("cache.slim");
    let Ok(cache) = fs::read_to_string(&cache_path) else {
        return;
    };
    for required in [
        "(record Key",
        "(record Number",
        "(record Probe",
        "(fn read_bounded_u64",
        "(fn weighted_checksum",
        "(fn project_key",
        "(fn encode",
        "(fn probe",
        "(call project/prepare_project_path",
        "(call project/generate_prepared_project",
    ] {
        if !contains_slim_pattern(&cache, required) {
            errors.push(format!(
                "self-host cache is missing bounded capability `{required}`"
            ));
        }
    }

    let session_path = directory.join("session.slim");
    let Ok(session) = fs::read_to_string(&session_path) else {
        return;
    };
    for required in ["(fn state_for_path", "(fn run_recovery"] {
        if !contains_slim_pattern(&session, required) {
            errors.push(format!(
                "self-host session is missing transactional capability `{required}`"
            ));
        }
    }

    let scheduler_path = directory.join("scheduler.slim");
    let Ok(scheduler) = fs::read_to_string(&scheduler_path) else {
        return;
    };
    for required in [
        "(record Task",
        "(record Batch",
        "(record Schedule",
        "(fn collect_ready",
        "(fn append_batches",
        "(fn schedule_layers",
        "(fn bounded_workers",
        "(fn plan",
    ] {
        if !contains_slim_pattern(&scheduler, required) {
            errors.push(format!(
                "self-host scheduler is missing bounded capability `{required}`"
            ));
        }
    }
}

fn check_memory_architecture(root: &Path, errors: &mut Vec<String>) {
    let sources = [
        root.join("runtime/slim_rt.h"),
        root.join("runtime/slim_rt.c"),
        root.join("selfhost/check.slim"),
        root.join("selfhost/effects.slim"),
        root.join("selfhost/memory.slim"),
        root.join("selfhost/codegen.slim"),
        root.join("selfhost/project.slim"),
        root.join("selfhost/slimc.slim"),
    ];
    let joined = sources
        .iter()
        .filter_map(|path| fs::read_to_string(path).ok())
        .collect::<String>();
    for required in [
        "SlimRegion",
        "SlimAllocStatus",
        "(record AllocationPlan",
        "(record DestructionPlan",
        "(record FunctionPlan ((function I64) (return_storage I64) (local_region Bool) (recursive Bool)",
        "(fn empty_plan",
        "(fn function_uses_local_region",
        "(fn function_plan_allocates",
        "(fn allocation_site_region_range",
        "(fn allocation_site_region",
        "(recur allocations site low middle)",
        "(recur allocations site next high)",
        "(fn span_has_recur",
        "(call span_has_recur source tokens body body_end)",
        "(fn call_requires",
        "(call effects/call_requires source tokens callee 1)",
        "(call syntax/ast_node_link tokens type_index)",
        "(let view typing/View (call typing/analyze input tokens declarations) (let plan memory/Plan (call memory/analyze input tokens declarations)",
        "(make typing/Checked (status status) (view view) (issues issues) (plan plan))",
        "(fn emit_program ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact)) (plan memory/Plan)",
        "(fn fact_type_index",
        "(call typing/fact_type facts expr)",
        "(call fact_type_index facts argument)",
        "(fn emit_match_value_binding ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact)) (inout allocations (Vec memory/AllocationPlan)) (module_items I64) (params I64) (value I64) (inout output (Vec U8)) (inout range-facts (Vec ranges/Fact)))",
        "(let type_index I64 (call fact_type_index facts value) (let boolean_match Bool (call i64.eq type_index -2)",
        "(fn emit_variant_match ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact))",
        "(let variant_type I64 (call fact_type_index facts value)",
        "(call checked_record_field_link source tokens cursor definition name_start name_end)",
        "(fn emit_case_bindings ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact)) (inout allocations (Vec memory/AllocationPlan)) (module_items I64) (params I64) (cursor I64) (payload_type I64) (inout output (Vec U8)) (inout range-facts (Vec ranges/Fact))) Unit (effects alloc partial) (let kind I64 (call syntax/ast_node_kind tokens cursor) (let done Bool (call i64.eq kind 1) (match done (true unit) (false (let type_index I64 (call fact_type_index facts cursor)",
        "(fn emit_expr_full ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact))",
        "(get view facts)",
        "(get prepared facts)",
        "(call codegen/emit_program input tokens facts plan output)",
        "(call codegen/emit_program source tokens facts plan output)",
        "(get function_plan function)",
        "(get function_plan local_region)",
        "(get function_plan recursive)",
        "(get function_plan allocations)",
        "(call memory/function_plan_allocates function_plan)",
        "(call memory/allocation_site_region allocations call_form)",
        "slim_region_destroy(&slim_function_region)",
        "slim_allocation_failed",
    ] {
        if !contains_slim_pattern(&joined, required) {
            errors.push(format!(
                "Core 0.4 memory architecture is missing `{required}`"
            ));
        }
    }
    for forbidden in [
        "slim_allocations",
        "slim_find_allocation",
        "slim_rt_trap(\"out of memory\")",
        "(fn find_type_item",
        "(fn effect_list_has_alloc",
        "(fn params_have_alloc_effect",
        "(fn builtin_requires_effect",
        "(fn call_requires_effect",
        "(fn builtin_call_allocates",
        "(fn call_allocates",
        "(fn builtin_argument_type",
        "(fn parameter_type_index",
        "(fn linked_source_type",
        "(call effects/params_have source tokens callee_params 1)",
        "(call memory/function_has_alloc_effect source tokens item)",
        "(call memory/function_uses_local_region source tokens item)",
        "(fn contains_atom",
        "(call contains_atom source tokens body body_end \"recur\")",
    ] {
        if joined.contains(forbidden) {
            errors.push(format!(
                "Core 0.4 memory architecture retains forbidden `{forbidden}`"
            ));
        }
    }
    let manifest = fs::read_to_string(root.join("conformance/manifest.tsv")).unwrap_or_default();
    if !manifest
        .contains("allocation-user-failure\tallocation-fail\tconformance/pass/lifetimes.slim")
    {
        errors.push("retained user-allocation failure fixture is missing".to_owned());
    }
    if !manifest.contains("typed-temporaries\trun\tconformance/pass/typed_temporaries.slim\tparity")
    {
        errors.push("typed-fact temporary conformance fixture is missing".to_owned());
    }
    if !manifest.contains(
        "planned-allocation-calls\trun\tconformance/pass/planned_allocation_calls.slim\tparity",
    ) {
        errors.push("retained allocation-call conformance fixture is missing".to_owned());
    }
    if !manifest.contains(
        "computed-variant-match\trun\tconformance/pass/computed_variant_match.slim\tparity",
    ) {
        errors.push("typed computed-match conformance fixture is missing".to_owned());
    }
}

fn check_direct_reduction(root: &Path, errors: &mut Vec<String>) {
    for required in [
        "design/rfcs/0028-direct-typed-reduction.md",
        "docs/REDUCTION.md",
        "selfhost/analysis.slim",
        "selfhost/reduce.slim",
        "conformance/pass/reduction.slim",
        "conformance/tool/reduction.expected.slim",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!(
                "Core 1A direct-reduction artifact is missing {required}"
            ));
        }
    }

    let rfc = fs::read_to_string(root.join("design/rfcs/0028-direct-typed-reduction.md"))
        .unwrap_or_default();
    for required in ["Status: accepted", "Primitive: none"] {
        if !contains_slim_pattern(&rfc, required) {
            errors.push(format!(
                "Core 1A direct reduction requires RFC-0028 field `{required}`"
            ));
        }
    }

    let reduction = fs::read_to_string(root.join("selfhost/reduce.slim")).unwrap_or_default();
    for required in [
        "(fn clone_reduced_node",
        "(call format/emit_module atoms reduced_tokens output)",
        "(fn normalize_from",
        "(fn normalize",
        "(fn emit_normal_form",
        "(call normalize_from canonical canonical 7)",
    ] {
        if !contains_slim_pattern(&reduction, required) {
            errors.push(format!(
                "Core 1A reducer is missing direct-tree capability or exact pass bound `{required}`"
            ));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    for required in [
        "(record BindingFact",
        "(fn find_binding",
        "(fn record_use",
        "(fn dependency_count",
        "(fact-limit 64)",
    ] {
        if !contains_slim_pattern(&analysis, required) {
            errors.push(format!(
                "Core 1A semantic analysis is missing bounded fact `{required}`"
            ));
        }
    }

    let compiler = fs::read_to_string(root.join("selfhost/slimc.slim")).unwrap_or_default();
    for command in ["\"reduce\"", "\"analyze\""] {
        if !compiler.contains(command) {
            errors.push(format!(
                "self-hosted compiler does not expose Core 1A command {command}"
            ));
        }
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    for forbidden in ["reduce/", "analysis/"] {
        if codegen.contains(forbidden) {
            errors.push(format!(
                "optional Core 1A tooling leaked into ordinary C generation through `{forbidden}`"
            ));
        }
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0028") {
        errors.push("RFC-0028 has Primitive: none and must not add Core surface".to_owned());
    }

    for directory in ["selfhost", "conformance", "examples", "docs", "design"] {
        visit_files(&root.join(directory), &mut |path| {
            if matches!(
                path.extension().and_then(|extension| extension.to_str()),
                Some("sil" | "slir")
            ) {
                errors.push(format!(
                    "separately parsed IR files are forbidden by RFC-0028: {}",
                    path.display()
                ));
            }
        });
    }
}

fn check_bounded_program_evidence(root: &Path, errors: &mut Vec<String>) {
    for required in [
        "design/rfcs/0029-bounded-program-evidence.md",
        "docs/QUALITY.md",
        "selfhost/edit.slim",
        "selfhost/equivalence.slim",
        "selfhost/proof.slim",
        "selfhost/quality.slim",
        "conformance/evidence/equivalent-left.slim",
        "conformance/evidence/equivalent-right.slim",
        "conformance/evidence/different.slim",
        "conformance/evidence/unsupported.slim",
        "conformance/evidence/quality.slim",
        "conformance/evidence/edit.patch",
        "conformance/evidence/edit-malformed.patch",
        "conformance/fail/tool_patch.slim",
        "benchmarks/agent/manifest.tsv",
    ] {
        if !root.join(required).is_file() {
            errors.push(format!("Core 1B evidence artifact is missing {required}"));
        }
    }

    let rfc = fs::read_to_string(root.join("design/rfcs/0029-bounded-program-evidence.md"))
        .unwrap_or_default();
    for required in ["Status: accepted", "Primitive: none"] {
        if !contains_slim_pattern(&rfc, required) {
            errors.push(format!("Core 1B requires RFC-0029 field `{required}`"));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    for required in [
        "(analysis 7",
        "(call quality/emit_module_facts",
        "(ownership-pressure",
        "(max-live-owned ",
        "(scope-end ",
    ] {
        if !contains_slim_pattern(&analysis, required) {
            errors.push(format!("Core 1B analysis is missing `{required}`"));
        }
    }

    let quality = fs::read_to_string(root.join("selfhost/quality.slim")).unwrap_or_default();
    for required in [
        "(record Metrics",
        "(state-model ",
        "(guarantee unknown)",
        "(cardinality (pow2 ",
        "(totality (guarantee",
        "\"rewrite-sites\"",
    ] {
        if !contains_slim_pattern(&quality, required) {
            errors.push(format!("Core 1B quality evidence is missing `{required}`"));
        }
    }

    let proof = fs::read_to_string(root.join("selfhost/proof.slim")).unwrap_or_default();
    for required in [
        "(reduction-proof 2",
        "(pass-limit 8)",
        "(site-limit 64)",
        "(call reduce/reduction_kind",
        "(call reduce/normalize original)",
        "(call reduce/canonicalize candidate)",
    ] {
        if !contains_slim_pattern(&proof, required) {
            errors.push(format!(
                "Core 1B reduction evidence is missing `{required}`"
            ));
        }
    }

    let equivalence =
        fs::read_to_string(root.join("selfhost/equivalence.slim")).unwrap_or_default();
    for required in [
        "(call i64.lt count 8)",
        "(call i64.le left-size 256)",
        "(call i64.le right-size 256)",
        "(status equivalent)",
        "(status different)",
        "(status unknown)",
        "(counterexample (inputs",
    ] {
        if !contains_slim_pattern(&equivalence, required) {
            errors.push(format!(
                "Core 1B equivalence checker is missing `{required}`"
            ));
        }
    }

    let edit = fs::read_to_string(root.join("selfhost/edit.slim")).unwrap_or_default();
    for required in [
        "\"slim-edit\"",
        "(call i64.le replacement_size 64)",
        "(call clone_candidate source tokens 0 target",
        "(call syntax/index_ast_boundaries candidate)",
        "(call format/emit_module atoms candidate output)",
    ] {
        if !contains_slim_pattern(&edit, required) {
            errors.push(format!("Core 1B structural editor is missing `{required}`"));
        }
    }

    let compiler = fs::read_to_string(root.join("selfhost/slimc.slim")).unwrap_or_default();
    for command in [
        "\"prove-reduction\"",
        "\"verify-reduction\"",
        "\"equivalent\"",
        "\"edit\"",
    ] {
        if !compiler.contains(command) {
            errors.push(format!(
                "self-hosted compiler does not expose Core 1B command {command}"
            ));
        }
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    for forbidden in ["quality/", "proof/", "equivalence/", "edit/"] {
        if codegen.contains(forbidden) {
            errors.push(format!(
                "optional Core 1B tooling leaked into ordinary C generation through `{forbidden}`"
            ));
        }
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("RFC-0029") {
        errors.push("RFC-0029 has Primitive: none and must not add Core surface".to_owned());
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "fn run_agent()",
        "broken_model_token_proxy",
        "fn neutral_lexical_tokens",
        "fn changed_span",
        "fn agent_manifest",
    ] {
        if !contains_slim_pattern(&benchmark, required) {
            errors.push(format!("Core 1B agent benchmark is missing `{required}`"));
        }
    }
    let verification = fs::read_to_string(root.join("scripts/verify.sh")).unwrap_or_default();
    if !verification.contains("slim-bench -- agent") {
        errors.push("Core 1B agent benchmark is absent from the release gate".to_owned());
    }
}

fn visit_rs_files(dir: &Path, action: &mut impl FnMut(&Path, &str)) {
    let Ok(entries) = fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            visit_rs_files(&path, action);
        } else if path.extension().and_then(|value| value.to_str()) == Some("rs")
            && let Ok(text) = fs::read_to_string(&path)
        {
            action(&path, &text);
        }
    }
}

fn visit_files(dir: &Path, action: &mut impl FnMut(&Path)) {
    let Ok(entries) = fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            visit_files(&path, action);
        } else {
            action(&path);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn repository_passes_governance() {
        let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
        let errors = check_repository(&root);
        assert!(errors.is_empty(), "{}", errors.join("\n"));
    }

    #[test]
    fn production_rust_growth_fails_governance() {
        let root = std::env::temp_dir().join(format!("slim-govern-budget-{}", std::process::id()));
        let _ = fs::remove_dir_all(&root);
        fs::create_dir_all(root.join("src")).unwrap();
        fs::create_dir_all(root.join("design")).unwrap();
        fs::write(root.join("src/feature.rs"), "fn feature() {}\n").unwrap();
        fs::write(
            root.join("design/rust-budget.tsv"),
            "src/feature.rs\tproduction\t1\n",
        )
        .unwrap();
        let mut errors = Vec::new();
        check_rust_budget(&root, &mut errors);
        assert!(
            errors
                .iter()
                .any(|error| error.contains("production Rust budget exceeded"))
        );
        fs::remove_dir_all(root).unwrap();
    }
}
