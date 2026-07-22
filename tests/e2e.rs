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

fn slimc() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("slimc")
}

fn slim_bootstrap() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("bootstrap.sh")
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
    let report = String::from_utf8(first.stdout).unwrap();
    assert!(report.starts_with("(analysis 1 (module vector-sum)"));
    assert!(report.contains("(fact-limit 64)"));
    assert!(report.contains("(function 71 sum"));
    assert!(report.contains("(binding 76 values (type (Vec I64)) (ownership owned)"));
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
            "(binding 230 items (type unknown) (ownership unknown) (uses 1) (last-use 235)"
        )
    );
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
