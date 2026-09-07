#!/bin/sh
set -eu
integer_compiler=${1:-build/toolchain/slimc}
integer_scope=${2:-full}
case "$integer_scope" in quick|full) ;; *) exit 64 ;; esac
integer_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-integers.XXXXXX")
trap 'rm -rf "$integer_dir"' EXIT HUP INT TERM
if test "$integer_scope" = full; then
  clang -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Wall -Wextra -Werror -I runtime bootstrap/slimc-seed.c runtime/slim_rt.c \
    -o "$integer_dir/compiler-sanitized"
  python3 scripts/verify-integers.py "$integer_dir" "$integer_compiler" "$integer_dir/compiler-sanitized"
else
  python3 scripts/verify-integers.py "$integer_dir" "$integer_compiler"
fi
