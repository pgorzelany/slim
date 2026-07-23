use std::collections::{BTreeMap, BTreeSet};
use std::fs;
use std::hint::black_box;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};
use std::sync::atomic::{AtomicU64, Ordering};
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
        "agent" => run_agent(),
        _ => {
            eprintln!(
                "usage: slim-bench <performance [--quick] | reduction [--quick] | incremental [--quick] | project [--quick] | compare [--quick] | agent>"
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

    println!("computed_arguments\tsource_bytes\temit_us\temit_ns_per_byte");
    let mut first_computed = None;
    let mut last_computed = None;
    for size in nested_sizes {
        let source = generated_computed_argument_program(*size);
        let path = directory
            .path
            .join(format!("computed-arguments-{size}.slim"));
        fs::write(&path, &source).expect("write computed-argument scaling source");
        let warmup = Command::new(&compiler)
            .arg(&path)
            .output()
            .expect("run computed-argument scaling warmup");
        if !warmup.status.success() || !warmup.stderr.is_empty() || warmup.stdout.is_empty() {
            fail_output("computed-argument scaling warmup", &warmup);
        }
        let mut times = Vec::with_capacity(samples);
        for _ in 0..samples {
            let (elapsed, output) = timed_output(
                Command::new(&compiler).arg(&path),
                "SLIM computed-argument emit",
            );
            if !output.status.success() || !output.stderr.is_empty() || output.stdout.is_empty() {
                fail_output("computed-argument scaling emit", &output);
            }
            black_box(&output.stdout);
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
        if first_computed.is_none() {
            first_computed = Some((*size, elapsed));
        }
        last_computed = Some((*size, elapsed));
    }
    let (first_size, first_time) =
        first_computed.expect("computed-argument series has a first sample");
    let (last_size, last_time) = last_computed.expect("computed-argument series has a last sample");
    let size_ratio = last_size as f64 / first_size as f64;
    let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
    let exponent = time_ratio.ln() / size_ratio.ln();
    let limit = performance_budget("emit-exponent", "generated-computed-arguments");
    if exponent > limit {
        eprintln!(
            "performance gate: computed-argument emit exponent {exponent:.3} exceeds {limit:.3} between {first_size} and {last_size} calls"
        );
        std::process::exit(1);
    }

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
        let size_ratio = last_size as f64 / first_size as f64;
        let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
        let exponent = time_ratio.ln() / size_ratio.ln();
        if exponent > 1.25 {
            eprintln!(
                "{role} scaling gate: process-level exponent {exponent:.3} exceeds 1.25 between {first_size} and {last_size} declarations"
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
    let output = Command::new(executable)
        .args(arguments)
        .output()
        .expect("benchmark executable");
    if !output.status.success() {
        fail_output(&format!("run {}", executable.display()), &output);
    }
    output.stdout
}

fn median_runtime(executable: &Path, arguments: &[PathBuf], samples: usize) -> Duration {
    let _ = run_output(executable, arguments);
    let mut times = Vec::with_capacity(samples);
    for _ in 0..samples {
        let start = Instant::now();
        black_box(run_output(executable, arguments));
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
    use super::{changed_span, neutral_lexical_tokens};

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
}
