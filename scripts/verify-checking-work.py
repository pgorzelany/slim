#!/usr/bin/env python3
"""Bounded independent fixtures for observed production checking traversal."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile


def source(shape, count):
    main = "\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
    if shape == "bindings":
        return "module probe\n\nfn value() -> I64:\n" + "".join(
            f"  let v_{i}: I64 = {i}\n" for i in range(count)) + "  0\n" + main
    if shape == "assignments":
        return "module probe\n\nfn value() -> I64:\n  var v: I64 = 0\n" + "".join(
            f"  v = {i}\n" for i in range(count)) + "  v\n" + main
    if shape == "branches":
        return "module probe\n\nfn value(flag: Bool) -> I64:\n  var v: I64 = 0\n" + "".join(
            f"  if flag:\n    v = {i}\n  else:\n    v = {i+1}\n" for i in range(count)) + "  v\n" + main
    if shape == "calls":
        expr = "identity(" * count + "0" + ")" * count
        return "module probe\n\nfn identity(value: I64) -> I64:\n  value\n\nfn value() -> I64:\n  " + expr + "\n" + main
    if shape == "builtins":
        expr = "1 + (" * count + "0" + ")" * count
        return "module probe\n\nfn value() -> I64:\n  " + expr + "\n" + main
    if shape == "aggregates":
        expr = "0"
        for _ in range(count):
            expr = "unbox(Box(value: " + expr + "))"
        return "module probe\n\nstruct Box:\n  value: I64\n\nfn unbox(box: Box) -> I64:\n  box.value\n\nfn value() -> I64:\n  " + expr + "\n" + main
    if shape == "payloads":
        expr = "unbox(Item::Value(" * count + "0" + "))" * count
        return "module probe\n\nenum Item:\n  Value(I64)\n\nfn unbox(item: Item) -> I64:\n  match item:\n    Value(value):\n      value\n\nfn value() -> I64:\n  " + expr + "\n" + main
    if shape == "recurrence":
        expr = "1 + (" * count + "v" + ")" * count
        return "module probe\n\nfn value(v: I64) -> I64 effects[partial]:\n  recur(" + expr + ")\n" + main
    raise AssertionError(shape)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ordinary", required=True, type=Path)
    parser.add_argument("--observed", required=True, type=Path)
    parser.add_argument("--report", required=True, type=Path)
    parser.add_argument("--geometry-only", action="store_true",
                        help="run the fixed geometric domain; ordinary full coverage is checked separately")
    args = parser.parse_args()
    args.report.parent.mkdir(parents=True, exist_ok=True)
    failure_source = args.report.with_suffix(".failed.slim")
    ordinary, observed = args.ordinary.resolve(), args.observed.resolve()
    root = Path(__file__).resolve().parents[1]
    corpus = sorted((root / "conformance/pass").glob("*.slim")) + sorted(
        (root / "conformance/fail").glob("*.slim")) + sorted(
        (root / "benchmarks/challenges").glob("*/program.slim")) + [root / "tests/fixtures/stable_codegen.slim"]
    rows = []
    covered = set()
    phases = set()
    with tempfile.TemporaryDirectory(prefix="slim-checking-work-") as directory:
        directory = Path(directory)
        report = directory / "work.tsv"

        def check(path, label, accepted=False):
            failure_source.write_bytes(path.read_bytes())
            left = subprocess.run([str(ordinary), "check", str(path)], capture_output=True, timeout=60)
            readings = []
            for _ in range(2):
                report.unlink(missing_ok=True)
                env = dict(os.environ, SLIM_CHECKING_REPORT=str(report))
                right = subprocess.run([str(observed), "check", str(path)], capture_output=True, env=env, timeout=60)
                assert (left.returncode, left.stdout, left.stderr) == (right.returncode, right.stdout, right.stderr), label
                if accepted:
                    assert right.returncode == 0, (label, right.stdout, right.stderr)
                lines = report.read_text().splitlines()
                assert lines[0] == "slim-checking\t2\texact\t1000000000", (label, lines[0])
                values = dict((key, int(value)) for key, value in (line.split("\t") for line in lines[1:]))
                assert values["machine_native_depth"] <= 1, label
                assert values["machine_active_at_exit"] == 0, label
                assert values["machine_entries"] == values["machine_ends"], label
                assert values["inference_entries"] == values["inference_ends"], label
                assert values["machine_failed"] == values["inference_failed"] == 0, label
                assert values["steps"] <= 16 * len(path.read_bytes()), label
                readings.append(values)
            assert readings[0] == readings[1], (label, readings)
            values = readings[0]
            phases.update(key for key, count in values.items() if key.startswith("resume_") and count)
            covered.update(key for key, count in values.items() if key.startswith("builtin_") and key.count("_") == 2 and count)
            for key, count in values.items():
                rows.append(f"{label}\t{left.returncode}\t{key}\t{count}")
            args.report.write_text("fixture\tstatus\tmetric\tvalue\n" + "\n".join(rows) + "\n")
            failure_source.unlink()
            return values

        if not args.geometry_only:
            for path in corpus:
                check(path, str(path.relative_to(root)))
            assert phases == {f"resume_{phase}" for phase in range(18)}, phases
            expected = {1: 2, 2: 2, 3: 1, 4: 2, 5: 2, 6: 3, 7: 2, 8: 2, 9: 1, 10: 2, 11: 6, 12: 2}
            required = {f"builtin_{op}_{phase}" for op, count in expected.items() for phase in range(count)}
            assert covered == required, (required - covered, covered - required)
            print(f"checking observation: {len(corpus)} corpus fixtures; all continuation/builtin phases; exact repeated work", flush=True)
        for shape in ("bindings", "assignments", "branches", "calls", "builtins", "aggregates", "payloads", "recurrence"):
            previous = None
            for count in (8, 32, 128, 512):
                path = directory / f"{shape}-{count}.slim"
                path.write_text(source(shape, count))
                values = check(path, f"generated/{shape}/{count}", accepted=True)
                assert values["machine_native_depth"] == 1
                if previous is not None:
                    assert values["steps"] * 2 <= previous["steps"] * 9, (shape, count, previous, values)
                previous = values
            print(f"checking observation: {shape}; sizes 8/32/128/512; native traversal depth 1", flush=True)
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text("fixture\tstatus\tmetric\tvalue\n" + "\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
