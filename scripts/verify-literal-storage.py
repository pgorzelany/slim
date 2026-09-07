#!/usr/bin/env python3
"""Check literal storage through the production compiler and native C backend."""

import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('compiler')
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    compiler = Path(args.compiler).resolve()
    root = Path(__file__).resolve().parent.parent
    environment = dict(os.environ)
    environment.pop('SLIM_ALLOC_FAIL_AT', None)
    expected = b'0i64.add|true|42|quote:" slash:\\|\n\r\t|\x00Z\xff|named|returned|last|' * 2
    with tempfile.TemporaryDirectory(prefix='slim-literal-storage-') as temporary:
        directory = Path(temporary)
        fixture = root / 'tests/fixtures/literal_storage.slim'
        generated = subprocess.run([compiler, fixture], capture_output=True, env=environment, check=True)
        assert not generated.stderr, generated.stderr
        repeated = subprocess.run([compiler, fixture], capture_output=True, env=environment, check=True)
        assert generated.stdout == repeated.stdout and not repeated.stderr
        source = directory / 'program.c'
        source.write_bytes(generated.stdout)
        executable = directory / 'program'
        flags = ['-std=c11', '-Wall', '-Wextra', '-Werror']
        if args.sanitize:
            flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
        else:
            flags += ['-O2']
        subprocess.run(shlex.split(os.environ.get('CC', 'cc')) + flags + [
            '-I', str(root / 'runtime'), str(source), str(root / 'runtime/slim_rt.c'),
            '-o', str(executable)], check=True, env=environment)
        ordinary = subprocess.run([executable], capture_output=True, env=environment)
        assert (ordinary.returncode, ordinary.stdout, ordinary.stderr) == (0, expected, b''), ordinary
        failed = 0
        for ordinal in range(1, 65):
            run = subprocess.run([executable], capture_output=True,
                                 env={**environment, 'SLIM_ALLOC_FAIL_AT': str(ordinal)})
            if run.returncode == 71:
                failed += 1
                assert run.stdout == b''
                assert run.stderr == f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode()
            else:
                assert (run.returncode, run.stdout, run.stderr) == (0, expected, b''), (ordinal, run)
        # One argv allocation and two growths each for vector and arena. A
        # literal descriptor uses automatic C storage, with no heap allocation.
        assert failed == 5, failed
        rejected = [
            ('Vec', 'I64', ' effects[alloc]', 'vec.push(@values, "wrong")', b'E0344@121:128\n'),
            ('Arena', 'I64', ' effects[alloc]', 'let identity: Id[I64] = arena.add(@values, "wrong")', b'E0344@150:157\n'),
            ('Vec', 'Bytes', ' effects[alloc]', 'vec.push(values, "wrong")', b'E0360@114:120\n'),
            ('Vec', 'Bytes', '', 'vec.push(@values, "wrong")', b'E0343@78:85\n'),
        ]
        for container, element, effects, operation, diagnostic in rejected:
            invalid = directory / 'rejected.slim'
            invalid.write_text(f'module rejected\n\nfn main(args: Vec[Bytes]) -> I64{effects}:\n'
                               f'  let values: {container}[{element}] = {container.lower()}.new()\n'
                               f'  {operation}\n  0\n')
            for command in ([compiler, 'check', invalid], [compiler, invalid]):
                run = subprocess.run(command, capture_output=True, env=environment)
                assert (run.returncode, run.stdout, run.stderr) == (1, diagnostic, b''), run
    print(f'literal storage: exact bytes, deterministic C, 4 ordered diagnostics; '
          f'64 fault positions, {failed} failures, {64 - failed} successes; sanitized={args.sanitize}')


if __name__ == '__main__':
    main()
