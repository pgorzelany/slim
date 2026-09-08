#!/usr/bin/env python3
"""Bounded native pool/task witness; never a production SLIM semantic check."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if not __debug__:
        parser.error('verification assertions must remain enabled')
    directory = args.output.resolve()
    directory.mkdir(parents=True, exist_ok=True)
    report = {'classification': 'bounded native witness', 'source_checking': 'not implemented',
              'commands': [], 'cases': []}
    paths = ['runtime/slim_pool.c', 'runtime/slim_pool.h', 'runtime/slim_rt.c',
             'runtime/slim_rt.h', 'tests/fixtures/pool_workers.c',
             'tests/fixtures/pool_observer.c', 'scripts/verify-pool-workers.py']
    report['source_hashes'] = {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths}
    environment = {k: v for k, v in os.environ.items() if not k.startswith('SLIM_')}

    def execute(argv, settings=None, exit_code=0):
        result = subprocess.run(list(map(str, argv)), cwd=ROOT,
                                env={**environment, **(settings or {})},
                                capture_output=True, timeout=30)
        item = {'argv': list(map(str, argv)), 'settings': settings or {},
                'exit': result.returncode, 'stdout': result.stdout.decode(errors='replace'),
                'stderr': result.stderr.decode(errors='replace')}
        report['commands'].append(item)
        assert result.returncode == exit_code, item
        return item

    try:
        for sanitized in (False, True):
            for tier in ('serial', 'posix'):
                label = f'{tier}-{"sanitized" if sanitized else "ordinary"}'
                flags = ['-std=c11', '-Wall', '-Wextra', '-Werror', '-DSLIM_PARALLEL',
                         '-I', str(ROOT/'runtime')]
                flags += ['-O1', '-g', '-fsanitize=address,undefined',
                          '-fno-omit-frame-pointer', '-DSLIM_POOL_ASAN'] if sanitized else ['-O2']
                if tier == 'posix':
                    flags += ['-DSLIM_POSIX_WORKERS', '-pthread']
                obj, binary = directory/f'{label}.o', directory/label
                execute(['cc', *flags, '-Dmalloc=pool_test_malloc', '-Dfree=pool_test_free',
                         '-c', 'runtime/slim_pool.c', '-o', obj])
                execute(['cc', *flags, 'tests/fixtures/pool_workers.c',
                         'tests/fixtures/pool_observer.c', 'runtime/slim_rt.c', obj, '-o', binary])
                modes = [('normal', {})] if tier == 'serial' else [
                    ('normal', {}), ('disabled', {'SLIM_TASK_DISABLE': '1'}),
                    ('declined', {'SLIM_TASK_FAIL_AT': '1'})]
                for mode, settings in modes:
                    for mask in range(4):
                        for first in range(2):
                            for fail_at in range(3):
                                item = execute([binary, mask, first, fail_at], settings)
                                attempts = mask.bit_count()
                                failed = 0 < fail_at <= attempts
                                spawned = int(tier == 'posix' and mode == 'normal')
                                expected = (f'admission_failed attempt={fail_at} tasks=0\n' if failed
                                            else f'returned mask={mask} spawned={spawned} first={first if spawned else 1} completed=2 metadata=unchanged reused=1\n')
                                assert item['stdout'] == expected and item['stderr'] == '', item
                                report['cases'].append({'build': label, 'mode': mode,
                                                        'mask': mask, 'first': first,
                                                        'fail_at': fail_at, 'admission_failed': failed})
                if tier == 'posix':
                    item = execute([binary, 3, 1, 0], {'SLIM_TASK_JOIN_FAIL_AT': '1'}, 70)
                    assert item['stderr'] == 'SLIM runtime trap: injected structured task join failure\n', item
        assert len(report['cases']) == 192
        report['result'] = 'passed'
    except Exception as error:
        report['result'] = 'failed'
        report['error'] = str(error)
        raise
    finally:
        (directory/'receipt.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'result': report['result'], 'cases': len(report['cases']),
                      'receipt': str(directory/'receipt.json')}, sort_keys=True))


if __name__ == '__main__':
    main()
