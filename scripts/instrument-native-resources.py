"""Observe actual native-region allocation/free and clone work in a host copy."""
from pathlib import Path
import re
import sys

root = Path(sys.argv[1])
code = (root/'slimc-seed.c').read_text()
pattern = r'(?m)^static [^\n]*\bslim_fn_nativecache_95clone_95rows\([^\n]*\) \{$'
matches = list(re.finditer(pattern, code))
assert len(matches) == 1
match = matches[0]
at = re.search(r'\bslim_v_at_n[0-9]+\b', match[0])[0]
source = re.search(r'\bslim_v_source_n[0-9]+\b', match[0])[0]
end = code.index('\nstatic ', match.end())
body = code[match.start():end]
assert body.count('slim_recur: ;') == 1
body = body.replace('slim_recur: ;', f'slim_recur: ;\nif ({at} < {source}.len) ++native_resource_cloned;')
(root/'slimc-seed.c').write_text('#include <stdint.h>\nstatic uint64_t native_resource_cloned;\n'+code[:match.start()]+body+code[end:])

runtime = Path('runtime/slim_rt.c').read_text()
anchor = '        free(allocation);'
assert runtime.count(anchor) == 1
runtime = runtime.replace(anchor, '        slim_test_observe_free(allocation);\n'+anchor)
prefix = '#include "slim_rt.h"\nstatic void slim_test_observe_free(SlimAllocation *allocation);\n'
suffix = r'''
#include <assert.h>
static SlimRegion *slim_test_tracked;
static uint64_t slim_test_freed;
void slim_test_track_region(SlimRegion *region) { slim_test_tracked = region; }
static void slim_test_observe_free(SlimAllocation *allocation) {
    if (allocation->region == slim_test_tracked)
        slim_test_freed += allocation->size + offsetof(SlimAllocation, data);
}
uint64_t slim_test_region_bytes(const SlimRegion *region) {
    uint64_t bytes = 0;
    for (const SlimAllocation *a = region->newest; a != NULL; a = a->next) {
        assert(a->region == region);
        bytes += a->size + offsetof(SlimAllocation, data);
    }
    return bytes;
}
uint64_t slim_test_freed_bytes(void) { return slim_test_freed; }
'''
(root/'resource-runtime.c').write_text(prefix+runtime+suffix)

path = root/'native.c'
code = path.read_text()
anchor = 'static Slim_type_nativecache_95State native_cache;'
assert code.count(anchor) == 1
code = code.replace(anchor, anchor+'\n'+Path('tests/fixtures/native_resource_probe.h').read_text())
for anchor, addition in [
    ('static bool native_system_identity(char *output, size_t size) {', '\n    if (native_resource_decline()) return false;'),
    ('    if (native_context_state == 2 && !*native_directory) native_context_state = 0;', '\n    native_resource_report("reset");'),
]:
    assert code.count(anchor) == 1
    code = code.replace(anchor, anchor+addition)
for anchor, addition in [
    ('    uint64_t begin = native_now();\n    int64_t epoch = native_request_word(payload)', '    native_resource_report("before");\n'),
    ('    int result = native_response(epoch, serial, status, selected.slim_field_workers,', '    native_resource_report("after");\n'),
]:
    assert code.count(anchor) == 1
    code = code.replace(anchor, addition+anchor)
path.write_text(code)
