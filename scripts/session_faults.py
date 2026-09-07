"""Bounded allocation-fault differential; compiler acceptance stays in SLIM."""
from pathlib import Path
import os
import subprocess
import sys


def run_faults(root, before, after, label, require_import=False, require_fragments=False):
    report_path = root / "fault-report.tsv"
    failed = succeeded = imported = 0
    for ordinal in range(1, 2049):
        args = [str(before), str(after), "work"]
        env = dict(os.environ, SLIM_ALLOC_FAIL_AT=str(ordinal))
        plain = subprocess.run([str(root / "ordinary"), *args], capture_output=True, env=env)
        report_path.unlink(missing_ok=True)
        native = subprocess.run([str(root / "observed"), *args], capture_output=True,
                                env=dict(env, SLIM_SESSION_REPORT=str(report_path)))
        assert (plain.returncode, plain.stdout, plain.stderr) == (native.returncode, native.stdout, native.stderr), (label, ordinal, plain.returncode, native.returncode, native.stderr[:400])
        assert plain.returncode in (0, 71), (label, ordinal, plain.returncode, plain.stderr[:400])
        if plain.returncode == 71:
            assert not plain.stdout, (label, ordinal, plain.stdout[:400])
            failed += 1
        else:
            succeeded += 1
        lines = report_path.read_text().splitlines()
        assert 1 <= len(lines) <= 3, (label, ordinal)
        assert lines[0].split("\t")[:2] == ["slim-session", "7"], (label, ordinal)
        rows = []
        for i, line in enumerate(lines[1:]):
            row = list(map(int, line.split("\t")))
            assert len(row) == 24 and row[0] == i and all(0 <= n <= 1000000000 for n in row[1:]), (label, ordinal)
            rows.append(row)
        if require_import and plain.returncode == 0:
            assert len(rows) == 2 and rows[1][6:8] == [1, 1], (label, ordinal, rows)
            imported += 1
        if require_fragments and plain.returncode == 0:
            assert len(rows) == 2 and rows[1][20] > 0 and rows[1][21] > 0, (label, ordinal, rows)
        print("session-" + label, ordinal, plain.returncode, len(rows), sep="\t", flush=True)
    assert failed > 0 and succeeded > 0, (label, failed, succeeded)
    if require_import:
        assert imported == succeeded, (label, imported, succeeded)
    print("session-" + label + "-summary", failed, succeeded, sep="\t", flush=True)


if __name__ == "__main__":
    assert len(sys.argv) == 5
    run_faults(Path(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3]), sys.argv[4],
               require_import=sys.argv[4] == "update-fault")
