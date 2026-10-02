#!/usr/bin/env python3
"""Bounded native host verification; no source semantic acceptance."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import socket
import subprocess
import sys
import threading
import time

ROOT = Path(__file__).resolve().parents[1]
CALLS = ['open', 'close', 'read', 'write', 'socket', 'connect', 'shutdown', 'send',
         'recv', 'poll', 'fcntl', 'setsockopt', 'getsockopt']


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if not __debug__:
        parser.error('verification assertions must remain enabled')
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    report = {'result': 'running', 'classification': 'bounded native component',
              'source_adoption': 'not implemented', 'commands': [], 'cases': []}
    paths = ['runtime/slim_host.c', 'runtime/slim_host.h', 'runtime/slim_pool.c',
             'runtime/slim_pool.h', 'runtime/slim_rt.c', 'runtime/slim_rt.h',
             'tests/fixtures/host_component.c', 'tests/fixtures/host_faults.c',
             'tests/fixtures/host_pool_observer.c', 'tests/fixtures/clock_boundary.c',
             'scripts/verify-host.py']
    report['sources'] = {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths}
    env = {k: v for k, v in os.environ.items() if not k.startswith(('SLIM_', 'HOST_'))}

    def run(argv, settings=None, code=0):
        result = subprocess.run(list(map(str, argv)), cwd=ROOT, env={**env, **(settings or {})},
                                capture_output=True, timeout=30)
        item = {'argv': list(map(str, argv)), 'settings': settings or {},
                'exit': result.returncode, 'stdout': result.stdout.decode(errors='replace'),
                'stdout_hex': result.stdout.hex(), 'stderr': result.stderr.decode(errors='replace')}
        report['commands'].append(item)
        assert result.returncode == code, item
        return item

    def check_result(item, success, prefix, payload=b''):
        expected = bytes(range(240, 240+prefix)) + (payload if success else b'')
        assert item['stdout'] == f'{int(success)} {len(expected)} {expected.hex()}\n', item
        assert item['stderr'].startswith('opens=') and item['stderr'].endswith(' live=0\n'), item

    def tcp_case(binary, payload, limit, settings=None, delay=0, ipv6=False, binary_request=False):
        received, errors = [], []
        address = '::1' if ipv6 else '127.0.0.1'
        listener = socket.socket(socket.AF_INET6 if ipv6 else socket.AF_INET)
        listener.bind((address, 0))
        listener.listen(1)
        listener.settimeout(.4)
        port = listener.getsockname()[1]

        def serve():
            try:
                with listener.accept()[0] as stream:
                    stream.settimeout(2)
                    request = bytearray()
                    while True:
                        chunk = stream.recv(4096)
                        if not chunk:
                            break
                        request.extend(chunk)
                    received.append(bytes(request))
                    if delay:
                        time.sleep(delay)
                    stream.sendall(payload)
            except (TimeoutError, BrokenPipeError, ConnectionResetError):
                pass
            except Exception as error:
                errors.append(repr(error))
            finally:
                listener.close()

        thread = threading.Thread(target=serve)
        thread.start()
        try:
            item = run([binary, 'tcp-binary' if binary_request else 'tcp', limit, 3, limit+3, address, port, 'REQUEST', 25 if delay else 1000], settings)
        finally:
            thread.join(3)
        assert not thread.is_alive() and not errors, errors
        success = not settings and not delay and len(payload) <= limit
        if settings and settings.get('HOST_ERROR') == 'eintr':
            success = len(payload) <= limit
        if settings and settings.get('HOST_SHORT'):
            success = len(payload) <= limit
        check_result(item, success, 3, payload)
        if success:
            assert received == [b'\x00\xffX' if binary_request else b'REQUEST'], received
        return item

    try:
        for sanitized in (False, True):
            label = 'sanitized' if sanitized else 'ordinary'
            flags = ['-std=c11', '-Wall', '-Wextra', '-Werror', '-pthread', '-I', str(ROOT/'runtime')]
            flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
                      '-fno-omit-frame-pointer', '-DSLIM_POOL_ASAN'] if sanitized else ['-O2']
            pool = output/f'pool-{label}.o'
            run(['cc', *flags, '-Dmalloc=host_pool_malloc', '-Dfree=host_pool_free', '-c',
                 'runtime/slim_pool.c', '-o', pool])
            for tier in ('posix', 'portable'):
                host = output/f'host-{label}-{tier}.o'
                binary = output/f'host-{label}-{tier}'
                options = ['-DSLIM_HOST_POSIX'] if tier == 'posix' else []
                run(['cc', *flags, *options, *[f'-D{c}=host_probe_{c}' for c in CALLS],
                     '-c', 'runtime/slim_host.c', '-o', host])
                run(['cc', *flags, 'tests/fixtures/host_component.c', 'tests/fixtures/host_faults.c',
                     'tests/fixtures/host_pool_observer.c', 'runtime/slim_rt.c', pool, host, '-o', binary])
                item = run([binary, 'arguments'])
                assert item['stdout'] == 'arguments passed\n' and not item['stderr'], item
                item = run([binary, 'invalid'])
                assert item['stdout'] == 'invalid passed\n' and not item['stderr'], item
                count = 0
                for limit in (-1, 0, 1, 63, 64, 65):
                    for prefix in (0, 3):
                        for spare in sorted({max(0, limit-1), max(0, limit), max(0, limit+1)}):
                            for size in sorted({0, max(0, limit-1), max(0, limit), max(0, limit+1)}):
                                payload = bytes(i % 256 for i in range(size))
                                path = output/'input'
                                path.write_bytes(payload)
                                item = run([binary, 'file', limit, prefix, prefix+spare, path, 'plain'])
                                success = tier == 'posix' and 0 <= size <= limit <= spare
                                check_result(item, success, prefix, payload)
                                if not (0 <= limit <= spare) or tier == 'portable':
                                    assert item['stderr'] == 'opens=0 closes=0 live=0\n', item
                                count += 1
                report['cases'].append({'build': str(binary), 'file_matrix': count})
                path = output/'input'
                path.write_bytes(b'content')
                for name, kind in [('', 'plain'), ('abc', 'nul-first'), ('abc', 'nul'), ('abc', 'nul-last'), ('x'*4095, 'plain'), ('x'*4096, 'plain'), (str(output), 'plain')]:
                    item = run([binary, 'file', 8, 3, 11, name, kind])
                    check_result(item, False, 3)
                    if len(name) > 4095 or kind.startswith('nul') or tier == 'portable':
                        assert item['stderr'] == 'opens=0 closes=0 live=0\n', item
                if tier == 'portable':
                    item = run([binary, 'tcp', 8, 3, 11, '127.0.0.1', 80, 'REQUEST', 1000])
                    check_result(item, False, 3)
                    run([binary, 'line', 'unsupported'], code=2)
                    continue
                for size in (1048575, 1048576, 1048577):
                    large = output/'large-input'
                    large.write_bytes((bytes(range(256)) * (size//256+1))[:size])
                    item = run([binary, 'large-file', size, large])
                    assert item['stdout'] == 'large passed\n' and not item['stderr'], item
                for operation in ('open', 'read', 'close'):
                    for kind in ('eio', 'eintr'):
                        settings = {'HOST_FAULT': operation, 'HOST_ERROR': kind}
                        item = run([binary, 'file', 8, 3, 11, path, 'plain'], settings)
                        check_result(item, kind == 'eintr' and operation != 'close', 3, b'content')
                item = run([binary, 'file', 8, 3, 11, path, 'plain'], {'HOST_SHORT': '1'})
                check_result(item, True, 3, b'content')
                for mutation, expected in [('grow', b'abcdefXYZ'), ('shrink', b'abc')]:
                    path.write_bytes(b'abcdef')
                    item = run([binary, 'file', 9, 3, 12, path, 'plain'],
                               {'HOST_SHORT': '1', 'HOST_MUTATE': mutation, 'HOST_MUTATE_PATH': str(path)})
                    check_result(item, True, 3, expected)
                for value in (-2**63, -1, 0, 1, 2**63-1):
                    item = run([binary, 'number', value])
                    assert item['stdout'] == str(value) and not item['stderr'], item
                item = run([binary, 'binary'])
                assert item['stdout_hex'] == bytes(range(256)).hex() and not item['stderr'], item
                for settings in ({}, {'HOST_SHORT': '1'}, {'HOST_FAULT': 'write', 'HOST_ERROR': 'eintr'}):
                    item = run([binary, 'line', 'fixed output'], settings)
                    assert item['stdout'] == 'fixed output\n' and not item['stderr'], item
                run([binary, 'line', 'failed'], {'HOST_FAULT': 'write'}, 2)
                run([binary, 'line', 'zero'], {'HOST_ZERO_WRITE': '1'}, 2)
                item = run([binary, 'pipe'])
                assert item['stdout'] == 'pipe passed\n' and not item['stderr'], item
                item = run([binary, 'trap-pipe'], code=70)
                assert not item['stdout'] and not item['stderr'], item
                item = run([binary, 'live-trap'], {'HOST_FORBID_FREE': '1'}, 70)
                assert item['stderr'] == 'SLIM runtime trap: live worker witness\n', item
                item = run([binary, 'live-trap-pipe'], {'HOST_FORBID_FREE': '1'}, 70)
                assert not item['stderr'] and not item['stdout'], item
                item = run([binary, 'arguments'], {'HOST_FORBID_FREE': '1'}, 99)
                assert item['stderr'] == 'forbidden pool free\n', item
                for limit in (0, 1, 64):
                    for size in sorted({0, max(0, limit-1), limit, limit+1}):
                        tcp_case(binary, bytes(i % 256 for i in range(size)), limit)
                for operation in ('socket', 'connect', 'fcntl', 'send', 'shutdown', 'recv', 'poll', 'close'):
                    tcp_case(binary, b'reply', 8, {'HOST_FAULT': operation})
                tcp_case(binary, b'reply', 8, {'HOST_FAULT': 'fcntl', 'HOST_FAULT_AT': '2'})
                tcp_case(binary, b'reply', 8, {'HOST_FAULT': 'getsockopt', 'HOST_CONNECT_PENDING': '1'})
                tcp_case(binary, b'reply', 8, {'HOST_FAULT': 'send-epipe'})
                for operation in ('send', 'recv', 'poll'):
                    tcp_case(binary, b'reply', 8, {'HOST_FAULT': operation, 'HOST_ERROR': 'eintr'})
                tcp_case(binary, b'reply', 8, {'HOST_SHORT': '1'})
                tcp_case(binary, b'reply', 8, delay=.1)
                tcp_case(binary, b'\x00\xffR', 8, ipv6=True, binary_request=True)
            for portable in (False, True):
                binary = output/f'clock-{label}-{portable}'
                override = []
                if portable:
                    header = output/'clock-portable.h'
                    header.write_text('#include <time.h>\n#undef CLOCK_MONOTONIC\n')
                    override = ['-include', str(header)]
                run(['cc', *flags, '-D_POSIX_C_SOURCE=200809L', *override,
                     '-Dclock_gettime=host_probe_clock', '-Dtimespec_get=host_probe_utc',
                     'runtime/slim_rt.c', 'tests/fixtures/clock_boundary.c', '-o', binary])
                for scenario in range(5):
                    item = run([binary, scenario])
                    assert item['stdout'] == 'clock passed\n' and not item['stderr'], item
        report['result'] = 'passed'
    except Exception as error:
        report['result'] = 'failed'
        report['error'] = repr(error)
        raise
    finally:
        (output/'receipt.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'result': report['result'], 'file_matrices': report['cases'],
                      'commands': len(report['commands']), 'receipt': str(output/'receipt.json')}))


if __name__ == '__main__':
    main()
