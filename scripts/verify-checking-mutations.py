#!/usr/bin/env python3
"""Fixed-budget argument/arity differential using complete native checker dumps."""
import argparse
import hashlib
import itertools
from pathlib import Path
import subprocess
import tempfile

PREFIX = """module mutations

struct Box:
  items: Vec[I64]
  value: I64

enum Item:
  Value(Vec[I64])

fn identity(value: I64) -> I64:
  value

fn size(values: Vec[I64]) -> I64:
  vec.len(values)

fn take(values: ^Vec[I64]) -> I64:
  vec.len(values)

fn touch(values: @Vec[I64]) -> I64:
  vec.len(values)

fn subject() -> I64 effects[alloc, io, partial]:
  let values: Vec[I64] = vec.new()
  let replacement: Vec[I64] = vec.new()
  let bytes_out: Vec[U8] = vec.new()
  let text: Bytes = "text"
  let arena: Arena[I64] = arena.new()
  let identity_id: Id[I64] = arena.add(@arena, 0)
  let box: Box = Box(items: vec.new(), value: 0)
"""
SUFFIX = "  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
OPERATIONS = [
    ("vec.new", "Vec[I64]", []), ("arena.new", "Arena[I64]", []),
    ("io.monotonic_ms", "I64", []),
    ("u8.to_i64", "I64", ["i64.to_u8(0)"]), ("i64.to_u8", "U8", ["0"]),
    ("bytes.len", "I64", ["text"]), ("bytes.get", "U8", ["text", "0"]),
    ("bytes.freeze", "Bytes", ["^bytes_out"]),
    ("io.print_i64", "Void", ["0"]), ("io.print_bytes", "Void", ["text"]),
    ("io.println", "Void", ["text"]), ("io.read_file", "Bool", ["text", "@bytes_out"]),
    ("io.tcp_exchange", "Bool", ["text", "0", "text", "1", "1", "@bytes_out"]),
    ("vec.len", "I64", ["values"]), ("vec.get", "I64", ["values", "0"]),
    ("vec.push", "Void", ["@values", "0"]), ("vec.set", "Void", ["@values", "0", "0"]),
    ("arena.add", "Id[I64]", ["@arena", "0"]), ("arena.get", "I64", ["arena", "identity_id"]),
    ("mem.replace", "Vec[I64]", ["@box.items", "replacement"]),
    ("identity", "I64", ["0"]), ("size", "I64", ["values"]),
    ("take", "I64", ["^values"]), ("touch", "I64", ["@values"]),
    ("Box", "Box", ["items: values", "value: 0"]),
    ("Item::Value", "Item", ["values"]),
    ("+", "I64", ["0", "1"]), ("==", "Bool", ["0", "1"]),
    ("&&", "Bool", ["true", "false"]), ("!", "Bool", ["true"]),
]
VALUES = ["0", "true", '"bad"', "void", "missing", "values", "box.items",
          "identity(0)", "vec.len(values)", "mem.replace(@box.items, replacement)"]
OPERATORS = {"+", "==", "&&", "!"}
CASE_CAP = 4096


def expression(name, arguments):
    if name == "!":
        return f"!({arguments[0]})"
    if name in OPERATORS:
        return f"({arguments[0]}) {name} ({arguments[1]})"
    return name + "(" + ", ".join(arguments) + ")"


def cases():
    for name, result_type, original in OPERATIONS:
        variants = [("base", original)]
        if name not in OPERATORS:
            variants.extend((f"arity-{count}", original[:count] + ["0"] * max(0, count - len(original)))
                            for count in range(9) if count != len(original))
        if len(original) > 1:
            variants.append(("reversed", list(reversed(original))))
        for position, value, mode in itertools.product(range(len(original)), VALUES, ("", "@", "^")):
            arguments = list(original)
            label = arguments[position].split(": ", 1)[0] + ": " if name == "Box" else ""
            arguments[position] = label + mode + value
            variants.append((f"argument-{position}-{VALUES.index(value)}-{mode or 'plain'}", arguments))
        for label, arguments in variants:
            expr = expression(name, arguments)
            line = "  " + expr + "\n" if result_type == "Void" else f"  let result: {result_type} = {expr}\n"
            yield name + "/" + label, PREFIX + line + SUFFIX, label == "base"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", required=True, type=Path)
    parser.add_argument("--candidate", required=True, type=Path)
    parser.add_argument("--report", required=True, type=Path)
    args = parser.parse_args()
    before, after = args.baseline.resolve(), args.candidate.resolve()
    domain = list(cases())
    assert len(domain) <= CASE_CAP, len(domain)
    args.report.parent.mkdir(parents=True, exist_ok=True)
    rows = ["case\tstatus\tsource_sha256\tstate_sha256"]
    accepted = rejected = parser_rejected = 0
    with tempfile.TemporaryDirectory(prefix="slim-checking-mutations-") as directory:
        path = Path(directory) / "program.slim"
        for index, (label, source, nominal) in enumerate(domain):
            path.write_text(source)
            left, right = [subprocess.run([str(exe), str(path)], capture_output=True, timeout=60) for exe in (before, after)]
            state = lambda result: (result.returncode, result.stdout, result.stderr)
            if state(left) != state(right) or left.returncode != 0 or left.stderr:
                args.report.with_suffix(".failed.slim").write_text(source)
                for name, result in (("before", left), ("after", right)):
                    args.report.with_suffix(f".{name}.stdout").write_bytes(result.stdout)
                    args.report.with_suffix(f".{name}.stderr").write_bytes(result.stderr)
                raise AssertionError((label, left.returncode, right.returncode))
            assert left.stdout.startswith(b"checking-state 1\n") and left.stdout.endswith(b"complete\n"), label
            status = left.stdout.splitlines()[1].decode()
            if nominal and status != "status 0":
                args.report.with_suffix(".failed.slim").write_text(source)
                args.report.with_suffix(".before.stdout").write_bytes(left.stdout)
                raise AssertionError((label, status))
            if status == "status 0": accepted += 1
            elif status == "parse-invalid": parser_rejected += 1
            else: rejected += 1
            rows.append(f"{label}\t{status}\t{hashlib.sha256(source.encode()).hexdigest()}\t{hashlib.sha256(left.stdout).hexdigest()}")
            if (index + 1) % 256 == 0:
                print("checked-state mutations:", index + 1, flush=True)
                args.report.write_text("\n".join(rows) + "\n")
    args.report.write_text("\n".join(rows) + "\n")
    print(f"checked-state mutations: {len(domain)} exact; {accepted} accepted; {rejected} checker-rejected; {parser_rejected} parser-rejected; cap {CASE_CAP}", flush=True)


if __name__ == "__main__":
    main()
