use std::fs;
use std::io::{ErrorKind, Read, Write};
use std::net::TcpListener;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::thread;
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};

fn temporary_directory(name: &str) -> PathBuf {
    let nonce = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_nanos();
    let path =
        std::env::temp_dir().join(format!("slim-test-{name}-{}-{nonce}", std::process::id()));
    fs::create_dir(&path).unwrap();
    path
}

fn slimc() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("slimc")
}

fn slim_bootstrap() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("bootstrap.sh")
}

fn native_compiler() -> std::ffi::OsString {
    std::env::var_os("CC").unwrap_or_else(|| "cc".into())
}

fn write_source(directory: &Path, source: &str) -> PathBuf {
    let path = directory.join("program.slim");
    fs::write(&path, source).unwrap();
    path
}

fn report_parentheses_are_balanced(report: &[u8]) -> bool {
    let mut depth = 0_i64;
    let mut in_string = false;
    let mut escaped = false;
    for byte in report {
        if in_string {
            if escaped {
                escaped = false;
            } else if *byte == b'\\' {
                escaped = true;
            } else if *byte == b'"' {
                in_string = false;
            }
            continue;
        }
        match *byte {
            b'"' => in_string = true,
            b'(' => depth += 1,
            b')' => {
                depth -= 1;
                if depth < 0 {
                    return false;
                }
            }
            _ => {}
        }
    }
    depth == 0 && !in_string && !escaped
}

#[test]
fn exposes_one_canonical_version_and_help_spelling() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let version = fs::read_to_string(root.join("VERSION")).unwrap();
    let reported = Command::new(slimc()).arg("--version").output().unwrap();
    assert!(reported.status.success());
    assert_eq!(
        String::from_utf8(reported.stdout).unwrap(),
        format!("slimc {} (self-hosted)\n", version.trim())
    );

    let help = Command::new(slimc()).arg("--help").output().unwrap();
    assert!(help.status.success());
    let help = String::from_utf8(help.stdout).unwrap();
    assert!(help.starts_with(&format!("SLIM compiler {} (self-hosted)\n", version.trim())));
    assert!(help.contains("slimc --version"));
    assert!(help.contains("slimc --help"));

    for alias in ["version", "help"] {
        let rejected = Command::new(slimc()).arg(alias).output().unwrap();
        assert!(
            !rejected.status.success(),
            "{alias} became a second spelling"
        );
    }
}

