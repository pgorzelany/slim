//! Observations of the verified production C compiler, never compiler semantics.
use std::collections::BTreeMap;
use std::ffi::OsString;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};
use std::time::Instant;

use super::{TemporaryDirectory, generated_program, repository_root, selfhost_compiler};

const CAP: u64 = 1_000_000_000;

#[derive(Clone, Copy)]
enum Point {
    Entry,
    Header,
}

struct Hook {
    metric: &'static str,
    function: &'static str,
    point: Point,
    amount: &'static str,
}

// Function names are the canonical qualified source identities. Only this fixed
// table is converted to generated C names; no input program is interpreted here.
const HOOKS: &[Hook] = &[
    Hook {
        metric: "name_insert_steps",
        function: "syntax.ensure_name_chars",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "name_lookup_steps",
        function: "syntax.lookup_name_node_chars",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "name_edge_steps",
        function: "syntax.find_name_edge",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "cache_checksum_steps",
        function: "cache.weighted_checksum",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "program_parse_calls",
        function: "syntax.parse_program_result",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "program_parse_input_bytes",
        function: "syntax.parse_program_result",
        point: Point::Entry,
        amount: "(uint64_t)slim_v_source.len",
    },
    Hook {
        metric: "source_lexer_steps",
        function: "syntax.lex_modern_from",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "data_lex_calls",
        function: "syntax.lex_data",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "data_lex_input_bytes",
        function: "syntax.lex_data",
        point: Point::Entry,
        amount: "(uint64_t)slim_v_input.len",
    },
    Hook {
        metric: "declaration_index_calls",
        function: "syntax.index_declarations",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "declaration_index_steps",
        function: "syntax.index_declarations_from",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "checker_calls",
        function: "check.check_source_mode",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "typing_calls",
        function: "typing.analyze",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "typing_declaration_steps",
        function: "typing.check_functions",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "inline_layout_type_visits",
        function: "typing.check_inline_layout_type",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "c_data_emit_calls",
        function: "codegen.emit_data_item",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "function_check_calls",
        function: "typing.check_function",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "binding_fact_materialization_steps",
        function: "typing.retain_binding_facts",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "expression_check_calls",
        function: "typing.infer_expr",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "ownership_find_calls",
        function: "ownership.root",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "ownership_frame_close_calls",
        function: "ownership.close_frame",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "memory_plan_calls",
        function: "memory.analyze",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "range_analysis_calls",
        function: "ranges.analyze",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "c_generation_calls",
        function: "codegen.emit_program",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "c_function_emit_calls",
        function: "codegen.emit_function",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "c_expression_emit_calls",
        function: "codegen.emit_expr_full",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "project_load_steps",
        function: "project.load_project_modules",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "cache_requests",
        function: "cache.run",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "cache_key_builds",
        function: "cache.project_key",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "cache_probes",
        function: "cache.probe",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "cache_hit_handlers",
        function: "cache.emit_hit",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "cache_miss_handlers",
        function: "cache.emit_miss",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "snapshot_build_calls",
        function: "query.build_snapshots",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "source_key_insertions",
        function: "query.insert_snapshot_key",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "source_key_lookups",
        function: "query.lookup_snapshot_key",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "revision_map_attempts",
        function: "query.prepare_mapping",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "revision_node_translations",
        function: "query.map_node",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "revision_span_translations",
        function: "query.map_span",
        point: Point::Entry,
        amount: "1",
    },
    Hook {
        metric: "source_span_compare_steps",
        function: "project.cross_span_chars_equal",
        point: Point::Header,
        amount: "1",
    },
    Hook {
        metric: "invalidation_estimate_calls",
        function: "query.measure_update",
        point: Point::Entry,
        amount: "1",
    },
];
const READ_CALL: &str = "file_read_calls";
const READ_BYTES: &str = "file_read_bytes";
const ALLOC_ATTEMPTS: &str = "allocation_attempts";
const ALLOC_BYTES: &str = "allocation_requested_bytes";

fn names() -> Vec<&'static str> {
    HOOKS
        .iter()
        .map(|hook| hook.metric)
        .chain([READ_CALL, READ_BYTES, ALLOC_ATTEMPTS, ALLOC_BYTES])
        .collect()
}

fn identity(bytes: &[u8]) -> u64 {
    bytes.iter().fold(0xcbf29ce484222325, |hash, byte| {
        (hash ^ u64::from(*byte)).wrapping_mul(0x100000001b3)
    })
}

fn replace_once(source: &str, anchor: &str, replacement: &str) -> Result<String, String> {
    let count = source.matches(anchor).count();
    if count != 1 {
        return Err(format!(
            "observation anchor `{anchor}` occurs {count} times, expected one"
        ));
    }
    Ok(source.replacen(anchor, replacement, 1))
}

// Resolve a source-owned formal in this exact generated function signature.
// Declaration suffixes are emitted by RFC-0122; never guess a node number.
fn hook_amount(hook: &Hook, signature: &str) -> Result<String, String> {
    if hook.amount == "1" {
        return Ok(String::from("1"));
    }
    let base = hook
        .amount
        .strip_prefix("(uint64_t)")
        .unwrap()
        .strip_suffix(".len")
        .unwrap();
    Ok(format!(
        "(uint64_t){}.len",
        hook_formal(hook.metric, base, signature)?
    ))
}

fn hook_formal(metric: &str, base: &str, signature: &str) -> Result<String, String> {
    let candidates: Vec<_> = signature
        .split(|c: char| !c.is_ascii_alphanumeric() && c != '_')
        .filter(|token| {
            *token == base
                || token.strip_prefix(base).is_some_and(|suffix| {
                    suffix.strip_prefix("_n").is_some_and(|digits| {
                        !digits.is_empty() && digits.bytes().all(|byte| byte.is_ascii_digit())
                    })
                })
        })
        .collect();
    if candidates.len() != 1 {
        return Err(format!(
            "{} formal anchor count is {}, expected one",
            metric,
            candidates.len()
        ));
    }
    Ok(candidates[0].to_owned())
}

