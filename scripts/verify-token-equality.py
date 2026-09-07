#!/usr/bin/env python3
"""Independent finite oracle for canonical virtual and source token spellings."""
import argparse
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executables", nargs="+")
    args = parser.parse_args()
    targets = ["", "i64.add", "i64.sub", "i64.mul", "i64.div", "i64.rem",
               "i64.eq", "i64.lt", "i64.le", "i64.gt", "i64.ge", "bool.not",
               "bool.and", "bool.or", "void", "true", "false", "Void",
               "i64.to_u8", "u8.to_i64", "bytes.get", "bytes.len", "bytes.freeze",
               "io.print_i64", "io.println", "io.tcp_exchange", "io.monotonic_ms",
               "vec.new", "arena.get", "mem.replace", "64.", "i", "opaque"]
    virtual = dict(zip(range(31, 44), targets[1:14]))
    virtual.update({120: "void", 121: "true", 122: "false", 130: "Void"})
    tags = [mode * 1000 + tag for mode in range(-2, 3) for tag in range(141)]
    tags += [-(2**63), 2**63 - 1, -1]
    expected = bytearray()
    for encoded in tags:
        tag = abs(encoded) % 1000 * (-1 if encoded < 0 else 1)
        for target in targets:
            for source in ("i64.add", "64.", "", None, "i"):
                expected.append(49 if virtual.get(tag) == target or source == target else 48)
    expected.extend(bytes(49 if target == "" else 48 for target in targets) * 4)
    assert len(expected) == 116952
    for executable in args.executables:
        result = subprocess.run([executable], capture_output=True, timeout=60)
        assert (result.returncode, result.stdout, result.stderr) == (0, expected, b""), executable
        for mode, message in (("bounds", b"byte index out of bounds"),
                              ("overflow", b"I64 subtraction overflow")):
            result = subprocess.run([executable, mode], capture_output=True, timeout=60)
            assert result.returncode == 70 and not result.stdout and message in result.stderr, (
                executable, mode, result.returncode, result.stdout, result.stderr)
    print(f"token equality: 116952 exact results and two eager traps on {len(args.executables)} executables")


if __name__ == "__main__":
    main()
