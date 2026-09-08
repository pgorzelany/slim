"""Public native corruption, capacity, reset and terminal-fault integration.

Uses a verification-only state/exec observer. Record capacity is the production
256-entry limit. The byte-capacity case lowers a valid table limit to its current
charge; it tests public fallback/recovery, not the physical 64MiB boundary.
"""
import argparse
from contextlib import contextmanager
import importlib.util
from pathlib import Path
import re
import struct
import tempfile

spec = importlib.util.spec_from_file_location('native_oracle', Path(__file__).with_name('verify-native-host.py'))
native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(native)
LIMIT = 67108864


@contextmanager
def session(host, name):
    with tempfile.TemporaryDirectory(prefix='slim-native-state-') as temporary:
        root = Path(temporary)
        trace, state = root/'executed.tsv', root/'state.tsv'
        client = native.Client([host], {'SLIM_NATIVE_TEST_CASE': name,
            'SLIM_NATIVE_EXEC_REPORT': str(trace), 'SLIM_NATIVE_STATE_REPORT': str(state)})
        client.trace = trace
        client.state_report = state
        try:
            yield client, root
        finally:
            if client.process.poll() is None:
                client.process.stdin.close()
                try:
                    client.process.wait(timeout=15)
                except native.subprocess.TimeoutExpired:
                    client.process.kill(); client.process.wait()


def update(client, root, value):
    source = f'module main\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64({value})\n  0\n'
    project = root/'source/slim.project'
    if project.exists():
        project.with_name('program.slim').write_text(source)
    else:
        project = native.module.project(root/'source', source)
    row = client.update(project)
    assert row['status'] == 0, row
    return row['published']


def state(client):
    fields = ['request', 'epoch', 'records', 'used', 'bytes_limit', 'disabled', 'records_limit']
    values = list(map(int, client.state_report.read_text().splitlines()[-1].split('\t')))
    assert len(values) == len(fields), values
    result = dict(zip(fields, values))
    assert result['records_limit'] == 256 and 0 <= result['records'] <= 256, result
    assert 0 <= result['used'] <= LIMIT and 1 <= result['bytes_limit'] <= LIMIT, result
    return result


def execute(root, artifact, value):
    target = root/'program'
    native.publish(target, artifact)
    native.execute(target, (0, str(value).encode(), b''))


def teardown(client):
    rows = client.trace_rows()
    captures = [Path(row.split('\t')[2]) for row in rows if row.startswith('/bin/sh\t')]
    assert len(captures) == 1 and not captures[0].exists(), captures
    assert sum(row.startswith('/bin/rm\t') for row in rows) == 1, rows


def reset(client, root, context, expected):
    client.reset()
    revision = update(client, root, 10)
    row, artifact = client.native(revision)
    assert row['status'] == 0 and row['starts'] == (1, 1, 1) and artifact == expected, row
    assert row['context'] == context and row['times'][0] == 0 and not row['reason'], row
    observed = state(client)
    assert observed['records'] == 3 and observed['bytes_limit'] == LIMIT and observed['disabled'] == 0, observed
    row, again = client.native(revision)
    assert row['status'] == 0 and row['starts'] == (0, 0, 0) and again == expected, row
    execute(root, artifact, 10)


def corruption(host, name):
    with session(host, name) as (client, root):
        revision = update(client, root, 10)
        cold, expected = client.native(revision)
        assert cold['status'] == 0 and state(client)['records'] == 3, cold
        row, artifact = client.native(revision)
        role = 0 if name == 'metadata' else int(name[-1])
        reason = 'corrupt-history' if name == 'metadata' else 'corrupt-entry'
        wanted = ';'.join(f'cache-{label}:{reason if index == role else "disabled-history"}'
                          for index, label in enumerate(['program', 'runtime', 'link']) if index >= role)
        assert row['status'] == 0 and row['starts'] == tuple(int(i >= role) for i in range(3)), row
        assert row['reason'] == wanted and artifact == expected, row
        assert state(client)['disabled'] == 1 and state(client)['used'] == LIMIT
        execute(root, artifact, 10)
        row, artifact = client.native(revision)
        assert row['status'] == 0 and row['starts'] == (1, 1, 1) and artifact == expected, row
        assert row['reason'] == ';'.join(f'cache-{label}:disabled-history' for label in ['program','runtime','link']), row
        reset(client, root, cold['context'], expected)
        client.quit(); teardown(client)
    print(f'native-state\t{name}\texact miss reason; complete fresh artifact; disabled history; reset restores reuse', flush=True)


def byte_capacity(host):
    with session(host, 'byte-capacity') as (client, root):
        revision = update(client, root, 10)
        cold, expected = client.native(revision)
        used = state(client)['used']
        revision = update(client, root, 11)
        for _ in range(2):
            row, artifact = client.native(revision)
            assert row['status'] == 0 and row['starts'] == (1, 0, 1), row
            assert row['reason'] == 'cache-program:capacity;cache-link:capacity', row
            observed = state(client)
            assert observed['used'] == observed['bytes_limit'] == used and observed['records'] == 3 and observed['disabled'] == 0, observed
            execute(root, artifact, 11)
        revision = update(client, root, 10)
        row, artifact = client.native(revision)
        assert row['status'] == 0 and row['starts'] == (0, 0, 0) and artifact == expected, row
        reset(client, root, cold['context'], expected)
        client.quit(); teardown(client)
    print('native-state\tbyte-capacity\tvalid lowered limit; no retained growth; old entries reusable; reset restores 64MiB limit', flush=True)


