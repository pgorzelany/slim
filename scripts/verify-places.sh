#!/bin/sh
set -eu
place_compiler=${1:-build/toolchain/slimc}
place_scope=${2:-full}
case "$place_scope" in quick|full) ;; *) exit 64 ;; esac
place_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-places.XXXXXX")
trap 'rm -rf "$place_dir"' EXIT HUP INT TERM
mkdir "$place_dir/probe"
cp selfhost/*.slim "$place_dir/probe/"
cp tests/fixtures/retained_places.slim "$place_dir/probe/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' selfhost/slim.project > "$place_dir/probe/slim.project"
cat >> "$place_dir/probe/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports check flow identity query retained syntax typing) (exports)))
MANIFEST
cat >> "$place_dir/probe/zzprobe.slim" <<'WRONG'

fn wrong_place_type(cache: retained.Cache, node: identity.NodeId) -> retained.PlaceResult effects[partial]:
  retained.place_fact(cache, node, 1)
WRONG
if "$place_compiler" check "$place_dir/probe/slim.project" > "$place_dir/nominal-diagnostic"; then
  echo "place verification: raw node unexpectedly accepted as PlaceId" >&2
  exit 1
fi
if ! grep -q '^E03[0-9][0-9]@zzprobe@' "$place_dir/nominal-diagnostic"; then
  cat "$place_dir/nominal-diagnostic" >&2
  exit 1
fi
cp tests/fixtures/retained_places.slim "$place_dir/probe/zzprobe.slim"
"$place_compiler" "$place_dir/probe/slim.project" > "$place_dir/probe.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$place_dir/probe.c" runtime/slim_rt.c -o "$place_dir/ordinary"
awk -f scripts/instrument-place-probe.awk "$place_dir/probe.c" > "$place_dir/observed.c"
if test "$place_scope" = full; then
  clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror \
    -I runtime -include benchmarks/instrumentation/place_probe.h "$place_dir/observed.c" \
    runtime/slim_rt.c benchmarks/instrumentation/place_probe.c -o "$place_dir/observed"
else
  clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime -include benchmarks/instrumentation/place_probe.h \
    "$place_dir/observed.c" runtime/slim_rt.c benchmarks/instrumentation/place_probe.c -o "$place_dir/observed"
fi
python3 scripts/verify-places.py "$place_dir" "$place_scope"
