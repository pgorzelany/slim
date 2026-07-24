use std::collections::{BTreeMap, BTreeSet};
use std::fs;
use std::hint::black_box;
#[cfg(unix)]
use std::io::{Read, Write};
#[cfg(unix)]
use std::net::TcpListener;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};
use std::sync::atomic::{AtomicU64, Ordering};
#[cfg(unix)]
use std::thread;
use std::time::{Duration, Instant};

fn main() {
    let command = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "performance".to_owned());
    match command.as_str() {
        "performance" => run_performance(),
        "reduction" => run_reduction(),
        "incremental" => run_incremental(),
        "project" => run_project(),
        "compare" => run_comparison(),
        "parallelism" => run_parallelism_evidence(),
        "resources" => run_resource_evidence(),
        "host" => run_host_evidence(),
        "parallel-runtime" => run_parallel_runtime(),
        "agent" => run_agent(),
        _ => {
            eprintln!(
                "usage: slim-bench <performance [--quick] | reduction [--quick] | incremental [--quick] | project [--quick] | compare [--quick] | parallelism | resources | host | parallel-runtime [--quick] | agent>"
            );
            std::process::exit(64);
        }
    }
}

fn repository_root() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR"))
}

fn selfhost_compiler() -> PathBuf {
    let root = repository_root();
    let compiler = root.join("build/toolchain/slimc");
    if !compiler.is_file() {
        require_success(
            &mut Command::new(root.join("bootstrap.sh")),
            "portable bootstrap",
        );
    }
    compiler
}

fn run_performance() {
    let quick = has_quick_flag();
    let sizes: &[usize] = if quick {
        &[250, 500, 1_000, 2_000]
    } else {
        &[1_000, 2_000, 4_000, 8_000]
    };
    let samples = if quick { 5 } else { 9 };
    let compiler = selfhost_compiler();
    let directory = TemporaryDirectory::new("scaling");
    println!(
        "declarations\tsource_bytes\tgenerated_bytes\tcheck_us\temit_us\tcheck_ns_per_byte\temit_ns_per_byte\temit_check_ratio"
    );
    let mut first = None;
    let mut last = None;
    let mut two_thousand_ratio = None;
    for size in sizes {
        let source = generated_program(*size);
        let path = directory.path.join(format!("generated-{size}.slim"));
        fs::write(&path, &source).expect("write scaling source");
        require_clean_output(compiler_output(&compiler, "check", &path), "scaling warmup");
        let mut check_times = Vec::with_capacity(samples);
        let mut emit_times = Vec::with_capacity(samples);
        let mut generated_bytes = 0;
        for _ in 0..samples {
            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg("check").arg(&path),
                "SLIM check",
            );
            require_clean_output(output, "scaling check");
            check_times.push(elapsed);

            let (elapsed, output) = timed_output(Command::new(&compiler).arg(&path), "SLIM emit");
            if !output.status.success() || !output.stderr.is_empty() || output.stdout.is_empty() {
                fail_output("scaling emit", &output);
            }
            generated_bytes = output.stdout.len();
            black_box(&output.stdout);
            emit_times.push(elapsed);
        }
        check_times.sort();
        emit_times.sort();
        let check = check_times[samples / 2];
        let emit = emit_times[samples / 2];
        let check_per_byte = check.as_nanos() as f64 / source.len() as f64;
        let emit_per_byte = emit.as_nanos() as f64 / source.len() as f64;
        let emit_check_ratio = emit.as_nanos() as f64 / check.as_nanos() as f64;
        println!(
            "{size}\t{}\t{generated_bytes}\t{}\t{}\t{check_per_byte:.2}\t{emit_per_byte:.2}\t{emit_check_ratio:.3}",
            source.len(),
            check.as_micros(),
            emit.as_micros()
        );
        if *size == 2_000 {
            two_thousand_ratio = Some(emit_check_ratio);
        }
        if first.is_none() {
            first = Some((*size, check, emit));
        }
        last = Some((*size, check, emit));
    }
    let (first_size, first_check, first_emit) =
        first.expect("performance benchmark has a first sample");
    let (last_size, last_check, last_emit) = last.expect("performance benchmark has a last sample");
    let size_ratio = last_size as f64 / first_size as f64;
    for (metric, first_time, last_time) in [
        ("check-exponent", first_check, last_check),
        ("emit-exponent", first_emit, last_emit),
    ] {
        let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
        let exponent = time_ratio.ln() / size_ratio.ln();
        let limit = performance_budget(metric, "generated-declarations");
        if exponent > limit {
            eprintln!(
                "performance gate: {metric} {exponent:.3} exceeds {limit:.3} between {first_size} and {last_size} declarations"
            );
            std::process::exit(1);
        }
    }
    let ratio = two_thousand_ratio.expect("performance series must include 2,000 declarations");
    let ratio_limit = performance_budget("emit-check-ratio", "generated-2000");
    if ratio > ratio_limit {
        eprintln!(
            "performance gate: 2,000-declaration emit/check ratio {ratio:.3} exceeds {ratio_limit:.3}"
        );
        std::process::exit(1);
    }

    let nested_sizes: &[usize] = if quick {
        &[125, 250, 500, 1_000]
    } else {
        &[250, 500, 1_000, 2_000]
    };
    println!("nested_bindings\tsource_bytes\tcheck_us\tcheck_ns_per_byte");
    let mut first_nested = None;
    let mut last_nested = None;
    for size in nested_sizes {
        let source = generated_nested_program(*size);
        let path = directory.path.join(format!("nested-{size}.slim"));
        fs::write(&path, &source).expect("write nested scaling source");
        require_clean_output(
            compiler_output(&compiler, "check", &path),
            "nested scaling warmup",
        );
        let mut times = Vec::with_capacity(samples);
        for _ in 0..samples {
            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg("check").arg(&path),
                "SLIM nested check",
            );
            require_clean_output(output, "nested scaling check");
            times.push(elapsed);
        }
        times.sort();
        let elapsed = times[samples / 2];
        println!(
            "{size}\t{}\t{}\t{:.2}",
            source.len(),
            elapsed.as_micros(),
            elapsed.as_nanos() as f64 / source.len() as f64
        );
        if first_nested.is_none() {
            first_nested = Some((*size, elapsed));
        }
        last_nested = Some((*size, elapsed));
    }
    let (first_size, first_time) = first_nested.expect("nested series has a first sample");
    let (last_size, last_time) = last_nested.expect("nested series has a last sample");
    let size_ratio = last_size as f64 / first_size as f64;
    let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
    let exponent = time_ratio.ln() / size_ratio.ln();
    let limit = performance_budget("check-exponent", "generated-nested-bindings");
    if exponent > limit {
        eprintln!(
            "performance gate: nested check exponent {exponent:.3} exceeds {limit:.3} between {first_size} and {last_size} bindings"
        );
        std::process::exit(1);
    }

    benchmark_generated_emit_scaling(
        &compiler,
        &directory.path,
        nested_sizes,
        samples,
        "computed_arguments",
        "computed-arguments",
        "computed-argument",
        "generated-computed-arguments",
        "calls",
        generated_computed_argument_program,
    );
    benchmark_generated_emit_scaling(
        &compiler,
        &directory.path,
        nested_sizes,
        samples,
        "aggregate_temporaries",
        "aggregate-temporaries",
        "aggregate-temporary",
        "generated-aggregate-temporaries",
        "fields",
        generated_aggregate_temporary_program,
    );
    benchmark_generated_emit_scaling(
        &compiler,
        &directory.path,
        nested_sizes,
        samples,
        "planned_allocation_calls",
        "planned-allocation-calls",
        "planned allocation-call",
        "generated-planned-allocation-calls",
        "calls",
        generated_planned_allocation_call_program,
    );
    benchmark_generated_emit_scaling(
        &compiler,
        &directory.path,
        nested_sizes,
        samples,
        "inout_binding_reads",
        "inout-reads",
        "inout-binding read",
        "generated-inout-binding-reads",
        "parameters",
        generated_inout_read_program,
    );

    println!("named_type_parameters\tsource_bytes\tcheck_us\tcheck_ns_per_byte");
    let mut first_named = None;
    let mut last_named = None;
    for size in nested_sizes {
        let source = generated_named_type_program(*size);
        let path = directory.path.join(format!("named-types-{size}.slim"));
        fs::write(&path, &source).expect("write named-type scaling source");
        require_clean_output(
            compiler_output(&compiler, "check", &path),
            "named-type scaling warmup",
        );
        let mut times = Vec::with_capacity(samples);
        for _ in 0..samples {
            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg("check").arg(&path),
                "SLIM named-type check",
            );
            require_clean_output(output, "named-type scaling check");
            times.push(elapsed);
        }
        times.sort();
        let elapsed = times[samples / 2];
        println!(
            "{size}\t{}\t{}\t{:.2}",
            source.len(),
            elapsed.as_micros(),
            elapsed.as_nanos() as f64 / source.len() as f64
        );
        if first_named.is_none() {
            first_named = Some((*size, elapsed));
        }
        last_named = Some((*size, elapsed));
    }
    let (first_size, first_time) = first_named.expect("named-type series has a first sample");
    let (last_size, last_time) = last_named.expect("named-type series has a last sample");
    let size_ratio = last_size as f64 / first_size as f64;
    let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
    let exponent = time_ratio.ln() / size_ratio.ln();
    let limit = performance_budget("check-exponent", "generated-named-type-parameters");
    if exponent > limit {
        eprintln!(
            "performance gate: named-type check exponent {exponent:.3} exceeds {limit:.3} between {first_size} and {last_size} parameters"
        );
        std::process::exit(1);
    }

    println!("owned_transfers\tsource_bytes\tcheck_us\tcheck_ns_per_byte");
    let mut first_owned = None;
    let mut last_owned = None;
    for size in nested_sizes {
        let source = generated_owned_transfer_program(*size);
        let path = directory.path.join(format!("owned-transfers-{size}.slim"));
        fs::write(&path, &source).expect("write owned-transfer scaling source");
        require_clean_output(
            compiler_output(&compiler, "check", &path),
            "owned-transfer scaling warmup",
        );
        let mut times = Vec::with_capacity(samples);
        for _ in 0..samples {
            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg("check").arg(&path),
                "SLIM owned-transfer check",
            );
            require_clean_output(output, "owned-transfer scaling check");
            times.push(elapsed);
        }
        times.sort();
        let elapsed = times[samples / 2];
        println!(
            "{size}\t{}\t{}\t{:.2}",
            source.len(),
            elapsed.as_micros(),
            elapsed.as_nanos() as f64 / source.len() as f64
        );
        if first_owned.is_none() {
            first_owned = Some((*size, elapsed));
        }
        last_owned = Some((*size, elapsed));
    }
    let (first_size, first_time) = first_owned.expect("owned-transfer series has a first sample");
    let (last_size, last_time) = last_owned.expect("owned-transfer series has a last sample");
    let size_ratio = last_size as f64 / first_size as f64;
    let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
    let exponent = time_ratio.ln() / size_ratio.ln();
    let limit = performance_budget("check-exponent", "generated-owned-transfers");
    if exponent > limit {
        eprintln!(
            "performance gate: owned-transfer check exponent {exponent:.3} exceeds {limit:.3} between {first_size} and {last_size} transfers"
        );
        std::process::exit(1);
    }
}

