#!/bin/sh
set -eu

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$repo_dir"

./bootstrap.sh
python3 -B scripts/verify-project-lists.py --compiler build/toolchain/slimc \
  --generated-c build/toolchain/slimc.c --receipt build/project-list-work.json
python3 -B scripts/verify-project-namespace.py --compiler build/toolchain/slimc \
  --generated-c build/toolchain/slimc.c --receipt build/project-namespace-work.json
manifest_dir=$(mktemp -d "$PWD/build/manifest-validation.XXXXXX")
python3 -B scripts/verify-manifest-validation.py --current \
  --compiler build/toolchain/slimc --generated-c build/toolchain/slimc.c \
  --output "$manifest_dir/current"
rm -rf "$manifest_dir"
mkdir -p "$PWD/build/overnight-project-impact"
impact_dir=$(mktemp -d "$PWD/build/overnight-project-impact/check.XXXXXX")
python3 -B scripts/verify-project-impact.py --compiler build/toolchain/slimc \
  --output "$impact_dir/current"
rm -rf "$impact_dir"
context_dir=$(mktemp -d "$PWD/build/overnight-project-impact/context-check.XXXXXX")
python3 -B scripts/verify-project-impact-context.py --current \
  --output "$context_dir/current" --compiler build/toolchain/slimc --cc "$(command -v cc)"
rm -rf "$context_dir"
cargo fmt --check
cargo clippy --all-targets -- -D warnings
cargo test
cargo run --quiet --bin slim-govern -- check
python3 scripts/test-task-time.py
SLIM_DEVELOPMENT_COMPILER="$PWD/build/toolchain/slimc" python3 benchmarks/development/test_evaluate.py
cargo run --quiet --bin slim-conform -- check
./scripts/check-library-corpus.sh
cargo run --release --quiet --bin slim-bench -- performance --quick
cargo run --release --quiet --bin slim-bench -- work --quick
cargo run --release --quiet --bin slim-bench -- reduction --quick
cargo run --release --quiet --bin slim-bench -- parallelism
cargo run --release --quiet --bin slim-bench -- resources
cargo run --release --quiet --bin slim-bench -- host
cargo run --release --quiet --bin slim-bench -- parallel-runtime --quick
cargo run --release --quiet --bin slim-bench -- incremental --quick
cargo run --release --quiet --bin slim-bench -- project --quick
cargo run --release --quiet --bin slim-bench -- applications --quick
cargo run --release --quiet --bin slim-bench -- compare --quick
cargo run --release --quiet --bin slim-bench -- agent
./scripts/verify-continuations.sh build/toolchain/slimc full

verify_dir=$(mktemp -d /tmp/slim-verify.XXXXXX)
trap 'rm -rf "$verify_dir"' EXIT HUP INT TERM

python3 -B scripts/verify-development-operation-cost.py --compiler build/toolchain/slimc \
  --output "$verify_dir/development-operation-cost"

python3 -B scripts/verify-development-summary.py --compiler build/toolchain/slimc \
  --output "$verify_dir/development-summary"

python3 -B scripts/verify-project-input.py --compiler build/toolchain/slimc \
  --output "$verify_dir/project-input"

mkdir -p "$PWD/build/overnight-project-input-node-boundaries"
node_boundary_dir=$(mktemp -d "$PWD/build/overnight-project-input-node-boundaries/check.XXXXXX")
python3 -B scripts/verify-project-input-node-boundaries.py freeze \
  --output "$node_boundary_dir/frozen" > "$node_boundary_dir/freeze-summary.json"
node_model_sha=$(python3 -c 'import hashlib,sys; data=open(sys.argv[1],"rb").read(4194305); assert len(data)<=4194304; print(hashlib.sha256(data).hexdigest())' "$node_boundary_dir/frozen/model.json")
node_freeze_sha=$(python3 -c 'import hashlib,sys; data=open(sys.argv[1],"rb").read(4194305); assert len(data)<=4194304; print(hashlib.sha256(data).hexdigest())' "$node_boundary_dir/frozen/freeze.json")
python3 -B scripts/verify-project-input-node-boundaries.py run \
  --output "$node_boundary_dir/native" --held "$node_boundary_dir/frozen" \
  --model-sha "$node_model_sha" --freeze-sha "$node_freeze_sha"
rm -rf "$node_boundary_dir"

