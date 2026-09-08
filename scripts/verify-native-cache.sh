#!/bin/sh
set -eu
native_compiler=${1:-build/toolchain/slimc}
native_scope=${2:-full}
case "$native_scope" in full|quick|faults|costs) ;; *) exit 64 ;; esac
native_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-native-query.XXXXXX")
trap 'rm -rf "$native_dir"' EXIT HUP INT TERM
mkdir "$native_dir/probe"
cp selfhost/*.slim "$native_dir/probe/"
cp tests/fixtures/native_cache.slim "$native_dir/probe/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;s/(exports Entry Header Key Probe State Stored start lookup publish)/(exports key_fingerprint metadata_seal set_header Entry Header Key Probe State Stored start lookup publish)/;$s/)$//' selfhost/slim.project > "$native_dir/probe/slim.project"
cat >> "$native_dir/probe/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports nativecache retained text) (exports borrowed borrowed_probe key)))
MANIFEST
if ! "$native_compiler" "$native_dir/probe/slim.project" > "$native_dir/probe.c"; then
    cat "$native_dir/probe.c" >&2
    exit 1
fi
"${CC:-cc}" -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$native_dir/probe.c" runtime/slim_rt.c -o "$native_dir/ordinary"
"${CC:-cc}" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror -I runtime "$native_dir/probe.c" runtime/slim_rt.c -o "$native_dir/sanitized"
cat > "$native_dir/borrowed.c" <<'C'
#define main native_fixture_main
#include "probe.c"
#undef main
int main(void) {
    SlimAllocStatus allocation;
    SlimRegion region;
    slim_alloc_status_init(&allocation);
    slim_rt_init(&region, &allocation);
    uint8_t context[] = "context", source[] = "source", artifact[] = "object";
    Slim_type_nativecache_95State state = slim_fn_zzprobe_95borrowed(
        slim_bytes_static(context, 7), slim_bytes_static(source, 6),
        slim_bytes_static(artifact, 6), &region);
    memset(context, 'x', 7);
    memset(source, 'x', 6);
    memset(artifact, 'x', 6);
    bool exact = slim_fn_zzprobe_95borrowed_95probe(&state, &region);
    slim_rt_shutdown();
    return exact ? 0 : 1;
}
C
"${CC:-cc}" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror -I runtime "$native_dir/borrowed.c" runtime/slim_rt.c -o "$native_dir/borrowed"
python3 -B scripts/instrument-native-cache.py "$native_dir/probe.c" "$native_dir/observed.c"
"${CC:-cc}" -std=c11 -O3 -Wall -Wextra -Werror -I runtime -I "$native_dir" tests/fixtures/native_cache_host.c -o "$native_dir/cost"
"${CC:-cc}" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -DNATIVE_QUERY_OBSERVED -Wall -Wextra -Werror -I runtime -I "$native_dir" tests/fixtures/native_cache_host.c -o "$native_dir/cost-observed"
python3 -B scripts/verify-native-cache.py "$native_dir" "$native_scope" "$native_compiler"
