use std::collections::{BTreeMap, BTreeSet};
use std::env;
use std::ffi::OsString;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};
use std::time::{SystemTime, UNIX_EPOCH};

use slim::bootstrap;
use slim::compiler;
use slim::span::Source;

const RUNTIME_C: &str = include_str!("../../runtime/slim_rt.c");
const RUNTIME_H: &str = include_str!("../../runtime/slim_rt.h");

#[derive(Clone, Debug)]
struct Fixture {
    id: String,
    mode: String,
    path: PathBuf,
    selfhost: String,
    expectation: String,
    coverage: BTreeSet<String>,
}

fn main() {
    if let Err(error) = run() {
        eprintln!("slim-conform: {error}");
        std::process::exit(1);
    }
}

fn run() -> Result<(), String> {
    let command = env::args().nth(1).unwrap_or_else(|| "check".to_owned());
    if command != "check" && command != "differential" {
        return Err("usage: slim-conform [check|differential]".to_owned());
    }
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let fixtures = load_manifest(&root)?;
    check_coverage(&root, &fixtures)?;
    let mut counts = BTreeMap::<String, usize>::new();
    for fixture in &fixtures {
        run_stage0_fixture(fixture)?;
        *counts.entry(fixture.mode.clone()).or_default() += 1;
    }
    let summary = counts
        .iter()
        .map(|(mode, count)| format!("{mode}={count}"))
        .collect::<Vec<_>>()
        .join(", ");
    let parity_count = fixtures
        .iter()
        .filter(|fixture| fixture.selfhost == "parity")
        .count();
    println!(
        "conformance: {} stage-0 fixtures passed ({summary}); {parity_count} marked for self-host parity",
        fixtures.len(),
    );

    if command == "differential" {
        let report = bootstrap::bootstrap(&root)?;
        let mut parity_run = 0;
        let mut deferred = BTreeMap::<String, usize>::new();
        for fixture in &fixtures {
            if fixture.selfhost == "parity" {
                run_selfhost_fixture(fixture, &report.compiler)?;
                parity_run += 1;
            } else {
                let reason = fixture
                    .selfhost
                    .strip_prefix("stage0-only(")
                    .and_then(|value| value.strip_suffix(')'))
                    .expect("manifest classification was validated");
                *deferred.entry(reason.to_owned()).or_default() += 1;
            }
        }
        if parity_run != parity_count {
            return Err(format!(
                "self-host differential silently skipped fixtures: expected {parity_count}, ran {parity_run}"
            ));
        }
        let deferred_summary = deferred
            .iter()
            .map(|(reason, count)| format!("{reason}={count}"))
            .collect::<Vec<_>>()
            .join(", ");
        println!(
            "differential: {parity_run} parity fixtures passed through {}; {} stage-0-only fixtures deferred ({deferred_summary}); bootstrap fixed at {} C bytes",
            report.compiler.display(),
            fixtures.len() - parity_run,
            report.fixed_c_bytes,
        );
    }
    Ok(())
}

fn load_manifest(root: &Path) -> Result<Vec<Fixture>, String> {
    let path = root.join("conformance/manifest.tsv");
    let text = fs::read_to_string(&path)
        .map_err(|error| format!("cannot read {}: {error}", path.display()))?;
    let mut fixtures = Vec::new();
    let mut ids = BTreeSet::new();
    for (line_index, line) in text.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() != 6 {
            return Err(format!(
                "{}:{} must contain six tab-separated columns",
                path.display(),
                line_index + 1
            ));
        }
        if !ids.insert(columns[0].to_owned()) {
            return Err(format!("duplicate fixture id {}", columns[0]));
        }
        if !matches!(
            columns[1],
            "check-pass" | "check-fail" | "run" | "trap" | "format" | "emit"
        ) {
            return Err(format!(
                "{}:{} has unknown mode {}",
                path.display(),
                line_index + 1,
                columns[1]
            ));
        }
        if columns[3] != "parity"
            && !(columns[3].starts_with("stage0-only(") && columns[3].ends_with(')'))
        {
            return Err(format!(
                "{}:{} must classify selfhost as parity or stage0-only(reason)",
                path.display(),
                line_index + 1
            ));
        }
        let source_path = root.join(columns[2]);
        if !source_path.is_file() {
            return Err(format!(
                "fixture {} source does not exist: {}",
                columns[0],
                source_path.display()
            ));
        }
        let coverage = columns[5]
            .split(',')
            .filter(|tag| !tag.is_empty())
            .map(str::to_owned)
            .collect();
        fixtures.push(Fixture {
            id: columns[0].to_owned(),
            mode: columns[1].to_owned(),
            path: source_path,
            selfhost: columns[3].to_owned(),
            expectation: columns[4].to_owned(),
            coverage,
        });
    }
    if fixtures.is_empty() {
        return Err("conformance manifest contains no fixtures".to_owned());
    }
    Ok(fixtures)
}

