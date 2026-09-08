"""Native input capture: exact bytes, failure rejection and declared capacities."""
import argparse
import hashlib
import os
from pathlib import Path
import stat
import subprocess
import tempfile


def verify(helper, large):
    helper = str(Path(helper).resolve())
    with tempfile.TemporaryDirectory(prefix='slim-copy-tests-') as temporary:
        root = Path(temporary)
        source = root / 'source.bin'
        content = bytes(range(256)) * 513
        source.write_bytes(content)
        cases = 0

        def run(name, plan, expected, paired='0', fault=None, rows=0):
            nonlocal cases
            directory = root / ('case-' + name)
            directory.mkdir()
            if name == 'symlink-parent':
                (directory / 'sdk').symlink_to(root, target_is_directory=True)
            if name == 'symlink-leaf':
                (directory / 'sdk').mkdir()
                (directory / 'sdk/output').symlink_to(source)
            if name == 'directory-parent':
                (directory / 'sdk').write_bytes(b'occupied')
            environment = os.environ.copy()
            environment.pop('SLIM_COPY_TEST_FAULT', None)
            if fault:
                environment['SLIM_COPY_TEST_FAULT'] = fault
            result = subprocess.run([helper, paired], input=plan, cwd=directory, env=environment,
                                    capture_output=True, timeout=120)
            assert result.returncode == expected, (name, result.returncode, result.stderr)
            actual = result.stdout.decode().splitlines()
            assert len(actual) == rows, (name, actual)
            for row in actual:
                origin, relative, size, checksum = row.split('\t')
                target = directory / relative
                assert target.is_file() and not target.is_symlink(), (name, target)
                assert target.stat().st_size == int(size)
                left, right = hashlib.sha256(), hashlib.sha256()
                with Path(origin).open('rb') as a, target.open('rb') as b:
                    while True:
                        original, copied = a.read(1048576), b.read(1048576)
                        assert original == copied, (name, relative)
                        if not original:
                            break
                        left.update(original); right.update(copied)
                assert left.hexdigest() == right.hexdigest() == checksum, (name, row)
                mode = stat.S_IMODE(target.stat().st_mode)
                expected_mode = 0o500 if Path(origin).stat().st_mode & 0o111 else 0o400
                assert mode == expected_mode, (name, mode, expected_mode)
            cases += 1
            print(f'native-copy\t{name}\t{expected}\t{rows}', flush=True)
            return directory

        def line(path=source, target='sdk/output'):
            return f'{path}\t{target}\n'.encode()

        run('bytes', line(), 0, rows=1)
        source.chmod(0o700)
        run('executable', line(target='toolchain/bin/tool'), 0, rows=1)
        source.chmod(0o600)
        empty = root / 'empty'; empty.write_bytes(b'')
        run('empty', line(empty), 0, rows=1)
        run('paired-limit', line(empty), 0, paired='1048576', rows=1)
        for name, paired in [('negative', '-1'), ('over-paired', '1048577'), ('overflow', '18446744073709551616'),
                             ('non-number', 'x'), ('empty-number', '')]:
            run(name, line(), 64, paired=paired)
        for name, data in [
            ('unterminated', line()[:-1]), ('missing-tab', str(source).encode()+b'\n'),
            ('extra-tab', line().replace(b'\tsdk/', b'\t\tsdk/')),
            ('nul', line().replace(b'\tsdk/', b'\t\0sdk/')),
            ('line-overflow', b'x'*8195+b'\n'),
            ('relative-source', b'source.bin\tsdk/output\n'),
            ('absolute-target', line(target=str(root/'escaped'))),
            ('wrong-target-root', line(target='other/output')),
            ('parent-escape', line(target='sdk/../escaped')),
            ('dot-component', line(target='sdk/./output')),
            ('space-target', line(target='sdk/with space')),
            ('missing-source', line(root/'absent')),
            ('directory-source', line(root))]:
            run(name, data, 2)
        for name in ['symlink-parent', 'symlink-leaf', 'directory-parent']:
            run(name, line(), 2)
        assert source.read_bytes() == content and not (root/'escaped').exists() and not (root/'output').exists()
        run('duplicate-target', line()+line(), 2, rows=1)
        tiny = root/'tiny'; tiny.write_bytes(b'x')
        plan = b''.join(line(tiny, f'sdk/input-{i}') for i in range(510))
        run('files-512', plan, 0, rows=510)
        bounded = run('files-513', plan+line(tiny,'sdk/extra'), 2, rows=510)
        assert not (bounded/'sdk/extra').exists()
        run('forced-copy', line(), 0, fault='fallback', rows=1)
        for fault in ['changed-copy', 'short-copy', 'grown-copy']:
            run(fault, line(), 2, fault=fault)
        if large:
            sparse = root/'sparse'
            capacity = 536870912 - 1048576
            with sparse.open('wb') as output:
                output.truncate(capacity)
            run('bytes-512MiB', line(sparse), 0, paired='1048576', rows=1)
            with sparse.open('ab') as output:
                output.truncate(capacity+1)
            rejected = run('bytes-512MiB-plus-one', line(sparse), 2, paired='1048576')
            assert not (rejected/'sdk/output').exists()
    print(f'native-copy\t{cases} exact byte/path/failure/capacity cases', flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('helper')
    parser.add_argument('--large', action='store_true')
    args = parser.parse_args()
    verify(args.helper, args.large)