mkdir -p "$PWD/build/overnight-project-input-boundaries"
edge_boundary_dir=$(mktemp -d "$PWD/build/overnight-project-input-boundaries/check.XXXXXX")
python3 -B scripts/verify-project-input-boundaries.py run \
  --compiler build/toolchain/slimc --output "$edge_boundary_dir/current"
rm -rf "$edge_boundary_dir"

clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime \
  bootstrap/slimc-seed.c runtime/slim_rt.c -o "$verify_dir/slimc-seed-sanitized"
"$verify_dir/slimc-seed-sanitized" check examples/hello.slim
"$verify_dir/slimc-seed-sanitized" examples/hello.slim > "$verify_dir/hello.c"
test -s "$verify_dir/hello.c"
python3 scripts/verify-literal-storage.py "$verify_dir/slimc-seed-sanitized" --sanitize
python3 scripts/verify-hex-literals.py "$verify_dir/slimc-seed-sanitized" --sanitize

# Exercise the production identity module, including extreme and stale handles,
# under the same sanitizers as the compiler. The independent expected values are
# checked by Cargo; this gate compares ordinary and sanitized native execution.
cp selfhost/identity.slim "$verify_dir/identity.slim"
cp tests/fixtures/source_identity.slim "$verify_dir/probe.slim"
cat > "$verify_dir/slim.project" <<'EOF'
(project 1 (entry probe)
  (module identity "identity.slim" (imports) (exports DeclarationId FileId Index NextRevision NodeId Revision Span View reset resolve_node resolve_span successor))
  (module probe "probe.slim" (imports identity) (exports)))
EOF
"$verify_dir/slimc-seed-sanitized" "$verify_dir/slim.project" > "$verify_dir/identity.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime \
  "$verify_dir/identity.c" runtime/slim_rt.c -o "$verify_dir/identity"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime \
  "$verify_dir/identity.c" runtime/slim_rt.c -o "$verify_dir/identity-sanitized"
"$verify_dir/identity" > "$verify_dir/identity.out"
"$verify_dir/identity-sanitized" > "$verify_dir/identity-sanitized.out"
cmp "$verify_dir/identity.out" "$verify_dir/identity-sanitized.out"

mkdir "$verify_dir/maps"
cp selfhost/*.slim "$verify_dir/maps/"
cp tests/fixtures/revision_mapping.slim "$verify_dir/maps/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' \
  selfhost/slim.project > "$verify_dir/maps/slim.project"
cat >> "$verify_dir/maps/slim.project" <<'EOF'
  (module zzprobe "zzprobe.slim" (imports identity project query syntax) (exports)))
EOF
"$verify_dir/slimc-seed-sanitized" "$verify_dir/maps/slim.project" > "$verify_dir/maps.c"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime \
  "$verify_dir/maps.c" runtime/slim_rt.c -o "$verify_dir/maps-sanitized"
test "$("$verify_dir/maps-sanitized")" = "ok exact revision maps"

mkdir "$verify_dir/functions"
cp selfhost/*.slim "$verify_dir/functions/"
cp tests/fixtures/function_checking.slim "$verify_dir/functions/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' \
  selfhost/slim.project > "$verify_dir/functions/slim.project"
cat >> "$verify_dir/functions/slim.project" <<'EOF'
  (module zzprobe "zzprobe.slim" (imports check ir syntax typing) (exports)))
EOF
"$verify_dir/slimc-seed-sanitized" "$verify_dir/functions/slim.project" > "$verify_dir/functions.c"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime \
  "$verify_dir/functions.c" runtime/slim_rt.c -o "$verify_dir/functions-sanitized"
for function_fixture in conformance/pass/*.slim benchmarks/challenges/*/program.slim; do
  test "$("$verify_dir/functions-sanitized" "$function_fixture")" = "ok isolated function checking"
done

"$verify_dir/slimc-seed-sanitized" conformance/pass/inline_forward_layouts.slim > "$verify_dir/layouts.c"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime \
  "$verify_dir/layouts.c" runtime/slim_rt.c -o "$verify_dir/layouts-sanitized"
test "$("$verify_dir/layouts-sanitized")" = "42"

