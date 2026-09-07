"""Public session framing and clean/incremental differential oracle (RFC-0143).

Only drives production executables. This client does not interpret SLIM source.
Run from the repository or installed release root.
"""
import argparse
import os
from pathlib import Path
import re
import select
import struct
import subprocess
import tempfile
import time


class Client:
    def __init__(self, command, environment=None):
        env = os.environ.copy()
        for name in ['SLIM_ALLOC_FAIL_AT', 'SLIM_HOST_ALLOC_FAIL_AT']:
            env.pop(name, None)
        env.update(environment or {})
        self.process = subprocess.Popen(command, stdin=subprocess.PIPE,
                                        stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                        bufsize=0, env=env)
        self.epoch = 1
        tag, greeting = self.receive()
        assert tag == b'H', (tag, greeting)
        rows = greeting.decode('ascii').splitlines()
        assert rows[0] == 'slim-session\t1'
        self.identity = dict(row.split('\t') for row in rows[1:])
        assert set(self.identity) == {'compiler', 'runtime', 'target', 'options'}
        for key in ['compiler', 'runtime', 'options']:
            assert re.fullmatch('[0-9a-f]{64}', self.identity[key])
        assert re.fullmatch('[A-Za-z0-9_.-]{1,128}', self.identity['target'])

    def read(self, size):
        pieces = []
        deadline = time.monotonic() + 120
        while size:
            remaining = deadline - time.monotonic()
            assert remaining > 0 and select.select([self.process.stdout], [], [], remaining)[0], 'response timeout'
            data = os.read(self.process.stdout.fileno(), min(size, 65536))
            assert data, ('incomplete response', size, self.process.poll())
            pieces.append(data)
            size -= len(data)
        return b''.join(pieces)

    def receive(self):
        header = self.read(5)
        size, = struct.unpack('>I', header[1:])
        assert size <= 87 + 4096 + 1048576 + 67108864, size
        return header[:1], self.read(size)

    def write(self, data):
        while data:
            count = os.write(self.process.stdin.fileno(), data)
            assert count > 0
            data = data[count:]

    def request(self, tag, payload=b'', fragmented=False):
        frame = tag + struct.pack('>I', len(payload)) + payload
        if fragmented:
            for byte in frame:
                self.write(bytes([byte]))
        else:
            self.write(frame)
        return self.receive()

    def update(self, path, fragmented=False):
        tag, payload = self.request(b'U', os.fsencode(path), fragmented)
        assert tag == b'S', (tag, payload)
        assert len(payload) >= 87
        words = struct.unpack('>8q', payload[:64])
        reason_size, diagnostics_size, code_size = struct.unpack('>3I', payload[75:87])
        assert len(payload) == 87 + reason_size + diagnostics_size + code_size
        assert payload[64] in (0, 1) and payload[65] in (0, 1) and payload[74] in (0, 1)
        a, b = 87 + reason_size, 87 + reason_size + diagnostics_size
        return dict(attempt=words[:2], published=words[2:4], status=words[4],
                    checks=words[5], reused=words[6], imported=words[7],
                    capacity=payload[64], snapshot=payload[65],
                    generated=struct.unpack('>q', payload[66:74])[0], code_reused=payload[74],
                    reason=payload[87:a], diagnostics=payload[a:b], code=payload[b:])

    def reset(self):
        self.epoch += 1
        assert self.request(b'R') == (b'A', struct.pack('>Q', self.epoch))

    def finish(self, status=0, stderr=b''):
        if not self.process.stdin.closed:
            self.process.stdin.close()
        try:
            code = self.process.wait(timeout=10)
            assert self.process.stdout.read() == b'', 'unexpected trailing response'
            error = self.process.stderr.read()
            assert code == status, (code, error)
            if stderr is not None:
                assert error == stderr, error
            return error
        finally:
            if self.process.poll() is None:
                self.process.kill()
                self.process.wait()
            self.process.stdout.close()
            self.process.stderr.close()

    def quit(self):
        assert self.request(b'Q') == (b'Q', b'')
        self.finish()


def clean(compiler, path):
    result = subprocess.run([compiler, str(path)], capture_output=True, timeout=120)
    assert result.returncode in (0, 1, 65), (path, result.returncode, result.stderr)
    assert result.stderr == b'', (path, result.stderr)
    return result.returncode, result.stdout


def compare(result, expected):
    status, output = expected
    assert result['status'] == status, (result, status)
    if status == 0:
        assert result['code'] == output and result['diagnostics'] == b''
    else:
        assert result['code'] == b'' and result['diagnostics'] == output, (result, output)


