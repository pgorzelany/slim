#!/bin/sh
set -eu

slim_library_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
slim_library_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-library-docs.XXXXXX")
trap 'rm -rf "$slim_library_work"' EXIT HUP INT TERM

if test "$#" -gt 1; then
    echo "usage: scripts/generate-library-docs.sh [--check]" >&2
    exit 64
fi
if test "$#" -eq 1 && test "$1" != "--check"; then
    echo "usage: scripts/generate-library-docs.sh [--check]" >&2
    exit 64
fi

"$slim_library_root/slimc" interfaces "$slim_library_root/library/slim.project" -o "$slim_library_work/interfaces"
"$slim_library_root/slimc" build "$slim_library_root/library/std-docs.project" -o "$slim_library_work/std-docs"
"$slim_library_work/std-docs" \
    "$slim_library_work/interfaces/std_ascii.sli" \
    "$slim_library_work/interfaces/std_bytes.sli" \
    "$slim_library_work/interfaces/std_cursor.sli" \
    "$slim_library_work/interfaces/std_decimal.sli" \
    "$slim_library_work/interfaces/std_i64.sli" \
    "$slim_library_work/interfaces/std_i64_vec.sli" \
    "$slim_library_work/interfaces/std_span.sli" \
    "$slim_library_work/interfaces/std_test.sli" \
    "$slim_library_work/interfaces/std_text.sli" \
    "$slim_library_work/interfaces/std_u8_vec.sli" \
    > "$slim_library_work/REFERENCE.md"

if test "$#" -eq 1; then
    if ! cmp -s "$slim_library_work/REFERENCE.md" "$slim_library_root/library/REFERENCE.md"; then
        echo "library reference is stale; run scripts/generate-library-docs.sh" >&2
        exit 1
    fi
else
    cp "$slim_library_work/REFERENCE.md" "$slim_library_root/library/REFERENCE.md"
fi