fn instrument(seed: &str) -> Result<String, String> {
    let lines: Vec<_> = seed.split_inclusive('\n').collect();
    let mut result = String::from("#include \"work_probe.h\"\n");
    let mut hits = vec![0; HOOKS.len()];
    let mut active = Vec::new();
    let expression_index = HOOKS
        .iter()
        .position(|hook| hook.metric == "expression_check_calls")
        .expect("fixed expression metric");
    let mut child_entry = None;
    let mut child_headers = 0;
    for line in lines {
        if line.starts_with("static SLIM_UNUSED_FUNCTION ") && line.trim_end().ends_with(" {") {
            active.clear();
            child_entry = None;
            if line.contains(" slim_fn_typing_95infer_95control_95walk(") {
                let returning = hook_formal("expression_check_calls", "slim_v_returning", line)?;
                let depth = hook_formal("expression_check_calls", "slim_v_depth", line)?;
                // RFC-0141 keeps the function-root entry above, then checks
                // children in the loop. Parent resumes and declined arguments
                // are not new expression entries; depth zero is already counted.
                child_entry = Some(format!("(uint64_t)(!{returning} && {depth} > 0)"));
            }
            for (index, hook) in HOOKS.iter().enumerate() {
                let encoded = hook.function.replace('.', "_").replace('_', "_95");
                if line.contains(&format!(" slim_fn_{encoded}(")) {
                    active.push(index);
                }
            }
            result.push_str(line);
            for &index in &active {
                if matches!(HOOKS[index].point, Point::Entry) {
                    hits[index] += 1;
                    result.push_str(&format!(
                        "slim_work_add({index}, {});\n",
                        hook_amount(&HOOKS[index], line)?
                    ));
                }
            }
        } else {
            result.push_str(line);
            if line == "slim_recur: ;\n" {
                if let Some(amount) = &child_entry {
                    child_headers += 1;
                    result.push_str(&format!("slim_work_add({expression_index}, {amount});\n"));
                }
                for &index in &active {
                    if matches!(HOOKS[index].point, Point::Header) {
                        hits[index] += 1;
                        result.push_str(&format!(
                            "slim_work_add({index}, {});\n",
                            HOOKS[index].amount
                        ));
                    }
                }
            }
        }
    }
    if child_headers != 1 {
        return Err(format!(
            "expression child observation anchor count is {child_headers}, expected one"
        ));
    }
    for (hook, count) in HOOKS.iter().zip(hits) {
        if count != 1 {
            return Err(format!(
                "{} observation anchor count is {count}, expected one",
                hook.metric
            ));
        }
    }
    replace_once(
        &result,
        "int main(int argc, char **argv) {\n",
        "int main(int argc, char **argv) {\nslim_work_init();\n",
    )
}

fn instrument_runtime(runtime: &str) -> Result<String, String> {
    let allocation = "    uint64_t attempt = atomic_fetch_add(&status->attempts, 1) + 1;\n";
    let runtime = replace_once(
        runtime,
        allocation,
        &format!(
            "{allocation}    slim_work_add({}, 1);\n    slim_work_add({}, (uint64_t)size);\n",
            HOOKS.len() + 2,
            HOOKS.len() + 3
        ),
    )?;
    let call = "bool slim_read_file(SlimBytes path, SlimVec *output) {\n";
    let runtime = replace_once(
        &runtime,
        call,
        &format!("{call}    slim_work_add({}, 1);\n", HOOKS.len()),
    )?;
    let read = "    size_t read = fread(output->data + output->len, 1, (size_t)length, file);\n";
    let runtime = replace_once(
        &runtime,
        read,
        &format!(
            "{read}    slim_work_add({}, (uint64_t)read);\n",
            HOOKS.len() + 1
        ),
    )?;
    Ok(format!("#include \"work_probe.h\"\n{runtime}"))
}

fn write_support(directory: &Path, cap: u64) {
    let metrics = names();
    fs::write(directory.join("work_probe.h"), format!("#include <stdint.h>\n#define SLIM_WORK_LIMIT UINT64_C({cap})\n#define SLIM_WORK_COUNT {}\nextern const char *const slim_work_names[SLIM_WORK_COUNT];\nvoid slim_work_add(unsigned index, uint64_t amount);\nvoid slim_work_init(void);\n", metrics.len())).unwrap();
    fs::write(directory.join("work_names.c"), format!("#include \"work_probe.h\"\nconst char *const slim_work_names[SLIM_WORK_COUNT] = {{ {} }};\n", metrics.iter().map(|name| format!("\"{name}\"")).collect::<Vec<_>>().join(", "))).unwrap();
}

#[derive(Debug, Clone, PartialEq, Eq)]
struct Counter {
    value: u64,
    bounded: bool,
}

fn read_report(path: &Path, cap: u64) -> Result<BTreeMap<String, Counter>, String> {
    let source = fs::read_to_string(path)
        .map_err(|error| format!("unknown work: missing or unreadable exit report: {error}"))?;
    let mut lines = source.lines();
    if lines.next() != Some(format!("slim-work\t1\t{cap}").as_str()) {
        return Err("unknown work: invalid schema/cap".into());
    }
    let mut result = BTreeMap::new();
    for name in names() {
        let row = lines.next().ok_or("unknown work: truncated report")?;
        let columns: Vec<_> = row.split('\t').collect();
        if columns.len() != 3 || columns[0] != name || !["exact", "bounded"].contains(&columns[1]) {
            return Err(format!("unknown work: invalid counter row {row}"));
        }
        let value: u64 = columns[2]
            .parse()
            .map_err(|_| "unknown work: invalid count")?;
        let bounded = columns[1] == "bounded";
        if value > cap || (bounded && value != cap) {
            return Err("unknown work: count violates cap".into());
        }
        result.insert(name.to_owned(), Counter { value, bounded });
    }
    if lines.next() != Some("end") || lines.next().is_some() {
        return Err("unknown work: incomplete or extra report data".into());
    }
    Ok(result)
}

struct Observation {
    output: Output,
    counters: BTreeMap<String, Counter>,
}

impl Observation {
    fn exact(&self, metric: &str) -> u64 {
        let counter = &self.counters[metric];
        assert!(!counter.bounded, "{metric} is bounded, not exact");
        counter.value
    }
}

