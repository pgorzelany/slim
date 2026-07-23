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
    assert!(report_parentheses_are_balanced(&first.stdout));
    let report = String::from_utf8(first.stdout).unwrap();
    assert!(report.starts_with("(analysis 3 (module vector-sum)"));
    assert!(report.contains("(fact-limit 64)"));
    assert!(report.contains("(quality (guarantee exact)"));
    assert!(report.contains("(function-quality 3 fill"));
    assert!(report.contains("(allocation-sites 1)"));
    assert!(report.contains("(totality (guarantee unknown) (reason calls-or-recursion))"));
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
    assert!(report.contains("(totality (guarantee exact) (status total))"));
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
        "(parallelism (guarantee exact) (function-limit 64) (edge-limit 4096)",
        "safe-left (guarantee exact) (status safe)",
        "safe-right (guarantee exact) (status safe)",
        "traps (guarantee exact) (status unavailable) (reason checked-trap)",
        "calls-trap (guarantee exact) (status unavailable) (reason callee-not-safe)",
        "allocates (guarantee exact) (status unavailable) (reason allocation-or-io)",
        "borrows (guarantee exact) (status unavailable) (reason exclusive-borrow)",
        "mutates (guarantee exact) (status unavailable) (reason mutation)",
        "repeats (guarantee exact) (status unavailable) (reason recurrence)",
        "cycle-left (guarantee unknown) (status unknown) (reason call-cycle)",
        "(race-free true) (deadlock-free true) (profitability unknown)",
        "(eligible-sites 1)",
    ] {
        assert!(
            report.contains(required),
            "missing parallel fact: {required}"
        );
    }
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
            .contains("needs-late (guarantee unknown) (status unknown) (reason function-limit)")
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
    assert!(edge_report.contains("build (guarantee unknown) (status unknown) (reason edge-limit)"));

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
    assert!(report.starts_with("(analysis 3 (module project)"));
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
    assert!(report.starts_with("(reduction-proof 1 (guarantee bounded)"));
    assert!(report.contains("(pass-limit 8) (site-limit 64)"));
    assert!(report.contains("dead-scalar-binding"));
    assert!(report.contains("right-identity"));
    assert!(report.ends_with(")\n"));

    let directory = temporary_directory("proof-replay");
    let reduced = directory.join("reduced.slim");
    let reduced_output = Command::new(slimc())
        .arg("reduce")
        .arg(&source)
        .output()
        .unwrap();
    assert!(reduced_output.status.success());
    fs::write(&reduced, reduced_output.stdout).unwrap();
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
        b"(equivalence 1 (status equivalent) (domain exact) (cases 4))\n"
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
        b"(equivalence 1 (status different) (domain exact) (counterexample (inputs false true) (left false) (right true)))\n"
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
        b"(equivalence 1 (status unknown) (reason unsupported-signature))\n"
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