#[allow(clippy::too_many_arguments)]
fn benchmark_generated_emit_scaling(
    compiler: &Path,
    directory: &Path,
    sizes: &[usize],
    samples: usize,
    heading: &str,
    file_stem: &str,
    role: &str,
    budget_subject: &str,
    units: &str,
    generate: fn(usize) -> String,
) {
    let rounds = balanced_round_count(samples);
    let mut cases = Vec::with_capacity(sizes.len());
    for size in sizes {
        let source = generate(*size);
        let path = directory.join(format!("{file_stem}-{size}.slim"));
        fs::write(&path, &source).expect("write generated emit-scaling source");
        let warmup = Command::new(compiler)
            .arg(&path)
            .output()
            .expect("run generated emit-scaling warmup");
        if !warmup.status.success() || !warmup.stderr.is_empty() || warmup.stdout.is_empty() {
            fail_output(&format!("{role} scaling warmup"), &warmup);
        }
        cases.push((*size, source.len(), path, Vec::with_capacity(rounds)));
    }

    // Alternate ascending and descending rounds so machine drift is shared by
    // both ends of the scaling series instead of being mistaken for growth.
    for round in 0..rounds {
        let mut order: Vec<usize> = (0..cases.len()).collect();
        if round % 2 == 1 {
            order.reverse();
        }
        for index in order {
            let (_, _, path, times) = &mut cases[index];
            let (elapsed, output) = timed_output(
                Command::new(compiler).arg(path),
                &format!("SLIM {role} emit"),
            );
            if !output.status.success() || !output.stderr.is_empty() || output.stdout.is_empty() {
                fail_output(&format!("{role} scaling emit"), &output);
            }
            black_box(&output.stdout);
            times.push(elapsed);
        }
    }

    println!("{heading}\tsource_bytes\temit_us\temit_ns_per_byte");
    let mut medians = Vec::with_capacity(cases.len());
    for (size, source_bytes, _, mut times) in cases {
        let elapsed = median_duration(&mut times);
        println!(
            "{size}\t{source_bytes}\t{}\t{:.2}",
            elapsed.as_micros(),
            elapsed.as_nanos() as f64 / source_bytes as f64
        );
        medians.push((size, elapsed));
    }

    let (first_size, first_time) = medians[0];
    let (last_size, last_time) = medians[medians.len() - 1];
    let size_ratio = last_size as f64 / first_size as f64;
    let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
    let exponent = time_ratio.ln() / size_ratio.ln();
    let limit = performance_budget("emit-exponent", budget_subject);
    if exponent > limit {
        eprintln!(
            "performance gate: {role} emit exponent {exponent:.3} exceeds {limit:.3} between {first_size} and {last_size} {units}"
        );
        std::process::exit(1);
    }
}

fn balanced_round_count(samples: usize) -> usize {
    assert!(samples > 0, "performance series must have samples");
    samples + samples % 2
}

fn median_duration(times: &mut [Duration]) -> Duration {
    assert!(!times.is_empty(), "performance series must have times");
    times.sort();
    let middle = times.len() / 2;
    if times.len().is_multiple_of(2) {
        (times[middle - 1] + times[middle]) / 2
    } else {
        times[middle]
    }
}

fn run_reduction() {
    let quick = has_quick_flag();
    let sizes: &[usize] = if quick {
        &[250, 500, 1_000, 2_000]
    } else {
        &[1_000, 2_000, 4_000, 8_000]
    };
    let samples = if quick { 5 } else { 9 };
    let compiler = selfhost_compiler();
    let directory = TemporaryDirectory::new("reduction");
    println!(
        "declarations\tbytes\treduce_us\tanalyze_us\tproof_us\treduced_bytes\tanalysis_bytes\tproof_bytes"
    );
    let mut first = None;
    let mut last = None;
    for size in sizes {
        let source = generated_program(*size);
        let path = directory.path.join(format!("generated-{size}.slim"));
        fs::write(&path, &source).expect("write reduction source");

        let warm_reduced = require_transform_output(
            compiler_output(&compiler, "reduce", &path),
            "reduction warmup",
        );
        let warm_analysis = require_transform_output(
            compiler_output(&compiler, "analyze", &path),
            "analysis warmup",
        );
        let warm_proof = require_transform_output(
            compiler_output(&compiler, "prove-reduction", &path),
            "proof warmup",
        );
        let mut reduce_times = Vec::with_capacity(samples);
        let mut analyze_times = Vec::with_capacity(samples);
        let mut proof_times = Vec::with_capacity(samples);
        for _ in 0..samples {
            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg("reduce").arg(&path),
                "SLIM reduction",
            );
            let reduced = require_transform_output(output, "reduction scaling");
            assert_eq!(reduced, warm_reduced, "reduction must be deterministic");
            black_box(&reduced);
            reduce_times.push(elapsed);

            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg("analyze").arg(&path),
                "SLIM semantic analysis",
            );
            let analysis = require_transform_output(output, "analysis scaling");
            assert_eq!(analysis, warm_analysis, "analysis must be deterministic");
            black_box(&analysis);
            analyze_times.push(elapsed);

            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg("prove-reduction").arg(&path),
                "SLIM reduction proof",
            );
            let proof = require_transform_output(output, "proof scaling");
            assert_eq!(proof, warm_proof, "proof output must be deterministic");
            black_box(&proof);
            proof_times.push(elapsed);
        }
        reduce_times.sort();
        analyze_times.sort();
        proof_times.sort();
        let reduce = reduce_times[samples / 2];
        let analyze = analyze_times[samples / 2];
        let proof = proof_times[samples / 2];
        println!(
            "{size}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
            source.len(),
            reduce.as_micros(),
            analyze.as_micros(),
            proof.as_micros(),
            warm_reduced.len(),
            warm_analysis.len(),
            warm_proof.len(),
        );
        if first.is_none() {
            first = Some((*size, reduce, analyze, proof));
        }
        last = Some((*size, reduce, analyze, proof));
    }

    let (first_size, first_reduce, first_analyze, first_proof) =
        first.expect("reduction benchmark has a first sample");
    let (last_size, last_reduce, last_analyze, last_proof) =
        last.expect("reduction benchmark has a last sample");
    for (role, first_time, last_time) in [
        ("reduction", first_reduce, last_reduce),
        ("analysis", first_analyze, last_analyze),
        ("proof", first_proof, last_proof),
    ] {
        let metric = format!("{role}-exponent");
        let limit = performance_budget(&metric, "generated-declarations");
        let size_ratio = last_size as f64 / first_size as f64;
        let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
        let exponent = time_ratio.ln() / size_ratio.ln();
        if exponent > limit {
            eprintln!(
                "{role} scaling gate: process-level exponent {exponent:.3} exceeds {limit:.2} between {first_size} and {last_size} declarations"
            );
            std::process::exit(1);
        }
    }
}

fn run_incremental() {
    let quick = has_quick_flag();
    let sizes: &[usize] = if quick { &[4, 8, 16] } else { &[8, 16, 32, 64] };
    let samples = if quick { 3 } else { 5 };
    let compiler = selfhost_compiler();
    println!("graph\tmodules\tscenario\tsession_us\twork");
    for graph in [ProjectGraph::Wide, ProjectGraph::Deep] {
        let mut series = BTreeMap::new();
        for modules in sizes {
            for scenario in [
                IncrementalScenario::NoChange,
                IncrementalScenario::PrivateBody,
                IncrementalScenario::PublicInterface,
            ] {
                let mut times = Vec::with_capacity(samples);
                let mut oracle = None;
                for _ in 0..samples {
                    let pair = SnapshotPair::new(graph, *modules, scenario);
                    let (elapsed, output) = timed_output(
                        Command::new(&compiler)
                            .arg("session")
                            .arg(pair.initial_manifest())
                            .arg(pair.updated_manifest()),
                        "SLIM incremental session",
                    );
                    if !output.status.success()
                        || !output.stderr.is_empty()
                        || output.stdout.is_empty()
                    {
                        fail_output("incremental session", &output);
                    }
                    if let Some(expected) = &oracle {
                        assert_eq!(
                            expected, &output.stdout,
                            "session work must be deterministic"
                        );
                    } else {
                        oracle = Some(output.stdout.clone());
                    }
                    times.push(elapsed);
                }
                times.sort();
                let median = times[samples / 2];
                let work = String::from_utf8(oracle.expect("incremental sample"))
                    .expect("session work is UTF-8");
                println!(
                    "{}\t{}\t{}\t{}\t{}",
                    graph.name(),
                    modules + 1,
                    scenario.name(),
                    median.as_micros(),
                    work.trim()
                );
                update_series(&mut series, scenario.name(), *modules, median);
            }
        }
        enforce_scaling_series("incremental-exponent", graph.name(), &series);
    }
}

