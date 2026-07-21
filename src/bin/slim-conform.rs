use std::collections::{BTreeMap, BTreeSet};
use std::env;
use std::ffi::OsString;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};
use std::time::{SystemTime, UNIX_EPOCH};

use slim::bootstrap;
use slim::compiler;
use slim::project;
use slim::project::session::ProjectSession;
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
    let project_fixtures = load_project_manifest(&root)?;
    check_coverage(&root, &fixtures)?;
    check_project_coverage(&root, &project_fixtures)?;
    let mut counts = BTreeMap::<String, usize>::new();
    for fixture in &fixtures {
        run_stage0_fixture(fixture)?;
        *counts.entry(fixture.mode.clone()).or_default() += 1;
    }
    for fixture in &project_fixtures {
        run_project_fixture(fixture)?;
        *counts
            .entry(format!("project-{}", fixture.mode))
            .or_default() += 1;
    }
    let summary = counts
        .iter()
        .map(|(mode, count)| format!("{mode}={count}"))
        .collect::<Vec<_>>()
        .join(", ");
    let parity_count = fixtures
        .iter()
        .chain(project_fixtures.iter())
        .filter(|fixture| fixture.selfhost == "parity")
        .count();
    println!(
        "conformance: {} stage-0 fixtures passed ({summary}); {parity_count} marked for self-host parity",
        fixtures.len() + project_fixtures.len(),
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
        for fixture in &project_fixtures {
            if fixture.selfhost == "parity" {
                run_selfhost_project_fixture(fixture, &report.compiler)?;
                parity_run += 1;
            } else {
                let reason = fixture
                    .selfhost
                    .strip_prefix("stage0-only(")
                    .and_then(|value| value.strip_suffix(')'))
                    .expect("project manifest classification was validated");
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
            fixtures.len() + project_fixtures.len() - parity_run,
            report.fixed_c_bytes,
        );
    }
    Ok(())
}

