use std::fs;
use std::hint::black_box;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};
use std::sync::atomic::{AtomicU64, Ordering};
use std::time::{Duration, Instant};

fn main() {
    let command = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "scaling".to_owned());
    match command.as_str() {
        "scaling" => run_scaling(),
        "reduction" => run_reduction(),
        "incremental" => run_incremental(),
        "project" => run_project(),
        "compare" => run_comparison(),
        _ => {
            eprintln!(
                "usage: slim-bench <scaling [--quick] | reduction [--quick] | incremental [--quick] | project [--quick] | compare>"
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

fn run_scaling() {
    let quick = has_quick_flag();
    let sizes: &[usize] = if quick {
        &[250, 500, 1_000, 2_000]
    } else {
        &[1_000, 2_000, 4_000, 8_000]
    };
    let samples = if quick { 5 } else { 9 };
    let compiler = selfhost_compiler();
    let directory = TemporaryDirectory::new("scaling");
    println!("declarations\tbytes\tcheck_us\temit_us\tcheck_ns_per_byte");
    let mut first = None;
    let mut last = None;
    for size in sizes {
        let source = generated_program(*size);
        let path = directory.path.join(format!("generated-{size}.slim"));
        fs::write(&path, &source).expect("write scaling source");
        require_clean_output(compiler_output(&compiler, "check", &path), "scaling warmup");
        let mut check_times = Vec::with_capacity(samples);
        let mut emit_times = Vec::with_capacity(samples);
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
            black_box(&output.stdout);
            emit_times.push(elapsed);
        }
        check_times.sort();
        emit_times.sort();
        let check = check_times[samples / 2];
        let emit = emit_times[samples / 2];
        let per_byte = check.as_nanos() as f64 / source.len() as f64;
        println!(
            "{size}\t{}\t{}\t{}\t{per_byte:.2}",
            source.len(),
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
            "scaling gate: process-level check exponent {exponent:.3} exceeds 1.25 between {first_size} and {last_size} declarations"
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
    println!("declarations\tbytes\treduce_us\tanalyze_us\treduced_bytes\tanalysis_bytes");
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
        let mut reduce_times = Vec::with_capacity(samples);
        let mut analyze_times = Vec::with_capacity(samples);
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
        }
        reduce_times.sort();
        analyze_times.sort();
        let reduce = reduce_times[samples / 2];
        let analyze = analyze_times[samples / 2];
        println!(
            "{size}\t{}\t{}\t{}\t{}\t{}",
            source.len(),
            reduce.as_micros(),
            analyze.as_micros(),
            warm_reduced.len(),
            warm_analysis.len()
        );
        if first.is_none() {
            first = Some((*size, reduce, analyze));
        }
        last = Some((*size, reduce, analyze));
    }

    let (first_size, first_reduce, first_analyze) =
        first.expect("reduction benchmark has a first sample");
    let (last_size, last_reduce, last_analyze) =
        last.expect("reduction benchmark has a last sample");
    for (role, first_time, last_time) in [
        ("reduction", first_reduce, last_reduce),
        ("analysis", first_analyze, last_analyze),
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
                let work = String::from_utf8(oracle.expect("incremental sample"))
                    .expect("session work is UTF-8");
                println!(
                    "{}\t{}\t{}\t{}\t{}",
                    graph.name(),
                    modules + 1,
                    scenario.name(),
                    times[samples / 2].as_micros(),
                    work.trim()
                );
            }
        }
    }
}

fn run_project() {
    let quick = has_quick_flag();
    let sizes: &[usize] = if quick { &[4, 8, 16] } else { &[8, 16, 32, 64] };
    let samples = if quick { 2 } else { 5 };
    let compiler = selfhost_compiler();
    println!("graph\tmodules\tjobs\temit_us\tgenerated_bytes");
    for graph in [ProjectGraph::Wide, ProjectGraph::Deep] {
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
                println!(
                    "{}\t{}\t{jobs}\t{}\t{}",
                    graph.name(),
                    modules + 1,
                    times[samples / 2].as_micros(),
                    generated.len()
                );
            }
        }
    }
}

fn run_comparison() {
    let root = repository_root();
    let compiler = root.join("slimc");
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
        source.push_str(" ((value I64)) I64 (effects) (call i64.add value ");
        source.push_str(&index.to_string());
        source.push_str(")) ");
    }
    source.push_str("(fn main ((args (Vec Bytes))) I64 (effects) (call function-0 0)))\n");
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
