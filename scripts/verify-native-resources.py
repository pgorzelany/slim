"""Public native ownership, real byte capacity, context decline and isolation.

Allocated bytes include runtime allocation headers, not malloc bookkeeping.
RSS/child peak are Darwin byte observations, not portable gates. File block sums
are per-file accounting and do not measure unique storage of cloned extents.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import importlib.util
import json
from pathlib import Path
import tempfile

spec = importlib.util.spec_from_file_location('native_oracle', Path(__file__).with_name('verify-native-host.py'))
native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(native)
LIMIT = 67108864


def client(host, root, mode='resources'):
    root.mkdir()
    trace, resources = root/'exec.tsv', root/'resources.tsv'
    result = native.Client([host], {'SLIM_NATIVE_TEST_CASE': mode,
        'SLIM_NATIVE_EXEC_REPORT': str(trace), 'SLIM_NATIVE_RESOURCE_REPORT': str(resources),
        'SLIM_NATIVE_STATE_REPORT': str(root/'state.tsv')})
    result.trace, result.resources = trace, resources
    return result


def snapshot(connection, stage='after'):
    rows = [row.split('\t') for row in connection.resources.read_text().splitlines() if row.startswith(stage+'\t')]
    fields = ['records','charged','allocated','freed','cloned','rss','peak_rss','child_peak_rss','manifest','diagnostics']
    assert rows and len(rows[-1]) == len(fields)+1, rows[-1:]
    result = dict(zip(fields, map(int, rows[-1][1:])))
    assert all(value >= 0 for value in result.values()) and result['charged'] <= LIMIT, result
    assert result['allocated'] <= 2*result['charged'] + 524288, result
    print('native-resource-snapshot\t'+json.dumps(dict(stage=stage,**result),sort_keys=True),flush=True)
    return result


def update(connection, root, payload):
    source = 'module main\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.println("'+payload+'")\n  0\n'
    manifest = root/'source/slim.project'
    if not manifest.exists():
        manifest = native.module.project(root/'source', source)
    else:
        manifest.with_name('program.slim').write_text(source)
    row = connection.update(manifest)
    assert row['status'] == 0, {k:v for k,v in row.items() if k!='code'}
    return row['published']


def context(connection):
    paths = [Path(row.split('\t')[2]) for row in connection.trace_rows() if row.startswith('/bin/sh\t')]
    assert len(paths) == 1, paths
    return paths[0]


def files(path):
    result = {}
    for member in path.rglob('*'):
        assert not member.is_symlink(), member
        if member.is_file():
            info = member.stat()
            result[str(member.relative_to(path))] = [info.st_size, info.st_ino, info.st_mode & 0o777, info.st_blocks*512]
    count, size = map(int,(path/'capture-size.tsv').read_text().split())
    assert count <= 512 and size <= 536870912
    print('native-resource-files\t'+json.dumps(dict(captured_files=count,captured_bytes=size,
        all_files=len(result),logical_bytes=sum(row[0] for row in result.values()),
        reported_block_bytes=sum(row[3] for row in result.values())),sort_keys=True),flush=True)
    return result


def execute(root, artifact, payload):
    target = root/'program'
    native.publish(target, artifact)
    native.execute(target, (0, (payload+'\n').encode(), b''))


def reset(connection, previous):
    connection.reset()
    row = snapshot(connection, 'reset')
    assert row['allocated'] == row['charged'] == row['records'] == 0, row
    assert row['freed'] - previous['freed'] == previous['allocated'], (previous,row)
    assert row['cloned'] == previous['cloned'], (previous,row)


def close(connection):
    if connection.process.poll() is None:
        connection.process.stdin.close()
        try:
            connection.process.wait(timeout=15)
        except native.subprocess.TimeoutExpired:
            connection.process.kill(); connection.process.wait()


def history(host, root, boundary):
    connection = client(host, root)
    try:
        baseline = None
        counts = range(32) if boundary else [1024,16384,262144,1048576]
        for index, value in enumerate(counts):
            payload = 'x'*(2097152 if boundary else value) + str(index)
            revision = update(connection, root, payload)
            row, artifact = connection.native(revision)
            assert row['status'] == 0, row
            observed = snapshot(connection)
            if baseline is None:
                baseline = files(context(connection))
            execute(root, artifact, payload)
            warm, again = connection.native(revision)
            repeated = snapshot(connection)
            assert warm['status'] == 0 and again == artifact, warm
            assert repeated['allocated'] == observed['allocated'] and repeated['cloned'] == observed['cloned'], (observed,repeated)
            if 'capacity' in row['reason']:
                assert boundary and observed['records'] < 256 and observed['charged'] < LIMIT, observed
                assert 'capacity' in warm['reason'], warm
                state = list(map(int,(root/'state.tsv').read_text().splitlines()[-1].split('\t')))
                assert state[4] == LIMIT and state[5] == 0, state
                print('native-resource-boundary\t'+json.dumps(dict(request=index+1,charged=observed['charged'],
                    records=observed['records'],unretained=row['reason']),sort_keys=True),flush=True)
                break
            assert warm['starts'] == (0,0,0), warm
        else:
            assert not boundary, 'native byte limit was not crossed within source-attempt budget'
        reset(connection, repeated)
        assert files(context(connection)) == baseline, 'reset changed captured files'
        revision = update(connection, root, payload)
        restored, recovered = connection.native(revision)
        assert restored['status'] == 0 and restored['starts'] == (1,1,1) and restored['times'][0] == 0, restored
        assert not restored['reason'] and recovered == artifact, restored
        final = snapshot(connection)
        assert final['records'] == 3 and final['allocated'] <= observed['allocated'], final
        path = context(connection)
        connection.quit()
        assert not path.exists(), path
        freed = snapshot(connection, 'reset')
        assert freed['allocated'] == 0 and freed['freed'] - final['freed'] == final['allocated'], freed
    finally:
        close(connection)
    print('native-resources\t'+('real-byte-boundary' if boundary else 'geometric-history')+'\towned bytes freed exactly; captured files stable across reset; quit removes context',flush=True)


def decline(host, root):
    connection = client(host, root, 'context-decline')
    try:
        revision = update(connection, root, 'decline')
        for _ in range(2):
            row, artifact = connection.native(revision)
            assert row['status'] == 2 and row['reason'] == 'native-context-unavailable' and not artifact, row
            assert not connection.trace_rows(), 'declined context started tools or cleanup'
            assert snapshot(connection)['allocated'] == 0
        connection.reset()
        revision = update(connection, root, 'decline')
        row, artifact = connection.native(revision)
        assert row['status'] == 0 and row['starts'] == (1,1,1), row
        execute(root, artifact, 'decline')
        path = context(connection)
        connection.quit(); assert not path.exists()
    finally:
        close(connection)
    print('native-resources\tcontext-decline\tno tools or retained allocation; decline persists; reset retries provider and succeeds',flush=True)


def concurrent(host, root):
    root.mkdir()
    roots = [root/'first',root/'second']
    connections = [client(host, path) for path in roots]
    try:
        with ThreadPoolExecutor(max_workers=2) as pool:
            def build(index, payload):
                revision = update(connections[index],roots[index],payload)
                return revision, connections[index].native(revision)
            cold = list(pool.map(lambda index: build(index,'same'), range(2)))
            assert all(item[1][0]['starts'] == (1,1,1) and item[1][0]['status'] == 0 for item in cold), cold
            assert cold[0][1][1] == cold[1][1][1]
            paths = [context(c) for c in connections]
            assert paths[0] != paths[1]
            assert (paths[0]/'toolchain/bin/clang').stat().st_ino != (paths[1]/'toolchain/bin/clang').stat().st_ino
            changed = list(pool.map(lambda index: build(index,f'changed{index}'),range(2)))
            for index, (revision, (row,artifact)) in enumerate(changed):
                assert row['starts'] == (1,0,1) and row['status'] == 0, row
                execute(roots[index],artifact,f'changed{index}')
            before = snapshot(connections[0])
            futures = [pool.submit(reset,connections[0],before),pool.submit(connections[1].native,changed[1][0])]
            futures[0].result()
            row, artifact = futures[1].result()
            assert row['starts'] == (0,0,0) and artifact == changed[1][1][1], row
            connections[0].quit()
            assert not paths[0].exists() and paths[1].exists()
            row, artifact = connections[1].native(changed[1][0])
            assert row['starts'] == (0,0,0) and artifact == changed[1][1][1], row
            connections[1].quit(); assert not paths[1].exists()
    finally:
        for connection in connections: close(connection)
    print('native-resources\tconcurrent-sessions\tparallel captures/edits exact; independent inodes, caches, reset and teardown',flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('observed_host')
    parser.add_argument('--case', action='append', choices=['history','boundary','decline','concurrent'])
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='slim-native-resources-') as temporary:
        root = Path(temporary)
        for name in args.case or ['history','boundary','decline','concurrent']:
            if name in ['history','boundary']:
                history(args.observed_host,root/name,name=='boundary')
            elif name == 'decline':
                decline(args.observed_host,root/name)
            else:
                concurrent(args.observed_host,root/name)
