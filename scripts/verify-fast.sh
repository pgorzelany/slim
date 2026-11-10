#!/bin/sh
# Focused feedback only. Full compiler/release gates remain mandatory.
set -eu

case "${1:-diagnostics}" in
    diagnostics|fixtures) scope=${1:-diagnostics} ;;
    *) echo "usage: $0 [diagnostics|fixtures]" >&2; exit 64 ;;
esac
test "$#" -le 1 || { echo "usage: $0 [diagnostics|fixtures]" >&2; exit 64; }
repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$repo_dir"

# Never run the short acceptance suite against a stale compiler binary.
./bootstrap.sh
python3 -B scripts/verify-repair-diagnostics.py --compiler build/toolchain/slimc
python3 -B scripts/test-native-session-diagnostics.py
if test "$scope" = fixtures; then
    python3 -B scripts/verify-fixtures.py
    cargo fmt --check
    cargo test --quiet --bin slim-bench work::tests
    cargo run --quiet --bin slim-govern -- check
fi
echo "focused $scope checks passed; full validation and review remain required"
