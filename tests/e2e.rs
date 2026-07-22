use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::time::{SystemTime, UNIX_EPOCH};

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

fn slimc() -> &'static str {
    env!("CARGO_BIN_EXE_slimc")
}

fn slim_bootstrap() -> &'static str {
    env!("CARGO_BIN_EXE_slim-bootstrap")
}

fn write_source(directory: &Path, source: &str) -> PathBuf {
    let path = directory.join("program.slim");
    fs::write(&path, source).unwrap();
    path
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
    assert!(lines.iter().all(|line| line.starts_with("{\"code\":")));
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
    assert!(stdout.contains("bootstrap fixed point:"), "{stdout}");
    assert!(stdout.contains("native smoke test passed"), "{stdout}");
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
