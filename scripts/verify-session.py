"""Differential host driver; all semantics execute in the production SLIM probe."""
from pathlib import Path
import os
import re
import subprocess
import sys
from session_faults import run_faults

root = Path(sys.argv[1])
full = sys.argv[2] != "quick"
report_path = root / "report.tsv"
ordinary = root / "ordinary"
observed = root / "observed"
compiler = sys.argv[3]


def project(name, source, manifest=None, **modules):
    directory = root / name
    directory.mkdir(exist_ok=True)
    module = re.search(r"^module (\w+)", source, re.M).group(1)
    manifest = manifest or f'(project 1 (entry {module}) (module {module} "program.slim" (imports) (exports)))\n'
    (directory / "slim.project").write_text(manifest)
    (directory / "program.slim").write_text(source)
    for filename, content in modules.items():
        (directory / filename).write_text(content)
    return directory / "slim.project"


def run(before, after, mode="work", expected=None, epochs=1, third=None):
    args = [str(before), str(after), mode]
    if third is not None:
        args.append(str(third))
    plain = subprocess.run([str(ordinary), *args], capture_output=True)
    report_path.unlink(missing_ok=True)
    native = subprocess.run([str(observed), *args], capture_output=True,
                            env=dict(os.environ, SLIM_SESSION_REPORT=str(report_path)))
    assert plain.returncode == native.returncode == 0, (mode, plain.returncode, native.returncode, plain.stdout[:400], native.stderr[:400])
    assert plain.stdout == native.stdout and plain.stderr == native.stderr, mode
    lines = report_path.read_text().splitlines()
    assert lines[0] == f"slim-session\t7\texact\t1000000000\t{epochs}", lines[0]
    rows = [list(map(int, line.split("\t"))) for line in lines[1:]]
    assert all(row[0] == i and len(row) == 24 for i, row in enumerate(rows)), rows
    for row in rows:
        assert row[17] == row[18], (mode, rows)
        assert (row[20] > 0) == (row[21] > 0), (mode, rows)
        if row[9] == 0:
            assert row[23] == 0, (mode, rows)
        if row[3] == 0:
            assert row[17:22] == [0] * 5, (mode, rows)
        if row[9] > 0:
            assert row[8:10] == [0, 1] and row[12] == 5 and row[23] == 4 and row[13] in (0, 2, 4, 6, 8) and row[15:17] == [3, 4], (mode, rows)
            assert row[10] + row[11] == 5 * (row[6] + row[7]), (mode, rows)

    if expected is not None:
        assert [row[1:4] for row in rows] == expected, (mode, rows, expected)
    reports = [line.split() for line in native.stdout.decode().splitlines() if re.match(r"^\d+ \d+ \d+ \d+ ", line)]
    # Full-epoch suppresses intermediate textual reports, but native phases remain visible.
    if mode != "full-epoch":
        assert len(reports) == len(rows), (mode, reports, rows)
        for report, row in zip(reports, rows):
            assert int(report[8]) == row[3], (mode, report, row)
            if int(report[5]) >= 0:
                assert int(report[5]) == row[2], (mode, report, row)
    print("session-case", mode, len(rows), "/".join(",".join(map(str, row[1:])) for row in rows), sep="\t", flush=True)
    return rows, reports


source = "module hello\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
initial = project("initial", source)
body = project("body", source.replace("  0", "  1"))
rejected = project("rejected", source.replace("  0", "  false"))
cold = [2, 1, 1]
run(initial, initial, expected=[cold, [0, 0, 0]])
run(initial, body, expected=[cold, cold])
for mode in ["compiler", "runtime", "target", "options", "metadata", "metadata-declarations", "metadata-edges", "metadata-manifest"]:
    run(initial, initial, mode, [cold, cold])
for mode in ["corrupt", "missing-code"]:
    run(initial, initial, mode, [cold, [0, 0, 1]])
run(initial, rejected, "recover", [cold, [2, 1, 0], [0, 0, 0]])
for mode in ["attempt-limit", "input-limit", "revision-limit", "bad-config"]:
    run(initial, initial, mode, [cold, [0, 0, 0]])
run(initial, body, "node-limit", [cold, [2, 0, 0], [0, 0, 0]])
run(initial, body, "code-limit", [cold, cold, [0, 0, 0]])
run(initial, initial, "bad-limits", [[0, 0, 0]])
run(initial, initial, "full-epoch", [cold] + [[0, 0, 0]] * 64)
run(initial, initial, "epochs", [cold, [0, 0, 0], cold, [0, 0, 0]], epochs=2)

# Comments, CRLF, relocation and manifest path changes retain exact diagnostics too.
comments = project("comments", "# moved source\n" + source.replace("  0", "  0 # trailing").replace("\n", "\r\n"))
run(initial, comments)
relocated = project("relocated", source,
                    '(project 1 (entry hello) (module hello "moved.slim" (imports) (exports)))\n',
                    **{"moved.slim": source})
run(initial, relocated)

