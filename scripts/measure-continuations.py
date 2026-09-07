#!/usr/bin/env python3
"""Same-host continuation costs with interleaved identical-baseline controls.

Supply uninstrumented baseline/candidate production compiler executables built
with identical C flags and runtime. Run after other native validation jobs end.
The report records process wall time including startup and I/O, not a portable
performance budget. Each group rotates one of the six permutations of two
identical baseline runs and one candidate run; no timing result grants a waiver.
"""
import argparse
import hashlib
import itertools
import platform
import runpy
from pathlib import Path
import statistics
import subprocess
import time


SHAPES = ("spine", "functions", "assignments", "branches", "branch_functions",
          "projection_functions", "builtin_functions", "call_functions",
          "aggregate_functions", "recurrence_functions")
NESTED_SHAPES = tuple("nested_" + shape for shape in (
    "bindings", "assignments", "branches", "calls", "builtins", "aggregates", "payloads", "recurrence"))


def source(shape, size):
    if shape in NESTED_SHAPES:
        fixtures = runpy.run_path(str(Path(__file__).with_name("verify-checking-work.py")))
        return fixtures["source"](shape.removeprefix("nested_"), size)
    main = "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
    if shape == "spine":
        return "module spine\n\nfn value() -> I64:\n" + "".join(
            f"  let v_{i}: I64 = {i}\n" for i in range(size)) + (
            "  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  value()\n")
    if shape == "assignments":
        return "module assignments\n\nfn value() -> I64:\n  var v: I64 = 0\n" + "".join(
            f"  v = {i}\n" for i in range(size)) + "  v\n\n" + main
    if shape == "branches":
        return "module branches\n\nfn value(flag: Bool) -> I64:\n  var v: I64 = 0\n" + "".join(
            f"  if flag:\n    v = {i}\n  else:\n    v = {i+1}\n"
            for i in range(size)) + "  v\n\n" + main
    prefix = f"module {shape}\n\n"
    if shape == "projection_functions":
        prefix += "struct Pair:\n  value: I64\n\n"
        declaration = "fn value_{i}(pair: Pair) -> I64:\n  pair.value\n\n"
    elif shape == "aggregate_functions":
        prefix += "struct Box:\n  value: I64\n\n"
        declaration = "fn value_{i}(v: I64) -> Box:\n  Box(value: v)\n\n"
    elif shape == "call_functions":
        prefix += "fn identity(v: I64) -> I64:\n  v\n\n"
        declaration = "fn value_{i}(v: I64) -> I64:\n  identity(v)\n\n"
    elif shape == "builtin_functions":
        declaration = "fn value_{i}(v: I64) -> I64:\n  v + 1\n\n"
    elif shape == "recurrence_functions":
        declaration = "fn value_{i}(v: I64) -> I64 effects[partial]:\n  recur(v)\n\n"
    elif shape == "branch_functions":
        declaration = "fn value_{i}(flag: Bool) -> I64:\n  if flag:\n    {i}\n  else:\n    {next}\n\n"
    elif shape == "functions":
        declaration = "fn value_{i}() -> I64:\n  let v: I64 = {i}\n  v\n\n"
    else:
        raise AssertionError(shape)
    return prefix + "".join(declaration.format(i=i, next=i+1) for i in range(size)) + main


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("--work", required=True, type=Path)
    parser.add_argument("--report", required=True, type=Path)
    parser.add_argument("--sizes", nargs="+", type=int, default=[125, 500, 2000, 4000])
    parser.add_argument("--shapes", nargs="+", choices=SHAPES + NESTED_SHAPES, default=list(SHAPES))
    parser.add_argument("--selfhost", type=Path, help="also check/emit this frozen source manifest")
    parser.add_argument("--groups", type=int, default=66)
    parser.add_argument("--commands", nargs="+", choices=["check", "emit"], default=["check", "emit"])
    args = parser.parse_args()
    if any(len(values) != len(set(values)) for values in (args.shapes, args.sizes, args.commands)):
        parser.error("shapes, sizes and commands must each be distinct")
    if not 6 <= args.groups <= 120 or args.groups % 6:
        parser.error("groups must be a multiple of six in 6..120")
    if not 1 <= len(args.sizes) <= 8 or any(size < 1 or size > 16000 for size in args.sizes):
        parser.error("supply 1..8 source sizes, each in 1..16000")
    if any(shape in NESTED_SHAPES for shape in args.shapes) and max(args.sizes) > 512:
        parser.error("the nested measurement domain has maximum depth 512; use --sizes 8 32 128 512")
    executables = {"a": args.baseline.resolve(), "b": args.baseline.resolve(),
                   "c": args.candidate.resolve()}
    identities = {key: digest(path) for key, path in executables.items()}
    args.work.mkdir(parents=True, exist_ok=True)
    args.report.parent.mkdir(parents=True, exist_ok=True)
    fixtures = []
    for shape in args.shapes:
        for size in args.sizes:
            path = args.work / f"{shape}-{size}.slim"
            path.write_text(source(shape, size))
            fixtures.append((shape, size, path.resolve()))
    if args.selfhost:
        fixtures.append(("selfhost", 0, args.selfhost.resolve()))
    permutations = list(itertools.permutations("abc"))
    with args.report.open("w") as output:
        output.write(f"# schema=1; host={platform.system()}-{platform.machine()}; "
                     f"groups={args.groups}; six rotating permutations; two warmups per executable; "
                     "a/b identical baseline; c candidate; uninstrumented process wall ns\n")
        for key, path in executables.items():
            output.write(f"# compiler_{key}={path}; sha256={identities[key]}\n")
        output.write("shape\tsize\tcommand\tgroup\torder\ta_ns\tb_ns\tc_ns\n")
        for shape, size, path in fixtures:
            output.write(f"# source={path}; sha256={digest(path)}\n")
            for command in args.commands:
                parameters = ["check", str(path)] if command == "check" else [str(path)]
                results = [subprocess.run([str(executables[key]), *parameters],
                                         capture_output=True, timeout=300) for key in ("a", "c")]
                before, after = results
                assert before.returncode == 0, (path, command, before.stdout, before.stderr)
                assert (before.returncode, before.stdout, before.stderr) == (
                    after.returncode, after.stdout, after.stderr), (path, command, "output mismatch")

                def timed(key):
                    start = time.perf_counter_ns()
                    result = subprocess.run([str(executables[key]), *parameters],
                                            stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, timeout=300)
                    elapsed = time.perf_counter_ns() - start
                    assert result.returncode == 0 and not result.stderr, (path, command, key, result.stderr)
                    return elapsed

                for _ in range(2):
                    for key in "abc":
                        timed(key)
                controls, candidates = [], []
                for group in range(args.groups):
                    order = permutations[group % 6]
                    times = {key: timed(key) for key in order}
                    output.write(f"{shape}\t{size}\t{command}\t{group}\t{''.join(order)}\t"
                                 f"{times['a']}\t{times['b']}\t{times['c']}\n")
                    output.flush()
                    controls.append(times["b"] / times["a"])
                    candidates.append(times["c"] / ((times["a"] + times["b"]) / 2))
                quartiles = statistics.quantiles(controls, n=4)
                print(shape, size, command, "candidate median", statistics.median(candidates),
                      "control IQR", quartiles[0], quartiles[2], flush=True)
    for key, path in executables.items():
        assert digest(path) == identities[key], (key, "compiler changed during measurement")


if __name__ == "__main__":
    main()