fn run_project() {
    let quick = has_quick_flag();
    let sizes: &[usize] = if quick { &[4, 8, 16] } else { &[8, 16, 32, 64] };
    let samples = if quick { 2 } else { 5 };
    let compiler = selfhost_compiler();
    println!("graph\tmodules\tjobs\temit_us\tgenerated_bytes");
    for graph in [ProjectGraph::Wide, ProjectGraph::Deep] {
        let mut serial_series = BTreeMap::new();
        for modules in sizes {
            let project = GeneratedProject::new(graph, *modules, "project");
            let mut deterministic = None;
            for jobs in [1, 2, 4] {
                let mut times = Vec::with_capacity(samples);
                let mut output_oracle = None;
                for _ in 0..samples {
                    let (elapsed, output) = timed_output(
                        Command::new(&compiler)
                            .arg(project.manifest())
                            .arg("--jobs")
                            .arg(jobs.to_string()),
                        "SLIM project emit",
                    );
                    if !output.status.success()
                        || !output.stderr.is_empty()
                        || output.stdout.is_empty()
                    {
                        fail_output("project emit", &output);
                    }
                    if let Some(expected) = &output_oracle {
                        assert_eq!(
                            expected, &output.stdout,
                            "repeated output must be deterministic"
                        );
                    } else {
                        output_oracle = Some(output.stdout.clone());
                    }
                    times.push(elapsed);
                }
                let generated = output_oracle.expect("project output sample");
                if let Some(expected) = &deterministic {
                    assert_eq!(expected, &generated, "worker count must not change output");
                } else {
                    deterministic = Some(generated.clone());
                }
                times.sort();
                let median = times[samples / 2];
                println!(
                    "{}\t{}\t{jobs}\t{}\t{}",
                    graph.name(),
                    modules + 1,
                    median.as_micros(),
                    generated.len()
                );
                if jobs == 1 {
                    update_series(&mut serial_series, "serial", *modules, median);
                }
            }
        }
        enforce_scaling_series("project-emit-exponent", graph.name(), &serial_series);
    }
}

fn update_series(
    series: &mut BTreeMap<String, (usize, Duration, usize, Duration)>,
    name: &str,
    size: usize,
    elapsed: Duration,
) {
    series
        .entry(name.to_owned())
        .and_modify(|sample| {
            sample.2 = size;
            sample.3 = elapsed;
        })
        .or_insert((size, elapsed, size, elapsed));
}

fn enforce_scaling_series(
    metric: &str,
    prefix: &str,
    series: &BTreeMap<String, (usize, Duration, usize, Duration)>,
) {
    for (name, (first_size, first_time, last_size, last_time)) in series {
        let size_ratio = *last_size as f64 / *first_size as f64;
        let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
        let exponent = time_ratio.ln() / size_ratio.ln();
        let workload = if name == "serial" {
            prefix.to_owned()
        } else {
            format!("{prefix}-{name}")
        };
        let limit = performance_budget(metric, &workload);
        if exponent > limit {
            eprintln!(
                "performance gate: {metric}/{workload} exponent {exponent:.3} exceeds {limit:.3} between {first_size} and {last_size} generated modules"
            );
            std::process::exit(1);
        }
    }
}

fn run_comparison() {
    let quick = has_quick_flag();
    let root = repository_root();
    let compiler = root.join("slimc");
    let build = ComparisonBuild::new();
    let challenges = challenge_manifest();
    let samples = if quick { 5 } else { 15 };
    let mut all_c_log_sum = 0.0f64;
    let mut all_rust_log_sum = 0.0f64;
    let mut serial_c_log_sum = 0.0f64;
    let mut serial_rust_log_sum = 0.0f64;
    let mut serial_count = 0usize;
    println!("challenge\tlanguage\tcompile_ms\truntime_us\tbinary_bytes");
    for challenge in &challenges {
        let directory = root.join("benchmarks/challenges").join(challenge);
        let slim_output = build.path.join(format!("{challenge}-slim"));
        let c_output = build.path.join(format!("{challenge}-c"));
        let rust_output = build.path.join(format!("{challenge}-rust"));

        let slim_compile = timed_success(
            Command::new(&compiler)
                .arg("build")
                .arg(directory.join("program.slim"))
                .arg("-o")
                .arg(&slim_output),
            "SLIM challenge build",
        );
        let c_compile = timed_success(
            Command::new(native_compiler())
                .arg("-std=c11")
                .arg("-O3")
                .arg("-Wall")
                .arg("-Wextra")
                .arg("-Werror")
                .arg(directory.join("program.c"))
                .arg("-o")
                .arg(&c_output),
            "C challenge build",
        );
        let rust_compile = timed_success(
            Command::new("rustc")
                .arg("--edition=2024")
                .arg("-D")
                .arg("warnings")
                .arg("-C")
                .arg("opt-level=3")
                .arg("-C")
                .arg("debuginfo=0")
                .arg(directory.join("program.rs"))
                .arg("-o")
                .arg(&rust_output),
            "Rust challenge build",
        );

        let arguments = if challenge == "bytefreq" {
            vec![build.path.join("input.bin")]
        } else {
            Vec::new()
        };
        let expected = run_output(&c_output, &arguments);
        let rust_result = run_output(&rust_output, &arguments);
        let slim_result = run_output(&slim_output, &arguments);
        if rust_result != expected || slim_result != expected {
            eprintln!("{challenge}: output mismatch between implementations");
            std::process::exit(1);
        }

        let c_runtime = median_runtime(&c_output, &arguments, samples);
        let rust_runtime = median_runtime(&rust_output, &arguments, samples);
        let slim_runtime = median_runtime(&slim_output, &arguments, samples);
        let runtime_ratio = slim_runtime.as_nanos() as f64 / c_runtime.as_nanos() as f64;
        let rust_ratio = slim_runtime.as_nanos() as f64 / rust_runtime.as_nanos() as f64;
        all_c_log_sum += runtime_ratio.ln();
        all_rust_log_sum += rust_ratio.ln();
        if !challenge_has_feature(challenge, "parallel-candidate") {
            serial_c_log_sum += runtime_ratio.ln();
            serial_rust_log_sum += rust_ratio.ln();
            serial_count += 1;
        }
        let runtime_limit = performance_budget("native-runtime-ratio", challenge);
        if runtime_ratio > runtime_limit {
            eprintln!(
                "performance gate: {challenge} SLIM/C runtime ratio {runtime_ratio:.3} exceeds {runtime_limit:.3}"
            );
            std::process::exit(1);
        }

        for (language, executable, compile_time, runtime) in [
            ("slim", &slim_output, slim_compile, slim_runtime),
            ("c", &c_output, c_compile, c_runtime),
            ("rust", &rust_output, rust_compile, rust_runtime),
        ] {
            let binary_size = fs::metadata(executable)
                .expect("benchmark binary metadata")
                .len();
            println!(
                "{challenge}\t{language}\t{:.3}\t{}\t{binary_size}",
                compile_time.as_secs_f64() * 1_000.0,
                runtime.as_micros()
            );
        }
    }
    assert!(
        serial_count > 0,
        "comparison corpus must retain serial work"
    );
    let all_count = challenges.len() as f64;
    let all_c_geomean = (all_c_log_sum / all_count).exp();
    let all_rust_geomean = (all_rust_log_sum / all_count).exp();
    let serial_c_geomean = (serial_c_log_sum / serial_count as f64).exp();
    let serial_rust_geomean = (serial_rust_log_sum / serial_count as f64).exp();
    println!("aggregate\tapplications\tslim_over_c\tslim_over_rust");
    println!(
        "all\t{}\t{all_c_geomean:.3}\t{all_rust_geomean:.3}",
        challenges.len()
    );
    println!("serial\t{serial_count}\t{serial_c_geomean:.3}\t{serial_rust_geomean:.3}");
    for (workload, ratio) in [
        ("expanded-all", all_c_geomean),
        ("expanded-serial", serial_c_geomean),
    ] {
        let limit = performance_budget("native-runtime-geomean", workload);
        if ratio > limit {
            eprintln!(
                "performance gate: {workload} SLIM/C runtime geometric mean {ratio:.3} exceeds {limit:.3}"
            );
            std::process::exit(1);
        }
    }
}

fn run_parallel_runtime() {
    let quick = has_quick_flag();
    let samples = if quick { 7 } else { 15 };
    let iterations: &[usize] = if quick {
        &[0, 10_000, 100_000, 2_000_000]
    } else {
        &[0, 1_000, 10_000, 100_000, 1_000_000, 2_000_000]
    };
    let root = repository_root();
    let source = root.join("benchmarks/challenges/state_machine");
    let build = ComparisonBuild::new();
    println!("iterations\trepetitions\tserial_us\tmanual_parallel_us\tparallel_over_serial");
    for &work in iterations {
        let repetitions = match work {
            0..=1_000 => 200,
            1_001..=10_000 => 100,
            10_001..=100_000 => 20,
            100_001..=1_000_000 => 4,
            _ => 2,
        };
        let serial = build.path.join(format!("state-machine-serial-{work}"));
        let parallel = build.path.join(format!("state-machine-parallel-{work}"));
        let definition = format!("-DSTATE_MACHINE_ITERATIONS={work}");
        let repetition_definition = format!("-DSTATE_MACHINE_REPETITIONS={repetitions}");
        timed_success(
            Command::new(native_compiler())
                .arg("-std=c11")
                .arg("-O3")
                .arg("-Wall")
                .arg("-Wextra")
                .arg("-Werror")
                .arg(&definition)
                .arg(&repetition_definition)
                .arg(source.join("program.c"))
                .arg("-o")
                .arg(&serial),
            "serial state-machine probe build",
        );
        timed_success(
            Command::new(native_compiler())
                .arg("-std=c11")
                .arg("-O3")
                .arg("-Wall")
                .arg("-Wextra")
                .arg("-Werror")
                .arg("-pthread")
                .arg(&definition)
                .arg(&repetition_definition)
                .arg(source.join("program_parallel.c"))
                .arg("-o")
                .arg(&parallel),
            "manual POSIX parallel state-machine probe build",
        );
        let expected = run_output(&serial, &[]);
        assert_eq!(
            run_output(&parallel, &[]),
            expected,
            "manual parallel probe must preserve state-machine output"
        );
        let serial_time = median_runtime(&serial, &[], samples);
        let parallel_time = median_runtime(&parallel, &[], samples);
        let ratio = parallel_time.as_nanos() as f64 / serial_time.as_nanos() as f64;
        println!(
            "{work}\t{repetitions}\t{}\t{}\t{ratio:.3}",
            serial_time.as_micros(),
            parallel_time.as_micros()
        );
        if work == 2_000_000 {
            let limit = performance_budget("manual-parallel-runtime-ratio", "state_machine");
            if ratio > limit {
                eprintln!(
                    "performance gate: state_machine manual parallel/serial ratio {ratio:.3} exceeds {limit:.3}"
                );
                std::process::exit(1);
            }
        }
    }

    println!("generated_challenge\tserial_us\tparallel_us\tparallel_over_serial");
    for challenge in ["state_machine", "signal_network"] {
        let executable = build.path.join(format!("{challenge}-generated"));
        let challenge_source = root
            .join("benchmarks/challenges")
            .join(challenge)
            .join("program.slim");
        let mut command = Command::new(root.join("slimc"));
        command
            .env(
                "SLIM_WORKER_TIER",
                if cfg!(unix) { "posix" } else { "serial" },
            )
            .arg("build")
            .arg(challenge_source)
            .arg("-o")
            .arg(&executable);
        timed_success(&mut command, "generated parallel challenge build");

        let serial_environment = [("SLIM_TASK_DISABLE", "1")];
        let expected = run_output_with_env(&executable, &[], &serial_environment);
        assert_eq!(
            run_output(&executable, &[]),
            expected,
            "{challenge}: generated parallel execution must preserve serial-fallback output"
        );
        let serial_time = median_runtime_with_env(&executable, &[], &serial_environment, samples);
        let parallel_time = median_runtime(&executable, &[], samples);
        let ratio = parallel_time.as_nanos() as f64 / serial_time.as_nanos() as f64;
        println!(
            "{challenge}\t{}\t{}\t{ratio:.3}",
            serial_time.as_micros(),
            parallel_time.as_micros()
        );
        if cfg!(unix) {
            let limit = performance_budget("generated-parallel-runtime-ratio", challenge);
            if ratio > limit {
                eprintln!(
                    "performance gate: {challenge} generated parallel/serial ratio {ratio:.3} exceeds {limit:.3}"
                );
                std::process::exit(1);
            }
        }
    }
}

