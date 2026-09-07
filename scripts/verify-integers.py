"""Exercise production checking/emission with independent decimal expectations."""
from pathlib import Path
import json
import os
import random
import subprocess
import sys

root = Path(sys.argv[1])
ordinary = sys.argv[2]
sanitized = sys.argv[3] if len(sys.argv) > 3 else None
compilers = [ordinary] + ([sanitized] if sanitized else [])


def run(args, **kwargs):
    result = subprocess.run(list(map(str, args)), capture_output=True, **kwargs)
    return result.returncode, result.stdout, result.stderr


def compile_source(source):
    outputs = [run([compiler, source]) for compiler in compilers]
    assert all(result == outputs[0] for result in outputs), (source, outputs)
    assert outputs[0][0] == 0 and not outputs[0][2], (source, outputs[0])
    return outputs[0][1]


minimum = -(1 << 63)
maximum = (1 << 63) - 1
values = [minimum, minimum + 1, maximum - 1, maximum, -1000000000, 1000000000,
          -10, -9, -8, -7, -1, 0, 1, 7, 8, 9, 10]
random_values = random.Random(136)
values += [random_values.randint(minimum, maximum) for _ in range(96)]
literals = []
for value in values:
    for padding in (0, 1, 19, 64):
        literal = ("-" if value < 0 else "") + "0" * padding + str(abs(value))
        literals.append((literal, str(value)))
literals += [("-0", "0"), ("-0000", "0"), ("0" * 65536 + "9", "9"),
             ("-" + "0" * 65536 + "9223372036854775808", str(minimum))]
source = root / "values.slim"
source.write_text("module integers\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n"
                  + "".join(f'  io.print_i64({literal})\n  io.println("")\n'
                            for literal, _ in literals) + "  0\n")
generated = root / "values.c"
generated.write_bytes(compile_source(source))
expected = "".join(value + "\n" for _, value in literals).encode()
for mode, flags in [("O0", ["-O0"]), ("O2", ["-O2"])]:
    executable = root / mode
    result = run(["clang", "-std=c11", "-Wall", "-Wextra", "-Werror", *flags,
                  "-I", "runtime", generated, "runtime/slim_rt.c", "-o", executable])
    assert result == (0, b"", b""), result
    assert run([executable]) == (0, expected, b""), mode
if sanitized:
    executable = root / "sanitized-values"
    result = run(["clang", "-std=c11", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
                  "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-I", "runtime",
                  generated, "runtime/slim_rt.c", "-o", executable])
    assert result == (0, b"", b""), result
    assert run([executable]) == (0, expected, b""), "sanitized values"
print("integer-values", len(literals), "exact-decimal-output", sep="\t", flush=True)

outside = [str(maximum + 1), str(minimum - 1), str(maximum + 2), str(minimum - 2),
           "10000000000000000000", "-10000000000000000000", "9" * 65536,
           "-" + "9" * 65536]
for number, literal in enumerate(outside):
    for padding in (0, 64):
        negative = literal.startswith("-")
        literal_value = ("-" if negative else "") + "0" * padding + literal.lstrip("-")
        prefix = "module outside\n\nfn main(args: Vec[Bytes]) -> I64:\n  "
        rejected = root / "outside.slim"
        rejected.write_text(prefix + literal_value + "\n")
        expected_error = f"E0361@{len(prefix)}:{len(prefix) + len(literal_value)}\n".encode()
        for command in (["check", rejected], [rejected]):
            outputs = [run([compiler, *command]) for compiler in compilers]
            assert all(result == outputs[0] for result in outputs), (number, outputs)
            assert outputs[0] == (1, expected_error, b""), (number, padding, outputs[0])
        status, stdout, stderr = run(["./slimc", "--message-format=json", "check", rejected])
        assert status != 0 and not stdout, (status, stdout, stderr)
        diagnostics = [json.loads(line) for line in stderr.splitlines()]
        assert len(diagnostics) == 1, diagnostics
        diagnostic = diagnostics[0]
        assert diagnostic["code"] == "E0361" and diagnostic["span"]["start"] == len(prefix), diagnostic
        assert diagnostic["span"]["end"] == len(prefix) + len(literal_value), diagnostic
        print("integer-reject", number, padding, len(literal_value), "E0361", sep="\t", flush=True)

if sanitized:
    small = root / "minimum.slim"
    small.write_text('module minimum\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n'
                     '  io.print_i64(-9223372036854775808)\n  0\n')
    expected_c = compile_source(small)
    failed = succeeded = 0
    for ordinal in range(1, 513):
        env = dict(os.environ, SLIM_ALLOC_FAIL_AT=str(ordinal))
        outputs = [run([compiler, small], env=env) for compiler in compilers]
        assert outputs[0] == outputs[1], (ordinal, outputs)
        status, stdout, stderr = outputs[0]
        if status == 71:
            assert not stdout and stderr, (ordinal, outputs[0])
            failed += 1
        else:
            assert outputs[0] == (0, expected_c, b""), (ordinal, outputs[0])
            succeeded += 1
        print("integer-fault", ordinal, status, sep="\t", flush=True)
    assert failed > 0 and succeeded > 0, (failed, succeeded)
    print("integer-fault-summary", failed, succeeded, sep="\t", flush=True)
