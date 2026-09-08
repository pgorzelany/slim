"""Verify explicit native-context unavailability while ordinary compilation works."""
import importlib.util
from pathlib import Path
import subprocess
import sys
import tempfile

spec = importlib.util.spec_from_file_location('native_oracle', Path(__file__).with_name('verify-native-host.py'))
native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(native)
native.framing(sys.argv[1])
with tempfile.TemporaryDirectory(prefix='slim-native-unavailable-') as temporary:
    root = Path(temporary)
    source = native.module.project(root/'source',
        'module main\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64(17)\n  0\n')
    client = native.Client([sys.argv[1]])
    try:
        for epoch in range(2):
            accepted = client.update(source)
            assert accepted['status'] == 0 and accepted['code'], accepted
            for repeat in range(2):
                row, artifact = client.native(accepted['published'])
                assert row['status'] == 2 and row['reason'] == 'native-context-unavailable', row
                assert not artifact and not row['context'] and row['starts'] == (0,0,0), row
                if repeat: assert row['times'][0] == 0, row
            if epoch == 0: client.reset()
        client.quit()
    finally:
        if client.process.poll() is None:
            client.process.stdin.close();client.process.wait(timeout=20)
    result = subprocess.run(['./slimc','run',str(source)],capture_output=True,timeout=120)
    assert (result.returncode,result.stdout,result.stderr) == (0,b'17',b''), result
print('native-platform\texplicit unavailable context; no native artifact; ordinary production build/run exact',flush=True)
