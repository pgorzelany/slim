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

fn nested_refinement_expression(remaining: usize, depth: usize) -> String {
    let indentation = "  ".repeat(depth);
    if remaining == 0 {
        return format!("{indentation}value\n");
    }
    format!(
        "{indentation}if value < 10:\n{}{indentation}else:\n{indentation}  value\n",
        nested_refinement_expression(remaining - 1, depth + 1)
    )
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
        "module answer\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64(42)\n  io.println(\"\")\n  0\n",
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
fn explicit_structured_parallel_joins_loopback_requests_and_adopts_owned_results() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("explicit-parallel");
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
    assert!(generated.contains("parallel:\n    let first: Reply"));
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
        "module nested_plan\n\nfn run(remaining: I64, state: Bool) -> Bool effects[partial]:\n  if (remaining <= 0):\n    state\n  else:\n    recur((remaining - 1), !state)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  if true:\n    let left: Bool = run(1000000, true)\n    let right: Bool = run(1000000, false)\n    0\n  else:\n    0\n",
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
        "module deterministic\n\nfn main(args: Vec[Bytes]) -> I64:\n  40 + 2\n",
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
    assert!(generated.contains("slim_result = INT64_C(40) + INT64_C(2);"));
    assert!(!generated.contains("slim_i64_add"));
    assert!(generated.contains("slim_fn_main"));

    let unknown = write_source(
        &directory,
        "module unknown_arithmetic\n\nfn add_one(value: I64) -> I64 effects[partial]:\n  (value + 1)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  let first: I64 = add_one(41)\n  add_one(first)\n",
    );
    let unknown_generated = directory.join("unknown.c");
    let status = Command::new(slimc())
        .arg("emit-c")
        .arg(unknown)
        .arg("-o")
        .arg(&unknown_generated)
        .status()
        .unwrap();
    assert!(status.success());
    assert!(
        fs::read_to_string(unknown_generated)
            .unwrap()
            .contains("slim_result = slim_i64_add(slim_v_value, INT64_C(1));"),
        "unknown arithmetic must retain its checked runtime operation"
    );
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn proven_parameter_constants_remove_only_supported_arithmetic_checks() {
    let directory = temporary_directory("parameter-constants");

    let exact = write_source(
        &directory,
        "module exact_parameter\n\nfn quotient(value: I64, divisor: I64) -> I64 effects[partial]:\n  (value / divisor)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  quotient(84, 2)\n",
    );
    let exact_c = directory.join("exact.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(exact)
            .arg("-o")
            .arg(&exact_c)
            .status()
            .unwrap()
            .success()
    );
    let exact_generated = fs::read_to_string(exact_c).unwrap();
    assert!(exact_generated.contains("slim_result = slim_v_value / slim_v_divisor;"));
    assert!(!exact_generated.contains("slim_i64_div"));

    let conflicting = write_source(
        &directory,
        "module conflicting_parameters\n\nfn quotient(value: I64, divisor: I64) -> I64 effects[partial]:\n  (value / divisor)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  let first: I64 = quotient(84, 2)\n  quotient(first, 3)\n",
    );
    let conflicting_c = directory.join("conflicting.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(conflicting)
            .arg("-o")
            .arg(&conflicting_c)
            .status()
            .unwrap()
            .success()
    );
    let conflicting_generated = fs::read_to_string(conflicting_c).unwrap();
    assert!(
        conflicting_generated.contains("slim_result = slim_v_value / slim_v_divisor;"),
        "a joined positive divisor interval should prove division total"
    );
    assert!(!conflicting_generated.contains("slim_i64_div"));

    let possible_zero = write_source(
        &directory,
        "module possible_zero_parameter\n\nfn quotient(value: I64, divisor: I64) -> I64 effects[partial]:\n  (value / divisor)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  if true:\n    quotient(84, 2)\n  else:\n    quotient(84, 0)\n",
    );
    let possible_zero_c = directory.join("possible-zero.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(possible_zero)
            .arg("-o")
            .arg(&possible_zero_c)
            .status()
            .unwrap()
            .success()
    );
    assert!(
        fs::read_to_string(possible_zero_c)
            .unwrap()
            .contains("slim_result = slim_i64_div(slim_v_value, slim_v_divisor);"),
        "a divisor interval containing zero must retain the checked operation"
    );

    let changed_recurrence = write_source(
        &directory,
        "module changed_recurrence\n\nfn quotient_loop(index: I64, limit: I64, divisor: I64) -> I64 effects[partial]:\n  if (index == limit):\n    index\n  else:\n    let quotient: I64 = (index / divisor)\n    recur((index + 1), limit, (divisor + 1))\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  quotient_loop(0, 10, 2)\n",
    );
    let changed_recurrence_c = directory.join("changed-recurrence.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(changed_recurrence)
            .arg("-o")
            .arg(&changed_recurrence_c)
            .status()
            .unwrap()
            .success()
    );
    let changed_recurrence_generated = fs::read_to_string(changed_recurrence_c).unwrap();
    assert!(
        changed_recurrence_generated.contains("slim_v_quotient = slim_v_index / slim_v_divisor;"),
        "a positively bounded recurrence accumulator should prove division total"
    );

    let decreasing_divisor = write_source(
        &directory,
        "module decreasing_divisor\n\nfn quotient_loop(index: I64, limit: I64, divisor: I64) -> I64 effects[partial]:\n  if (index == limit):\n    index\n  else:\n    let quotient: I64 = (index / divisor)\n    recur((index + 1), limit, (divisor - 1))\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  quotient_loop(0, 10, 2)\n",
    );
    let decreasing_divisor_c = directory.join("decreasing-divisor.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(decreasing_divisor)
            .arg("-o")
            .arg(&decreasing_divisor_c)
            .status()
            .unwrap()
            .success()
    );
    assert!(
        fs::read_to_string(decreasing_divisor_c)
            .unwrap()
            .contains("slim_v_quotient = slim_i64_div(slim_v_index, slim_v_divisor);"),
        "an unsupported decreasing recurrence must retain checked division"
    );

    let bounded = write_source(
        &directory,
        "module bounded_parameter_propagation\n\nfn deepest(value: I64, divisor: I64) -> I64 effects[partial]:\n  (value / divisor)\n\nfn level_four(value: I64, divisor: I64) -> I64 effects[partial]:\n  deepest(value, divisor)\n\nfn level_three(value: I64, divisor: I64) -> I64 effects[partial]:\n  level_four(value, divisor)\n\nfn level_two(value: I64, divisor: I64) -> I64 effects[partial]:\n  level_three(value, divisor)\n\nfn level_one(value: I64, divisor: I64) -> I64 effects[partial]:\n  level_two(value, divisor)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  level_one(84, 2)\n",
    );
    let bounded_c = directory.join("bounded.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(bounded)
            .arg("-o")
            .arg(&bounded_c)
            .status()
            .unwrap()
            .success()
    );
    assert!(
        fs::read_to_string(bounded_c)
            .unwrap()
            .contains("slim_result = slim_i64_div(slim_v_value, slim_v_divisor);"),
        "facts beyond the fixed propagation budget must remain unknown"
    );

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn exact_counted_recurrences_expand_only_under_complete_proof() {
    let directory = temporary_directory("counted-recurrence-expansion");

    let early_result = write_source(
        &directory,
        "module counted_stages\n\nfn count(index: I64, total: I64) -> I64 effects[partial]:\n  if (index == 4):\n    total\n  else:\n    if (index == 2):\n      recur((index + 1), total)\n    else:\n      if (index == 3):\n        42\n      else:\n        recur((index + 1), (total + 1))\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  count(0, 0)\n",
    );
    let early_c = directory.join("early.c");
    let emitted = Command::new(slimc())
        .arg("emit-c")
        .arg(&early_result)
        .arg("-o")
        .arg(&early_c)
        .output()
        .unwrap();
    assert!(
        emitted.status.success(),
        "{}",
        String::from_utf8_lossy(&emitted.stderr)
    );
    let early_generated = fs::read_to_string(&early_c).unwrap();
    assert_eq!(
        early_generated
            .matches("if (slim_v_index < INT64_C(4)) do {")
            .count(),
        4
    );
    assert!(!early_generated.contains("while (slim_v_index"));

    let analysis = Command::new(slimc())
        .arg("analyze")
        .arg(&early_result)
        .output()
        .unwrap();
    assert!(analysis.status.success());
    let report = String::from_utf8(analysis.stdout).unwrap();
    assert!(report.contains("(counted-loops (fact-limit 64)"));
    assert!(report.contains("(start 0) (bound 4) (step 1) (iterations 4)"));
    assert!(report.contains(
        "(counted-loop-count 1) (reported-facts 1) (facts-truncated false) (guarantee exact)"
    ));

    let executable = directory.join("early");
    assert!(
        Command::new(slimc())
            .arg("build")
            .arg(&early_result)
            .arg("-o")
            .arg(&executable)
            .status()
            .unwrap()
            .success()
    );
    assert_eq!(Command::new(executable).status().unwrap().code(), Some(42));

    let maximum = write_source(
        &directory,
        "module maximum_counted_stages\n\nfn count(index: I64) -> I64 effects[partial]:\n  if (index == 16):\n    index\n  else:\n    recur((index + 1))\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  count(0)\n",
    );
    let maximum_c = directory.join("maximum.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&maximum)
            .arg("-o")
            .arg(&maximum_c)
            .status()
            .unwrap()
            .success()
    );
    assert_eq!(
        fs::read_to_string(maximum_c)
            .unwrap()
            .matches("if (slim_v_index < INT64_C(16)) do {")
            .count(),
        16
    );

    let conflicting = write_source(
        &directory,
        "module conflicting_counted_starts\n\nfn count(index: I64) -> I64 effects[partial]:\n  if (index == 4):\n    index\n  else:\n    recur((index + 1))\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  let first: I64 = count(0)\n  count(1)\n",
    );
    let conflicting_c = directory.join("conflicting.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&conflicting)
            .arg("-o")
            .arg(&conflicting_c)
            .status()
            .unwrap()
            .success()
    );
    let conflicting_generated = fs::read_to_string(conflicting_c).unwrap();
    assert!(!conflicting_generated.contains("do {"));
    assert!(conflicting_generated.contains("slim_recur: ;"));

    let over_budget = write_source(
        &directory,
        "module over_budget_counted_stages\n\nfn count(index: I64) -> I64 effects[partial]:\n  if (index == 17):\n    index\n  else:\n    recur((index + 1))\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  count(0)\n",
    );
    let over_budget_c = directory.join("over-budget.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&over_budget)
            .arg("-o")
            .arg(&over_budget_c)
            .status()
            .unwrap()
            .success()
    );
    let over_budget_generated = fs::read_to_string(over_budget_c).unwrap();
    assert!(!over_budget_generated.contains("do {"));
    assert!(over_budget_generated.contains("slim_recur: ;"));

    let explicit_fork = write_source(
        &directory,
        "module forked_counted_stages\n\nfn count(index: I64) -> I64 effects[partial]:\n  if (index == 4):\n    index\n  else:\n    parallel:\n      recur((index + 1))\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  count(0)\n",
    );
    let fork_check = Command::new(slimc())
        .arg("check")
        .arg(&explicit_fork)
        .output()
        .unwrap();
    assert!(!fork_check.status.success());
    assert!(
        String::from_utf8(fork_check.stderr)
            .unwrap()
            .contains("E0356")
    );

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn bounded_ranges_cross_calls_and_version_collection_checks_safely() {
    let directory = temporary_directory("versioned-collection-checks");

    let propagated = write_source(
        &directory,
        "module bounded_call_ranges\n\nfn consume(value: I64) -> I64 effects[partial]:\n  (value + 1)\n\nfn normalize(value: I64) -> I64 effects[partial]:\n  let bounded: I64 = (value % 10)\n  consume(bounded)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  if true:\n    normalize(0)\n  else:\n    normalize(100)\n",
    );
    let propagated_c = directory.join("propagated.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&propagated)
            .arg("-o")
            .arg(&propagated_c)
            .status()
            .unwrap()
            .success()
    );
    let propagated_generated = fs::read_to_string(propagated_c).unwrap();
    assert!(propagated_generated.contains("slim_v_value % INT64_C(10)"));
    assert!(propagated_generated.contains("slim_v_value + INT64_C(1)"));
    assert!(!propagated_generated.contains("slim_i64_add(slim_v_value"));
    let propagated_report = Command::new(slimc())
        .arg("analyze")
        .arg(&propagated)
        .output()
        .unwrap();
    assert!(propagated_report.status.success());
    let propagated_report = String::from_utf8(propagated_report.stdout).unwrap();
    assert!(propagated_report.contains("(status total) (lower 0) (upper 9)"));
    assert!(propagated_report.contains("(status total) (lower 1) (upper 10)"));

    let fallback_success = write_source(
        &directory,
        "module versioned_get_fallback\n\nfn read(inout values: Vec[I64], index: I64) -> I64 effects[partial]:\n  vec.get(values, index)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(values, 42)\n  if true:\n    read(values, 0)\n  else:\n    read(values, 9)\n",
    );
    let fallback_c = directory.join("fallback.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&fallback_success)
            .arg("-o")
            .arg(&fallback_c)
            .status()
            .unwrap()
            .success()
    );
    let fallback_generated = fs::read_to_string(&fallback_c).unwrap();
    assert!(fallback_generated.contains(".len > INT64_C(9) ? slim_v_index"));
    assert!(fallback_generated.contains("slim_vec_check_index"));
    let fallback_executable = directory.join("fallback");
    assert!(
        Command::new(slimc())
            .arg("build")
            .arg(&fallback_success)
            .arg("-o")
            .arg(&fallback_executable)
            .status()
            .unwrap()
            .success()
    );
    assert_eq!(
        Command::new(fallback_executable).status().unwrap().code(),
        Some(42)
    );

    let fallback_trap = write_source(
        &directory,
        "module versioned_get_trap\n\nfn read(inout values: Vec[I64], index: I64) -> I64 effects[partial]:\n  vec.get(values, index)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(values, 42)\n  if false:\n    read(values, 0)\n  else:\n    read(values, 9)\n",
    );
    let trap_executable = directory.join("trap");
    assert!(
        Command::new(slimc())
            .arg("build")
            .arg(&fallback_trap)
            .arg("-o")
            .arg(&trap_executable)
            .status()
            .unwrap()
            .success()
    );
    let trapped = Command::new(trap_executable).output().unwrap();
    assert!(!trapped.status.success());
    assert!(
        String::from_utf8(trapped.stderr)
            .unwrap()
            .contains("index out of bounds")
    );

    let versioned_set = write_source(
        &directory,
        "module versioned_set_fallback\n\nfn store(inout values: Vec[I64], index: I64, value: I64) -> Void effects[partial]:\n  vec.set(values, index, value)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(values, 0)\n  if true:\n    store(values, 0, 42)\n  else:\n    store(values, 9, 42)\n  vec.get(values, 0)\n",
    );
    let set_c = directory.join("set.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&versioned_set)
            .arg("-o")
            .arg(&set_c)
            .status()
            .unwrap()
            .success()
    );
    let set_generated = fs::read_to_string(&set_c).unwrap();
    assert!(set_generated.contains(".len > INT64_C(9) ? slim_v_index"));
    let set_executable = directory.join("set");
    assert!(
        Command::new(slimc())
            .arg("build")
            .arg(&versioned_set)
            .arg("-o")
            .arg(&set_executable)
            .status()
            .unwrap()
            .success()
    );
    assert_eq!(
        Command::new(set_executable).status().unwrap().code(),
        Some(42)
    );

    let negative = write_source(
        &directory,
        "module negative_index\n\nfn read(inout values: Vec[I64], index: I64) -> I64 effects[partial]:\n  vec.get(values, index)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(values, 42)\n  read(values, -1)\n",
    );
    let negative_c = directory.join("negative.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&negative)
            .arg("-o")
            .arg(&negative_c)
            .status()
            .unwrap()
            .success()
    );
    let negative_generated = fs::read_to_string(negative_c).unwrap();
    assert!(!negative_generated.contains(".len > INT64_C("));
    assert!(negative_generated.contains("slim_vec_check_index"));

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn typed_vector_set_preserves_aggregate_values_and_bounds_checks() {
    let directory = temporary_directory("typed-vector-set");
    let source = write_source(
        &directory,
        "module typed_vector_set\n\nstruct Pair:\n  left: I64\n  right: I64\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let pairs: Vec[Pair] = vec.new()\n  let first: Pair = Pair(left: 1, right: 2)\n  vec.push(pairs, first)\n  let second: Pair = Pair(left: 20, right: 22)\n  vec.set(pairs, 0, second)\n  let result: Pair = vec.get(pairs, 0)\n  (result.left + result.right)\n",
    );
    let generated = directory.join("program.c");
    assert!(
        Command::new(slimc())
            .arg("emit-c")
            .arg(&source)
            .arg("-o")
            .arg(&generated)
            .status()
            .unwrap()
            .success()
    );
    let generated = fs::read_to_string(generated).unwrap();
    assert!(!generated.contains("slim_vec_set("));
    assert!(generated.contains(".len > INT64_C(0)"));
    assert!(generated.contains("slim_vec_check_index"));
    assert!(generated.contains("] = slim_v_second;"));

    let executable = directory.join("program");
    assert!(
        Command::new(slimc())
            .arg("build")
            .arg(source)
            .arg("-o")
            .arg(&executable)
            .status()
            .unwrap()
            .success()
    );
    let run = Command::new(executable).output().unwrap();
    assert_eq!(run.status.code(), Some(42));
    assert!(run.stdout.is_empty());
    assert!(run.stderr.is_empty());

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn propagates_typed_allocation_failure() {
    let directory = temporary_directory("allocation-failure");
    let source = write_source(
        &directory,
        "module allocation_failure\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()\n  vec.push(values, 42)\n  0\n",
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
        "module bad\n\nfn main(args: Vec[Bytes]) -> I64:\n  match true:\n    true:\n      missing\n",
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
        "module formatted\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n",
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
fn rejects_every_removed_pre_09_spelling() {
    let directory = temporary_directory("legacy-syntax");
    let cases = [
        (
            "record",
            "module legacy\n\nrecord Old:\n  value: I64\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n",
        ),
        (
            "variant",
            "module legacy\n\nvariant Old:\n  None\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n",
        ),
        (
            "Unit",
            "module legacy\n\nfn old() -> Unit:\n  unit\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n",
        ),
        (
            "fork",
            "module legacy\n\nfn main(args: Vec[Bytes]) -> I64:\n  fork:\n    0\n",
        ),
        (
            "make",
            "module legacy\n\nstruct Old:\n  value: I64\n\nfn main(args: Vec[Bytes]) -> I64:\n  let old: Old = make Old(value = 0)\n  0\n",
        ),
        (
            "get",
            "module legacy\n\nstruct Old:\n  value: I64\n\nfn main(args: Vec[Bytes]) -> I64:\n  let old: Old = Old(value: 0)\n  get(old value)\n",
        ),
        (
            "case",
            "module legacy\n\nenum Old:\n  Some(I64)\n\nfn main(args: Vec[Bytes]) -> I64:\n  let old: Old = case Old::Some(0)\n  0\n",
        ),
        (
            "set",
            "module legacy\n\nfn main(args: Vec[Bytes]) -> I64:\n  var value: I64 = 0\n  set value = 1\n  value\n",
        ),
        (
            "missing commas",
            "module legacy\n\nfn add(left: I64 right: I64) -> I64:\n  left + right\n\nfn main(args: Vec[Bytes]) -> I64:\n  add(20 22)\n",
        ),
        (
            "kebab identifier",
            "module legacy\n\nfn old-name() -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n",
        ),
        (
            "slash qualification",
            "module legacy\n\nfn main(args: Vec[Bytes]) -> I64:\n  math/answer(40)\n",
        ),
        (
            "named arithmetic",
            "module legacy\n\nfn main(args: Vec[Bytes]) -> I64:\n  i64.add(20, 22)\n",
        ),
        (
            "hyphenated builtin",
            "module legacy\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print-i64(42)\n  0\n",
        ),
    ];
    for (index, (label, source)) in cases.into_iter().enumerate() {
        let path = directory.join(format!("legacy-{index}.slim"));
        fs::write(&path, source).unwrap();
        let output = Command::new(slimc())
            .arg("check")
            .arg(path)
            .output()
            .unwrap();
        assert!(!output.status.success(), "legacy {label} was accepted");
    }
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
    assert!(report.starts_with("(analysis 7 (module vector_sum)"));
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
        "(analysis 7 (module integer_ranges)",
        "(integer-proofs (domain -1000000000 1000000000)",
        "(refinement-limit 64) (parameter-pass-limit 4)",
        "(refinements 6) (refinements-truncated false)",
        "(checked-site 26 (status total) (lower 6) (upper 6))",
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
        "guarded_upper (guarantee exact) (status safe)",
        "guarded_lower (guarantee exact) (status safe)",
        "exact_arithmetic (guarantee exact) (status safe)",
        "unguarded (guarantee exact) (status unavailable) (reason checked-trap)",
        "zero_divisor (guarantee exact) (status unavailable) (reason checked-trap)",
        "domain_limit (guarantee exact) (status unavailable) (reason checked-trap)",
        "constant_remainder (guarantee exact) (status safe) (blockers)",
        "constant_division (guarantee exact) (status safe) (blockers)",
        "possible_division_overflow (guarantee exact) (status unavailable) (reason checked-trap)",
        "total_countdown (guarantee exact) (status safe) (blockers)",
        "total_countdown (guarantee exact) (effects partial) (cost-vector 1",
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
        "(analysis 7 (module resource_work)",
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

    let mut profiles = "module resource_profile_bound\n".to_owned();
    for index in 0..17 {
        profiles.push_str(&format!(
            "fn run_{index}(remaining: I64) -> I64 effects[partial]:\n  if remaining <= 0:\n    0\n  else:\n    recur(remaining - 1)\n\n"
        ));
    }
    profiles.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
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

    let mut body = String::new();
    for index in 0..65 {
        body.push_str(&format!("  let value_{index}: I64 = run({index})\n"));
    }
    body.push_str("  0");
    let calls = format!(
        "module resource_call_bound\n\nfn run(remaining: I64) -> I64 effects[partial]:\n  if remaining <= 0:\n    0\n  else:\n    recur(remaining - 1)\n\nfn many() -> I64 effects[partial]:\n{body}\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
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
    let body = nested_refinement_expression(33, 1);
    let source = format!(
        "module integer_range_bound\n\nfn nested(value: I64) -> I64:\n{body}\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
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
    assert!(report.contains("(refinement-limit 64) (parameter-pass-limit 4) (refinements 64)"));
    assert!(report.contains("(refinements-truncated true)"));
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn integer_checked_site_report_limit_is_explicit_and_deterministic() {
    let directory = temporary_directory("integer-site-bound");
    let mut source = "module integer_site_bound\n".to_owned();
    for index in 0..65 {
        source.push_str(&format!("fn f{index}(value: I64) -> I64:\n  value + 1\n\n"));
    }
    source.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
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
        "safe_left (guarantee exact) (status safe) (blockers)",
        "safe_right (guarantee exact) (status safe) (blockers)",
        "overdeclared (guarantee exact) (status safe) (blockers)",
        "traps (guarantee exact) (status unavailable) (reason checked-trap) (blockers checked-trap)",
        "calls_trap (guarantee exact) (status unavailable) (reason callee-not-safe) (blockers callee-not-safe)",
        "allocates (guarantee exact) (status unavailable) (reason allocation-or-io) (blockers allocation-or-io)",
        "borrows (guarantee exact) (status unavailable) (reason exclusive-borrow) (blockers exclusive-borrow)",
        "mutates (guarantee exact) (status unavailable) (reason mutation) (blockers mutation)",
        "repeats (guarantee exact) (status unavailable) (reason recurrence) (blockers recurrence)",
        "countdown (guarantee exact) (status safe) (blockers)",
        "countdown_pair (guarantee exact) (status safe) (blockers)",
        "overlap (guarantee exact) (status safe) (blockers)",
        "cycle_left (guarantee unknown) (status unknown) (reason call-cycle) (blockers call-cycle)",
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
    let mut body = String::new();
    for index in 0..130 {
        body.push_str(&format!("  let task_{index}: Bool = work()\n"));
    }
    body.push_str("  true");
    let source = format!(
        "module parallel_schedule_bound\n\nfn work() -> Bool:\n  true\n\nfn plan() -> Bool:\n{body}\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
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

    let mut function_source =
        String::from("module parallel_function_bound\n\nfn needs_late() -> Bool:\n  late()\n");
    for index in 0..63 {
        function_source.push_str(&format!("fn filler_{index}() -> Bool:\n  true\n\n"));
    }
    function_source
        .push_str("fn late() -> Bool:\n  true\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n");
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
            .contains("needs_late (guarantee unknown) (status unknown) (reason function-limit) (blockers function-limit)")
    );

    let mut edge_source = String::from("module parallel_edge_bound\n\nstruct Wide:\n");
    for index in 0..4097 {
        edge_source.push_str(&format!("  field_{index}: Bool\n"));
    }
    edge_source.push_str("\nfn leaf() -> Bool:\n  true\n\nfn build() -> Wide:\n  Wide(\n");
    for index in 0..4097 {
        let separator = if index == 4096 { "" } else { "," };
        edge_source.push_str(&format!("    field_{index}: leaf(){separator}\n"));
    }
    edge_source.push_str("  )\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n");
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
    assert!(reduced_source.contains("fn idempotent(value: Bool) -> Bool:\n  value"));
    assert!(reduced_source.contains("fn identity_match(value: Bool) -> Bool:\n  value"));
    assert!(
        reduced_source.contains("fn common_result(condition: Bool, value: Bool) -> Bool:\n  value")
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
    assert!(nonapplicable_source.contains("(!value) && (!value)"));
    assert!(nonapplicable_source.contains("if !condition:\n    value\n  else:\n    value"));
    assert!(nonapplicable_source.contains("if condition:\n    left\n  else:\n    right"));
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
        b"module equivalent_left\n\nfn subject(a: Bool, b: Bool) -> Bool:\n  false\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
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
        "module malformed\n\nfn main(args: Vec[Bytes]) -> I64:\n",
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
    // changing passes must hit the exact RFC-0028 limit and return the fallback.
    for _ in 0..8 {
        expression = format!("!({expression})");
    }
    let source_text = format!(
        "module bounded_reduction\n\nfn main(args: Vec[Bytes]) -> I64:\n  if {expression}:\n    0\n  else:\n    1\n"
    );
    let source = write_source(&directory, &source_text);
    let first = Command::new(slimc())
        .arg("reduce")
        .arg(&source)
        .output()
        .unwrap();
    assert!(first.status.success());
    assert_ne!(first.stdout, source_text.as_bytes());

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
        "module arguments\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64(vec.len(args))\n  io.println(\"\")\n  0\n",
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
        "module app\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  let answer: I64 = math.answer(40)\n  io.print_i64(answer)\n  io.println(\"\")\n  0\n",
    )
    .unwrap();
    fs::write(
        directory.join("math.slim"),
        "module math\n\nfn answer(value: I64) -> I64:\n  value + 2\n",
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
        "(interface 2 math (fn answer ((owned I64)) I64 (effects)))\n"
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
