#!/bin/sh
set -eu
analysis_compiler=${1:-build/toolchain/slimc}
analysis_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-session-analysis.XXXXXX")
trap 'rm -rf "$analysis_dir"' EXIT HUP INT TERM
mkdir "$analysis_dir/probe"
cp selfhost/*.slim "$analysis_dir/probe/"
cp tests/fixtures/session_analysis.slim "$analysis_dir/probe/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' selfhost/slim.project > "$analysis_dir/probe/slim.project"
cat >> "$analysis_dir/probe/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports parallel project quality ranges retained session) (exports)))
MANIFEST
if ! "$analysis_compiler" "$analysis_dir/probe/slim.project" > "$analysis_dir/probe.c"; then
  cat "$analysis_dir/probe.c" >&2
  exit 1
fi
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$analysis_dir/probe.c" runtime/slim_rt.c -o "$analysis_dir/ordinary"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror -I runtime "$analysis_dir/probe.c" runtime/slim_rt.c -o "$analysis_dir/sanitized"
python3 -B scripts/verify-session-analysis.py "$analysis_dir"
