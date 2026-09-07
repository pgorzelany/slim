//! Build the historical SLIM estimate fixture with the production compiler.
//! No parsing, checking, inference, or acceptance semantics live in this helper.
use std::path::{Path, PathBuf};
use std::process::Command;

pub fn build(compiler: &Path, directory: &Path) -> Result<PathBuf, String> {
    let root = Path::new(env!("CARGO_MANIFEST_DIR"));
    let output = Command::new("sh")
        .arg(root.join("scripts/build-session-estimate.sh"))
        .arg(compiler)
        .arg(directory)
        .current_dir(root)
        .output()
        .map_err(|error| format!("build estimate fixture: {error}"))?;
    if !output.status.success() {
        return Err(format!(
            "build estimate fixture: {} / {} / {}",
            output.status,
            String::from_utf8_lossy(&output.stdout),
            String::from_utf8_lossy(&output.stderr)
        ));
    }
    Ok(directory.join("estimate"))
}