struct Runner {
    ordinary: PathBuf,
    observed: PathBuf,
    directory: PathBuf,
    sequence: usize,
}

impl Runner {
    fn observe(&mut self, case: &str, arguments: &[OsString], fault: Option<usize>) -> Observation {
        self.sequence += 1;
        let report = self.directory.join(format!("work-{}.tsv", self.sequence));
        assert!(!report.exists(), "stale observation report");
        let mut outputs = Vec::new();
        for compiler in [&self.ordinary, &self.observed] {
            let mut command = Command::new(compiler);
            command
                .args(arguments)
                .env("SLIM_WORK_REPORT", &report)
                .env_remove("SLIM_ALLOC_FAIL_AT");
            if let Some(ordinal) = fault {
                command.env("SLIM_ALLOC_FAIL_AT", ordinal.to_string());
            }
            outputs.push(command.output().expect("run compiler observation"));
            if outputs.len() == 1 {
                assert!(
                    !report.exists(),
                    "ordinary compiler unexpectedly wrote an observation report"
                );
            }
        }
        assert_eq!(
            outputs[0].status, outputs[1].status,
            "{case}: exit status differs"
        );
        assert_eq!(
            outputs[0].stdout, outputs[1].stdout,
            "{case}: output differs"
        );
        assert_eq!(
            outputs[0].stderr, outputs[1].stderr,
            "{case}: diagnostics differ"
        );
        let counters =
            read_report(&report, CAP).unwrap_or_else(|reason| panic!("{case}: {reason}"));
        let repeated_report = report.with_extension("repeat.tsv");
        assert!(!repeated_report.exists());
        let mut repeated = Command::new(&self.observed);
        repeated
            .args(arguments)
            .env("SLIM_WORK_REPORT", &repeated_report)
            .env_remove("SLIM_ALLOC_FAIL_AT");
        if let Some(ordinal) = fault {
            repeated.env("SLIM_ALLOC_FAIL_AT", ordinal.to_string());
        }
        let repeated = repeated.output().expect("repeat compiler observation");
        assert_eq!(
            repeated.status, outputs[1].status,
            "{case}: nondeterministic exit"
        );
        assert_eq!(
            repeated.stdout, outputs[1].stdout,
            "{case}: nondeterministic output"
        );
        assert_eq!(
            repeated.stderr, outputs[1].stderr,
            "{case}: nondeterministic diagnostics"
        );
        assert_eq!(
            read_report(&repeated_report, CAP).unwrap(),
            counters,
            "{case}: nondeterministic work counts"
        );
        let mut input = Vec::new();
        for argument in arguments {
            input.extend_from_slice(argument.to_string_lossy().as_bytes());
            if let Ok(bytes) = fs::read(argument) {
                input.extend_from_slice(&bytes);
            }
            let path = Path::new(argument);
            if path
                .extension()
                .is_some_and(|extension| extension == "project")
            {
                let mut modules: Vec<_> = fs::read_dir(path.parent().unwrap())
                    .unwrap()
                    .map(|entry| entry.unwrap().path())
                    .filter(|path| {
                        path.extension()
                            .is_some_and(|extension| extension == "slim")
                    })
                    .collect();
                modules.sort();
                for module in modules {
                    input.extend_from_slice(&fs::read(module).unwrap());
                }
            }
        }
        let output = outputs.pop().unwrap();
        if case.starts_with("snapshot-") {
            println!(
                "# {case}: invalidation-estimate-output={:?}; these are not observed compiler operations",
                String::from_utf8_lossy(&output.stdout)
            );
        }
        println!(
            "# operation={case}; input_fnv1a64={:016x}; output_fnv1a64={:016x}; output_bytes={}; exit={:?}; observed_repetitions=2; counts_per_process; external_cc_calls=0",
            identity(&input),
            identity(&output.stdout),
            output.stdout.len(),
            output.status.code()
        );
        for name in names() {
            let counter = &counters[name];
            println!(
                "{case}\t{name}\t{}\t{}\t{CAP}",
                if counter.bounded { "bounded" } else { "exact" },
                counter.value
            );
        }
        Observation { output, counters }
    }
}

fn args(parts: &[&Path]) -> Vec<OsString> {
    parts
        .iter()
        .map(|path| path.as_os_str().to_owned())
        .collect()
}

