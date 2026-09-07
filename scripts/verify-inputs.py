"""Host orchestration only; production SLIM computes every compared fact."""
from pathlib import Path
import subprocess
import os
import sys

work = Path(sys.argv[1])
base = Path("tests/fixtures/retained_inputs_calls.slim").read_text()
expected = b"ok previous input queries; four rounds; both modes; six limits\n"


def source(name, content):
    path = work / (name + ".slim")
    path.write_bytes(content.encode())
    return path


def run(binary, before, after=None, damage=False):
    args = [str(work / binary), str(before)]
    output = b"ok four input rounds and both modes; six limits\n"
    if after is not None:
        args.append(str(after))
        output = expected
    if damage:
        args.append("damage")
        output = b"ok input history damage and recovery\n"
    result = subprocess.run(args, capture_output=True, timeout=180)
    if (result.returncode, result.stdout, result.stderr) != (0, output, b""):
        raise RuntimeError((binary, before, after, damage, result.returncode,
                            result.stdout, result.stderr[:2000]))


for binary in ["ordinary", "sanitized"]:
    result = subprocess.run([str(work / binary)], capture_output=True, timeout=180)
    if (result.returncode, result.stdout, result.stderr) != (0, b"", b""):
        raise RuntimeError(("version bounds", binary, result))
print("input-versions\texact combined four; fifth produced version and malformed chains decline", flush=True)

initial = source("initial", base)
# Independent caller edits and complete topology/parameter changes.
edits = {
    "unchanged": base,
    "relocation": "# source relocation\n\n" + base,
    "crlf": base.replace("\n", "\r\n"),
    "callee-body": base.replace("  first + second", "  first - second"),
    "recurrence-mask": base.replace("recur(n - 1, invariant)", "recur(n - 1, invariant + 1)"),
    "caller-value": base.replace('combine(1, identity(3),', 'combine(2, identity(3),'),
    "unknown-to-known": base.replace('bytes.len("unknown")', '6'),
    "insert-function": base.replace('fn unused', 'fn added(x: I64) -> I64:\n  x\n\nfn unused'),
    "remove-unused": base.replace('fn unused(value: I64) -> I64:\n  value\n\n', ''),
    "parameter-name": base.replace('fn identity(value: I64) -> I64:\n  value', 'fn identity(changed: I64) -> I64:\n  changed'),
    "parameter-order": base.replace('first: I64, second: I64', 'second: I64, first: I64'),
    "parameter-type": base.replace('second: I64, text: Bytes', 'second: I64, text: I64')
        .replace('"known"', '1').replace(', "unknown")', ', 2)')
        .replace('"out of domain"', '3').replace('"nested"', '4'),
    "callee-effect": base.replace('fn identity(value: I64) -> I64:', 'fn identity(value: I64) -> I64 effects[io]:'),
    "borrow-mode": base.replace('fn borrow(values: Vec[I64]', 'fn borrow(values: @Vec[I64]').replace('borrow(values,', 'borrow(@values,'),
}
prefix, functions = base.split("fn unused", 1)
blocks = ("fn unused" + functions).split("\n\nfn ")
edits["reorder-functions"] = prefix + "\n\n".join(reversed([blocks[0]] + ["fn " + b for b in blocks[1:]]))
pairs = [(name, initial, source(name, content)) for name, content in edits.items()]
chain = "module chain\n\nfn leaf(value: I64) -> I64:\n  value\n\nfn middle(value: I64) -> I64:\n  leaf(value)\n\nfn main(args: Vec[Bytes]) -> I64:\n  middle(1)\n"
chain_before = source("chain-before", chain)
for label, value in [("two", "2"), ("outside-domain", "1000000001"), ("unknown", "vec.len(args)")]:
    pairs.append(("propagated-" + label, chain_before,
                  source("propagated-" + label, chain.replace("middle(1)", f"middle({value})"))))
order = "module order\n\nfn target(x: I64) -> I64:\n  x\n\nfn alpha() -> I64:\n  target(2)\n\nfn beta() -> I64:\n  target(8)\n\nfn main(args: Vec[Bytes]) -> I64:\n  alpha() + beta()\n"
a = "fn alpha() -> I64:\n  target(2)"
b = "fn beta() -> I64:\n  target(8)"
pairs.append(("incoming-order", source("order-before", order),
              source("order-after", order.replace(a + "\n\n" + b, b + "\n\n" + a))))
for label, before, after in pairs:
    for binary in ["ordinary", "sanitized"]:
        run(binary, before, after)
    print("input-edit", label, "exact", sep="\t", flush=True)

paths = sorted(Path("conformance/pass").glob("*.slim"))
paths += sorted(Path("benchmarks/challenges").glob("*/program.slim"))
paths.append(initial)
for path in paths:
    for binary in ["ordinary", "sanitized"]:
        run(binary, path)
        run(binary, path, path)