const PARALLELISM_REASONS: [(&str, &str); 11] = [
    ("declared_effects", "declared-effects"),
    ("exclusive_borrow", "exclusive-borrow"),
    ("mutation", "mutation"),
    ("checked_trap", "checked-trap"),
    ("allocation_or_io", "allocation-or-io"),
    ("recurrence", "recurrence"),
    ("callee_not_safe", "callee-not-safe"),
    ("call_cycle", "call-cycle"),
    ("function_limit", "function-limit"),
    ("edge_limit", "edge-limit"),
    ("missing_typed_fact", "missing-typed-fact"),
];

#[derive(Debug, PartialEq, Eq)]
struct ResourceEvidence {
    source_bytes: usize,
    recurrence_profiles: usize,
    profiled_call_sites: usize,
    exact_call_work_sites: usize,
    unknown_call_work_sites: usize,
    maximum_exact_iterations: usize,
    expression_nodes: usize,
    allocation_sites: usize,
    trap_sites: usize,
    owned_bindings: usize,
    max_live_owned: usize,
    effectful_functions: usize,
    allocation_effect_functions: usize,
    partial_functions: usize,
    total_functions: usize,
}

fn run_resource_evidence() {
    let root = repository_root();
    let compiler = selfhost_compiler();
    let challenges = challenge_manifest();
    let mut measured = Vec::with_capacity(challenges.len());
    for challenge in &challenges {
        let path = root
            .join("benchmarks/challenges")
            .join(challenge)
            .join("program.slim");
        let first = require_transform_output(
            compiler_output(&compiler, "analyze", &path),
            "resource application analysis",
        );
        let second = require_transform_output(
            compiler_output(&compiler, "analyze", &path),
            "repeated resource application analysis",
        );
        assert_eq!(first, second, "{challenge}: analysis must be deterministic");
        assert!(
            report_parentheses_are_balanced(&first),
            "{challenge}: analysis report must be balanced"
        );
        let report = std::str::from_utf8(&first).expect("analysis report must be UTF-8");
        measured.push((challenge.clone(), measure_resource_evidence(&path, report)));
    }

    println!(
        "challenge\tsource_bytes\trecurrence_profiles\tprofiled_call_sites\texact_call_work_sites\tunknown_call_work_sites\tmaximum_exact_iterations\texpression_nodes\tallocation_sites\ttrap_sites\towned_bindings\tmax_live_owned\teffectful_functions\tallocation_effect_functions\tpartial_functions\ttotal_functions"
    );
    for (challenge, evidence) in &measured {
        println!(
            "{challenge}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
            evidence.source_bytes,
            evidence.recurrence_profiles,
            evidence.profiled_call_sites,
            evidence.exact_call_work_sites,
            evidence.unknown_call_work_sites,
            evidence.maximum_exact_iterations,
            evidence.expression_nodes,
            evidence.allocation_sites,
            evidence.trap_sites,
            evidence.owned_bindings,
            evidence.max_live_owned,
            evidence.effectful_functions,
            evidence.allocation_effect_functions,
            evidence.partial_functions,
            evidence.total_functions
        );
    }

    let baseline = resource_baseline();
    for (challenge, evidence) in &measured {
        let expected = baseline
            .get(challenge)
            .unwrap_or_else(|| panic!("missing resource baseline for {challenge}"));
        assert_eq!(
            evidence, expected,
            "{challenge}: resource evidence changed; record the reason and update the durable baseline"
        );
    }
    assert_eq!(
        baseline.len(),
        challenges.len(),
        "resource baseline contains a challenge absent from the manifest"
    );
}

#[cfg(unix)]
fn structured_host_server(
    listener: TcpListener,
    expected: &'static [u8],
    response: &'static [u8],
    requests: usize,
    delay: Duration,
) -> thread::JoinHandle<()> {
    thread::spawn(move || {
        for _ in 0..requests {
            let (mut stream, _) = listener.accept().expect("accept loopback request");
            let mut request = Vec::new();
            stream
                .read_to_end(&mut request)
                .expect("read loopback request");
            assert_eq!(request, expected);
            thread::sleep(delay);
            stream.write_all(response).expect("write loopback response");
        }
    })
}

#[cfg(unix)]
fn run_structured_host_application(
    root: &Path,
    build: &TemporaryDirectory,
    application: &str,
    left_request: &'static [u8],
    right_request: &'static [u8],
    response: &'static [u8],
) {
    let left = TcpListener::bind("127.0.0.1:0").expect("bind left loopback listener");
    let right = TcpListener::bind("127.0.0.1:0").expect("bind right loopback listener");
    let left_port = left.local_addr().unwrap().port();
    let right_port = right.local_addr().unwrap().port();
    let template = fs::read_to_string(
        root.join("benchmarks/host")
            .join(format!("{application}.slim")),
    )
    .expect("read structured host application");
    let source = build.path.join(format!("{application}.slim"));
    fs::write(
        &source,
        template
            .replace("8080", &left_port.to_string())
            .replace("8081", &right_port.to_string()),
    )
    .expect("write structured host application");

    let serial = build.path.join(format!("{application}-serial"));
    let parallel = build.path.join(format!("{application}-parallel"));
    require_success(
        Command::new(root.join("slimc"))
            .env("SLIM_WORKER_TIER", "serial")
            .arg("build")
            .arg(&source)
            .arg("-o")
            .arg(&serial),
        "structured host serial build",
    );
    require_success(
        Command::new(root.join("slimc"))
            .env("SLIM_WORKER_TIER", "posix")
            .arg("build")
            .arg(&source)
            .arg("-o")
            .arg(&parallel),
        "structured host parallel build",
    );

    let samples = 5;
    let runs = 2 * (samples + 1);
    let delay = Duration::from_millis(80);
    let left_server = structured_host_server(left, left_request, response, runs, delay);
    let right_server = structured_host_server(right, right_request, response, runs, delay);
    assert_eq!(run_output(&serial, &[]), b"OK\n");
    assert_eq!(run_output(&parallel, &[]), b"OK\n");
    let mut serial_times = Vec::with_capacity(samples);
    let mut parallel_times = Vec::with_capacity(samples);
    for _ in 0..samples {
        let (serial_time, serial_output) =
            timed_output(&mut Command::new(&serial), "structured host serial run");
        assert!(serial_output.status.success());
        assert_eq!(serial_output.stdout, b"OK\n");
        assert!(serial_output.stderr.is_empty());
        serial_times.push(serial_time);

        let (parallel_time, parallel_output) =
            timed_output(&mut Command::new(&parallel), "structured host parallel run");
        assert!(parallel_output.status.success());
        assert_eq!(parallel_output.stdout, b"OK\n");
        assert!(parallel_output.stderr.is_empty());
        parallel_times.push(parallel_time);
    }
    left_server.join().unwrap();
    right_server.join().unwrap();
    serial_times.sort();
    parallel_times.sort();
    let serial_time = serial_times[samples / 2];
    let parallel_time = parallel_times[samples / 2];
    let ratio = parallel_time.as_nanos() as f64 / serial_time.as_nanos() as f64;
    println!("structured_host\tserial_us\tparallel_us\tparallel_over_serial");
    println!(
        "{application}\t{}\t{}\t{ratio:.3}",
        serial_time.as_micros(),
        parallel_time.as_micros()
    );
    let limit = performance_budget("structured-host-runtime-ratio", application);
    if ratio > limit {
        eprintln!(
            "performance gate: structured host {application} parallel/serial ratio {ratio:.3} exceeds {limit:.3}"
        );
        std::process::exit(1);
    }
}