# Observe actual retained fragments, including misses from damaged optional
# metadata. Re-sealed cases exercise structural validation independently of the
# accidental-corruption checksum; no external cache is admitted by this test.
fragment_source = "module hello\n\nfn answer() -> I64:\n  42\n\nfn keep(x: I64) -> I64:\n  x\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
fragment_before = project("fragment-before", fragment_source)
fragment_after = project("fragment-after", fragment_source.replace("  0\n", "  1\n"))
rows, _ = run(fragment_before, fragment_after)
assert rows[0][17:21] == [3, 3, 0, 0] and rows[1][17:21] == [1, 1, 0, 4], rows
fragment_frames = ["epoch", "revision", "owner-epoch", "owner-revision", "owner-file", "owner-slot", "entry-plan", "entry-counted", "entry-duplicate", "slot", "entries", "slots", "prototype-start", "wrapper-start", "body-start", "footer-start", "code-bytes", "prototype-end", "body-end"]
for mode in fragment_frames + ["seal", "prototype-checksum", "body-checksum"]:
    rows, _ = run(fragment_before, fragment_after, "emission-" + mode)
    assert rows[1][17:22] == [3, 3, 0, 0, 0], (mode, rows)
for mode in fragment_frames:
    rows, _ = run(fragment_before, fragment_after, "emission-sealed-" + mode)
    assert rows[1][17:22] == [3, 3, 0, 0, 0], (mode, rows)
for mode in ["prototype-payload", "body-payload", "sealed-prototype-checksum", "sealed-body-checksum"]:
    rows, _ = run(fragment_before, fragment_after, "emission-" + mode)
    assert rows[1][17:21] == [2, 2, 0, 2], (mode, rows)

wrapper_source = "module hello\n\nfn observed(x: I64) -> I64 effects[io]:\n  let tick: I64 = io.monotonic_ms()\n  x\n\n" + "".join(f"fn work{i}(x: I64, y: I64) -> I64 effects[io, partial]:\n  parallel:\n    let a: I64 = observed(x)\n    let b: I64 = observed(y)\n    a + b\n\n" for i in range(2)) + "fn main(args: Vec[Bytes]) -> I64 effects[io, partial]:\n  let first: I64 = work0(0, 0)\n  let second: I64 = work1(0, 0)\n  first + second\n"
wrapper_before = project("wrapper-before", wrapper_source)
wrapper_after = project("wrapper-after", wrapper_source.replace("fn observed", "fn inserted() -> I64:\n  7\n\nfn observed"))
rows, _ = run(wrapper_before, wrapper_after)
assert rows[0][17:21] == [4, 4, 4, 0] and rows[1][17:21] == [1, 1, 0, 12], rows
wrapper_frames = ["wrapper-site", "wrapper-task", "wrapper-duplicate", "wrapper-span-start", "wrapper-span-end", "wrappers", "sites"]
site_fields = ["site-" + field for field in ["site", "first", "second", "join", "first-work", "second-work", "allocates", "explicit", "executable"]]
for mode in wrapper_frames + site_fields + ["wrapper-span-checksum"]:
    rows, _ = run(wrapper_before, wrapper_after, "emission-" + mode)
    assert rows[1][17:22] == [5, 5, 4, 0, 0], (mode, rows)
for mode in wrapper_frames:
    rows, _ = run(wrapper_before, wrapper_after, "emission-sealed-" + mode)
    assert rows[1][17:22] == [5, 5, 4, 0, 0], (mode, rows)
for mode in ["wrapper-payload", "sealed-wrapper-span-checksum"]:
    rows, _ = run(wrapper_before, wrapper_after, "emission-" + mode)
    assert rows[1][17:21] == [1, 1, 1, 11], (mode, rows)

for before, after in [(fragment_before, fragment_after), (wrapper_before, wrapper_after)]:
    run(before, after, "emission-budget")
    run(before, after, "emission-proof")
for field in ["analyzed", "lower-known", "lower", "upper-known", "upper", "total"]:
    rows, _ = run(fragment_before, fragment_after, "emission-derived-range-" + field)
    assert rows[1][17:21] == [2, 2, 0, 2], (field, rows)
for field in ["first", "second", "join", "first-work", "second-work", "allocates", "explicit"]:
    rows, _ = run(wrapper_before, wrapper_after, "emission-sealed-site-" + field)
    assert rows[1][17:21] == [2, 2, 2, 8], (field, rows)
counted_source = fragment_source.replace("fn answer() -> I64:\n  42", "fn answer(x: I64) -> I64 effects[partial]:\n  if x == 3:\n    0\n  else:\n    recur(x + 1)").replace("  x\n", "  x\n\nfn invoke() -> I64 effects[partial]:\n  answer(0)\n")
counted_before = project("fragment-counted-before", counted_source)
# Change only main, so the counted owner would otherwise import.
counted_after = project("fragment-counted-after", counted_source.rsplit("  0\n", 1)[0] + "  1\n")
rows, _ = run(counted_before, counted_after)
assert rows[1][17:21] == [1, 1, 0, 6], rows
for field in ["controller", "start", "bound", "step", "iterations", "base", "body"]:
    rows, _ = run(counted_before, counted_after, "emission-derived-counted-" + field)
    assert rows[1][17:21] == [2, 2, 0, 4], (field, rows)
for field in ["item", "absent"]:
    rows, _ = run(counted_before, counted_after, "emission-derived-counted-" + field)
    assert rows[1][17:22] == [4, 4, 0, 0, 0], (field, rows)

for field in ["function", "local-region", "recursive"]:
    rows, _ = run(fragment_before, fragment_after, "emission-derived-plan-" + field)
    assert rows[1][17:21] == [2, 2, 0, 2], (field, rows)
