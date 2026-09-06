#!/bin/sh
set -eu
project_compiler=${1:-build/toolchain/slimc}
project_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-retained-project.XXXXXX")
trap 'rm -rf "$project_dir"' EXIT HUP INT TERM
mkdir "$project_dir/probe" "$project_dir/input" "$project_dir/rejected"
cp selfhost/*.slim "$project_dir/probe/"
cp tests/fixtures/retained_project.slim "$project_dir/probe/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' selfhost/slim.project > "$project_dir/probe/slim.project"
cat >> "$project_dir/probe/slim.project" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports identity memory project retained syntax typing) (exports)))
MANIFEST
"$project_compiler" "$project_dir/probe/slim.project" > "$project_dir/probe.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime "$project_dir/probe.c" runtime/slim_rt.c -o "$project_dir/ordinary"
awk -v scope=project -f scripts/instrument-retained-probe.awk "$project_dir/probe.c" > "$project_dir/observed.c"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror \
  -I runtime -include benchmarks/instrumentation/retained_probe.h "$project_dir/observed.c" \
  runtime/slim_rt.c benchmarks/instrumentation/retained_probe.c -o "$project_dir/observed"
project_cases=0
project_manifest="$project_dir/input/slim.project"
for project_fixture in conformance/pass/*.slim benchmarks/challenges/*/program.slim; do
  project_module=$(awk '$1 == "module" { print $2; exit }' "$project_fixture")
  printf '(project 1 (entry %s) (module %s "program.slim" (imports) (exports)))\n' "$project_module" "$project_module" > "$project_manifest"
  cp "$project_fixture" "$project_dir/input/program.slim"
  "$project_dir/ordinary" "$project_manifest" "$project_manifest" work > "$project_dir/ordinary.out"
  SLIM_RETAINED_REPORT="$project_dir/report.tsv" "$project_dir/observed" "$project_manifest" "$project_manifest" work > "$project_dir/observed.out"
  SLIM_RETAINED_REPORT="$project_dir/repeat.tsv" "$project_dir/observed" "$project_manifest" "$project_manifest" work > "$project_dir/repeat.out"
  cmp "$project_dir/ordinary.out" "$project_dir/observed.out"
  cmp "$project_dir/ordinary.out" "$project_dir/repeat.out"
  cmp "$project_dir/report.tsv" "$project_dir/repeat.tsv"
  awk -F '\t' 'NR == 1 { if ($0 != "slim-retained\t1\texact\t1000000000") exit 1 }
    NR == 2 { if ($1 != 0 || $2 <= 0) exit 1 }
    NR == 3 { if ($1 != 1 || $2 != 0) exit 1 } END { if (NR != 3) exit 1 }' "$project_dir/report.tsv"
  project_cases=$((project_cases + 1))
done
printf '(project 1 (entry hello) (module hello "program.slim" (imports) (exports)))\n' > "$project_manifest"
cp examples/hello.slim "$project_dir/input/program.slim"
cp "$project_manifest" "$project_dir/rejected/slim.project"
printf 'module hello\n\nfn main(args: Vec[Bytes]) -> I64:\n  false\n' > "$project_dir/rejected/program.slim"
for project_mode in stale capacity recover; do
  project_updated="$project_manifest"
  if test "$project_mode" = recover; then project_updated="$project_dir/rejected/slim.project"; fi
  "$project_dir/ordinary" "$project_manifest" "$project_updated" "$project_mode" > "$project_dir/ordinary.out"
  SLIM_RETAINED_REPORT="$project_dir/report.tsv" "$project_dir/observed" "$project_manifest" "$project_updated" "$project_mode" > "$project_dir/observed.out"
  cmp "$project_dir/ordinary.out" "$project_dir/observed.out"
  awk -v recovery="$project_mode" -F '\t' 'NR == 1 { if ($0 != "slim-retained\t1\texact\t1000000000") exit 1 }
    NR == 2 || NR == 3 { if ($1 != NR-2 || $2 != 1) exit 1 }
    NR == 4 { if ($1 != 2 || $2 != 0) exit 1 }
    END { if (NR != (recovery == "recover" ? 4 : 3)) exit 1 }' "$project_dir/report.tsv"
done
project_failed=0
project_succeeded=0
project_at=1
while test "$project_at" -le 2048; do
  project_status=0
  SLIM_ALLOC_FAIL_AT="$project_at" "$project_dir/ordinary" "$project_manifest" "$project_manifest" work \
    > "$project_dir/ordinary.out" 2> "$project_dir/ordinary.err" || project_status=$?
  project_observed_status=0
  rm -f "$project_dir/report.tsv"
  SLIM_RETAINED_REPORT="$project_dir/report.tsv" SLIM_ALLOC_FAIL_AT="$project_at" \
    "$project_dir/observed" "$project_manifest" "$project_manifest" work \
    > "$project_dir/observed.out" 2> "$project_dir/observed.err" || project_observed_status=$?
  test "$project_status" -eq "$project_observed_status"
  cmp "$project_dir/ordinary.out" "$project_dir/observed.out"
  cmp "$project_dir/ordinary.err" "$project_dir/observed.err"
  awk -F '\t' 'NR == 1 { if ($0 != "slim-retained\t1\texact\t1000000000") exit 1 }
    NR > 1 { if (NF != 2 || $1 != NR-2 || $2 !~ /^[0-9]+$/ || $2 > 1000000000) exit 1 }
    END { if (NR < 1 || NR > 3) exit 1 }' "$project_dir/report.tsv"
  if test "$project_status" -eq 71; then
    test ! -s "$project_dir/ordinary.out"
    project_failed=$((project_failed + 1))
  else
    test "$project_status" -eq 0
    project_succeeded=$((project_succeeded + 1))
  fi
  printf 'retained-project-fault\t%s\t%s\t' "$project_at" "$project_status"
  awk -F '\t' 'NR == 2 { first=$2 } NR == 3 { second=$2 } END { printf "%d\t%d\t%d\n", NR-1, first, second }' "$project_dir/report.tsv"
  project_at=$((project_at + 1))
done
test "$project_failed" -gt 0
test "$project_succeeded" -gt 0
echo "verification: retained projects $project_cases fixtures, $project_failed allocation failures, $project_succeeded successful fault ordinals"