fn run_host_evidence() {
    let root = repository_root();
    let build = TemporaryDirectory::new("host-clock");
    let slim = build.path.join("clock-slim");
    let c = build.path.join("clock-c");
    require_success(
        Command::new(root.join("slimc"))
            .arg("build")
            .arg(root.join("benchmarks/host/clock.slim"))
            .arg("-o")
            .arg(&slim),
        "SLIM host clock build",
    );
    require_success(
        Command::new(native_compiler())
            .arg("-std=c11")
            .arg("-O2")
            .arg("-DNDEBUG")
            .arg("-Wall")
            .arg("-Wextra")
            .arg("-Werror")
            .arg("-I")
            .arg(root.join("runtime"))
            .arg(root.join("benchmarks/host/clock.c"))
            .arg(root.join("runtime/slim_rt.c"))
            .arg("-o")
            .arg(&c),
        "C host clock reference build",
    );
    assert!(run_output(&slim, &[]).is_empty());
    assert!(run_output(&c, &[]).is_empty());
    let slim_time = median_runtime(&slim, &[], 9);
    let c_time = median_runtime(&c, &[], 9);
    let ratio = slim_time.as_nanos() as f64 / c_time.as_nanos() as f64;
    println!("calls\tslim_us\tc_us\tslim_over_c");
    println!(
        "100000\t{}\t{}\t{ratio:.3}",
        slim_time.as_micros(),
        c_time.as_micros()
    );
    let limit = performance_budget("host-clock-runtime-ratio", "clock-100000");
    if ratio > limit {
        eprintln!("performance gate: host clock SLIM/C ratio {ratio:.3} exceeds {limit:.3}");
        std::process::exit(1);
    }

    let hello_c = build.path.join("hello.c");
    let hello_host = build.path.join("hello-host");
    let hello_without_network = build.path.join("hello-without-network");
    require_success(
        Command::new(root.join("slimc"))
            .arg("emit-c")
            .arg(root.join("examples/hello.slim"))
            .arg("-o")
            .arg(&hello_c),
        "host-cost hello emission",
    );
    require_success(
        Command::new(native_compiler())
            .arg("-std=c11")
            .arg("-O2")
            .arg("-DNDEBUG")
            .arg("-Wall")
            .arg("-Wextra")
            .arg("-Werror")
            .arg("-I")
            .arg(root.join("runtime"))
            .arg(&hello_c)
            .arg(root.join("runtime/slim_rt.c"))
            .arg("-o")
            .arg(&hello_host),
        "host-cost hello build",
    );
    require_success(
        Command::new(native_compiler())
            .arg("-std=c11")
            .arg("-O2")
            .arg("-DNDEBUG")
            .arg("-Wall")
            .arg("-Wextra")
            .arg("-Werror")
            .arg("-DSLIM_DISABLE_NETWORK=1")
            .arg("-I")
            .arg(root.join("runtime"))
            .arg(&hello_c)
            .arg(root.join("runtime/slim_rt.c"))
            .arg("-o")
            .arg(&hello_without_network),
        "host-cost hello reference build",
    );
    assert_eq!(run_output(&hello_host, &[]), b"hello from SLIM\n");
    assert_eq!(
        run_output(&hello_without_network, &[]),
        b"hello from SLIM\n"
    );
    let host_bytes = fs::metadata(&hello_host).unwrap().len();
    let reference_bytes = fs::metadata(&hello_without_network).unwrap().len();
    let binary_ratio = host_bytes as f64 / reference_bytes as f64;
    println!("binary\thost_bytes\twithout_network_bytes\thost_over_reference");
    println!("hello\t{host_bytes}\t{reference_bytes}\t{binary_ratio:.3}");
    let binary_limit = performance_budget("host-network-binary-ratio", "hello");
    if binary_ratio > binary_limit {
        eprintln!(
            "performance gate: unused host network binary ratio {binary_ratio:.3} exceeds {binary_limit:.3}"
        );
        std::process::exit(1);
    }

    #[cfg(unix)]
    {
        run_structured_host_application(&root, &build, "dual_fetch", b"LEFT", b"RIGHT", b"PONG");
        run_structured_host_application(&root, &build, "dual_health", b"A", b"B", b"OK");
    }
}

fn measure_resource_evidence(path: &Path, report: &str) -> ResourceEvidence {
    assert!(report.starts_with("(analysis 7 "));
    let resources = report_section(report, "(resource-evidence ", " (quality ");
    let quality = report_section(report, "(quality ", " (parallelism ");
    let functions = report_number(quality, "(quality (guarantee exact) (functions ");
    let (effectful_functions, allocation_effect_functions, partial_functions) =
        report_effect_counts(quality);
    assert_eq!(
        report_numbers(quality, "(expression-nodes ").len(),
        functions,
        "every application function must report expression nodes"
    );
    let total_functions = quality.matches("(status total)").count();
    ResourceEvidence {
        source_bytes: fs::metadata(path)
            .expect("resource application metadata")
            .len() as usize,
        recurrence_profiles: report_number(resources, "(recurrence-profile-count "),
        profiled_call_sites: report_number(resources, "(profiled-call-site-count "),
        exact_call_work_sites: report_number(resources, "(exact-call-work-sites "),
        unknown_call_work_sites: report_number(resources, "(unknown-call-work-sites "),
        maximum_exact_iterations: report_number(resources, "(maximum-exact-iterations "),
        expression_nodes: report_numbers(quality, "(expression-nodes ")
            .into_iter()
            .sum(),
        allocation_sites: report_numbers(quality, "(allocation-sites ")
            .into_iter()
            .sum(),
        trap_sites: report_numbers(quality, "(trap-sites ").into_iter().sum(),
        owned_bindings: report_numbers(report, "(owned-bindings ").into_iter().sum(),
        max_live_owned: report_numbers(report, "(max-live-owned ")
            .into_iter()
            .max()
            .unwrap_or(0),
        effectful_functions,
        allocation_effect_functions,
        partial_functions,
        total_functions,
    }
}

fn resource_baseline() -> BTreeMap<String, ResourceEvidence> {
    let path = repository_root().join("benchmarks/resource-baseline.tsv");
    let contents = fs::read_to_string(path).expect("read resource baseline");
    let mut baseline = BTreeMap::new();
    for (line_number, line) in contents.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        assert_eq!(
            columns.len(),
            16,
            "resource baseline line {} must have sixteen columns",
            line_number + 1
        );
        let number = |index: usize| {
            columns[index].parse::<usize>().unwrap_or_else(|_| {
                panic!(
                    "invalid resource evidence on line {}, column {}",
                    line_number + 1,
                    index + 1
                )
            })
        };
        let evidence = ResourceEvidence {
            source_bytes: number(1),
            recurrence_profiles: number(2),
            profiled_call_sites: number(3),
            exact_call_work_sites: number(4),
            unknown_call_work_sites: number(5),
            maximum_exact_iterations: number(6),
            expression_nodes: number(7),
            allocation_sites: number(8),
            trap_sites: number(9),
            owned_bindings: number(10),
            max_live_owned: number(11),
            effectful_functions: number(12),
            allocation_effect_functions: number(13),
            partial_functions: number(14),
            total_functions: number(15),
        };
        assert!(
            baseline.insert(columns[0].to_owned(), evidence).is_none(),
            "duplicate resource baseline for {}",
            columns[0]
        );
    }
    baseline
}

#[derive(Debug, PartialEq, Eq)]
struct ParallelismEvidence {
    source_bytes: usize,
    functions: usize,
    checked_sites: usize,
    reported_total_sites: usize,
    refinements: usize,
    safe_functions: usize,
    reasons: [usize; PARALLELISM_REASONS.len()],
    blockers: [usize; PARALLELISM_REASONS.len()],
    candidate_sites: usize,
    selected_sites: usize,
    reported_sites: usize,
    executable_sites: usize,
    executed_sites: usize,
    eligible_sites: usize,
}

fn run_parallelism_evidence() {
    let root = repository_root();
    let compiler = selfhost_compiler();
    let challenges = challenge_manifest();
    let baseline = parallelism_baseline();
    println!(
        "challenge\tsource_bytes\tfunctions\tchecked_sites\treported_total_sites\trefinements\tsafe_functions\t{}\t{}\tcandidate_sites\tselected_sites\treported_sites\texecutable_sites\texecuted_sites\teligible_sites",
        PARALLELISM_REASONS
            .iter()
            .map(|(column, _)| *column)
            .collect::<Vec<_>>()
            .join("\t"),
        PARALLELISM_REASONS
            .iter()
            .map(|(column, _)| format!("blocker_{column}"))
            .collect::<Vec<_>>()
            .join("\t")
    );
    let mut measured = Vec::with_capacity(challenges.len());
    for challenge in &challenges {
        let path = root
            .join("benchmarks/challenges")
            .join(challenge)
            .join("program.slim");
        let first = require_transform_output(
            compiler_output(&compiler, "analyze", &path),
            "parallelism application analysis",
        );
        let second = require_transform_output(
            compiler_output(&compiler, "analyze", &path),
            "repeated parallelism application analysis",
        );
        assert_eq!(first, second, "{challenge}: analysis must be deterministic");
        assert!(
            report_parentheses_are_balanced(&first),
            "{challenge}: analysis report must be balanced"
        );
        let report = std::str::from_utf8(&first).expect("analysis report must be UTF-8");
        let evidence = measure_parallelism_evidence(&path, report);
        print_parallelism_evidence(challenge, &evidence);
        measured.push((challenge.clone(), evidence));
    }
    for (challenge, evidence) in &measured {
        let expected = baseline
            .get(challenge)
            .unwrap_or_else(|| panic!("missing parallelism baseline for {challenge}"));
        assert_eq!(
            evidence, expected,
            "{challenge}: parallelism evidence changed; record the reason and update the durable baseline"
        );
    }
    assert_eq!(
        baseline.len(),
        challenges.len(),
        "parallelism baseline contains a challenge absent from the manifest"
    );
}

fn measure_parallelism_evidence(path: &Path, report: &str) -> ParallelismEvidence {
    assert!(report.starts_with("(analysis 7 "));
    let integer = report_section(report, "(integer-proofs ", " (resource-evidence ");
    let parallel = report_section(report, "(parallelism ", " (function ");
    let functions = report_number(report, "(quality (guarantee exact) (functions ");
    let safe_functions = parallel.matches("(status safe)").count();
    let mut reasons = [0; PARALLELISM_REASONS.len()];
    for (index, (_, report_name)) in PARALLELISM_REASONS.iter().enumerate() {
        reasons[index] = parallel.matches(&format!("(reason {report_name})")).count();
    }
    let (blocker_sets, blockers) = parallelism_blockers(parallel);
    assert_eq!(
        safe_functions + reasons.iter().sum::<usize>(),
        functions,
        "every application function must have one parallel status"
    );
    assert_eq!(
        blocker_sets, functions,
        "every application function must have one complete blocker set"
    );
    let candidate_sites = report_number(parallel, "(candidate-sites ");
    let eligible_sites = report_number(parallel, "(eligible-sites ");
    assert_eq!(
        candidate_sites, eligible_sites,
        "eligible-site compatibility count must equal the complete candidate count"
    );
    ParallelismEvidence {
        source_bytes: fs::metadata(path)
            .expect("parallelism application metadata")
            .len() as usize,
        functions,
        checked_sites: report_number(integer, "(checked-site-count "),
        reported_total_sites: integer.matches("(status total)").count(),
        refinements: report_number(integer, "(refinements "),
        safe_functions,
        reasons,
        blockers,
        candidate_sites,
        selected_sites: report_number(parallel, "(selected-sites "),
        reported_sites: report_number(parallel, "(reported-sites "),
        executable_sites: report_number(parallel, "(executable-sites "),
        executed_sites: report_number(parallel, "(executed-sites "),
        eligible_sites,
    }
}