fragment_alloc = fragment_source.replace("fn answer() -> I64:\n  42", "fn answer() -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(@values, 42)\n  vec.len(values)")
alloc_before = project("fragment-alloc-before", fragment_alloc)
alloc_after = project("fragment-alloc-after", fragment_alloc.rsplit("  0\n", 1)[0] + "  1\n")
for field in ["allocations", "allocation-site", "allocation-region"]:
    rows, _ = run(alloc_before, alloc_after, "emission-derived-plan-" + field)
    assert rows[1][17:21] == [2, 2, 0, 2], (field, rows)

# Rebinding metadata after whole-artifact reuse must preserve later fragment
# eligibility. A repaired C artifact also retains valid metadata for the next edit.
fragment_moved = project("fragment-moved", fragment_source, '(project 1 (entry hello) (module hello "moved.slim" (imports) (exports)))\n', **{"moved.slim": fragment_source})
for mode, middle in [("work", fragment_moved), ("corrupt", fragment_before), ("missing-code", fragment_before)]:
    rows, _ = run(fragment_before, middle, mode, third=fragment_after)
    assert rows[2][17:21] == [1, 1, 0, 4], (mode, rows)
    if mode == "work":
        assert rows[1][3] == 0 and rows[1][17:22] == [0] * 5 and rows[1][22] == 3, rows
rows, _ = run(fragment_before, fragment_before, "emission-seal", third=fragment_after)
assert rows[1][17:] == [0] * 7 and rows[2][17:22] == [3, 3, 0, 0, 0], rows

# Explicit sites and inferred pure sites need not have function-lexical global
# order. Import wrappers in the current site's order, including selection limits.
mixed = wrapper_source.replace("fn main", "fn count(index: I64) -> I64 effects[partial]:\n  if index <= 0:\n    0\n  else:\n    recur(index - 1)\n\nfn automatic() -> I64 effects[partial]:\n  let a: I64 = count(2000000)\n  let b: I64 = count(2000000)\n  a + b\n\nfn main")
mixed_before = project("fragment-mixed-before", mixed)
mixed_after = project("fragment-mixed-after", mixed.replace("fn observed", "fn inserted() -> I64:\n  7\n\nfn observed"))
rows, _ = run(mixed_before, mixed_after)
assert rows[0][19] == 6 and rows[1][17:21] == [1, 1, 0, 18], rows
# An unchanged automatic caller consumes the callee's recurrence work. Changing
# that body must invalidate the caller even when ordinary typing imports it.
for step, produced_wrappers in [(2, 2), (3, 0)]:
    changed = project(f"fragment-mixed-step-{step}", mixed.replace("recur(index - 1)", f"recur(index - {step})"))
    rows, _ = run(mixed_before, changed)
    assert rows[1][2] == 1 and rows[1][17:21] == [2, 2, produced_wrappers, 12], (step, rows)

for sites in [63, 64, 65]:
    header = "module hello\n\nfn observed(x: I64) -> I64 effects[io]:\n  let tick: I64 = io.monotonic_ms()\n  x\n\n"
    def worker(name):
        return f"fn {name}(x: I64, y: I64) -> I64 effects[io, partial]:\n  parallel:\n    let a: I64 = observed(x)\n    let b: I64 = observed(y)\n    a + b\n\n"
    functions = "".join(worker(f"work{i}") for i in range(sites))
    main0 = "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
    before = project(f"fragment-sites-{sites}", header + functions + main0)
    after = project(f"fragment-sites-{sites}-insert", header + worker("inserted") + functions + main0)
    rows, _ = run(before, after)
    # The ordinary graph retains 64 functions: observed plus 63 workers.
    # Inserting a worker moves work62 outside that complete graph. Its old
    # executable site disappears, despite identical checked body/plan/ranges.
    selected = min(sites, 63)
    produced = 2
    imported_sites = 62
    imported_functions = sites + 3 - produced
    assert rows[0][19] == 2 * selected, (sites, rows)
    assert rows[1][17:21] == [produced, produced, 2, 2 * (imported_functions + imported_sites)], (sites, rows)

# Reach the independent 64-site limit inside one function, without exhausting
# the graph's function budget. A 65th candidate remains serial in clean output.
for sites in [63, 64, 65]:
    header = "module hello\n\nfn count(index: I64) -> I64 effects[partial]:\n  if index <= 0:\n    0\n  else:\n    recur(index - 1)\n\n"
    chain = "fn many() -> I64 effects[partial]:\n" + "".join(f"  let first{i}: I64 = count(1000000)\n  let second{i}: I64 = count(1000000)\n" for i in range(sites)) + f"  first{sites - 1} + second{sites - 1}\n\n"
    source_many = header + chain + "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
    before = project(f"fragment-chain-{sites}", source_many)
    after = project(f"fragment-chain-{sites}-insert", source_many.replace("fn count", "fn inserted() -> I64:\n  7\n\nfn count"))
    rows, _ = run(before, after)
    wrappers = 2 * min(sites, 64)
    assert rows[0][19] == wrappers and rows[1][17:21] == [1, 1, 0, 6 + wrappers], (sites, rows)

