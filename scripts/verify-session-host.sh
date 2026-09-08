#!/bin/sh
set -eu
host_binary=${1:-build/toolchain/slim-session}
host_compiler=${2:-build/toolchain/slimc}
host_scope=${3:-full}
case "$host_scope" in full|protocol) ;; *) exit 64 ;; esac
host_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-host-verify.XXXXXX")
trap 'rm -rf "$host_dir"' EXIT HUP INT TERM
# Identity is deliberately copied from the compared ordinary host: this binary
# is a test observer of that build, never a distributable compiler artifact.
cp "$(dirname -- "$host_binary")/session-identity.h" "$host_dir/session-identity.h"
cp "$(dirname -- "$host_binary")/native.c" "$(dirname -- "$host_binary")/native-inputs.h" "$host_dir/"
awk -v host=1 -f scripts/instrument-session-probe.awk bootstrap/slimc-seed.c > "$host_dir/slimc-seed.c"
python3 - "$host_dir/session.c" <<'PY'
from pathlib import Path
import sys
source = Path('compiler/session.c').read_text()
for anchor, insert, count in [
    ('int main(int argc, char **argv) {', '\n    slim_session_probe_init();\n    host_resource_init();', 1),
    ('slim_rt_init(&root, &allocation);', '\n    slim_session_probe_host_begin(&root);', 2),
    ('slim_rt_shutdown();', '\n    slim_session_probe_host_end(&root);', 2),
]:
    assert source.count(anchor) == count, anchor
    source = source.replace(anchor, anchor + insert)
for anchor, replacement, count in [
    ('        void *next = realloc(host_diagnostics, capacity);', '        void *next = realloc(host_diagnostics, capacity);', 1),
    ('        if (host_allocations == UINT64_MAX', '        host_resource_host_attempt(capacity);\n        if (host_allocations == UINT64_MAX', 1),
    ('        host_diagnostics = next;', '        host_resource_resize(host_diagnostic_capacity, capacity);\n        host_diagnostics = next;', 1),
    ('    free(host_diagnostics);', '    host_resource_resize(host_diagnostic_capacity, 0);\n    free(host_diagnostics);', 1),
    ('        result = host_response(report, code);', '        result = host_response(report, code);\n        host_resource_snapshot(0, report.slim_field_attempted.slim_field_epoch, report.slim_field_attempted.slim_field_serial);', 1),
    ('            host_clear_capture();', '            host_clear_capture();\n            host_resource_snapshot(1, epoch, 0);', 1),
    ('    host_clear_capture();\n    return result;', '    host_clear_capture();\n    host_resource_snapshot(2, epoch, 0);\n    return result;', 1),
]:
    assert source.count(anchor) == count, anchor
    source = source.replace(anchor, replacement)
Path(sys.argv[1]).write_text(source)
runtime = Path('runtime/slim_rt.c').read_text()
for anchor, replacement, count in [
    ('    uint64_t attempt = atomic_fetch_add(&status->attempts, 1) + 1;\n', '    uint64_t attempt = atomic_fetch_add(&status->attempts, 1) + 1;\n    host_resource_attempt(size);\n', 1),
    ('    allocation->next = region->newest;\n', '    host_resource_add(size, offsetof(SlimAllocation, data));\n    allocation->next = region->newest;\n', 1),
    ('free(allocation);', 'host_resource_free(allocation->size, offsetof(SlimAllocation, data)); free(allocation);', 2),
    ('        memcpy(new_pointer, pointer, copied);', '        host_resource_copy(copied);\n        memcpy(new_pointer, pointer, copied);', 1),
]:
    assert runtime.count(anchor) == count, anchor
    runtime = runtime.replace(anchor, replacement)
Path(sys.argv[1]).with_name('runtime.c').write_text(runtime)
PY
host_cc=${CC:-cc}
"$host_cc" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Wall -Wextra -Werror -I runtime -include benchmarks/instrumentation/host_resource.h \
    -Dslim_print_bytes=slim_private_print_bytes -Dslim_print_i64=slim_private_print_i64 \
    -Dslim_println=slim_private_println -c "$host_dir/runtime.c" -o "$host_dir/runtime.o"
