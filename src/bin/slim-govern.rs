use std::collections::{BTreeMap, BTreeSet};
use std::env;
use std::fs;
use std::path::{Path, PathBuf};

const REQUIRED_HEADINGS: [&str; 5] = [
    "## Need",
    "## Alternatives",
    "## Costs",
    "## Evidence",
    "## Removal",
];

#[derive(Debug)]
struct Decision {
    id: String,
    status: String,
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

    let decisions = load_decisions(&root.join("design/decisions"), &mut errors);
    check_decisions(&decisions, &mut errors);
    check_surface(&root.join("design/surface.tsv"), &decisions, &mut errors);
    check_semantic_ledger(
        &root.join("design/project-semantics.tsv"),
        "project-semantics.tsv",
        &decisions,
        &mut errors,
    );
    check_conformance_coverage(root, &mut errors);
    check_dependencies(&root.join("Cargo.toml"), &mut errors);
    check_rust_safety(&root.join("src"), &mut errors);
    check_rust_budget(root, &mut errors);
    check_toolchain_cutover(root, &mut errors);
    check_selfhost_architecture(root, &mut errors);
    check_core_1d_acceptance(root, &decisions, &mut errors);
    check_runtime_fast_paths(root, &decisions, &mut errors);
    check_allocation_free_region_elision(root, &decisions, &mut errors);
    check_core_1e_acceptance(root, &decisions, &mut errors);
    check_parallelism_evidence(root, &decisions, &mut errors);
    check_integer_proof_evidence(root, &decisions, &mut errors);
    check_parallelism_application_baseline(root, &decisions, &mut errors);
    check_memory_architecture(root, &mut errors);
    check_direct_reduction(root, &mut errors);
    check_bounded_program_evidence(root, &mut errors);
    check_performance_architecture(root, &decisions, &mut errors);
    errors
}

