#!/bin/sh
set -eu
parse_compiler=${1:-build/toolchain/slimc}
parse_scope=${2:-full}
case "$parse_scope" in quick|full) ;; *) exit 64 ;; esac
parse_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-parsing.XXXXXX")
trap 'rm -rf "$parse_dir"' EXIT HUP INT TERM
mkdir "$parse_dir/probe"
cp selfhost/*.slim "$parse_dir/probe/"
cp tests/fixtures/retained_parsing.slim "$parse_dir/probe/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' selfhost/slim.project > "$parse_dir/probe/slim.project"
cat >> "$parse_dir/probe/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports identity retained syntax) (exports)))
MANIFEST
"$parse_compiler" "$parse_dir/probe/slim.project" > "$parse_dir/probe.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$parse_dir/probe.c" runtime/slim_rt.c -o "$parse_dir/ordinary"
awk -f scripts/instrument-parse-probe.awk "$parse_dir/probe.c" > "$parse_dir/observed.c"
if test "$parse_scope" = full; then
  clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror \
    -I runtime -include benchmarks/instrumentation/parse_probe.h "$parse_dir/observed.c" \
    runtime/slim_rt.c benchmarks/instrumentation/parse_probe.c -o "$parse_dir/observed"
else
  clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime -include benchmarks/instrumentation/parse_probe.h \
    "$parse_dir/observed.c" runtime/slim_rt.c benchmarks/instrumentation/parse_probe.c -o "$parse_dir/observed"
fi
python3 scripts/verify-parsing.py "$parse_dir" "$parse_scope"