fn build_observed(ordinary: &Path, directory: &Path, sanitize: bool) -> PathBuf {
    let root = repository_root();
    let seed = fs::read(root.join("bootstrap/slimc-seed.c")).unwrap();
    let generated = Command::new(ordinary)
        .arg(root.join("selfhost/slim.project"))
        .output()
        .unwrap();
    assert!(
        generated.status.success() && generated.stderr.is_empty(),
        "production compiler cannot reproduce seed"
    );
    assert_eq!(
        generated.stdout, seed,
        "production compiler does not reproduce the current seed"
    );
    let source = instrument(std::str::from_utf8(&seed).unwrap()).expect("instrument verified seed");
    let runtime = fs::read_to_string(root.join("runtime/slim_rt.c")).unwrap();
    let runtime = instrument_runtime(&runtime).expect("instrument actual file reads");
    write_support(directory, CAP);
    fs::write(directory.join("compiler.c"), &source).unwrap();
    fs::write(directory.join("runtime.c"), &runtime).unwrap();
    let probe = fs::read(root.join("benchmarks/instrumentation/work_probe.c")).unwrap();
    fs::write(directory.join("probe.c"), &probe).unwrap();
    let binary = directory.join("observed-compiler");
    let cc = std::env::var_os("CC").unwrap_or_else(|| "cc".into());
    let flags = if sanitize {
        vec![
            "-std=c11",
            "-O1",
            "-g",
            "-fsanitize=address,undefined",
            "-fno-omit-frame-pointer",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-USLIM_PARALLEL",
            "-USLIM_POSIX_WORKERS",
        ]
    } else {
        vec![
            "-std=c11",
            "-O2",
            "-DNDEBUG",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-USLIM_PARALLEL",
            "-USLIM_POSIX_WORKERS",
        ]
    };
    let output = Command::new(&cc)
        .args(&flags)
        .arg("-I")
        .arg(root.join("runtime"))
        .arg("-I")
        .arg(directory)
        .arg(directory.join("compiler.c"))
        .arg(directory.join("runtime.c"))
        .arg(directory.join("probe.c"))
        .arg(directory.join("work_names.c"))
        .arg("-o")
        .arg(&binary)
        .output()
        .unwrap();
    assert!(
        output.status.success(),
        "instrumentation build: {}",
        String::from_utf8_lossy(&output.stderr)
    );
    println!(
        "# schema=1; counter_cap={CAP}; host={}-{}; instrumentation=RFC-0121; serial; setup_cc_calls=1; flags={}",
        std::env::consts::OS,
        std::env::consts::ARCH,
        flags.join(",")
    );
    let version = Command::new(&cc).arg("--version").output().unwrap();
    assert!(version.status.success());
    println!(
        "# toolchain={}; probe_fnv1a64={:016x}; harness_fnv1a64={:016x}",
        String::from_utf8_lossy(&version.stdout)
            .lines()
            .next()
            .unwrap_or("unknown"),
        identity(&probe),
        identity(include_bytes!("work.rs"))
    );
    println!(
        "# runtime_header_fnv1a64={:016x}; setup_native_reproduction_calls=1; file-read counters exclude the observer report I/O",
        identity(&fs::read(root.join("runtime/slim_rt.h")).unwrap())
    );
    println!(
        "# seed_bytes={}; seed_fnv1a64={:016x}; observed_source_fnv1a64={:016x}; observed_runtime_fnv1a64={:016x}; compiler_fnv1a64={:016x}; observed_binary_fnv1a64={:016x}; cc={cc:?}",
        seed.len(),
        identity(&seed),
        identity(source.as_bytes()),
        identity(runtime.as_bytes()),
        identity(&fs::read(ordinary).unwrap()),
        identity(&fs::read(&binary).unwrap())
    );
    for hook in HOOKS {
        println!(
            "# metric={}; source={}; point={}; amount={}",
            hook.metric,
            hook.function,
            if matches!(hook.point, Point::Entry) {
                "native-entry"
            } else {
                "loop-header-including-terminal"
            },
            hook.amount
        );
    }
    println!("case\tmetric\tguarantee\tcount\tcap");
    binary
}

pub(super) fn run() {
    let options: Vec<_> = std::env::args().skip(2).collect();
    assert!(
        options
            .iter()
            .all(|option| ["--quick", "--sanitize"].contains(&option.as_str())),
        "usage: slim-bench work [--quick] [--sanitize]"
    );
    let sanitize = options.iter().any(|option| option == "--sanitize");
    let ordinary = selfhost_compiler();
    let ordinary_bytes = fs::read(&ordinary).unwrap();
    let seed_before = fs::read(repository_root().join("bootstrap/slimc-seed.c")).unwrap();
    let runtime_sources: Vec<_> = [
        "runtime/slim_rt.c",
        "runtime/slim_rt.h",
        "benchmarks/instrumentation/work_probe.c",
    ]
    .into_iter()
    .map(|path| {
        (
            repository_root().join(path),
            fs::read(repository_root().join(path)).unwrap(),
        )
    })
    .collect();
    let directory = TemporaryDirectory::new("observed-work");
    let observed = build_observed(&ordinary, &directory.path, sanitize);
    let mut runner = Runner {
        ordinary,
        observed,
        directory: directory.path.clone(),
        sequence: 0,
    };
    let sizes = if options.iter().any(|option| option == "--quick") {
        [125, 250, 500, 1_000]
    } else {
        [1_000, 2_000, 4_000, 8_000]
    };
    for size in sizes {
        let path = directory.path.join(format!("declarations-{size}.slim"));
        let source = generated_program(size);
        fs::write(&path, source.as_bytes()).unwrap();
        for command in ["check", "emit"] {
            let arguments = if command == "check" {
                args(&[Path::new("check"), &path])
            } else {
                args(&[&path])
            };
            let result = runner.observe(&format!("{command}-{size}"), &arguments, None);
            assert!(result.output.status.success());
            assert_eq!(result.exact("program_parse_calls"), 1);
            assert_eq!(
                result.exact("program_parse_input_bytes"),
                source.len() as u64
            );
            assert_eq!(result.exact("checker_calls"), 1);
            assert_eq!(result.exact("typing_calls"), 1);
            assert_eq!(result.exact("declaration_index_steps"), size as u64 + 2);
            assert_eq!(result.exact("typing_declaration_steps"), size as u64 + 2);
            assert_eq!(result.exact("function_check_calls"), size as u64 + 1);
            assert!(result.exact(ALLOC_ATTEMPTS) <= 16 * size as u64 + 128);
            assert_eq!(
                result.exact("binding_fact_materialization_steps"),
                2 * (size as u64 + 1)
            );
            assert_eq!(result.exact("expression_check_calls"), 2 * size as u64 + 3);
            assert!(result.exact("source_lexer_steps") <= 2 * source.len() as u64 + 1);
            assert!(result.exact("name_insert_steps") > 0);
            assert!(result.exact("name_lookup_steps") > 0);
            assert!(result.exact("name_edge_steps") <= 64 * (source.len() as u64 + 1));
            assert_eq!(
                result.exact("c_generation_calls"),
                u64::from(command == "emit")
            );
            if command == "emit" {
                assert_eq!(result.exact("c_function_emit_calls"), size as u64 + 1);
            }
        }
    }
    for size in [125, 250, 500, 1_000] {
        let mut source = String::from(
            "module reset_work\n\nfn consume(values: ^Vec[I64]) -> Void:\n  void\n\nfn exercise(flag: Bool) -> Void effects[alloc]:\n",
        );
        for index in 0..size {
            source.push_str(&format!(
                "  var owner_{index}: Vec[I64] = vec.new()\n  consume(^owner_{index})\n"
            ));
        }
        for arm in ["if flag", "else"] {
            source.push_str(&format!("  {arm}:\n"));
            for index in 0..size {
                source.push_str(&format!("    owner_{index} = vec.new()\n"));
            }
            source.push_str("    void\n");
        }
        for index in 0..size {
            source.push_str(&format!("  consume(^owner_{index})\n"));
        }
        source.push_str("  void\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n");
        let path = directory.path.join(format!("reset-{size}.slim"));
        fs::write(&path, source).unwrap();
        let result = runner.observe(
            &format!("reset-{size}"),
            &args(&[Path::new("check"), &path]),
            None,
        );
        assert!(result.output.status.success());
        assert_eq!(result.exact("ownership_frame_close_calls"), 3 * size as u64);
        assert!(result.exact("ownership_find_calls") <= 16 * size as u64 + 32);
    }
    layout_order_campaign(&mut runner);
    revision_mapping_campaign(&mut runner);
    project_campaign(&mut runner);
    let root = repository_root();
    let mut challenges: Vec<_> = fs::read_dir(root.join("benchmarks/challenges"))
        .unwrap()
        .map(|entry| entry.unwrap().path().join("program.slim"))
        .collect();
    challenges.sort();
    for path in challenges {
        if path.is_file() {
            let name = path
                .parent()
                .unwrap()
                .file_name()
                .unwrap()
                .to_string_lossy();
            let result = runner.observe(&format!("native-{name}"), &args(&[&path]), None);
            assert!(result.output.status.success());
            assert_eq!(result.exact("checker_calls"), 1);
            assert_eq!(result.exact("c_generation_calls"), 1);
        }
    }
    let witness = root.join("conformance/pass/reinit_branches.slim");
    let mut failed_inside_function = false;
    let mut passed_beyond_function_allocations = false;
    for ordinal in 1..=128 {
        let result = runner.observe(
            &format!("allocation-fault-{ordinal}"),
            &args(&[Path::new("check"), &witness]),
            Some(ordinal),
        );
        assert!([Some(0), Some(71)].contains(&result.output.status.code()));
        assert_eq!(result.exact("c_generation_calls"), 0);
        if result.output.status.code() == Some(71) {
            assert_eq!(result.exact(ALLOC_ATTEMPTS), ordinal as u64);
            assert!(result.output.stdout.is_empty());
            failed_inside_function |= result.exact("function_check_calls") > 0;
        } else {
            passed_beyond_function_allocations = true;
        }
        if ordinal == 1 {
            assert_eq!(result.exact("program_parse_calls"), 0);
            assert_eq!(result.exact("checker_calls"), 0);
        }
    }
    assert!(failed_inside_function);
    assert!(passed_beyond_function_allocations);
    assert_eq!(
        fs::read(&runner.ordinary).unwrap(),
        ordinary_bytes,
        "ordinary compiler changed during observation"
    );
    assert_eq!(
        fs::read(root.join("bootstrap/slimc-seed.c")).unwrap(),
        seed_before,
        "portable seed changed during observation"
    );
    for (path, bytes) in runtime_sources {
        assert_eq!(
            fs::read(&path).unwrap(),
            bytes,
            "{} changed during observation",
            path.display()
        );
    }
    println!(
        "# work campaign passed; instrumented output equals the ordinary production compiler; no retained incremental checking claim"
    );
}

