#!/bin/sh
set -eu

slim_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
slim_version=$(sed -n '1p' "$slim_root/VERSION")
slim_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-release-verify.XXXXXX")
trap 'rm -rf "$slim_work"' EXIT HUP INT TERM

"$slim_root/scripts/package-release.sh" "$slim_work/first"
"$slim_root/scripts/package-release.sh" "$slim_work/second"
slim_archive="$slim_work/first/slim-$slim_version.tar.gz"
slim_second="$slim_work/second/slim-$slim_version.tar.gz"

if ! cmp -s "$slim_archive" "$slim_second"; then
    echo "release verification: repeated archives differ" >&2
    exit 1
fi

bsdtar -tf "$slim_archive" > "$slim_work/archive-paths"
if awk '
    /^\// || /(^|\/)\.\.(\/|$)/ { invalid = 1 }
    END { exit invalid ? 0 : 1 }
' "$slim_work/archive-paths"
then
    echo "release verification: archive contains an unsafe path" >&2
    exit 1
fi

mkdir "$slim_work/install"
bsdtar -xf "$slim_archive" -C "$slim_work/install"
slim_install="$slim_work/install/slim-$slim_version"
test -d "$slim_install"
test ! -e "$slim_install/.git"
test ! -e "$slim_install/website"
test ! -e "$slim_install/build"
test ! -e "$slim_install/target"

(
    cd "$slim_install"
    while read -r slim_digest slim_path; do
        if command -v sha256sum >/dev/null 2>&1; then
            slim_actual=$(sha256sum "$slim_path" | awk '{ print $1 }')
        else
            slim_actual=$(shasum -a 256 "$slim_path" | awk '{ print $1 }')
        fi
        if test "$slim_actual" != "$slim_digest"; then
            echo "release verification: source digest mismatch: $slim_path" >&2
            exit 1
        fi
    done < SOURCE-MANIFEST.sha256

    test "$(./slimc --version)" = "slimc $slim_version (self-hosted)"
    ./bootstrap.sh
    ./slimc check examples/hello.slim
    test "$(./slimc run examples/hello.slim)" = "hello from SLIM"
    test "$(./slimc run examples/bytes.slim)" = "Hi"
    test "$(./slimc run examples/countdown.slim)" = "55"
    test "$(./slimc run examples/data.slim)" = "42"
    test "$(./slimc run examples/inout.slim)" = "42"
    test "$(./slimc run examples/vector_sum.slim)" = "4950"
    test "$(./slimc run conformance/projects/basic/slim.project)" = "41"
    test "$(./slimc run benchmarks/challenges/sieve/program.slim)" = "78498"
    test "$(./slimc analyze examples/countdown.slim | sed -n '1p')" != ""
    test "$(./slimc builtins | sed -n '1p')" = "i64.add"

    ./slimc emit-c examples/hello.slim -o "$slim_work/hello.c"
    sed 's/#define SLIM_RUNTIME_ABI_VERSION 1/#define SLIM_RUNTIME_ABI_VERSION 2/' \
        runtime/slim_rt.h > "$slim_work/slim_rt.h"
    if "${CC:-cc}" -std=c11 -Wall -Wextra -Werror -I "$slim_work" \
        -c "$slim_work/hello.c" -o "$slim_work/mismatch.o" \
        > "$slim_work/mismatch.out" 2> "$slim_work/mismatch.err"
    then
        echo "release verification: mismatched runtime ABI compiled" >&2
        exit 1
    fi
)

echo "release verification: reproducible archive and clean install passed"