def project(directory, source):
    directory.mkdir()
    (directory / 'program.slim').write_text(source)
    module = re.search(r'(?m)^module ([A-Za-z_][A-Za-z_0-9]*)$', source)
    assert module
    manifest = directory / 'slim.project'
    manifest.write_text(f'(project 1 (entry {module[1]}) (module {module[1]} "program.slim" (imports) (exports)))\n')
    return manifest


def run(command, compiler, full):
    basic = Path('conformance/projects/basic/slim.project')
    bad = Path('conformance/projects/type-error/slim.project')
    expected = clean(compiler, basic)
    client = Client(command)
    first = client.update(basic, fragmented=True)
    compare(first, expected)
    assert first['checks'] > 0 and first['generated'] == 1
    same = client.update(basic)
    compare(same, expected)
    assert same['snapshot'] == same['code_reused'] == 1
    assert same['checks'] == same['generated'] == 0
    assert same['published'] == first['published']
    rejected = client.update(bad)
    compare(rejected, clean(compiler, bad))
    assert rejected['published'] == first['published']
    recovered = client.update(basic)
    compare(recovered, expected)
    assert recovered['snapshot'] == recovered['code_reused'] == 1
    client.reset()
    reset = client.update(basic)
    compare(reset, expected)
    assert reset['attempt'] == reset['published'] == (2, 2)
    assert reset['checks'] > 0 and reset['snapshot'] == reset['code_reused'] == 0
    client.quit()
    print('session-host\trecovery-reset-fragmented\texact', flush=True)

    malformed = [
        (b'X\0\0\0\0', b'H0001'), (b'R\0\0\0\1', b'H0001'),
        (b'Q\0\0\0\1', b'H0001'), (b'U\0\0\0\0', b'H0002'),
        (b'U\0\0\x10\x01', b'H0002'), (b'U\xff\xff\xff\xff', b'H0002'),
        (b'U\0\0\0\1\0', b'H0002'), (b'U\0', b'H0001'),
        (b'U\0\0\0\2x', b'H0001'),
    ]
    for frame, error in malformed:
        client = Client(command)
        client.write(frame)
        client.process.stdin.close()
        assert client.receive() == (b'E', error)
        client.finish(65)
    client = Client(command)
    client.finish()  # EOF at a frame boundary.
    invalid = subprocess.run(command + ['old', 'arguments'], capture_output=True, timeout=10)
    assert invalid.returncode == 64 and invalid.stdout == b''
    legacy = subprocess.run([compiler, 'session', str(basic), str(basic)], capture_output=True, timeout=10)
    assert legacy.returncode == 64 and b'public slimc launcher' in legacy.stdout and legacy.stderr == b''
    client = Client(command)
    client.process.stdout.close()
    client.write(b'U' + struct.pack('>I', len(os.fsencode(basic))) + os.fsencode(basic))
    client.process.stdin.close()
    assert client.process.wait(timeout=10) == 65
    assert client.process.stderr.read() == b''
    client.process.stderr.close()
    print('session-host\tmalformed-eof-cli-broken-output\texact', flush=True)

    with tempfile.TemporaryDirectory(prefix='slim-host-') as temporary:
        directory = Path(temporary)
        source = 'module unusual\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
        strange = project(directory / 'space \n quotation " dollar $ and ` tick', source)
        client = Client(command)
        compare(client.update(strange, fragmented=True), clean(compiler, strange))
        client.quit()
        if full:
            paths = sorted(Path('conformance/pass').glob('*.slim'))
            paths += sorted(Path('benchmarks/challenges').glob('*/program.slim'))
            paths.append(Path('tests/fixtures/retained_inputs_calls.slim'))
            client = Client(command)
            for index, path in enumerate(paths):
                text = path.read_text()
                before = project(directory / f'before-{index}', text)
                after = project(directory / f'after-{index}', text + '\nfn host_added(value: I64) -> I64:\n  value\n')
                for candidate in [before, before, after]:
                    compare(client.update(candidate), clean(compiler, candidate))
                client.reset()
            client.quit()
            manifests = sorted(set(line.split('\t')[2] for line in
                                   Path('conformance/projects/manifest.tsv').read_text().splitlines()
                                   if line and not line.startswith('#')))
            client = Client(command)
            for path in manifests:
                before = client.update(basic)
                actual = client.update(path)
                compare(actual, clean(compiler, path))
                if actual['status']:
                    assert actual['published'] == before['published']
                compare(client.update(basic), expected)
                client.reset()
            client.quit()
            print(f'session-host\tcorpus\t{len(paths)} sources x cold/unchanged/insertion; {len(manifests)} project paths and recovery', flush=True)

    client = Client(command)
    for index in range(64):
        current = client.update(basic)
        compare(current, expected)
        assert current['attempt'] == (1, index + 2)
    for _ in range(2):
        limit = client.update(basic)
        assert limit['status'] == 65 and limit['reason'] == b'S0003' and limit['code'] == b''
        assert limit['published'] == current['published']
    with tempfile.TemporaryDirectory(prefix='slim-host-saturated-') as temporary:
        blocked = Path(temporary) / 'would-block.project'
        os.mkfifo(blocked)
        saturated = client.update(blocked)
        assert saturated['status'] == 65 and saturated['reason'] == b'S0003'
        assert saturated['published'] == current['published'] and saturated['code'] == b''
    client.reset()
    compare(client.update(basic), expected)
    client.quit()
    print('session-host\tattempt-limit-no-file-read-and-reset\texact', flush=True)

    client = Client(command, {'SLIM_HOST_ALLOC_FAIL_AT': '1'})
    assert client.request(b'U', os.fsencode(bad)) == (b'E', b'H0006')
    client.finish(71)
    client = Client(command, {'SLIM_HOST_ALLOC_FAIL_AT': '2'})
    compare(client.update(bad), clean(compiler, bad))
    client.quit()
    failures = successes = 0
    # A tiny project crosses the entire cold/update allocation domain cheaply.
    with tempfile.TemporaryDirectory(prefix='slim-host-fault-') as temporary:
        tiny = project(Path(temporary) / 'tiny', 'module tiny\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
        tiny_expected = clean(compiler, tiny)
        for ordinal in range(1, 1025 if full else 17):
            client = Client(command, {'SLIM_ALLOC_FAIL_AT': str(ordinal)})
            try:
                client.write(b'U' + struct.pack('>I', len(os.fsencode(tiny))) + os.fsencode(tiny))
            except BrokenPipeError:
                pass  # The initial session allocation can fail before input.
            tag, payload = client.receive()
            if tag == b'E':
                assert payload == b'H0006'
                error = client.finish(71, stderr=None)
                assert error == f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode()
                failures += 1
            else:
                assert tag == b'S' and struct.unpack('>q', payload[32:40])[0] == 0
                assert payload.endswith(tiny_expected[1])
                client.quit()
                successes += 1
        assert failures > 0
        if full:
            assert successes > 0, 'fault domain did not cross all cold allocations'
    print(f'session-host\tallocation-faults\t{failures} failed; {successes} completed', flush=True)


def observe_work(command, compiler, report):
    basic = Path('conformance/projects/basic/slim.project')
    updated = Path('conformance/projects/session-updated/slim.project')
    client = Client(command, {'SLIM_SESSION_REPORT': report})
    for path in [basic, basic, updated, updated]:
        compare(client.update(path), clean(compiler, path))
    client.reset()
    compare(client.update(updated), clean(compiler, updated))
    client.quit()
    rows = Path(report).read_text().splitlines()
    assert rows[0] == 'slim-session\t7\texact\t1000000000\t2', rows[0]
    assert len(rows) == 6, rows
    counts = [list(map(int, row.split('\t'))) for row in rows[1:]]
    for index, values in enumerate(counts):
        assert len(values) == 24 and values[0] == index and all(0 <= v < 1000000000 for v in values)
        if index in (1, 3):
            assert values[1:] == [0] * 23, ('unchanged update ran a producer', values)
        if index in (0, 4):
            assert all(value > 0 for value in values[1:4]), ('cold update not observed', values)
        print('session-host-work', *values, sep='\t', flush=True)
    assert counts[2][2] < counts[0][2], 'body edit did not retain independent function checking'
    print('session-host\tobserved-zero-unchanged-work-and-two-physical-epoch-cleanups\texact', flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', help='direct adapter executable; default uses ./slimc session')
    parser.add_argument('--compiler', default='build/toolchain/slimc')
    parser.add_argument('--quick', action='store_true')
    parser.add_argument('--work-report', help='run only the instrumented query/cleanup oracle')
    options = parser.parse_args()
    command = [options.host] if options.host else ['./slimc', 'session']
    if options.work_report:
        observe_work(command, options.compiler, options.work_report)
    else:
        run(command, options.compiler, not options.quick)
