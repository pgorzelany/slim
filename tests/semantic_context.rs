use std::path::PathBuf;
use std::process::Command;

#[test]
fn production_semantic_context_matches_its_independent_contract() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let output = Command::new("python3")
        .arg(root.join("scripts/verify-context.py"))
        .arg("--compiler")
        .arg(root.join("build/toolchain/slimc"))
        .arg("--section")
        .arg("all")
        .current_dir(&root)
        .output()
        .expect("execute stdlib-only production-context verification");
    assert!(
        output.status.success(),
        "context conformance failed:\n{}\n{}",
        String::from_utf8_lossy(&output.stdout),
        String::from_utf8_lossy(&output.stderr)
    );
}

#[test]
fn pilot_transport_preserves_budgets_and_failure_accounting() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let output = Command::new("python3")
        .args(["-B", "-m", "unittest", "discover", "-s"])
        .arg(root.join("benchmarks/agent-development"))
        .args(["-p", "test_transport.py"])
        .current_dir(&root)
        .output()
        .expect("execute independent pilot transport tests");
    assert!(
        output.status.success(),
        "{}",
        String::from_utf8_lossy(&output.stderr)
    );
}