# Execute C obtained from a retained update. Byte equality is checked first;
# then the unchanged runtime's serial, POSIX, declined-spawn and join paths run.
for label, before, after in [("wrappers", wrapper_before, wrapper_after), ("mixed", mixed_before, mixed_after)]:
    expected = subprocess.run([compiler, str(after)], capture_output=True, check=True).stdout
    outputs = []
    for binary in [ordinary, observed]:
        result = subprocess.run([str(binary), str(before), str(after), "emission-output"], capture_output=True)
        assert result.returncode == 0 and not result.stderr and result.stdout == expected + b"\n", (label, result.returncode, result.stderr[:200])
        outputs.append(result.stdout)
    cfile = root / (label + ".c")
    cfile.write_bytes(outputs[0])
    executable = root / (label + "-native")
    for flags in [[], ["-DSLIM_POSIX_WORKERS=1", "-pthread"]]:
        subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", "-DSLIM_PARALLEL=1", *flags, "-I", "runtime", str(cfile), "runtime/slim_rt.c", "-o", str(executable)], capture_output=True, check=True)
        for fail in ["0", "1"]:
            result = subprocess.run([str(executable)], capture_output=True, env=dict(os.environ, SLIM_TASK_FAIL_AT=fail))
            assert result.returncode == 0 and not result.stdout and not result.stderr, (label, flags, fail, result)
        if flags:
            result = subprocess.run([str(executable)], capture_output=True, env=dict(os.environ, SLIM_TASK_JOIN_FAIL_AT="1"))
            assert result.returncode == 70 and result.stderr == b"SLIM runtime trap: injected structured task join failure\n", (label, result)
    print("session-retained-native", label, "exact", sep="\t", flush=True)

# Real file replacement after first complete capture; compare to an untouched copy.
captured = project("captured", source)
replacement = captured.parent / "program.slim"
env = dict(os.environ, SLIM_SESSION_REPORT=str(report_path),
           SLIM_SESSION_CAPTURE_REPLACE=str(replacement),
           SLIM_SESSION_CAPTURE_SOURCE=str(rejected.parent / "program.slim"))
p = subprocess.run([str(observed), str(captured), str(initial), "captured"], env=env, capture_output=True)
assert p.returncode == 0 and not p.stderr, (p.returncode, p.stdout[:400], p.stderr[:400])
assert replacement.read_text() == (rejected.parent / "program.slim").read_text()
assert [list(map(int, line.split("\t"))) for line in report_path.read_text().splitlines()[1:]] == [[0, 2, 1, 1, 2, 0, 1, 0, 0, 1, 1, 4, 5, 0, 1, 3, 4, 1, 1, 0, 0, 0, 1, 4], [1] + [0] * 23]
print("session-captured-input\texact\tfile-replaced-before-preparation", flush=True)

# Global interface and body-derived facts always compare against a clean preparation.
manifest = '(project 1 (entry hello) (module data "data.slim" (imports) (exports helper)) (module hello "program.slim" (imports data) (exports)))\n'
main = source.replace("  0", "  data.helper(1)")
helper = "module data\n\nfn helper(value: I64) -> I64:\n  value + 1\n"
pair = project("pair", main, manifest, **{"data.slim": helper})
for name, new_main, new_helper in [
    ("helper-body", main, helper.replace("+ 1", "+ 2")),
    ("argument", main.replace("helper(1)", "helper(2)"), helper),
    ("interface", main.replace("helper(1)", "helper(true)"), "module data\n\nfn helper(value: Bool) -> I64:\n  if value:\n    1\n  else:\n    0\n"),
    ("effect", main.replace("-> I64:", "-> I64 effects[io]:"), helper.replace("-> I64:", "-> I64 effects[io]:").replace("  value + 1", "  io.print_i64(value)\n  value + 1")),
    ("insertion", main, helper + "\nfn unused() -> I64:\n  0\n"),
]:
    changed = project(name, new_main, manifest, **{"data.slim": new_helper})
    run(pair, changed)
    if name == "insertion":
        run(changed, pair)
# The literal domain is checked on changed declarations before snapshot publication.
for name, literal in [("minimum", "-9223372036854775808"), ("decimal", "0009"),
                      ("padded-minimum", "-0009223372036854775808")]:
    changed = project("literal-" + name, main, manifest,
                      **{"data.slim": helper.replace("value + 1", literal)})
    run(pair, changed)
    for outside in ["9223372036854775808", "-0009223372036854775809"]:
        rejected_literal = project("literal-outside-" + name + str(len(outside)), main, manifest,
                                   **{"data.slim": helper.replace("value + 1", outside)})
        run(changed, rejected_literal, "recover")

plan_changed = project("plan-main-edit", main.replace("helper(1)", "helper(2)"), manifest, **{"data.slim": helper})
for mode in ["plan-entries", "plan-slots", "plan-values", "plan-tag", "plan-node", "plan-byte", "plan-owner", "plan-source", "plan-count", "plan-start", "plan-allocation-start", "plan-destruction-count"]:
    rows, _ = run(pair, plan_changed, mode)
    assert rows[1][2] == 1 and rows[1][6:8] == [2, 0], (mode, rows)
# A main argument edit changes the helper's later-pass input key even with the
# same helper source. Damaged early-pass history adds a third producer execution.
rows, _ = run(pair, plan_changed)
assert rows[1][10:12] == [2, 8], rows
for mode in ["range-wrapper", "range-history", "range-heads", "range-head", "range-entries", "range-keys", "range-key", "range-facts", "range-tag", "range-node", "range-owner", "range-source", "range-prefix", "range-slice", "range-next", 'range-key-start', 'range-key-count', 'range-fact-start', 'range-recurrence-start', 'range-recurrence-count', 'range-counted-slice-start', 'range-counted-count', 'range-key-node', 'range-key-entry', 'range-key-lower', 'range-key-upper', 'range-fact-lower', 'range-fact-upper', 'range-fact-order']:
    rows, _ = run(pair, plan_changed, mode)
    assert rows[1][10:12] == [3, 7], (mode, rows)
