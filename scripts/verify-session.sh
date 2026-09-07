#!/bin/sh
set -eu
session_compiler=${1:-build/toolchain/slimc}
session_scope=${2:-full}
case "$session_scope" in quick|matrix|full) ;; *) exit 64 ;; esac
session_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-session.XXXXXX")
trap 'rm -rf "$session_dir"' EXIT HUP INT TERM
mkdir "$session_dir/probe"
cp selfhost/*.slim "$session_dir/probe/"
cp tests/fixtures/emission_damage.slim "$session_dir/probe/zdamage.slim"
cp tests/fixtures/transactional_session.slim "$session_dir/probe/zzprobe.slim"
# Expose budget and integrity helpers only inside this test project's module boundary.
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;s/(exports RangeBudget /(exports range_save range_heads range_find range_import Ranged RangeComplete PlanBudget Planned update range_checked plan_checked plan_fits RangeBudget /;s/(exports Span Entry Wrapper/(exports seal_cache Span Entry Wrapper/;$s/)$//' selfhost/slim.project > "$session_dir/probe/slim.project"
cat >> "$session_dir/probe/slim.project" <<'MANIFEST'
  (module zdamage "zdamage.slim" (imports fragments identity memory parallel project ranges retained session text) (exports check_budgets check_transition damage))
  (module zzprobe "zzprobe.slim" (imports identity memory project ranges retained session syntax typing zdamage) (exports)))
MANIFEST
if ! "$session_compiler" "$session_dir/probe/slim.project" > "$session_dir/probe.c"; then
  cat "$session_dir/probe.c" >&2
  exit 1
fi
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$session_dir/probe.c" runtime/slim_rt.c -o "$session_dir/ordinary"
awk -f scripts/instrument-session-probe.awk "$session_dir/probe.c" > "$session_dir/observed.c"
if test "$session_scope" != quick; then
  clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror \
    -I runtime -include benchmarks/instrumentation/session_probe.h "$session_dir/observed.c" \
    runtime/slim_rt.c benchmarks/instrumentation/session_probe.c -o "$session_dir/observed"
else
  clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime -include benchmarks/instrumentation/session_probe.h \
    "$session_dir/observed.c" runtime/slim_rt.c benchmarks/instrumentation/session_probe.c -o "$session_dir/observed"
fi
python3 -B scripts/verify-session.py "$session_dir" "$session_scope" "$session_compiler"
