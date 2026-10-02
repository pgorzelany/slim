#!/bin/sh
set -eu
input_compiler=${1:-build/toolchain/slimc}
input_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-inputs.XXXXXX")
trap 'rm -rf "$input_dir"' EXIT HUP INT TERM
mkdir "$input_dir/project"
cp selfhost/*.slim "$input_dir/project/"
cp tests/fixtures/retained_inputs.slim "$input_dir/project/zzprobe.slim"
python3 -B scripts/prepare-probe-manifest.py selfhost/slim.project "$input_dir/project/slim.project" \
  --export retained "input_admitted input_frames_valid input_previous_head input_query_chain input_seal_history empty_input_queries" \
  --export ranges "analyze_functions" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports check identity ranges retained syntax typing) (exports))
MANIFEST
if ! "$input_compiler" "$input_dir/project/slim.project" > "$input_dir/probe.c"; then
  cat "$input_dir/probe.c" >&2
  exit 1
fi
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$input_dir/probe.c" runtime/slim_rt.c -o "$input_dir/ordinary"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror -I runtime "$input_dir/probe.c" runtime/slim_rt.c -o "$input_dir/sanitized"
python3 -B scripts/instrument-input-probe.py "$input_dir/probe.c" > "$input_dir/observed.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$input_dir/observed.c" runtime/slim_rt.c -o "$input_dir/observed"
python3 -B scripts/verify-inputs.py "$input_dir"