run(pair, pair, "range-budget")
guarded_helper = "module data\n\nfn helper(value: I64) -> I64:\n  if value <= 0:\n    0\n  else:\n    value + 1\n"
guarded = project("range-guarded", main, manifest, **{"data.slim": guarded_helper})
guarded_changed = project("range-guarded-changed", main.replace("helper(1)", "helper(2)"), manifest, **{"data.slim": guarded_helper})
for mode in ["range-refinements", "range-parent", "range-refinement-node", "range-refinement-value"]:
    rows, _ = run(guarded, guarded_changed, mode)
    assert rows[1][10:12] == [3, 7], (mode, rows)
run(guarded, guarded, "range-budget")
run(guarded, guarded, "range-storage")
for mode in ["range-refinement-start", "range-refinement-count"]:
    rows, _ = run(guarded, guarded_changed, mode)
    assert rows[1][10:12] == [3, 7], (mode, rows)

# Proof records carry both local nodes and numeric fields. A source-preserving
# helper query must miss when a saved proof domain or pool is damaged.
recurrence_helper = "module data\n\nfn helper(value: I64) -> I64 effects[partial]:\n  if value <= 0:\n    0\n  else:\n    recur(value - 1)\n"
counted_helper = "module data\n\nfn helper(value: I64) -> I64 effects[partial]:\n  if value == 3:\n    0\n  else:\n    recur(value + 1)\n"
proof_main = main.replace("-> I64:", "-> I64 effects[partial]:").replace("helper(1)", "helper(0)")
proof = project("range-recurrence", proof_main, manifest, **{"data.slim": recurrence_helper})
proof_changed = project("range-recurrence-changed", proof_main.replace("helper(0)", "helper(0) + 0"), manifest, **{"data.slim": recurrence_helper})
clean_rows, _ = run(proof, proof_changed)
for mode in ['range-recurrences', 'range-recurrence-item', 'range-recurrence-position', 'range-recurrence-bound', 'range-recurrence-step']:
    rows, _ = run(proof, proof_changed, mode)
    assert rows[1][10] > clean_rows[1][10], (mode, clean_rows, rows)
run(proof, proof, "range-budget")
# Insertion and reordering relocate proof node fields while preserving scalars.
shifted = project("range-recurrence-shift", proof_main, manifest, **{"data.slim": recurrence_helper.replace("fn helper", "fn earlier() -> I64:\n  7\n\nfn helper")})
run(proof, shifted)
run(shifted, proof)
proof = project("range-counted", proof_main, manifest, **{"data.slim": counted_helper})
proof_changed = project("range-counted-changed", proof_main.replace("helper(0)", "helper(0) + 0"), manifest, **{"data.slim": counted_helper})
clean_rows, _ = run(proof, proof_changed)
for mode in ['range-counted', 'range-counted-item', 'range-counted-controller', 'range-counted-base', 'range-counted-body', 'range-counted-start', 'range-counted-bound', 'range-counted-step', 'range-counted-iterations']:
    rows, _ = run(proof, proof_changed, mode)
    assert rows[1][10] > clean_rows[1][10], (mode, clean_rows, rows)
run(proof, proof, "range-budget")
# Insertion and reordering relocate proof node fields while preserving scalars.
shifted = project("range-counted-shift", proof_main, manifest, **{"data.slim": counted_helper.replace("fn helper", "fn earlier() -> I64:\n  7\n\nfn helper")})
run(proof, shifted)
run(shifted, proof)
for bound in [0, 1, 15, 16, 17]:
    helper_source = counted_helper.replace("value == 3", f"value == {bound}")
    proof = project(f"range-counted-bound-{bound}", proof_main, manifest, **{"data.slim": helper_source})
    changed = project(f"range-counted-bound-{bound}-changed", proof_main.replace("helper(0)", "helper(0) + 0"), manifest, **{"data.slim": helper_source})
    run(proof, changed, "range-counted-positive" if bound <= 16 else "range-counted-unknown")

for literal in [-1000000001, -1000000000, -999999999, 999999999, 1000000000, 1000000001]:
    before = project(f"range-domain-{literal}", main.replace("helper(1)", f"helper({literal})"), manifest, **{"data.slim": helper})
    run(pair, before)
    run(before, pair)

for depth in [3, 4, 5, 6]:
    chain = "module data\n\nfn f_0(x: I64) -> I64:\n  x + 1\n\n" + "".join(f"fn f_{i}(x: I64) -> I64:\n  f_{i - 1}(x + 1)\n\n" for i in range(1, depth + 1))
    app = main.replace("helper(1)", f"f_{depth}(1)")
    layout = manifest.replace("exports helper", f"exports f_{depth}")
    before = project(f"range-depth-{depth}", app, layout, **{"data.slim": chain})
    after = project(f"range-depth-{depth}-changed", app.replace(f"f_{depth}(1)", f"f_{depth}(2)"), layout, **{"data.slim": chain})
    run(before, after, "range-depth-known" if depth == 3 else "range-depth-unknown")