fn print_parallelism_evidence(challenge: &str, evidence: &ParallelismEvidence) {
    print!(
        "{challenge}\t{}\t{}\t{}\t{}\t{}\t{}",
        evidence.source_bytes,
        evidence.functions,
        evidence.checked_sites,
        evidence.reported_total_sites,
        evidence.refinements,
        evidence.safe_functions
    );
    for count in evidence.reasons {
        print!("\t{count}");
    }
    for count in evidence.blockers {
        print!("\t{count}");
    }
    println!(
        "\t{}\t{}\t{}\t{}\t{}\t{}",
        evidence.candidate_sites,
        evidence.selected_sites,
        evidence.reported_sites,
        evidence.executable_sites,
        evidence.executed_sites,
        evidence.eligible_sites
    );
}

fn parallelism_baseline() -> BTreeMap<String, ParallelismEvidence> {
    let path = repository_root().join("benchmarks/parallelism-baseline.tsv");
    let contents = fs::read_to_string(path).expect("read parallelism baseline");
    let mut baseline = BTreeMap::new();
    for (line_number, line) in contents.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        assert_eq!(
            columns.len(),
            35,
            "parallelism baseline line {} must have thirty-five columns",
            line_number + 1
        );
        let number = |index: usize| {
            columns[index].parse::<usize>().unwrap_or_else(|_| {
                panic!(
                    "invalid parallelism evidence on line {}, column {}",
                    line_number + 1,
                    index + 1
                )
            })
        };
        let mut reasons = [0; PARALLELISM_REASONS.len()];
        for (offset, value) in reasons.iter_mut().enumerate() {
            *value = number(offset + 7);
        }
        let mut blockers = [0; PARALLELISM_REASONS.len()];
        for (offset, value) in blockers.iter_mut().enumerate() {
            *value = number(offset + 18);
        }
        let evidence = ParallelismEvidence {
            source_bytes: number(1),
            functions: number(2),
            checked_sites: number(3),
            reported_total_sites: number(4),
            refinements: number(5),
            safe_functions: number(6),
            reasons,
            blockers,
            candidate_sites: number(29),
            selected_sites: number(30),
            reported_sites: number(31),
            executable_sites: number(32),
            executed_sites: number(33),
            eligible_sites: number(34),
        };
        assert!(
            baseline.insert(columns[0].to_owned(), evidence).is_none(),
            "duplicate parallelism baseline for {}",
            columns[0]
        );
    }
    baseline
}

fn parallelism_blockers(report: &str) -> (usize, [usize; PARALLELISM_REASONS.len()]) {
    let mut remaining = report;
    let mut sets = 0;
    let mut counts = [0; PARALLELISM_REASONS.len()];
    while let Some(start) = remaining.find("(blockers") {
        let blocker_tail = &remaining[start + "(blockers".len()..];
        let end = blocker_tail
            .find(')')
            .expect("parallel blocker set must be closed");
        let blockers = &blocker_tail[..end];
        for (index, (_, report_name)) in PARALLELISM_REASONS.iter().enumerate() {
            if blockers
                .split_ascii_whitespace()
                .any(|name| name == *report_name)
            {
                counts[index] += 1;
            }
        }
        sets += 1;
        remaining = &blocker_tail[end + 1..];
    }
    (sets, counts)
}

fn report_section<'a>(report: &'a str, start: &str, end: &str) -> &'a str {
    let start_index = report
        .find(start)
        .unwrap_or_else(|| panic!("analysis report is missing {start}"));
    let tail = &report[start_index..];
    let end_index = tail
        .find(end)
        .unwrap_or_else(|| panic!("analysis report is missing section terminator {end}"));
    &tail[..end_index]
}

fn report_number(report: &str, marker: &str) -> usize {
    let start = report
        .find(marker)
        .unwrap_or_else(|| panic!("analysis report is missing {marker}"))
        + marker.len();
    let digits = report[start..]
        .bytes()
        .take_while(u8::is_ascii_digit)
        .count();
    assert!(digits > 0, "analysis report has no number after {marker}");
    report[start..start + digits]
        .parse()
        .expect("analysis count must fit usize")
}

fn report_numbers(report: &str, marker: &str) -> Vec<usize> {
    let mut numbers = Vec::new();
    let mut remaining = report;
    while let Some(index) = remaining.find(marker) {
        let tail = &remaining[index + marker.len()..];
        let digits = tail.bytes().take_while(u8::is_ascii_digit).count();
        assert!(digits > 0, "analysis report has no number after {marker}");
        numbers.push(
            tail[..digits]
                .parse()
                .expect("analysis count must fit usize"),
        );
        remaining = &tail[digits..];
    }
    numbers
}

fn report_effect_counts(report: &str) -> (usize, usize, usize) {
    let mut remaining = report;
    let mut effectful = 0;
    let mut allocation = 0;
    let mut partial = 0;
    while let Some(index) = remaining.find("(effects") {
        let tail = &remaining[index + "(effects".len()..];
        let end = tail.find(')').expect("declared effects must be closed");
        let effects = tail[..end].split_ascii_whitespace().collect::<Vec<_>>();
        if !effects.is_empty() {
            effectful += 1;
        }
        if effects.contains(&"alloc") {
            allocation += 1;
        }
        if effects.contains(&"partial") {
            partial += 1;
        }
        remaining = &tail[end + 1..];
    }
    (effectful, allocation, partial)
}

fn report_parentheses_are_balanced(report: &[u8]) -> bool {
    let mut depth = 0_i64;
    let mut quoted = false;
    let mut escaped = false;
    for byte in report {
        if quoted {
            if escaped {
                escaped = false;
            } else if *byte == b'\\' {
                escaped = true;
            } else if *byte == b'"' {
                quoted = false;
            }
        } else if *byte == b'"' {
            quoted = true;
        } else if *byte == b'(' {
            depth += 1;
        } else if *byte == b')' {
            depth -= 1;
            if depth < 0 {
                return false;
            }
        }
    }
    depth == 0 && !quoted && !escaped
}

fn challenge_manifest() -> Vec<String> {
    let path = repository_root().join("benchmarks/challenges/manifest.tsv");
    let contents = fs::read_to_string(path).expect("read challenge manifest");
    let mut challenges = Vec::new();
    let mut seen = BTreeSet::new();
    for (line_number, line) in contents.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        assert_eq!(
            columns.len(),
            2,
            "challenge manifest line {} must have two columns",
            line_number + 1
        );
        assert!(
            seen.insert(columns[0].to_owned()),
            "duplicate challenge {}",
            columns[0]
        );
        challenges.push(columns[0].to_owned());
    }
    assert!(
        !challenges.is_empty(),
        "challenge manifest must not be empty"
    );
    challenges
}

fn challenge_has_feature(challenge: &str, feature: &str) -> bool {
    let path = repository_root().join("benchmarks/challenges/manifest.tsv");
    let contents = fs::read_to_string(path).expect("read challenge manifest");
    for line in contents.lines() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() == 2 && columns[0] == challenge {
            return columns[1].split(',').any(|candidate| candidate == feature);
        }
    }
    panic!("challenge {challenge} is absent from the manifest")
}

fn performance_budget(metric: &str, workload: &str) -> f64 {
    let path = repository_root().join("benchmarks/performance-budgets.tsv");
    let contents = fs::read_to_string(path).expect("read performance budgets");
    let mut budgets = BTreeMap::new();
    for (line_number, line) in contents.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        assert_eq!(
            columns.len(),
            5,
            "performance budget line {} must have five columns",
            line_number + 1
        );
        let key = (columns[0].to_owned(), columns[1].to_owned());
        let value = columns[2]
            .parse::<f64>()
            .unwrap_or_else(|_| panic!("invalid performance limit on line {}", line_number + 1));
        assert!(
            budgets.insert(key, value).is_none(),
            "duplicate performance budget"
        );
    }
    budgets
        .get(&(metric.to_owned(), workload.to_owned()))
        .copied()
        .unwrap_or_else(|| panic!("missing performance budget {metric}/{workload}"))
}

fn run_agent() {
    let root = repository_root();
    let build = TemporaryDirectory::new("agent");
    println!(
        "case\tlanguage\tbroken_bytes\tfixed_bytes\tbroken_lexical_tokens\tfixed_lexical_tokens\tbroken_model_token_proxy\tfixed_model_token_proxy\tremoved_bytes\tinserted_bytes\tdiagnostic_bytes\tbroken_accepted\tfixed_accepted\tbroken_feedback_us\tfixed_feedback_us\tsafety_outcome"
    );
    for (case, safety_outcome) in agent_manifest() {
        for (language, extension) in [("slim", "slim"), ("c", "c"), ("rust", "rs")] {
            let directory = root
                .join("benchmarks/agent/cases")
                .join(&case)
                .join(language);
            let broken_path = directory.join(format!("broken.{extension}"));
            let fixed_path = directory.join(format!("fixed.{extension}"));
            let broken = fs::read(&broken_path).expect("read broken agent fixture");
            let fixed = fs::read(&fixed_path).expect("read fixed agent fixture");
            let (removed_bytes, inserted_bytes) = changed_span(&broken, &fixed);
            let (broken_feedback, broken_output) = median_agent_feedback(
                &root,
                language,
                &broken_path,
                &build.path,
                &format!("{case}-broken"),
            );
            let (fixed_feedback, fixed_output) = median_agent_feedback(
                &root,
                language,
                &fixed_path,
                &build.path,
                &format!("{case}-fixed"),
            );
            if broken_output.status.success() || !fixed_output.status.success() {
                eprintln!(
                    "agent fixture invariant failed for {case}/{language}: broken={} fixed={}\nbroken stdout:\n{}\nbroken stderr:\n{}\nfixed stdout:\n{}\nfixed stderr:\n{}",
                    broken_output.status,
                    fixed_output.status,
                    String::from_utf8_lossy(&broken_output.stdout),
                    String::from_utf8_lossy(&broken_output.stderr),
                    String::from_utf8_lossy(&fixed_output.stdout),
                    String::from_utf8_lossy(&fixed_output.stderr),
                );
                std::process::exit(1);
            }
            let diagnostic_bytes = broken_output.stdout.len() + broken_output.stderr.len();
            println!(
                "{case}\t{language}\t{}\t{}\t{}\t{}\t{}\t{}\t{removed_bytes}\t{inserted_bytes}\t{diagnostic_bytes}\tfalse\ttrue\t{}\t{}\t{safety_outcome}",
                broken.len(),
                fixed.len(),
                neutral_lexical_tokens(&broken),
                neutral_lexical_tokens(&fixed),
                broken.len().div_ceil(4),
                fixed.len().div_ceil(4),
                broken_feedback.as_micros(),
                fixed_feedback.as_micros(),
            );
        }
    }
}

