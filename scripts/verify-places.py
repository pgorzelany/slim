"""Host assertions over the production SLIM place/flow probe."""
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
    path.write_text(source)
    return path


def run(*args, env=None):
    plain = subprocess.run([str(ordinary), *map(str, args)], capture_output=True, env=env)
    native = subprocess.run([str(observed), *map(str, args)], capture_output=True, env=env)
    assert plain.returncode == native.returncode == 0, (args, plain.returncode, native.returncode, plain.stdout[:400], native.stderr[:400])
    assert plain.stdout == native.stdout and plain.stderr == native.stderr, args
    return native.stdout.decode().splitlines()

shadowing = 'module places\n\nstruct Leaf:\n  value: I64\n\nstruct Box:\n  leaf: Leaf\n\nfn shadowed(box: Box) -> I64:\n  let first: I64 = box.leaf.value\n  let box: Box = Box(leaf: Leaf(value: 2))\n  first + box.leaf.value\n\nfn main(args: Vec[Bytes]) -> I64:\n  shadowed(Box(leaf: Leaf(value: 1)))\n'
modes = 'module modes\n\nstruct Box:\n  values: Vec[I64]\n\nenum Wrapped:\n  Some(Box)\n\nfn owned(box: ^Box) -> I64:\n  vec.len(box.values)\n\nfn shared(box: Box) -> I64:\n  let alias: Vec[I64] = box.values\n  vec.len(alias)\n\nfn exclusive(box: @Box) -> I64:\n  vec.len(box.values)\n\nfn payload(value: Wrapped) -> I64:\n  match value:\n    Some(box):\n      vec.len(box.values)\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
initial = write("initial", shadowing)
mode_path = write("modes", modes)
lines = run(initial)
rows = [line.split() for line in lines if line.startswith("shadowed exact ")]
outer = [list(map(int, row[2:])) for row in rows if row[-2:] == ["2", "3"]]
assert [(row[2], row[3], row[4]) for row in outer] == [(5, 2, 0), (28, 4, 0)], outer
assert any("unknown computed-place-base" in line for line in lines)
mode_rows = [line.split() for line in run(mode_path) if " exact " in line]
for function, mode in [("owned", 0), ("shared", 1), ("exclusive", 2), ("payload", 1)]:
    selected = [row for row in mode_rows if row[0] == function]
    assert selected and all(int(row[6]) == mode for row in selected), (function, selected)
print("place-bindings\texact\tshadowing-owned-shared-exclusive-enum-alias", flush=True)

for name, source, expected in [
    ("unchanged", shadowing, 98),
    ("inserted", shadowing.replace("fn shadowed", "fn added() -> I64:\n  0\n\nfn shadowed"), 98),
    ("relocated", "# shifted source\n" + shadowing, 98),
    ("crlf", shadowing.replace("\n", "\r\n"), 0),
    ("renamed", shadowing.replace("shadowed", "renamed"), 0),
    ("body", shadowing.replace("value: 2", "value: 3"), 36),
]:
    after = write(name, source)
    output = run(initial, after)
    assert output == [f"mapped {expected}"], (name, output)
    print("place-mapping", name, expected, sep="\t", flush=True)
    run(after)

for name, source in [
    ("layout", modes.replace("struct Box:\n", "struct Box:\n  stamp: I64\n")),
    ("ownership", modes.replace("fn owned(box: ^Box)", "fn owned(box: @Box)")),
]:
    after = write(name, source)
    output = run(mode_path, after)
    assert len(output) == 1 and output[0].startswith("mapped ") and int(output[0].split()[1]) > 0, (name, output)
    run(after)
    print("place-mapping", name, output[0].split()[1], sep="\t", flush=True)

fixtures = sorted(Path("conformance/pass").glob("*.slim")) + sorted(Path("benchmarks/challenges").glob("*/program.slim"))
if not full:
    fixtures = [Path("examples/hello.slim")]
for fixture in fixtures:
    run(fixture)
print("place-corpus", len(fixtures), "checked-bindings-and-flow", sep="\t", flush=True)

for depth in ([1, 2, 8, 16, 32, 64, 128, 256] if full else [1, 2, 8, 16]):
    source = "module geometric\n\nstruct T0:\n  value: I64\n\n"
    for i in range(1, depth):
        source += f"struct T{i}:\n  value: T{i-1}\n\n"
    source += f"fn read(box: T{depth-1}) -> I64:\n  box" + ".value" * depth + "\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
    path = write(f"depth-{depth}", source)
    for mode, visits in [("full", depth + 1), ("short", 1)]:
        prior = None
        for repeat in range(2):
            output = run(path, "one", mode, env=dict(os.environ, SLIM_PLACE_REPORT=str(report)))
            observed_work = report.read_text()
            assert observed_work == f"slim-place\t1\texact\t1000000000\n0\t{visits}\n", (depth, mode, observed_work)
            if mode == "full":
                assert output[0].startswith("exact ") and int(output[0].split()[-1]) == visits, (depth, output)
            else:
                assert output == ["bounded 1"], output
            if prior is not None:
                assert prior == (output, observed_work), (depth, mode)
            prior = output, observed_work
        print("place-work", depth, mode, visits, sep="\t", flush=True)

if full:
    failed = succeeded = 0
    for ordinal in range(1, 2049):
        env = dict(os.environ, SLIM_ALLOC_FAIL_AT=str(ordinal))
        plain = subprocess.run([str(ordinary), str(initial)], capture_output=True, env=env)
        native = subprocess.run([str(observed), str(initial)], capture_output=True, env=env)
        assert (plain.returncode, plain.stdout, plain.stderr) == (native.returncode, native.stdout, native.stderr), (ordinal, plain.returncode, native.returncode, native.stderr[:400])
        assert plain.returncode in (0, 71), (ordinal, plain.returncode, plain.stderr[:400])
        if plain.returncode == 71:
            assert not plain.stdout, (ordinal, plain.stdout[:400])
            failed += 1
        else:
            succeeded += 1
        print("place-fault", ordinal, plain.returncode, sep="\t", flush=True)
    assert failed > 0 and succeeded > 0, (failed, succeeded)
    print("place-fault-summary", failed, succeeded, sep="\t", flush=True)