for count in [31, 32, 33]:
    functions = "".join(f"fn f_{i}(x: I64) -> I64:\n  if x <= 0:\n    0\n  else:\n    x + 1\n\n" for i in range(count))
    app = main.replace("helper(1)", "f_0(1)")
    layout = manifest.replace("exports helper", "exports f_0")
    before = project(f"range-refinements-{count}", app, layout, **{"data.slim": "module data\n\n" + functions})
    inserted = "fn inserted(x: I64) -> I64:\n  if x <= 0:\n    0\n  else:\n    x + 1\n\n"
    after = project(f"range-refinements-{count}-insert", app, layout, **{"data.slim": "module data\n\n" + inserted + functions})
    rows, _ = run(before, after, "range-refinement-boundary")
    assert rows[1][10] == 34, (count, rows)

for values in [63, 64, 65]:
    locals_source = "".join(f"  let v{i}: I64 = {i}\n" for i in range(values - 1))
    wide_helper = "module data\n\nfn helper(value: I64) -> I64:\n" + locals_source + "  value\n"
    before = project(f"plan-values-{values}", main, manifest, **{"data.slim": wide_helper})
    prefix = "fn unused() -> I64:\n  3\n\n"
    shifted_helper = wide_helper.replace("fn helper", prefix + "fn helper")
    shifted = project(f"plan-values-{values}-shift", main, manifest, **{"data.slim": shifted_helper})
    rows, _ = run(before, shifted)
    assert rows[1][6:8] == [1, 2], (values, rows)
    reversed_helper = wide_helper + "\n" + prefix
    reordered = project(f"plan-values-{values}-reorder", main, manifest, **{"data.slim": reversed_helper})
    rows, _ = run(shifted, reordered)
    assert rows[1][6:8] == [0, 3], (values, rows)
    if values == 65:
        changed = project("plan-tail-tag-edit", main.replace("helper(1)", "helper(2)"), manifest, **{"data.slim": wide_helper})
        rows, _ = run(before, changed, "plan-tail-tag")
        assert rows[1][6:8] == [2, 0], rows

invalid_argument = project("invalid-argument", main.replace("helper(1)", "helper(false)"), manifest, **{"data.slim": helper})
run(pair, invalid_argument, "recover")
run(pair, pair, "plan-budget")
allocation_helper = "module data\n\nfn helper(value: I64) -> I64 effects[alloc, partial]:\n  let values: Vec[I64] = vec.new()\n  vec.push(@values, value)\n  vec.len(values)\n"
allocation_main = main.replace("-> I64:", "-> I64 effects[alloc, partial]:")
allocation_pair = project("allocation-plan-budget", allocation_main, manifest, **{"data.slim": allocation_helper})
run(allocation_pair, allocation_pair, "plan-budget")
allocation_changed = project("allocation-plan-edited", allocation_main.replace("helper(1)", "helper(2)"), manifest, **{"data.slim": allocation_helper})
for mode in ["plan-allocations", "plan-destructions", "plan-allocation-node", "plan-destruction-node"]:
    rows, _ = run(allocation_pair, allocation_changed, mode)
    assert rows[1][6:8] == [2, 0], (mode, rows)

layout_manifest = manifest.replace("exports helper", "exports Leaf Node helper")
layout_source = "module data\n\nstruct Leaf:\n  value: I64\n\nstruct Node:\n  leaf: Leaf\n\nfn helper(value: Node) -> I64:\n  0\n"
layout_before = project("plan-layout-before", source, layout_manifest, **{"data.slim": layout_source})
layout_after = project("plan-layout-after", source, layout_manifest, **{"data.slim": layout_source.replace("value: I64", "value: Vec[I64]")})
rows, _ = run(layout_before, layout_after)
assert rows[1][6:8] == [1, 1], rows
mode_after = project("plan-mode-after", source, layout_manifest, **{"data.slim": layout_source.replace("value: I64", "value: Vec[I64]").replace("helper(value: Node)", "helper(value: @Node)")})
rows, _ = run(layout_after, mode_after)
assert rows[1][6:8] == [1, 1], rows

for mode in ["history-token-fit", "history-token-under"]:
    run(pair, pair, mode, expected=[[3, 2, 1], [0, 0, 0]])
parameters = ", ".join(f"v{i}: @Vec[I64]" for i in range(32))
lexical_helper = f"module data\nfn helper({parameters}) -> I64:\n  0\n"
lexical_project = project("lexical-history", source, manifest, **{"data.slim": lexical_helper})
for mode in ["history-lexeme-fit", "history-lexeme-under"]:
    run(lexical_project, lexical_project, mode, expected=[[3, 2, 1], [0, 0, 0]])

