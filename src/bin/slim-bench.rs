use std::fs;
use std::hint::black_box;
use std::path::PathBuf;
use std::process::Command;
use std::time::{Duration, Instant};

use slim::compiler;
use slim::incremental::{IncrementalSession, WorkStats};
use slim::project;
use slim::project::session::{ProjectSession, ProjectWorkStats};
use slim::span::Source;

fn main() {
    let command = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "scaling".to_owned());
    match command.as_str() {
        "scaling" => run_scaling(),
        "incremental" => run_incremental(),
        "project" => run_project(),
        "compare" => run_comparison(),
        _ => {
            eprintln!(
                "usage: slim-bench <scaling [--quick] | incremental [--quick] | project [--quick] | compare>"
            );
            std::process::exit(64);
        }
    }
}

fn run_scaling() {
    let quick = std::env::args().any(|argument| argument == "--quick");
    let sizes: &[usize] = if quick {
        &[250, 500, 1_000, 2_000]
    } else {
        &[1_000, 2_000, 4_000, 8_000]
    };
    let samples = if quick { 5 } else { 9 };

    println!("declarations\tbytes\tcheck_us\temit_us\tcheck_ns_per_byte");
    let mut first: Option<(usize, Duration)> = None;
    let mut last: Option<(usize, Duration)> = None;
    for size in sizes {
        let source_text = generated_project(*size);
        let path = PathBuf::from(format!("generated-{size}.slim"));
        let _ = measure(&source_text, &path);
        let mut check_times = Vec::new();
        let mut emit_times = Vec::new();
        for _ in 0..samples {
            let start = Instant::now();
            let compilation = compiler::compile(Source::new(&path, source_text.clone()));
            let check_elapsed = start.elapsed();
            assert!(compilation.succeeded(), "generated project must check");
            let start = Instant::now();
            black_box(compilation.emit_c().unwrap());
            let emit_elapsed = start.elapsed();
            check_times.push(check_elapsed);
            emit_times.push(emit_elapsed);
        }
        check_times.sort();
        emit_times.sort();
        let check = check_times[samples / 2];
        let emit = emit_times[samples / 2];
        let per_byte = check.as_nanos() as f64 / source_text.len() as f64;
        println!(
            "{size}\t{}\t{}\t{}\t{per_byte:.2}",
            source_text.len(),
            check.as_micros(),
            emit.as_micros()
        );
        if first.is_none() {
            first = Some((*size, check));
        }
        last = Some((*size, check));
    }
    let (first_size, first_time) = first.expect("scaling benchmark has a first sample");
    let (last_size, last_time) = last.expect("scaling benchmark has a last sample");
    let size_ratio = last_size as f64 / first_size as f64;
    let time_ratio = last_time.as_nanos() as f64 / first_time.as_nanos() as f64;
    let exponent = time_ratio.ln() / size_ratio.ln();
    if exponent > 1.25 {
        eprintln!(
            "scaling gate: end-to-end exponent {exponent:.3} exceeds 1.25 between {first_size} and {last_size} declarations"
        );
        std::process::exit(1);
    }
}