mkdir "$verify_dir/flow"
cp selfhost/*.slim "$verify_dir/flow/"
cp tests/fixtures/function_flow.slim "$verify_dir/flow/zzprobe.slim"
sed '/(module driver /d;s/(entry driver)/(entry zzprobe)/;$s/)$//' \
  selfhost/slim.project > "$verify_dir/flow/slim.project"
cat >> "$verify_dir/flow/slim.project" <<'EOF'
  (module zzprobe "zzprobe.slim" (imports check flow identity ir memory syntax text typing) (exports)))
EOF
"$verify_dir/slimc-seed-sanitized" "$verify_dir/flow/slim.project" > "$verify_dir/flow.c"
clang -std=c11 -O1 -Wall -Wextra -Werror -I runtime \
  "$verify_dir/flow.c" runtime/slim_rt.c -o "$verify_dir/flow-ordinary"
awk -f scripts/instrument-flow-probe.awk "$verify_dir/flow.c" > "$verify_dir/flow-observed.c"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime -include benchmarks/instrumentation/flow_probe.h \
  "$verify_dir/flow-observed.c" runtime/slim_rt.c benchmarks/instrumentation/flow_probe.c \
  -o "$verify_dir/flow-sanitized"
for flow_fixture in conformance/pass/*.slim benchmarks/challenges/*/program.slim; do
  "$verify_dir/flow-ordinary" "$flow_fixture" > "$verify_dir/flow.out"
  test "$(cat "$verify_dir/flow.out")" = "ok bounded flow"
  rm -f "$verify_dir/flow-work.tsv" "$verify_dir/flow-repeat.tsv"
  SLIM_FLOW_REPORT="$verify_dir/flow-work.tsv" "$verify_dir/flow-sanitized" "$flow_fixture" > "$verify_dir/flow-observed.out"
  cmp "$verify_dir/flow.out" "$verify_dir/flow-observed.out"
  SLIM_FLOW_REPORT="$verify_dir/flow-repeat.tsv" "$verify_dir/flow-sanitized" "$flow_fixture" > "$verify_dir/flow-repeat.out"
  cmp "$verify_dir/flow.out" "$verify_dir/flow-repeat.out"
  cmp "$verify_dir/flow-work.tsv" "$verify_dir/flow-repeat.tsv"
  awk -F '\t' 'NR == 1 { if ($0 != "slim-flow\t1\texact\t1000000000") exit 1 }
    NR == 2 { if (NF != 8 || $3 != $4 + $2 || $1 != $5 + $6 + $7 || $8 != 0) exit 1 }
    END { if (NR != 2) exit 1 }' "$verify_dir/flow-work.tsv"
done
flow_fault_at=1
flow_failed=0
flow_failed_in_walk=0
flow_succeeded=0
while test "$flow_fault_at" -le 512; do
  flow_ordinary_status=0
  SLIM_ALLOC_FAIL_AT="$flow_fault_at" "$verify_dir/flow-ordinary" examples/hello.slim \
    > "$verify_dir/flow-fault.out" 2> "$verify_dir/flow-fault.err" || flow_ordinary_status=$?
  rm -f "$verify_dir/flow-fault.tsv"
  flow_observed_status=0
  SLIM_FLOW_REPORT="$verify_dir/flow-fault.tsv" SLIM_ALLOC_FAIL_AT="$flow_fault_at" \
    "$verify_dir/flow-sanitized" examples/hello.slim > "$verify_dir/flow-observed.out" \
    2> "$verify_dir/flow-observed.err" || flow_observed_status=$?
  test "$flow_ordinary_status" -eq "$flow_observed_status"
  cmp "$verify_dir/flow-fault.out" "$verify_dir/flow-observed.out"
  cmp "$verify_dir/flow-fault.err" "$verify_dir/flow-observed.err"
  test "$(head -n 1 "$verify_dir/flow-fault.tsv")" = "$(printf 'slim-flow\t1\texact\t1000000000')"
  awk -F '\t' 'NR == 2 { if (NF != 8) exit 1; for (i = 1; i <= 8; i++) if ($i !~ /^[0-9]+$/ || $i > 1000000000) exit 1 } END { if (NR != 2) exit 1 }' "$verify_dir/flow-fault.tsv"
  if test "$flow_observed_status" -eq 71; then
    test ! -s "$verify_dir/flow-fault.out"
    flow_failed=$((flow_failed + 1))
    if awk -F '\t' 'NR == 2 { exit !($3 > 0) }' "$verify_dir/flow-fault.tsv"; then
      flow_failed_in_walk=$((flow_failed_in_walk + 1))
    fi
  else
    test "$flow_observed_status" -eq 0
    test "$(cat "$verify_dir/flow-fault.out")" = "ok bounded flow"
    flow_succeeded=$((flow_succeeded + 1))
  fi
  printf 'flow-fault\t%s\t%s\t' "$flow_fault_at" "$flow_observed_status"
  tail -n 1 "$verify_dir/flow-fault.tsv"
  flow_fault_at=$((flow_fault_at + 1))
