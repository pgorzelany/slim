"""Differential host driver; all semantics execute in the production SLIM probe."""
from pathlib import Path
import os
import re
import subprocess
import sys

root = Path(sys.argv[1])
full = sys.argv[2] == "full"
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


def run(before, after, mode="work", expected=None, epochs=1):
    args = [str(before), str(after), mode]
    plain = subprocess.run([str(ordinary), *args], capture_output=True)
    report_path.unlink(missing_ok=True)
    native = subprocess.run([str(observed), *args], capture_output=True,
                            env=dict(os.environ, SLIM_SESSION_REPORT=str(report_path)))
    assert plain.returncode == native.returncode == 0, (mode, plain.returncode, native.returncode, plain.stdout[:400], native.stderr[:400])
    assert plain.stdout == native.stdout and plain.stderr == native.stderr, mode
    lines = report_path.read_text().splitlines()
    assert lines[0] == f"slim-session\t2\texact\t1000000000\t{epochs}", lines[0]
    rows = [list(map(int, line.split("\t"))) for line in lines[1:]]
    assert all(row[0] == i and len(row) == 6 for i, row in enumerate(rows)), rows
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

# Real file replacement after first complete capture; compare to an untouched copy.
captured = project("captured", source)
replacement = captured.parent / "program.slim"
env = dict(os.environ, SLIM_SESSION_REPORT=str(report_path),
           SLIM_SESSION_CAPTURE_REPLACE=str(replacement),
           SLIM_SESSION_CAPTURE_SOURCE=str(rejected.parent / "program.slim"))
p = subprocess.run([str(observed), str(captured), str(initial), "captured"], env=env, capture_output=True)
assert p.returncode == 0 and not p.stderr, (p.returncode, p.stdout[:400], p.stderr[:400])
assert replacement.read_text() == (rejected.parent / "program.slim").read_text()
assert report_path.read_text().splitlines()[1:] == ["0\t2\t1\t1\t2\t0", "1\t0\t0\t0\t0\t0"]
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

invalid_argument = project("invalid-argument", main.replace("helper(1)", "helper(false)"), manifest, **{"data.slim": helper})
run(pair, invalid_argument, "recover")
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

fixtures = sorted(Path("conformance/pass").glob("*.slim")) + sorted(Path("benchmarks/challenges").glob("*/program.slim"))
if not full:
    fixtures = [Path("examples/hello.slim")]
for fixture in fixtures:
    wrapped = project("wrapped", fixture.read_text())
    rows, reports = run(wrapped, wrapped)
    assert rows[1][1:] == [0, 0, 0, 0, 0], fixture
    assert reports[1][2:4] == reports[0][2:4], fixture
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
        assert rows[0][4:] == [2 * (size + 1), 0], (size, rows)
        if kind == "unchanged":
            assert rows[1][4:] == [0, 0], (size, rows)
        else:
            assert rows[1][4] == 2 and rows[1][5] > 0, (size, rows)
        imports = 0 if kind == "unchanged" else 15 * size + 7
        assert int(reports[1][7]) == imports, (size, kind, reports)
        print("session-geometric", size, kind, reports[1][5], reports[1][6], imports, rows[1][4], rows[1][5], sep="\t", flush=True)
    for mode in ["parse-metadata", "parse-modules", "parse-module-names", "parse-module-edges"]:
        rows, reports = run(before, after, mode, expected=[[3, size + 1, 1], [3, 1, 1]])
        assert rows[1][4] == size + 2 and rows[1][5] > 0, (size, mode, rows)
    if size <= 2:
        run(before, after, "node-limit", expected=[[3, size + 1, 1], [3, 0, 0], [0, 0, 0]])

if full:
    failed = succeeded = 0
    for ordinal in range(1, 2049):
        args = [str(initial), str(initial), "work"]
        env = dict(os.environ, SLIM_ALLOC_FAIL_AT=str(ordinal))
        plain = subprocess.run([str(ordinary), *args], capture_output=True, env=env)
        report_path.unlink(missing_ok=True)
        native = subprocess.run([str(observed), *args], capture_output=True,
                                env=dict(env, SLIM_SESSION_REPORT=str(report_path)))
        assert (plain.returncode, plain.stdout, plain.stderr) == (native.returncode, native.stdout, native.stderr), (ordinal, plain.returncode, native.returncode, native.stderr[:400])
        assert plain.returncode in (0, 71), (ordinal, plain.returncode, plain.stderr[:400])
        if plain.returncode == 71:
            assert not plain.stdout, (ordinal, plain.stdout[:400])
            failed += 1
        else:
            succeeded += 1
        lines = report_path.read_text().splitlines()
        assert 1 <= len(lines) <= 3, ordinal
        assert lines[0].split("\t")[:2] == ["slim-session", "2"], ordinal
        for i, line in enumerate(lines[1:]):
            row = list(map(int, line.split("\t")))
            assert len(row) == 6 and row[0] == i and all(0 <= n <= 1000000000 for n in row[1:]), ordinal
        print("session-fault", ordinal, plain.returncode, len(lines)-1, sep="\t", flush=True)
    assert failed > 0 and succeeded > 0, (failed, succeeded)
    print("session-fault-summary", failed, succeeded, sep="\t", flush=True)