renamed = project("renamed", main.replace("helper", "renamed"), manifest.replace("exports helper", "exports renamed"), **{"data.slim": helper.replace("helper", "renamed")})
run(pair, renamed)
module_renamed = project("module-renamed", main.replace("data.", "dep."), manifest.replace("module data", "module dep").replace("imports data", "imports dep"), **{"data.slim": helper.replace("module data", "module dep")})
run(pair, module_renamed)
run(initial, pair)
run(pair, initial)
rows, reports = run(pair, initial, "reinsert")
assert rows[2][4] == 4, rows
module_order = '(project 1 (entry hello) (module hello "program.slim" (imports data) (exports)) (module data "data.slim" (imports) (exports helper)))\n'
module_reorder = project("module-reorder", main, module_order, **{"data.slim": helper})
run(pair, module_reorder, "recover", expected=[[3, 2, 1], [0, 0, 0], [0, 0, 0]])
with_extra = manifest.replace('(module data', '(module aux "aux.slim" (imports) (exports)) (module data')
module_insert = project("module-insert", main, with_extra, **{"data.slim": helper, "aux.slim": "module aux\nfn idle() -> I64:\n  0\n"})
run(pair, module_insert)
rows, reports = run(module_insert, pair, "reinsert")
assert rows[2][4] == 2, rows
reordered_before = project("reordered-before", main, manifest, **{"data.slim": helper + "\nfn idle() -> I64:\n  0\n"})
reordered_after = project("reordered-after", main, manifest, **{"data.slim": helper.replace("fn helper", "fn idle() -> I64:\n  0\n\nfn helper")})
run(reordered_before, reordered_after)
recursive_main = main.replace("-> I64:", "-> I64 effects[partial]:")
recursive_helper = helper.replace("-> I64:", "-> I64 effects[partial]:")
recursive_before = project("recursive-before", recursive_main, manifest, **{"data.slim": recursive_helper})
recursive = project("recursive", recursive_main, manifest, **{"data.slim": recursive_helper.replace("value + 1", "recur(value)")})
run(recursive_before, recursive)
copyability = "module copyability\n\nstruct Leaf:\n  value: I64\n\nstruct Box:\n  leaf: Leaf\n\nfn inspect(box: Box) -> I64:\n  0\n\nfn idle() -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let box: Box = Box(leaf: Leaf(value: 0))\n  inspect(box)\n"
copy_before = project("copy-before", copyability)
copy_after = project("copy-after", copyability.replace("value: I64", "value: Vec[I64]").replace("value: 0", "value: vec.new()"))
run(copy_before, copy_after)
owned = "module modes\n\nstruct Leaf:\n  values: Vec[I64]\n\nstruct Box:\n  leaf: Leaf\n\nfn take(value: ^Box) -> I64:\n  0\n\nfn relay(value: ^Box) -> I64:\n  take(^value)\n\nfn idle() -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()\n  relay(^Box(leaf: Leaf(values: values)))\n"
owned_before = project("owned-before", owned)
exclusive = project("exclusive", owned.replace("take(value: ^Box)", "take(value: @Box)").replace("take(^value)", "take(@value)"))
run(owned_before, exclusive)
missing_move = project("missing-move", owned.replace("take(^value)", "take(value)"))
run(owned_before, missing_move, "recover")

# Project rejects retain the ordinary diagnostic ordering, including read failures.
for name, content, layout, modules in [
    ("wrong-entry", main, manifest.replace("entry hello", "entry absent"), {"data.slim": helper}),
    ("missing-export", main, manifest.replace("exports helper", "exports absent"), {"data.slim": helper}),
    ("missing-import", main, manifest.replace("imports data", "imports"), {"data.slim": helper}),
    ("missing-module", main, manifest, {}),
    ("wrong-identity", main.replace("module hello", "module wrong"), manifest, {"data.slim": helper}),
    ("syntax-before-read", "module app\n\nfn main(\n", '(project 1 (entry app) (module app "program.slim" (imports data) (exports)) (module data "data.slim" (imports) (exports helper)))\n', {}),
    ("invalid-manifest", main, '(project 9)\n', {}),
]:
    changed = project(name, content, layout, **modules)
    clean = subprocess.run([compiler, "check", str(changed)], capture_output=True)
    assert clean.returncode != 0, name
    for binary in [ordinary, observed]:
        candidate = subprocess.run([str(binary), str(pair), str(changed), "check"], capture_output=True)
        assert (clean.returncode, clean.stdout, clean.stderr) == (candidate.returncode, candidate.stdout, candidate.stderr), (name, clean.stdout[:400], candidate.stdout[:400], candidate.stderr[:400])
    print("session-rejection", name, clean.returncode, sep="\t", flush=True)

fixtures = sorted(Path("conformance/pass").glob("*.slim")) + sorted(Path("benchmarks/challenges").glob("*/program.slim")) + [Path("tests/fixtures/stable_codegen.slim")]
if not full:
    fixtures = [Path("examples/hello.slim")]
for fixture in fixtures:
    print("session-corpus", fixture, sep="\t", flush=True)
    wrapped = project("wrapped", fixture.read_text())
    rows, reports = run(wrapped, wrapped)
    assert rows[1][1:] == [0] * 23, fixture
    assert reports[1][2:4] == reports[0][2:4], fixture
    shifted = project("wrapped-trivia", "# force preparation of unchanged canonical source\n" + fixture.read_text())
    rows, _ = run(wrapped, shifted)
    assert rows[1][6:8] == [0, rows[0][6]], (fixture, rows)
    assert rows[1][10] == 0 and rows[1][11] == 5 * rows[0][6], (fixture, rows)
    original = fixture.read_text()
    header = re.search(r"(?m)^module [^\n]+\n", original)
    assert header and "fn slim_fragment_padding(" not in original, fixture
    padded = project("wrapped-padding", original[:header.end()] + "\nfn slim_fragment_padding() -> I64:\n  7\n\n" + original[header.end():])
    rows, _ = run(wrapped, padded, "reinsert")
    assert rows[1][2] == 1 and rows[2][2] == 0, (fixture, rows)