def record_capacity(host):
    with session(host, 'records') as (client, root):
        # Two supported profiles reach native capacity within the independent
        # production 64-source-attempt limit. Neither limit is changed.
        template = Path('benchmarks/challenges/state_machine/program.slim').read_text()
        manifest = native.module.project(root/'source', template)
        expected = {}
        for value in range(64):
            manifest.with_name('program.slim').write_text(template.replace('io.print_i64(answer)', f'io.print_i64(answer + {value})'))
            accepted = client.update(manifest)
            assert accepted['status'] == 0 and accepted['code'].startswith(b'#define SLIM_PARALLEL 1\n'), accepted
            revision = accepted['published']
            for workers in [0, 1]:
                row, artifact = client.native(revision, workers)
                assert row['status'] == 0 and row['profile'] == workers + 1, row
                assert row['starts'] == ((1, 1, 1) if value == 0 else (1, 0, 1)), row
                observed = state(client)
                count = (3 if workers == 0 else 6) if value == 0 else 6 + 4*(value - 1) + 2*(workers + 1)
                assert observed['records'] == min(count, 256), observed
                assert observed['bytes_limit'] == LIMIT and observed['used'] < LIMIT and observed['disabled'] == 0, observed
                reason = 'cache-program:capacity;cache-link:capacity' if (value, workers) == (63, 1) else ''
                assert row['reason'] == reason, (value, workers, row)
                target = root/'program'
                native.publish(target, artifact)
                native.execute(target, (0, f'{value}\n'.encode(), b''))
                if value == 63:
                    expected[workers] = artifact
        exhausted = client.update(manifest)
        assert exhausted['status'] == 65 and exhausted['reason'] == b'S0003' and exhausted['published'] == revision, exhausted
        for workers, work in [(0, (0, 0, 0)), (1, (1, 0, 1))]:
            row, artifact = client.native(revision, workers)
            assert row['status'] == 0 and row['starts'] == work and state(client)['records'] == 256, row
            assert artifact == expected[workers], workers
        context = row['context']
        client.reset()
        accepted = client.update(manifest)
        assert accepted['status'] == 0, accepted
        for workers in [0, 1]:
            row, artifact = client.native(accepted['published'], workers)
            assert row['status'] == 0 and row['starts'] == (1, 1, 1) and artifact == expected[workers], row
            assert row['context'] == context and row['times'][0] == 0 and not row['reason'], row
            assert state(client)['records'] == 3*(workers + 1), state(client)
            row, artifact = client.native(accepted['published'], workers)
            assert row['starts'] == (0, 0, 0) and artifact == expected[workers], row
        client.quit(); teardown(client)
    print('native-state\trecord-capacity\t256 actual native records in 64 source updates; next records declined; last-good profiles and reset exact', flush=True)


def terminal(host, name):
    with session(host, name) as (client, root):
        revision = update(client, root, 10)
        if name == 'cleanup':
            cold, _ = client.native(revision)
            assert cold['status'] == 0, cold
            revision = update(client, root, 11)
            status, code, work = 65, b'H0007', (1, 0, 0)
        else:
            role = int(name[-1])
            status, code, work = 71, b'H0006', tuple(int(i <= role) for i in range(3))
        previous = len(client.trace_rows())
        response = client.request(b'B', struct.pack('>QQB', *revision, 0))
        assert response == (b'E', code), (name, response)
        error = client.finish(status, stderr=None)
        if name == 'cleanup':
            assert error == b'', error
        else:
            assert re.fullmatch(rb'SLIM allocation failure: exhausted at allocation [1-9][0-9]*\n', error), error
        client.check_trace(previous, work)
        teardown(client)
    print(f'native-state\t{name}\tterminal {status}/{code.decode()}; no artifact or trailing frame; actual work and cleanup exact', flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('observed_host')
    names = ['metadata', 'artifact-0', 'artifact-1', 'artifact-2', 'byte-capacity',
             'record-capacity', 'cleanup', 'allocation-0', 'allocation-1', 'allocation-2']
    parser.add_argument('--case', action='append', choices=names)
    args = parser.parse_args()
    selected = args.case or names
    for name in selected:
        if name == 'byte-capacity':
            byte_capacity(args.observed_host)
        elif name == 'record-capacity':
            record_capacity(args.observed_host)
        elif name in ['metadata', 'artifact-0', 'artifact-1', 'artifact-2']:
            corruption(args.observed_host, name)
        else:
            terminal(args.observed_host, name)
    print(f'native-state\t{len(selected)} public state/capacity/terminal campaigns exact', flush=True)
