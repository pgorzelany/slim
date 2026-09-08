"""Cold native host allocation ordinals, terminal frames and physical cleanup."""
import argparse
import importlib.util
from pathlib import Path
import struct
import tempfile

spec = importlib.util.spec_from_file_location('native_oracle', Path(__file__).with_name('verify-native-host.py'))
native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(native)


def verify(host, observe):
    with tempfile.TemporaryDirectory(prefix='slim-native-faults-') as temporary:
        root = Path(temporary)
        source = native.module.project(root / 'source',
            'module main\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64(10)\n  0\n')
        # Context digest, context manifest, program object, runtime object,
        # link report, executable. Ordinal 7 is an unreached cold-path control.
        work = [(0, 0, 0), (0, 0, 0), (1, 0, 0), (1, 1, 0), (1, 1, 1), (1, 1, 1)]
        for ordinal in range(1, 8):
            trace = root / f'executed-{ordinal}.tsv' if observe else None
            environment = {'SLIM_NATIVE_ALLOC_FAIL_AT': str(ordinal)}
            if trace:
                environment['SLIM_NATIVE_EXEC_REPORT'] = str(trace)
            client = native.Client([host], environment)
            client.trace = trace
            try:
                accepted = client.update(source)
                assert accepted['status'] == 0, accepted
                previous = len(client.trace_rows())
                if ordinal < 7:
                    response = client.request(b'B', struct.pack('>QQB', *accepted['published'], 0))
                    assert response == (b'E', b'H0006'), (ordinal, response)
                    client.finish(71)
                    client.check_trace(previous, work[ordinal-1])
                else:
                    row, artifact = client.native(accepted['published'])
                    assert row['status'] == 0 and row['starts'] == (1, 1, 1), row
                    repeated, again = client.native(accepted['published'])
                    assert again == artifact and repeated['starts'] == (0, 0, 0), repeated
                    target = root / 'program'
                    native.publish(target, artifact)
                    native.execute(target, (0, b'10', b''))
                    client.quit()
                if trace:
                    rows = client.trace_rows()
                    assert sum(row.startswith('/bin/sh\t') for row in rows) == 1, (ordinal, rows)
                    assert sum(row.startswith('/bin/rm\t') for row in rows) == 1, (ordinal, rows)
                    for row in rows:
                        if row.startswith('/bin/sh\t'):
                            assert not Path(row.split('\t')[2]).exists(), (ordinal, 'context survived failure')
                print(f'native-host-fault\t{ordinal}\t' + ('terminal 71; no artifact' if ordinal < 7 else 'complete cold/warm artifacts execute'), flush=True)
            finally:
                if client.process.poll() is None:
                    client.process.stdin.close()
                    try:
                        client.process.wait(timeout=15)
                    except native.subprocess.TimeoutExpired:
                        client.process.kill()
                        client.process.wait()
    print('native-host-faults\t6 cold-path native allocation failures and ordinal-7 control exact' +
          ('; actual work and context removal observed' if observe else ''), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('host')
    parser.add_argument('--observe', action='store_true')
    args = parser.parse_args()
    verify(args.host, args.observe)