fn write_project(directory: &Path, variant: &str) -> PathBuf {
    fs::create_dir_all(directory).unwrap();
    let mut library = String::from(
        "module library\n\nstruct Pair:\n  left: I64\n  right: I64\n\nfn compute(value: I64) -> I64:\n  value + 1\n\nfn observe(values: Vec[I64]) -> I64:\n  vec.len(values)\n\nfn spare() -> I64:\n  0\n",
    );
    let mut app = String::from(
        "module app\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let pair: library.Pair = library.Pair(left: 40, right: 1)\n  let computed: I64 = library.compute(pair.left)\n  let items: Vec[I64] = vec.new()\n  let count: I64 = library.observe(items)\n  0\n",
    );
    let mut exports = "Pair compute observe spare".to_owned();
    match variant {
        "base" => {}
        "body" => library = library.replace("value + 1", "value + 2"),
        "signature" => {
            library = library
                .replace("compute(value: I64)", "compute(value: I64, extra: I64)")
                .replace("value + 1", "value + extra");
            app = app.replace("compute(pair.left)", "compute(pair.left, 1)");
        }
        "layout" => {
            library = library.replace("  right: I64", "  right: I64\n  extra: I64");
            app = app.replace("right: 1)", "right: 1, extra: 0)");
        }
        "effect" => {
            library = library.replace(
                "compute(value: I64) -> I64:",
                "compute(value: I64) -> I64 effects[alloc]:",
            )
        }
        "borrow-mode" | "invalid-borrow-mode" => {
            library = library.replace("values: Vec[I64]", "values: ^Vec[I64]");
            if variant == "borrow-mode" {
                app = app.replace("observe(items)", "observe(^items)");
            }
        }
        "insertion" => library.push_str("\nfn extra() -> I64:\n  1\n"),
        "deletion" => {
            library = library.replace("\nfn spare() -> I64:\n  0\n", "");
            exports = exports.replace(" spare", "");
        }
        "rename" => {
            library = library.replace("compute", "calculate");
            app = app.replace("compute", "calculate");
            exports = exports.replace("compute", "calculate");
        }
        "reordering" => {
            library = library.replace("\nfn spare() -> I64:\n  0\n", "");
            library = library.replace(
                "module library\n",
                "module library\n\nfn spare() -> I64:\n  0\n",
            );
        }
        "invalid-syntax" => app = "module app\nfn broken(\n".into(),
        _ => panic!("unknown work fixture variant"),
    }
    fs::write(directory.join("library.slim"), library).unwrap();
    fs::write(directory.join("app.slim"), app).unwrap();
    let manifest = directory.join("slim.project");
    fs::write(&manifest, format!("(project 1 (entry app)\n  (module app \"app.slim\" (imports library) (exports))\n  (module library \"library.slim\" (imports) (exports {exports})))\n")).unwrap();
    manifest
}

