#!/bin/sh
set -eu

slim_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
slim_seed="$slim_root/bootstrap/slimc-seed.c"
slim_digest_file="$slim_root/bootstrap/slimc-seed.sha256"
slim_runtime_c="$slim_root/runtime/slim_rt.c"
slim_project="$slim_root/selfhost/slim.project"
slim_cc=${CC:-cc}
slim_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-seed-refresh.XXXXXX")
trap 'rm -rf "$slim_work"' EXIT HUP INT TERM

compile_native() {
    "$slim_cc" -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror \
        -I "$slim_root/runtime" "$1" "$slim_runtime_c" -o "$2"
}

compile_native "$slim_seed" "$slim_work/seed"
"$slim_work/seed" "$slim_project" > "$slim_work/generation-1.c"
compile_native "$slim_work/generation-1.c" "$slim_work/generation-1"
"$slim_work/generation-1" "$slim_project" > "$slim_work/generation-2.c"
compile_native "$slim_work/generation-2.c" "$slim_work/generation-2"
"$slim_work/generation-2" "$slim_project" > "$slim_work/generation-3.c"

if ! cmp -s "$slim_work/generation-2.c" "$slim_work/generation-3.c"; then
    echo "seed refresh: generations 2 and 3 differ; seed was not changed" >&2
    exit 1
fi

cp "$slim_work/generation-2.c" "$slim_seed"
if command -v sha256sum >/dev/null 2>&1; then
    slim_digest=$(sha256sum "$slim_seed" | awk '{ print $1 }')
elif command -v shasum >/dev/null 2>&1; then
    slim_digest=$(shasum -a 256 "$slim_seed" | awk '{ print $1 }')
else
    echo "seed refresh: sha256sum or shasum is required" >&2
    exit 1
fi
printf '%s  slimc-seed.c\n' "$slim_digest" > "$slim_digest_file"
slim_size=$(wc -c < "$slim_seed" | tr -d ' ')
echo "seed refresh: fixed point captured at $slim_size C bytes"
echo "seed refresh: SHA-256 $slim_digest"