fn check_coverage(root: &Path, fixtures: &[Fixture]) -> Result<(), String> {
    let surface_path = root.join("design/surface.tsv");
    let surface = fs::read_to_string(&surface_path)
        .map_err(|error| format!("cannot read {}: {error}", surface_path.display()))?;
    let mut required = BTreeSet::new();
    for line in surface.lines() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() == 4 {
            required.insert(format!("{}:{}", columns[0], columns[1]));
        }
    }
    required.extend(
        [
            "effect:enforcement",
            "match:exhaustive",
            "ownership:exclusive-borrow",
            "ownership:move",
            "ownership:no-alias",
            "recovery:malformed",
            "recovery:parser",
            "runtime:allocation",
            "runtime:bounds-trap",
            "runtime:overflow-trap",
            "tool:emit-deterministic",
            "tool:format",
            "type:strong",
        ]
        .into_iter()
        .map(str::to_owned),
    );
    let covered: BTreeSet<_> = fixtures
        .iter()
        .flat_map(|fixture| fixture.coverage.iter().cloned())
        .collect();
    let missing: Vec<_> = required.difference(&covered).cloned().collect();
    if !missing.is_empty() {
        return Err(format!(
            "conformance coverage is missing: {}",
            missing.join(", ")
        ));
    }
    Ok(())
}

fn run_stage0_fixture(fixture: &Fixture) -> Result<(), String> {
    let text = fs::read_to_string(&fixture.path)
        .map_err(|error| format!("{}: cannot read source: {error}", fixture.id))?;
    let source = Source::new(&fixture.path, text);
    match fixture.mode.as_str() {
        "check-pass" => {
            let compilation = compiler::compile(source);
            require_success(fixture, &compilation)
        }
        "check-fail" => {
            let compilation = compiler::compile(source);
            if compilation.succeeded() {
                return Err(format!(
                    "{}: expected rejection, but check passed",
                    fixture.id
                ));
            }
            let actual = compilation
                .diagnostics
                .iter()
                .map(|diagnostic| {
                    format!(
                        "{}@{}:{}",
                        diagnostic.code, diagnostic.primary.start, diagnostic.primary.end
                    )
                })
                .collect::<Vec<_>>()
                .join(",");
            if fixture.expectation == "TODO" {
                return Err(format!(
                    "{}: replace TODO diagnostic expectation with {actual}",
                    fixture.id
                ));
            }
            if actual != fixture.expectation {
                return Err(format!(
                    "{}: diagnostics differ\nexpected: {}\nactual:   {actual}",
                    fixture.id, fixture.expectation
                ));
            }
            Ok(())
        }
        "format" => {
            let formatted = compiler::format_source(&source)
                .map_err(|diagnostics| diagnostic_error(fixture, &diagnostics))?;
            let formatted_source = Source::new(&fixture.path, formatted.clone());
            let second = compiler::format_source(&formatted_source)
                .map_err(|diagnostics| diagnostic_error(fixture, &diagnostics))?;
            if formatted != second {
                return Err(format!(
                    "{}: canonical formatting is not idempotent",
                    fixture.id
                ));
            }
            let compilation = compiler::compile(formatted_source);
            require_success(fixture, &compilation)
        }
        "emit" => {
            let first = compiler::compile(source.clone());
            require_success(fixture, &first)?;
            let second = compiler::compile(source);
            require_success(fixture, &second)?;
            if first.emit_c() != second.emit_c() {
                return Err(format!("{}: generated C is not deterministic", fixture.id));
            }
            Ok(())
        }
        "run" | "trap" => {
            let compilation = compiler::compile(source);
            require_success(fixture, &compilation)?;
            let generated = compilation
                .emit_c()
                .ok_or_else(|| format!("{}: successful check emitted no C", fixture.id))?;
            let output = compile_and_run(fixture, &generated)?;
            check_process_expectation(fixture, &output)
        }
        _ => unreachable!("manifest mode validated"),
    }
}

