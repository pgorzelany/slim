#!/usr/bin/env python3
"""Compare complete checker dumps from explicitly selected native probes."""
import argparse
import hashlib
import os
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ordinary", required=True, type=Path)
    parser.add_argument("--candidate", required=True, type=Path)
    parser.add_argument("--report", required=True, type=Path)
    parser.add_argument("--faults", action="store_true", help="compare allocation ordinals 1..512 on two fixed fixtures")
    args = parser.parse_args()
    ordinary, candidate = args.ordinary.resolve(), args.candidate.resolve()
    root = Path(__file__).resolve().parents[1]
    fixtures = sorted((root / "conformance/pass").glob("*.slim")) + sorted(
        (root / "conformance/fail").glob("*.slim")) + sorted(
        (root / "benchmarks/challenges").glob("*/program.slim")) + [root / "tests/fixtures/stable_codegen.slim"]
    env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0", UBSAN_OPTIONS="halt_on_error=1")
    env.pop("SLIM_ALLOC_FAIL_AT", None)
    args.report.parent.mkdir(parents=True, exist_ok=True)
    rows = []

    def compare(path, environment, ordinal):
        left, right = [subprocess.run([str(exe), str(path)], capture_output=True, env=environment, timeout=60)
                       for exe in (ordinary, candidate)]
        values = lambda result: (result.returncode, result.stdout, result.stderr)
        if values(left) != values(right):
            args.report.with_suffix(".failed.slim").write_bytes(path.read_bytes())
            for label, result in (("before", left), ("after", right)):
                args.report.with_suffix(f".{label}.stdout").write_bytes(result.stdout)
                args.report.with_suffix(f".{label}.stderr").write_bytes(result.stderr)
            raise AssertionError((path, ordinal, left.returncode, right.returncode))
        assert left.returncode in ((0,) if ordinal == 0 else (0, 71)), (path, ordinal, left.returncode, left.stderr)
        if left.returncode == 0:
            assert not left.stderr, (path, ordinal, left.stderr)
            assert left.stdout.startswith(b"checking-state 1\n") and left.stdout.endswith(b"complete\n"), (path, ordinal)
        else:
            assert not left.stdout, (path, ordinal)
            expected_error = f"SLIM allocation failure: exhausted at allocation {ordinal}\n".encode()
            assert left.stderr == expected_error, (path, ordinal, left.stderr)
        rows.append(f"{path.relative_to(root)}\t{ordinal}\t{left.returncode}\t{len(left.stdout)}\t{hashlib.sha256(left.stdout).hexdigest()}")
        return left

    for path in fixtures:
        compare(path, env, 0)
    print(f"complete checking-state comparison: {len(fixtures)} fixtures", flush=True)
    if args.faults:
        for name in ("reinit_branches", "local_loan_initializer"):
            path = root / f"conformance/pass/{name}.slim"
            expected = compare(path, env, 0)
            failed = succeeded = 0
            for ordinal in range(1, 513):
                result = compare(path, dict(env, SLIM_ALLOC_FAIL_AT=str(ordinal)), ordinal)
                if result.returncode == 0:
                    assert result.stdout == expected.stdout, (path, ordinal)
                    succeeded += 1
                else:
                    failed += 1
            assert failed and succeeded, (name, failed, succeeded)
            print(f"checking faults: {name}; {failed} failed, {succeeded} successful", flush=True)
    identities = "".join(f"# {name}_binary_sha256: {hashlib.sha256(path.read_bytes()).hexdigest()}\n"
                         for name, path in (("ordinary", ordinary), ("candidate", candidate)))
    args.report.write_text(identities + "fixture\tordinal\tstatus\tbytes\tstdout_sha256\n" + "\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