done
test "$flow_failed" -gt 0
test "$flow_failed_in_walk" -gt 0
test "$flow_succeeded" -gt 0
echo "verification: flow faults $flow_failed, failures after task walking $flow_failed_in_walk, successes $flow_succeeded"

./scripts/verify-retained.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-inputs.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-session-analysis.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-session-host.sh build/toolchain/slim-session "$verify_dir/slimc-seed-sanitized"
python3 -B scripts/verify-session-host-identity.py
python3 -B scripts/measure-session-host.py
./scripts/verify-retained-project.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-session.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-parallel-query.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-native-cache.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-native-selection.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-native-platform.sh build/toolchain/slim-session "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-places.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-parsing.sh "$verify_dir/slimc-seed-sanitized"
sh scripts/verify-integers.sh build/toolchain/slimc full

sh scripts/build-session-estimate.sh "$verify_dir/slimc-seed-sanitized" "$verify_dir/estimate" sanitize
"$verify_dir/estimate/estimate" session conformance/projects/basic/slim.project \
  conformance/projects/basic/slim.project > "$verify_dir/identity-session.out"
identity_fault_at=1
identity_faults=0
while test "$identity_fault_at" -le 128; do
  if SLIM_ALLOC_FAIL_AT="$identity_fault_at" "$verify_dir/estimate/estimate" \
    session conformance/projects/basic/slim.project conformance/projects/basic/slim.project \
    > "$verify_dir/identity-fault.out" 2> "$verify_dir/identity-fault.err"; then
    # Ordinals beyond this input's allocations must preserve ordinary output.
    cmp "$verify_dir/identity-session.out" "$verify_dir/identity-fault.out"
    test ! -s "$verify_dir/identity-fault.err"
  else
    identity_fault_status=$?
    test "$identity_fault_status" -eq 71
    test ! -s "$verify_dir/identity-fault.out"
    test "$(cat "$verify_dir/identity-fault.err")" = "SLIM allocation failure: exhausted at allocation $identity_fault_at"
    identity_faults=$((identity_faults + 1))
  fi
  identity_fault_at=$((identity_fault_at + 1))
done
test "$identity_faults" -gt 0
echo "verification: $identity_faults source-index allocation failures checked in 128 ordinals"

if SLIM_ALLOC_FAIL_AT=1 build/toolchain/slimc check examples/hello.slim \
  >"$verify_dir/compiler-fault.out" 2>"$verify_dir/compiler-fault.err"; then
  echo "verification: compiler allocation failure unexpectedly succeeded" >&2
  exit 1
else
  compiler_fault_status=$?
fi
test "$compiler_fault_status" -eq 71
test ! -s "$verify_dir/compiler-fault.out"
test "$(cat "$verify_dir/compiler-fault.err")" = "SLIM allocation failure: exhausted at allocation 1"

# The sieve challenge covers Boolean vector literals, vector mutation, and
# punctuation-bearing identifiers in one production build/run path.
./slimc build benchmarks/challenges/sieve/program.slim -o "$verify_dir/sieve"
test "$("$verify_dir/sieve")" = "78498"

./slimc emit-c examples/vector_sum.slim -o "$verify_dir/program.c"
./slimc runtime "$verify_dir"
clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I "$verify_dir" \
  "$verify_dir/program.c" "$verify_dir/slim_rt.c" -o "$verify_dir/program"
test "$("$verify_dir/program")" = "4950"
for fault_at in 1 2 3 4 5 6; do
  if SLIM_ALLOC_FAIL_AT="$fault_at" "$verify_dir/program" >"$verify_dir/fault.out" 2>"$verify_dir/fault.err"; then
    echo "verification: injected allocation failure $fault_at unexpectedly succeeded" >&2
    exit 1
  else
    fault_status=$?
  fi
  test "$fault_status" -eq 71
  test ! -s "$verify_dir/fault.out"
  test "$(cat "$verify_dir/fault.err")" = "SLIM allocation failure: exhausted at allocation $fault_at"
done

echo "verification: all gates passed"
