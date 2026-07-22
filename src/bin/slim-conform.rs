use std::collections::{BTreeMap, BTreeSet};
use std::env;
use std::ffi::OsString;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};
use std::time::{SystemTime, UNIX_EPOCH};

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
    let project_fixtures = load_project_manifest(&root)?;
    check_coverage(&root, &fixtures)?;
    check_project_coverage(&root, &project_fixtures)?;
    let compiler = root.join("build/toolchain/slimc");
    if !compiler.is_file() {
        let status = Command::new(root.join("bootstrap.sh"))
            .status()
            .map_err(|error| format!("cannot run portable bootstrap: {error}"))?;
        if !status.success() {
            return Err(format!("portable bootstrap failed with {status}"));
        }
    }
    let mut counts = BTreeMap::<String, usize>::new();
    for fixture in &fixtures {
        if fixture.selfhost != "parity" {
            return Err(format!(
                "{} retains forbidden semantic fallback classification {}",
                fixture.id, fixture.selfhost
            ));
        }
        run_selfhost_fixture(fixture, &compiler)?;
        *counts.entry(fixture.mode.clone()).or_default() += 1;
    }
    for fixture in &project_fixtures {
        if fixture.selfhost != "parity" {
            return Err(format!(
                "{} retains forbidden semantic fallback classification {}",
                fixture.id, fixture.selfhost
            ));
        }
        run_selfhost_project_fixture(fixture, &compiler)?;
        *counts
            .entry(format!("project-{}", fixture.mode))
            .or_default() += 1;
    }
    check_malformed_input_robustness(&compiler)?;
    let summary = counts
        .iter()
        .map(|(mode, count)| format!("{mode}={count}"))
        .collect::<Vec<_>>()
        .join(", ");
    let fixture_count = fixtures.len() + project_fixtures.len();
    let fixed_bytes = fs::metadata(root.join("bootstrap/slimc-seed.c"))
        .map_err(|error| format!("cannot inspect bootstrap seed: {error}"))?
        .len();
    println!(
        "conformance: {fixture_count} fixtures and 2000 deterministic malformed-input mutations passed through the SLIM compiler, including 100 reduce/analyze probes ({summary}); no semantic fallback; bootstrap seed is {fixed_bytes} C bytes",
    );
    Ok(())
}

fn check_malformed_input_robustness(compiler: &Path) -> Result<(), String> {
    const ALPHABET: &[u8] = b"()abcXYZ012-; \"\n";
    const CURATED: &[&[u8]] = &[
        b"",
        b"abc",
        b"()",
        b"(module x)",
        b"(module x (fn))",
        b"(module x (fn main))",
        b"(module x (fn main ((args (Vec Bytes))) I64 (effects) (call)))",
        b"(module x (fn main ((args (Vec Bytes))) I64 (effects) (let)))",
        b"(module x))",
        b"(module x (fn main ((args (Vec Bytes))) I64 (effects) \"abc))",
    ];
    let directory = temporary_directory("malformed-inputs")?;
    let path = directory.join("mutated.slim");
    let result = (|| {
        for (case, source) in CURATED.iter().enumerate() {
            fs::write(&path, source).map_err(|error| error.to_string())?;
            for command in [Some("check"), Some("reduce"), Some("analyze"), None] {
                let mut process = Command::new(compiler);
                if let Some(command) = command {
                    process.arg(command);
                }
                let output = process
                    .arg(&path)
                    .output()
                    .map_err(|error| format!("cannot run malformed-input case {case}: {error}"))?;
                if output.status.code() != Some(1)
                    || !output.stderr.is_empty()
                    || !output.stdout.starts_with(b"E")
                {
                    return Err(format!(
                        "malformed-input case {case} escaped deterministic rejection: {} / {:?} / {:?}",
                        output.status, output.stdout, output.stderr
                    ));
                }
            }
        }

        let mut state = 0x5eed_u64;
        for case in 0..2_000 {
            let length = case % 257;
            let mut source = Vec::with_capacity(length);
            for _ in 0..length {
                state = state
                    .wrapping_mul(6_364_136_223_846_793_005)
                    .wrapping_add(1);
                source.push(ALPHABET[(state as usize) % ALPHABET.len()]);
            }
            fs::write(&path, &source).map_err(|error| error.to_string())?;
            let commands: &[&str] = if case % 20 == 0 {
                &["check", "reduce", "analyze"]
            } else {
                &["check"]
            };
            for command in commands {
                let output = Command::new(compiler)
                    .arg(command)
                    .arg(&path)
                    .output()
                    .map_err(|error| {
                        format!(
                            "cannot run malformed-input mutation {case} with {command}: {error}"
                        )
                    })?;
                let status = output.status.code();
                if !matches!(status, Some(0 | 1)) || !output.stderr.is_empty() {
                    return Err(format!(
                        "malformed-input mutation {case} trapped in {command}: {} / {:?} / {:?} / {:?}",
                        output.status, source, output.stdout, output.stderr
                    ));
                }
                if status == Some(1) && !output.stdout.starts_with(b"E") {
                    return Err(format!(
                        "malformed-input mutation {case} lacks a stable {command} diagnostic identity: {:?}",
                        output.stdout
                    ));
                }
            }
        }
        Ok(())
    })();
    let _ = fs::remove_dir_all(directory);
    result
}