fn run_comparison() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = std::env::current_exe()
        .expect("current benchmark executable")
        .with_file_name("slimc");
    if !compiler.is_file() {
        eprintln!(
            "comparison requires {} next to slim-bench; build both with --release",
            compiler.display()
        );
        std::process::exit(2);
    }
    let build = ComparisonBuild::new();
    let challenges = [
        "gcd_fib",
        "sieve",
        "bfs",
        "matrix",
        "merge_sort",
        "bytefreq",
    ];
    println!("challenge\tlanguage\tcompile_ms\truntime_us\tbinary_bytes");
    for challenge in challenges {
        let directory = root.join("benchmarks/challenges").join(challenge);
        let slim_output = build.path.join(format!("{challenge}-slim"));
        let c_output = build.path.join(format!("{challenge}-c"));
        let rust_output = build.path.join(format!("{challenge}-rust"));

        let slim_compile = timed_command(
            Command::new(&compiler)
                .arg("build")
                .arg(directory.join("program.slim"))
                .arg("-o")
                .arg(&slim_output),
        );
        let c_compile = timed_command(
            Command::new("clang")
                .arg("-std=c11")
                .arg("-O3")
                .arg("-Wall")
                .arg("-Wextra")
                .arg("-Werror")
                .arg(directory.join("program.c"))
                .arg("-o")
                .arg(&c_output),
        );
        let rust_compile = timed_command(
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

        for (language, executable, compile_time) in [
            ("slim", &slim_output, slim_compile),
            ("c", &c_output, c_compile),
            ("rust", &rust_output, rust_compile),
        ] {
            let runtime = median_runtime(executable, &arguments, 15);
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

fn run_incremental() {
    let quick = std::env::args().any(|argument| argument == "--quick");
    let sizes: &[usize] = if quick {
        &[250, 500, 1_000, 2_000]
    } else {
        &[1_000, 2_000, 4_000, 8_000]
    };
    let samples = if quick { 3 } else { 5 };
    println!("declarations\tscenario\tupdate_us\tparsed\tlowered\tchecked\tgenerated\treused");
    for size in sizes {
        let declaration_count = size + 1;
        let central = size / 2;
        for scenario in [
            "cold",
            "no-change",
            "body",
            "leaf-interface",
            "central-interface",
        ] {
            let mut times = Vec::with_capacity(samples);
            let mut observed = None;
            for _ in 0..samples {
                let base = generated_incremental_project(*size, 1, None);
                let mut session = IncrementalSession::new();
                if scenario != "cold" {
                    let initial = session.update(Source::new("incremental.slim", base.clone()));
                    assert!(initial.succeeded(), "generated project must check");
                }
                let edited = match scenario {
                    "cold" | "no-change" => base,
                    "body" => generated_incremental_project(*size, 1_000, None),
                    "leaf-interface" => generated_incremental_project(*size, 1, Some(size - 1)),
                    "central-interface" => generated_incremental_project(*size, 1, Some(central)),
                    _ => unreachable!(),
                };
                let start = Instant::now();
                let update = session.update(Source::new("incremental.slim", edited));
                let elapsed = start.elapsed();
                assert!(
                    update.succeeded(),
                    "incremental benchmark update must check"
                );
                black_box(update.emit_c());
                validate_incremental_work(
                    scenario,
                    *size,
                    central,
                    declaration_count,
                    &update.stats,
                );
                if let Some(previous) = &observed {
                    assert_eq!(previous, &update.stats, "work counts must be deterministic");
                } else {
                    observed = Some(update.stats.clone());
                }
                times.push(elapsed);
            }
            times.sort();
            let elapsed = times[samples / 2];
            let work = observed.expect("at least one sample");
            println!(
                "{declaration_count}\t{scenario}\t{}\t{}\t{}\t{}\t{}\t{}",
                elapsed.as_micros(),
                work.parsed,
                work.lowered,
                work.checked,
                work.generated,
                work.reused
            );
        }
    }
}

fn run_project() {
    let quick = std::env::args().any(|argument| argument == "--quick");
    let sizes: &[usize] = if quick { &[4, 8, 16] } else { &[8, 16, 32, 64] };
    let samples = if quick { 2 } else { 5 };
    println!(
        "graph\tmodules\tdeclarations\tscenario\tjobs\tmax_layer_width\tupdate_us\tmodules_read\tmodules_parsed\tmodules_lowered\tmodules_checked\tmodules_generated\tmodules_reused\tdeclarations_parsed\tdeclarations_lowered\tdeclarations_checked\tdeclarations_generated\tdeclarations_reused\tinvalidation_closure\tpersistent_hits\tpersistent_misses"
    );
    for graph in [ProjectGraph::Wide, ProjectGraph::Deep] {
        for modules in sizes {
            let declarations = modules * 2 + 1;
            for scenario in [
                ProjectScenario::Cold,
                ProjectScenario::NoChange,
                ProjectScenario::PrivateBody,
                ProjectScenario::PublicInterface,
                ProjectScenario::PersistentWarm,
            ] {
                let (elapsed, stats) = measure_project_scenario(graph, *modules, scenario, samples);
                validate_project_work(graph, *modules, declarations, scenario, &stats);
                print_project_row(
                    graph,
                    *modules,
                    declarations,
                    scenario.name(),
                    1,
                    elapsed,
                    &stats,
                );
            }
            let mut oracle = None;
            for jobs in [1, 2, 4] {
                let (elapsed, output) = measure_project_jobs(graph, *modules, jobs, samples);
                if let Some(expected) = &oracle {
                    assert_eq!(
                        expected, &output,
                        "worker-count output must be deterministic"
                    );
                } else {
                    oracle = Some(output);
                }
                let stats = ProjectWorkStats {
                    modules_read: *modules + 1,
                    modules_parsed: *modules + 1,
                    modules_lowered: *modules + 1,
                    modules_checked: *modules + 1,
                    modules_generated: *modules + 1,
                    declarations_parsed: declarations,
                    declarations_lowered: declarations,
                    declarations_checked: declarations,
                    declarations_generated: declarations,
                    invalidation_closure: declarations,
                    ..ProjectWorkStats::default()
                };
                print_project_row(
                    graph,
                    *modules,
                    declarations,
                    "clean-workers",
                    jobs,
                    elapsed,
                    &stats,
                );
            }
        }
    }
}

#[derive(Clone, Copy, Debug)]
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

#[derive(Clone, Copy, Debug)]
enum ProjectScenario {
    Cold,
    NoChange,
    PrivateBody,
    PublicInterface,
    PersistentWarm,
}

impl ProjectScenario {
    const fn name(self) -> &'static str {
        match self {
            Self::Cold => "cold",
            Self::NoChange => "no-change",
            Self::PrivateBody => "private-body",
            Self::PublicInterface => "public-interface",
            Self::PersistentWarm => "persistent-warm",
        }
    }
}

fn measure_project_scenario(
    graph: ProjectGraph,
    modules: usize,
    scenario: ProjectScenario,
    samples: usize,
) -> (Duration, ProjectWorkStats) {
    let mut times = Vec::with_capacity(samples);
    let mut observed = None;
    for _ in 0..samples {
        let project = GeneratedProject::new(graph, modules);
        let (elapsed, compilation) = match scenario {
            ProjectScenario::Cold => {
                let mut session = ProjectSession::new();
                let start = Instant::now();
                let compilation = session.update(project.source());
                (start.elapsed(), compilation)
            }
            ProjectScenario::NoChange => {
                let mut session = ProjectSession::new();
                assert!(session.update(project.source()).succeeded());
                let start = Instant::now();
                let compilation = session.update(project.source());
                (start.elapsed(), compilation)
            }
            ProjectScenario::PrivateBody => {
                let mut session = ProjectSession::new();
                assert!(session.update(project.source()).succeeded());
                project.edit_private_body();
                let start = Instant::now();
                let compilation = session.update(project.source());
                (start.elapsed(), compilation)
            }
            ProjectScenario::PublicInterface => {
                let mut session = ProjectSession::new();
                assert!(session.update(project.source()).succeeded());
                project.edit_public_interface();
                let start = Instant::now();
                let compilation = session.update(project.source());
                (start.elapsed(), compilation)
            }
            ProjectScenario::PersistentWarm => {
                let cache = project.root.join("cache/v1");
                let mut writer = ProjectSession::with_cache_directory(cache.clone());
                assert!(writer.update(project.source()).succeeded());
                let mut reader = ProjectSession::with_cache_directory(cache);
                let start = Instant::now();
                let compilation = reader.update(project.source());
                (start.elapsed(), compilation)
            }
        };
        assert!(compilation.succeeded(), "generated project must check");
        black_box(compilation.emit_c());
        if let Some(previous) = &observed {
            assert_eq!(
                previous, &compilation.stats,
                "project work counts must be deterministic"
            );
        } else {
            observed = Some(compilation.stats.clone());
        }
        times.push(elapsed);
    }
    times.sort();
    (
        times[times.len() / 2],
        observed.expect("at least one sample"),
    )
}

fn measure_project_jobs(
    graph: ProjectGraph,
    modules: usize,
    jobs: usize,
    samples: usize,
) -> (Duration, String) {
    let project = GeneratedProject::new(graph, modules);
    let mut times = Vec::with_capacity(samples);
    let mut output = None;
    for _ in 0..samples {
        let start = Instant::now();
        let compilation = project::compile_with_jobs(project.source(), jobs);
        let elapsed = start.elapsed();
        assert!(compilation.succeeded(), "generated project must check");
        let artifacts = format!(
            "{:?}\n{}",
            compilation.interfaces,
            compilation.emit_c().unwrap()
        );
        if let Some(previous) = &output {
            assert_eq!(previous, &artifacts);
        } else {
            output = Some(artifacts);
        }
        times.push(elapsed);
    }
    times.sort();
    (times[times.len() / 2], output.expect("at least one sample"))
}

fn validate_project_work(
    graph: ProjectGraph,
    modules: usize,
    declarations: usize,
    scenario: ProjectScenario,
    stats: &ProjectWorkStats,
) {
    let expected = match scenario {
        ProjectScenario::Cold => (declarations, declarations, declarations, declarations, 0),
        ProjectScenario::NoChange => (0, 0, 0, 0, declarations),
        ProjectScenario::PrivateBody => (1, 1, 1, 1, declarations - 1),
        ProjectScenario::PublicInterface => {
            let closure = match graph {
                ProjectGraph::Wide => 2,
                ProjectGraph::Deep => modules * 2,
            };
            (1, 1, closure, closure, declarations - closure)
        }
        ProjectScenario::PersistentWarm => (0, 0, 0, 0, declarations),
    };
    assert_eq!(
        (
            stats.declarations_parsed,
            stats.declarations_lowered,
            stats.declarations_checked,
            stats.declarations_generated,
            stats.declarations_reused,
        ),
        expected,
        "unexpected project work for {} {}",
        graph.name(),
        scenario.name()
    );
    assert!(
        !stats.fallback_clean,
        "benchmark must use project reuse path"
    );
}

#[allow(clippy::too_many_arguments)]
fn print_project_row(
    graph: ProjectGraph,
    modules: usize,
    declarations: usize,
    scenario: &str,
    jobs: usize,
    elapsed: Duration,
    stats: &ProjectWorkStats,
) {
    println!(
        "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
        graph.name(),
        modules + 1,
        declarations,
        scenario,
        jobs,
        match graph {
            ProjectGraph::Wide => modules,
            ProjectGraph::Deep => 1,
        },
        elapsed.as_micros(),
        stats.modules_read,
        stats.modules_parsed,
        stats.modules_lowered,
        stats.modules_checked,
        stats.modules_generated,
        stats.modules_reused,
        stats.declarations_parsed,
        stats.declarations_lowered,
        stats.declarations_checked,
        stats.declarations_generated,
        stats.declarations_reused,
        stats.invalidation_closure,
        stats.persistent_hits,
        stats.persistent_misses,
    );
}

fn validate_incremental_work(
    scenario: &str,
    functions: usize,
    central: usize,
    declarations: usize,
    work: &WorkStats,
) {
    let expected = match scenario {
        "cold" => (declarations, declarations, declarations, declarations, 0),
        "no-change" => (0, 0, 0, 0, declarations),
        "body" => (1, 1, 1, 1, declarations - 1),
        "leaf-interface" => (1, 1, 2, 2, declarations - 2),
        "central-interface" => {
            let closure = functions - central + 1;
            (1, 1, closure, closure, declarations - closure)
        }
        _ => unreachable!(),
    };
    assert_eq!(
        (
            work.parsed,
            work.lowered,
            work.checked,
            work.generated,
            work.reused
        ),
        expected,
        "unexpected work for {scenario}"
    );
    assert!(!work.fallback_clean, "benchmark must use incremental path");
}

fn timed_command(command: &mut Command) -> Duration {
    let start = Instant::now();
    let output = command.output().expect("benchmark compiler command");
    let elapsed = start.elapsed();
    if !output.status.success() {
        eprintln!(
            "benchmark build failed: {}",
            String::from_utf8_lossy(&output.stderr)
        );
        std::process::exit(1);
    }
    elapsed
}

fn run_output(executable: &PathBuf, arguments: &[PathBuf]) -> Vec<u8> {
    let output = Command::new(executable)
        .args(arguments)
        .output()
        .expect("benchmark executable");
    if !output.status.success() {
        eprintln!("{} failed", executable.display());
        std::process::exit(1);
    }
    output.stdout
}

fn median_runtime(executable: &PathBuf, arguments: &[PathBuf], samples: usize) -> Duration {
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

struct ComparisonBuild {
    path: PathBuf,
}

impl ComparisonBuild {
    fn new() -> Self {
        let path = std::env::temp_dir().join(format!("slim-comparison-{}", std::process::id()));
        if path.exists() {
            fs::remove_dir_all(&path).expect("remove previous comparison directory");
        }
        fs::create_dir(&path).expect("create comparison directory");
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

fn measure(source: &str, path: &PathBuf) -> Duration {
    let start = Instant::now();
    let compilation = compiler::compile(Source::new(path, source.to_owned()));
    assert!(compilation.succeeded());
    black_box(compilation);
    start.elapsed()
}

fn generated_project(declarations: usize) -> String {
    let mut source = String::with_capacity(declarations * 64);
    source.push_str("(module scaling ");
    for index in 0..declarations {
        source.push_str("(fn function-");
        source.push_str(&index.to_string());
        source.push_str(" ((value I64)) I64 (effects) (call i64.add value ");
        source.push_str(&(index as i64).to_string());
        source.push_str(")) ");
    }
    source.push_str("(fn main ((args (Vec Bytes))) I64 (effects) (call function-0 0)))\n");
    source
}

fn generated_incremental_project(
    functions: usize,
    leaf_addend: i64,
    inout_function: Option<usize>,
) -> String {
    assert!(functions > 0);
    let mut source = String::with_capacity(functions * 96);
    source.push_str("(module incremental ");
    for index in 0..functions {
        source.push_str("(fn function-");
        source.push_str(&index.to_string());
        if inout_function == Some(index) {
            source.push_str(" ((inout value I64)) I64 (effects) ");
        } else {
            source.push_str(" ((value I64)) I64 (effects) ");
        }
        if index == 0 {
            source.push_str("(call i64.add value ");
            source.push_str(&leaf_addend.to_string());
            source.push_str(")) ");
        } else {
            source.push_str("(call function-");
            source.push_str(&(index - 1).to_string());
            source.push_str(" value)) ");
        }
    }
    source
        .push_str("(fn main ((args (Vec Bytes))) I64 (effects) (let value I64 40 (call function-");
    source.push_str(&(functions - 1).to_string());
    source.push_str(" value))))\n");
    source
}

struct GeneratedProject {
    root: PathBuf,
}

impl GeneratedProject {
    fn new(graph: ProjectGraph, modules: usize) -> Self {
        assert!(modules > 0);
        let root = std::env::temp_dir().join(format!(
            "slim-project-bench-{}-{}-{}",
            std::process::id(),
            graph.name(),
            PROJECT_SERIAL.fetch_add(1, std::sync::atomic::Ordering::Relaxed)
        ));
        let _ = fs::remove_dir_all(&root);
        fs::create_dir(&root).expect("create project benchmark directory");
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
        Self { root }
    }

    fn source(&self) -> Source {
        let path = self.root.join("slim.project");
        Source::new(
            &path,
            fs::read_to_string(&path).expect("read benchmark manifest"),
        )
    }

    fn edit_private_body(&self) {
        self.replace_in_first_module("input 1", "input 2");
    }

    fn edit_public_interface(&self) {
        self.replace_in_first_module("((value I64))", "((value I64) (valid Bool))");
    }

    fn replace_in_first_module(&self, before: &str, after: &str) {
        let path = self.root.join(format!("{}.slim", module_name(0)));
        let source = fs::read_to_string(&path).expect("read edited benchmark module");
        let edited = source.replacen(before, after, 1);
        assert_ne!(source, edited, "benchmark edit marker must exist");
        fs::write(path, edited).expect("write edited benchmark module");
    }
}

impl Drop for GeneratedProject {
    fn drop(&mut self) {
        let _ = fs::remove_dir_all(&self.root);
    }
}

fn module_name(index: usize) -> String {
    format!("m{index:05}")
}

static PROJECT_SERIAL: std::sync::atomic::AtomicU64 = std::sync::atomic::AtomicU64::new(0);
