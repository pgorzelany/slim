#!/bin/sh
set -eu

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$repo_dir"

cargo fmt --check
cargo clippy --all-targets -- -D warnings
cargo test
cargo run --quiet --bin slim-govern -- check
cargo run --quiet --bin slim-conform -- differential
cargo run --release --quiet --bin slim-bench -- scaling --quick
cargo run --release --quiet --bin slim-bench -- incremental --quick
cargo run --release --quiet --bin slim-bench -- project --quick

verify_dir=$(mktemp -d /tmp/slim-verify.XXXXXX)
trap 'rm -rf "$verify_dir"' EXIT HUP INT TERM

cargo run --quiet --bin slimc -- emit-c examples/vector_sum.slim -o "$verify_dir/program.c"
cargo run --quiet --bin slimc -- runtime "$verify_dir"
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