print("input-corpus", len(paths), "fresh and previous; six limits; both arrays; four rounds; ordinary and ASan/UBSan", sep="\t", flush=True)
for binary in ["ordinary", "sanitized"]:
    run(binary, initial, source("damage-after", base.replace('  first + second', '  first - second')), damage=True)
print("input-damage", "45 raw and 42 resealed structurally invalid cases; absent/multiple history; fallback and recovery", sep="\t", flush=True)

# Independently counted fixture: seven functions, ten parameters, twenty-one
# argument contributions. In an admitted unchanged update, each callee imports
# one preceding result and then three current results. Three functions have
# no incoming arguments and leave both already-cleared arrays at their defaults. Expected oracle scans
# run outside the native observer's explicit input-API window.
reports = []
for repeat in range(2):
    report = work / f"input-work-{repeat}.tsv"
    result = subprocess.run([str(work / "observed"), str(initial), str(initial)],
                            capture_output=True, timeout=180,
                            env=dict(os.environ, SLIM_INPUT_REPORT=str(report)))
    if (result.returncode, result.stdout, result.stderr) != (0, expected, b""):
        raise RuntimeError(("observed input work", result))
    reports.append(report.read_bytes())
if reports[0] != reports[1]:
    raise RuntimeError("input work was not deterministic")
lines = reports[0].decode().splitlines()
if lines[0] != "slim-inputs\t1\texact\t1000000000" or len(lines) != 8:
    raise RuntimeError(lines)
columns = lines[1].split("\t")
rows = [dict(zip(columns, map(int, line.split("\t")), strict=True)) for line in lines[2:]]
full = dict(phase=0, key_rows=84, adjacency_rows=21, topology_producers=0, structure_imports=7, transfer_calls=4,
            current_lookups=16, previous_lookups=4, current_imports=12, previous_imports=4,
            scalar_merges=0, fallback_scans=0, query_producers=0, query_versions=16,
            invariant_producers=0, discovery_node_visits=0, copied_keys=21,
            copied_results=8, copied_entries=4, owner_map_rows=18,
            imported_invariants=10, imported_arguments=21)
if rows[0] != full:
    raise RuntimeError(("admitted complete input-work prediction", rows[0], full))
for phase in [1, 2, 3, 5]:
    row = rows[phase]
    if row["phase"] != phase or row["transfer_calls"] != 4 or row["fallback_scans"] != 8 or row["scalar_merges"] != 168:
        raise RuntimeError(("bounded ordinary fallback", row))
# The topology-only allowance imports identity's three argument contributions
# before optional query storage declines: six fewer scalar merges than fallback.
if rows[4]["previous_imports"] != 1 or rows[4]["fallback_scans"] != 6 or rows[4]["scalar_merges"] != 162:
    raise RuntimeError(("topology-only admission declines later query storage", rows[4]))
print("input-work\texact deterministic producers, imports, comparisons and copies; six admission limits", flush=True)

# A complete fan-in list must remain linear even when most functions have no
# incoming arguments of their own. Sizes straddle the smallest cases and grow
# geometrically. Predictions count source calls independently of cache layout.
for size in [1, 2, 125, 250, 500, 1000, 2000, 4000]:
    fan = "module fan\n\nfn target(value: I64) -> I64:\n  value\n\n"
    fan += "".join(f"fn caller{i}() -> I64:\n  target({i})\n\n" for i in range(size))
    fan += "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
    path = source(f"fanin-{size}", fan)
    report = work / "fanin-work.tsv"
    result = subprocess.run([str(work / "observed"), str(path), str(path)],
                            capture_output=True, timeout=180,
                            env=dict(os.environ, SLIM_INPUT_REPORT=str(report)))
    if (result.returncode, result.stdout, result.stderr) != (0, expected, b""):
        raise RuntimeError(("fanin input work", size, result))
    lines = report.read_text().splitlines()
    if lines[0] != "slim-inputs\t1\texact\t1000000000" or len(lines) != 8:
        raise RuntimeError((size, lines))
    columns = lines[1].split("\t")
    observed = dict(zip(columns, map(int, lines[2].split("\t")), strict=True))
    predicted = dict(phase=0, key_rows=4*size, adjacency_rows=size,
                     topology_producers=0, structure_imports=size+2, transfer_calls=4,
                     current_lookups=4, previous_lookups=1, current_imports=3, previous_imports=1,
                     scalar_merges=0, fallback_scans=0, query_producers=0, query_versions=4,
                     invariant_producers=0, discovery_node_visits=0, copied_keys=size,
                     copied_results=1, copied_entries=1, owner_map_rows=2*(size+2),
                     imported_invariants=2, imported_arguments=size)
    if observed != predicted:
        raise RuntimeError(("geometric complete incoming work", size, observed, predicted))
    print("input-geometric", size, "exact four comparisons per argument; complete adjacency and copies", sep="\t", flush=True)