fn run_selfhost_fixture(fixture: &Fixture, compiler: &Path) -> Result<(), String> {
    if matches!(fixture.mode.as_str(), "check-fail" | "format") {
        return Err(format!(
            "{}: mode {} cannot claim self-host parity because the restricted compiler does not implement it",
            fixture.id, fixture.mode
        ));
    }
    let first = bootstrap::run_compiler(compiler, &fixture.path)
        .map_err(|error| format!("{}: self-host compilation failed: {error}", fixture.id))?;
    let second = bootstrap::run_compiler(compiler, &fixture.path).map_err(|error| {
        format!(
            "{}: repeated self-host compilation failed: {error}",
            fixture.id
        )
    })?;
    if first != second {
        return Err(format!(
            "{}: self-host generated C is not byte deterministic",
            fixture.id
        ));
    }
    let generated = std::str::from_utf8(&first)
        .map_err(|error| format!("{}: self-host emitted non-UTF-8 C: {error}", fixture.id))?;
    let output = compile_and_run(fixture, generated)?;
    match fixture.mode.as_str() {
        "run" | "trap" => check_process_expectation(fixture, &output),
        "check-pass" | "emit" if output.status.success() => Ok(()),
        "check-pass" | "emit" => Err(format!(
            "{}: self-host output compiled but failed at runtime with {}\n{}",
            fixture.id,
            output.status,
            String::from_utf8_lossy(&output.stderr)
        )),
        _ => unreachable!("unsupported parity modes rejected above"),
    }
}

fn require_success(fixture: &Fixture, compilation: &compiler::Compilation) -> Result<(), String> {
    if compilation.succeeded() {
        return Ok(());
    }
    Err(diagnostic_error(fixture, &compilation.diagnostics))
}

fn diagnostic_error(fixture: &Fixture, diagnostics: &[slim::diagnostic::Diagnostic]) -> String {
    let rendered = diagnostics
        .iter()
        .map(|diagnostic| {
            format!(
                "{}@{}:{} {}",
                diagnostic.code,
                diagnostic.primary.start,
                diagnostic.primary.end,
                diagnostic.message
            )
        })
        .collect::<Vec<_>>()
        .join("\n");
    format!("{}: unexpected diagnostics\n{rendered}", fixture.id)
}

fn compile_and_run(fixture: &Fixture, generated: &str) -> Result<Output, String> {
    let nonce = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map_err(|error| format!("system clock error: {error}"))?
        .as_nanos();
    let directory = env::temp_dir().join(format!("slim-conform-{}-{nonce}", std::process::id()));
    fs::create_dir(&directory)
        .map_err(|error| format!("cannot create {}: {error}", directory.display()))?;
    let result = compile_and_run_in(fixture, generated, &directory);
    if let Err(error) = fs::remove_dir_all(&directory) {
        eprintln!(
            "slim-conform warning: cannot remove {}: {error}",
            directory.display()
        );
    }
    result
}

