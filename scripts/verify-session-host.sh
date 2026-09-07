#!/bin/sh
set -eu
host_binary=${1:-build/toolchain/slim-session}
host_compiler=${2:-build/toolchain/slimc}
host_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-host-verify.XXXXXX")
trap 'rm -rf "$host_dir"' EXIT HUP INT TERM
# Identity is deliberately copied from the compared ordinary host: this binary
# is a test observer of that build, never a distributable compiler artifact.
cp "$(dirname -- "$host_binary")/session-identity.h" "$host_dir/session-identity.h"
awk -v host=1 -f scripts/instrument-session-probe.awk bootstrap/slimc-seed.c > "$host_dir/slimc-seed.c"
python3 - "$host_dir/session.c" <<'PY'
from pathlib import Path
import sys
source = Path('compiler/session.c').read_text()
for anchor, insert, count in [
    ('int main(int argc, char **argv) {', '\n    slim_session_probe_init();', 1),
    ('slim_rt_init(&root, &allocation);', '\n    slim_session_probe_host_begin(&root);', 2),
    ('slim_rt_shutdown();', '\n    slim_session_probe_host_end(&root);', 2),
]:
    assert source.count(anchor) == count, anchor
    source = source.replace(anchor, anchor + insert)
Path(sys.argv[1]).write_text(source)
PY
host_cc=${CC:-cc}
"$host_cc" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Wall -Wextra -Werror -I runtime \
    -Dslim_print_bytes=slim_private_print_bytes -Dslim_print_i64=slim_private_print_i64 \
    -Dslim_println=slim_private_println -c runtime/slim_rt.c -o "$host_dir/runtime.o"
"$host_cc" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Wall -Wextra -Werror -I runtime -I "$host_dir" \
    -include benchmarks/instrumentation/session_probe.h "$host_dir/session.c" \
    "$host_dir/runtime.o" benchmarks/instrumentation/session_probe.c -o "$host_dir/observed"
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
    -include benchmarks/instrumentation/session_probe.h "$host_dir/capture.c" \
    "$host_dir/runtime.o" benchmarks/instrumentation/session_probe.c -o "$host_dir/capture"
"$host_dir/capture"
echo 'session-host: exact 1 MiB capture boundary, all 13 host allocation failures and byte-order checks passed'
python3 -B scripts/verify-session-host.py --host "$host_binary" --compiler "$host_compiler"
python3 -B scripts/verify-session-host.py --host "$host_dir/observed" --compiler "$host_compiler"
python3 -B scripts/verify-session-host.py --host "$host_dir/observed" --compiler "$host_compiler" \
    --work-report "$host_dir/work.tsv"
