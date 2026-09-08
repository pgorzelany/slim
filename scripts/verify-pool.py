#!/usr/bin/env python3
"""Explicit bounded native checks for RFC-0154; no SLIM semantic authority."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import time

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts/models'))
from m2_pool import INITIAL, REQUESTS, MAX_STATES, MAX_TRANSITIONS, oracle  # noqa: E402


def checked(argv, output, **kwargs):
    start = time.monotonic()
    result = subprocess.run(argv, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            timeout=120, **kwargs)
    output.append({'argv': list(map(str, argv)), 'exit': result.returncode,
                   'elapsed_seconds': round(time.monotonic() - start, 6),
                   'stdout': result.stdout.decode(errors='replace'),
                   'stderr': result.stderr.decode(errors='replace')})
    if result.returncode:
        raise RuntimeError(f'command failed: {argv}\n{result.stderr.decode(errors="replace")}')
    return result.stdout


def cases():
    seen = {INITIAL}
    frontier = [(INITIAL, ())]
    result = []
    for _ in range(4):
        following = []
        for state, path in frontier:
            actions = [('allocate', size, fail) for size in REQUESTS for fail in (False, True)]
            actions += [('release', item[0], False) for item in state.live]
            actions += [('release', -1, False)]
            for action in actions:
                if len(result) >= MAX_TRANSITIONS:
                    raise RuntimeError('native model transition budget exhausted: unknown')
                _, successor = oracle(state, action)
                trace = (*path, action)
                result.append(trace)
                if successor not in seen:
                    if len(seen) >= MAX_STATES:
                        raise RuntimeError('native model state budget exhausted: unknown')
                    seen.add(successor)
                    following.append((successor, trace))
        frontier = following
    assert len(result) == 1015 and len(seen) == 104
    return result


def campaign(traces, pattern):
    commands, expected, labels = [], [], []
    for trace_number, trace in enumerate(traces):
        commands.append('R')
        state, slots, headers = INITIAL, {}, {}
        for step, action in enumerate(trace):
            kind, value, fail = action
            result, successor = oracle(state, action)
            if kind == 'allocate':
                slot = next(i for i in range(15) if i not in slots.values())
                recursive = pattern == 1 or (pattern == 2 and step % 2 == 1)
                header = 64 if recursive else 16
                payload = value * 8192 - header if value else 0
                commands.append(f'A {slot} {payload} {int(recursive)} {int(fail)}')
                offset = result[1] * 8192 if result[0] == 'allocated' else 2**64 - 1
                expected.append(f'A {1 if result[0] == "exhausted" else 0} {offset}')
                if result[0] == 'allocated':
                    slots[result[1]] = slot
                    headers[result[1]] = header
            else:
                slot = slots.get(value, 15)
                commands.append(f'F {slot}')
                expected.append(f'F {1 if result[0] == "released" else 0}')
                if result[0] == 'released':
                    del slots[value]
                    del headers[value]
            labels.append({'trace': trace_number, 'step': step, 'action': action})
            commands.append('S')
            requested = sum(size * 8192 - headers[offset] for offset, _, size in successor.live)
            rounded = sum(8192 * 2**order for _, order, _ in successor.live)
            snapshot = f'S {len(successor.live)} {requested} {rounded}'
            snapshot += ''.join(f' {order+7}:{offset*8192}' for order, offset in successor.free)
            expected.append(snapshot)
            labels.append({'trace': trace_number, 'step': step, 'snapshot': True})
            state = successor
    return ('\n'.join(commands)+'\n').encode(), expected, labels


def verify(binary, traces, output, directory, label):
    checked([str(binary), 'selftest'], output)
    reports = []
    for pattern in range(3):
        commands, expected, labels = campaign(traces, pattern)
        start = time.monotonic()
        result = subprocess.run([str(binary), 'protocol'], input=commands, cwd=ROOT,
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=120)
        (directory / f'{label}-{pattern}.stdin').write_bytes(commands)
        (directory / f'{label}-{pattern}.stdout').write_bytes(result.stdout)
        (directory / f'{label}-{pattern}.stderr').write_bytes(result.stderr)
        actual = result.stdout.decode().splitlines()
        if result.returncode or actual != expected:
            mismatch = next((i for i, (left, right) in enumerate(zip(actual, expected)) if left != right),
                            min(len(actual), len(expected)))
            detail = {'variant': label, 'pattern': pattern, 'line': mismatch,
                      'context': labels[mismatch] if mismatch < len(labels) else None,
                      'expected': expected[mismatch] if mismatch < len(expected) else None,
                      'actual': actual[mismatch] if mismatch < len(actual) else None,
                      'exit': result.returncode, 'stderr': result.stderr.decode(errors='replace')}
            raise AssertionError(json.dumps(detail, sort_keys=True))
        reports.append({'pattern': pattern, 'traces': len(traces), 'compared_lines': len(expected),
                        'elapsed_seconds': round(time.monotonic()-start,6),
                        'input_sha256': hashlib.sha256(commands).hexdigest(),
                        'output_sha256': hashlib.sha256(result.stdout).hexdigest()})
    return reports


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    if not __debug__:
        parser.error('verification assertions must remain enabled')
    directory = args.output.resolve()
    directory.mkdir(parents=True, exist_ok=True)
    commands = []
    report = {'classification': 'bounded', 'source_integration': 'not implemented',
              'commands': commands, 'variants': {}}
    paths = ['runtime/slim_pool.c', 'runtime/slim_pool.h', 'tests/fixtures/pool_component.c',
             'tests/fixtures/pool_observer.c', 'scripts/models/m2_pool.py', 'scripts/verify-pool.py']
    report['source_hashes'] = {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths}
    try:
        traces = cases()
        for variant in (['ordinary', 'sanitized'] if args.sanitize else ['ordinary']):
            flags = ['-std=c11', '-Wall', '-Wextra', '-Werror', '-I', str(ROOT/'runtime')]
            flags += ['-O2'] if variant == 'ordinary' else [
                '-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                '-DSLIM_POOL_ASAN', '-DSLIM_POOL_INSTRUMENT']
            obj = directory / f'pool-{variant}.o'
            binary = directory / f'pool-{variant}'
            checked(['cc', *flags, '-Dmalloc=pool_test_malloc', '-Dfree=pool_test_free',
                     '-c', 'runtime/slim_pool.c', '-o', str(obj)], commands)
            checked(['cc', *flags, 'tests/fixtures/pool_component.c', 'tests/fixtures/pool_observer.c',
                     str(obj), '-o', str(binary)], commands)
            report['variants'][variant] = verify(binary, traces, commands, directory, variant)
            if variant == 'sanitized':
                for witness in ('uaf', 'overrun'):
                    result = subprocess.run([str(binary), witness], cwd=ROOT, stdout=subprocess.PIPE,
                                            stderr=subprocess.PIPE, timeout=120)
                    stderr = result.stderr.decode(errors='replace')
                    (directory / f'{witness}.stderr').write_text(stderr)
                    if result.returncode == 0 or 'ERROR: AddressSanitizer' not in stderr or 'WRITE of size 1' not in stderr or 'pool_component.c' not in stderr:
                        raise AssertionError(f'{witness} was not rejected by the intended sanitizer witness: {stderr}')
                    report[witness] = {'exit': result.returncode, 'stderr_sha256': hashlib.sha256(result.stderr).hexdigest()}
        report['result'] = 'passed'
    except Exception as error:
        report['result'] = 'failed'
        report['error'] = str(error)
        raise
    finally:
        (directory/'receipt.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'result': report['result'], 'variants': list(report['variants']),
                      'traces_per_pattern': 1015, 'patterns_per_variant': 3,
                      'receipt': str(directory/'receipt.json')}, sort_keys=True))


if __name__ == '__main__':
    main()