fn compile_and_run_in(
    fixture: &Fixture,
    generated: &str,
    directory: &Path,
) -> Result<Output, String> {
    let source = directory.join("program.c");
    let runtime_c = directory.join("slim_rt.c");
    let runtime_h = directory.join("slim_rt.h");
    let executable = directory.join("program");
    fs::write(&source, generated)
        .map_err(|error| format!("cannot write {}: {error}", source.display()))?;
    fs::write(&runtime_c, RUNTIME_C)
        .map_err(|error| format!("cannot write {}: {error}", runtime_c.display()))?;
    fs::write(&runtime_h, RUNTIME_H)
        .map_err(|error| format!("cannot write {}: {error}", runtime_h.display()))?;
    let native_compiler = env::var_os("CC").unwrap_or_else(|| OsString::from("clang"));
    let build = Command::new(&native_compiler)
        .arg("-std=c11")
        .arg("-O2")
        .arg("-Wall")
        .arg("-Wextra")
        .arg("-Werror")
        .arg("-I")
        .arg(directory)
        .arg(&source)
        .arg(&runtime_c)
        .arg("-o")
        .arg(&executable)
        .output()
        .map_err(|error| format!("cannot execute {:?}: {error}", native_compiler))?;
    if !build.status.success() {
        return Err(format!(
            "{}: native build failed with {}\n{}{}",
            fixture.id,
            build.status,
            String::from_utf8_lossy(&build.stdout),
            String::from_utf8_lossy(&build.stderr)
        ));
    }
    Command::new(&executable)
        .arg(&fixture.path)
        .output()
        .map_err(|error| format!("{}: cannot run native output: {error}", fixture.id))
}

fn check_process_expectation(fixture: &Fixture, output: &Output) -> Result<(), String> {
    let expected = parse_process_expectation(&fixture.expectation)
        .map_err(|error| format!("{}: {error}", fixture.id))?;
    let actual_status = output.status.code().unwrap_or(-1);
    if actual_status != expected.status
        || output.stdout != expected.stdout
        || output.stderr != expected.stderr
    {
        return Err(format!(
            "{}: process result differs\nexpected: exit={}, stdout={:?}, stderr={:?}\nactual:   exit={}, stdout={:?}, stderr={:?}",
            fixture.id,
            expected.status,
            String::from_utf8_lossy(&expected.stdout),
            String::from_utf8_lossy(&expected.stderr),
            actual_status,
            String::from_utf8_lossy(&output.stdout),
            String::from_utf8_lossy(&output.stderr)
        ));
    }
    Ok(())
}

struct ProcessExpectation {
    status: i32,
    stdout: Vec<u8>,
    stderr: Vec<u8>,
}

fn parse_process_expectation(text: &str) -> Result<ProcessExpectation, String> {
    let mut status = None;
    let mut stdout = None;
    let mut stderr = None;
    for field in text.split(';') {
        let (key, value) = field
            .split_once('=')
            .ok_or_else(|| format!("invalid process expectation field {field:?}"))?;
        match key {
            "exit" => {
                status = Some(
                    value
                        .parse()
                        .map_err(|_| format!("invalid exit status {value:?}"))?,
                )
            }
            "stdout" => stdout = Some(decode_escapes(value)?),
            "stderr" => stderr = Some(decode_escapes(value)?),
            _ => return Err(format!("unknown process expectation key {key:?}")),
        }
    }
    Ok(ProcessExpectation {
        status: status.ok_or_else(|| "process expectation is missing exit".to_owned())?,
        stdout: stdout.ok_or_else(|| "process expectation is missing stdout".to_owned())?,
        stderr: stderr.ok_or_else(|| "process expectation is missing stderr".to_owned())?,
    })
}

fn decode_escapes(text: &str) -> Result<Vec<u8>, String> {
    let mut output = Vec::new();
    let mut bytes = text.bytes();
    while let Some(byte) = bytes.next() {
        if byte != b'\\' {
            output.push(byte);
            continue;
        }
        match bytes.next() {
            Some(b'n') => output.push(b'\n'),
            Some(b'r') => output.push(b'\r'),
            Some(b't') => output.push(b'\t'),
            Some(b'\\') => output.push(b'\\'),
            Some(other) => {
                return Err(format!(
                    "unknown expectation escape \\{}",
                    char::from(other)
                ));
            }
            None => return Err("trailing backslash in expectation".to_owned()),
        }
    }
    Ok(output)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parses_process_expectation() {
        let expectation =
            parse_process_expectation("exit=7;stdout=hello\\n;stderr=bad\\tvalue").unwrap();
        assert_eq!(expectation.status, 7);
        assert_eq!(expectation.stdout, b"hello\n");
        assert_eq!(expectation.stderr, b"bad\tvalue");
    }

    #[test]
    fn repository_manifest_loads_and_covers_surface() {
        let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
        let fixtures = load_manifest(&root).unwrap();
        check_coverage(&root, &fixtures).unwrap();
    }
}