fn miss_artifact(output: &[u8]) -> (&[u8], &[u8]) {
    assert_eq!(
        output.first(),
        Some(&b'M'),
        "expected native cache miss frame"
    );
    let frame = &output[1..];
    assert!(frame.len() >= 35 && frame.starts_with(b"SLIMCACHE\0\x03"));
    let key = usize::try_from(u64::from_be_bytes(frame[11..19].try_into().unwrap())).unwrap();
    let length = usize::try_from(u64::from_be_bytes(frame[19..27].try_into().unwrap())).unwrap();
    let start = 27usize.checked_add(key).unwrap();
    let end = start.checked_add(length).unwrap();
    assert_eq!(end.checked_add(8).unwrap(), frame.len());
    (frame, &frame[start..end])
}

fn assert_miss(result: &Observation, clean: &[u8]) {
    assert!(result.output.status.success());
    assert_eq!(result.exact("cache_requests"), 1);
    assert_eq!(result.exact("cache_miss_handlers"), 1);
    assert_eq!(result.exact("cache_hit_handlers"), 0);
    assert_eq!(result.exact("checker_calls"), 1);
    assert_eq!(result.exact("c_generation_calls"), 1);
    // Two module parses plus the existing flattened-source reparse.
    assert_eq!(result.exact("program_parse_calls"), 3);
    assert_eq!(miss_artifact(&result.output.stdout).1, clean);
}

fn assert_hit(result: &Observation, clean: &[u8]) {
    assert!(result.output.status.success());
    assert_eq!(result.output.stdout.first(), Some(&b'H'));
    assert_eq!(&result.output.stdout[1..], clean);
    assert_eq!(result.exact("cache_hit_handlers"), 1);
    assert_eq!(result.exact("cache_miss_handlers"), 0);
    for metric in [
        "program_parse_calls",
        "checker_calls",
        "typing_calls",
        "c_generation_calls",
        "c_function_emit_calls",
    ] {
        assert_eq!(result.exact(metric), 0, "cache hit performed {metric}");
    }
    assert!(result.exact("data_lex_calls") > 0);
    assert!(result.exact("file_read_calls") >= 4);
    assert!(result.exact("file_read_bytes") > clean.len() as u64);
}

fn project_campaign(runner: &mut Runner) {
    let directory = runner.directory.join("project");
    let manifest = write_project(&directory, "base");
    let cache = runner.directory.join("artifact.cache");
    let clean = runner.observe("project-clean", &args(&[&manifest]), None);
    assert!(clean.output.status.success());
    execute_backend(&runner.directory, "base", &clean.output.stdout);
    let cold = runner.observe(
        "cache-cold",
        &args(&[Path::new("cache"), &manifest, &cache]),
        None,
    );
    assert_miss(&cold, &clean.output.stdout);
    let frame = miss_artifact(&cold.output.stdout).0.to_vec();
    fs::write(&cache, &frame).unwrap();
    let hit = runner.observe(
        "cache-unchanged",
        &args(&[Path::new("cache"), &manifest, &cache]),
        None,
    );
    assert_hit(&hit, &clean.output.stdout);
    let session = runner.observe(
        "snapshot-unchanged",
        &args(&[Path::new("session"), &manifest, &manifest]),
        None,
    );
    assert!(session.output.status.success());
    assert_eq!(session.output.stdout, b"0 0 0 0\n");
    assert_eq!(session.exact("program_parse_calls"), 4);
    assert_eq!(session.exact("snapshot_build_calls"), 2);
    assert_eq!(session.exact("invalidation_estimate_calls"), 1);
    assert_eq!(session.exact("checker_calls"), 0);
    assert_eq!(session.exact("c_generation_calls"), 0);
    for variant in [
        "body",
        "signature",
        "layout",
        "effect",
        "borrow-mode",
        "insertion",
        "deletion",
        "rename",
        "reordering",
    ] {
        let updated = write_project(&runner.directory.join(format!("edit-{variant}")), variant);
        let generated = runner.observe(&format!("clean-{variant}"), &args(&[&updated]), None);
        assert!(generated.output.status.success());
        execute_backend(&runner.directory, variant, &generated.output.stdout);
        let result = runner.observe(
            &format!("cache-{variant}"),
            &args(&[Path::new("cache"), &updated, &cache]),
            None,
        );
        assert_miss(&result, &generated.output.stdout);
        let accepted_cache = runner.directory.join(format!("accepted-{variant}.cache"));
        fs::write(&accepted_cache, miss_artifact(&result.output.stdout).0).unwrap();
        let accepted_hit = runner.observe(
            &format!("cache-warm-{variant}"),
            &args(&[Path::new("cache"), &updated, &accepted_cache]),
            None,
        );
        assert_hit(&accepted_hit, &generated.output.stdout);
        let session = runner.observe(
            &format!("snapshot-{variant}"),
            &args(&[Path::new("session"), &manifest, &updated]),
            None,
        );
        assert!(session.output.status.success());
        assert_eq!(session.exact("checker_calls"), 0);
        assert_eq!(session.exact("c_generation_calls"), 0);
        assert_eq!(session.exact("program_parse_calls"), 4);
        assert_eq!(session.exact("invalidation_estimate_calls"), 1);
    }
    let relocated = write_project(&runner.directory.join("relocated"), "base");
    let relocated_clean = runner.observe("clean-relocated", &args(&[&relocated]), None);
    assert_eq!(relocated_clean.output.stdout, clean.output.stdout);
    let relocated_hit = runner.observe(
        "cache-relocated",
        &args(&[Path::new("cache"), &relocated, &cache]),
        None,
    );
    assert_hit(&relocated_hit, &clean.output.stdout);
    for invalid in ["invalid-syntax", "invalid-borrow-mode"] {
        let rejected = write_project(&runner.directory.join(invalid), invalid);
        let clean_error = runner.observe(&format!("clean-{invalid}"), &args(&[&rejected]), None);
        let cached_error = runner.observe(
            &format!("cache-{invalid}"),
            &args(&[Path::new("cache"), &rejected, &cache]),
            None,
        );
        assert!(!cached_error.output.status.success());
        assert_eq!(cached_error.output.stdout, clean_error.output.stdout);
        assert_eq!(cached_error.exact("c_generation_calls"), 0);
        assert_eq!(
            fs::read(&cache).unwrap(),
            frame,
            "failed update changed last good cache"
        );
        let recovered = runner.observe(
            &format!("cache-recovered-{invalid}"),
            &args(&[Path::new("cache"), &manifest, &cache]),
            None,
        );
        assert_hit(&recovered, &clean.output.stdout);
        let session = runner.observe(
            &format!("snapshot-recovered-{invalid}"),
            &args(&[Path::new("session"), &manifest, &rejected, &manifest]),
            None,
        );
        assert!(session.output.status.success());
        assert!(session.exact("checker_calls") >= 2);
        assert_eq!(session.exact("c_generation_calls"), 0);
        assert_eq!(session.exact("snapshot_build_calls"), 2);
        assert!(session.exact("program_parse_calls") >= 8);
    }
    for damage in ["truncated-header", "truncated-key", "checksum", "schema"] {
        let mut corrupt = frame.clone();
        match damage {
            "truncated-header" => corrupt.truncate(13),
            "truncated-key" => corrupt.truncate(28),
            "checksum" => {
                let last = corrupt.len() - 1;
                corrupt[last] ^= 1;
            }
            "schema" => corrupt[10] = 255,
            _ => unreachable!(),
        }
        fs::write(&cache, corrupt).unwrap();
        let result = runner.observe(
            &format!("cache-corrupt-{damage}"),
            &args(&[Path::new("cache"), &manifest, &cache]),
            None,
        );
        assert_miss(&result, &clean.output.stdout);
    }
}

