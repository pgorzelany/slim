"""Differential observations of the production canonical parser; no host semantics."""
from pathlib import Path
import os
import subprocess
import sys

root = Path(sys.argv[1])
full = sys.argv[2] == "full"
ordinary = root / "ordinary"
observed = root / "observed"
report = root / "report.tsv"


def write(name, source):
    path = root / (name + ".slim")
    path.write_bytes(source.encode())
    return path


def run(before, after, mode="work", expected=None):
    args = [str(before), str(after), mode]
    plain = subprocess.run([str(ordinary), *args], capture_output=True)
    report.unlink(missing_ok=True)
    native = subprocess.run([str(observed), *args], capture_output=True,
                            env=dict(os.environ, SLIM_PARSE_REPORT=str(report)))
    assert plain.returncode == native.returncode == 0, (args, plain.returncode, native.returncode, plain.stdout[:400], native.stderr[:400])
    assert (plain.stdout, plain.stderr) == (native.stdout, native.stderr), args
    work = list(map(int, native.stdout.split()))
    lines = report.read_text().splitlines()
    assert lines[0].startswith("slim-parse\t1\texact\t1000000000\t"), lines
    rows = [list(map(int, line.split("\t"))) for line in lines[1:]]
    assert len(rows) == 2 and rows[0][0] == 0 and rows[1][0] == 1, rows
    if work[0] >= 0:
        assert work[0] == rows[1][1], (work, rows)
    assert work[2] == rows[1][2], (work, rows)
    if expected is not None:
        assert work[:2] == expected, (args, work, expected)
    return work, rows


source = "module sample\n\n" + "".join(f"fn helper_{i}() -> I64:\n  {i}\n\n" for i in range(10)) + "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
initial = write("initial", source)
for name, text, expected in [
    ("unchanged", source, [0, 11]),
    ("body", source.replace("  5\n", "  6\n"), [1, 10]),
    ("insert", source.replace("fn helper_5", "fn added() -> I64:\n  4\n\nfn helper_5"), [1, 11]),
    ("kind", source.replace("fn helper_5() -> I64:\n  5", "struct helper_5:\n  field: I64"), [2, 9]),
    ("rename", source.replace("helper_5", "renamed"), [1, 10]),
    ("module", source.replace("module sample", "module changed"), [11, 0]),
    ("leading-comment", "# relocated\n" + source, [0, 11]),
    ("boundary", source.replace("  5\n\nfn helper_6", "  5 fn helper_6"), None),
    ("delete", source.replace("fn helper_5() -> I64:\n  5\n\n", ""), [0, 10]),
    ("reorder", source.replace("fn helper_4() -> I64:\n  4\n\nfn helper_5() -> I64:\n  5", "fn helper_5() -> I64:\n  5\n\nfn helper_4() -> I64:\n  4"), [0, 11]),
    ("crlf", source.replace("\n", "\r\n"), None),
    ("last-trivia", source + "# final comment\n", [0, 11]),
]:
    after = write(name, text)
    work, rows = run(initial, after, expected=expected)
    print("parse-case", name, *work, sep="\t", flush=True)
for mode in ["missing-tokens", "missing-lexemes", "missing-declarations", "missing-names", "missing-edges", "config", "epoch"]:
    work, rows = run(initial, initial, mode, [11, 0])
    print("parse-case", mode, *work, sep="\t", flush=True)
for mode in ["invalid-owner", "invalid-config"]:
    work, rows = run(initial, initial, mode, [-1, 0])
    print("parse-case", mode, *work, sep="\t", flush=True)

for mode, expected in [("prefix", [0, 11]), ("config-max", [11, 0]), ("config-over", [-1, 0])]:
    work, rows = run(initial, initial, mode, expected)
    print("parse-case", mode, *work, sep="\t", flush=True)
run(initial, root / "body.slim", "prefix", [1, 10])
run(initial, root / "boundary.slim", "prefix")

paths = sorted(Path("conformance").rglob("*.slim")) + sorted(Path("benchmarks/challenges").glob("*/program.slim"))
if not full:
    paths = [Path("examples/hello.slim")]
