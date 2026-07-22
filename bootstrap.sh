#!/bin/sh
set -eu

slim_root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
slim_seed="$slim_root/bootstrap/slimc-seed.c"
slim_digest_file="$slim_root/bootstrap/slimc-seed.sha256"
slim_runtime_c="$slim_root/runtime/slim_rt.c"
slim_runtime_h="$slim_root/runtime/slim_rt.h"
slim_project="$slim_root/selfhost/slim.project"
slim_cc=${CC:-cc}
slim_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-bootstrap.XXXXXX")
trap 'rm -rf "$slim_work"' EXIT HUP INT TERM

slim_expected=$(awk 'NR == 1 { print $1 }' "$slim_digest_file")
if command -v sha256sum >/dev/null 2>&1; then
    slim_actual=$(sha256sum "$slim_seed" | awk '{ print $1 }')
elif command -v shasum >/dev/null 2>&1; then
    slim_actual=$(shasum -a 256 "$slim_seed" | awk '{ print $1 }')
else
    echo "bootstrap: sha256sum or shasum is required to verify the seed" >&2
    exit 1
fi

if test "$slim_actual" != "$slim_expected"; then
    echo "bootstrap: seed digest mismatch" >&2
    echo "expected: $slim_expected" >&2
    echo "actual:   $slim_actual" >&2
    exit 1
fi

compile_native() {
    "$slim_cc" -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror \
        -I "$slim_root/runtime" "$1" "$slim_runtime_c" -o "$2"
}

compile_native "$slim_seed" "$slim_work/seed"
"$slim_work/seed" "$slim_project" > "$slim_work/generation-1.c"

if ! cmp -s "$slim_seed" "$slim_work/generation-1.c"; then
    echo "bootstrap: checked-in seed does not reproduce from selfhost/slim.project" >&2
    exit 1
fi

compile_native "$slim_work/generation-1.c" "$slim_work/generation-1"
"$slim_work/generation-1" "$slim_project" > "$slim_work/generation-2.c"

if ! cmp -s "$slim_work/generation-1.c" "$slim_work/generation-2.c"; then
    echo "bootstrap: successive SLIM compiler generations differ" >&2
    exit 1
fi

"$slim_work/generation-1" "$slim_root/examples/hello.slim" > "$slim_work/hello.c"
compile_native "$slim_work/hello.c" "$slim_work/hello"
"$slim_work/hello" > "$slim_work/hello.out"
printf 'hello from SLIM\n' > "$slim_work/hello.expected"
if ! cmp -s "$slim_work/hello.expected" "$slim_work/hello.out"; then
    echo "bootstrap: native smoke program produced unexpected output" >&2
    exit 1
fi

slim_output="$slim_root/build/toolchain"
mkdir -p "$slim_output"
cp "$slim_work/generation-1" "$slim_output/slimc.next"
mv -f "$slim_output/slimc.next" "$slim_output/slimc"
cp "$slim_work/generation-2.c" "$slim_output/slimc.c"
cp "$slim_runtime_c" "$slim_output/slim_rt.c"
cp "$slim_runtime_h" "$slim_output/slim_rt.h"

slim_size=$(wc -c < "$slim_work/generation-2.c" | tr -d ' ')
echo "bootstrap: fixed point verified at $slim_size C bytes"
echo "bootstrap: compiler available at build/toolchain/slimc"