fn agent_manifest() -> Vec<(String, String)> {
    let path = repository_root().join("benchmarks/agent/manifest.tsv");
    let contents = fs::read_to_string(path).expect("read agent manifest");
    let mut cases = Vec::new();
    let mut seen = BTreeSet::new();
    for (line_number, line) in contents.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        assert_eq!(
            columns.len(),
            2,
            "agent manifest line {} must have two columns",
            line_number + 1
        );
        assert!(
            seen.insert(columns[0]),
            "duplicate agent case {}",
            columns[0]
        );
        cases.push((columns[0].to_owned(), columns[1].to_owned()));
    }
    assert!(!cases.is_empty(), "agent manifest must not be empty");
    cases
}

fn median_agent_feedback(
    root: &Path,
    language: &str,
    source: &Path,
    build: &Path,
    role: &str,
) -> (Duration, Output) {
    let mut samples = Vec::with_capacity(5);
    let mut representative = None;
    for sample in 0..5 {
        let mut command = agent_check_command(root, language, source, build, role, sample);
        let (elapsed, output) = timed_output(&mut command, "agent compiler feedback");
        if representative.is_none() {
            representative = Some(output);
        }
        samples.push(elapsed);
    }
    samples.sort();
    (
        samples[samples.len() / 2],
        representative.expect("agent benchmark output"),
    )
}

fn agent_check_command(
    root: &Path,
    language: &str,
    source: &Path,
    build: &Path,
    role: &str,
    sample: usize,
) -> Command {
    match language {
        "slim" => {
            let mut command = Command::new(root.join("slimc"));
            command.arg("check").arg(source);
            command
        }
        "c" => {
            let mut command = Command::new(native_compiler());
            command
                .current_dir(source.parent().expect("C fixture directory"))
                .arg("-std=c11")
                .arg("-Wall")
                .arg("-Wextra")
                .arg("-Werror")
                .arg("-fsyntax-only")
                .arg(source.file_name().expect("C fixture name"));
            command
        }
        "rust" => {
            let mut command = Command::new("rustc");
            command
                .current_dir(source.parent().expect("Rust fixture directory"))
                .arg("--edition=2024")
                .arg("-D")
                .arg("warnings")
                .arg("--emit=metadata")
                .arg(source.file_name().expect("Rust fixture name"))
                .arg("-o")
                .arg(build.join(format!("{role}-{sample}.rmeta")));
            command
        }
        _ => unreachable!("agent languages are fixed"),
    }
}

fn changed_span(before: &[u8], after: &[u8]) -> (usize, usize) {
    let prefix = before
        .iter()
        .zip(after)
        .take_while(|(left, right)| left == right)
        .count();
    let remaining_before = before.len() - prefix;
    let remaining_after = after.len() - prefix;
    let suffix = before[prefix..]
        .iter()
        .rev()
        .zip(after[prefix..].iter().rev())
        .take(remaining_before.min(remaining_after))
        .take_while(|(left, right)| left == right)
        .count();
    (remaining_before - suffix, remaining_after - suffix)
}

fn neutral_lexical_tokens(source: &[u8]) -> usize {
    let mut index = 0;
    let mut tokens = 0;
    while index < source.len() {
        let byte = source[index];
        if byte.is_ascii_whitespace() {
            index += 1;
        } else if byte.is_ascii_alphanumeric() || byte == b'_' {
            tokens += 1;
            index += 1;
            while index < source.len()
                && (source[index].is_ascii_alphanumeric() || source[index] == b'_')
            {
                index += 1;
            }
        } else if byte == b'"' {
            tokens += 1;
            index += 1;
            while index < source.len() {
                if source[index] == b'\\' && index + 1 < source.len() {
                    index += 2;
                } else if source[index] == b'"' {
                    index += 1;
                    break;
                } else {
                    index += 1;
                }
            }
        } else {
            tokens += 1;
            index += 1;
        }
    }
    tokens
}

fn has_quick_flag() -> bool {
    std::env::args().any(|argument| argument == "--quick")
}

fn native_compiler() -> std::ffi::OsString {
    std::env::var_os("CC").unwrap_or_else(|| "cc".into())
}

fn compiler_output(compiler: &Path, command: &str, source: &Path) -> Output {
    Command::new(compiler)
        .arg(command)
        .arg(source)
        .output()
        .expect("run SLIM compiler")
}

fn timed_output(command: &mut Command, role: &str) -> (Duration, Output) {
    let start = Instant::now();
    let output = command
        .output()
        .unwrap_or_else(|error| panic!("cannot run {role}: {error}"));
    (start.elapsed(), output)
}

fn timed_success(command: &mut Command, role: &str) -> Duration {
    let (elapsed, output) = timed_output(command, role);
    if !output.status.success() {
        fail_output(role, &output);
    }
    elapsed
}

fn require_success(command: &mut Command, role: &str) {
    let output = command
        .output()
        .unwrap_or_else(|error| panic!("cannot run {role}: {error}"));
    if !output.status.success() {
        fail_output(role, &output);
    }
}

fn require_clean_output(output: Output, role: &str) {
    if !output.status.success() || !output.stdout.is_empty() || !output.stderr.is_empty() {
        fail_output(role, &output);
    }
}

fn require_transform_output(output: Output, role: &str) -> Vec<u8> {
    if !output.status.success() || output.stdout.is_empty() || !output.stderr.is_empty() {
        fail_output(role, &output);
    }
    output.stdout
}

fn fail_output(role: &str, output: &Output) -> ! {
    eprintln!(
        "{role} failed with {}\nstdout:\n{}\nstderr:\n{}",
        output.status,
        String::from_utf8_lossy(&output.stdout),
        String::from_utf8_lossy(&output.stderr)
    );
    std::process::exit(1);
}

fn run_output(executable: &Path, arguments: &[PathBuf]) -> Vec<u8> {
    run_output_with_env(executable, arguments, &[])
}

fn run_output_with_env(
    executable: &Path,
    arguments: &[PathBuf],
    environment: &[(&str, &str)],
) -> Vec<u8> {
    let output = Command::new(executable)
        .args(arguments)
        .envs(environment.iter().copied())
        .output()
        .expect("benchmark executable");
    if !output.status.success() {
        fail_output(&format!("run {}", executable.display()), &output);
    }
    output.stdout
}

fn median_runtime(executable: &Path, arguments: &[PathBuf], samples: usize) -> Duration {
    median_runtime_with_env(executable, arguments, &[], samples)
}

fn median_runtime_with_env(
    executable: &Path,
    arguments: &[PathBuf],
    environment: &[(&str, &str)],
    samples: usize,
) -> Duration {
    let _ = run_output_with_env(executable, arguments, environment);
    let mut times = Vec::with_capacity(samples);
    for _ in 0..samples {
        let start = Instant::now();
        black_box(run_output_with_env(executable, arguments, environment));
        times.push(start.elapsed());
    }
    times.sort();
    times[samples / 2]
}

fn generated_program(declarations: usize) -> String {
    let mut source = String::with_capacity(declarations * 80);
    source.push_str("(module scaling ");
    for index in 0..declarations {
        source.push_str("(fn function-");
        source.push_str(&index.to_string());
        if index == 0 {
            source.push_str(" ((value I64)) I64 (effects) (call i64.add value 0)) ");
        } else {
            source.push_str(" ((value I64)) I64 (effects) (call function-0 value)) ");
        }
    }
    source.push_str("(fn main ((args (Vec Bytes))) I64 (effects) (call function-0 0)))\n");
    source
}

fn generated_nested_program(bindings: usize) -> String {
    let mut source = String::with_capacity(bindings * 58);
    source.push_str(
        "(module nested (fn identity ((value I64)) I64 (effects) value) (fn deep ((seed I64)) I64 (effects) ",
    );
    for index in 0..bindings {
        source.push_str("(let value-");
        source.push_str(&index.to_string());
        source.push_str(" I64 (call identity seed) ");
    }
    source.push_str("seed");
    for _ in 0..bindings {
        source.push(')');
    }
    source.push_str(") (fn main ((args (Vec Bytes))) I64 (effects) (call deep 0)))\n");
    source
}

fn generated_computed_argument_program(calls: usize) -> String {
    let mut source = String::with_capacity(calls * 16);
    source.push_str(
        "(module computed-arguments (fn identity ((value I64)) I64 (effects) value) (fn chain ((seed I64)) I64 (effects) ",
    );
    for _ in 0..calls {
        source.push_str("(call identity ");
    }
    source.push_str("seed");
    for _ in 0..calls {
        source.push(')');
    }
    source.push_str(") (fn main ((args (Vec Bytes))) I64 (effects) (call chain 0)))\n");
    source
}

fn generated_aggregate_temporary_program(fields: usize) -> String {
    let mut source = String::with_capacity(fields * 110);
    source.push_str("(module aggregate-temporaries (record Wide (");
    for index in 0..fields {
        source.push_str("(field-");
        source.push_str(&index.to_string());
        source.push_str(" I64)");
    }
    source.push_str(")) (variant Payload ((Values");
    for _ in 0..fields {
        source.push_str(" I64");
    }
    source.push_str(
        "))) (fn identity ((value I64)) I64 (effects) value) (fn build () I64 (effects) (let record-value Wide (make Wide ",
    );
    for index in 0..fields {
        source.push_str("(field-");
        source.push_str(&index.to_string());
        source.push_str(" (call identity ");
        source.push_str(&index.to_string());
        source.push_str("))");
    }
    source.push_str(" ) (let variant-value Payload (case Payload Values");
    for index in 0..fields {
        source.push_str(" (call identity ");
        source.push_str(&index.to_string());
        source.push(')');
    }
    source.push_str(") 0))) (fn main ((args (Vec Bytes))) I64 (effects) (call build)))\n");
    source
}

