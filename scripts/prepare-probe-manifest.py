#!/usr/bin/env python3
"""Construct a canonical test-only selfhost manifest with explicit probe exports.

This edits the established one-module-per-line selfhost template; it is not a
manifest parser or an acceptance authority. The production compiler checks the
result before any probe is built. Template or injection drift fails loudly.
"""

import argparse
from pathlib import Path
import re
import sys


NAME = r"[A-Za-z_][A-Za-z0-9_]*"
MODULE = r"[A-Za-z_][A-Za-z0-9_.]*"
ROW = re.compile(r"(  \(module (" + MODULE + r") .* \(exports)(?: ([^()]*))?(\)\))")


def names(text):
    values = text.split() if text else []
    assert all(re.fullmatch(NAME, value) for value in values), values
    assert len(values) == len(set(values)), values
    return values


def row_data(line):
    match = ROW.fullmatch(line)
    assert match, f"unexpected probe template row: {line}"
    exports = names(match[3])
    assert exports == sorted(exports, key=lambda name: name.encode()), line
    return match, exports


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--export", action="append", nargs=2, default=[], metavar=("MODULE", "NAMES"))
    args = parser.parse_args()
    lines = args.source.read_text().splitlines()
    assert lines[0] == "(project 1 (entry driver)", "selfhost project header changed"
    assert lines[-1].endswith(")))"), "selfhost project terminator changed"
    lines[-1] = lines[-1][:-1]
    rows = {}
    for line in lines[1:]:
        match, exports = row_data(line)
        module = match[2]
        assert module not in rows, module
        rows[module] = (line, match, exports)
    assert "driver" in rows
    del rows["driver"]
    touched = set()
    for module, requested in args.export:
        assert module in rows and module not in touched, f"missing or repeated export target: {module}"
        additions = names(requested)
        assert additions, module
        _, match, exports = rows[module]
        assert not set(exports).intersection(additions), f"probe exports already public: {module}"
        combined = sorted(exports + additions, key=lambda name: name.encode())
        assert set(combined) - set(exports) == set(additions)
        line = match[1] + " " + " ".join(combined) + match[4]
        updated, checked = row_data(line)
        assert checked == combined
        rows[module] = (line, updated, combined)
        touched.add(module)
    assert len(touched) == len(args.export)
    for line in sys.stdin.read().splitlines():
        if not line:
            continue
        match, exports = row_data(line)
        module = match[2]
        assert module not in rows, f"duplicate probe module: {module}"
        rows[module] = (line, match, exports)
    assert "zzprobe" in rows, "probe entry module was not injected"
    result = "(project 1 (entry zzprobe)\n"
    result += "\n".join(rows[module][0] for module in sorted(rows, key=lambda name: name.encode()))
    args.output.write_text(result + ")\n")


if __name__ == "__main__":
    main()