for path in paths:
    run(path, path)
    text = path.read_text()
    for variant in ["# relocated\n" + text, text.replace("\n", "\r\n"), text + "\nfn inserted() -> I64:\n  4\n", text + "\nfn unfinished("]:
        run(path, write("edit", variant))
print("parse-corpus", len(paths) * 5, "full-token-and-diagnostic-comparison", sep="\t", flush=True)

# Each edit is applied to a valid prior source and checked by the same normal parser.
mutations = 2000 if full else 32
for index in range(mutations):
    at = (index * 7919) % len(source)
    replacement = ["", "\n", "\t", ":", "(", ")", "[", "@", "#", "\x00"][index % 10]
    run(initial, write("mutation", source[:at] + replacement + source[at + 1:]))
print("parse-mutations", mutations, "exact", sep="\t", flush=True)

for count in ([125, 250, 500, 1000, 2000, 4000] if full else [8, 16, 32]):
    text = "module geo\n\n" + "".join(f"fn helper_{i}() -> I64:\n  {i}\n\n" for i in range(count)) + "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
    before = write("geo", text)
    changed = write("geo-edit", text.replace(f"  {count // 2}\n", "  0\n", 1))
    for name, after, expected in [("unchanged", before, [0, count + 1]), ("body", changed, [1, count])]:
        one = run(before, after, expected=expected)
        two = run(before, after, expected=expected)
        assert one == two, (count, name, one, two)
        print("parse-work", count, name, *one[0], sep="\t", flush=True)

if full:
    for length in [67108864, 67108865]:
        text = source + "#" + "x" * (length - len(source) - 1)
        path = write("source-capacity", text)
        mode = "cache-yes" if length == 67108864 else "cache-no"
        run(initial, path, mode)
        print("parse-capacity", "source-bytes", length, mode, sep="\t", flush=True)
        del text
    fields = "module sample\nstruct Large:\n" + "  field: I64\n" * 249994 + "  first: Vec[I64]\n  second: Vec[I64]\n"
    for name, text, mode in [("exact", fields, "cache-yes"), ("over", fields.replace("  field: I64", "  field: Vec[I64]", 1), "cache-no")]:
        work, rows = run(initial, write("node-capacity", text), mode)
        assert work[3] == (1000000 if name == "exact" else 1000003), work
        print("parse-capacity", "canonical-tokens", name, mode, work[3], sep="\t", flush=True)
    arguments = "module sample\nfn call() -> I64:\n  sink(" + ",".join(["@x"] * 333329) + ") + 0\n"
    for name, text, mode in [("exact", arguments, "cache-yes"), ("over", arguments.replace(") + 0", ") + !0"), "cache-no")]:
        work, rows = run(initial, write("lexeme-capacity", text), mode)
        if name == "exact":
            assert work[4] == 1000000, work
        print("parse-capacity", "lexemes", name, mode, work[4], sep="\t", flush=True)
    failed = succeeded = 0
    for ordinal in range(1, 2049):
        env = dict(os.environ, SLIM_ALLOC_FAIL_AT=str(ordinal))
        args = [str(initial), str(root / "body.slim")]
        plain = subprocess.run([str(ordinary), *args], capture_output=True, env=env)
        native = subprocess.run([str(observed), *args], capture_output=True, env=env)
        assert (plain.returncode, plain.stdout, plain.stderr) == (native.returncode, native.stdout, native.stderr), (ordinal, plain.returncode, native.returncode, native.stderr[:400])
        assert plain.returncode in (0, 71), (ordinal, plain.returncode, plain.stderr[:400])
        if plain.returncode == 71:
            assert not plain.stdout, (ordinal, plain.stdout[:400])
            failed += 1
        else:
            succeeded += 1
        print("parse-fault", ordinal, plain.returncode, sep="\t", flush=True)
    assert failed > 0 and succeeded > 0, (failed, succeeded)
    print("parse-fault-summary", failed, succeeded, sep="\t", flush=True)