print(f"session-differential\t{len(fixtures)}\tcomplete-prepared-fields-and-C", flush=True)

for size in ([1, 2, 125, 250, 500, 1000, 2000, 4000] if full else [1, 2]):
    data = "module data\n\n" + "".join(f"fn f_{i}(x: I64) -> I64:\n  x\n\n" for i in range(size))
    app = "module hello\n\nfn main(args: Vec[Bytes]) -> I64:\n  data.f_0(1)\n"
    layout = '(project 1 (entry hello) (module data "data.slim" (imports) (exports f_0)) (module hello "program.slim" (imports data) (exports)))\n'
    before = project(f"geo-{size}-before", app, layout, **{"data.slim": data})
    after = project(f"geo-{size}-after", app, layout, **{"data.slim": data.replace("  x\n", "  42\n", 1)})
    for kind, updated in [("unchanged", before), ("body", after)]:
        expected = [[3, size + 1, 1], [0, 0, 0] if kind == "unchanged" else [3, 1, 1]]
        rows, reports = run(before, updated, expected=expected)
        assert rows[0][4:8] == [2 * (size + 1), 0, size + 1, 0], (size, rows)
        assert rows[0][10:12] == [size + 2, 4 * size + 3], (size, rows)
        if kind == "unchanged":
            assert rows[1][4:8] == [0, 0, 0, 0], (size, rows)
        else:
            assert rows[1][4] == 2 and rows[1][5] > 0, (size, rows)
            assert rows[1][6:8] == [1, size], (size, rows)
            assert rows[1][10:12] == [2, 5 * (size + 1) - 2], (size, rows)
        if kind == "body":
            assert rows[1][17:21] == [1, 1, 0, 2 * size], (size, rows)
        else:
            assert rows[1][17:22] == [0] * 5, (size, rows)
        imports = 0 if kind == "unchanged" else 15 * size + 7
        assert int(reports[1][7]) == imports, (size, kind, reports)
        print("session-geometric", size, kind, reports[1][5], reports[1][6], imports, rows[1][4], rows[1][5], rows[1][6], rows[1][7], sep="\t", flush=True)
    for mode in ["parse-metadata", "parse-modules", "parse-module-names", "parse-module-edges"]:
        rows, reports = run(before, after, mode, expected=[[3, size + 1, 1], [3, 1, 1]])
        assert rows[1][4] == size + 2 and rows[1][5] > 0, (size, mode, rows)
    if size <= 2:
        run(before, after, "node-limit", expected=[[3, size + 1, 1], [3, 0, 0], [0, 0, 0]])

# Dense functions exercise pooled rows, not just one saved scalar per function.
for size in ([32, 128, 512] if full else [2]):
    locals_source = "".join(f"  let v{i}: I64 = {i}\n" for i in range(63))
    data = "module data\n\n" + "".join(f"fn f_{i}(x: I64) -> I64:\n" + locals_source + "  x\n\n" for i in range(size))
    app = "module hello\n\nfn main(args: Vec[Bytes]) -> I64:\n  data.f_0(1)\n"
    layout = '(project 1 (entry hello) (module data "data.slim" (imports) (exports f_0)) (module hello "program.slim" (imports data) (exports)))\n'
    before = project(f"dense-{size}-before", app, layout, **{"data.slim": data})
    after = project(f"dense-{size}-after", app, layout, **{"data.slim": data.replace("  x\n", "  42\n", 1)})
    rows, _ = run(before, after, expected=[[3, size + 1, 1], [3, 1, 1]])
    assert rows[0][6:8] == [size + 1, 0] and rows[1][6:8] == [1, size], (size, rows)
    print("session-dense", size, 64, rows[1][6], rows[1][7], sep="\t", flush=True)

# Counted proof storage is not capped by the 64-row reporting limit. Every
# cursor lookup does constant record work; cold N and warm 2N lookups are exact.
for size in ([63, 64, 65, 125, 250, 500, 1000, 2000] if full else [63, 64, 65]):
    functions = "".join(f"fn counted_{i}(index: I64) -> I64 effects[partial]:\n  if index == 3:\n    42\n  else:\n    recur(index + 1)\n\nfn call_{i}() -> I64 effects[partial]:\n  counted_{i}(0)\n\n" for i in range(size))
    counted = "module hello\n\n" + functions + "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
    before = project(f"emission-counted-{size}", counted)
    after = project(f"emission-counted-{size}-edit", counted.rsplit("  0\n", 1)[0] + "  1\n")
    rows, _ = run(before, after)
    total = 2 * size + 1
    assert rows[0][22] == total and rows[1][22] == 2 * total, (size, rows)
    assert rows[1][17:21] == [1, 1, 0, 4 * size], (size, rows)
    clean = subprocess.run([compiler, str(before)], capture_output=True, check=True).stdout
    assert clean.count(b") do {") == 3 * size, size
    print("session-counted-work", size, rows[0][22], rows[1][22], rows[1][21], sep="\t", flush=True)

if sys.argv[2] == "full":
    run_faults(root, initial, initial, "fault")
    run_faults(root, pair, plan_changed, "update-fault", require_import=True)
    run_faults(root, fragment_before, fragment_after, "fragment-fault", require_fragments=True)