"$host_cc" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Wall -Wextra -Werror -I runtime -I "$host_dir" \
    -include benchmarks/instrumentation/session_probe.h -include benchmarks/instrumentation/host_resource.h "$host_dir/session.c" \
    "$host_dir/runtime.o" benchmarks/instrumentation/session_probe.c benchmarks/instrumentation/host_resource.c -o "$host_dir/observed"
python3 - "$host_dir" <<'PY_CAPTURE'
from pathlib import Path
import sys
root = Path(sys.argv[1])
source = (root / 'session.c').read_text()
assert source.count('int main(int argc, char **argv) {') == 1
source = source.replace('int main(int argc, char **argv) {', 'int slim_host_capture_unused_main(int argc, char **argv) {')
(root / 'capture.c').write_text(source + '\n' + Path('tests/fixtures/session_host_capture.c').read_text())
PY_CAPTURE
"$host_cc" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Wall -Wextra -Werror -I runtime -I "$host_dir" \
    -include benchmarks/instrumentation/session_probe.h -include benchmarks/instrumentation/host_resource.h "$host_dir/capture.c" \
    "$host_dir/runtime.o" benchmarks/instrumentation/session_probe.c benchmarks/instrumentation/host_resource.c -o "$host_dir/capture"
"$host_dir/capture"
echo 'session-host: exact 1 MiB capture boundary, all 13 host allocation failures and byte-order checks passed'
cat > "$host_dir/resource-bound.c" <<'EOF'
#include "host_resource.h"
#include <stdlib.h>
int main(int argc, char **argv) {
    if (argc != 2) return 64;
    host_resource_init();
    unsigned count = (unsigned)atoi(argv[1]);
    for (unsigned i = 0; i < count; ++i) host_resource_snapshot(0, 1, i);
    return 0;
}
EOF
"$host_cc" -std=c11 -O1 -g -fsanitize=address,undefined -Wall -Wextra -Werror \
    -I benchmarks/instrumentation "$host_dir/resource-bound.c" \
    benchmarks/instrumentation/host_resource.c -o "$host_dir/resource-bound"
python3 - "$host_dir" <<'PY_BOUND'
import os
from pathlib import Path
import subprocess
import sys
directory = Path(sys.argv[1])
for count in [255, 256, 257]:
    output = directory / f'resource-bound-{count}.tsv'
    subprocess.run([str(directory / 'resource-bound'), str(count)], check=True,
                   env={**os.environ, 'SLIM_HOST_RESOURCE_REPORT': str(output)})
    lines = output.read_text().splitlines()
    status = 'exact' if count <= 256 else 'bounded'
    assert lines[0] == f'slim-host-resources\t1\t{status}\t256'
    assert len(lines) == min(count, 256) + 2
print('session-host-resources-bound\texact\t255/256/257 snapshots preserve explicit saturation', flush=True)
PY_BOUND
if test "$host_scope" = protocol; then
    python3 -B scripts/verify-session-host.py --host "$host_binary" --compiler "$host_compiler" --partial-only
    python3 -B scripts/verify-session-host.py --host "$host_dir/observed" --compiler "$host_compiler" --partial-only
    exit 0
fi
python3 -B scripts/verify-session-host.py --host "$host_binary" --compiler "$host_compiler"
python3 -B scripts/verify-session-host.py --host "$host_dir/observed" --compiler "$host_compiler"
python3 -B scripts/verify-session-host.py --host "$host_dir/observed" --compiler "$host_compiler" \
    --work-report "$host_dir/work.tsv"
for host_limit in input nodes code; do
    python3 -B scripts/verify-session-host-limits.py "$host_limit" --host "$host_binary" --compiler "$host_compiler"
    python3 -B scripts/verify-session-host-limits.py "$host_limit" --host "$host_dir/observed" --compiler "$host_compiler"
done
python3 -B scripts/verify-session-host.py --host "$host_dir/observed" --compiler "$host_compiler" \
    --resource-report "$host_dir/resources.tsv"
