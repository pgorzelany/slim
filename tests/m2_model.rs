use std::path::Path;
use std::process::{Command, Output};

fn pool_model(options: &[&str], optimized: bool) -> Output {
    let root = Path::new(env!("CARGO_MANIFEST_DIR"));
    let mut command = Command::new("python3");
    if optimized {
        command.arg("-O");
    }
    command
        .arg(root.join("scripts/models/m2_pool.py"))
        .args(options)
        .output()
        .expect("execute the bounded allocator model")
}

#[test]
fn pool_model_agrees_with_independent_occupancy_and_index_oracles() {
    let output = pool_model(
        &[
            "--check",
            "--depth",
            "4",
            "--state-limit",
            "104",
            "--transition-limit",
            "1015",
        ],
        false,
    );
    assert!(
        output.status.success(),
        "{}",
        String::from_utf8_lossy(&output.stderr)
    );
    let report = String::from_utf8(output.stdout).unwrap();
    for fact in [
        "\"classification\": \"bounded\"",
        "\"states\": 104",
        "\"transitions\": 1015",
        "\"arithmetic_cases\": 281",
        "\"operations\": 268",
        "\"maximum_word_visits\": 4",
        "\"maximum_pool_index_bytes\": 4261024",
        "unknown; not implemented or verified by this model",
    ] {
        assert!(report.contains(fact), "missing model fact {fact}: {report}");
    }
}

#[test]
fn pool_model_exhaustion_is_unknown_and_never_a_truncated_pass() {
    for (option, value, reason) in [
        (
            "--state-limit",
            "103",
            "state budget exhausted; result unknown",
        ),
        (
            "--transition-limit",
            "1014",
            "transition budget exhausted; result unknown",
        ),
    ] {
        let output = pool_model(&["--check", option, value], false);
        assert_eq!(output.status.code(), Some(1));
        assert!(output.stdout.is_empty());
        assert!(String::from_utf8_lossy(&output.stderr).contains(reason));
    }
}

#[test]
fn pool_model_requires_explicit_checked_search_and_valid_limits() {
    for options in [
        vec![],
        vec!["--check", "--depth", "0"],
        vec!["--check", "--depth", "7"],
        vec!["--check", "--state-limit", "0"],
        vec!["--check", "--state-limit", "20001"],
        vec!["--check", "--transition-limit", "0"],
        vec!["--check", "--transition-limit", "250001"],
    ] {
        let output = pool_model(&options, false);
        assert_eq!(output.status.code(), Some(2));
        assert!(output.stdout.is_empty());
    }
    let output = pool_model(&["--check"], true);
    assert_eq!(output.status.code(), Some(2));
    assert!(String::from_utf8_lossy(&output.stderr).contains("assertions must remain enabled"));
}

#[test]
fn native_pool_matches_the_independent_model_and_sanitizer_witnesses() {
    let root = Path::new(env!("CARGO_MANIFEST_DIR"));
    let nonce = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap()
        .as_nanos();
    let directory =
        std::env::temp_dir().join(format!("slim-native-pool-{}-{nonce}", std::process::id()));
    let output = Command::new("python3")
        .arg(root.join("scripts/verify-pool.py"))
        .args(["--check", "--sanitize", "--output"])
        .arg(&directory)
        .output()
        .expect("execute native allocator comparisons");
    assert!(
        output.status.success(),
        "native pool receipt: {}\n{}\n{}",
        directory.display(),
        String::from_utf8_lossy(&output.stdout),
        String::from_utf8_lossy(&output.stderr)
    );
    assert!(String::from_utf8_lossy(&output.stdout).contains("\"result\": \"passed\""));
    std::fs::remove_dir_all(directory).unwrap();
}

#[test]
fn native_reserved_buffers_return_after_structured_join() {
    let root = Path::new(env!("CARGO_MANIFEST_DIR"));
    let nonce = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap()
        .as_nanos();
    let directory =
        std::env::temp_dir().join(format!("slim-pool-workers-{}-{nonce}", std::process::id()));
    let output = Command::new("python3")
        .arg(root.join("scripts/verify-pool-workers.py"))
        .args(["--check", "--output"])
        .arg(&directory)
        .output()
        .expect("execute native pool/task witness");
    assert!(
        output.status.success(),
        "native worker receipt: {}\n{}\n{}",
        directory.display(),
        String::from_utf8_lossy(&output.stdout),
        String::from_utf8_lossy(&output.stderr)
    );
    assert!(String::from_utf8_lossy(&output.stdout).contains("\"result\": \"passed\""));
    std::fs::remove_dir_all(directory).unwrap();
}

#[test]
fn bounded_native_host_storage_and_clock_boundaries() {
    let root = Path::new(env!("CARGO_MANIFEST_DIR"));
    let nonce = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap()
        .as_nanos();
    let directory =
        std::env::temp_dir().join(format!("slim-native-host-{}-{nonce}", std::process::id()));
    let output = Command::new("python3")
        .arg(root.join("scripts/verify-host.py"))
        .args(["--check", "--output"])
        .arg(&directory)
        .output()
        .expect("execute native host boundaries");
    assert!(
        output.status.success(),
        "native host receipt: {}\n{}\n{}",
        directory.display(),
        String::from_utf8_lossy(&output.stdout),
        String::from_utf8_lossy(&output.stderr)
    );
    assert!(String::from_utf8_lossy(&output.stdout).contains("\"result\": \"passed\""));
    std::fs::remove_dir_all(directory).unwrap();
}