fn execute_backend(directory: &Path, case: &str, generated: &[u8]) {
    let root = repository_root();
    let source = directory.join(format!("backend-{case}.c"));
    let binary = directory.join(format!("backend-{case}"));
    fs::write(&source, generated).unwrap();
    let mut calls = 0u64;
    let started = Instant::now();
    let output = Command::new(std::env::var_os("CC").unwrap_or_else(|| "cc".into()))
        .args(["-std=c11", "-O2", "-Wall", "-Wextra", "-Werror"])
        .arg("-I")
        .arg(root.join("runtime"))
        .arg(source)
        .arg(root.join("runtime/slim_rt.c"))
        .arg("-o")
        .arg(&binary)
        .output();
    calls += 1;
    let output = output.expect("execute external backend");
    let elapsed = started.elapsed();
    assert!(
        output.status.success(),
        "backend {case}: {}",
        String::from_utf8_lossy(&output.stderr)
    );
    let program = Command::new(binary).output().unwrap();
    assert!(
        program.status.success() && program.stdout.is_empty() && program.stderr.is_empty(),
        "native fixture {case} failed"
    );
    println!("backend-{case}\texternal_cc_calls\texact\t{calls}\t{CAP}");
    println!(
        "# backend-{case}: external_cc_ns={}; generated_fnv1a64={:016x}; application_exit=0; flags=-std=c11,-O2,-Wall,-Wextra,-Werror; this backend time is separate from frontend work",
        elapsed.as_nanos(),
        identity(generated)
    );
}

fn revision_mapping_campaign(runner: &mut Runner) {
    let directory = runner.directory.join("revision-mapping");
    fs::create_dir(&directory).unwrap();
    for size in [125, 250, 500, 1_000] {
        let declarations: Vec<_> = (0..size)
            .map(|index| format!("fn value_{index}() -> I64:\n  {index}\n\n"))
            .collect();
        let mut initial = String::from("module app\n\n");
        initial.extend(declarations.iter().map(String::as_str));
        initial.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
        let mut reordered = String::from("# shifted origin\nmodule app\n\n");
        reordered.extend(declarations.iter().rev().map(String::as_str));
        reordered.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
        fs::write(directory.join("initial.slim"), &initial).unwrap();
        fs::write(directory.join("reordered.slim"), &reordered).unwrap();
        for name in ["initial", "reordered"] {
            fs::write(
                directory.join(format!("{name}.project")),
                format!(
                    "(project 1 (entry app) (module app \"{name}.slim\" (imports) (exports)))\n"
                ),
            )
            .unwrap();
        }
        for name in ["initial", "reordered"] {
            let result = runner.observe(
                &format!("revision-map-{name}-{size}"),
                &args(&[
                    Path::new("session"),
                    &directory.join("initial.project"),
                    &directory.join(format!("{name}.project")),
                ]),
                None,
            );
            assert!(result.output.status.success());
            assert_eq!(result.output.stdout, b"0 0 0 0\n");
            assert_eq!(result.exact("program_parse_calls"), 2);
            assert_eq!(result.exact("checker_calls"), 0);
            assert_eq!(result.exact("c_generation_calls"), 0);
            let is_reordered = name == "reordered";
            assert_eq!(
                result.exact("source_key_insertions"),
                if is_reordered { size + 1 } else { 0 }
            );
            assert_eq!(
                result.exact("source_key_lookups"),
                if is_reordered { size - size % 2 } else { 0 }
            );
            for metric in [
                "revision_map_attempts",
                "revision_node_translations",
                "revision_span_translations",
            ] {
                assert_eq!(result.exact(metric), size + 1);
            }
            let source_bytes = (initial.len() + reordered.len()) as u64;
            for metric in [
                "name_insert_steps",
                "name_lookup_steps",
                "source_span_compare_steps",
            ] {
                assert!(
                    result.exact(metric) <= 4 * source_bytes,
                    "{metric} exceeded linear source budget"
                );
            }
            assert!(result.exact("name_edge_steps") <= 64 * source_bytes);
        }
    }
    fs::write(directory.join("initial.slim"), "module app\n\nfn first() -> I64:\n  1\n\nfn second() -> I64:\n  2\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n").unwrap();
    fs::write(directory.join("reordered.slim"), "module app\n\nfn second() -> I64:\n  2\n\nfn first() -> I64:\n  1\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n").unwrap();
    let mut failed_inside_index = false;
    let mut passed_beyond_allocations = false;
    for ordinal in 1..=128 {
        let result = runner.observe(
            &format!("revision-map-fault-{ordinal}"),
            &args(&[
                Path::new("session"),
                &directory.join("initial.project"),
                &directory.join("reordered.project"),
            ]),
            Some(ordinal),
        );
        if result.output.status.code() == Some(71) {
            assert!(result.output.stdout.is_empty());
            failed_inside_index |= result.exact("source_key_insertions") > 0;
        } else {
            assert!(result.output.status.success());
            assert_eq!(result.output.stdout, b"0 0 0 0\n");
            assert_eq!(result.exact("source_key_insertions"), 3);
            passed_beyond_allocations = true;
        }
    }
    assert!(
        failed_inside_index,
        "fault campaign did not reach lazy index construction"
    );
    assert!(
        passed_beyond_allocations,
        "fault campaign did not cross all allocations of its fixture"
    );
}

