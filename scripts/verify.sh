#!/bin/sh
set -eu

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$repo_dir"

./bootstrap.sh
cargo fmt --check
cargo clippy --all-targets -- -D warnings
cargo test
cargo run --quiet --bin slim-govern -- check
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

verify_dir=$(mktemp -d /tmp/slim-verify.XXXXXX)
trap 'rm -rf "$verify_dir"' EXIT HUP INT TERM

clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Wall -Wextra -Werror -I runtime \
  bootstrap/slimc-seed.c runtime/slim_rt.c -o "$verify_dir/slimc-seed-sanitized"
"$verify_dir/slimc-seed-sanitized" check examples/hello.slim
"$verify_dir/slimc-seed-sanitized" examples/hello.slim > "$verify_dir/hello.c"
test -s "$verify_dir/hello.c"

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

"$verify_dir/slimc-seed-sanitized" session conformance/projects/basic/slim.project \
  conformance/projects/basic/slim.project > "$verify_dir/identity-session.out"
identity_fault_at=1
identity_faults=0
while test "$identity_fault_at" -le 128; do
  if SLIM_ALLOC_FAIL_AT="$identity_fault_at" "$verify_dir/slimc-seed-sanitized" \
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
