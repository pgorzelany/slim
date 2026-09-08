"""Public native I/O, compiler, linker and cleanup failures with exact recovery.

Requires the exec-observer host. Faults alter only disposable request paths;
captured compiler, SDK and runtime inputs remain unchanged.
"""
import argparse
import importlib.util
from pathlib import Path
import tempfile

spec = importlib.util.spec_from_file_location('native_oracle', Path(__file__).with_name('verify-native-host.py'))
native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(native)


def verify(host):
    with tempfile.TemporaryDirectory(prefix='slim-native-recovery-') as temporary:
        root = Path(temporary)
        source = native.module.project(root/'source',
            'module main\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64(10)\n  0\n')
        trace = root/'executed.tsv'
        client = native.Client([host], {'SLIM_NATIVE_EXEC_REPORT': str(trace)})
        client.trace = trace
        try:
            accepted = client.update(source)
            assert accepted['status'] == 0, accepted
            cold, expected = client.native(accepted['published'])
            assert cold['status'] == 0 and cold['starts'] == (1, 1, 1), cold
            captures = [Path(row.split('\t')[2]) for row in client.trace_rows() if row.startswith('/bin/sh\t')]
            assert len(captures) == 1, captures
            context = captures[0]
            outside = root/'untouched'
            outside.write_bytes(b'unchanged')
            cases = [
                ('source-write', 'program.c', True, (0, 0, 0), (1, 1, 1)),
                ('program-compiler', 'program.o', False, (1, 0, 0), (1, 1, 1)),
                ('runtime-compiler', 'runtime.o', False, (1, 1, 0), (0, 1, 1)),
                ('linker', 'program', False, (1, 1, 1), (0, 0, 1)),
            ]
            for name, filename, symlink, failed_work, recovered_work in cases:
                client.reset()
                accepted = client.update(source)
                assert accepted['status'] == 0, accepted
                fault = context/filename
                assert not fault.exists() and not fault.is_symlink(), fault
                if symlink:
                    fault.symlink_to(outside)
                else:
                    fault.mkdir()
                failed, artifact = client.native(accepted['published'])
                assert failed['status'] == 3 and not artifact and failed['starts'] == failed_work, (name, failed)
                assert failed['context'] == cold['context'] and failed['times'][0] == 0, failed
                assert failed['reason'] == ('native-build-failed' if symlink else 'native-work-cleanup-failed'), failed
                if symlink:
                    assert not fault.is_symlink(), 'request symlink survived cleanup'
                else:
                    assert failed['diagnostics'], (name, 'missing backend diagnostic')
                    fault.rmdir()
                assert outside.read_bytes() == b'unchanged', 'source write followed symlink'
                recovered, artifact = client.native(accepted['published'])
                assert recovered['status'] == 0 and artifact == expected and recovered['starts'] == recovered_work, (name, recovered)
                assert recovered['hits'] == tuple(int(value == 0) for value in recovered_work), recovered
                assert not recovered['reason'] and not recovered['diagnostics'] and recovered['times'][0] == 0, recovered
                target = root/'program'
                native.publish(target, artifact)
                native.execute(target, (0, b'10', b''))
                warm, artifact = client.native(accepted['published'])
                assert warm['status'] == 0 and artifact == expected and warm['starts'] == (0, 0, 0), warm
                print(f'native-recovery\t{name}\tno failed artifact; prior valid objects reused; recovered bytes execute', flush=True)

            # Cleanup is part of success even when no producer runs.
            fault = context/'link.bin'
            fault.mkdir()
            failed, artifact = client.native(accepted['published'])
            assert failed['status'] == 3 and failed['reason'] == 'native-work-cleanup-failed', failed
            assert not artifact and failed['starts'] == (0, 0, 0) and failed['hits'] == (1, 1, 1), failed
            fault.rmdir()
            recovered, artifact = client.native(accepted['published'])
            assert recovered['status'] == 0 and artifact == expected and recovered['starts'] == (0, 0, 0), recovered
            client.quit()
            assert not context.exists(), 'context survived quit'
            rows = client.trace_rows()
            assert sum(row.startswith('/bin/sh\t') for row in rows) == 1, rows
            assert sum(row.startswith('/bin/rm\t') for row in rows) == 1, rows
            print('native-recovery\tfive public failure/recovery cases exact; one capture; physical teardown observed', flush=True)
        finally:
            if client.process.poll() is None:
                client.process.stdin.close()
                try:
                    client.process.wait(timeout=15)
                except native.subprocess.TimeoutExpired:
                    client.process.kill()
                    client.process.wait()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('observed_host')
    args = parser.parse_args()
    verify(args.observed_host)
