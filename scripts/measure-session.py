#!/usr/bin/env python3
"""Measure uninstrumented retained sessions on one host after validation jobs end.

Build tests/fixtures/session_latency.slim as zzprobe against the baseline and
candidate selfhost manifests (entry zzprobe, imports project retained session),
using their production compilers and the same C compiler/options/runtime. Supply
those O2 executables and their generated C below. Timings include process startup,
input capture, cold preparation, update, and destruction; they are not isolated
warm-query timings or portable performance budgets.
"""
import argparse
import datetime
import hashlib
from pathlib import Path
import statistics
import subprocess
import time


def timed(command):
    start = time.perf_counter_ns()
    result = subprocess.run(list(map(str, command)), capture_output=True, timeout=300)
    elapsed = time.perf_counter_ns() - start
    assert result.returncode == 0 and not result.stdout and not result.stderr, (
        command, result.returncode, result.stdout[:300], result.stderr[:300])
    return elapsed


def source(kind, size):
    if kind == "counted":
        functions = "".join(
            f"fn f{i}(index: I64) -> I64 effects[partial]:\n"
            "  if index == 3:\n    42\n  else:\n    recur(index + 1)\n\n"
            f"fn call{i}() -> I64 effects[partial]:\n  f{i}(0)\n\n"
            for i in range(size))
        before = "module app\n\n" + functions + "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
        after = before.rsplit("  0\n", 1)[0] + "  1\n"
    else:
        locals_source = "".join(f"  let v{i}: I64 = {i}\n" for i in range(63)) if kind == "dense" else ""
        functions = "".join(f"fn f{i}(x: I64) -> I64:\n{locals_source}  x\n\n" for i in range(size))
        before = "module app\n\n" + functions + "fn main(args: Vec[Bytes]) -> I64:\n  f0(1)\n"
        after = before.replace("  x\n", "  42\n", 1)
    return before, after


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline")
    parser.add_argument("candidate")
    parser.add_argument("--baseline-revision", required=True)
    parser.add_argument("--baseline-c", required=True)
    parser.add_argument("--candidate-c", required=True)
    parser.add_argument("--candidate-seed", required=True)
    parser.add_argument("--work", required=True)
    parser.add_argument("--report", required=True)
    parser.add_argument("--samples", type=int, default=11)
    args = parser.parse_args()
    assert 1 <= args.samples <= 101
    work = Path(args.work)
    work.mkdir(parents=True, exist_ok=True)
    report = Path(args.report)
    report.parent.mkdir(parents=True, exist_ok=True)
    digest = lambda path: hashlib.sha256(Path(path).read_bytes()).hexdigest()
    metadata = (f"# date={datetime.date.today().isoformat()}; baseline={args.baseline_revision}; "
                f"baseline_probe_sha256={digest(args.baseline_c)}; "
                f"candidate_probe_sha256={digest(args.candidate_c)}; "
                f"candidate_seed_sha256={digest(args.candidate_seed)}; O2; "
                f"two warmup pairs and {args.samples} alternating pairs; "
                "uninstrumented complete process totals; two-clean uses the candidate; "
                "same host only, no portable timing claim\n")
    with report.open("w") as output:
        output.write(metadata + "geometry\tsize\tupdate\tcomparison\tpair\tbaseline_ns\tcandidate_ns\tratio\n")
        for kind, sizes in [("scalar", [125, 250, 500, 1000, 2000, 4000]),
                            ("dense", [32, 128, 512]),
                            ("counted", [64, 125, 250, 500, 1000, 2000])]:
            for size in sizes:
                paths = []
                for variant, program in zip(["before", "after"], source(kind, size)):
                    directory = work / f"{kind}-{size}-{variant}"
                    directory.mkdir(exist_ok=True)
                    path = directory / "slim.project"
                    path.write_text('(project 1 (entry app) (module app "program.slim" (imports) (exports)))\n')
                    (directory / "program.slim").write_text(program)
                    paths.append(path)
                for update, changed in [("unchanged", paths[0]), ("body", paths[1])]:
                    for comparison in ["prior-session", "two-clean"]:
                        baseline = args.baseline if comparison == "prior-session" else args.candidate
                        mode = "reuse" if comparison == "prior-session" else "cold"
                        left = [baseline, paths[0], changed, mode]
                        right = [args.candidate, paths[0], changed, "reuse"]
                        for _ in range(2):
                            timed(left)
                            timed(right)
                        measured = []
                        for pair in range(args.samples):
                            if pair % 2 == 0:
                                old, new = timed(left), timed(right)
                            else:
                                new, old = timed(right), timed(left)
                            row = [kind, size, update, comparison, pair, old, new, new / old]
                            measured.append(row)
                            output.write("\t".join(map(str, row)) + "\n")
                        output.flush()
                        print(kind, size, update, comparison,
                              round(statistics.median(row[5] for row in measured) / 1e6, 3),
                              round(statistics.median(row[6] for row in measured) / 1e6, 3), flush=True)


if __name__ == "__main__":
    main()