fn layout_order_campaign(runner: &mut Runner) {
    for size in [125, 250, 500, 1_000] {
        let mut source = String::from("module layouts\n\n");
        for index in 0..size {
            source.push_str(&format!(
                "struct Root_{index}:\n  left: Left_{index}\n  right: Right_{index}\n\nenum Left_{index}:\n  Some(Leaf_{index})\n  None\n\nstruct Right_{index}:\n  leaf: Leaf_{index}\n\nstruct Leaf_{index}:\n  value: I64\n\n"
            ));
        }
        source.push_str("fn main(args: Vec[Bytes]) -> I64:\n  0\n");
        let path = runner.directory.join(format!("layouts-{size}.slim"));
        fs::write(&path, &source).unwrap();
        for command in ["check", "emit"] {
            let arguments = if command == "check" {
                args(&[Path::new("check"), &path])
            } else {
                args(&[&path])
            };
            let result = runner.observe(&format!("layouts-{command}-{size}"), &arguments, None);
            assert!(result.output.status.success());
            assert_eq!(result.exact("inline_layout_type_visits"), 5 * size as u64);
            assert_eq!(
                result.exact("c_data_emit_calls"),
                if command == "emit" {
                    4 * size as u64
                } else {
                    0
                }
            );
            assert_eq!(result.exact("function_check_calls"), 1);
            assert!(result.exact(ALLOC_ATTEMPTS) <= 16 * size as u64 + 128);
        }
    }
    let witness = repository_root().join("conformance/pass/inline_forward_layouts.slim");
    let mut failed_during_layout = false;
    let mut failed_during_emission = false;
    let mut passed_beyond_allocations = false;
    for ordinal in 1..=256 {
        let result = runner.observe(
            &format!("layouts-fault-{ordinal}"),
            &args(&[&witness]),
            Some(ordinal),
        );
        match result.output.status.code() {
            Some(71) => {
                assert!(result.output.stdout.is_empty());
                assert_eq!(result.exact(ALLOC_ATTEMPTS), ordinal as u64);
                failed_during_layout |= result.exact("inline_layout_type_visits") > 0
                    && result.exact("c_generation_calls") == 0;
                failed_during_emission |= result.exact("c_data_emit_calls") > 0;
            }
            Some(0) => passed_beyond_allocations = true,
            status => panic!("unexpected layout allocation-fault status: {status:?}"),
        }
    }
    assert!(failed_during_layout);
    assert!(failed_during_emission);
    assert!(passed_beyond_allocations);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn observation_anchors_reject_missing_or_duplicate_definitions() {
        let seed = fs::read_to_string(repository_root().join("bootstrap/slimc-seed.c")).unwrap();
        assert!(instrument(&seed).is_ok());
        assert!(instrument("").is_err());
        assert!(instrument(&format!("{seed}{seed}")).is_err());
        assert!(replace_once("same same", "same", "new").is_err());
        assert!(instrument_runtime("").is_err());
    }

    #[test]
    fn missing_and_incomplete_observations_are_unknown() {
        let directory = TemporaryDirectory::new("missing-work-report");
        let report = directory.path.join("report");
        assert!(read_report(&report, CAP).unwrap_err().contains("unknown"));
        fs::write(&report, format!("slim-work\t1\t{CAP}\n")).unwrap();
        assert!(read_report(&report, CAP).is_err());
    }

    #[test]
    fn native_counters_saturate_without_overflow_and_preserve_exact_zeros() {
        let root = repository_root();
        let directory = TemporaryDirectory::new("work-counter-cap");
        write_support(&directory.path, 7);
        let source = directory.path.join("exercise.c");
        fs::write(&source, "#include \"work_probe.h\"\nint main(void) { slim_work_init(); slim_work_add(0, UINT64_MAX); slim_work_add(0, UINT64_MAX); slim_work_add(1, 7); slim_work_add(1, 1); slim_work_add(2, 6); slim_work_add(3, 7); return 0; }\n").unwrap();
        let binary = directory.path.join("exercise");
        let build = Command::new("cc")
            .args([
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-Werror",
                "-fsanitize=undefined",
            ])
            .arg("-I")
            .arg(&directory.path)
            .arg(&source)
            .arg(root.join("benchmarks/instrumentation/work_probe.c"))
            .arg(directory.path.join("work_names.c"))
            .arg("-o")
            .arg(&binary)
            .output()
            .unwrap();
        assert!(
            build.status.success(),
            "{}",
            String::from_utf8_lossy(&build.stderr)
        );
        let report = directory.path.join("report.tsv");
        let output = Command::new(binary)
            .env("SLIM_WORK_REPORT", &report)
            .output()
            .unwrap();
        assert!(output.status.success() && output.stdout.is_empty() && output.stderr.is_empty());
        let counters = read_report(&report, 7).unwrap();
        let metrics = names();
        for name in &metrics[0..2] {
            assert_eq!(
                counters[*name],
                Counter {
                    value: 7,
                    bounded: true
                }
            );
        }
        assert_eq!(
            counters[metrics[2]],
            Counter {
                value: 6,
                bounded: false
            }
        );
        assert_eq!(
            counters[metrics[3]],
            Counter {
                value: 7,
                bounded: false
            }
        );
        for name in &metrics[4..] {
            assert_eq!(
                counters[*name],
                Counter {
                    value: 0,
                    bounded: false
                }
            );
        }
    }
}