fn load_manifest(root: &Path) -> Result<Vec<Fixture>, String> {
    load_fixture_manifest(
        root,
        "conformance/manifest.tsv",
        &[
            "check-pass",
            "check-fail",
            "run",
            "trap",
            "allocation-fail",
            "format",
            "emit",
        ],
        false,
    )
}

fn load_project_manifest(root: &Path) -> Result<Vec<Fixture>, String> {
    load_fixture_manifest(
        root,
        "conformance/projects/manifest.tsv",
        &[
            "check-pass",
            "check-fail",
            "run",
            "emit",
            "format",
            "relocate",
            "cache-corruption",
            "jobs",
            "incremental",
            "interfaces",
        ],
        true,
    )
}

fn load_fixture_manifest(
    root: &Path,
    relative_path: &str,
    allowed_modes: &[&str],
    _projects: bool,
) -> Result<Vec<Fixture>, String> {
    let path = root.join(relative_path);
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
        if !allowed_modes.contains(&columns[1]) {
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

fn check_project_coverage(root: &Path, fixtures: &[Fixture]) -> Result<(), String> {
    let ledger_path = root.join("design/project-semantics.tsv");
    let ledger = fs::read_to_string(&ledger_path)
        .map_err(|error| format!("cannot read {}: {error}", ledger_path.display()))?;
    let required: BTreeSet<_> = ledger
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .filter_map(|line| {
            let columns = line.split('\t').collect::<Vec<_>>();
            (columns.len() == 4).then(|| format!("{}:{}", columns[0], columns[1]))
        })
        .collect();
    let covered: BTreeSet<_> = fixtures
        .iter()
        .flat_map(|fixture| fixture.coverage.iter().cloned())
        .collect();
    let missing: Vec<_> = required.difference(&covered).cloned().collect();
    if missing.is_empty() {
        Ok(())
    } else {
        Err(format!(
            "project conformance coverage is missing: {}",
            missing.join(", ")
        ))
    }
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
            "ownership:inout-no-escape",
            "recovery:malformed",
            "recovery:parser",
            "runtime:allocation",
            "runtime:allocation-failure",
            "runtime:bounds-trap",
            "runtime:overflow-trap",
            "runtime:region-destruction",
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

fn temporary_directory(role: &str) -> Result<PathBuf, String> {
    let nonce = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map_err(|error| format!("system clock error: {error}"))?
        .as_nanos();
    let path = env::temp_dir().join(format!("slim-{role}-{}-{nonce}", std::process::id()));
    fs::create_dir(&path).map_err(|error| format!("cannot create {}: {error}", path.display()))?;
    Ok(path)
}

fn copy_tree(source: &Path, destination: &Path) -> Result<(), String> {
    fs::create_dir_all(destination)
        .map_err(|error| format!("cannot create {}: {error}", destination.display()))?;
    for entry in fs::read_dir(source)
        .map_err(|error| format!("cannot read {}: {error}", source.display()))?
    {
        let entry = entry.map_err(|error| error.to_string())?;
        if entry.file_name() == ".slim-cache" {
            continue;
        }
        let target = destination.join(entry.file_name());
        if entry.path().is_dir() {
            copy_tree(&entry.path(), &target)?;
        } else {
            fs::copy(entry.path(), &target).map_err(|error| {
                format!(
                    "cannot copy {} to {}: {error}",
                    entry.path().display(),
                    target.display()
                )
            })?;
        }
    }
    Ok(())
}

fn run_selfhost_fixture(fixture: &Fixture, compiler: &Path) -> Result<(), String> {
    if fixture.mode == "check-fail" {
        let run = || {
            Command::new(compiler)
                .arg("check")
                .arg(&fixture.path)
                .output()
                .map_err(|error| format!("{}: cannot run self-hosted checker: {error}", fixture.id))
        };
        let first = run()?;
        let second = run()?;
        if first.status.code() != Some(1) || second.status.code() != Some(1) {
            return Err(format!(
                "{}: self-hosted checker did not reject deterministically: {} / {}",
                fixture.id, first.status, second.status
            ));
        }
        if !first.stderr.is_empty() || !second.stderr.is_empty() {
            return Err(format!(
                "{}: self-hosted checker wrote unexpected unstructured diagnostics",
                fixture.id
            ));
        }
        let identity = |bytes: &[u8]| {
            String::from_utf8_lossy(bytes)
                .lines()
                .collect::<Vec<_>>()
                .join(",")
        };
        let actual = identity(&first.stdout);
        if actual != fixture.expectation || second.stdout != first.stdout {
            return Err(format!(
                "{}: self-hosted diagnostics differ\nexpected: {}\nactual:   {actual}",
                fixture.id, fixture.expectation
            ));
        }
        return Ok(());
    }
    if fixture.mode == "format" {
        let first = Command::new(compiler)
            .arg("fmt")
            .arg(&fixture.path)
            .output()
            .map_err(|error| format!("{}: cannot run SLIM formatter: {error}", fixture.id))?;
        if !first.status.success() || !first.stderr.is_empty() {
            return Err(format!(
                "{}: SLIM formatter failed with {}\n{}",
                fixture.id,
                first.status,
                String::from_utf8_lossy(&first.stderr)
            ));
        }
        let directory = temporary_directory("format-fixed-point")?;
        let formatted = directory.join("formatted.slim");
        fs::write(&formatted, &first.stdout).map_err(|error| error.to_string())?;
        let second = Command::new(compiler)
            .arg("fmt")
            .arg(&formatted)
            .output()
            .map_err(|error| format!("{}: cannot rerun SLIM formatter: {error}", fixture.id))?;
        let checked = Command::new(compiler)
            .arg("check")
            .arg(&formatted)
            .output()
            .map_err(|error| format!("{}: cannot check formatted source: {error}", fixture.id))?;
        let result = if !second.status.success()
            || !checked.status.success()
            || !second.stderr.is_empty()
            || !checked.stdout.is_empty()
            || !checked.stderr.is_empty()
            || second.stdout != first.stdout
        {
            Err(format!(
                "{}: SLIM canonical format is not deterministic, idempotent, and accepted",
                fixture.id
            ))
        } else {
            Ok(())
        };
        let _ = fs::remove_dir_all(directory);
        if !first.stderr.is_empty() {
            return Err(format!(
                "{}: SLIM formatter wrote unexpected diagnostics",
                fixture.id
            ));
        }
        return result;
    }
    let first = run_compiler(compiler, &fixture.path)
        .map_err(|error| format!("{}: SLIM compilation failed: {error}", fixture.id))?;
    let second = run_compiler(compiler, &fixture.path)
        .map_err(|error| format!("{}: repeated SLIM compilation failed: {error}", fixture.id))?;
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
        "run" | "trap" | "allocation-fail" => check_process_expectation(fixture, &output),
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

fn run_selfhost_project_fixture(fixture: &Fixture, compiler: &Path) -> Result<(), String> {
    let run_check = || {
        Command::new(compiler)
            .arg("check")
            .arg(&fixture.path)
            .output()
            .map_err(|error| {
                format!(
                    "{}: cannot run self-hosted project check: {error}",
                    fixture.id
                )
            })
    };
    match fixture.mode.as_str() {
        "check-pass" => {
            let first = run_check()?;
            let second = run_check()?;
            if !first.status.success()
                || !second.status.success()
                || !first.stdout.is_empty()
                || !first.stderr.is_empty()
                || second.stdout != first.stdout
                || second.stderr != first.stderr
            {
                return Err(format!(
                    "{}: self-hosted project check was not clean and deterministic",
                    fixture.id
                ));
            }
            Ok(())
        }
        "check-fail" => {
            let first = run_check()?;
            let second = run_check()?;
            if first.status.code() != Some(1)
                || second.status.code() != Some(1)
                || !first.stderr.is_empty()
                || second.stdout != first.stdout
            {
                return Err(format!(
                    "{}: self-hosted project rejection was not deterministic",
                    fixture.id
                ));
            }
            let actual = String::from_utf8_lossy(&first.stdout)
                .lines()
                .collect::<Vec<_>>()
                .join(",");
            if actual != fixture.expectation {
                return Err(format!(
                    "{}: self-hosted project diagnostics differ\nexpected: {}\nactual:   {actual}",
                    fixture.id, fixture.expectation
                ));
            }
            Ok(())
        }
        "format" => {
            let first = Command::new(compiler)
                .arg("fmt")
                .arg(&fixture.path)
                .output()
                .map_err(|error| {
                    format!("{}: cannot run SLIM project formatter: {error}", fixture.id)
                })?;
            let directory = temporary_directory("project-format-fixed-point")?;
            let formatted = directory.join("slim.project");
            fs::write(&formatted, &first.stdout).map_err(|error| error.to_string())?;
            let second = Command::new(compiler)
                .arg("fmt")
                .arg(&formatted)
                .output()
                .map_err(|error| {
                    format!(
                        "{}: cannot rerun SLIM project formatter: {error}",
                        fixture.id
                    )
                })?;
            let result = if !first.status.success()
                || !second.status.success()
                || !first.stderr.is_empty()
                || !second.stderr.is_empty()
                || second.stdout != first.stdout
            {
                return Err(format!(
                    "{}: SLIM project format is not deterministic and idempotent",
                    fixture.id
                ));
            } else {
                Ok(())
            };
            let _ = fs::remove_dir_all(directory);
            result
        }
        "interfaces" => {
            let run = || {
                Command::new(compiler)
                    .arg("interfaces")
                    .arg(&fixture.path)
                    .output()
                    .map_err(|error| {
                        format!(
                            "{}: cannot run self-hosted interface emitter: {error}",
                            fixture.id
                        )
                    })
            };
            let first = run()?;
            let second = run()?;
            if !first.status.success()
                || !second.status.success()
                || !first.stderr.is_empty()
                || second.stdout != first.stdout
            {
                return Err(format!(
                    "{}: SLIM public interfaces failed or are nondeterministic",
                    fixture.id
                ));
            }
            Ok(())
        }
        "run" | "emit" => {
            let first = run_compiler(compiler, &fixture.path).map_err(|error| {
                format!("{}: self-host project emit failed: {error}", fixture.id)
            })?;
            let second = run_compiler(compiler, &fixture.path).map_err(|error| {
                format!(
                    "{}: repeated self-host project emit failed: {error}",
                    fixture.id
                )
            })?;
            if first != second {
                return Err(format!(
                    "{}: self-host project C is not byte deterministic",
                    fixture.id
                ));
            }
            let generated = std::str::from_utf8(&first)
                .map_err(|error| format!("{}: project emitted non-UTF-8 C: {error}", fixture.id))?;
            let output = compile_and_run(fixture, generated)?;
            if fixture.mode == "run" {
                check_process_expectation(fixture, &output)
            } else if output.status.success() {
                Ok(())
            } else {
                Err(format!(
                    "{}: self-host project C failed at runtime with {}",
                    fixture.id, output.status
                ))
            }
        }
        "relocate" => {
            let directory = temporary_directory("selfhost-project-relocate")?;
            copy_tree(
                fixture
                    .path
                    .parent()
                    .ok_or_else(|| format!("{}: manifest has no parent", fixture.id))?,
                &directory,
            )?;
            let relocated = directory.join(
                fixture
                    .path
                    .file_name()
                    .ok_or_else(|| format!("{}: manifest has no file name", fixture.id))?,
            );
            let original = run_compiler(compiler, &fixture.path).map_err(|error| {
                format!("{}: original self-host emit failed: {error}", fixture.id)
            });
            let moved = run_compiler(compiler, &relocated).map_err(|error| {
                format!("{}: relocated self-host emit failed: {error}", fixture.id)
            });
            let result = original.and_then(|original| {
                let moved = moved?;
                if original == moved {
                    Ok(())
                } else {
                    Err(format!(
                        "{}: relocation changed self-hosted project C",
                        fixture.id
                    ))
                }
            });
            let _ = fs::remove_dir_all(directory);
            result
        }
        "jobs" => {
            let mut outputs = Vec::new();
            for jobs in [1, 2, 4, usize::MAX] {
                let output = Command::new(compiler)
                    .arg(&fixture.path)
                    .arg("--jobs")
                    .arg(jobs.to_string())
                    .output()
                    .map_err(|error| {
                        format!(
                            "{}: cannot run self-host with {jobs} jobs: {error}",
                            fixture.id
                        )
                    })?;
                if !output.status.success() || !output.stderr.is_empty() {
                    return Err(format!(
                        "{}: self-host project failed with {jobs} jobs",
                        fixture.id
                    ));
                }
                outputs.push(output.stdout);
            }
            if outputs.windows(2).any(|pair| pair[0] != pair[1]) {
                return Err(format!(
                    "{}: requested job count changed self-hosted project C",
                    fixture.id
                ));
            }
            let invalid = fixture
                .path
                .parent()
                .and_then(Path::parent)
                .ok_or_else(|| format!("{}: cannot locate project rules", fixture.id))?
                .join("rules/unknown-import.project");
            let mut diagnostics = Vec::new();
            for jobs in [1, 2, 4, usize::MAX] {
                let output = Command::new(compiler)
                    .arg(&invalid)
                    .arg("--jobs")
                    .arg(jobs.to_string())
                    .output()
                    .map_err(|error| {
                        format!(
                            "{}: cannot run invalid self-host project with {jobs} jobs: {error}",
                            fixture.id
                        )
                    })?;
                diagnostics.push((output.status.code(), output.stdout, output.stderr));
            }
            if diagnostics.windows(2).any(|pair| pair[0] != pair[1]) {
                return Err(format!(
                    "{}: requested job count changed self-hosted diagnostics",
                    fixture.id
                ));
            }
            Ok(())
        }
        "incremental" => run_selfhost_incremental_fixture(fixture, compiler),
        "cache-corruption" => run_selfhost_cache_fixture(fixture, compiler),
        _ => unreachable!("project manifest mode validated"),
    }
}

fn run_selfhost_cache_fixture(fixture: &Fixture, compiler: &Path) -> Result<(), String> {
    let directory = temporary_directory("selfhost-project-cache")?;
    copy_tree(
        fixture
            .path
            .parent()
            .ok_or_else(|| format!("{}: manifest has no parent", fixture.id))?,
        &directory,
    )?;
    let manifest = directory.join(
        fixture
            .path
            .file_name()
            .ok_or_else(|| format!("{}: manifest has no file name", fixture.id))?,
    );
    let cache = directory.join("cache.bin");
    let missing = directory.join("missing-cache.bin");
    let clean = run_compiler(compiler, &manifest)
        .map_err(|error| format!("{}: clean cache oracle failed: {error}", fixture.id))?;

    let miss = run_selfhost_cache(compiler, &manifest, &missing, fixture)?;
    if miss.first() != Some(&b'M') || miss.len() <= 1 {
        return Err(format!(
            "{}: cold self-host cache did not report a bounded miss",
            fixture.id
        ));
    }
    let entry = miss[1..].to_vec();
    fs::write(&cache, &entry).map_err(|error| error.to_string())?;
    require_selfhost_cache_hit(fixture, compiler, &manifest, &cache, &clean)?;

    let key_length = read_cache_u64(&entry, 11)?;
    let artifact_length = read_cache_u64(&entry, 19)?;
    let artifact_start = 27_usize
        .checked_add(key_length)
        .ok_or_else(|| format!("{}: cache key length overflow", fixture.id))?;
    let checksum_start = artifact_start
        .checked_add(artifact_length)
        .ok_or_else(|| format!("{}: cache artifact length overflow", fixture.id))?;
    if checksum_start.checked_add(8) != Some(entry.len()) {
        return Err(format!(
            "{}: self-host emitted a noncanonical cache frame",
            fixture.id
        ));
    }

    let mut truncations = vec![0, 1, 9, 10, 11, 18, 19, 26, 27, entry.len() / 2];
    truncations.extend([
        entry.len().saturating_sub(9),
        entry.len().saturating_sub(8),
        entry.len().saturating_sub(1),
    ]);
    truncations.sort_unstable();
    truncations.dedup();
    for length in truncations {
        require_selfhost_cache_rebuild(
            fixture,
            compiler,
            &manifest,
            &cache,
            &entry[..length],
            &entry,
        )?;
    }

    let mut mutation_offsets = vec![0, 9, 10, 11, 18, 19, 26, 27];
    mutation_offsets.extend([
        artifact_start.saturating_sub(1),
        artifact_start,
        checksum_start.saturating_sub(1),
        checksum_start,
        entry.len() - 1,
    ]);
    mutation_offsets.sort_unstable();
    mutation_offsets.dedup();
    for offset in mutation_offsets {
        let mut corrupted = entry.clone();
        corrupted[offset] ^= 1;
        require_selfhost_cache_rebuild(fixture, compiler, &manifest, &cache, &corrupted, &entry)?;
    }

    let module = directory.join("math.slim");
    let before = fs::read_to_string(&module).map_err(|error| error.to_string())?;
    let after = before.replacen("value 1", "value 2", 1);
    if before == after {
        return Err(format!(
            "{}: cache fixture lacks source identity marker",
            fixture.id
        ));
    }
    fs::write(&module, after).map_err(|error| error.to_string())?;
    fs::write(&cache, &entry).map_err(|error| error.to_string())?;
    let stale = run_selfhost_cache(compiler, &manifest, &cache, fixture)?;
    if stale.first() != Some(&b'M') || stale[1..] == entry {
        return Err(format!(
            "{}: stale self-host cache identity was accepted",
            fixture.id
        ));
    }
    let updated_entry = &stale[1..];
    fs::write(&cache, updated_entry).map_err(|error| error.to_string())?;
    let updated_clean = run_compiler(compiler, &manifest)
        .map_err(|error| format!("{}: updated cache oracle failed: {error}", fixture.id))?;
    let result = require_selfhost_cache_hit(fixture, compiler, &manifest, &cache, &updated_clean);
    let _ = fs::remove_dir_all(directory);
    result
}

fn run_selfhost_cache(
    compiler: &Path,
    manifest: &Path,
    cache: &Path,
    fixture: &Fixture,
) -> Result<Vec<u8>, String> {
    let output = Command::new(compiler)
        .arg("cache")
        .arg(manifest)
        .arg(cache)
        .output()
        .map_err(|error| format!("{}: cannot run self-hosted cache: {error}", fixture.id))?;
    if output.status.code() == Some(0) && output.stderr.is_empty() {
        return Ok(output.stdout);
    }
    Err(format!(
        "{}: self-hosted cache failed: {} / {:?}",
        fixture.id, output.status, output.stderr
    ))
}

fn require_selfhost_cache_hit(
    fixture: &Fixture,
    compiler: &Path,
    manifest: &Path,
    cache: &Path,
    clean: &[u8],
) -> Result<(), String> {
    let hit = run_selfhost_cache(compiler, manifest, cache, fixture)?;
    if hit.first() == Some(&b'H') && hit[1..] == *clean {
        return Ok(());
    }
    Err(format!(
        "{}: warm self-host cache did not reproduce clean C",
        fixture.id
    ))
}

fn require_selfhost_cache_rebuild(
    fixture: &Fixture,
    compiler: &Path,
    manifest: &Path,
    cache: &Path,
    corrupted: &[u8],
    canonical: &[u8],
) -> Result<(), String> {
    fs::write(cache, corrupted).map_err(|error| error.to_string())?;
    let rebuilt = run_selfhost_cache(compiler, manifest, cache, fixture)?;
    if rebuilt.first() == Some(&b'M') && rebuilt[1..] == *canonical {
        return Ok(());
    }
    Err(format!(
        "{}: corrupted self-host cache did not rebuild deterministically",
        fixture.id
    ))
}

fn read_cache_u64(bytes: &[u8], offset: usize) -> Result<usize, String> {
    let field: [u8; 8] = bytes
        .get(offset..offset + 8)
        .ok_or_else(|| "self-host cache integer is truncated".to_owned())?
        .try_into()
        .map_err(|_| "self-host cache integer has the wrong width".to_owned())?;
    usize::try_from(u64::from_be_bytes(field))
        .map_err(|_| "self-host cache integer exceeds host size".to_owned())
}

fn run_selfhost_incremental_fixture(fixture: &Fixture, compiler: &Path) -> Result<(), String> {
    let directory = temporary_directory("selfhost-project-incremental")?;
    let initial_directory = directory.join("initial");
    let updated_directory = directory.join("updated");
    let source_directory = fixture
        .path
        .parent()
        .ok_or_else(|| format!("{}: manifest has no parent", fixture.id))?;
    copy_tree(source_directory, &initial_directory)?;
    copy_tree(source_directory, &updated_directory)?;
    let manifest_name = fixture
        .path
        .file_name()
        .ok_or_else(|| format!("{}: manifest has no file name", fixture.id))?;
    let initial_manifest = initial_directory.join(manifest_name);
    let updated_manifest = updated_directory.join(manifest_name);
    require_selfhost_session_work(
        fixture,
        compiler,
        &initial_manifest,
        &initial_manifest,
        b"0 0 0 0\n",
        "no-change update",
    )?;
    let updated_module = updated_directory.join("math.slim");
    let before = fs::read_to_string(&updated_module).map_err(|error| error.to_string())?;
    let after = before.replacen("value 1", "value 2", 1);
    if before == after {
        return Err(format!(
            "{}: incremental fixture lacks `value 1` edit marker",
            fixture.id
        ));
    }
    fs::write(&updated_module, after).map_err(|error| error.to_string())?;

    require_selfhost_session_work(
        fixture,
        compiler,
        &initial_manifest,
        &updated_manifest,
        b"1 1 1 1\n",
        "private implementation edit",
    )?;

    let invalid_directory = directory.join("invalid");
    copy_tree(source_directory, &invalid_directory)?;
    let invalid_manifest = invalid_directory.join(manifest_name);
    let invalid_module = invalid_directory.join("math.slim");
    let before = fs::read_to_string(&invalid_module).map_err(|error| error.to_string())?;
    let after = before.replacen("(fn answer ", "(fn broken ", 1);
    if before == after {
        return Err(format!(
            "{}: incremental fixture lacks recovery edit marker",
            fixture.id
        ));
    }
    fs::write(&invalid_module, after).map_err(|error| error.to_string())?;
    require_selfhost_recovery(
        fixture,
        compiler,
        &initial_manifest,
        &invalid_manifest,
        &updated_manifest,
        b"E0414@math@116:122\n1 1 1 1\n",
    )?;

    let initial_app = initial_directory.join("app.slim");
    let updated_app = updated_directory.join("app.slim");
    for app in [&initial_app, &updated_app] {
        let before = fs::read_to_string(app).map_err(|error| error.to_string())?;
        let after = before.replacen("(effects io)", "(effects io partial)", 1);
        if before == after {
            return Err(format!(
                "{}: incremental fixture lacks app effect marker",
                fixture.id
            ));
        }
        fs::write(app, after).map_err(|error| error.to_string())?;
    }
    let before = fs::read_to_string(&updated_module).map_err(|error| error.to_string())?;
    let after = before.replacen("(effects)", "(effects partial)", 1);
    if before == after {
        return Err(format!(
            "{}: incremental fixture lacks interface effect marker",
            fixture.id
        ));
    }
    fs::write(&updated_module, after).map_err(|error| error.to_string())?;
    require_selfhost_session_work(
        fixture,
        compiler,
        &initial_manifest,
        &updated_manifest,
        b"1 1 2 2\n",
        "exported interface edit",
    )?;

    let result = {
        let first = run_compiler(compiler, &updated_manifest)
            .map_err(|error| format!("{}: updated self-host emit failed: {error}", fixture.id))?;
        let second = run_compiler(compiler, &updated_manifest).map_err(|error| {
            format!(
                "{}: repeated updated self-host emit failed: {error}",
                fixture.id
            )
        })?;
        if first == second {
            Ok(())
        } else {
            Err(format!(
                "{}: incremental target differs from its deterministic clean oracle",
                fixture.id
            ))
        }
    };
    let _ = fs::remove_dir_all(directory);
    result
}

fn require_selfhost_recovery(
    fixture: &Fixture,
    compiler: &Path,
    initial: &Path,
    invalid: &Path,
    recovered: &Path,
    expected: &[u8],
) -> Result<(), String> {
    let output = Command::new(compiler)
        .arg("session")
        .arg(initial)
        .arg(invalid)
        .arg(recovered)
        .output()
        .map_err(|error| {
            format!(
                "{}: cannot run self-hosted transactional session: {error}",
                fixture.id
            )
        })?;
    if output.status.code() == Some(0) && output.stderr.is_empty() && output.stdout == expected {
        return Ok(());
    }
    Err(format!(
        "{}: self-hosted session replaced its last-good state: {} / {:?} / {:?}",
        fixture.id, output.status, output.stdout, output.stderr
    ))
}

fn require_selfhost_session_work(
    fixture: &Fixture,
    compiler: &Path,
    initial: &Path,
    updated: &Path,
    expected: &[u8],
    scenario: &str,
) -> Result<(), String> {
    let output = Command::new(compiler)
        .arg("session")
        .arg(initial)
        .arg(updated)
        .output()
        .map_err(|error| format!("{}: cannot run self-hosted session: {error}", fixture.id))?;
    if output.status.code() == Some(0) && output.stderr.is_empty() && output.stdout == expected {
        return Ok(());
    }
    Err(format!(
        "{}: self-hosted session did not report exact {scenario} work: {} / {:?} / {:?}",
        fixture.id, output.status, output.stdout, output.stderr
    ))
}

fn run_compiler(compiler: &Path, source: &Path) -> Result<Vec<u8>, String> {
    let output = Command::new(compiler)
        .arg(source)
        .output()
        .map_err(|error| format!("cannot run {}: {error}", compiler.display()))?;
    if !output.status.success() {
        let tail_start = output.stdout.len().saturating_sub(1_000);
        return Err(format!(
            "{} failed with {}\n{}\nlast generated bytes:\n{}",
            compiler.display(),
            output.status,
            String::from_utf8_lossy(&output.stderr),
            String::from_utf8_lossy(&output.stdout[tail_start..])
        ));
    }
    if !output.stderr.is_empty() {
        return Err(format!(
            "{} wrote unexpected diagnostics:\n{}",
            compiler.display(),
            String::from_utf8_lossy(&output.stderr)
        ));
    }
    Ok(output.stdout)
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
    let mut run = Command::new(&executable);
    run.arg(&fixture.path);
    if fixture.mode == "allocation-fail" {
        run.env("SLIM_ALLOC_FAIL_AT", "2");
    }
    run.output()
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
        let projects = load_project_manifest(&root).unwrap();
        check_project_coverage(&root, &projects).unwrap();
    }
}
