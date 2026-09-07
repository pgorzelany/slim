#!/bin/sh
set -eu
checking_compiler=${1:-build/toolchain/slimc}
checking_scope=${2:-full}
case "$checking_scope" in quick|full) ;; *) exit 64 ;; esac
checking_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-checking.XXXXXX")
trap 'rm -rf "$checking_dir"' EXIT HUP INT TERM

# Cross the explicit observation cap without a billion-step test. The override
# affects only this standalone counter boundary fixture.
clang -std=c11 -O1 -g -fsanitize=address,undefined -DSLIM_CHECKING_PROBE_CAP=4 \
  -Wall -Wextra -Werror -I benchmarks/instrumentation \
  tests/fixtures/checking_probe_limits.c benchmarks/instrumentation/checking_probe.c \
  -o "$checking_dir/counter-limits"
SLIM_CHECKING_REPORT="$checking_dir/exact.tsv" "$checking_dir/counter-limits" exact
SLIM_CHECKING_REPORT="$checking_dir/bounded.tsv" "$checking_dir/counter-limits" bounded
python3 - "$checking_dir" <<'PY'
from pathlib import Path
import sys
root = Path(sys.argv[1])
for status in ("exact", "bounded"):
    lines = (root / f"{status}.tsv").read_text().splitlines()
    assert lines[0] == f"slim-checking\t2\t{status}\t4", lines[0]
    values = dict((key, int(value)) for key, value in (line.split("\t") for line in lines[1:]))
    assert all(0 <= value <= 4 for value in values.values())
    assert values["steps"] == values["resume_17"] == values["builtin_11_5"] == values["builtin_2_1"] == 4
    assert values["machine_active_at_exit"] == 0
    assert values["machine_native_depth"] == 1
print("checking observation: exact cap and saturated cap passed")
PY

# Token comparison shares one immutable token read. Check virtual spellings,
# exact source spans, invalid indices and eager traps against an independent
# finite oracle before using it throughout the continuation corpus.
mkdir "$checking_dir/token-probe"
cp selfhost/*.slim "$checking_dir/token-probe/"
cp tests/fixtures/token_equality.slim "$checking_dir/token-probe/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' selfhost/slim.project > "$checking_dir/token-probe/slim.project"
cat >> "$checking_dir/token-probe/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports syntax) (exports)))
MANIFEST
"$checking_compiler" "$checking_dir/token-probe/slim.project" > "$checking_dir/token-probe.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime \
  "$checking_dir/token-probe.c" runtime/slim_rt.c -o "$checking_dir/token-probe-ordinary"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime "$checking_dir/token-probe.c" runtime/slim_rt.c \
  -o "$checking_dir/token-probe-sanitized"
python3 scripts/verify-token-equality.py "$checking_dir/token-probe-ordinary" "$checking_dir/token-probe-sanitized"

# Build the ordinary and observed compilers from exactly the same current source.
# The passed compiler is the sole producer; no Rust semantics or fallback exists.
"$checking_compiler" selfhost/slim.project > "$checking_dir/compiler.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime \
  "$checking_dir/compiler.c" runtime/slim_rt.c -o "$checking_dir/ordinary"
python3 scripts/instrument-checking-probe.py < "$checking_dir/compiler.c" > "$checking_dir/observed.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime \
  -include benchmarks/instrumentation/checking_probe.h "$checking_dir/observed.c" \
  runtime/slim_rt.c benchmarks/instrumentation/checking_probe.c -o "$checking_dir/observed"
python3 scripts/verify-checking-work.py --ordinary "$checking_dir/ordinary" \
  --observed "$checking_dir/observed" --report "$checking_dir/work.tsv"
python3 scripts/verify-continuation-ownership.py --compiler "$checking_dir/ordinary" \
  --report "$checking_dir/ownership.tsv"

if test "$checking_scope" = full; then
  clang -std=c11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined \
    -Wall -Wextra -Werror -I runtime -include benchmarks/instrumentation/checking_probe.h \
    "$checking_dir/observed.c" runtime/slim_rt.c benchmarks/instrumentation/checking_probe.c \
    -o "$checking_dir/observed-sanitized"
  python3 scripts/verify-checking-work.py --ordinary "$checking_dir/ordinary" \
    --observed "$checking_dir/observed-sanitized" --geometry-only \
    --report "$checking_dir/sanitized-work.tsv"
  mkdir "$checking_dir/probe"
  cp selfhost/*.slim "$checking_dir/probe/"
  cp tests/fixtures/checking_continuations.slim "$checking_dir/probe/zzprobe.slim"
  sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' selfhost/slim.project > "$checking_dir/probe/slim.project"
  cat >> "$checking_dir/probe/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports check syntax text typing) (exports)))
MANIFEST
  "$checking_compiler" "$checking_dir/probe/slim.project" > "$checking_dir/state.c"
  clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime \
    "$checking_dir/state.c" runtime/slim_rt.c -o "$checking_dir/state"
  clang -std=c11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined \
    -Wall -Wextra -Werror -I runtime "$checking_dir/state.c" runtime/slim_rt.c -o "$checking_dir/state-sanitized"
  python3 scripts/verify-checking-state.py --ordinary "$checking_dir/state" \
    --candidate "$checking_dir/state-sanitized" --faults --report "$checking_dir/state.tsv"
  python3 scripts/verify-checking-mutations.py --baseline "$checking_dir/state" \
    --candidate "$checking_dir/state-sanitized" --report "$checking_dir/mutations.tsv"
fi
