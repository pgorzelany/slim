#!/bin/sh
set -eu
retained_compiler=${1:-build/toolchain/slimc}
retained_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-retained.XXXXXX")
trap 'rm -rf "$retained_dir"' EXIT HUP INT TERM
mkdir "$retained_dir/project"
cp selfhost/*.slim "$retained_dir/project/"
cp tests/fixtures/retained_typing.slim "$retained_dir/project/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' selfhost/slim.project > "$retained_dir/project/slim.project"
cat >> "$retained_dir/project/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports check codegen identity retained syntax typing) (exports)))
MANIFEST
"$retained_compiler" "$retained_dir/project/slim.project" > "$retained_dir/probe.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$retained_dir/probe.c" runtime/slim_rt.c -o "$retained_dir/ordinary"
awk -f scripts/instrument-retained-probe.awk "$retained_dir/probe.c" > "$retained_dir/observed.c"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror \
  -I runtime -include benchmarks/instrumentation/retained_probe.h "$retained_dir/observed.c" \
  runtime/slim_rt.c benchmarks/instrumentation/retained_probe.c -o "$retained_dir/observed"
retained_cases=0
for retained_fixture in conformance/pass/*.slim benchmarks/challenges/*/program.slim; do
  "$retained_dir/ordinary" "$retained_fixture" "$retained_fixture" > "$retained_dir/ordinary.out"
  rm -f "$retained_dir/report.tsv" "$retained_dir/repeat.tsv"
  SLIM_RETAINED_REPORT="$retained_dir/report.tsv" "$retained_dir/observed" "$retained_fixture" "$retained_fixture" > "$retained_dir/observed.out"
  SLIM_RETAINED_REPORT="$retained_dir/repeat.tsv" "$retained_dir/observed" "$retained_fixture" "$retained_fixture" > "$retained_dir/repeat.out"
  cmp "$retained_dir/ordinary.out" "$retained_dir/observed.out"
  cmp "$retained_dir/ordinary.out" "$retained_dir/repeat.out"
  cmp "$retained_dir/report.tsv" "$retained_dir/repeat.tsv"
  awk 'NF != 3 || $1 != 0 || $2 <= 0 || $3 <= 0 { exit 1 } END { if (NR != 1) exit 1 }' "$retained_dir/ordinary.out"
  awk -F '\t' 'NR == 1 { if ($0 != "slim-retained\t1\texact\t1000000000") exit 1 }
    NR == 2 { if ($1 != 0 || $2 <= 0) exit 1 }
    NR == 3 { if ($1 != 1 || $2 != 0) exit 1 } END { if (NR != 3) exit 1 }' "$retained_dir/report.tsv"
  retained_cases=$((retained_cases + 1))
done
"$retained_dir/ordinary" examples/hello.slim conformance/fail/use_after_move.slim boundaries > "$retained_dir/boundary-ordinary.out"
SLIM_RETAINED_REPORT="$retained_dir/boundary.tsv" "$retained_dir/observed" examples/hello.slim conformance/fail/use_after_move.slim boundaries > "$retained_dir/boundary-observed.out"
cmp "$retained_dir/boundary-ordinary.out" "$retained_dir/boundary-observed.out"
awk -F '\t' 'NR == 1 { if ($0 != "slim-retained\t1\texact\t1000000000") exit 1 }
  NR == 6 || NR == 7 || NR == 11 || NR == 13 { if ($2 != 0) exit 1 }
  NR == 4 || NR == 5 || NR == 8 || NR == 9 || NR == 10 || NR == 12 || NR == 14 { if ($2 != 1) exit 1 }
  END { if (NR != 14) exit 1 }' "$retained_dir/boundary.tsv"
retained_failed=0
retained_succeeded=0
retained_at=1
while test "$retained_at" -le 512; do
  retained_status=0
  SLIM_ALLOC_FAIL_AT="$retained_at" "$retained_dir/ordinary" examples/hello.slim examples/hello.slim \
    > "$retained_dir/ordinary.out" 2> "$retained_dir/ordinary.err" || retained_status=$?
  retained_observed_status=0
  rm -f "$retained_dir/report.tsv"
  SLIM_RETAINED_REPORT="$retained_dir/report.tsv" SLIM_ALLOC_FAIL_AT="$retained_at" \
    "$retained_dir/observed" examples/hello.slim examples/hello.slim \
    > "$retained_dir/observed.out" 2> "$retained_dir/observed.err" || retained_observed_status=$?
  test "$retained_status" -eq "$retained_observed_status"
  cmp "$retained_dir/ordinary.out" "$retained_dir/observed.out"
  cmp "$retained_dir/ordinary.err" "$retained_dir/observed.err"
  awk -F '\t' 'NR == 1 { if ($0 != "slim-retained\t1\texact\t1000000000") exit 1 }
    NR > 1 { if (NF != 2 || $1 != NR-2 || $2 !~ /^[0-9]+$/ || $2 > 1000000000) exit 1 }
    END { if (NR < 1 || NR > 3) exit 1 }' "$retained_dir/report.tsv"
  if test "$retained_status" -eq 71; then
    test ! -s "$retained_dir/ordinary.out"
    retained_failed=$((retained_failed + 1))
  else
    test "$retained_status" -eq 0
    retained_succeeded=$((retained_succeeded + 1))
  fi
  printf 'retained-fault\t%s\t%s\t' "$retained_at" "$retained_status"
  awk -F '\t' 'NR == 2 { first=$2 } NR == 3 { second=$2 } END { printf "%d\t%d\t%d\n", NR-1, first, second }' "$retained_dir/report.tsv"
  retained_at=$((retained_at + 1))
done
test "$retained_failed" -gt 0
test "$retained_succeeded" -gt 0
echo "verification: retained typing $retained_cases fixtures, $retained_failed allocation failures, $retained_succeeded successful fault ordinals"