#[test]
fn generated_c_requires_the_exact_runtime_abi() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("runtime-abi");
    let generated = directory.join("hello.c");
    let emitted = Command::new(slimc())
        .arg("emit-c")
        .arg(root.join("examples/hello.slim"))
        .arg("-o")
        .arg(&generated)
        .output()
        .unwrap();
    assert!(
        emitted.status.success(),
        "{}",
        String::from_utf8_lossy(&emitted.stderr)
    );
    let source = fs::read_to_string(&generated).unwrap();
    assert!(
        source.contains(
            "_Static_assert(SLIM_RUNTIME_ABI_VERSION == 1, \"SLIM runtime ABI mismatch\");"
        )
    );

    let matching = Command::new(native_compiler())
        .args(["-std=c11", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(root.join("runtime"))
        .arg("-c")
        .arg(&generated)
        .arg("-o")
        .arg(directory.join("matching.o"))
        .output()
        .unwrap();
    assert!(
        matching.status.success(),
        "{}",
        String::from_utf8_lossy(&matching.stderr)
    );

    let header = fs::read_to_string(root.join("runtime/slim_rt.h"))
        .unwrap()
        .replace(
            "#define SLIM_RUNTIME_ABI_VERSION 1",
            "#define SLIM_RUNTIME_ABI_VERSION 2",
        );
    fs::write(directory.join("slim_rt.h"), header).unwrap();
    let mismatched = Command::new(native_compiler())
        .args(["-std=c11", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(&directory)
        .arg("-c")
        .arg(&generated)
        .arg("-o")
        .arg(directory.join("mismatched.o"))
        .output()
        .unwrap();
    assert!(!mismatched.status.success());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn checks_builds_and_runs_native_program() {
    let directory = temporary_directory("native");
    let source = write_source(
        &directory,
        "(module answer (fn main ((args (Vec Bytes))) I64 (effects io) (let shown Unit (call io.print-i64 42) (let newline Unit (call io.println \"\") 0))))\n",
    );
    let check = Command::new(slimc())
        .arg("check")
        .arg(&source)
        .output()
        .unwrap();
    assert!(
        check.status.success(),
        "{}",
        String::from_utf8_lossy(&check.stderr)
    );

    let executable = directory.join("nested/output/answer");
    let build = Command::new(slimc())
        .arg("build")
        .arg(&source)
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let run = Command::new(&executable).output().unwrap();
    assert!(run.status.success());
    assert_eq!(run.stdout, b"42\n");
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn monotonic_clock_is_typed_effectful_and_allocation_free() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/pass/monotonic_clock.slim");
    let emitted = Command::new(slimc()).arg(&source).output().unwrap();
    assert!(emitted.status.success());
    assert!(emitted.stderr.is_empty());
    let generated = String::from_utf8(emitted.stdout).unwrap();
    assert_eq!(generated.matches("slim_monotonic_ms()").count(), 2);

    let analysis = Command::new(slimc())
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    assert!(analysis.status.success());
    let report = String::from_utf8(analysis.stdout).unwrap();
    assert!(report.contains("(effects io) (cost-vector 1"));
    assert!(report.contains(
        "(expression-nodes 13) (calls 4) (matches 1) (mutations 0) (recurs 0) (allocation-sites 0) (trap-sites 0)"
    ));

    let run = Command::new(slimc())
        .arg("run")
        .arg(&source)
        .output()
        .unwrap();
    assert!(run.status.success());
    assert_eq!(run.stdout, b"OK\n");
    assert!(run.stderr.is_empty());
}

#[cfg(unix)]
#[test]
fn bounded_tcp_exchange_preserves_failure_state_and_closes_connections() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("tcp-exchange");
    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    listener.set_nonblocking(true).unwrap();
    let port = listener.local_addr().unwrap().port();

    let template = fs::read_to_string(root.join("benchmarks/host/tcp_client.slim")).unwrap();
    let source = write_source(&directory, &template.replace("8080", &port.to_string()));
    let executable = directory.join("tcp-exchange");
    let build = Command::new(slimc())
        .arg("build")
        .arg(&source)
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let server = thread::spawn(move || {
        let deadline = Instant::now() + Duration::from_secs(5);
        for _ in 0..2 {
            let mut stream = loop {
                match listener.accept() {
                    Ok((stream, _)) => break stream,
                    Err(error)
                        if error.kind() == ErrorKind::WouldBlock && Instant::now() < deadline =>
                    {
                        thread::sleep(Duration::from_millis(5));
                    }
                    Err(error) => panic!("loopback accept failed: {error}"),
                }
            };
            stream.set_nonblocking(false).unwrap();
            stream
                .set_read_timeout(Some(Duration::from_secs(2)))
                .unwrap();
            let mut request = Vec::new();
            stream.read_to_end(&mut request).unwrap();
            assert_eq!(request, b"PING");
            stream.write_all(b"PONG").unwrap();
        }
    });

    let run = Command::new(&executable).output().unwrap();
    server.join().unwrap();
    assert!(
        run.status.success(),
        "stdout={} stderr={}",
        String::from_utf8_lossy(&run.stdout),
        String::from_utf8_lossy(&run.stderr)
    );
    assert!(run.stdout.is_empty());
    assert!(run.stderr.is_empty());

    let emitted = Command::new(slimc()).arg(&source).output().unwrap();
    assert!(emitted.status.success());
    let generated = String::from_utf8(emitted.stdout).unwrap();
    assert_eq!(generated.matches("slim_tcp_exchange(").count(), 3);

    let analysis = Command::new(slimc())
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    assert!(analysis.status.success());
    let report = String::from_utf8(analysis.stdout).unwrap();
    assert!(report.contains("(effects alloc io partial)"));
    assert!(report.contains("(allocation-sites 1) (trap-sites 2)"));
    assert!(report.contains("(reason allocation-or-io)"));
    fs::remove_dir_all(directory).unwrap();
}

#[cfg(unix)]
#[test]
fn explicit_structured_fork_joins_loopback_requests_and_adopts_owned_results() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("explicit-fork");
    let left = TcpListener::bind("127.0.0.1:0").unwrap();
    let right = TcpListener::bind("127.0.0.1:0").unwrap();
    let left_port = left.local_addr().unwrap().port();
    let right_port = right.local_addr().unwrap().port();

    let template = fs::read_to_string(root.join("benchmarks/host/dual_fetch.slim")).unwrap();
    let source = write_source(
        &directory,
        &template
            .replace("8080", &left_port.to_string())
            .replace("8081", &right_port.to_string()),
    );
    let serial = directory.join("serial");
    let parallel = directory.join("parallel");
    for (tier, executable) in [("serial", &serial), ("posix", &parallel)] {
        let build = Command::new(slimc())
            .env("SLIM_WORKER_TIER", tier)
            .arg("build")
            .arg(&source)
            .arg("-o")
            .arg(executable)
            .output()
            .unwrap();
        assert!(
            build.status.success(),
            "{}",
            String::from_utf8_lossy(&build.stderr)
        );
    }

    let serve = |listener: TcpListener, expected: &'static [u8]| {
        thread::spawn(move || {
            for _ in 0..4 {
                let (mut stream, _) = listener.accept().unwrap();
                let mut request = Vec::new();
                stream.read_to_end(&mut request).unwrap();
                assert_eq!(request, expected);
                thread::sleep(Duration::from_millis(120));
                stream.write_all(b"PONG").unwrap();
            }
        })
    };
    let left_server = serve(left, b"LEFT");
    let right_server = serve(right, b"RIGHT");

    let serial_run = Command::new(&serial).output().unwrap();
    let parallel_run = Command::new(&parallel).output().unwrap();
    let fallback = Command::new(&parallel)
        .env("SLIM_TASK_DISABLE", "1")
        .output()
        .unwrap();
    let allocation_failure = Command::new(&parallel)
        .env("SLIM_ALLOC_FAIL_AT", "2")
        .output()
        .unwrap();

    left_server.join().unwrap();
    right_server.join().unwrap();
    for output in [&serial_run, &parallel_run, &fallback] {
        assert!(output.status.success());
        assert_eq!(output.stdout, b"OK\n");
        assert!(output.stderr.is_empty());
    }
    assert_eq!(allocation_failure.status.code(), Some(71));
    assert!(allocation_failure.stdout.is_empty());
    assert_eq!(
        allocation_failure.stderr,
        b"SLIM allocation failure: exhausted at allocation 2\n"
    );

    let generated = fs::read_to_string(directory.join("program.slim")).unwrap();
    assert!(generated.contains("(fork (let first Reply"));
    let emitted = Command::new(slimc()).arg(&source).output().unwrap();
    assert!(emitted.status.success());
    let emitted = String::from_utf8(emitted.stdout).unwrap();
    for required in [
        "#define SLIM_PARALLEL 1",
        "slim_parallel_first_region",
        "slim_parallel_second_region",
        "slim_region_adopt(slim_allocation_region, &slim_parallel_first_region)",
        "slim_region_adopt(slim_allocation_region, &slim_parallel_second_region)",
    ] {
        assert!(
            emitted.contains(required),
            "generated C is missing {required}"
        );
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn unsupported_network_target_returns_typed_failure() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("unsupported-network");
    let executable = directory.join("unsupported-network");
    let build = Command::new(native_compiler())
        .arg("-std=c11")
        .arg("-O2")
        .arg("-Wall")
        .arg("-Wextra")
        .arg("-Werror")
        .arg("-DSLIM_DISABLE_NETWORK=1")
        .arg("-I")
        .arg(root.join("runtime"))
        .arg(root.join("tests/fixtures/unsupported_network.c"))
        .arg(root.join("runtime/slim_rt.c"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );
    let run = Command::new(&executable).output().unwrap();
    assert!(run.status.success());
    assert!(run.stdout.is_empty());
    assert!(run.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn structured_worker_runtime_falls_back_and_prevents_nesting() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("parallel-runtime");
    let fixture = root.join("tests/fixtures/parallel_runtime.c");
    let runtime = root.join("runtime/slim_rt.c");

    let serial = directory.join("serial-tier");
    let serial_build = Command::new(native_compiler())
        .arg("-std=c11")
        .arg("-O2")
        .arg("-Wall")
        .arg("-Wextra")
        .arg("-Werror")
        .arg("-DSLIM_PARALLEL=1")
        .arg("-I")
        .arg(root.join("runtime"))
        .arg(&fixture)
        .arg(&runtime)
        .arg("-o")
        .arg(&serial)
        .output()
        .unwrap();
    assert!(
        serial_build.status.success(),
        "{}",
        String::from_utf8_lossy(&serial_build.stderr)
    );
    let serial_run = Command::new(&serial).output().unwrap();
    assert!(serial_run.status.success());
    assert_eq!(serial_run.stdout, b"0 0 42\n");

    #[cfg(unix)]
    {
        let posix = directory.join("posix-tier");
        let posix_build = Command::new(native_compiler())
            .arg("-std=c11")
            .arg("-O2")
            .arg("-Wall")
            .arg("-Wextra")
            .arg("-Werror")
            .arg("-DSLIM_PARALLEL=1")
            .arg("-DSLIM_POSIX_WORKERS=1")
            .arg("-pthread")
            .arg("-I")
            .arg(root.join("runtime"))
            .arg(&fixture)
            .arg(&runtime)
            .arg("-o")
            .arg(&posix)
            .output()
            .unwrap();
        assert!(
            posix_build.status.success(),
            "{}",
            String::from_utf8_lossy(&posix_build.stderr)
        );
        let parallel = Command::new(&posix).output().unwrap();
        assert!(parallel.status.success());
        assert_eq!(parallel.stdout, b"1 0 42\n");

        for setting in ["SLIM_TASK_FAIL_AT", "SLIM_TASK_DISABLE"] {
            let fallback = Command::new(&posix).env(setting, "1").output().unwrap();
            assert!(fallback.status.success());
            assert_eq!(fallback.stdout, b"0 0 42\n");
        }

        let join_failure = Command::new(&posix)
            .env("SLIM_TASK_JOIN_FAIL_AT", "1")
            .output()
            .unwrap();
        assert_eq!(join_failure.status.code(), Some(70));
        assert!(join_failure.stdout.is_empty());
        assert_eq!(
            join_failure.stderr,
            b"SLIM runtime trap: injected structured task join failure\n"
        );
    }

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn adopted_region_storage_remains_parent_owned_and_resizable() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("region-adoption");
    let executable = directory.join("region-adoption");
    let build = Command::new(native_compiler())
        .arg("-std=c11")
        .arg("-O2")
        .arg("-Wall")
        .arg("-Wextra")
        .arg("-Werror")
        .arg("-I")
        .arg(root.join("runtime"))
        .arg(root.join("tests/fixtures/region_adoption.c"))
        .arg(root.join("runtime/slim_rt.c"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );
    let run = Command::new(&executable).output().unwrap();
    assert!(run.status.success());
    assert_eq!(run.stdout, b"OK\n");
    assert!(run.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn production_codegen_executes_profitable_plan_with_serial_fallback() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("parallel-codegen");
    let source = root.join("benchmarks/challenges/state_machine/program.slim");
    let analysis = Command::new(slimc())
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    assert!(analysis.status.success());
    let analysis = String::from_utf8(analysis.stdout).unwrap();
    for required in [
        "(task-work 2000000 2000000)",
        "(profitability exact) (profitable true) (target-tier posix-v1)",
        "(executable-sites 1) (executed-sites 1)",
        "(execution (guarantee exact) (status enabled) (tier posix-v1-with-serial-fallback))",
    ] {
        assert!(
            analysis.contains(required),
            "state-machine analysis is missing {required}"
        );
    }
    let generated = directory.join("state-machine.c");
    let emit = Command::new(slimc())
        .arg("emit-c")
        .arg(&source)
        .arg("-o")
        .arg(&generated)
        .output()
        .unwrap();
    assert!(
        emit.status.success(),
        "{}",
        String::from_utf8_lossy(&emit.stderr)
    );
    let generated_text = fs::read_to_string(&generated).unwrap();
    for required in [
        "#define SLIM_PARALLEL 1",
        "typedef struct {",
        "SlimParallel_",
        "slim_task_spawn",
        "slim_task_join",
        "slim_task_run_inline",
        "slim_parallel_first.slim_result",
        "slim_parallel_second.slim_result",
    ] {
        assert!(
            generated_text.contains(required),
            "generated parallel C is missing {required}"
        );
    }
    assert_eq!(
        generated_text
            .matches("static void slim_parallel_run_")
            .count(),
        2
    );
    assert_eq!(generated_text.matches("slim_task_run_inline").count(), 2);

    let serial = directory.join("serial");
    let serial_build = Command::new(slimc())
        .env("SLIM_WORKER_TIER", "serial")
        .arg("build")
        .arg(&source)
        .arg("-o")
        .arg(&serial)
        .output()
        .unwrap();
    assert!(
        serial_build.status.success(),
        "{}",
        String::from_utf8_lossy(&serial_build.stderr)
    );
    let serial_run = Command::new(&serial).output().unwrap();
    assert!(serial_run.status.success());
    assert_eq!(serial_run.stdout, b"0\n");

    let automatic = directory.join("automatic");
    let automatic_build = Command::new(slimc())
        .arg("build")
        .arg(&source)
        .arg("-o")
        .arg(&automatic)
        .output()
        .unwrap();
    assert!(
        automatic_build.status.success(),
        "{}",
        String::from_utf8_lossy(&automatic_build.stderr)
    );
    for setting in ["SLIM_TASK_FAIL_AT", "SLIM_TASK_DISABLE"] {
        let fallback = Command::new(&automatic).env(setting, "1").output().unwrap();
        assert!(fallback.status.success());
        assert_eq!(fallback.stdout, b"0\n");
    }
    #[cfg(unix)]
    {
        let joined = Command::new(&automatic)
            .env("SLIM_TASK_JOIN_FAIL_AT", "1")
            .output()
            .unwrap();
        assert_eq!(joined.status.code(), Some(70));
        assert_eq!(
            joined.stderr,
            b"SLIM runtime trap: injected structured task join failure\n"
        );
    }

    let signal_source = root.join("benchmarks/challenges/signal_network/program.slim");
    let signal = directory.join("signal-network");
    let signal_build = Command::new(slimc())
        .arg("build")
        .arg(&signal_source)
        .arg("-o")
        .arg(&signal)
        .output()
        .unwrap();
    assert!(
        signal_build.status.success(),
        "{}",
        String::from_utf8_lossy(&signal_build.stderr)
    );
    let signal_parallel = Command::new(&signal).output().unwrap();
    let signal_fallback = Command::new(&signal)
        .env("SLIM_TASK_DISABLE", "1")
        .output()
        .unwrap();
    assert!(signal_parallel.status.success());
    assert!(signal_fallback.status.success());
    assert_eq!(signal_parallel.stdout, b"0\n");
    assert_eq!(signal_parallel.stdout, signal_fallback.stdout);

    let nested_source = write_source(
        &directory,
        "(module nested-plan (fn run ((remaining I64) (state Bool)) Bool (effects partial) (match (call i64.le remaining 0) (true state) (false (recur (call i64.sub remaining 1) (call bool.not state))))) (fn main ((args (Vec Bytes))) I64 (effects partial) (match true (true (let left Bool (call run 1000000 true) (let right Bool (call run 1000000 false) 0))) (false 0))))\n",
    );
    let nested_analysis = Command::new(slimc())
        .arg("analyze")
        .arg(&nested_source)
        .output()
        .unwrap();
    assert!(nested_analysis.status.success());
    let nested_analysis = String::from_utf8(nested_analysis.stdout).unwrap();
    assert!(nested_analysis.contains("(task-work 1000000 1000000)"));
    assert!(nested_analysis.contains("(executable-sites 0) (executed-sites 0)"));
    let nested_generated = directory.join("nested.c");
    let nested_emit = Command::new(slimc())
        .arg("emit-c")
        .arg(&nested_source)
        .arg("-o")
        .arg(&nested_generated)
        .output()
        .unwrap();
    assert!(nested_emit.status.success());
    assert!(
        !fs::read_to_string(nested_generated)
            .unwrap()
            .contains("SLIM_PARALLEL")
    );

    let hello = directory.join("hello.c");
    let hello_emit = Command::new(slimc())
        .arg("emit-c")
        .arg(root.join("examples/hello.slim"))
        .arg("-o")
        .arg(&hello)
        .output()
        .unwrap();
    assert!(hello_emit.status.success());
    let hello_text = fs::read_to_string(hello).unwrap();
    for absent in ["SLIM_PARALLEL", "SlimTask", "slim_parallel_"] {
        assert!(
            !hello_text.contains(absent),
            "unselected generated C unexpectedly contains {absent}"
        );
    }

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn emits_c_deterministically() {
    let directory = temporary_directory("determinism");
    let source = write_source(
        &directory,
        "(module deterministic (fn main ((args (Vec Bytes))) I64 (effects) (call i64.add 40 2)))\n",
    );
    let first = directory.join("first.c");
    let second = directory.join("second.c");
    for output in [&first, &second] {
        let status = Command::new(slimc())
            .arg("emit-c")
            .arg(&source)
            .arg("-o")
            .arg(output)
            .status()
            .unwrap();
        assert!(status.success());
    }
    assert_eq!(fs::read(first).unwrap(), fs::read(&second).unwrap());
    let generated = fs::read_to_string(&second).unwrap();
    assert!(generated.contains("slim_i64_add"));
    assert!(generated.contains("slim_fn_main"));
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn propagates_typed_allocation_failure() {
    let directory = temporary_directory("allocation-failure");
    let source = write_source(
        &directory,
        "(module allocation-failure (fn main ((args (Vec Bytes))) I64 (effects alloc) (let values (Vec I64) (call vec.new) (let pushed Unit (call vec.push values 42) 0))))\n",
    );
    let executable = directory.join("program");
    let build = Command::new(slimc())
        .arg("build")
        .arg(&source)
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );
    let failed = Command::new(&executable)
        .env("SLIM_ALLOC_FAIL_AT", "2")
        .output()
        .unwrap();
    assert_eq!(failed.status.code(), Some(71));
    assert!(failed.stdout.is_empty());
    assert_eq!(
        failed.stderr,
        b"SLIM allocation failure: exhausted at allocation 2\n"
    );
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn returns_structured_multiple_diagnostics() {
    let directory = temporary_directory("diagnostics");
    let source = write_source(
        &directory,
        "(module bad (fn main ((args (Vec Bytes))) I64 (effects) (match true (true missing))))\n",
    );
    let output = Command::new(slimc())
        .arg("--message-format=json")
        .arg("check")
        .arg(&source)
        .output()
        .unwrap();
    assert_eq!(output.status.code(), Some(1));
    let stderr = String::from_utf8(output.stderr).unwrap();
    let lines: Vec<_> = stderr.lines().collect();
    assert!(lines.len() >= 2, "{stderr}");
    assert!(
        lines
            .iter()
            .all(|line| line.starts_with("{\"schema\":1,\"code\":"))
    );
    assert!(stderr.contains("E0314"));
    assert!(stderr.contains("E0336"));
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn formatter_is_idempotent_through_cli() {
    let directory = temporary_directory("format");
    let source = write_source(
        &directory,
        " (module  formatted\n (fn main ((args (Vec Bytes))) I64 (effects) 0)) ; comment\n",
    );
    let status = Command::new(slimc())
        .arg("fmt")
        .arg(&source)
        .status()
        .unwrap();
    assert!(status.success());
    let first = fs::read(&source).unwrap();
    let status = Command::new(slimc())
        .arg("fmt")
        .arg(&source)
        .status()
        .unwrap();
    assert!(status.success());
    assert_eq!(first, fs::read(&source).unwrap());
    let status = Command::new(slimc())
        .arg("fmt")
        .arg(&source)
        .arg("--check")
        .status()
        .unwrap();
    assert!(status.success());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn direct_reduction_is_idempotent_and_behavior_preserving() {
    let directory = temporary_directory("direct-reduction");
    let original = write_source(
        &directory,
        include_str!("../conformance/pass/reduction.slim"),
    );
    let expected = include_bytes!("../conformance/tool/reduction.expected.slim");

    let first = Command::new(slimc())
        .arg("reduce")
        .arg(&original)
        .output()
        .unwrap();
    assert!(
        first.status.success(),
        "{}",
        String::from_utf8_lossy(&first.stderr)
    );
    assert_eq!(first.stdout, expected);

    let reduced = directory.join("reduced.slim");
    fs::write(&reduced, &first.stdout).unwrap();
    let second = Command::new(slimc())
        .arg("reduce")
        .arg(&reduced)
        .output()
        .unwrap();
    assert!(second.status.success());
    assert_eq!(second.stdout, first.stdout);

    let mut executables = Vec::new();
    for (name, source) in [("original", &original), ("reduced", &reduced)] {
        let executable = directory.join(name);
        let build = Command::new(slimc())
            .arg("build")
            .arg(source)
            .arg("-o")
            .arg(&executable)
            .output()
            .unwrap();
        assert!(
            build.status.success(),
            "{}",
            String::from_utf8_lossy(&build.stderr)
        );
        executables.push(executable);
    }

    let original_run = Command::new(&executables[0]).output().unwrap();
    let reduced_run = Command::new(&executables[1]).output().unwrap();
    assert_eq!(original_run.status.code(), reduced_run.status.code());
    assert_eq!(original_run.stdout, b"42\n");
    assert_eq!(original_run.stdout, reduced_run.stdout);
    assert_eq!(original_run.stderr, reduced_run.stderr);

    for fault_at in [1, 2, 3] {
        let original_failure = Command::new(&executables[0])
            .env("SLIM_ALLOC_FAIL_AT", fault_at.to_string())
            .output()
            .unwrap();
        let reduced_failure = Command::new(&executables[1])
            .env("SLIM_ALLOC_FAIL_AT", fault_at.to_string())
            .output()
            .unwrap();
        assert_eq!(
            original_failure.status.code(),
            reduced_failure.status.code()
        );
        assert_eq!(original_failure.stdout, reduced_failure.stdout);
        assert_eq!(original_failure.stderr, reduced_failure.stderr);
    }

    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let preserved_original = root.join("conformance/pass/reduction_preserve.slim");
    let preserved_output = Command::new(slimc())
        .arg("reduce")
        .arg(&preserved_original)
        .output()
        .unwrap();
    assert!(preserved_output.status.success());
    let preserved_reduced = directory.join("preserved-reduced.slim");
    fs::write(&preserved_reduced, preserved_output.stdout).unwrap();
    let mut preserved_executables = Vec::new();
    for (name, source) in [
        ("preserved-original", &preserved_original),
        ("preserved-reduced", &preserved_reduced),
    ] {
        let executable = directory.join(name);
        let build = Command::new(slimc())
            .arg("build")
            .arg(source)
            .arg("-o")
            .arg(&executable)
            .output()
            .unwrap();
        assert!(
            build.status.success(),
            "{}",
            String::from_utf8_lossy(&build.stderr)
        );
        preserved_executables.push(executable);
    }
    let original_trap = Command::new(&preserved_executables[0]).output().unwrap();
    let reduced_trap = Command::new(&preserved_executables[1]).output().unwrap();
    assert_eq!(original_trap.status.code(), Some(70));
    assert_eq!(original_trap.status.code(), reduced_trap.status.code());
    assert_eq!(original_trap.stdout, b"kept\n");
    assert_eq!(original_trap.stdout, reduced_trap.stdout);
    assert_eq!(
        original_trap.stderr,
        b"SLIM runtime trap: I64 addition overflow\n"
    );
    assert_eq!(original_trap.stderr, reduced_trap.stderr);
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn semantic_analysis_is_stable_and_bounded() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("examples/vector_sum.slim");
    let first = Command::new(slimc())
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    let second = Command::new(slimc())
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    assert!(first.status.success());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    assert!(report.starts_with("(analysis 7 (module vector-sum)"));
    assert!(report.contains("(fact-limit 64)"));
    assert!(report.contains("(quality (guarantee exact)"));
    assert!(report.contains("(function-quality 3 fill"));
    assert!(report.contains("(allocation-sites 1)"));
    assert!(report.contains("(totality (guarantee unknown) (reason recursion-or-unproved-call))"));
    assert!(report.contains("(function 71 sum"));
    assert!(
        report.contains("(binding 76 values (type (Vec I64)) (ownership owned) (scope-end 138)")
    );
    assert!(report.contains("(uses 3)"));
    assert!(report.contains("(last-use 130)"));
    assert!(report.ends_with(")\n"));

    let pattern_report = Command::new(slimc())
        .arg("analyze")
        .arg(root.join("conformance/pass/lifetimes.slim"))
        .output()
        .unwrap();
    assert!(pattern_report.status.success());
    let pattern_report = String::from_utf8(pattern_report.stdout).unwrap();
    assert!(
        pattern_report.contains(
            "(binding 230 items (type unknown) (ownership unknown) (scope-end 238) (uses 1) (last-use 235)"
        )
    );
}

#[test]
fn quality_analysis_classifies_exact_and_unknown_facts() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/evidence/quality.slim");
    let output = Command::new(slimc())
        .arg("analyze")
        .arg(source)
        .output()
        .unwrap();
    assert!(output.status.success());
    assert!(output.stderr.is_empty());
    let report = String::from_utf8(output.stdout).unwrap();
    assert!(report.contains("(state-model TenFlags (guarantee exact) (cardinality (pow2 10)))"));
    assert!(
        report.contains(
            "(state-model Dynamic (guarantee unknown) (reason dynamic-or-unresolved-type))"
        )
    );
    assert!(report.contains(
        "(state-model Decision (guarantee exact) (cardinality (sum (pow2 0) (pow2 1) (pow2 8))))"
    ));
    assert!(
        report
            .contains("(cost-vector 1 (source-size (model expression-tokens-v1) (guarantee exact)")
    );
    assert!(report.contains(
        "(runtime-work (model dynamic-work-v1) (guarantee unknown) (reason execution-frequency-or-call-bound))"
    ));
    assert!(report.contains(
        "(peak-memory (model peak-bytes-v1) (guarantee unknown) (reason allocation-volume-or-layout-bound))"
    ));
    assert!(report.contains("(effect-surface (model declared-effect-kinds-v1) (guarantee exact)"));
    assert!(report.contains(
        "(failure-surface (model static-allocation-and-trap-sites-v1) (guarantee exact)"
    ));
    assert!(report.contains("(proof-burden (model static-obligations-v1) (guarantee exact)"));
    assert!(report.contains("(totality (guarantee exact) (status total))"));
}

#[test]
fn integer_ranges_prove_guarded_arithmetic_and_preserve_unknowns() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/evidence/integer_ranges.slim");
    let analyze = || {
        Command::new(slimc())
            .arg("analyze")
            .arg(&source)
            .output()
            .unwrap()
    };
    let first = analyze();
    let second = analyze();
    assert!(first.status.success());
    assert!(first.stderr.is_empty());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    for required in [
        "(analysis 7 (module integer-ranges)",
        "(integer-proofs (domain -1000000000 1000000000)",
        "(refinements 6) (refinements-truncated false)",
        "(checked-site 26 (status total) (lower unknown) (upper 10))",
        "(checked-site 62 (status total) (lower -10) (upper unknown))",
        "(checked-site 84 (status total) (lower 44) (upper 44))",
        "(checked-site 105 (status total) (lower 42) (upper 42))",
        "(checked-site 140 (status unknown)",
        "(checked-site 156 (status unknown)",
        "(checked-site 172 (status unknown)",
        "(checked-site 192 (status total) (lower -2) (upper 2))",
        "(checked-site 212 (status total) (lower unknown) (upper unknown))",
        "(checked-site 232 (status unknown)",
        "(checked-site 273 (status total) (lower 0) (upper unknown))",
        "guarded-upper (guarantee exact) (status safe)",
        "guarded-lower (guarantee exact) (status safe)",
        "exact-arithmetic (guarantee exact) (status safe)",
        "unguarded (guarantee exact) (status unavailable) (reason checked-trap)",
        "zero-divisor (guarantee exact) (status unavailable) (reason checked-trap)",
        "domain-limit (guarantee exact) (status unavailable) (reason checked-trap)",
        "constant-remainder (guarantee exact) (status safe) (blockers)",
        "constant-division (guarantee exact) (status safe) (blockers)",
        "possible-division-overflow (guarantee exact) (status unavailable) (reason checked-trap)",
        "total-countdown (guarantee exact) (status safe) (blockers)",
        "total-countdown (guarantee exact) (effects partial) (cost-vector 1",
        "(expression-nodes 11)",
        "(recurs 1) (allocation-sites 0) (trap-sites 1) (totality (guarantee exact) (status total))",
        "(eligible-sites 1)",
    ] {
        assert!(
            report.contains(required),
            "missing integer fact: {required}"
        );
    }
}

#[test]
fn resource_evidence_reports_exact_zero_and_unknown_recurrence_work() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/evidence/resource_work.slim");
    let analyze = || {
        Command::new(slimc())
            .arg("analyze")
            .arg(&source)
            .output()
            .unwrap()
    };
    let first = analyze();
    let second = analyze();
    assert!(first.status.success());
    assert!(first.stderr.is_empty());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    for required in [
        "(analysis 7 (module resource-work)",
        "(resource-evidence (profile-limit 16) (call-site-report-limit 64)",
        "(recurrence-profile 3 run (guarantee exact) (controller-position 0) (stop-bound 0) (step 1))",
        "(call-work 53 3 run (guarantee exact) (iterations 10))",
        "(call-work 69 3 run (guarantee exact) (iterations 0))",
        "(call-work 89 3 run (guarantee unknown) (reason nonliteral-controller))",
        "(recurrence-profile-count 1) (reported-recurrence-profiles 1) (recurrence-profiles-truncated false)",
        "(profiled-call-site-count 3) (reported-call-site-count 3) (exact-call-work-sites 2) (unknown-call-work-sites 1)",
        "(maximum-exact-iterations 10) (guarantee exact)",
    ] {
        assert!(
            report.contains(required),
            "missing resource fact: {required}"
        );
    }
}

#[test]
fn resource_evidence_limits_profiles_and_reported_call_sites() {
    let directory = temporary_directory("resource-bounds");

    let mut profiles = "(module resource-profile-bound".to_owned();
    for index in 0..17 {
        profiles.push_str(&format!(
            " (fn run-{index} ((remaining I64)) I64 (effects partial) (match (call i64.le remaining 0) (true 0) (false (recur (call i64.sub remaining 1)))))"
        ));
    }
    profiles.push_str(" (fn main ((args (Vec Bytes))) I64 (effects) 0))\n");
    let profile_path = write_source(&directory, &profiles);
    let profile_output = Command::new(slimc())
        .arg("analyze")
        .arg(&profile_path)
        .output()
        .unwrap();
    assert!(profile_output.status.success());
    assert!(report_parentheses_are_balanced(&profile_output.stdout));
    let profile_report = String::from_utf8(profile_output.stdout).unwrap();
    assert!(profile_report.contains(
        "(recurrence-profile-count 17) (reported-recurrence-profiles 16) (recurrence-profiles-truncated true)"
    ));
    assert_eq!(profile_report.matches("(recurrence-profile ").count(), 16);
    assert!(profile_report.contains("(guarantee bounded)"));

    let mut body = "0".to_owned();
    for index in (0..65).rev() {
        body = format!("(let value-{index} I64 (call run {index}) {body})");
    }
    let calls = format!(
        "(module resource-call-bound (fn run ((remaining I64)) I64 (effects partial) (match (call i64.le remaining 0) (true 0) (false (recur (call i64.sub remaining 1))))) (fn many () I64 (effects partial) {body}) (fn main ((args (Vec Bytes))) I64 (effects) 0))\n"
    );
    let call_path = directory.join("calls.slim");
    fs::write(&call_path, calls).unwrap();
    let analyze = || {
        Command::new(slimc())
            .arg("analyze")
            .arg(&call_path)
            .output()
            .unwrap()
    };
    let first = analyze();
    let second = analyze();
    assert!(first.status.success());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let call_report = String::from_utf8(first.stdout).unwrap();
    assert!(call_report.contains(
        "(profiled-call-site-count 65) (reported-call-site-count 64) (exact-call-work-sites 65) (unknown-call-work-sites 0)"
    ));
    assert!(call_report.contains("(maximum-exact-iterations 64) (guarantee bounded)"));
    assert_eq!(call_report.matches("(call-work ").count(), 64);

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn integer_range_refinement_limit_is_explicit_and_deterministic() {
    let directory = temporary_directory("integer-range-bound");
    let mut body = "value".to_owned();
    for _ in 0..33 {
        body = format!("(match (call i64.lt value 10) (true {body}) (false value))");
    }
    let source = format!(
        "(module integer-range-bound (fn nested ((value I64)) I64 (effects) {body}) (fn main ((args (Vec Bytes))) I64 (effects) 0))\n"
    );
    let path = write_source(&directory, &source);
    let analyze = || {
        Command::new(slimc())
            .arg("analyze")
            .arg(&path)
            .output()
            .unwrap()
    };
    let first = analyze();
    let second = analyze();
    assert!(first.status.success());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    assert!(report.contains("(refinement-limit 64) (refinements 64)"));
    assert!(report.contains("(refinements-truncated true)"));
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn integer_checked_site_report_limit_is_explicit_and_deterministic() {
    let directory = temporary_directory("integer-site-bound");
    let mut source = "(module integer-site-bound".to_owned();
    for index in 0..65 {
        source.push_str(&format!(
            " (fn f{index} ((value I64)) I64 (effects) (call i64.add value 1))"
        ));
    }
    source.push_str(" (fn main ((args (Vec Bytes))) I64 (effects) 0))\n");
    let path = write_source(&directory, &source);
    let analyze = || {
        Command::new(slimc())
            .arg("analyze")
            .arg(&path)
            .output()
            .unwrap()
    };
    let first = analyze();
    let second = analyze();
    assert!(first.status.success());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    assert!(report.contains("(checked-site-report-limit 64)"));
    assert!(report.contains("(checked-site-count 65) (guarantee bounded)"));
    assert_eq!(report.matches("(checked-site ").count(), 64);
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn parallelism_analysis_proves_only_independent_reorder_safe_work() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/evidence/parallelism.slim");
    let analyze = || {
        Command::new(slimc())
            .arg("analyze")
            .arg(&source)
            .output()
            .unwrap()
    };
    let first = analyze();
    let second = analyze();
    assert!(first.status.success());
    assert!(first.stderr.is_empty());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    for required in [
        "(parallelism (guarantee exact) (function-limit 64) (edge-limit 4096) (resolution-pass-limit 64) (schedule-limit 64)",
        "safe-left (guarantee exact) (status safe) (blockers)",
        "safe-right (guarantee exact) (status safe) (blockers)",
        "overdeclared (guarantee exact) (status safe) (blockers)",
        "traps (guarantee exact) (status unavailable) (reason checked-trap) (blockers checked-trap)",
        "calls-trap (guarantee exact) (status unavailable) (reason callee-not-safe) (blockers callee-not-safe)",
        "allocates (guarantee exact) (status unavailable) (reason allocation-or-io) (blockers allocation-or-io)",
        "borrows (guarantee exact) (status unavailable) (reason exclusive-borrow) (blockers exclusive-borrow)",
        "mutates (guarantee exact) (status unavailable) (reason mutation) (blockers mutation)",
        "repeats (guarantee exact) (status unavailable) (reason recurrence) (blockers recurrence)",
        "countdown (guarantee exact) (status safe) (blockers)",
        "countdown-pair (guarantee exact) (status safe) (blockers)",
        "overlap (guarantee exact) (status safe) (blockers)",
        "cycle-left (guarantee unknown) (status unknown) (reason call-cycle) (blockers call-cycle)",
        "(race-free true) (deadlock-free true) (profitability unknown) (profitability-reason target-work-unavailable)",
        "(schedule (policy lexical-earliest-nonoverlap) (guarantee exact) (candidate-sites 4) (selected-sites 3) (reported-sites 3) (executable-sites 0) (executed-sites 0))",
        "(eligible-sites 4)",
        "(execution (guarantee exact) (status disabled) (reason no-profitable-capture-safe-site))",
    ] {
        assert!(
            report.contains(required),
            "missing parallel fact: {required}"
        );
    }
    assert_eq!(report.matches("(fork-site ").count(), 3);
}

#[test]
fn parallelism_schedule_limit_is_explicit_and_deterministic() {
    let directory = temporary_directory("parallel-schedule-bound");
    let mut body = "true".to_owned();
    for index in (0..130).rev() {
        body = format!("(let task-{index} Bool (call work) {body})");
    }
    let source = format!(
        "(module parallel-schedule-bound (fn work () Bool (effects) true) (fn plan () Bool (effects) {body}) (fn main ((args (Vec Bytes))) I64 (effects) 0))\n"
    );
    let path = write_source(&directory, &source);
    let analyze = || {
        Command::new(slimc())
            .arg("analyze")
            .arg(&path)
            .output()
            .unwrap()
    };
    let first = analyze();
    let second = analyze();
    assert!(first.status.success());
    assert!(first.stderr.is_empty());
    assert_eq!(first.stdout, second.stdout);
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    assert!(report.contains(
        "(schedule (policy lexical-earliest-nonoverlap) (guarantee bounded) (candidate-sites 129) (selected-sites 65) (reported-sites 64) (executable-sites 0) (executed-sites 0))"
    ));
    assert!(report.contains("(eligible-sites 129)"));
    assert_eq!(report.matches("(fork-site ").count(), 64);
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn parallelism_analysis_reports_function_and_edge_bounds() {
    let directory = temporary_directory("parallel-bounds");

    let mut function_source = String::from(
        "(module parallel-function-bound (fn needs-late () Bool (effects) (call late)) ",
    );
    for index in 0..63 {
        function_source.push_str(&format!("(fn filler-{index} () Bool (effects) true) "));
    }
    function_source.push_str(
        "(fn late () Bool (effects) true) (fn main ((args (Vec Bytes))) I64 (effects) 0))\n",
    );
    let function_path = write_source(&directory, &function_source);
    let function_output = Command::new(slimc())
        .arg("analyze")
        .arg(&function_path)
        .output()
        .unwrap();
    assert!(function_output.status.success());
    let function_report = String::from_utf8(function_output.stdout).unwrap();
    assert!(function_report.contains("(parallelism (guarantee bounded)"));
    assert!(
        function_report
            .contains("needs-late (guarantee unknown) (status unknown) (reason function-limit) (blockers function-limit)")
    );

    let mut edge_source = String::from("(module parallel-edge-bound (record Wide (");
    for index in 0..4097 {
        edge_source.push_str(&format!("(field-{index} Bool)"));
    }
    edge_source
        .push_str(")) (fn leaf () Bool (effects) true) (fn build () Wide (effects) (make Wide ");
    for index in 0..4097 {
        edge_source.push_str(&format!("(field-{index} (call leaf))"));
    }
    edge_source.push_str(")) (fn main ((args (Vec Bytes))) I64 (effects) 0))\n");
    let edge_path = directory.join("edges.slim");
    fs::write(&edge_path, edge_source).unwrap();
    let edge_output = Command::new(slimc())
        .arg("analyze")
        .arg(&edge_path)
        .output()
        .unwrap();
    assert!(edge_output.status.success());
    let edge_report = String::from_utf8(edge_output.stdout).unwrap();
    assert!(edge_report.contains("(parallelism (guarantee bounded)"));
    assert!(edge_report.contains(
        "build (guarantee unknown) (status unknown) (reason edge-limit) (blockers edge-limit)"
    ));

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn project_analysis_dogfoods_the_bounded_parallelism_view() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let output = Command::new(slimc())
        .arg("analyze")
        .arg(root.join("selfhost/slim.project"))
        .output()
        .unwrap();
    assert!(output.status.success());
    assert!(output.stderr.is_empty());
    assert!(report_parentheses_are_balanced(&output.stdout));
    let report = String::from_utf8(output.stdout).unwrap();
    assert!(report.starts_with("(analysis 7 (module project)"));
    assert!(report.contains("(parallelism (guarantee bounded) (function-limit 64)"));
    assert!(report.contains("analysis_binding_active (guarantee exact) (status safe)"));
}

#[test]
fn reduction_proofs_are_deterministic_and_replayed_independently() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/pass/reduction.slim");
    let first = Command::new(slimc())
        .arg("prove-reduction")
        .arg(&source)
        .output()
        .unwrap();
    let second = Command::new(slimc())
        .arg("prove-reduction")
        .arg(&source)
        .output()
        .unwrap();
    assert!(first.status.success());
    assert_eq!(first.stdout, second.stdout);
    let report = String::from_utf8(first.stdout).unwrap();
    assert!(
        report
            .starts_with("(reduction-proof 2 (guarantee bounded) (cost-model canonical-tokens-v1)")
    );
    assert!(report.contains("(pass-limit 8) (site-limit 64)"));
    assert!(report.contains("dead-scalar-binding"));
    assert!(report.contains("right-identity"));
    assert!(report.contains("boolean-idempotence"));
    assert!(report.contains("boolean-identity-match"));
    assert!(report.contains("common-match-result"));
    assert!(report.ends_with(")\n"));

    let directory = temporary_directory("proof-replay");
    let reduced = directory.join("reduced.slim");
    let reduced_output = Command::new(slimc())
        .arg("reduce")
        .arg(&source)
        .output()
        .unwrap();
    assert!(reduced_output.status.success());
    let reduced_source = String::from_utf8(reduced_output.stdout.clone()).unwrap();
    assert!(reduced_source.contains("(fn idempotent ((value Bool)) Bool (effects) value)"));
    assert!(reduced_source.contains("(fn identity-match ((value Bool)) Bool (effects) value)"));
    assert!(
        reduced_source
            .contains("(fn common-result ((condition Bool) (value Bool)) Bool (effects) value)")
    );
    fs::write(&reduced, reduced_output.stdout).unwrap();
    let reduced_again = Command::new(slimc())
        .arg("reduce")
        .arg(&reduced)
        .output()
        .unwrap();
    assert!(reduced_again.status.success());
    assert_eq!(reduced_again.stdout, fs::read(&reduced).unwrap());

    let nonapplicable = Command::new(slimc())
        .arg("reduce")
        .arg(root.join("conformance/evidence/reduction-nonapplicable.slim"))
        .output()
        .unwrap();
    assert!(nonapplicable.status.success());
    let nonapplicable_source = String::from_utf8(nonapplicable.stdout).unwrap();
    assert!(
        nonapplicable_source
            .contains("(call bool.and (call bool.not value) (call bool.not value))")
    );
    assert!(
        nonapplicable_source
            .contains("(match (call bool.not condition) (true value) (false value))")
    );
    assert!(nonapplicable_source.contains("(match condition (true left) (false right))"));
    let verified = Command::new(slimc())
        .arg("verify-reduction")
        .arg(&source)
        .arg(&reduced)
        .output()
        .unwrap();
    assert!(verified.status.success());
    assert_eq!(
        verified.stdout,
        b"(reduction-verification 1 (status verified))\n"
    );

    let different = Command::new(slimc())
        .arg("verify-reduction")
        .arg(&source)
        .arg(root.join("conformance/pass/scalars.slim"))
        .output()
        .unwrap();
    assert!(different.status.success());
    assert_eq!(
        different.stdout,
        b"(reduction-verification 1 (status different))\n"
    );
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn finite_equivalence_proves_or_returns_the_first_counterexample() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let evidence = root.join("conformance/evidence");
    let left = evidence.join("equivalent-left.slim");
    let right = evidence.join("equivalent-right.slim");
    let equivalent = Command::new(slimc())
        .arg("equivalent")
        .arg(&left)
        .arg(&right)
        .output()
        .unwrap();
    assert!(equivalent.status.success());
    assert_eq!(
        equivalent.stdout,
        b"(equivalence 2 (status equivalent) (domain exact) (domain-kind boolean-product) (cases 4) (accepted-states 1) (cost-model expression-tokens-v1) (left-cost 6) (right-cost 12))\n"
    );

    let different = Command::new(slimc())
        .arg("equivalent")
        .arg(&left)
        .arg(evidence.join("different.slim"))
        .output()
        .unwrap();
    assert!(different.status.success());
    assert_eq!(
        different.stdout,
        b"(equivalence 2 (status different) (domain exact) (domain-kind boolean-product) (left-accepted-states 1) (right-accepted-states 3) (counterexample (inputs false true) (left false) (right true)) (cost-model expression-tokens-v1) (left-cost 6) (right-cost 6))\n"
    );

    let unsupported = Command::new(slimc())
        .arg("equivalent")
        .arg(&left)
        .arg(evidence.join("unsupported.slim"))
        .output()
        .unwrap();
    assert!(unsupported.status.success());
    assert_eq!(
        unsupported.stdout,
        b"(equivalence 2 (status unknown) (reason unsupported-signature))\n"
    );

    let u8_left = evidence.join("u8-equivalent-left.slim");
    let u8_right = evidence.join("u8-equivalent-right.slim");
    let u8_equivalent = Command::new(slimc())
        .arg("equivalent")
        .arg(&u8_left)
        .arg(&u8_right)
        .output()
        .unwrap();
    assert!(u8_equivalent.status.success());
    assert_eq!(
        u8_equivalent.stdout,
        b"(equivalence 2 (status equivalent) (domain exact) (domain-kind u8) (cases 256) (accepted-states 2) (cost-model expression-tokens-v1) (left-cost 26) (right-cost 26))\n"
    );

    let u8_different = Command::new(slimc())
        .arg("equivalent")
        .arg(&u8_left)
        .arg(evidence.join("u8-different.slim"))
        .output()
        .unwrap();
    assert!(u8_different.status.success());
    assert_eq!(
        u8_different.stdout,
        b"(equivalence 2 (status different) (domain exact) (domain-kind u8) (left-accepted-states 2) (right-accepted-states 3) (counterexample (inputs 3) (left false) (right true)) (cost-model expression-tokens-v1) (left-cost 26) (right-cost 26))\n"
    );

    let u8_unknown = Command::new(slimc())
        .arg("equivalent")
        .arg(&u8_left)
        .arg(evidence.join("u8-unsupported-expression.slim"))
        .output()
        .unwrap();
    assert!(u8_unknown.status.success());
    assert_eq!(
        u8_unknown.stdout,
        b"(equivalence 2 (status unknown) (reason unsupported-expression))\n"
    );
}

#[test]
fn structural_edits_are_versioned_bounded_and_normally_checked() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let evidence = root.join("conformance/evidence");
    let source = evidence.join("equivalent-left.slim");
    let edited = Command::new(slimc())
        .arg("edit")
        .arg(&source)
        .arg(evidence.join("edit.patch"))
        .output()
        .unwrap();
    assert!(edited.status.success());
    assert_eq!(
        edited.stdout,
        b"(module equivalent-left (fn subject ((a Bool) (b Bool)) Bool (effects) false) (fn main ((args (Vec Bytes))) I64 (effects) 0))\n"
    );

    let malformed = Command::new(slimc())
        .arg("edit")
        .arg(&source)
        .arg(evidence.join("edit-malformed.patch"))
        .output()
        .unwrap();
    assert_eq!(malformed.status.code(), Some(1));
    assert!(malformed.stdout.is_empty());
    assert!(
        String::from_utf8(malformed.stderr)
            .unwrap()
            .contains("E0411@0:0")
    );

    let directory = temporary_directory("edit-normal-check");
    let invalid_patch = directory.join("invalid.patch");
    fs::write(&invalid_patch, "(slim-edit 1 (node 20) (replace 0))\n").unwrap();
    let invalid = Command::new(slimc())
        .arg("--message-format=json")
        .arg("edit")
        .arg(&source)
        .arg(&invalid_patch)
        .output()
        .unwrap();
    assert_eq!(invalid.status.code(), Some(1));
    assert!(invalid.stdout.is_empty());
    assert!(
        String::from_utf8(invalid.stderr)
            .unwrap()
            .contains("\"code\":\"E0344\"")
    );
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn reduction_reuses_normal_diagnostics_and_rejects_projects() {
    let directory = temporary_directory("reduction-diagnostics");
    let malformed = write_source(
        &directory,
        "(module malformed (fn main ((args (Vec Bytes))) I64 (effects) (match true (true 0)))\n",
    );
    let rejected = Command::new(slimc())
        .arg("--message-format=json")
        .arg("reduce")
        .arg(&malformed)
        .output()
        .unwrap();
    assert_eq!(rejected.status.code(), Some(1));
    assert!(rejected.stdout.is_empty());
    let diagnostic = String::from_utf8(rejected.stderr).unwrap();
    assert!(diagnostic.contains("\"code\":\"E0102\""));

    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let project = Command::new(slimc())
        .arg("reduce")
        .arg(root.join("selfhost/slim.project"))
        .output()
        .unwrap();
    assert_eq!(project.status.code(), Some(1));
    assert!(project.stdout.is_empty());
    assert!(
        String::from_utf8(project.stderr)
            .unwrap()
            .contains("E0410@0:0")
    );
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn deep_reduction_falls_back_to_an_idempotent_canonical_program() {
    let directory = temporary_directory("bounded-reduction");
    let mut expression = "true".to_owned();
    // Seven changing passes plus one stability pass are accepted. Eight
    // changing passes must hit the exact D0028 limit and return the fallback.
    for _ in 0..8 {
        expression = format!("(call bool.not {expression})");
    }
    let source_text = format!(
        "(module bounded-reduction (fn main ((args (Vec Bytes))) I64 (effects) (match {expression} (true 0) (false 1))))\n"
    );
    let source = write_source(&directory, &source_text);
    let first = Command::new(slimc())
        .arg("reduce")
        .arg(&source)
        .output()
        .unwrap();
    assert!(first.status.success());
    assert_eq!(first.stdout, source_text.as_bytes());

    let reduced = directory.join("reduced.slim");
    fs::write(&reduced, &first.stdout).unwrap();
    let second = Command::new(slimc())
        .arg("reduce")
        .arg(&reduced)
        .output()
        .unwrap();
    assert!(second.status.success());
    assert_eq!(second.stdout, first.stdout);

    let run = Command::new(slimc())
        .arg("run")
        .arg(&reduced)
        .output()
        .unwrap();
    assert_eq!(run.status.code(), Some(0));
    assert!(run.stdout.is_empty());
    assert!(run.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn passes_program_arguments_explicitly() {
    let directory = temporary_directory("arguments");
    let source = write_source(
        &directory,
        "(module arguments (fn main ((args (Vec Bytes))) I64 (effects io) (let shown Unit (call io.print-i64 (call vec.len args)) (let newline Unit (call io.println \"\") 0))))\n",
    );
    let output = Command::new(slimc())
        .arg("run")
        .arg(&source)
        .arg("--")
        .arg("one")
        .arg("two")
        .output()
        .unwrap();
    assert!(
        output.status.success(),
        "{}",
        String::from_utf8_lossy(&output.stderr)
    );
    assert_eq!(output.stdout, b"3\n");
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn self_hosted_compiler_reaches_a_fixed_point() {
    let output = Command::new(slim_bootstrap()).output().unwrap();
    assert!(
        output.status.success(),
        "{}",
        String::from_utf8_lossy(&output.stderr)
    );
    let stdout = String::from_utf8(output.stdout).unwrap();
    assert!(
        stdout.contains("bootstrap: fixed point verified at ") && stdout.contains(" C bytes"),
        "{stdout}"
    );
    assert!(
        stdout.contains("bootstrap: compiler available at build/toolchain/slimc"),
        "{stdout}"
    );
}

#[test]
fn checks_builds_and_emits_interfaces_for_explicit_project() {
    let directory = temporary_directory("project");
    fs::write(
        directory.join("app.slim"),
        "(module app (fn main ((args (Vec Bytes))) I64 (effects io) (let answer I64 (call math/answer 40) (let shown Unit (call io.print-i64 answer) (let newline Unit (call io.println \"\") 0)))))\n",
    )
    .unwrap();
    fs::write(
        directory.join("math.slim"),
        "(module math (fn answer ((value I64)) I64 (effects) (call i64.add value 2)))\n",
    )
    .unwrap();
    let manifest = directory.join("slim.project");
    fs::write(
        &manifest,
        "(project 1 (entry app) (module app \"app.slim\" (imports math) (exports)) (module math \"math.slim\" (imports) (exports answer)))\n",
    )
    .unwrap();

    let check = Command::new(slimc())
        .arg("check")
        .arg(&manifest)
        .arg("--jobs")
        .arg("2")
        .output()
        .unwrap();
    assert!(
        check.status.success(),
        "{}",
        String::from_utf8_lossy(&check.stderr)
    );

    let executable = directory.join("answer");
    let build = Command::new(slimc())
        .arg("build")
        .arg(&manifest)
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );
    let run = Command::new(&executable).output().unwrap();
    assert!(run.status.success());
    assert_eq!(run.stdout, b"42\n");

    let interfaces = directory.join("interfaces");
    let emitted = Command::new(slimc())
        .arg("interfaces")
        .arg(&manifest)
        .arg("-o")
        .arg(&interfaces)
        .output()
        .unwrap();
    assert!(
        emitted.status.success(),
        "{}",
        String::from_utf8_lossy(&emitted.stderr)
    );
    assert_eq!(
        fs::read_to_string(interfaces.join("math.sli")).unwrap(),
        "(interface 1 math (fn answer ((owned I64)) I64 (effects)))\n"
    );

    let relocated = temporary_directory("project-relocated");
    for file in ["app.slim", "math.slim", "slim.project"] {
        fs::copy(directory.join(file), relocated.join(file)).unwrap();
    }
    let relocated_manifest = relocated.join("slim.project");
    let original_c = directory.join("original.c");
    let relocated_c = relocated.join("relocated.c");
    for (project, output) in [
        (&manifest, &original_c),
        (&relocated_manifest, &relocated_c),
    ] {
        let emitted = Command::new(slimc())
            .arg("emit-c")
            .arg(project)
            .arg("-o")
            .arg(output)
            .output()
            .unwrap();
        assert!(
            emitted.status.success(),
            "{}",
            String::from_utf8_lossy(&emitted.stderr)
        );
    }
    assert_eq!(
        fs::read(original_c).unwrap(),
        fs::read(relocated_c).unwrap()
    );
    let relocated_interfaces = relocated.join("interfaces");
    let emitted = Command::new(slimc())
        .arg("interfaces")
        .arg(&relocated_manifest)
        .arg("-o")
        .arg(&relocated_interfaces)
        .output()
        .unwrap();
    assert!(emitted.status.success());
    assert_eq!(
        fs::read(interfaces.join("math.sli")).unwrap(),
        fs::read(relocated_interfaces.join("math.sli")).unwrap()
    );
    fs::remove_dir_all(relocated).unwrap();
    fs::remove_dir_all(directory).unwrap();
}
