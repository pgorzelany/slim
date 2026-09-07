"""Cross the real public input, canonical-node and generated-C epoch capacities."""
import argparse
import hashlib
from pathlib import Path
import resource
import runpy
import tempfile

api = runpy.run_path(str(Path(__file__).with_name('verify-session-host.py')))
Client, clean, compare, project = (api[name] for name in ['Client', 'clean', 'compare', 'project'])
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('kind', choices=['input', 'nodes', 'code'])
parser.add_argument('--host')
parser.add_argument('--compiler', default='build/toolchain/slimc')
options = parser.parse_args()
command = [options.host] if options.host else ['./slimc', 'session']
capacity = 67108864

with tempfile.TemporaryDirectory(prefix='slim-host-limit-') as temporary:
    directory = Path(temporary)
    if options.kind == 'input':
        source = 'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
        manifest = project(directory / 'input', source)
        # Exactly 2 MiB per captured project, including the manifest itself.
        size = 2097152
        padding = size - len(source.encode()) - len(manifest.read_bytes()) - 2
        source += '#' + '.' * padding + '\n'
        (manifest.parent / 'program.slim').write_text(source)
        assert len(manifest.read_bytes()) + len(source.encode()) == size
        expected = clean(options.compiler, manifest)
        client = Client(command)
        for at in range(capacity // size):
            result = client.update(manifest)
            compare(result, expected)
            assert result['attempt'] == (1, at + 2)
        previous = result['published']
        for _ in range(2):
            result = client.update(manifest)
            assert result['status'] == 65 and result['reason'] == b'S0004'
            assert result['published'] == previous and result['code'] == b''
        client.reset()
        result = client.update(manifest)
        compare(result, expected)
        assert result['published'] == (2, 2) and result['snapshot'] == 0
        client.quit()
        print('session-host-limit\tinput\texact\t32 admitted x 2097152 bytes; next two rejected; reset cold', flush=True)
    else:
        if options.kind == 'nodes':
            source = 'module app\n\n' + ''.join(f'fn value_{i}() -> I64:\n  {i}\n\n' for i in range(4000))
            source += 'fn main(args: Vec[Bytes]) -> I64:\n  0\n'
            changed = source.replace('fn main(args: Vec[Bytes]) -> I64:\n  0', 'fn main(args: Vec[Bytes]) -> I64:\n  1')
            reason = b'S0005'
        else:
            source = 'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  let value: Bytes = "' + 'a' * 1048576 + '"\n  0\n'
            changed = source[:-4] + '  1\n'
            reason = b'S0006'
        before = project(directory / 'before', source)
        after = project(directory / 'after', changed)
        paths = [before, after]
        expected = [clean(options.compiler, path) for path in paths]
        assert all(status == 0 for status, _ in expected), [status for status, _ in expected]
        assert expected[0][1] != expected[1][1]
        client = Client(command)
        generated = 0
        admitted = 0
        last_good = None
        last_path = None
        for at in range(64):
            choice = at % 2
            result = client.update(paths[choice])
            if result['status']:
                assert result['status'] == 65 and result['reason'] == reason, (at, result['status'], result['reason'])
                assert result['code'] == b'' and result['published'] == last_good
                if options.kind == 'code':
                    assert generated <= capacity < generated + len(expected[choice][1])
                rejected_at = at + 1
                break
            compare(result, expected[choice])
            assert result['code_reused'] == 0 and result['generated'] == 1
            generated += len(result['code'])
            assert generated <= capacity
            admitted += 1
            last_good = result['published']
            last_path = choice
        else:
            raise AssertionError('fixture did not cross the intended epoch capacity before the attempt limit')
        assert admitted > 0
        again = client.update(paths[choice])
        assert again['status'] == 65 and again['reason'] == reason and again['published'] == last_good
        assert again['code'] == b''
        # A retained complete artifact remains reusable after node/C capacity failure.
        recovered = client.update(paths[last_path])
        compare(recovered, expected[last_path])
        assert recovered['published'] == last_good and recovered['snapshot'] == recovered['code_reused'] == 1
        client.reset()
        reset = client.update(paths[choice])
        compare(reset, expected[choice])
        assert reset['published'] == (2, 2) and reset['snapshot'] == 0
        client.quit()
        print('session-host-limit', options.kind, 'exact', f'{admitted} admitted; request {rejected_at} rejected; repeated rejection, last-good reuse and reset', f'generated_bytes={generated}', f'source_sha256={hashlib.sha256(source.encode()).hexdigest()}', sep='\t', flush=True)
# Platform-labelled absolute process evidence, not a portable regression budget.
# ru_maxrss here includes both the clean oracle and the worker children.
usage = resource.getrusage(resource.RUSAGE_CHILDREN)
print('session-host-limit-resources', options.kind, f'children_maxrss_native_units={usage.ru_maxrss}',
      f'children_user_s={usage.ru_utime}', f'children_system_s={usage.ru_stime}', sep='\t', flush=True)
