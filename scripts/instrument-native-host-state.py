"""Add state/failure observers to a verification copy of the native host."""
from pathlib import Path
import sys

path = Path(sys.argv[1])
code = path.read_text()
changes = [
    ('static Slim_type_nativecache_95State native_cache;',
     '\n' + Path('tests/fixtures/native_session_probe.h').read_text()),
    ('static int native_group_live(pid_t leader) {',
     '\n    if (native_probe_group_failure) { native_probe_group_failure = false; return -1; }'),
    ('static void native_publish(Slim_type_nativecache_95Key key, unsigned role, NativeWork *work, NativeBytes output) {',
     '\n    native_probe_publish(role);'),
]
for anchor, addition in changes:
    assert code.count(anchor) == 1, anchor
    code = code.replace(anchor, anchor + addition)
anchor = '            status = native_build(selected, epoch, &work, &executable);'
assert code.count(anchor) == 1
code = code.replace(anchor, '            native_probe_before();\n' + anchor)
anchor = '    int result = native_response(epoch, serial, status, selected.slim_field_workers,'
assert code.count(anchor) == 1
code = code.replace(anchor, '    native_probe_report();\n' + anchor)
path.write_text(code)