fn generated_planned_allocation_call_program(calls: usize) -> String {
    let mut source = String::with_capacity(calls * 17);
    source.push_str(
        "(module planned-allocation-calls (fn allocate ((value I64)) I64 (effects alloc) (let values (Vec I64) (call vec.new) value)) (fn chain ((seed I64)) I64 (effects alloc) ",
    );
    for _ in 0..calls {
        source.push_str("(call allocate ");
    }
    source.push_str("seed");
    for _ in 0..calls {
        source.push(')');
    }
    source.push_str(") (fn main ((args (Vec Bytes))) I64 (effects alloc) (call chain 0)))\n");
    source
}

fn generated_inout_read_program(parameters: usize) -> String {
    let mut source = String::with_capacity(parameters * 58);
    source.push_str("(module inout-reads (fn read (");
    for index in 0..parameters {
        source.push_str("(inout value-");
        source.push_str(&index.to_string());
        source.push_str(" I64)");
    }
    source.push_str(") I64 (effects partial) ");
    for index in 0..parameters {
        source.push_str("(call i64.add value-");
        source.push_str(&index.to_string());
        source.push(' ');
    }
    source.push('0');
    for _ in 0..parameters {
        source.push(')');
    }
    source.push_str(") (fn main ((args (Vec Bytes))) I64 (effects) 0))\n");
    source
}

fn generated_named_type_program(functions: usize) -> String {
    let mut source = String::with_capacity(functions * 70);
    source.push_str("(module named-types ");
    for index in 0..functions {
        source.push_str("(fn worker-");
        source.push_str(&index.to_string());
        source.push_str(" ((value Payload)) I64 (effects) 0) ");
    }
    source.push_str(
        "(record Payload ((bytes Bytes))) (fn main ((args (Vec Bytes))) I64 (effects) 0))\n",
    );
    source
}

fn generated_owned_transfer_program(transfers: usize) -> String {
    let mut source = String::with_capacity(transfers * 100);
    source.push_str(
        "(module owned-transfers (fn consume ((value (Vec I64))) Unit (effects) unit) (fn transfer (",
    );
    for index in 0..transfers {
        source.push_str("(value-");
        source.push_str(&index.to_string());
        source.push_str(" (Vec I64))");
    }
    source.push_str(") I64 (effects) ");
    for index in 0..transfers {
        source.push_str("(let moved-");
        source.push_str(&index.to_string());
        source.push_str(" Unit (call consume value-");
        source.push_str(&index.to_string());
        source.push_str(") ");
    }
    source.push('0');
    for _ in 0..transfers {
        source.push(')');
    }
    source.push_str(") (fn main ((args (Vec Bytes))) I64 (effects) 0))\n");
    source
}

#[derive(Clone, Copy)]
enum ProjectGraph {
    Wide,
    Deep,
}

impl ProjectGraph {
    const fn name(self) -> &'static str {
        match self {
            Self::Wide => "wide",
            Self::Deep => "deep",
        }
    }
}

#[derive(Clone, Copy)]
enum IncrementalScenario {
    NoChange,
    PrivateBody,
    PublicInterface,
}

impl IncrementalScenario {
    const fn name(self) -> &'static str {
        match self {
            Self::NoChange => "no-change",
            Self::PrivateBody => "private-body",
            Self::PublicInterface => "public-interface",
        }
    }
}

struct GeneratedProject {
    directory: TemporaryDirectory,
}

impl GeneratedProject {
    fn new(graph: ProjectGraph, modules: usize, role: &str) -> Self {
        assert!(modules > 0);
        let directory = TemporaryDirectory::new(role);
        write_project(&directory.path, graph, modules);
        Self { directory }
    }

    fn manifest(&self) -> PathBuf {
        self.directory.path.join("slim.project")
    }
}

struct SnapshotPair {
    directory: TemporaryDirectory,
}

impl SnapshotPair {
    fn new(graph: ProjectGraph, modules: usize, scenario: IncrementalScenario) -> Self {
        let directory = TemporaryDirectory::new("incremental");
        let initial = directory.path.join("initial");
        let updated = directory.path.join("updated");
        write_project(&initial, graph, modules);
        copy_tree(&initial, &updated);
        match scenario {
            IncrementalScenario::NoChange => {}
            IncrementalScenario::PrivateBody => {
                replace_in_first_module(&updated, "input 1", "input 2")
            }
            IncrementalScenario::PublicInterface => {
                replace_in_first_module(&updated, "((value I64))", "((value I64) (valid Bool))")
            }
        }
        Self { directory }
    }

    fn initial_manifest(&self) -> PathBuf {
        self.directory.path.join("initial/slim.project")
    }

    fn updated_manifest(&self) -> PathBuf {
        self.directory.path.join("updated/slim.project")
    }
}

fn write_project(root: &Path, graph: ProjectGraph, modules: usize) {
    fs::create_dir_all(root).expect("create project benchmark directory");
    let mut manifest = String::from("(project 1 (entry app) ");
    let app_imports = match graph {
        ProjectGraph::Wide => (0..modules).map(module_name).collect::<Vec<_>>().join(" "),
        ProjectGraph::Deep => module_name(modules - 1),
    };
    manifest.push_str(&format!(
        "(module app \"app.slim\" (imports {app_imports}) (exports)) "
    ));
    let app_dependency = match graph {
        ProjectGraph::Wide => module_name(0),
        ProjectGraph::Deep => module_name(modules - 1),
    };
    fs::write(
        root.join("app.slim"),
        format!(
            "(module app (fn main ((args (Vec Bytes))) I64 (effects) (call {app_dependency}/value 40)))\n"
        ),
    )
    .expect("write benchmark entry module");
    for index in 0..modules {
        let module = module_name(index);
        let imports = match graph {
            ProjectGraph::Wide => String::new(),
            ProjectGraph::Deep if index > 0 => module_name(index - 1),
            ProjectGraph::Deep => String::new(),
        };
        manifest.push_str(&format!(
            "(module {module} \"{module}.slim\" (imports {imports}) (exports Marker value)) "
        ));
        let body = match graph {
            ProjectGraph::Deep if index > 0 => {
                format!("(call {}/value input)", module_name(index - 1))
            }
            ProjectGraph::Wide | ProjectGraph::Deep => "(call i64.add input 1)".to_owned(),
        };
        fs::write(
            root.join(format!("{module}.slim")),
            format!(
                "(module {module} (record Marker ((value I64))) (fn value ((input I64)) I64 (effects) {body}))\n"
            ),
        )
        .expect("write benchmark module");
    }
    manifest.push_str(")\n");
    fs::write(root.join("slim.project"), manifest).expect("write benchmark manifest");
}

fn replace_in_first_module(root: &Path, before: &str, after: &str) {
    let path = root.join(format!("{}.slim", module_name(0)));
    let source = fs::read_to_string(&path).expect("read edited benchmark module");
    let edited = source.replacen(before, after, 1);
    assert_ne!(source, edited, "benchmark edit marker must exist");
    fs::write(path, edited).expect("write edited benchmark module");
}

fn copy_tree(source: &Path, destination: &Path) {
    fs::create_dir_all(destination).expect("create project snapshot");
    for entry in fs::read_dir(source).expect("read project snapshot") {
        let entry = entry.expect("read project entry");
        let target = destination.join(entry.file_name());
        if entry.path().is_dir() {
            copy_tree(&entry.path(), &target);
        } else {
            fs::copy(entry.path(), target).expect("copy project entry");
        }
    }
}

fn module_name(index: usize) -> String {
    format!("m{index:05}")
}

struct TemporaryDirectory {
    path: PathBuf,
}

impl TemporaryDirectory {
    fn new(role: &str) -> Self {
        let serial = TEMPORARY_SERIAL.fetch_add(1, Ordering::Relaxed);
        let path =
            std::env::temp_dir().join(format!("slim-bench-{role}-{}-{serial}", std::process::id()));
        let _ = fs::remove_dir_all(&path);
        fs::create_dir_all(&path).expect("create benchmark directory");
        Self { path }
    }
}

impl Drop for TemporaryDirectory {
    fn drop(&mut self) {
        let _ = fs::remove_dir_all(&self.path);
    }
}

struct ComparisonBuild {
    path: PathBuf,
}

impl ComparisonBuild {
    fn new() -> Self {
        let directory = TemporaryDirectory::new("comparison");
        let path = directory.path.clone();
        std::mem::forget(directory);
        let input: Vec<u8> = (0..8 * 1024 * 1024)
            .map(|index| (index & 0xff) as u8)
            .collect();
        fs::write(path.join("input.bin"), input).expect("write comparison input");
        Self { path }
    }
}

impl Drop for ComparisonBuild {
    fn drop(&mut self) {
        let _ = fs::remove_dir_all(&self.path);
    }
}

static TEMPORARY_SERIAL: AtomicU64 = AtomicU64::new(0);

#[cfg(test)]
mod tests {
    use super::{
        Duration, balanced_round_count, changed_span, median_duration, neutral_lexical_tokens,
    };

    #[test]
    fn edit_span_removes_shared_context() {
        assert_eq!(
            changed_span(b"left mystery right", b"left false right"),
            (7, 5)
        );
        assert_eq!(changed_span(b"same", b"same"), (0, 0));
    }

    #[test]
    fn neutral_lexer_is_language_independent_and_deterministic() {
        assert_eq!(neutral_lexical_tokens(b"call(bool.and, value)"), 8);
        assert_eq!(neutral_lexical_tokens(b"\"one token\" + name_1"), 3);
    }

    #[test]
    fn generated_emit_sampling_balances_both_series_directions() {
        assert_eq!(balanced_round_count(5), 6);
        assert_eq!(balanced_round_count(8), 8);
        let mut times = [
            Duration::from_millis(40),
            Duration::from_millis(10),
            Duration::from_millis(30),
            Duration::from_millis(20),
        ];
        assert_eq!(median_duration(&mut times), Duration::from_millis(25));
    }
}