fn load_manifest(root: &Path) -> Result<Vec<Fixture>, String> {
    load_fixture_manifest(
        root,
        "conformance/manifest.tsv",
        &["check-pass", "check-fail", "run", "trap", "format", "emit"],
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

fn run_project_fixture(fixture: &Fixture) -> Result<(), String> {
    let source = read_source(&fixture.path)?;
    match fixture.mode.as_str() {
        "check-pass" => require_project_success(fixture, &project::compile(source)),
        "check-fail" => {
            let compilation = project::compile(source);
            if compilation.succeeded() {
                return Err(format!(
                    "{}: expected project rejection, but check passed",
                    fixture.id
                ));
            }
            let actual = project_diagnostic_identity(&compilation.diagnostics);
            if fixture.expectation == "TODO" {
                return Err(format!(
                    "{}: replace TODO diagnostic expectation with {actual}",
                    fixture.id
                ));
            }
            if actual != fixture.expectation {
                return Err(format!(
                    "{}: project diagnostics differ\nexpected: {}\nactual:   {actual}",
                    fixture.id, fixture.expectation
                ));
            }
            Ok(())
        }
        "run" => {
            let compilation = project::compile(source);
            require_project_success(fixture, &compilation)?;
            let output = compile_and_run(
                fixture,
                compilation
                    .emit_c()
                    .ok_or_else(|| format!("{}: project emitted no C", fixture.id))?,
            )?;
            check_process_expectation(fixture, &output)
        }
        "emit" => {
            let first = project::compile_with_jobs(source.clone(), 1);
            let second = project::compile_with_jobs(source, 1);
            require_project_success(fixture, &first)?;
            require_project_success(fixture, &second)?;
            if first.emit_c() != second.emit_c() || first.interfaces != second.interfaces {
                return Err(format!(
                    "{}: repeated project artifacts are not byte deterministic",
                    fixture.id
                ));
            }
            Ok(())
        }
        "interfaces" => {
            let first = project::compile_with_jobs(source.clone(), 1);
            let second = project::compile_with_jobs(source, 1);
            require_project_success(fixture, &first)?;
            require_project_success(fixture, &second)?;
            if first.interfaces != second.interfaces {
                return Err(format!(
                    "{}: repeated project interfaces are not byte deterministic",
                    fixture.id
                ));
            }
            Ok(())
        }
        "format" => {
            let first = compiler::format_source(&source)
                .map_err(|diagnostics| format!("{}: {diagnostics:#?}", fixture.id))?;
            let formatted = Source::new(&fixture.path, first.clone());
            let second = compiler::format_source(&formatted)
                .map_err(|diagnostics| format!("{}: {diagnostics:#?}", fixture.id))?;
            if first != second || project::manifest::parse(&formatted).is_err() {
                return Err(format!(
                    "{}: project manifest formatting is not canonical and valid",
                    fixture.id
                ));
            }
            Ok(())
        }
        "relocate" => run_relocation_fixture(fixture),
        "cache-corruption" => run_cache_corruption_fixture(fixture),
        "jobs" => run_jobs_fixture(fixture),
        "incremental" => run_incremental_fixture(fixture),
        _ => unreachable!("project manifest mode validated"),
    }
}

fn run_relocation_fixture(fixture: &Fixture) -> Result<(), String> {
    let directory = temporary_directory("project-relocate")?;
    copy_tree(
        fixture
            .path
            .parent()
            .ok_or_else(|| format!("{}: manifest has no parent", fixture.id))?,
        &directory,
    )?;
    let relocated_path = directory.join(
        fixture
            .path
            .file_name()
            .ok_or_else(|| format!("{}: manifest has no file name", fixture.id))?,
    );
    let original = project::compile(read_source(&fixture.path)?);
    let relocated = project::compile(read_source(&relocated_path)?);
    let result = require_project_success(fixture, &original).and_then(|()| {
        require_project_success(fixture, &relocated)?;
        if original.emit_c() != relocated.emit_c() || original.interfaces != relocated.interfaces {
            return Err(format!(
                "{}: relocation changed project artifacts",
                fixture.id
            ));
        }
        Ok(())
    });
    let _ = fs::remove_dir_all(directory);
    result
}

fn run_cache_corruption_fixture(fixture: &Fixture) -> Result<(), String> {
    let directory = temporary_directory("project-cache")?;
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
    let first = project::compile_cached_with_jobs(read_source(&manifest)?, 1);
    require_project_success(fixture, &first)?;
    let warm = project::compile_cached_with_jobs(read_source(&manifest)?, 1);
    require_project_success(fixture, &warm)?;
    if first.emit_c() != warm.emit_c() || first.interfaces != warm.interfaces {
        return Err(format!("{}: warm cache changed artifacts", fixture.id));
    }
    let cache_directory = directory.join(".slim-cache/v1");
    for entry in fs::read_dir(&cache_directory)
        .map_err(|error| format!("{}: cannot read cache: {error}", fixture.id))?
    {
        let path = entry.map_err(|error| error.to_string())?.path();
        if path.is_file() {
            let mut bytes = fs::read(&path).map_err(|error| error.to_string())?;
            bytes.truncate(bytes.len() / 2);
            fs::write(&path, bytes).map_err(|error| error.to_string())?;
        }
    }
    let recovered = project::compile_cached_with_jobs(read_source(&manifest)?, 1);
    let result = require_project_success(fixture, &recovered).and_then(|()| {
        if first.emit_c() != recovered.emit_c() || first.interfaces != recovered.interfaces {
            return Err(format!(
                "{}: corruption recovery changed artifacts",
                fixture.id
            ));
        }
        Ok(())
    });
    let _ = fs::remove_dir_all(directory);
    result
}

fn run_jobs_fixture(fixture: &Fixture) -> Result<(), String> {
    let mut outputs = Vec::new();
    for jobs in [1, 2, 4, usize::MAX] {
        let compilation = project::compile_with_jobs(read_source(&fixture.path)?, jobs);
        require_project_success(fixture, &compilation)?;
        outputs.push((
            compilation.emit_c().map(str::to_owned),
            compilation.interfaces,
        ));
    }
    if outputs.windows(2).any(|pair| pair[0] != pair[1]) {
        return Err(format!(
            "{}: worker count changed project artifacts",
            fixture.id
        ));
    }
    Ok(())
}

fn run_incremental_fixture(fixture: &Fixture) -> Result<(), String> {
    let directory = temporary_directory("project-incremental")?;
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
    let mut session = ProjectSession::new();
    let initial = session.update(read_source(&manifest)?);
    if !initial.succeeded() {
        return Err(format!("{}: initial incremental check failed", fixture.id));
    }
    let module = directory.join("math.slim");
    let before = fs::read_to_string(&module).map_err(|error| error.to_string())?;
    let after = before.replacen("value 1", "value 2", 1);
    if before == after {
        return Err(format!(
            "{}: incremental fixture lacks `value 1` edit marker",
            fixture.id
        ));
    }
    fs::write(&module, after).map_err(|error| error.to_string())?;
    let updated = session.update(read_source(&manifest)?);
    let clean = project::compile(read_source(&manifest)?);
    let result = if !updated.succeeded() || !clean.succeeded() {
        Err(format!("{}: incremental edit failed", fixture.id))
    } else if updated.stats.declarations_parsed != 1
        || updated.stats.declarations_lowered != 1
        || updated.stats.declarations_checked != 1
        || updated.stats.declarations_generated != 1
        || updated.emit_c() != clean.emit_c()
    {
        Err(format!(
            "{}: private edit did not reuse the exact expected work: {:?}",
            fixture.id, updated.stats
        ))
    } else {
        Ok(())
    };
    let _ = fs::remove_dir_all(directory);
    result
}

fn read_source(path: &Path) -> Result<Source, String> {
    let text = fs::read_to_string(path)
        .map_err(|error| format!("cannot read {}: {error}", path.display()))?;
    Ok(Source::new(path, text))
}

fn require_project_success(
    fixture: &Fixture,
    compilation: &project::ProjectCompilation,
) -> Result<(), String> {
    if compilation.succeeded() {
        return Ok(());
    }
    Err(format!(
        "{}: unexpected project diagnostics\n{}",
        fixture.id,
        project_diagnostic_identity(&compilation.diagnostics)
    ))
}

fn project_diagnostic_identity(diagnostics: &[project::ProjectDiagnostic]) -> String {
    diagnostics
        .iter()
        .map(|diagnostic| {
            format!(
                "{}@{}@{}:{}",
                diagnostic.diagnostic.code,
                diagnostic.module.as_deref().unwrap_or("-"),
                diagnostic.diagnostic.primary.start,
                diagnostic.diagnostic.primary.end
            )
        })
        .collect::<Vec<_>>()
        .join(",")
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
        let source = read_source(&fixture.path)?;
        let expected = compiler::format_source(&source)
            .map_err(|diagnostics| diagnostic_error(fixture, &diagnostics))?;
        let run = || {
            Command::new(compiler)
                .arg("fmt")
                .arg(&fixture.path)
                .output()
                .map_err(|error| {
                    format!("{}: cannot run self-hosted formatter: {error}", fixture.id)
                })
        };
        let first = run()?;
        let second = run()?;
        if !first.status.success() || !second.status.success() {
            return Err(format!(
                "{}: self-hosted formatter failed with {} / {}\n{}{}",
                fixture.id,
                first.status,
                second.status,
                String::from_utf8_lossy(&first.stderr),
                String::from_utf8_lossy(&second.stderr)
            ));
        }
        if !first.stderr.is_empty() || !second.stderr.is_empty() {
            return Err(format!(
                "{}: self-hosted formatter wrote unexpected diagnostics",
                fixture.id
            ));
        }
        if first.stdout != expected.as_bytes() || second.stdout != first.stdout {
            return Err(format!(
                "{}: self-hosted canonical format differs from stage 0",
                fixture.id
            ));
        }
        return Ok(());
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
            let source = read_source(&fixture.path)?;
            let expected = compiler::format_source(&source)
                .map_err(|diagnostics| format!("{}: {diagnostics:#?}", fixture.id))?;
            let run = || {
                Command::new(compiler)
                    .arg("fmt")
                    .arg(&fixture.path)
                    .output()
                    .map_err(|error| {
                        format!(
                            "{}: cannot run self-hosted project formatter: {error}",
                            fixture.id
                        )
                    })
            };
            let first = run()?;
            let second = run()?;
            if !first.status.success()
                || !second.status.success()
                || !first.stderr.is_empty()
                || first.stdout != expected.as_bytes()
                || second.stdout != first.stdout
            {
                return Err(format!(
                    "{}: self-hosted project format differs or is nondeterministic",
                    fixture.id
                ));
            }
            Ok(())
        }
        "interfaces" => {
            let compilation = project::compile(read_source(&fixture.path)?);
            require_project_success(fixture, &compilation)?;
            let expected = compilation
                .interfaces
                .values()
                .map(|artifact| artifact.bytes.as_str())
                .collect::<String>();
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
                || first.stdout != expected.as_bytes()
                || second.stdout != first.stdout
            {
                return Err(format!(
                    "{}: self-hosted public interfaces differ or are nondeterministic",
                    fixture.id
                ));
            }
            Ok(())
        }
        "run" | "emit" => {
            let first = bootstrap::run_compiler(compiler, &fixture.path).map_err(|error| {
                format!("{}: self-host project emit failed: {error}", fixture.id)
            })?;
            let second = bootstrap::run_compiler(compiler, &fixture.path).map_err(|error| {
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
            let original = bootstrap::run_compiler(compiler, &fixture.path).map_err(|error| {
                format!("{}: original self-host emit failed: {error}", fixture.id)
            });
            let moved = bootstrap::run_compiler(compiler, &relocated).map_err(|error| {
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
            for jobs in [1, 2, 4] {
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
            Ok(())
        }
        "cache-corruption" | "incremental" => Err(format!(
            "{}: {} cannot claim parity before the self-hosted session exists",
            fixture.id, fixture.mode
        )),
        _ => unreachable!("project manifest mode validated"),
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
        let projects = load_project_manifest(&root).unwrap();
        check_project_coverage(&root, &projects).unwrap();
    }
}