fn check_parallelism_evidence(
    root: &Path,
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    match decisions.get("D0062") {
        Some(decision)
            if decision.status == "accepted"
                && decision.kind == "architecture"
                && decision.primitive == "none"
                && decision.score >= 60 => {}
        Some(_) => errors.push(
            "Core 1F evidence requires accepted primitive-free D0062 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("Core 1F parallelism evidence decision D0062 is missing".to_owned()),
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
        if !parallel.contains(required) {
            errors.push(format!(
                "Core 1F bounded parallelism evidence is missing `{required}`"
            ));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    for required in [
        "(analysis 4",
        "(inout typed_facts (Vec typing/Fact))",
        "(call parallel/emit_module_facts source tokens typed_facts range_facts output)",
    ] {
        if !analysis.contains(required) {
            errors.push(format!(
                "Core 1F analysis does not consume the checked view through `{required}`"
            ));
        }
    }

    let compiler = fs::read_to_string(root.join("selfhost/slimc.slim")).unwrap_or_default();
    if !compiler.contains("(call check/check_source input tokens)")
        || !compiler.contains("(call analysis/emit_module input tokens typed_facts output)")
        || !compiler.contains("(fn analyze_project_path")
        || !compiler.contains("(call project/prepare_project_path path)")
    {
        errors.push(
            "source and project analysis must consume normal checked artifacts without a second semantic path"
                .to_owned(),
        );
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("D0062") {
        errors.push("D0062 has Primitive: none and must not add Core language surface".to_owned());
    }

    let budgets =
        fs::read_to_string(root.join("benchmarks/performance-budgets.tsv")).unwrap_or_default();
    if !budgets.contains("analysis-exponent\tgenerated-declarations\t1.25\texponent\tD0062") {
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
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    match decisions.get("D0063") {
        Some(decision)
            if decision.status == "accepted"
                && decision.kind == "architecture"
                && decision.primitive == "none"
                && decision.score >= 60 => {}
        Some(_) => errors.push(
            "integer proof evidence requires accepted primitive-free D0063 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("integer proof evidence decision D0063 is missing".to_owned()),
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
        if !ranges.contains(required) {
            errors.push(format!(
                "bounded integer proof implementation is missing `{required}`"
            ));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    for required in [
        "(analysis 4",
        "(call ranges/analyze source tokens typed_facts)",
        "(call ranges/emit-module-facts source tokens range_view output)",
    ] {
        if !analysis.contains(required) {
            errors.push(format!("analysis version 4 is missing `{required}`"));
        }
    }

    let quality = fs::read_to_string(root.join("selfhost/quality.slim")).unwrap_or_default();
    let parallel = fs::read_to_string(root.join("selfhost/parallel.slim")).unwrap_or_default();
    if !quality.contains("(call ranges/fact-total range_facts body)")
        || !parallel.contains("(call ranges/fact-total range_facts expr)")
    {
        errors.push(
            "quality and parallelism must consume the same exact-node integer totality facts"
                .to_owned(),
        );
    }

    let surface = fs::read_to_string(root.join("design/surface.tsv")).unwrap_or_default();
    if surface.contains("D0063") {
        errors.push("D0063 has Primitive: none and must not add Core language surface".to_owned());
    }

    let e2e = fs::read_to_string(root.join("tests/e2e.rs")).unwrap_or_default();
    for required in [
        "integer_ranges_prove_guarded_arithmetic_and_preserve_unknowns",
        "integer_range_refinement_limit_is_explicit_and_deterministic",
        "integer_checked_site_report_limit_is_explicit_and_deterministic",
        "(checked-site-count 65) (guarantee bounded)",
        "zero-divisor (guarantee exact) (status unavailable)",
        "domain-limit (guarantee exact) (status unavailable)",
    ] {
        if !e2e.contains(required) {
            errors.push(format!("integer proof evidence is missing `{required}`"));
        }
    }
}

fn check_parallelism_application_baseline(
    root: &Path,
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    match decisions.get("D0064") {
        Some(decision)
            if decision.status == "accepted"
                && decision.kind == "architecture"
                && decision.primitive == "none"
                && decision.score >= 60 => {}
        Some(_) => errors.push(
            "parallelism application baseline requires accepted primitive-free D0064 scoring at least 60"
                .to_owned(),
        ),
        None => errors.push("parallelism application baseline decision D0064 is missing".to_owned()),
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
        if !benchmark.contains(required) {
            errors.push(format!(
                "durable parallelism benchmark is missing `{required}`"
            ));
        }
    }

    let baseline =
        fs::read_to_string(root.join("benchmarks/parallelism-baseline.tsv")).unwrap_or_default();
    if !baseline.contains("# schema=1")
        || baseline
            .lines()
            .filter(|line| !line.starts_with('#') && !line.is_empty())
            .count()
            != 12
    {
        errors.push(
            "parallelism baseline must retain schema 1 and all twelve applications".to_owned(),
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
    if surface.contains("D0064") {
        errors.push("D0064 has Primitive: none and must not add Core language surface".to_owned());
    }
}

fn check_core_1e_acceptance(
    root: &Path,
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    match decisions.get("D0061") {
        Some(decision) if decision.status == "accepted" && decision.score >= 60 => {}
        Some(_) => {
            errors.push("Core 1E acceptance requires accepted D0061 scoring at least 60".to_owned())
        }
        None => errors.push("Core 1E acceptance decision D0061 is missing".to_owned()),
    }

    let roadmap = fs::read_to_string(root.join("ROADMAP.md")).unwrap_or_default();
    for required in [
        "Status: Core 1E safety-preserving native efficiency complete",
        "### Core 1E: safety-preserving native efficiency\n\nStatus: complete",
        "D0061 accepts Core 1E",
    ] {
        if !roadmap.contains(required) {
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
        "native-runtime-ratio\tsieve\t2.50\tslim-over-c\tD0061",
        "native-runtime-ratio\tmatrix\t2.50\tslim-over-c\tD0061",
        "native-runtime-ratio\tbytefreq\t2.50\tslim-over-c\tD0061",
        "native-runtime-ratio\trecords\t2.50\tslim-over-c\tD0061",
        "native-runtime-ratio\tvariants\t1.75\tslim-over-c\tD0061",
    ] {
        if !budgets.contains(required) {
            errors.push(format!("Core 1E tightened budget is missing `{required}`"));
        }
    }
}

fn check_allocation_free_region_elision(
    root: &Path,
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    match decisions.get("D0060") {
        Some(decision) if decision.status == "accepted" && decision.score >= 60 => {}
        Some(_) => errors.push(
            "allocation-free region elision requires accepted D0060 scoring at least 60".to_owned(),
        ),
        None => errors.push("allocation-free region decision D0060 is missing".to_owned()),
    }

    let codegen = fs::read_to_string(root.join("selfhost/codegen.slim")).unwrap_or_default();
    for required in [
        "(let allocation_effect Bool (call memory/function_plan_allocates function_plan)",
        "(let uses_child_region Bool (call bool.and local_region allocation_effect)",
        "(let region Unit (match uses_child_region",
        "(let destroyed Unit (match uses_child_region",
    ] {
        if !codegen.contains(required) {
            errors.push(format!(
                "allocation-free region elision is missing `{required}`"
            ));
        }
    }
    if codegen.contains("(let region Unit (match local_region")
        || codegen.contains("(let destroyed Unit (match local_region")
    {
        errors.push("code generation restored unconditional empty child regions".to_owned());
    }
}

fn check_runtime_fast_paths(
    root: &Path,
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    match decisions.get("D0059") {
        Some(decision) if decision.status == "accepted" && decision.score >= 60 => {}
        Some(_) => errors.push(
            "checked runtime fast paths require accepted D0059 scoring at least 60".to_owned(),
        ),
        None => errors.push("checked runtime fast-path decision D0059 is missing".to_owned()),
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
        if !header.contains(required) {
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
}

fn check_core_1d_acceptance(
    root: &Path,
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    match decisions.get("D0058") {
        Some(decision) if decision.status == "accepted" && decision.score >= 60 => {}
        Some(_) => {
            errors.push("Core 1D acceptance requires accepted D0058 scoring at least 60".to_owned())
        }
        None => errors.push("Core 1D acceptance decision D0058 is missing".to_owned()),
    }

    let roadmap = fs::read_to_string(root.join("ROADMAP.md")).unwrap_or_default();
    for required in [
        "### Core 1D: complete typed compiler view\n\nStatus: complete",
        "Core 1D is accepted by D0058.",
    ] {
        if !roadmap.contains(required) {
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
    decisions: &BTreeMap<String, Decision>,
    errors: &mut Vec<String>,
) {
    for relative in [
        "docs/PERFORMANCE.md",
        "design/decisions/D0030-durable-performance-evidence.md",
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
            match decisions.get(columns[4]) {
                Some(decision) if decision.status == "accepted" && decision.score >= 60 => {}
                Some(_) => errors.push(format!(
                    "performance budget {key} requires an accepted decision scoring at least 60"
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
            if columns.len() != 2 {
                errors.push(format!(
                    "challenges/manifest.tsv:{} must have two columns",
                    line_index + 1
                ));
                continue;
            }
            let challenge = columns[0];
            challenge_features.extend(columns[1].split(',').map(str::to_owned));
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
        if !benchmark.contains(required) {
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

    let decision =
        fs::read_to_string(root.join("design/decisions/D0027-portable-c-bootstrap-seed.md"))
            .unwrap_or_default();
    if !decision.contains("Status: accepted") {
        errors.push("toolchain cutover requires accepted decision D0027".to_owned());
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

fn load_decisions(dir: &Path, errors: &mut Vec<String>) -> BTreeMap<String, Decision> {
    let mut decisions = BTreeMap::new();
    let entries = match fs::read_dir(dir) {
        Ok(entries) => entries,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", dir.display()));
            return decisions;
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
        let Some(id) = file_name.split('-').next() else {
            continue;
        };
        if id.len() != 5 || !id.starts_with('D') || !id[1..].chars().all(|ch| ch.is_ascii_digit()) {
            errors.push(format!("invalid decision filename {file_name}"));
            continue;
        }

        let text = match fs::read_to_string(&path) {
            Ok(text) => text,
            Err(error) => {
                errors.push(format!("cannot read {}: {error}", path.display()));
                continue;
            }
        };
        for heading in REQUIRED_HEADINGS {
            if !text.lines().any(|line| line.trim() == heading) {
                errors.push(format!("{file_name} is missing {heading}"));
            }
        }

        let fields = parse_fields(&text);
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

        if decisions
            .insert(
                id.to_owned(),
                Decision {
                    id: id.to_owned(),
                    status,
                    kind,
                    primitive,
                    ratings,
                    score,
                },
            )
            .is_some()
        {
            errors.push(format!("duplicate decision id {id}"));
        }
    }
    decisions
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

fn check_decisions(decisions: &BTreeMap<String, Decision>, errors: &mut Vec<String>) {
    let mut primitives = BTreeSet::new();
    for decision in decisions.values() {
        let weights = [20, 20, 20, 20, 15, 5];
        let weighted: i32 = decision
            .ratings
            .iter()
            .zip(weights)
            .map(|(rating, weight)| rating * weight)
            .sum();
        if weighted % 2 != 0 {
            errors.push(format!(
                "{} score is fractional; choose ratings whose weighted sum is even",
                decision.id
            ));
        }
        let calculated = weighted / 2;
        if decision.score != calculated {
            errors.push(format!(
                "{} declares score {} but calculated score is {}",
                decision.id, decision.score, calculated
            ));
        }
        if decision.status == "accepted" && decision.kind == "language" {
            if decision.score < 40 {
                errors.push(format!(
                    "{} accepted language score is below 40",
                    decision.id
                ));
            }
            if decision.ratings[..4].iter().any(|rating| *rating < 0) {
                errors.push(format!(
                    "{} has a negative hard-gate rating in safety/compile/runtime/minimal",
                    decision.id
                ));
            }
            if !decision.ratings.contains(&2) {
                errors.push(format!("{} has no primary +2 benefit", decision.id));
            }
        }
        if decision.primitive != "none" && !primitives.insert(decision.primitive.clone()) {
            errors.push(format!(
                "{} duplicates accepted primitive {}",
                decision.id, decision.primitive
            ));
        }
    }
}

fn check_surface(path: &Path, decisions: &BTreeMap<String, Decision>, errors: &mut Vec<String>) {
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
        match decisions.get(columns[3]) {
            Some(decision) if decision.status == "accepted" => {}
            Some(_) => errors.push(format!(
                "surface {category_name} cites unaccepted {}",
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
    decisions: &BTreeMap<String, Decision>,
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
        match decisions.get(columns[3]) {
            Some(decision) if decision.status == "accepted" => {}
            Some(_) => errors.push(format!(
                "{display_name} {category_name} cites unaccepted {}",
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
                "unsafe Rust requires a dedicated accepted decision: {}",
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
                "production Rust budget exceeded for {relative}: {} > {maximum} bytes; implement capability in SLIM or accept a new architecture decision",
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
        "(module codegen \"codegen.slim\" (imports memory syntax text typing) (exports emit_program))",
        "(module memory \"memory.slim\" (imports effects ir syntax) (exports AllocationPlan DestructionPlan FunctionPlan Plan ValuePlan allocation_site_region analyze empty_plan function_plan_allocates type_storage_kind))",
        "(module project \"project.slim\" (imports check codegen memory scheduler syntax text typing validate)",
        "(module typing \"typing.slim\" (imports ir memory syntax) (exports Checked Fact Issue TypeRef View analyze append_issue builtin_known empty_view fact_type linked_binding_declaration linked_binding_is_inout))",
        "(module validate \"validate.slim\" (imports syntax) (exports executable_shape_valid module_shape_valid module_shape_valid_from))",
    ] {
        if !project.contains(required) {
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
        (
            "(call io.read-file",
            check.matches("(call io.read-file").count(),
        ),
        (
            "(call syntax/lex",
            check.matches("(call syntax/lex").count(),
        ),
    ] {
        if count != 1 {
            errors.push(format!(
                "self-host checker must perform `{operation}` exactly once, found {count}"
            ));
        }
    }
    if !check.contains("(Vec ir/Declaration)") {
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
        "(call syntax/set_token_link tokens cursor definition)",
        "(call syntax/set_token_link tokens expr case_form)",
        "(call syntax/set_token_link tokens cursor case_cursor)",
        "(fn check_recursive_argument_identity",
        "(call append_issue \"E0350\" argument argument issues)",
    ] {
        if !typing.contains(required) {
            errors.push(format!(
                "self-host typed issue channel is missing interval capability `{required}`"
            ));
        }
    }
    if typing.contains("(record Issue ((code Bytes) (token I64)))") {
        errors.push("self-host typed issue channel regressed to point-only spans".to_owned());
    }
    for required in ["(fn report_issue", "(get issue start)", "(get issue end)"] {
        if !check.contains(required) {
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
        if !check.contains(required) {
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
        if !typing.contains(required) {
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
        "project-recur-rebind\tcheck-fail\tconformance/projects/recur-rebind/slim.project\tparity\tE0350@app@99:104,E0350@app@105:109",
    ) {
        errors.push("recursive-inout project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-nonexhaustive\tcheck-fail\tconformance/projects/nonexhaustive/slim.project\tparity\tE0336@app@56:77",
    ) {
        errors.push("nonexhaustive project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-boolean-recovery\tcheck-fail\tconformance/projects/boolean-recovery/slim.project\tparity\tE0336@app@56:96,E0314@app@74:81,E0335@app@83:95,E0344@app@89:94",
    ) {
        errors.push("Boolean recovery project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-ownership\tcheck-fail\tconformance/projects/ownership/slim.project\tparity\tE0315@app@248:254,E0347@app@329:335,E0315@app@482:488,E0347@app@576:582",
    ) {
        errors.push("ownership project projection fixture is missing".to_owned());
    }
    if !project_manifest.contains(
        "project-malformed-module\tcheck-fail\tconformance/projects/malformed-module/slim.project\tparity\tE0102@app@0:59\tproject:module-shape,project:diagnostics",
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
        if !project_source.contains(required) {
            errors.push(format!(
                "self-host project checker is missing manifest capability `{required}`"
            ));
        }
    }
    for required in ["(get issue start)", "(get issue end)"] {
        if !project_source.contains(required) {
            errors.push(format!(
                "self-host project checker is missing interval projection `{required}`"
            ));
        }
    }
    if !project_source.contains("(get checked issues)") {
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
        if !syntax.contains(required) {
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
        "(call item_list_valid source tokens (call i64.add root 3) closing)",
        "(call module_shape_valid_from source tokens 0 end)",
    ] {
        if !validate.contains(required) {
            errors.push(format!(
                "self-host module validation is missing bounded-slice capability `{required}`"
            ));
        }
    }

    let codegen_path = directory.join("codegen.slim");
    let Ok(codegen) = fs::read_to_string(&codegen_path) else {
        return;
    };
    let bounded_record_decision =
        fs::read_to_string(root.join("design/decisions/D0046-bounded-record-member-lookup.md"))
            .unwrap_or_default();
    for required in ["# D0046: Bounded record member lookup", "Status: accepted"] {
        if !bounded_record_decision.contains(required) {
            errors.push(format!(
                "bounded record member lookup is missing accepted decision evidence `{required}`"
            ));
        }
    }
    if codegen.contains("link_declaration_names") {
        errors.push("self-host code generation redundantly rebuilds declaration links".to_owned());
    }
    for required in [
        "(let variant_type I64 (call fact_type_index facts value)",
        "(let item I64 (call syntax/token_link tokens record)",
        "(let variant_link I64 (call syntax/token_link tokens variant)",
        "(let variant_link I64 (call syntax/token_link tokens variant_type)",
    ] {
        if !codegen.contains(required) {
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
        if !codegen.contains(required) {
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
        if !typing.contains(required) {
            errors.push(format!(
                "self-host typing is missing linked binding-mode capability `{required}`"
            ));
        }
    }
    if typing.contains("syntax/name_is_inout source tokens params result") {
        errors.push("self-host typing restored borrowed-return parameter scans".to_owned());
    }
    for required in [
        "(let case_link I64 (call syntax/token_link tokens expr)",
        "(let case_link I64 (call syntax/token_link tokens cursor)",
    ] {
        if !codegen.contains(required) {
            errors.push(format!(
                "self-host code generation is missing bounded variant member query `{required}`"
            ));
        }
    }
    for required in [
        "(fn checked_record_field_link",
        "(call syntax/token_link tokens field_form)",
        "(call checked_record_field_link source tokens cursor definition name_start name_end)",
    ] {
        if !codegen.contains(required) {
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
        if !manifest.contains(required) {
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
        if !query.contains(required) {
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
        if !cache.contains(required) {
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
        if !session.contains(required) {
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
        if !scheduler.contains(required) {
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
        "(fn span_has_form",
        "(call span_has_form source tokens body body_end \"recur\")",
        "(fn call_requires",
        "(call effects/call_requires source tokens callee 1)",
        "(call syntax/token_link tokens type_index)",
        "(let view typing/View (call typing/analyze input tokens declarations) (let plan memory/Plan (call memory/analyze input tokens declarations)",
        "(make typing/Checked (status status) (view view) (issues issues) (plan plan))",
        "(fn emit_program ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact)) (plan memory/Plan)",
        "(fn fact_type_index",
        "(call typing/fact_type facts expr)",
        "(call fact_type_index facts argument)",
        "(fn emit_match_value_binding ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact)) (inout allocations (Vec memory/AllocationPlan)) (module_items I64) (params I64) (value I64) (inout output (Vec U8)))",
        "(let type_index I64 (call fact_type_index facts value) (let boolean_match Bool (call i64.eq type_index -2)",
        "(fn emit_variant_match ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact))",
        "(let variant_type I64 (call fact_type_index facts value)",
        "checked_record_field_link source tokens cursor definition name_start name_end) (let type_index I64 (call fact_type_index facts value)",
        "(fn emit_case_bindings ((source Bytes) (inout tokens (Vec syntax/Token)) (inout facts (Vec typing/Fact)) (inout allocations (Vec memory/AllocationPlan)) (module_items I64) (params I64) (cursor I64) (payload_type I64) (inout output (Vec U8))) Unit (effects alloc partial) (let kind I64 (call syntax/token_kind tokens cursor) (let done Bool (call i64.eq kind 1) (match done (true unit) (false (let type_index I64 (call fact_type_index facts cursor)",
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
        if !joined.contains(required) {
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
        "design/decisions/D0028-direct-typed-reduction.md",
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

    let decision =
        fs::read_to_string(root.join("design/decisions/D0028-direct-typed-reduction.md"))
            .unwrap_or_default();
    for required in ["Status: accepted", "Primitive: none"] {
        if !decision.contains(required) {
            errors.push(format!(
                "Core 1A direct reduction requires D0028 field `{required}`"
            ));
        }
    }

    let reduction = fs::read_to_string(root.join("selfhost/reduce.slim")).unwrap_or_default();
    for required in [
        "(fn emit_expression",
        "(fn normalize_from",
        "(fn normalize",
        "(fn emit_normal_form",
        "(call normalize_from canonical canonical 7)",
    ] {
        if !reduction.contains(required) {
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
        if !analysis.contains(required) {
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
    if surface.contains("D0028") {
        errors.push("D0028 has Primitive: none and must not add Core surface".to_owned());
    }

    for directory in ["selfhost", "conformance", "examples", "docs", "design"] {
        visit_files(&root.join(directory), &mut |path| {
            if matches!(
                path.extension().and_then(|extension| extension.to_str()),
                Some("sil" | "slir")
            ) {
                errors.push(format!(
                    "separately parsed IR files are forbidden by D0028: {}",
                    path.display()
                ));
            }
        });
    }
}

fn check_bounded_program_evidence(root: &Path, errors: &mut Vec<String>) {
    for required in [
        "design/decisions/D0029-bounded-program-evidence.md",
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

    let decision =
        fs::read_to_string(root.join("design/decisions/D0029-bounded-program-evidence.md"))
            .unwrap_or_default();
    for required in ["Status: accepted", "Primitive: none"] {
        if !decision.contains(required) {
            errors.push(format!("Core 1B requires D0029 field `{required}`"));
        }
    }

    let analysis = fs::read_to_string(root.join("selfhost/analysis.slim")).unwrap_or_default();
    for required in [
        "(analysis 4",
        "(call quality/emit_module_facts",
        "(ownership-pressure",
        "(max-live-owned ",
        "(scope-end ",
    ] {
        if !analysis.contains(required) {
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
        if !quality.contains(required) {
            errors.push(format!("Core 1B quality evidence is missing `{required}`"));
        }
    }

    let proof = fs::read_to_string(root.join("selfhost/proof.slim")).unwrap_or_default();
    for required in [
        "(reduction-proof 1",
        "(pass-limit 8)",
        "(site-limit 64)",
        "(call reduce/reduction_kind",
        "(call reduce/normalize original)",
        "(call reduce/canonicalize candidate)",
    ] {
        if !proof.contains(required) {
            errors.push(format!(
                "Core 1B reduction evidence is missing `{required}`"
            ));
        }
    }

    let equivalence =
        fs::read_to_string(root.join("selfhost/equivalence.slim")).unwrap_or_default();
    for required in [
        "(call i64.lt count 8)",
        "(call i64.le left_size 256)",
        "(call i64.le right_size 256)",
        "(status equivalent)",
        "(status different)",
        "(status unknown)",
        "(counterexample (inputs",
    ] {
        if !equivalence.contains(required) {
            errors.push(format!(
                "Core 1B equivalence checker is missing `{required}`"
            ));
        }
    }

    let edit = fs::read_to_string(root.join("selfhost/edit.slim")).unwrap_or_default();
    for required in [
        "\"slim-edit\"",
        "(call i64.le replacement_size 64)",
        "(call emit_form source tokens 0 target",
    ] {
        if !edit.contains(required) {
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
    if surface.contains("D0029") {
        errors.push("D0029 has Primitive: none and must not add Core surface".to_owned());
    }

    let benchmark = fs::read_to_string(root.join("src/bin/slim-bench.rs")).unwrap_or_default();
    for required in [
        "fn run_agent()",
        "broken_model_token_proxy",
        "fn neutral_lexical_tokens",
        "fn changed_span",
        "fn agent_manifest",
    ] {
        if !benchmark.contains(required) {
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
