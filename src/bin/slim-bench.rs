use std::fs;
use std::hint::black_box;
use std::path::PathBuf;
use std::process::Command;
use std::time::{Duration, Instant};

use slim::compiler;
use slim::span::Source;

fn main() {
    let command = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "scaling".to_owned());
    match command.as_str() {
        "scaling" => run_scaling(),
        "compare" => run_comparison(),
        _ => {
            eprintln!("usage: slim-bench <scaling [--quick] | compare>");
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
    let mut previous: Option<(usize, Duration)> = None;
    let mut failed = false;
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
        if let Some((previous_size, previous_time)) = previous {
            let size_ratio = *size as f64 / previous_size as f64;
            let time_ratio = check.as_nanos() as f64 / previous_time.as_nanos() as f64;
            let exponent = time_ratio.ln() / size_ratio.ln();
            if exponent > 1.25 {
                eprintln!(
                    "scaling gate: observed exponent {exponent:.3} exceeds 1.25 between {previous_size} and {size} declarations"
                );
                failed = true;
            }
        }
        previous = Some((*size, check));
    }
    if failed {
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
