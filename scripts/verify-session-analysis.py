"""Compare complete derived parallel views and range/quality/parallel reports."""
from pathlib import Path
import re
import subprocess
import sys

root = Path(sys.argv[1])


def project(label, source):
    module = re.search(r"(?m)^module ([A-Za-z_][A-Za-z_0-9]*)$", source)
    assert module, label
    directory = root / label
    directory.mkdir()
    (directory / "program.slim").write_text(source)
    path = directory / "slim.project"
    path.write_text(f'(project 1 (entry {module[1]}) (module {module[1]} "program.slim" (imports) (exports)))\n')
    return path


def compare(before, after):
    for binary in ["ordinary", "sanitized"]:
        result = subprocess.run([str(root / binary), str(before), str(after)],
                                capture_output=True, timeout=180)
        assert (result.returncode, result.stdout, result.stderr) == (0, b"", b""), (
            before, after, binary, result.returncode, result.stdout, result.stderr[:800])


paths = sorted(Path("conformance/pass").glob("*.slim"))
paths += sorted(Path("benchmarks/challenges").glob("*/program.slim"))
paths.append(Path("tests/fixtures/retained_inputs_calls.slim"))
for index, path in enumerate(paths):
    source = path.read_text()
    before = project(f"corpus-{index}", source)
    compare(before, before)
    assert "session_analysis_added" not in source
    after = project(f"insert-{index}", source + "\nfn session_analysis_added(value: I64) -> I64:\n  value\n")
    compare(before, after)
print("session-analysis-corpus", len(paths), "cold, unchanged and insertion; complete parallel fields and report components; ordinary and ASan/UBSan", sep="\t", flush=True)

base = Path("tests/fixtures/retained_inputs_calls.slim").read_text()
before = project("calls-before", base)
for label, old, new in [
    ("callee-body", "  first + second", "  first - second"),
    ("recurrence-mask", "recur(n - 1, invariant)", "recur(n - 1, invariant + 1)"),
    ("caller-value", "combine(1, identity(3),", "combine(2, identity(3),"),
    ("unknown-to-known", 'bytes.len("unknown")', "6"),
    ("parameter-order", "first: I64, second: I64", "second: I64, first: I64"),
    ("callee-effect", "fn identity(value: I64) -> I64:", "fn identity(value: I64) -> I64 effects[io]:"),
]:
    assert old in base, label
    compare(before, project(label, base.replace(old, new)))
    print("session-analysis-edit", label, "exact", sep="\t", flush=True)
