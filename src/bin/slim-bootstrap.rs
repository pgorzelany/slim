use slim::bootstrap;

fn main() {
    if let Err(error) = run() {
        eprintln!("slim-bootstrap: {error}");
        std::process::exit(1);
    }
}

fn run() -> Result<(), String> {
    let root = bootstrap::workspace_root()?;
    let report = bootstrap::bootstrap(&root)?;
    println!(
        "bootstrap fixed point: {} identical C bytes; native smoke test passed",
        report.fixed_c_bytes
    );
    Ok(())
}
