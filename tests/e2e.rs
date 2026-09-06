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

// Optimization/ABI assertions intentionally ignore local identifier suffixes.
// Binding identity itself is tested through raw C and native lexical-scope cases.
fn codegen_without_local_ids(source: &str) -> String {
    source
        .split_inclusive(|c: char| !c.is_ascii_alphanumeric() && c != '_')
        .map(|piece| {
            let end = piece
                .bytes()
                .take_while(|byte| byte.is_ascii_alphanumeric() || *byte == b'_')
                .count();
            let token = &piece[..end];
            if token.starts_with("slim_v_")
                && let Some((base, digits)) = token.rsplit_once("_n")
                && !digits.is_empty()
                && digits.bytes().all(|byte| byte.is_ascii_digit())
            {
                return format!("{base}{}", &piece[end..]);
            }
            piece.to_owned()
        })
        .collect()
}

fn read_codegen_for_assertions(path: impl AsRef<Path>) -> std::io::Result<String> {
    fs::read_to_string(path).map(|source| codegen_without_local_ids(&source))
}

#[test]
fn experimental_standard_library_is_canonical_and_deterministic() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let project = root.join("library/slim.project");

    let formatted = Command::new(slimc())
        .args(["fmt", project.to_str().unwrap(), "--check"])
        .output()
        .unwrap();
    assert!(
        formatted.status.success(),
        "{}",
        String::from_utf8_lossy(&formatted.stderr)
    );

    let checked = Command::new(slimc())
        .arg("check")
        .arg(&project)
        .output()
        .unwrap();
    assert!(
        checked.status.success(),
        "{}",
        String::from_utf8_lossy(&checked.stderr)
    );

    let first = Command::new(slimc())
        .arg("run")
        .arg(&project)
        .output()
        .unwrap();
    let second = Command::new(slimc())
        .arg("run")
        .arg(&project)
        .output()
        .unwrap();
    assert!(first.status.success());
    assert_eq!(first.stdout, second.stdout);
    assert_eq!(
        first.stdout,
        b"ok ascii\nok i64\nok span\nok text\nok bytes\nok ascii-colon\nok decimal\nok cursor\nok args\nok i64-vec\nok u8-vec\npassed 11 failed 0\n"
    );
    assert!(first.stderr.is_empty());
    assert!(second.stderr.is_empty());

    let directory = temporary_directory("stdlib-interfaces");
    let left = directory.join("left");
    let right = directory.join("right");
    for output in [&left, &right] {
        let interfaces = Command::new(slimc())
            .arg("interfaces")
            .arg(&project)
            .arg("-o")
            .arg(output)
            .output()
            .unwrap();
        assert!(
            interfaces.status.success(),
            "{}",
            String::from_utf8_lossy(&interfaces.stderr)
        );
    }
    for module in [
        "std_ascii",
        "std_bytes",
        "std_cursor",
        "std_decimal",
        "std_i64",
        "std_i64_vec",
        "std_span",
        "std_test",
        "std_text",
        "std_u8_vec",
    ] {
        assert_eq!(
            fs::read(left.join(format!("{module}.sli"))).unwrap(),
            fs::read(right.join(format!("{module}.sli"))).unwrap()
        );
    }

    let executable = directory.join("standard-library-tests");
    let built = Command::new(slimc())
        .arg("build")
        .arg(&project)
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        built.status.success(),
        "{}",
        String::from_utf8_lossy(&built.stderr)
    );
    let allocation_failure = Command::new(&executable)
        .env("SLIM_ALLOC_FAIL_AT", "1")
        .output()
        .unwrap();
    assert_eq!(allocation_failure.status.code(), Some(71));
    assert!(allocation_failure.stdout.is_empty());
    assert_eq!(
        allocation_failure.stderr,
        b"SLIM allocation failure: exhausted at allocation 1\n"
    );
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn interface_diff_reports_compatible_and_breaking_api_changes() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("interface-diff");
    let executable = directory.join("slim-api-diff");
    let build = Command::new(slimc())
        .arg("build")
        .arg(root.join("library/api-diff.project"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let fixtures = root.join("library/tools/fixtures");
    let previous = fixtures.join("api-previous.sli");
    let current = fixtures.join("api-current.sli");
    let breaking = Command::new(&executable)
        .arg(&previous)
        .arg(&current)
        .output()
        .unwrap();
    assert_eq!(breaking.status.code(), Some(1));
    assert_eq!(
        breaking.stdout,
        b"interface-diff 1\nremoved remove\nchanged update\nadded added\nsummary added 1 changed 1 removed 1\ncompatible no\n"
    );
    assert!(breaking.stderr.is_empty());

    let compatible = Command::new(&executable)
        .arg(&current)
        .arg(&current)
        .output()
        .unwrap();
    assert!(compatible.status.success());
    assert_eq!(
        compatible.stdout,
        b"interface-diff 1\nsummary added 0 changed 0 removed 0\ncompatible yes\n"
    );
    assert!(compatible.stderr.is_empty());

    let malformed = directory.join("malformed.sli");
    fs::write(&malformed, b"(interface 2 sample)\n").unwrap();
    let rejected = Command::new(&executable)
        .arg(&malformed)
        .arg(&current)
        .output()
        .unwrap();
    assert_eq!(rejected.status.code(), Some(65));
    assert_eq!(rejected.stdout, b"interface-diff: invalid interface\n");
    assert!(rejected.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn ndjson_application_parses_nested_json_and_validates_telemetry() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("ndjson");
    let executable = directory.join("slim-ndjson");
    let build = Command::new(slimc())
        .arg("build")
        .arg(root.join("library/ndjson.project"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let fixtures = root.join("library/applications/ndjson/fixtures");
    let valid = Command::new(&executable)
        .arg(fixtures.join("telemetry.ndjson"))
        .output()
        .unwrap();
    assert!(valid.status.success());
    assert_eq!(
        valid.stdout,
        b"device alpha count 2 ok 1 sum 52\ndevice beta count 1 ok 0 sum -5\n"
    );
    assert!(valid.stderr.is_empty());

    for (fixture, expected) in [
        ("duplicate.ndjson", "error 11 at 2\n"),
        ("fraction.ndjson", "error 17 at 37\n"),
        ("malformed.ndjson", "error 5 at 64\n"),
        ("nested-unknown.ndjson", "error 12 at 39\n"),
        ("overflow.ndjson", "error 18 at 54\n"),
        ("unseparated.ndjson", "error 6 at 38\n"),
    ] {
        let rejected = Command::new(&executable)
            .arg(fixtures.join(fixture))
            .output()
            .unwrap();
        assert_eq!(rejected.status.code(), Some(1), "{fixture}");
        assert_eq!(rejected.stdout, expected.as_bytes(), "{fixture}");
        assert!(rejected.stderr.is_empty(), "{fixture}");
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn ledger_application_preserves_rejected_transaction_state() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("ledger");
    let executable = directory.join("slim-ledger");
    let build = Command::new(slimc())
        .arg("build")
        .arg(root.join("library/ledger.project"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let fixtures = root.join("library/applications/ledger/fixtures");
    let valid = Command::new(&executable)
        .arg(fixtures.join("ledger.log"))
        .output()
        .unwrap();
    assert!(valid.status.success());
    assert_eq!(
        valid.stdout,
        b"accepted 6 rejected 2\naccount alice balance 80 open yes credits 1 debits 1\naccount bob balance 0 open no credits 1 debits 1\ntotal 80\n"
    );
    assert!(valid.stderr.is_empty());

    let rollback = Command::new(&executable)
        .arg(fixtures.join("rollback.log"))
        .output()
        .unwrap();
    assert!(rollback.status.success());
    assert_eq!(
        rollback.stdout,
        b"accepted 2 rejected 1\naccount alice balance 10 open yes credits 0 debits 0\naccount bob balance 0 open yes credits 0 debits 0\ntotal 10\n"
    );
    assert!(rollback.stderr.is_empty());

    for (fixture, expected) in [
        ("invalid.log", "error 20 at 0\n"),
        ("overflow.log", "error 24 at 29\n"),
        ("trailing.log", "error 22 at 13\n"),
    ] {
        let rejected = Command::new(&executable)
            .arg(fixtures.join(fixture))
            .output()
            .unwrap();
        assert_eq!(rejected.status.code(), Some(1), "{fixture}");
        assert_eq!(rejected.stdout, expected.as_bytes(), "{fixture}");
        assert!(rejected.stderr.is_empty(), "{fixture}");
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn sat_application_solves_and_rejects_dimacs_inputs() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("sat");
    let executable = directory.join("slim-sat");
    let build = Command::new(slimc())
        .arg("build")
        .arg(root.join("library/sat.project"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let fixtures = root.join("library/applications/sat/fixtures");
    let satisfiable = Command::new(&executable)
        .arg(fixtures.join("sat.cnf"))
        .output()
        .unwrap();
    assert!(satisfiable.status.success());
    assert_eq!(satisfiable.stdout, b"s SATISFIABLE\nv -1 2 3 4 0\n");
    assert!(satisfiable.stderr.is_empty());

    let unsatisfiable = Command::new(&executable)
        .arg(fixtures.join("unsat.cnf"))
        .output()
        .unwrap();
    assert!(unsatisfiable.status.success());
    assert_eq!(unsatisfiable.stdout, b"s UNSATISFIABLE\n");
    assert!(unsatisfiable.stderr.is_empty());

    let invalid = Command::new(&executable)
        .arg(fixtures.join("invalid.cnf"))
        .output()
        .unwrap();
    assert_eq!(invalid.status.code(), Some(65));
    assert_eq!(invalid.stdout, b"error 14 at 10\n");
    assert!(invalid.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn raster_application_emits_deterministic_pgm_pixels() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("raster");
    let executable = directory.join("slim-raster");
    let build = Command::new(slimc())
        .arg("build")
        .arg(root.join("library/raster.project"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let fixtures = root.join("library/applications/raster/fixtures");
    let image = Command::new(&executable)
        .arg(fixtures.join("scene.txt"))
        .output()
        .unwrap();
    assert!(image.status.success());
    assert!(image.stderr.is_empty());
    let header = b"P5\n16 12\n255\n";
    assert!(image.stdout.starts_with(header));
    let pixels = &image.stdout[header.len()..];
    assert_eq!(pixels.len(), 192);
    assert_eq!(
        pixels.iter().map(|value| u64::from(*value)).sum::<u64>(),
        8040
    );
    assert_eq!(pixels.iter().filter(|value| **value != 0).count(), 68);
    assert_eq!(pixels.iter().filter(|value| **value == 80).count(), 42);
    assert_eq!(pixels.iter().filter(|value| **value == 180).count(), 26);

    let invalid = Command::new(&executable)
        .arg(fixtures.join("invalid.txt"))
        .output()
        .unwrap();
    assert_eq!(invalid.status.code(), Some(65));
    assert_eq!(invalid.stdout, b"error 25 at 4\n");
    assert!(invalid.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn lz4_application_round_trips_and_decodes_overlapping_matches() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("lz4");
    let executable = directory.join("slim-lz4");
    let build = Command::new(slimc())
        .arg("build")
        .arg(root.join("library/lz4.project"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let fixtures = root.join("library/applications/lz4/fixtures");
    let source_path = fixtures.join("repetitive.txt");
    let source = fs::read(&source_path).unwrap();
    let compressed = Command::new(&executable)
        .args(["compress", source_path.to_str().unwrap()])
        .output()
        .unwrap();
    assert!(compressed.status.success());
    assert_eq!(compressed.stdout.len(), 84);
    assert!(compressed.stdout.len() < source.len());
    assert!(compressed.stderr.is_empty());

    let compressed_path = directory.join("roundtrip.lz4");
    fs::write(&compressed_path, &compressed.stdout).unwrap();
    let decompressed = Command::new(&executable)
        .args(["decompress", compressed_path.to_str().unwrap()])
        .output()
        .unwrap();
    assert!(decompressed.status.success());
    assert_eq!(decompressed.stdout, source);
    assert!(decompressed.stderr.is_empty());

    let overlap_path = directory.join("overlap.lz4");
    fs::write(&overlap_path, [0x11_u8, b'a', 1, 0]).unwrap();
    let overlap = Command::new(&executable)
        .args(["decompress", overlap_path.to_str().unwrap()])
        .output()
        .unwrap();
    assert!(overlap.status.success());
    assert_eq!(overlap.stdout, b"aaaaaa");

    let invalid = Command::new(&executable)
        .args(["decompress", fixtures.join("invalid.lz4").to_str().unwrap()])
        .output()
        .unwrap();
    assert_eq!(invalid.status.code(), Some(65));
    assert_eq!(invalid.stdout, b"error 3 at 1\n");
    assert!(invalid.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[cfg(unix)]
#[test]
fn bounded_http_application_validates_and_reports_loopback_responses() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("http");
    let executable = directory.join("slim-http");
    let build = Command::new(slimc())
        .arg("build")
        .arg(root.join("library/http.project"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        build.status.success(),
        "{}",
        String::from_utf8_lossy(&build.stderr)
    );

    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    let port = listener.local_addr().unwrap().port();
    let server = thread::spawn(move || {
        let (mut stream, _) = listener.accept().unwrap();
        let mut request = Vec::new();
        stream.read_to_end(&mut request).unwrap();
        assert_eq!(
            request,
            b"GET /health HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\nUser-Agent: slim-http/1\r\n\r\n"
        );
        stream
            .write_all(b"HTTP/1.1 200 OK\r\ncOnTeNt-LeNgTh: 5\r\nX-Test: yes\r\n\r\nhello")
            .unwrap();
    });
    let valid = Command::new(&executable)
        .args(["127.0.0.1", &port.to_string(), "/health"])
        .output()
        .unwrap();
    server.join().unwrap();
    assert!(
        valid.status.success(),
        "stdout={} stderr={}",
        String::from_utf8_lossy(&valid.stdout),
        String::from_utf8_lossy(&valid.stderr)
    );
    assert_eq!(valid.stdout, b"status 200\nlength 5\nhello");
    assert!(valid.stderr.is_empty());

    let invalid_listener = TcpListener::bind("127.0.0.1:0").unwrap();
    let invalid_port = invalid_listener.local_addr().unwrap().port();
    let invalid_server = thread::spawn(move || {
        let (mut stream, _) = invalid_listener.accept().unwrap();
        let mut request = Vec::new();
        stream.read_to_end(&mut request).unwrap();
        stream
            .write_all(b"HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\nhello")
            .unwrap();
    });
    let invalid = Command::new(&executable)
        .args(["127.0.0.1", &invalid_port.to_string(), "/health"])
        .output()
        .unwrap();
    invalid_server.join().unwrap();
    assert_eq!(invalid.status.code(), Some(65));
    assert_eq!(invalid.stdout, b"error 17 at 38\n");
    assert!(invalid.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
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
    let generated = codegen_without_local_ids(&String::from_utf8(emitted.stdout).unwrap());
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
    let generated = codegen_without_local_ids(&String::from_utf8(emitted.stdout).unwrap());
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
    let generated = read_codegen_for_assertions(&second).unwrap();
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
        read_codegen_for_assertions(unknown_generated)
            .unwrap()
            .contains("slim_result = slim_i64_add(slim_v_value, INT64_C(1));"),
        "unknown arithmetic must retain its checked runtime operation"
    );
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn ownership_modes_have_distinct_checked_abi_capabilities() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/tool/format.slim");
    let output = Command::new(slimc()).arg(source).output().unwrap();
    assert!(output.status.success());
    let generated = codegen_without_local_ids(&String::from_utf8(output.stdout).unwrap());
    assert!(
        generated.contains(
            "static SLIM_UNUSED_FUNCTION int64_t slim_fn_read(SlimVec slim_v_values, SlimRegion *slim_region);"
        )
    );
    assert!(generated.contains(
        "static SLIM_UNUSED_FUNCTION int64_t slim_fn_touch(SlimVec * slim_v_values, SlimRegion *slim_region);"
    ));
    assert!(generated.contains(
        "static SLIM_UNUSED_FUNCTION int64_t slim_fn_consume(SlimVec slim_v_values, SlimRegion *slim_region);"
    ));
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
    let exact_generated = read_codegen_for_assertions(exact_c).unwrap();
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
    let conflicting_generated = read_codegen_for_assertions(conflicting_c).unwrap();
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
        read_codegen_for_assertions(possible_zero_c)
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
    let changed_recurrence_generated = read_codegen_for_assertions(changed_recurrence_c).unwrap();
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
        read_codegen_for_assertions(decreasing_divisor_c)
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
        read_codegen_for_assertions(bounded_c)
            .unwrap()
            .contains("slim_result = slim_i64_div(slim_v_value, slim_v_divisor);"),
        "facts beyond the fixed propagation budget must remain unknown"
    );

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn ownership_reports_use_checked_local_and_payload_modes() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    for (fixture, expected) in [
        (
            "field_fresh_owner_shared",
            vec!["alias (type (Vec I64)) (ownership shared)"],
        ),
        (
            "enum_owned_multiple",
            vec![
                "first (type (Vec I64)) (ownership owned)",
                "second (type (Vec I64)) (ownership owned)",
            ],
        ),
        (
            "enum_multiple_payloads",
            vec![
                "first (type (Vec I64)) (ownership shared)",
                "second (type (Vec I64)) (ownership shared)",
            ],
        ),
        (
            "enum_copyable_repeated",
            vec!["number (type I64) (ownership copy)"],
        ),
    ] {
        let output = Command::new(&compiler)
            .arg("analyze")
            .arg(root.join(format!("conformance/pass/{fixture}.slim")))
            .output()
            .unwrap();
        assert!(output.status.success(), "{fixture}: {output:?}");
        let report = String::from_utf8(output.stdout).unwrap();
        for fact in expected {
            assert!(report.contains(fact), "{fixture}: missing {fact}: {report}");
        }
        assert!(!report.contains("missing-checked-binding"), "{report}");
    }
    let directory = temporary_directory("payload-report-limit");
    let types = vec!["Vec[I64]"; 65].join(", ");
    let names = (0..65)
        .map(|i| format!("payload_{i}"))
        .collect::<Vec<_>>()
        .join(", ");
    let source = directory.join("payloads.slim");
    fs::write(&source, format!("module payloads\n\nenum Many:\n  All({types})\n\nfn observe(value: Many) -> I64:\n  match value:\n    All({names}):\n      0\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n")).unwrap();
    let output = Command::new(compiler)
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    assert!(output.status.success(), "{output:?}");
    let report = String::from_utf8(output.stdout).unwrap();
    assert!(
        report.contains("(facts-truncated true) (ownership-pressure (guarantee bounded)"),
        "{report}"
    );
    assert!(
        report.contains("payload_62 (type (Vec I64)) (ownership shared)"),
        "{report}"
    );
    assert!(!report.contains("payload_63 (type"), "{report}");
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn replacement_executes_counted_stages_and_retains_mutation_evidence() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/pass/replacement_counted.slim");
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("counted-replacement");
    let analysis = Command::new(&compiler)
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    assert!(analysis.status.success(), "{analysis:?}");
    let report = String::from_utf8(analysis.stdout).unwrap();
    assert!(report.contains("(counted-loop-count 1) (reported-facts 1)"));
    assert!(report.contains("(mutations 1)"), "{report}");
    assert!(
        report.contains("(blockers exclusive-borrow mutation"),
        "{report}"
    );
    let emitted = Command::new(&compiler).arg(&source).output().unwrap();
    assert!(emitted.status.success(), "{emitted:?}");
    let generated = codegen_without_local_ids(&String::from_utf8(emitted.stdout).unwrap());
    assert_eq!(
        generated
            .matches("if (slim_v_index < INT64_C(3)) do {")
            .count(),
        3,
        "the specialized path must execute"
    );
    let executable = directory.join("replacement");
    let built = Command::new(slimc())
        .arg("build")
        .arg(&source)
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(built.status.success(), "{built:?}");
    let output = Command::new(executable).output().unwrap();
    assert_eq!(output.status.code(), Some(0), "{output:?}");
    assert!(output.stdout.is_empty() && output.stderr.is_empty());
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn exclusive_assignment_executes_through_counted_lowering() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source = root.join("conformance/pass/exclusive_assignment_counted.slim");
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("counted-exclusive-assignment");
    let analysis = Command::new(&compiler)
        .arg("analyze")
        .arg(&source)
        .output()
        .unwrap();
    assert!(analysis.status.success(), "{analysis:?}");
    let report = String::from_utf8(analysis.stdout).unwrap();
    assert!(report.contains("(start 0) (bound 3) (step 1) (iterations 3)"));
    assert!(report.contains("(counted-loop-count 1) (reported-facts 1)"));
    let emitted = Command::new(&compiler).arg(&source).output().unwrap();
    assert!(emitted.status.success(), "{emitted:?}");
    let generated = codegen_without_local_ids(&String::from_utf8(emitted.stdout).unwrap());
    assert_eq!(
        generated
            .matches("if (slim_v_index < INT64_C(3)) do {")
            .count(),
        3,
        "the specialized path must run; ordinary recurrence is not this test"
    );
    let executable = directory.join("assignment");
    let built = Command::new(slimc())
        .arg("build")
        .arg(&source)
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(built.status.success(), "{built:?}");
    let output = Command::new(executable).output().unwrap();
    assert_eq!(output.status.code(), Some(0), "{output:?}");
    assert!(output.stdout.is_empty() && output.stderr.is_empty());
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
    let early_generated = read_codegen_for_assertions(&early_c).unwrap();
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
        read_codegen_for_assertions(maximum_c)
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
    let conflicting_generated = read_codegen_for_assertions(conflicting_c).unwrap();
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
    let over_budget_generated = read_codegen_for_assertions(over_budget_c).unwrap();
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
    let propagated_generated = read_codegen_for_assertions(propagated_c).unwrap();
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
        "module versioned_get_fallback\n\nfn read(values: Vec[I64], index: I64) -> I64 effects[partial]:\n  vec.get(values, index)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(@values, 42)\n  if true:\n    read(values, 0)\n  else:\n    read(values, 9)\n",
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
    let fallback_generated = read_codegen_for_assertions(&fallback_c).unwrap();
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
        "module versioned_get_trap\n\nfn read(values: Vec[I64], index: I64) -> I64 effects[partial]:\n  vec.get(values, index)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(@values, 42)\n  if false:\n    read(values, 0)\n  else:\n    read(values, 9)\n",
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
        "module versioned_set_fallback\n\nfn store(values: @Vec[I64], index: I64, value: I64) -> Void effects[partial]:\n  vec.set(@values, index, value)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(@values, 0)\n  if true:\n    store(@values, 0, 42)\n  else:\n    store(@values, 9, 42)\n  vec.get(values, 0)\n",
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
    let set_generated = read_codegen_for_assertions(&set_c).unwrap();
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
        "module negative_index\n\nfn read(values: Vec[I64], index: I64) -> I64 effects[partial]:\n  vec.get(values, index)\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(@values, 42)\n  read(values, -1)\n",
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
    let negative_generated = read_codegen_for_assertions(negative_c).unwrap();
    assert!(!negative_generated.contains(".len > INT64_C("));
    assert!(negative_generated.contains("slim_vec_check_index"));

    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn typed_vector_set_preserves_aggregate_values_and_bounds_checks() {
    let directory = temporary_directory("typed-vector-set");
    let source = write_source(
        &directory,
        "module typed_vector_set\n\nstruct Pair:\n  left: I64\n  right: I64\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:\n  let pairs: Vec[Pair] = vec.new()\n  let first: Pair = Pair(left: 1, right: 2)\n  vec.push(@pairs, first)\n  let second: Pair = Pair(left: 20, right: 22)\n  vec.set(@pairs, 0, second)\n  let result: Pair = vec.get(pairs, 0)\n  (result.left + result.right)\n",
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
    let generated = read_codegen_for_assertions(generated).unwrap();
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
        "module allocation_failure\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()\n  vec.push(@values, 42)\n  0\n",
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
fn formatter_preserves_statement_boundaries_and_generated_c() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("format-boundaries");
    let mut sources = Vec::new();
    // Complete bounded domain: three preceding local/assignment forms crossed
    // with eight operator/grouping shapes, plus the borrowed-name witness and
    // multiline call/constructor/recurrence arguments.
    for prefix in [
        "  let alias: I64 = a\n",
        "  var alias: I64 = b\n  alias = a\n",
        "  let alias: I64 = if true:\n    a\n  else:\n    b\n",
    ] {
        for (result_type, expression) in [
            ("I64", "alias + b - 2"),
            ("I64", "(alias + b) * 2"),
            ("I64", "alias - (b - 2)"),
            ("I64", "alias + b * 2"),
            ("I64", "(alias + b) / 2"),
            ("I64", "scalar(alias, b) + scalar(b, alias) - 2"),
            ("Bool", "(alias + b) == (b + alias)"),
            ("Bool", "(alias < b) || (b <= alias)"),
        ] {
            sources.push(format!("module format_boundaries\n\nfn scalar(a: I64, b: I64) -> I64:\n  a + b\n\nfn exercise(a: I64, b: I64) -> {result_type}:\n{prefix}  {expression}\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"));
        }
    }
    for fixture in [
        "benchmarks/reproducers/formatter_leading_group.slim",
        "conformance/pass/multiline_call_arguments.slim",
    ] {
        sources.push(fs::read_to_string(root.join(fixture)).unwrap());
    }
    for (index, source) in sources.iter().enumerate() {
        let path = directory.join(format!("case-{index}.slim"));
        fs::write(&path, source).unwrap();
        let emit = |path: &Path| {
            let output = Command::new(&compiler).arg(path).output().unwrap();
            assert!(output.status.success(), "case {index}: {output:?}");
            output.stdout
        };
        let original_c = emit(&path);
        let formatted = Command::new(&compiler)
            .arg("fmt")
            .arg(&path)
            .output()
            .unwrap();
        assert!(formatted.status.success(), "case {index}: {formatted:?}");
        fs::write(&path, &formatted.stdout).unwrap();
        assert_eq!(original_c, emit(&path), "case {index}");
        let repeated = Command::new(&compiler)
            .arg("fmt")
            .arg(&path)
            .output()
            .unwrap();
        assert!(repeated.status.success(), "case {index}: {repeated:?}");
        assert_eq!(formatted.stdout, repeated.stdout, "case {index}");
    }
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
        report.contains("(binding 76 values (type (Vec I64)) (ownership shared) (scope-end 138)")
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
            "(binding 230 items (type (Vec I64)) (ownership shared) (scope-end 238) (uses 1) (last-use 235)"
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
        "(interface 3 math (fn answer ((copy I64)) I64 (effects)))\n"
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

#[test]
fn branch_ownership_matches_all_paths_in_bounded_action_domain() {
    // Independent path oracle for 486 programs: no-op, read, and move in five
    // positions, both nested-tree orientations, and alternating local/call
    // transfers. The production checker is the only compiler used here.
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    if !compiler.is_file() {
        assert!(Command::new(slim_bootstrap()).status().unwrap().success());
    }
    let directory = temporary_directory("branch-ownership-domain");
    for orientation in 0..2 {
        for pattern in 0..243usize {
            let mut digits = pattern;
            let mut actions = [0; 5];
            for action in &mut actions {
                *action = digits % 3;
                digits /= 3;
            }
            let accepted = (1..4).all(|leaf| {
                let mut moved = false;
                [0, leaf, 4]
                    .into_iter()
                    .all(|position| match actions[position] {
                        0 => true,
                        1 => !moved,
                        _ => {
                            let valid = !moved;
                            moved = true;
                            valid
                        }
                    })
            });
            let action = |position: usize, spaces: usize| {
                let prefix = " ".repeat(spaces);
                match actions[position] {
                    0 => format!("{prefix}void\n"),
                    1 => format!("{prefix}let read_{position}: I64 = vec.len(values)\n"),
                    _ if (pattern + position).is_multiple_of(2) => {
                        format!("{prefix}consume(^values)\n")
                    }
                    _ => format!("{prefix}let moved_{position}: Vec[I64] = values\n"),
                }
            };
            let leaf = |position: usize, spaces: usize| {
                format!("{}{}void\n", action(position, spaces), " ".repeat(spaces))
            };
            let mut source = String::from(
                "module branch_domain\n\nfn consume(value: ^Vec[I64]) -> Void:\n  void\n\nfn exercise(flag: Bool, other: Bool) -> Void effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n",
            );
            source.push_str(&action(0, 2));
            source.push_str("  if flag:\n");
            if orientation == 0 {
                source.push_str("    if other:\n");
                source.push_str(&leaf(1, 6));
                source.push_str("    else:\n");
                source.push_str(&leaf(2, 6));
                source.push_str("  else:\n");
                source.push_str(&leaf(3, 4));
            } else {
                source.push_str(&leaf(1, 4));
                source.push_str("  else:\n    if other:\n");
                source.push_str(&leaf(2, 6));
                source.push_str("    else:\n");
                source.push_str(&leaf(3, 6));
            }
            source.push_str(&action(4, 2));
            source.push_str("  void\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n");
            let path = write_source(&directory, &source);
            let output = Command::new(&compiler)
                .arg("check")
                .arg(path)
                .output()
                .unwrap();
            assert_eq!(
                output.status.success(),
                accepted,
                "orientation={orientation}, pattern={pattern}, actions={actions:?}\n{source}\n{}{}",
                String::from_utf8_lossy(&output.stdout),
                String::from_utf8_lossy(&output.stderr)
            );
            if !accepted {
                assert!(String::from_utf8_lossy(&output.stdout).contains("E0315@"));
            }
        }
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn reinitialization_matches_all_paths_in_bounded_action_domain() {
    // Independent path oracle for 15,552 programs. Each position contains
    // no-op, read, move, reset, move-then-reset, or reset-then-move. Enumerate
    // each complete path in both nested-tree orientations; no checker state
    // machinery is reused. Preserve the original 486-program domain above.
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    if !compiler.is_file() {
        assert!(Command::new(slim_bootstrap()).status().unwrap().success());
    }
    let directory = temporary_directory("reinitialization-domain");
    for orientation in 0..2 {
        for pattern in 0..7_776usize {
            let mut digits = pattern;
            let mut actions = [0; 5];
            for action in &mut actions {
                *action = digits % 6;
                digits /= 6;
            }
            let sequence = |action| match action {
                0 => &[][..],
                1 => &[1][..],
                2 => &[2][..],
                3 => &[3][..],
                4 => &[2, 3][..],
                _ => &[3, 2][..],
            };
            let accepted = (1..4).all(|leaf| {
                let mut available = true;
                [0, leaf, 4].into_iter().all(|position| {
                    sequence(actions[position])
                        .iter()
                        .all(|action| match action {
                            1 => available,
                            2 => {
                                let valid = available;
                                available = false;
                                valid
                            }
                            _ => {
                                available = true;
                                true
                            }
                        })
                })
            });
            let action = |position: usize, spaces: usize| {
                let prefix = " ".repeat(spaces);
                let mut text = String::new();
                for action in sequence(actions[position]) {
                    match action {
                        1 => text.push_str(&format!(
                            "{prefix}let read_{position}: I64 = vec.len(values)\n"
                        )),
                        2 if (pattern + position).is_multiple_of(2) => {
                            text.push_str(&format!("{prefix}consume(^values)\n"))
                        }
                        2 => text.push_str(&format!(
                            "{prefix}let moved_{position}: Vec[I64] = values\n"
                        )),
                        _ => text.push_str(&format!("{prefix}values = vec.new()\n")),
                    }
                }
                text
            };
            let leaf = |position: usize, spaces: usize| {
                format!("{}{}void\n", action(position, spaces), " ".repeat(spaces))
            };
            let mut source = String::from(
                "module branch_domain\n\nfn consume(value: ^Vec[I64]) -> Void:\n  void\n\nfn exercise(flag: Bool, other: Bool) -> Void effects[alloc, partial]:\n  var values: Vec[I64] = vec.new()\n",
            );
            source.push_str(&action(0, 2));
            source.push_str("  if flag:\n");
            if orientation == 0 {
                source.push_str("    if other:\n");
                source.push_str(&leaf(1, 6));
                source.push_str("    else:\n");
                source.push_str(&leaf(2, 6));
                source.push_str("  else:\n");
                source.push_str(&leaf(3, 4));
            } else {
                source.push_str(&leaf(1, 4));
                source.push_str("  else:\n    if other:\n");
                source.push_str(&leaf(2, 6));
                source.push_str("    else:\n");
                source.push_str(&leaf(3, 6));
            }
            source.push_str(&action(4, 2));
            source.push_str("  void\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n");
            let path = write_source(&directory, &source);
            let output = Command::new(&compiler)
                .arg("check")
                .arg(path)
                .output()
                .unwrap();
            assert_eq!(
                output.status.success(),
                accepted,
                "orientation={orientation}, pattern={pattern}, actions={actions:?}\n{source}\n{}{}",
                String::from_utf8_lossy(&output.stdout),
                String::from_utf8_lossy(&output.stderr)
            );
            if !accepted {
                assert!(String::from_utf8_lossy(&output.stdout).contains("E0315@"));
            }
        }
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn reinitialization_preserves_untouched_arms_across_deep_scopes() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("reinitialization-deep-scopes");
    fn resets(source: &mut String, indent: usize, owners: usize) {
        let pad = " ".repeat(indent);
        for owner in 0..owners {
            source.push_str(&format!("{pad}owner_{owner} = vec.new()\n"));
        }
        source.push_str(&format!("{pad}void\n"));
    }
    fn branch(source: &mut String, depth: usize, indent: usize, owners: usize, complete: bool) {
        if depth == 0 {
            resets(source, indent, owners);
            return;
        }
        let pad = " ".repeat(indent);
        source.push_str(&format!("{pad}if flag:\n"));
        if depth.is_multiple_of(2) {
            branch(source, depth - 1, indent + 2, owners, complete);
        } else if complete {
            resets(source, indent + 2, owners);
        } else {
            source.push_str(&format!("{pad}  void\n"));
        }
        source.push_str(&format!("{pad}else:\n"));
        if !depth.is_multiple_of(2) {
            branch(source, depth - 1, indent + 2, owners, complete);
        } else if complete {
            resets(source, indent + 2, owners);
        } else {
            source.push_str(&format!("{pad}  void\n"));
        }
    }
    // A complete tree resets each owner at every leaf. The incomplete tree
    // resets only the deepest leaf: all other leaves retain the moved entry.
    // Check every owner independently, across skipped and explicitly joined scopes.
    for depth in [1, 8, 64, 128] {
        for owners in [1, 17] {
            for complete in [false, true] {
                let mut source = String::from(
                    "module deep_reset\n\nfn consume(values: ^Vec[I64]) -> Void:\n  void\n\nfn exercise(flag: Bool) -> Void effects[alloc]:\n",
                );
                for owner in 0..owners {
                    source.push_str(&format!(
                        "  var owner_{owner}: Vec[I64] = vec.new()\n  consume(^owner_{owner})\n"
                    ));
                }
                branch(&mut source, depth, 2, owners, complete);
                for owner in 0..owners {
                    source.push_str(&format!(
                        "  let count_{owner}: I64 = vec.len(owner_{owner})\n"
                    ));
                }
                source.push_str("  void\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n");
                let path = write_source(&directory, &source);
                let output = Command::new(&compiler)
                    .arg("check")
                    .arg(path)
                    .output()
                    .unwrap();
                assert_eq!(
                    output.status.success(),
                    complete,
                    "depth={depth}, owners={owners}, complete={complete}: {}{}",
                    String::from_utf8_lossy(&output.stdout),
                    String::from_utf8_lossy(&output.stderr)
                );
                if !complete {
                    let diagnostics = String::from_utf8(output.stdout).unwrap();
                    assert_eq!(diagnostics.lines().count(), owners, "{diagnostics}");
                    assert!(diagnostics.lines().all(|line| line.starts_with("E0315@")));
                }
            }
        }
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn termination_graph_matches_all_three_function_graphs_and_deep_chains() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("termination-graphs");
    // Complete domain: all 512 directed graphs on three functions, including
    // self edges. Transitive closure is independent of the compiler's DFS.
    for edges in 0..512usize {
        let mut reachable = [[false; 3]; 3];
        let mut source = String::from("module termination_graph\n\n");
        for (from, row) in reachable.iter_mut().enumerate() {
            source.push_str(&format!("fn vertex_{from}() -> Void:\n"));
            for (to, edge) in row.iter_mut().enumerate() {
                *edge = edges & (1 << (from * 3 + to)) != 0;
                if *edge {
                    source.push_str(&format!("  vertex_{to}()\n"));
                }
            }
            source.push_str("  void\n\n");
        }
        source.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
        for via in 0..3 {
            for from in 0..3 {
                for to in 0..3 {
                    reachable[from][to] |= reachable[from][via] && reachable[via][to];
                }
            }
        }
        let cyclic = (0..3).any(|vertex| reachable[vertex][vertex]);
        let path = write_source(&directory, &source);
        let output = Command::new(&compiler)
            .arg("check")
            .arg(path)
            .output()
            .unwrap();
        assert_eq!(
            output.status.success(),
            !cyclic,
            "edges={edges}\n{source}\n{}",
            String::from_utf8_lossy(&output.stdout)
        );
        if cyclic {
            assert!(String::from_utf8_lossy(&output.stdout).starts_with("E0343@"));
        }
    }
    // Cross the parallel analyzer's unrelated 64-function reporting limit and
    // exercise a deep graph without using the native call stack for DFS.
    for size in [65, 2_048] {
        for cyclic in [false, true] {
            let mut source = String::from("module deep_termination\n\n");
            for index in 0..size {
                source.push_str(&format!("fn vertex_{index}() -> I64:\n"));
                if index + 1 < size {
                    source.push_str(&format!("  vertex_{}()\n\n", index + 1));
                } else if cyclic {
                    source.push_str("  vertex_0()\n\n");
                } else {
                    source.push_str("  0\n\n");
                }
            }
            source.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
            let path = write_source(&directory, &source);
            let output = Command::new(&compiler)
                .arg("check")
                .arg(path)
                .output()
                .unwrap();
            assert_eq!(
                output.status.success(),
                !cyclic,
                "size={size}, cyclic={cyclic}: {}",
                String::from_utf8_lossy(&output.stdout)
            );
        }
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn recurrence_totality_requires_every_prefix_and_argument() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("termination-proof-prefixes");
    for name in [
        "bad_prefix",
        "bad_argument",
        "wrong_controller",
        "zero_step",
        "out_of_domain",
    ] {
        let source =
            fs::read_to_string(root.join(format!("conformance/fail/termination_{name}.slim")))
                .unwrap();
        let source = source.replacen("-> I64:", "-> I64 effects[partial]:", 1);
        let path = write_source(&directory, &source);
        let output = Command::new(&compiler)
            .arg("analyze")
            .arg(path)
            .output()
            .unwrap();
        assert!(
            output.status.success(),
            "{name}: {}",
            String::from_utf8_lossy(&output.stdout)
        );
        let report = String::from_utf8(output.stdout).unwrap();
        assert!(
            report.contains("(recurrence-profile-count 0)"),
            "{name}: {report}"
        );
        assert!(
            report.split("(blockers ").skip(1).any(|part| {
                part.split(')')
                    .next()
                    .unwrap()
                    .split_whitespace()
                    .any(|reason| reason == "recurrence")
            }),
            "{name}: {report}"
        );
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn missing_partial_stays_rejected_when_refinement_budget_is_exhausted() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("termination-refinement-limit");
    for size in [32, 33, 65] {
        let mut source = String::from("module recurrence_budget\n\n");
        for index in 0..size {
            source.push_str(&format!("fn down_{index:08}(value: I64) -> I64:\n  if value <= 0:\n    0\n  else:\n    recur(value - 1)\n\n"));
        }
        source.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
        let path = write_source(&directory, &source);
        let output = Command::new(&compiler)
            .arg("check")
            .arg(path)
            .output()
            .unwrap();
        assert_eq!(
            output.status.success(),
            size == 32,
            "size={size}: {}",
            String::from_utf8_lossy(&output.stdout)
        );
        if size > 32 {
            assert_eq!(
                String::from_utf8(output.stdout).unwrap(),
                "E0343@2978:2983\n"
            );
        }
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn lexical_shadowing_matches_alpha_renamed_native_programs() {
    let directory = temporary_directory("lexical-shadowing");
    let compiler = PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("build/toolchain/slimc");
    // Fixed domain: two branch values, four depths, parameter/local roots,
    // and original/alpha-renamed programs. Expected result is independent of C.
    for depth in [1, 2, 8, 32] {
        for branch in [false, true] {
            for parameter in [false, true] {
                for renamed in [false, true] {
                    let name = |level| {
                        if renamed {
                            format!("value_{level}")
                        } else {
                            String::from("value")
                        }
                    };
                    let mut source = if parameter {
                        format!("module lexical\n\nfn exercise({}: I64) -> I64:\n", name(0))
                    } else {
                        format!(
                            "module lexical\n\nfn exercise() -> I64:\n  let {}: I64 = 10\n",
                            name(0)
                        )
                    };
                    for level in 1..=depth {
                        source.push_str(&format!("  let {}: I64 = if {branch}:\n    let {}: I64 = {} + 1\n    {}\n  else:\n    let {}: I64 = {} + 2\n    {}\n", name(level), name(level), name(level-1), name(level), name(level), name(level-1), name(level)));
                    }
                    source.push_str(&format!(
                        "  {}\n\nfn main(args: Vec[Bytes]) -> I64:\n  exercise({})\n",
                        name(depth),
                        if parameter { "10" } else { "" }
                    ));
                    let path = write_source(&directory, &source);
                    let generated = Command::new(&compiler).arg(path).output().unwrap();
                    assert!(generated.status.success(), "{source}: {generated:?}");
                    let text = String::from_utf8(generated.stdout.clone()).unwrap();
                    assert!(text.contains("slim_v_value") && text.contains("_n"));
                    let c = directory.join("program.c");
                    let binary = directory.join("program");
                    fs::write(&c, generated.stdout).unwrap();
                    let output = Command::new(native_compiler())
                        .args([
                            "-std=c11", "-O1", "-Wall", "-Wextra", "-Werror", "-I", "runtime",
                        ])
                        .arg(c)
                        .arg("runtime/slim_rt.c")
                        .arg("-o")
                        .arg(&binary)
                        .output()
                        .unwrap();
                    assert!(output.status.success(), "{source}: {output:?}");
                    let run = Command::new(binary).output().unwrap();
                    assert_eq!(
                        run.status.code(),
                        Some(10 + depth * if branch { 1 } else { 2 }),
                        "{source}: {run:?}"
                    );
                    assert!(run.stdout.is_empty() && run.stderr.is_empty());
                }
            }
        }
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn production_source_identity_resolution_rejects_stale_and_extreme_handles() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("source-identity");
    fs::copy(
        root.join("selfhost/identity.slim"),
        directory.join("identity.slim"),
    )
    .unwrap();
    fs::write(
        directory.join("probe.slim"),
        include_str!("fixtures/source_identity.slim"),
    )
    .unwrap();
    let manifest = directory.join("slim.project");
    fs::write(&manifest, "(project 1 (entry probe)\n  (module identity \"identity.slim\" (imports) (exports DeclarationId FileId Index NextRevision NodeId Revision Span View reset resolve_node resolve_span successor))\n  (module probe \"probe.slim\" (imports identity) (exports)))\n").unwrap();
    let generated = Command::new(root.join("build/toolchain/slimc"))
        .arg(&manifest)
        .output()
        .unwrap();
    assert!(
        generated.status.success(),
        "{}",
        String::from_utf8_lossy(&generated.stdout)
    );
    let c = directory.join("probe.c");
    fs::write(&c, &generated.stdout).unwrap();
    let executable = directory.join("probe");
    let compiled = Command::new(native_compiler())
        .args(["-std=c11", "-O1", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(root.join("runtime"))
        .arg(&c)
        .arg(root.join("runtime/slim_rt.c"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        compiled.status.success(),
        "{}",
        String::from_utf8_lossy(&compiled.stderr)
    );
    let values = [i64::MIN, -1, 0, 1, 3, i64::MAX - 1, i64::MAX];
    let mut expected = String::new();
    for total in values {
        for first in values {
            for count in values {
                for ordinal in values {
                    // Wider independent arithmetic ensures the oracle itself cannot overflow.
                    let valid = total >= 0
                        && first >= 0
                        && count >= 0
                        && ordinal >= 0
                        && i128::from(first) + i128::from(count) <= i128::from(total)
                        && ordinal < count;
                    let result = if valid {
                        i128::from(first) + i128::from(ordinal)
                    } else {
                        -1
                    };
                    expected.push_str(&format!("{result}\n"));
                }
            }
        }
    }
    for start in values {
        for end in values {
            let result = if 0 <= start && start <= end && end <= 3 {
                start
            } else {
                -1
            };
            expected.push_str(&format!("{result}\n"));
        }
    }
    for epoch in values {
        for serial in values {
            for file in values {
                for declaration in values {
                    let file_matches = epoch == 1 && serial == 1 && file == 0;
                    let node = if file_matches && declaration == 0 {
                        3
                    } else {
                        -1
                    };
                    let span = if file_matches { 0 } else { -1 };
                    expected.push_str(&format!("{node} {span}\n"));
                }
            }
        }
    }
    for epoch in values {
        for serial in values {
            if epoch > 0 && serial > 0 && serial < i64::MAX {
                expected.push_str(&format!("{epoch} {}\n", serial + 1));
            } else {
                expected.push_str("none\n");
            }
            if epoch > 0 && serial > 0 && epoch < i64::MAX {
                expected.push_str(&format!("{} 1\n", epoch + 1));
            } else {
                expected.push_str("none\n");
            }
        }
    }
    let output = Command::new(&executable).output().unwrap();
    assert!(
        output.status.success(),
        "{}",
        String::from_utf8_lossy(&output.stderr)
    );
    assert_eq!(output.stdout, expected.as_bytes());
    assert!(output.stderr.is_empty());

    // Nominal identities must not become interchangeable scalar positions.
    fs::write(directory.join("probe.slim"), "module probe\n\nfn main(args: Vec[Bytes]) -> I64:\n  let revision: identity.Revision = identity.Revision(epoch: 1, serial: 1)\n  let file: identity.FileId = identity.FileId(revision: revision, slot: 0)\n  let node: identity.NodeId = file\n  node.ordinal\n").unwrap();
    let rejected = Command::new(root.join("build/toolchain/slimc"))
        .arg("check")
        .arg(&manifest)
        .output()
        .unwrap();
    assert!(!rejected.status.success());
    let diagnostic = String::from_utf8(rejected.stdout).unwrap();
    assert!(diagnostic.contains("E0344@probe@"), "{diagnostic}");
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn production_session_rejects_partial_indexes_and_preserves_source_comparisons() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let compiler = root.join("build/toolchain/slimc");
    let directory = temporary_directory("session-source-index");
    let initial = directory.join("initial");
    let updated = directory.join("relocated");
    for folder in [&initial, &updated] {
        fs::create_dir(folder).unwrap();
        fs::write(
            folder.join("slim.project"),
            "(project 1 (entry app) (module app \"app.slim\" (imports) (exports)))\n",
        )
        .unwrap();
    }
    let source =
        "module app\n\nfn value() -> I64:\n  42\n\nfn main(args: Vec[Bytes]) -> I64:\n  value()\n";
    fs::write(initial.join("app.slim"), source).unwrap();
    let invoke = |left: &Path, right: &Path| {
        let mut child = Command::new(&compiler)
            .arg("session")
            .arg(left)
            .arg(right)
            .stdout(std::process::Stdio::piped())
            .stderr(std::process::Stdio::piped())
            .spawn()
            .unwrap();
        let deadline = Instant::now() + Duration::from_secs(5);
        loop {
            if child.try_wait().unwrap().is_some() {
                break;
            }
            if Instant::now() >= deadline {
                child.kill().unwrap();
                let _ = child.wait();
                panic!("session did not reject bounded malformed input");
            }
            thread::sleep(Duration::from_millis(5));
        }
        child.wait_with_output().unwrap()
    };
    let first = initial.join("slim.project");
    let second = updated.join("slim.project");
    for changed in [
        source.to_owned(),
        format!("# comment\n{source}"),
        "module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  value()\n\nfn value() -> I64:\n  42\n"
            .to_owned(),
    ] {
        fs::write(updated.join("app.slim"), changed).unwrap();
        let output = invoke(&first, &second);
        assert!(output.status.success(), "{:?}", output);
        assert_eq!(output.stdout, b"0 0 0 0\n");
        assert!(output.stderr.is_empty());
    }
    let crlf = source.replace("\n", "\r\n");
    fs::write(initial.join("app.slim"), &crlf).unwrap();
    fs::write(updated.join("app.slim"), &crlf).unwrap();
    assert_eq!(invoke(&first, &second).stdout, b"0 0 0 0\n");
    fs::write(initial.join("app.slim"), source).unwrap();
    fs::write(updated.join("app.slim"), source.replace("42", "43")).unwrap();
    assert_eq!(invoke(&first, &second).stdout, b"1 1 1 1\n");
    let nested = "module app\n\nfn value(x: I64) -> I64:\n  x\n\nfn main(args: Vec[Bytes]) -> I64:\n  value(1)\n";
    fs::write(initial.join("app.slim"), nested).unwrap();
    fs::write(
        updated.join("app.slim"),
        nested.replace("value(1)", "value(false)"),
    )
    .unwrap();
    assert_eq!(invoke(&first, &second).stdout, b"1 1 1 1\n");
    fs::write(initial.join("app.slim"), source).unwrap();
    for broken in [
        "",
        "()",
        "(project 1 (entry app)",
        "(project 1 (entry app) (module app \"app.slim\" (imports)))",
    ] {
        fs::write(&second, broken).unwrap();
        let output = invoke(&first, &second);
        assert_eq!(output.status.code(), Some(65));
        assert_eq!(output.stdout, b"Q0001: invalid source identity\n");
    }
    fs::copy(&first, &second).unwrap();
    fs::write(
        updated.join("app.slim"),
        "module app\n\nfn main(args: Vec[Bytes]) -> I64:\n",
    )
    .unwrap();
    let malformed = invoke(&first, &second);
    assert_eq!(malformed.status.code(), Some(65));
    assert!(
        String::from_utf8(malformed.stdout)
            .unwrap()
            .contains("Q0001: invalid source identity")
    );
    fs::write(updated.join("app.slim"), "module app\n\nfn value() -> I64:\n  1\n\nfn value() -> I64:\n  2\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n").unwrap();
    let duplicate = invoke(&first, &second);
    assert_eq!(duplicate.status.code(), Some(65));
    assert_eq!(duplicate.stdout, b"Q0001: invalid source identity\n");
    fs::remove_file(updated.join("app.slim")).unwrap();
    assert_eq!(invoke(&first, &second).status.code(), Some(65));
    fs::remove_file(&second).unwrap();
    assert_eq!(invoke(&first, &second).status.code(), Some(65));
    fs::write(&second, fs::read(&first).unwrap()).unwrap();
    fs::write(updated.join("app.slim"), source).unwrap();
    assert_eq!(invoke(&first, &second).stdout, b"0 0 0 0\n");
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn production_revision_maps_preserve_nodes_spans_and_reject_stale_history() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("revision-maps");
    for entry in fs::read_dir(root.join("selfhost")).unwrap() {
        let path = entry.unwrap().path();
        if path
            .extension()
            .is_some_and(|extension| extension == "slim")
        {
            fs::copy(&path, directory.join(path.file_name().unwrap())).unwrap();
        }
    }
    let source_manifest = fs::read_to_string(root.join("selfhost/slim.project")).unwrap();
    let mut manifest = source_manifest
        .lines()
        .filter(|line| !line.contains("(module driver "))
        .collect::<Vec<_>>()
        .join("\n")
        .replace("(entry driver)", "(entry zzprobe)");
    assert!(manifest.ends_with(')'));
    manifest.pop();
    manifest.push_str("\n  (module zzprobe \"zzprobe.slim\" (imports identity project query syntax) (exports)))\n");
    fs::write(directory.join("slim.project"), manifest).unwrap();
    for fixture in [
        include_str!("fixtures/revision_mapping.slim").to_owned(),
        include_str!("fixtures/revision_mapping.slim").replace("\\n", "\\r\\n"),
    ] {
        fs::write(directory.join("zzprobe.slim"), fixture).unwrap();
        let generated = Command::new(root.join("build/toolchain/slimc"))
            .arg(directory.join("slim.project"))
            .output()
            .unwrap();
        assert!(
            generated.status.success(),
            "{}",
            String::from_utf8_lossy(&generated.stdout)
        );
        let c = directory.join("probe.c");
        fs::write(&c, generated.stdout).unwrap();
        let executable = directory.join("probe");
        let compiled = Command::new(native_compiler())
            .args(["-std=c11", "-O1", "-Wall", "-Wextra", "-Werror"])
            .arg("-I")
            .arg(root.join("runtime"))
            .arg(&c)
            .arg(root.join("runtime/slim_rt.c"))
            .arg("-o")
            .arg(&executable)
            .output()
            .unwrap();
        assert!(
            compiled.status.success(),
            "{}",
            String::from_utf8_lossy(&compiled.stderr)
        );
        let output = Command::new(&executable).output().unwrap();
        assert!(output.status.success(), "{:?}", output);
        assert_eq!(output.stdout, b"ok exact revision maps\n");
        assert!(output.stderr.is_empty());
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn production_function_checks_are_independent_of_order_and_scratch_history() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("function-checking");
    for entry in fs::read_dir(root.join("selfhost")).unwrap() {
        let path = entry.unwrap().path();
        if path
            .extension()
            .is_some_and(|extension| extension == "slim")
        {
            fs::copy(&path, directory.join(path.file_name().unwrap())).unwrap();
        }
    }
    let source_manifest = fs::read_to_string(root.join("selfhost/slim.project")).unwrap();
    let mut manifest = source_manifest
        .lines()
        .filter(|line| !line.contains("(module driver "))
        .collect::<Vec<_>>()
        .join("\n")
        .replace("(entry driver)", "(entry zzprobe)");
    assert!(manifest.ends_with(')'));
    manifest.pop();
    manifest.push_str(
        "\n  (module zzprobe \"zzprobe.slim\" (imports check ir syntax typing) (exports)))\n",
    );
    fs::write(directory.join("slim.project"), manifest).unwrap();
    fs::write(
        directory.join("zzprobe.slim"),
        include_str!("fixtures/function_checking.slim"),
    )
    .unwrap();
    let generated = Command::new(root.join("build/toolchain/slimc"))
        .arg(directory.join("slim.project"))
        .output()
        .unwrap();
    assert!(
        generated.status.success(),
        "{}",
        String::from_utf8_lossy(&generated.stdout)
    );
    let c = directory.join("probe.c");
    fs::write(&c, generated.stdout).unwrap();
    let executable = directory.join("probe");
    let compiled = Command::new(native_compiler())
        .args(["-std=c11", "-O1", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(root.join("runtime"))
        .arg(&c)
        .arg(root.join("runtime/slim_rt.c"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        compiled.status.success(),
        "{}",
        String::from_utf8_lossy(&compiled.stderr)
    );
    let mut fixtures: Vec<PathBuf> = fs::read_dir(root.join("conformance/pass"))
        .unwrap()
        .map(|entry| entry.unwrap().path())
        .filter(|path| {
            path.extension()
                .is_some_and(|extension| extension == "slim")
        })
        .collect();
    fixtures.extend(
        fs::read_dir(root.join("benchmarks/challenges"))
            .unwrap()
            .map(|entry| entry.unwrap().path().join("program.slim"))
            .filter(|path| path.is_file()),
    );
    fixtures.sort();
    assert!(fixtures.len() >= 92);
    for fixture in fixtures {
        let output = Command::new(&executable).arg(&fixture).output().unwrap();
        assert!(
            output.status.success(),
            "{}: {:?}",
            fixture.display(),
            output
        );
        assert_eq!(
            output.stdout,
            b"ok isolated function checking\n",
            "{}",
            fixture.display()
        );
        assert!(output.stderr.is_empty());
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn checked_layout_order_builds_every_aggregate_permutation_and_forward_module() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("checked-layout-order");
    let compile_run = |input: &Path| {
        let generate = || {
            let result = Command::new(root.join("build/toolchain/slimc"))
                .arg(input)
                .output()
                .unwrap();
            assert!(result.status.success(), "{result:?}");
            assert!(result.stderr.is_empty());
            result.stdout
        };
        let generated = generate();
        assert_eq!(generated, generate());
        let c = directory.join("generated.c");
        let executable = directory.join("generated");
        fs::write(&c, &generated).unwrap();
        let compiled = Command::new(native_compiler())
            .args(["-std=c11", "-O1", "-Wall", "-Wextra", "-Werror"])
            .arg("-I")
            .arg(root.join("runtime"))
            .arg(&c)
            .arg(root.join("runtime/slim_rt.c"))
            .arg("-o")
            .arg(&executable)
            .output()
            .unwrap();
        assert!(compiled.status.success(), "{compiled:?}");
        let executed = Command::new(executable).output().unwrap();
        assert!(executed.status.success(), "{executed:?}");
        assert_eq!(executed.stdout, b"42\n");
        assert!(executed.stderr.is_empty());
        String::from_utf8(generated).unwrap()
    };
    let fixture =
        fs::read_to_string(root.join("conformance/pass/inline_forward_layouts.slim")).unwrap();
    let sections: Vec<_> = fixture.split("\n\n").collect();
    let mut cases = 0;
    for a in 0..4 {
        for b in 0..4 {
            for c in 0..4 {
                for d in 0..4 {
                    if a == b || a == c || a == d || b == c || b == d || c == d {
                        continue;
                    }
                    let mut source = String::from("module layout\n\n");
                    // Forest uses opaque collection storage and may precede the
                    // inline DAG, including its legal collection self-reference.
                    source.push_str(sections[5]);
                    source.push_str("\n\n");
                    for index in [a, b, c, d] {
                        source.push_str(sections[index + 1]);
                        source.push_str("\n\n");
                    }
                    source.push_str(&sections[6..].join("\n\n"));
                    let input = write_source(&directory, &source);
                    let generated = compile_run(&input);
                    let mut positions = Vec::new();
                    for name in ["Root", "Left", "Right", "Leaf", "Forest"] {
                        let definition = format!("struct Slim_type_{name} {{");
                        assert_eq!(generated.matches(&definition).count(), 1);
                        positions.push(generated.find(&definition).unwrap());
                    }
                    assert!(positions[3] < positions[1] && positions[3] < positions[2]);
                    assert!(positions[1] < positions[0] && positions[2] < positions[0]);
                    assert!(positions[4] < positions[0]);
                    cases += 1;
                }
            }
        }
    }
    assert_eq!(cases, 24);
    fs::write(directory.join("app.slim"), "module app\n\nstruct Wrapper:\n  payload: shapes.Choice\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  let wrapper: Wrapper = Wrapper(payload: shapes.Choice::Some(zstorage.Leaf(value: 42)))\n  match wrapper.payload:\n    Some(leaf):\n      io.print_i64(leaf.value)\n      io.println(\"\")\n      0\n    None:\n      1\n").unwrap();
    fs::write(
        directory.join("shapes.slim"),
        "module shapes\n\nenum Choice:\n  Some(zstorage.Leaf)\n  None\n",
    )
    .unwrap();
    fs::write(
        directory.join("zstorage.slim"),
        "module zstorage\n\nstruct Leaf:\n  value: I64\n",
    )
    .unwrap();
    let manifest = directory.join("slim.project");
    fs::write(&manifest, "(project 1 (entry app)\n  (module app \"app.slim\" (imports shapes zstorage) (exports))\n  (module shapes \"shapes.slim\" (imports zstorage) (exports Choice))\n  (module zstorage \"zstorage.slim\" (imports) (exports Leaf)))\n").unwrap();
    compile_run(&manifest);
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn production_function_flow_is_bounded_and_preserves_normal_paths() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("function-flow");
    for entry in fs::read_dir(root.join("selfhost")).unwrap() {
        let path = entry.unwrap().path();
        if path
            .extension()
            .is_some_and(|extension| extension == "slim")
        {
            fs::copy(&path, directory.join(path.file_name().unwrap())).unwrap();
        }
    }
    let source_manifest = fs::read_to_string(root.join("selfhost/slim.project")).unwrap();
    let mut manifest = source_manifest
        .lines()
        .filter(|line| !line.contains("(module driver "))
        .collect::<Vec<_>>()
        .join("\n")
        .replace("(entry driver)", "(entry zzprobe)");
    assert!(manifest.ends_with(')'));
    manifest.pop();
    manifest.push_str(
        "\n  (module zzprobe \"zzprobe.slim\" (imports check flow identity ir memory syntax text typing) (exports)))\n",
    );
    fs::write(directory.join("slim.project"), manifest).unwrap();
    fs::write(
        directory.join("zzprobe.slim"),
        include_str!("fixtures/function_flow.slim"),
    )
    .unwrap();
    let generated = Command::new(root.join("build/toolchain/slimc"))
        .arg(directory.join("slim.project"))
        .output()
        .unwrap();
    assert!(
        generated.status.success(),
        "{}",
        String::from_utf8_lossy(&generated.stdout)
    );
    let c = directory.join("probe.c");
    fs::write(&c, generated.stdout).unwrap();
    let executable = directory.join("probe");
    let compiled = Command::new(native_compiler())
        .args(["-std=c11", "-O1", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(root.join("runtime"))
        .arg(&c)
        .arg(root.join("runtime/slim_rt.c"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        compiled.status.success(),
        "{}",
        String::from_utf8_lossy(&compiled.stderr)
    );
    let mut fixtures: Vec<PathBuf> = fs::read_dir(root.join("conformance/pass"))
        .unwrap()
        .map(|entry| entry.unwrap().path())
        .filter(|path| {
            path.extension()
                .is_some_and(|extension| extension == "slim")
        })
        .collect();
    fixtures.extend(
        fs::read_dir(root.join("benchmarks/challenges"))
            .unwrap()
            .map(|entry| entry.unwrap().path().join("program.slim"))
            .filter(|path| path.is_file()),
    );
    fixtures.sort();
    assert!(fixtures.len() >= 93);
    for fixture in fixtures {
        let output = Command::new(&executable).arg(&fixture).output().unwrap();
        assert!(
            output.status.success(),
            "{}: {:?}",
            fixture.display(),
            output
        );
        assert_eq!(output.stdout, b"ok bounded flow\n", "{}", fixture.display());
        assert!(output.stderr.is_empty());
    }
    for name in [
        "use_after_move.slim",
        "inline_self_cycle.slim",
        "unknown_record_type.slim",
    ] {
        let result = Command::new(&executable)
            .arg(root.join("conformance/fail").join(name))
            .output()
            .unwrap();
        assert!(result.status.success(), "{name}: {result:?}");
        assert_eq!(result.stdout, b"ok rejected flow\n");
        assert!(result.stderr.is_empty());
    }
    for size in [125, 250, 500, 1_000, 2_000, 4_000] {
        let mut source = String::from("module deep_flow\n\nfn main(args: Vec[Bytes]) -> I64:\n");
        for index in 0..size {
            source.push_str(&format!("  let value_{index}: I64 = {index}\n"));
        }
        source.push_str("  0\n");
        let input = write_source(&directory, &source);
        let output = Command::new(&executable)
            .arg(&input)
            .arg("dump")
            .output()
            .unwrap();
        assert!(output.status.success(), "deep {size}: {output:?}");
        let report = String::from_utf8(output.stdout).unwrap();
        assert!(report.ends_with("ok bounded flow\n"));
        let counts: Vec<usize> = report
            .lines()
            .next()
            .unwrap()
            .split_whitespace()
            .skip(1)
            .map(|value| value.parse().unwrap())
            .collect();
        assert_eq!(counts[1], 3 * size + 4);
        assert_eq!(counts[2], 3 * size + 2);
        assert_eq!(counts[3], 2 * size + 1);
    }
    // These paths are specified independently from the production builder.
    // Edge kind 4 is explicitly unresolved abrupt-call behavior, not normal flow.
    let normal_paths = |source: &str, function_index: usize| {
        let input = write_source(&directory, source);
        let output = Command::new(&executable)
            .args([input.as_os_str(), std::ffi::OsStr::new("dump")])
            .output()
            .unwrap();
        assert!(output.status.success(), "{output:?}");
        let text = String::from_utf8(output.stdout).unwrap();
        let section = text.split("function ").skip(1).nth(function_index).unwrap();
        let mut operations = Vec::new();
        let mut edges = Vec::new();
        for line in section.lines().skip(1) {
            if line.starts_with("block ") || line.starts_with("edge ") {
                let fields: Vec<usize> = line
                    .split_whitespace()
                    .skip(1)
                    .map(|s| s.parse().unwrap())
                    .collect();
                if line.starts_with("block ") {
                    assert_eq!(fields[0], operations.len());
                    operations.push(fields[1]);
                } else if fields[1] != 4 {
                    edges.push((fields[2], fields[3], fields[1]));
                }
            }
        }
        let mut pending = vec![(0usize, Vec::new())];
        let mut paths = Vec::new();
        while let Some((block, mut path)) = pending.pop() {
            assert!(path.len() < operations.len() + 1);
            path.push(operations[block]);
            if block == 1 || operations[block] == 18 {
                paths.push(path);
            } else {
                for &(from, to, _) in &edges {
                    if from == block {
                        pending.push((to, path.clone()));
                    }
                }
            }
        }
        paths.sort();
        paths
    };
    let eager = "module flow_test\n\nfn first() -> Bool effects[io]:\n  io.println(\"first\")\n  true\n\nfn second() -> Bool effects[io]:\n  io.println(\"second\")\n  false\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  let value: Bool = first() && second()\n  if value:\n    0\n  else:\n    1\n";
    let paths = normal_paths(eager, 2);
    let expected = vec![0, 9, 10, 8, 9, 10, 8, 9, 10, 4, 3, 14, 15, 3, 16, 17, 6, 1];
    assert_eq!(paths, vec![expected.clone(), expected]);
    let recursive = "module flow_test\n\nfn sum(n: I64, total: I64) -> I64 effects[partial]:\n  if n > 0:\n    recur(n - 1, total + n)\n  else:\n    total\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n";
    let paths = normal_paths(recursive, 0);
    assert_eq!(paths.len(), 2);
    assert_eq!(
        paths.iter().filter(|path| path.last() == Some(&18)).count(),
        1
    );
    let recur_path = paths.iter().find(|path| path.last() == Some(&18)).unwrap();
    assert_eq!(&recur_path[recur_path.len() - 2..], &[8, 18]);
    assert_eq!(recur_path.iter().filter(|op| **op == 18).count(), 1);
    assert_eq!(recur_path.iter().filter(|op| **op == 7).count(), 0);
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn retained_typing_reuses_only_valid_current_semantics() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("retained-typing");
    for entry in fs::read_dir(root.join("selfhost")).unwrap() {
        let path = entry.unwrap().path();
        if path
            .extension()
            .is_some_and(|extension| extension == "slim")
        {
            fs::copy(&path, directory.join(path.file_name().unwrap())).unwrap();
        }
    }
    let mut manifest = fs::read_to_string(root.join("selfhost/slim.project"))
        .unwrap()
        .lines()
        .filter(|line| !line.contains("(module driver "))
        .collect::<Vec<_>>()
        .join("\n")
        .replace("(entry driver)", "(entry zzprobe)");
    assert!(manifest.ends_with(')'));
    manifest.pop();
    manifest.push_str("\n  (module zzprobe \"zzprobe.slim\" (imports check codegen identity retained syntax typing) (exports)))\n");
    fs::write(directory.join("slim.project"), manifest).unwrap();
    fs::write(
        directory.join("zzprobe.slim"),
        include_str!("fixtures/retained_typing.slim"),
    )
    .unwrap();
    let generated = Command::new(root.join("build/toolchain/slimc"))
        .arg(directory.join("slim.project"))
        .output()
        .unwrap();
    assert!(generated.status.success(), "{generated:?}");
    let c = directory.join("probe.c");
    fs::write(&c, generated.stdout).unwrap();
    let observed = Command::new("awk")
        .arg("-f")
        .arg(root.join("scripts/instrument-retained-probe.awk"))
        .arg(&c)
        .output()
        .unwrap();
    assert!(
        observed.status.success(),
        "observer status {:?}: {}",
        observed.status,
        String::from_utf8_lossy(&observed.stderr)
    );
    fs::write(&c, observed.stdout).unwrap();
    let executable = directory.join("probe");
    let compiled = Command::new(native_compiler())
        .args(["-std=c11", "-O1", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(root.join("runtime"))
        .arg("-include")
        .arg(root.join("benchmarks/instrumentation/retained_probe.h"))
        .arg(&c)
        .arg(root.join("runtime/slim_rt.c"))
        .arg(root.join("benchmarks/instrumentation/retained_probe.c"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        compiled.status.success(),
        "{}",
        String::from_utf8_lossy(&compiled.stderr)
    );
    let run = |before: &Path, after: &Path| {
        let report = directory.join("observed.tsv");
        let _ = fs::remove_file(&report);
        let output = Command::new(&executable)
            .arg(before)
            .arg(after)
            .env("SLIM_RETAINED_REPORT", &report)
            .output()
            .unwrap();
        assert!(
            output.status.success(),
            "{} -> {}: {output:?}",
            before.display(),
            after.display()
        );
        assert!(output.stderr.is_empty(), "{output:?}");
        let text = String::from_utf8(output.stdout).unwrap();
        let fields: Vec<i64> = text
            .lines()
            .last()
            .unwrap()
            .split_whitespace()
            .map(|v| v.parse().unwrap())
            .collect();
        assert_eq!(fields.len(), 3, "{text}");
        let observation = fs::read_to_string(&report).unwrap();
        let lines: Vec<_> = observation.lines().collect();
        assert_eq!(lines[0], "slim-retained\t1\texact\t1000000000");
        assert_eq!(lines.len(), 3);
        if fields[0] >= 0 {
            assert_eq!(lines[2], format!("1\t{}", fields[0]));
        }
        fields
    };
    let mut fixtures: Vec<_> = fs::read_dir(root.join("conformance/pass"))
        .unwrap()
        .map(|entry| entry.unwrap().path())
        .filter(|path| {
            path.extension()
                .is_some_and(|extension| extension == "slim")
        })
        .collect();
    fixtures.extend(
        fs::read_dir(root.join("benchmarks/challenges"))
            .unwrap()
            .map(|entry| entry.unwrap().path().join("program.slim"))
            .filter(|path| path.is_file()),
    );
    fixtures.sort();
    assert!(fixtures.len() >= 94);
    for path in &fixtures {
        let fields = run(path, path);
        assert_eq!(fields[0], 0, "{}", path.display());
        assert!(fields[1] > 0 && fields[2] > 0);
    }
    let base = "module changes\n\nstruct Leaf:\n  value: I64\n\nstruct Box:\n  leaf: Leaf\n\nfn helper(x: I64) -> I64:\n  x\n\nfn read(box: Box) -> I64:\n  box.leaf.value\n\nfn idle() -> I64:\n  42\n\nfn main(args: Vec[Bytes]) -> I64:\n  helper(read(Box(leaf: Leaf(value: 1))))\n";
    let before = directory.join("before.slim");
    let after = directory.join("after.slim");
    fs::write(&before, base).unwrap();
    let parts: Vec<_> = base.split("\n\n").collect();
    let reordered = [0, 6, 4, 1, 5, 2, 3].map(|i| parts[i]).join("\n\n") + "\n";
    for (source, expected) in [
        (base.to_owned(), Some([0, 4])),
        (
            base.replace("  x\n", "  let copy: I64 = x\n  copy\n"),
            Some([1, 3]),
        ),
        (
            base.replace("fn helper", "fn added() -> I64:\n  3\n\nfn helper"),
            Some([1, 4]),
        ),
        (
            base.replace("fn idle() -> I64:\n  42\n\n", ""),
            Some([0, 3]),
        ),
        (reordered, Some([0, 4])),
        (
            format!("# start\n{}", base.replace("  42", "  # comment\n  42")),
            Some([1, 3]),
        ),
        (base.replace("helper", "renamed"), Some([2, 2])),
        // Synthetic closing nodes may end at a callee anchor, before its arguments.
        (base.replace("value: 1", "value: 2"), Some([1, 3])),
        (base.replace("value: 1", "value: false"), Some([1, 3])),
        (base.replace("value: 1", "value: \"wrong\""), Some([1, 3])),
        (base.replace("box.leaf.value", "box.leaf.other"), None),
        (base.replace("helper(read(", "helper(idle("), None),
        (
            base.replace("module changes", "module different"),
            Some([4, 0]),
        ),
        (
            base.replace("helper(x: I64)", "helper(x: Bool)")
                .replace("  x\n", "  if x:\n    1\n  else:\n    0\n"),
            None,
        ),
        (
            base.replace("value: I64", "value: Bool")
                .replace("value: 1", "value: true"),
            None,
        ),
        (
            base.replace(
                "helper(x: I64) -> I64:",
                "helper(x: I64) -> I64 effects[io]:",
            ),
            None,
        ),
        (base.to_owned() + "\nfn helper() -> I64:\n  0\n", None),
    ] {
        fs::write(&after, source).unwrap();
        let work = run(&before, &after);
        if let Some(expected) = expected {
            assert_eq!(&work[..2], &expected);
        }
    }
    // The final candidate above is rejected; the same old good cache must survive it.
    let report = directory.join("boundaries.tsv");
    let recovery = Command::new(&executable)
        .arg(&before)
        .arg(&after)
        .arg("boundaries")
        .env("SLIM_RETAINED_REPORT", &report)
        .output()
        .unwrap();
    assert!(recovery.status.success(), "{recovery:?}");
    let observed = fs::read_to_string(report).unwrap();
    let lines: Vec<_> = observed.lines().collect();
    assert_eq!(lines.len(), 32);
    assert_eq!(lines[3], "2\t4");
    assert_eq!(lines[4], "3\t4");
    assert_eq!(lines[5], "4\t0");
    assert_eq!(lines[6], "5\t0");
    assert_eq!(lines[7], "6\t4");
    assert_eq!(lines[8], "7\t4");
    assert_eq!(lines[9], "8\t4");
    assert_eq!(lines[10], "9\t0");
    assert_eq!(lines[11], "10\t4");
    assert_eq!(lines[12], "11\t0");
    assert_eq!(lines[13], "12\t4");
    assert_eq!(lines[14], "13\t0");
    assert_eq!(lines[15], "14\t2");
    assert_eq!(lines[16], "15\t0");
    assert_eq!(lines[17], "16\t2");
    // Invalid storage falls back before writing any imported facts.
    for (case, expected) in [1, 1, 2, 2, 1, 1, 1].into_iter().enumerate() {
        let phase = 17 + case * 2;
        assert_eq!(lines[phase + 1], format!("{phase}\t0"));
        assert_eq!(lines[phase + 2], format!("{}\t{expected}", phase + 1));
    }
    for size in [125, 250, 500, 1_000] {
        let mut source = String::from("module geometric\n\n");
        for i in 0..size {
            source.push_str(&format!(
                "fn f_{i}(value: I64) -> I64:\n  let result: I64 = value\n  result\n\n"
            ));
        }
        source.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
        fs::write(&before, &source).unwrap();
        assert_eq!(run(&before, &before), vec![0, size + 1, 21 * size + 18]);
        let changed = source.replace(
            &format!(
                "fn f_{}(value: I64) -> I64:\n  let result: I64 = value",
                size / 2
            ),
            &format!(
                "fn f_{}(value: I64) -> I64:\n  let result: I64 = 42",
                size / 2
            ),
        );
        fs::write(&after, changed).unwrap();
        assert_eq!(run(&before, &after), vec![1, size, 21 * size - 3]);
    }
    let arithmetic = base
        .replace(
            "fn main(args: Vec[Bytes]) -> I64:",
            "fn main(args: Vec[Bytes]) -> I64 effects[partial]:",
        )
        .replace("value: 1", "value: 1 + 2");
    fs::write(&before, &arithmetic).unwrap();
    fs::write(&after, arithmetic.replace("1 + 2", "1 - 2")).unwrap();
    assert_eq!(&run(&before, &after)[..2], &[1, 3]);
    let strings = "module strings\n\nfn count(value: Bytes) -> I64:\n  bytes.len(value)\n\nfn main(args: Vec[Bytes]) -> I64:\n  count(\"one\")\n";
    fs::write(&before, strings).unwrap();
    fs::write(&after, strings.replace("one", "longer")).unwrap();
    assert_eq!(&run(&before, &after)[..2], &[1, 1]);
    let copyability = "module copyability\n\nstruct Leaf:\n  value: I64\n\nstruct Box:\n  leaf: Leaf\n\nfn inspect(box: Box) -> I64:\n  0\n\nfn idle() -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let box: Box = Box(leaf: Leaf(value: 0))\n  inspect(box)\n";
    fs::write(&before, copyability).unwrap();
    fs::write(
        &after,
        copyability
            .replace("value: I64", "value: Vec[I64]")
            .replace("value: 0", "value: vec.new()"),
    )
    .unwrap();
    assert_eq!(&run(&before, &after)[..2], &[2, 1]);
    let ownership = "module mode_changes\n\nstruct Leaf:\n  values: Vec[I64]\n\nstruct Box:\n  leaf: Leaf\n\nfn take(value: ^Box) -> I64:\n  0\n\nfn relay(value: ^Box) -> I64:\n  take(^value)\n\nfn idle() -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()\n  relay(^Box(leaf: Leaf(values: values)))\n";
    fs::write(&before, ownership).unwrap();
    fs::write(
        &after,
        ownership
            .replace("take(value: ^Box)", "take(value: @Box)")
            .replace("take(^value)", "take(@value)"),
    )
    .unwrap();
    assert_eq!(&run(&before, &after)[..2], &[2, 2]);
    fs::write(
        &after,
        ownership.replace("take(value: ^Box)", "take(value: Box)"),
    )
    .unwrap();
    run(&before, &after);
    // Body-derived termination evidence is rechecked even when callers' typing is reused.
    fs::write(&before, base).unwrap();
    fs::write(&after, base.replace("  x\n", "  helper(x)\n")).unwrap();
    run(&before, &after);
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn retained_project_preparation_preserves_validation_and_current_origins() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let directory = temporary_directory("retained-project");
    let compiler = root.join("build/toolchain/slimc");
    let probe_project = directory.join("probe-project");
    fs::create_dir(&probe_project).unwrap();
    for entry in fs::read_dir(root.join("selfhost")).unwrap() {
        let path = entry.unwrap().path();
        if path
            .extension()
            .is_some_and(|extension| extension == "slim")
        {
            fs::copy(&path, probe_project.join(path.file_name().unwrap())).unwrap();
        }
    }
    let mut manifest = fs::read_to_string(root.join("selfhost/slim.project"))
        .unwrap()
        .lines()
        .filter(|line| !line.contains("(module driver "))
        .collect::<Vec<_>>()
        .join("\n")
        .replace("(entry driver)", "(entry zzprobe)");
    assert!(manifest.ends_with(')'));
    manifest.pop();
    manifest.push_str("\n  (module zzprobe \"zzprobe.slim\" (imports identity memory project retained syntax typing) (exports)))\n");
    fs::write(probe_project.join("slim.project"), manifest).unwrap();
    fs::write(
        probe_project.join("zzprobe.slim"),
        include_str!("fixtures/retained_project.slim"),
    )
    .unwrap();
    let generated = Command::new(&compiler)
        .arg(probe_project.join("slim.project"))
        .output()
        .unwrap();
    assert!(
        generated.status.success(),
        "{}",
        String::from_utf8_lossy(&generated.stderr)
    );
    let c = directory.join("probe.c");
    fs::write(&c, generated.stdout).unwrap();
    let observed = Command::new("awk")
        .args(["-v", "scope=project", "-f"])
        .arg(root.join("scripts/instrument-retained-probe.awk"))
        .arg(&c)
        .output()
        .unwrap();
    assert!(
        observed.status.success(),
        "{}",
        String::from_utf8_lossy(&observed.stderr)
    );
    fs::write(&c, observed.stdout).unwrap();
    let executable = directory.join("probe");
    let compiled = Command::new(native_compiler())
        .args(["-std=c11", "-O1", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(root.join("runtime"))
        .arg("-include")
        .arg(root.join("benchmarks/instrumentation/retained_probe.h"))
        .arg(&c)
        .arg(root.join("runtime/slim_rt.c"))
        .arg(root.join("benchmarks/instrumentation/retained_probe.c"))
        .arg("-o")
        .arg(&executable)
        .output()
        .unwrap();
    assert!(
        compiled.status.success(),
        "{}",
        String::from_utf8_lossy(&compiled.stderr)
    );
    let report = directory.join("native.tsv");
    let invoke = |before: &Path, after: &Path, mode: &str| {
        let output = Command::new(&executable)
            .arg(before)
            .arg(after)
            .arg(mode)
            .env("SLIM_RETAINED_REPORT", &report)
            .output()
            .unwrap();
        let observed = fs::read_to_string(&report).unwrap();
        let mut lines = observed.lines();
        assert_eq!(lines.next(), Some("slim-retained\t1\texact\t1000000000"));
        let counts: Vec<i64> = lines
            .enumerate()
            .map(|(index, line)| {
                let (phase, count) = line.split_once('\t').unwrap();
                assert_eq!(phase.parse::<usize>().unwrap(), index);
                count.parse().unwrap()
            })
            .collect();
        (output, counts)
    };
    let work = |before: &Path, after: &Path| {
        let (output, counts) = invoke(before, after, "work");
        assert!(
            output.status.success(),
            "status {:?}, stdout {}, stderr {}",
            output.status,
            String::from_utf8_lossy(&output.stdout),
            String::from_utf8_lossy(&output.stderr)
        );
        assert!(output.stderr.is_empty());
        let work: Vec<i64> = String::from_utf8(output.stdout)
            .unwrap()
            .split_whitespace()
            .map(|value| value.parse().unwrap())
            .collect();
        assert_eq!(work.len(), 3);
        assert_eq!(counts.len(), 2);
        assert_eq!(counts[1], work[0]);
        work
    };
    let corpus = directory.join("corpus");
    fs::create_dir(&corpus).unwrap();
    let mut paths: Vec<_> = fs::read_dir(root.join("conformance/pass"))
        .unwrap()
        .map(|entry| entry.unwrap().path())
        .filter(|path| {
            path.extension()
                .is_some_and(|extension| extension == "slim")
        })
        .collect();
    paths.extend(
        fs::read_dir(root.join("benchmarks/challenges"))
            .unwrap()
            .map(|entry| entry.unwrap().path().join("program.slim"))
            .filter(|path| path.is_file()),
    );
    paths.sort();
    for path in paths {
        let source = fs::read_to_string(&path).unwrap();
        let module = source
            .lines()
            .find_map(|line| line.strip_prefix("module "))
            .unwrap();
        fs::write(corpus.join("program.slim"), &source).unwrap();
        let manifest = corpus.join("slim.project");
        fs::write(&manifest, format!("(project 1 (entry {module}) (module {module} \"program.slim\" (imports) (exports)))\n")).unwrap();
        let result = work(&manifest, &manifest);
        assert_eq!(result[0], 0, "{}", path.display());
        assert!(result[1] > 0);
    }
    let before = directory.join("initial");
    let after = directory.join("relocated");
    fs::create_dir(&before).unwrap();
    fs::create_dir(&after).unwrap();
    let manifest = "(project 1 (entry app) (module app \"app.slim\" (imports data) (exports)) (module data \"data.slim\" (imports) (exports Box helper)))\n";
    let data = "module data\n\nstruct Box:\n  value: I64\n\nfn helper(x: I64) -> I64:\n  x\n\nfn idle() -> I64:\n  9\n";
    let app = "module app\n\nfn read(box: data.Box) -> I64:\n  box.value\n\nfn main(args: Vec[Bytes]) -> I64:\n  data.helper(read(data.Box(value: 1)))\n";
    let write = |folder: &Path, manifest: &str, data: &str, app: &str| {
        fs::write(folder.join("slim.project"), manifest).unwrap();
        fs::write(folder.join("data.slim"), data).unwrap();
        fs::write(folder.join("app.slim"), app).unwrap();
    };
    write(&before, manifest, data, app);
    let initial = before.join("slim.project");
    let updated = after.join("slim.project");
    write(&after, manifest, data, app);
    for (mode, expected) in [("stale", "4 0 0\n"), ("capacity", "-1 0 0\n")] {
        let (attempt, counts) = invoke(&initial, &updated, mode);
        assert!(attempt.status.success());
        assert_eq!(attempt.stdout, expected.as_bytes());
        assert_eq!(counts, vec![4, 4]);
    }
    for (new_manifest, new_data, new_app, expected) in [
        (
            manifest.to_owned(),
            data.to_owned(),
            app.to_owned(),
            Some([0, 4]),
        ),
        (
            manifest.to_owned(),
            data.replace("  x\n", "  42\n"),
            app.to_owned(),
            Some([1, 3]),
        ),
        (
            manifest.to_owned(),
            data.replace("fn idle() -> I64:\n  9\n", ""),
            app.to_owned(),
            Some([0, 3]),
        ),
        (
            manifest.to_owned(),
            format!("{data}\nfn added() -> I64:\n  2\n"),
            app.to_owned(),
            Some([1, 4]),
        ),
        (
            manifest.replace("Box helper", "Box renamed"),
            data.replace("helper", "renamed"),
            app.replace("helper", "renamed"),
            Some([2, 2]),
        ),
        (
            manifest.to_owned(),
            data.replace("\nfn idle() -> I64:\n  9\n", "")
                .replace("struct Box:", "fn idle() -> I64:\n  9\n\nstruct Box:"),
            app.to_owned(),
            Some([0, 4]),
        ),
        (
            manifest.to_owned(),
            data.replace("\n", "\r\n"),
            format!("# current origin\n{app}"),
            Some([0, 4]),
        ),
        (
            manifest.to_owned(),
            data.replace("value: I64", "value: Bool"),
            app.replace("  box.value", "  if box.value:\n    1\n  else:\n    0")
                .replace("value: 1", "value: true"),
            None,
        ),
    ] {
        write(&after, &new_manifest, &new_data, &new_app);
        let actual = work(&initial, &updated);
        if let Some(expected) = expected {
            assert_eq!(&actual[..2], &expected);
        }
        let (emitted, _) = invoke(&initial, &updated, "emit");
        let clean = Command::new(&compiler).arg(&updated).output().unwrap();
        assert!(emitted.status.success() && clean.status.success());
        assert_eq!(emitted.stdout, clean.stdout);
        assert_eq!(emitted.stderr, clean.stderr);
    }
    let owned_data = data.replace("value: I64", "value: Vec[I64]");
    let shared_app = app
        .replace("  box.value", "  vec.len(box.value)")
        .replace(
            "fn main(args: Vec[Bytes]) -> I64:",
            "fn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()",
        )
        .replace("value: 1", "value: values");
    write(&before, manifest, &owned_data, &shared_app);
    let owned_app = shared_app.replace("box: data.Box", "box: ^data.Box");
    write(
        &after,
        manifest,
        &owned_data,
        &owned_app.replace("read(data.Box", "read(^data.Box"),
    );
    assert_eq!(&work(&initial, &updated)[..2], &[2, 2]);
    write(&after, manifest, &owned_data, &owned_app);
    let (rejected, _) = invoke(&initial, &updated, "check");
    let clean = Command::new(&compiler)
        .arg("check")
        .arg(&updated)
        .output()
        .unwrap();
    assert!(!clean.status.success());
    assert_eq!(rejected.status.code(), clean.status.code());
    assert_eq!(rejected.stdout, clean.stdout);
    assert_eq!(rejected.stderr, clean.stderr);
    let (recovered, counts) = invoke(&initial, &updated, "recover");
    assert!(recovered.status.success());
    assert_eq!(counts[2], 0);
    write(&before, manifest, data, app);
    let extended = manifest.replace(
        "(exports Box helper)))",
        "(exports Box helper)) (module extra \"extra.slim\" (imports) (exports)))",
    );
    let extra = "module extra\n\nfn extra() -> I64:\n  7\n";
    write(&after, &extended, data, app);
    fs::write(after.join("extra.slim"), extra).unwrap();
    assert_eq!(&work(&initial, &updated)[..2], &[1, 4]);
    write(&before, &extended, data, app);
    fs::write(before.join("extra.slim"), extra).unwrap();
    write(&after, manifest, data, app);
    assert_eq!(&work(&initial, &updated)[..2], &[0, 4]);
    write(&before, manifest, data, app);
    write(
        &after,
        &manifest
            .replace("(module data ", "(module lib ")
            .replace("(imports data)", "(imports lib)"),
        &data.replace("module data", "module lib"),
        &app.replace("data.", "lib."),
    );
    assert_eq!(&work(&initial, &updated)[..2], &[4, 0]);
    write(
        &after,
        manifest,
        &data.replace("struct Box:", "struct Outer:\n  box: Box\n\nstruct Box:"),
        app,
    );
    assert_eq!(&work(&initial, &updated)[..2], &[0, 4]);
    for (new_manifest, new_data, new_app) in [
        (manifest.replace("(module data \"data.slim\" (imports)", "(module data \"data.slim\" (imports app)"), data.to_owned(), app.to_owned()),
        (manifest.replace("(project 1", "(project 2"), data.to_owned(), app.to_owned()),
        (manifest.replace("data.slim", "missing.slim"), data.to_owned(), app.to_owned()),
        (manifest.to_owned(), "".to_owned(), app.to_owned()),
        ("(project 1 (entry app) (module data \"data.slim\" (imports) (exports Box helper)) (module app \"app.slim\" (imports data) (exports)))\n".to_owned(), data.to_owned(), app.to_owned()),
        (
            manifest.replace("(imports data)", "(imports)"),
            data.to_owned(),
            app.to_owned(),
        ),
        (
            manifest.replace("Box helper", "Box"),
            data.to_owned(),
            app.to_owned(),
        ),
        (
            manifest.replace("(entry app)", "(entry data)"),
            data.to_owned(),
            app.to_owned(),
        ),
        (
            manifest.to_owned(),
            data.replace("module data", "module other"),
            app.to_owned(),
        ),
        (
            manifest.to_owned(),
            data.replace(
                "helper(x: I64) -> I64:",
                "helper(x: I64) -> I64 effects[io]:",
            ),
            app.to_owned(),
        ),
        (
            manifest.to_owned(),
            data.replace("helper(x: I64)", "helper(x: Bool)")
                .replace("  x\n", "  1\n"),
            app.to_owned(),
        ),
        (
            manifest.to_owned(),
            data.to_owned(),
            format!(
                "# shifted diagnostic\n{}",
                app.replace("value: 1", "value: false")
            ),
        ),
        (
            manifest.to_owned(),
            format!("{data}\nfn helper() -> I64:\n  0\n"),
            app.to_owned(),
        ),
        (
            manifest.to_owned(),
            data.to_owned(),
            app.replace("  data.helper", "   data.helper"),
        ),
    ] {
        write(&after, &new_manifest, &new_data, &new_app);
        let (rejected, _) = invoke(&initial, &updated, "check");
        let clean = Command::new(&compiler)
            .arg("check")
            .arg(&updated)
            .output()
            .unwrap();
        assert!(
            !clean.status.success(),
            "case unexpectedly accepted: {new_manifest} {new_data} {new_app}"
        );
        assert_eq!(rejected.status.code(), clean.status.code());
        assert_eq!(rejected.stdout, clean.stdout);
        assert_eq!(rejected.stderr, clean.stderr);
        let (recovered, counts) = invoke(&initial, &updated, "recover");
        assert!(
            recovered.status.success(),
            "{}",
            String::from_utf8_lossy(&recovered.stdout)
        );
        assert_eq!(counts.len(), 3);
        assert_eq!(counts[2], 0);
        assert!(
            String::from_utf8(recovered.stdout)
                .unwrap()
                .lines()
                .last()
                .unwrap()
                .starts_with("0 4 ")
        );
    }
    for size in [125, 250, 500, 1_000] {
        let data = format!(
            "module data\n\n{}",
            (0..size)
                .map(|index| format!("fn f_{index}(x: I64) -> I64:\n  x\n\n"))
                .collect::<String>()
        );
        let app = "module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  data.f_0(1)\n";
        let manifest = "(project 1 (entry app) (module app \"app.slim\" (imports data) (exports)) (module data \"data.slim\" (imports) (exports f_0)))\n";
        write(&before, manifest, &data, app);
        write(&after, manifest, &data, app);
        assert_eq!(work(&initial, &updated), vec![0, size + 1, 15 * size + 22]);
        write(&after, manifest, &data.replacen("  x\n", "  42\n", 1), app);
        assert_eq!(work(&initial, &updated), vec![1, size, 15 * size + 7]);
    }
    fs::remove_dir_all(directory).unwrap();
}

#[test]
fn transactional_session_publishes_complete_snapshots_and_observes_reuse() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let output = Command::new("sh")
        .arg(root.join("scripts/verify-session.sh"))
        .arg(root.join("build/toolchain/slimc"))
        .arg("quick")
        .current_dir(&root)
        .output()
        .unwrap();
    assert!(
        output.status.success(),
        "session verification failed: {}\n{}",
        String::from_utf8_lossy(&output.stdout),
        String::from_utf8_lossy(&output.stderr)
    );
}
