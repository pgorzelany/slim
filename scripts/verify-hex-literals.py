#!/usr/bin/env python3
"""Independent native oracle for hexadecimal and trigraph byte-literal boundaries."""

import argparse
import itertools
import os
from pathlib import Path
import shlex
import subprocess
import tempfile


def run(command, environment):
    return subprocess.run(command, capture_output=True, env=environment, timeout=120)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('compiler')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--baseline')
    args = parser.parse_args()
    compiler = Path(args.compiler).resolve()
    root = Path(__file__).resolve().parent.parent
    environment = dict(os.environ)
    environment.pop('SLIM_ALLOC_FAIL_AT', None)
    digits = '0123456789ABCDEFabcdef'
    spellings = [''.join(pair) for pair in itertools.product(digits, repeat=2)]
    assert len(spellings) == 484
    controls = [
        (r'""', b''),
        (r'"\x00"', b'\x00'),
        (r'"\xff"', b'\xff'),
        (r'"\x00G"', b'\x00G'),
        (r'"\xffg"', b'\xffg'),
        (r'"\\x00A"', b'\\x00A'),
        (r'"\\\\x00A"', b'\\\\x00A'),
        (r'"\"quoted\"\\"', b'"quoted"\\'),
        (r'"\n\r\t"', b'\n\r\t'),
        (r'"\x00\x41"', b'\x00A'),
        (r'"?"', b'?'),
        (r'"??"', b'??'),
        (r'"??x"', b'??x'),
        (r'"?/?=?!"', b'?/?=?!'),
        (r'"\x3f?/"', b'??/'),
        (r'"\x3f\x3f/"', b'??/'),
    ]
    for code in range(32, 127):
        suffix = chr(code)
        if suffix not in "=/'()!<>-":
            escaped = suffix.replace('\\', '\\\\').replace('"', '\\"')
            controls.append(('"??' + escaped + '"', b'??' + suffix.encode()))
    extended = [
        (r'"\\\x00A"', b'\\\x00A'),
        (r'"\"\x00A\""', b'"\x00A"'),
        (r'"\x00ABCDEF0123456789"', b'\x00ABCDEF0123456789'),
        (r'"\x00\x41A"', b'\x00AA'),
        (r'"\x00A\x41B\xffF"', b'\x00AAB\xffF'),
        (r'"??\x00A"', b'??\x00A'),
        (r'"\x00A??/"', b'\x00A??/'),
    ]
    with tempfile.TemporaryDirectory(prefix='slim-hex-literals-') as temporary:
        directory = Path(temporary)
        invalid = [
            ('module hex_type\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n'
             '  let values: Vec[I64] = vec.new()\n  vec.push(@values, "\\x00A")\n  0\n',
             b'E0344@121:128\n'),
            ('module hex_unclosed\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n'
             '  io.print_bytes("\\x00A)\n  0\n', b'E0107@84:85\n'),
        ]
        for body, diagnostic in invalid:
            source = directory / 'invalid.slim'
            source.write_text(body)
            for command in ([compiler, 'check', source], [compiler, source]):
                rejected = run(command, environment)
                assert (rejected.returncode, rejected.stdout, rejected.stderr) == (1, diagnostic, b'')
        flags = ['-std=c11', '-Wall', '-Wextra', '-Werror']
        flags += (['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
                  if args.sanitize else ['-O2'])

        def verify(label, groups, unchanged=False):
            source = directory / f'{label}.slim'
            body = 'module hex_literals\n\n'
            expected = bytearray()
            boundaries = []
            for index, cases in enumerate(groups):
                body += f'fn group_{index}() -> Void effects[io]:\n'
                for literal, value in cases:
                    body += (f'  io.print_i64(bytes.len({literal}))\n'
                             '  io.print_bytes(":")\n'
                             f'  io.print_bytes({literal})\n'
                             '  io.println("")\n')
                    framed = str(len(value)).encode() + b':' + value + b'\n'
                    boundaries.append((len(expected), len(expected) + len(framed), literal))
                    expected.extend(framed)
                body += '\n'
            body += 'fn main(args: Vec[Bytes]) -> I64 effects[io]:\n'
            body += ''.join(f'  group_{index}()\n' for index in range(len(groups)))
            body += '  0\n'
            source.write_text(body)
            checked = run([compiler, 'check', source], environment)
            assert (checked.returncode, checked.stdout, checked.stderr) == (0, b'', b''), checked
            generated = run([compiler, source], environment)
            assert generated.returncode == 0 and not generated.stderr, generated
            repeated = run([compiler, source], environment)
            assert (repeated.returncode, repeated.stdout, repeated.stderr) == (0, generated.stdout, b'')
            if unchanged and args.baseline:
                previous = run([str(Path(args.baseline).resolve()), source], environment)
                assert (previous.returncode, previous.stdout, previous.stderr) == (0, generated.stdout, b'')
            native_source = directory / f'{label}.c'
            native_source.write_bytes(generated.stdout)
            executable = directory / label
            built = run(shlex.split(os.environ.get('CC', 'cc')) + flags + [
                '-I', str(root / 'runtime'), str(native_source), str(root / 'runtime/slim_rt.c'),
                '-o', str(executable)], environment)
            assert built.returncode == 0, (label, built.stderr[:2000])
            native = run([executable], environment)
            assert native.returncode == 0 and not native.stderr, (label, native)
            if native.stdout != expected:
                mismatch = next((at for at, pair in enumerate(zip(native.stdout, expected))
                                 if pair[0] != pair[1]), min(len(native.stdout), len(expected)))
                witness = next((row for row in boundaries if row[0] <= mismatch < row[1]),
                               (len(expected), len(expected), 'after final literal'))
                start, end, literal = witness
                raise AssertionError((label, 'first differing byte', mismatch, literal,
                                      bytes(expected[start:end]), native.stdout[start:end]))
            for ordinal in range(1, 9):
                faulty = run([executable], {**environment, 'SLIM_ALLOC_FAIL_AT': str(ordinal)})
                if ordinal == 1:
                    assert (faulty.returncode, faulty.stdout, faulty.stderr) == (
                        71, b'', b'SLIM allocation failure: exhausted at allocation 1\n')
                else:
                    assert (faulty.returncode, faulty.stdout, faulty.stderr) == (0, expected, b'')
            print('hex literals:', label, sum(map(len, groups)), 'exact cases;',
                  len(expected), 'native bytes; 8 fault positions', flush=True)

        verify('controls', [controls[start:start + 16] for start in range(0, len(controls), 16)],
               unchanged=True)
        verify('extended', [extended])
        trigraphs = []
        for suffix in "=/'()!<>-":
            for length in range(2, 9):
                value = '?' * length + suffix
                trigraphs.append(('"' + value + '"', value.encode()))
            value = '??' + suffix
            trigraphs.extend([
                ('"' + value * 2 + '"', (value * 2).encode()),
                ('"\\\\' + value + '"', b'\\' + value.encode()),
                ('"\\x3f' + value + '"', b'?' + value.encode()),
            ])
        verify('trigraphs', [trigraphs[start:start + 16] for start in range(0, len(trigraphs), 16)])
        assert len(trigraphs) == 90
        total = 0
        # Small helper bodies and at most 64 calls in main stay within the
        # separately documented recursive name-resolution prepass test domain.
        for start in range(0, len(spellings), 64):
            groups = []
            for spelling in spellings[start:start + 64]:
                groups.append([(f'"\\x{spelling}{following}"',
                                bytes([int(spelling, 16), ord(following)]))
                               for following in digits])
            verify(f'matrix-{start // 64}', groups)
            total += sum(map(len, groups))
        assert total == 10648
    print(f'byte literals: {total} exhaustive hex boundary cases, {len(trigraphs)} trigraph cases, '
          f'{len(controls)} controls, {len(extended)} composite cases, 2 exact diagnostics; '
          f'sanitized={args.sanitize}')


if __name__ == '__main__':
    main()
