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

    let executable = directory.join("answer");
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
    assert_eq!(fs::read(first).unwrap(), fs::read(second).unwrap());
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
