#!/bin/sh
set -eu
host_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
host_output=${1:-"$host_root/build/toolchain"}
test "$#" -le 1 || { echo 'usage: build-session-host.sh [OUTPUT_DIRECTORY]' >&2; exit 64; }
host_cc=${CC:-cc}
host_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-session-build.XXXXXX")
host_publication=
host_cleanup() {
    rm -rf "$host_work"
    if test -n "$host_publication"; then
        rm -f "$host_publication"
    fi
}
trap host_cleanup EXIT HUP INT TERM
# Fingerprint and compile the same captured inputs, even if the checkout changes.
cp "$host_root/bootstrap/slimc-seed.c" "$host_work/slimc-seed.c"
cp "$host_root/bootstrap/slimc-seed.sha256" "$host_work/slimc-seed.sha256"
cp "$host_root/runtime/slim_rt.c" "$host_root/runtime/slim_rt.h" "$host_work/"
cp "$host_root/compiler/session.c" "$host_work/session.c"
cp "$host_root/compiler/native.c" "$host_root/compiler/native-context.sh" "$host_root/compiler/native-copy.c" "$host_work/"
"$host_cc" -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror "$host_work/native-copy.c" -o "$host_work/copy-inputs"
cp "$host_root/scripts/embed-native-inputs.sh" "$host_work/embed-native-inputs.sh"
sh "$host_work/embed-native-inputs.sh" "$host_work"
cp "$host_root/scripts/build-session-host.sh" "$host_work/build-session-host.sh"
host_digest() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum < "$1" | awk 'NR == 1 { print $1 }'
    else
        shasum -a 256 < "$1" | awk 'NR == 1 { print $1 }'
    fi
}
host_seed=$(host_digest "$host_work/slimc-seed.c")
host_expected=$(awk 'NR == 1 { print $1 }' "$host_work/slimc-seed.sha256")
test "$host_seed" = "$host_expected" || { echo 'session host: seed digest mismatch' >&2; exit 1; }
host_cc_path=$(command -v "$host_cc")
test -f "$host_cc_path" && test -x "$host_cc_path" || { echo 'session host: CC must name an executable file' >&2; exit 1; }
"$host_cc" --version > "$host_work/cc-version"
host_target=$("$host_cc" -dumpmachine)
case "$host_target" in ''|*[!A-Za-z0-9_.-]*) echo 'session host: invalid native target identity' >&2; exit 1 ;; esac
test "${#host_target}" -le 128 || exit 1
case "${SLIM_SESSION_SANITIZE:-0}" in
    0) set -- -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror ;;
    1) set -- -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror ;;
    *) echo 'session host: SLIM_SESSION_SANITIZE must be 0 or 1' >&2; exit 64 ;;
esac
printf '%s\n' "$@" > "$host_work/options"
printf '%s\n' 'session-protocol=1' 'operation=retained-project-c' 'diagnostic-capture=1048576' \
    'runtime-print-symbols=private' >> "$host_work/options"
host_options=$(host_digest "$host_work/options")
{
    printf 'header\t%s\n' "$(host_digest "$host_work/slim_rt.h")"
    printf 'source\t%s\n' "$(host_digest "$host_work/slim_rt.c")"
    printf 'abi\t1\n'
} > "$host_work/runtime-identity"
host_runtime=$(host_digest "$host_work/runtime-identity")
{
    printf 'seed\t%s\n' "$host_seed"
    printf 'adapter\t%s\n' "$(host_digest "$host_work/session.c")"
    printf 'native-copy-source\t%s\n' "$(host_digest "$host_work/native-copy.c")"
    printf 'native-adapter\t%s\n' "$(host_digest "$host_work/native.c")"
    printf 'native-recipe\t%s\n' "$(host_digest "$host_work/native-context.sh")"
    printf 'native-inputs\t%s\n' "$(host_digest "$host_work/native-inputs.h")"
    printf 'native-embedding\t%s\n' "$(host_digest "$host_work/embed-native-inputs.sh")"
    printf 'recipe\t%s\n' "$(host_digest "$host_work/build-session-host.sh")"
    printf 'runtime\t%s\n' "$host_runtime"
    printf 'cc-binary\t%s\n' "$(host_digest "$host_cc_path")"
    printf 'cc-version\t%s\n' "$(host_digest "$host_work/cc-version")"
    printf 'target\t%s\n' "$host_target"
    printf 'options\t%s\n' "$host_options"
} > "$host_work/build-identity"
host_compiler=$(host_digest "$host_work/build-identity")
{
    printf '#define SLIM_SESSION_COMPILER_ID "%s"\n' "$host_compiler"
    printf '#define SLIM_SESSION_RUNTIME_ID "%s"\n' "$host_runtime"
    printf '#define SLIM_SESSION_TARGET "%s"\n' "$host_target"
    printf '#define SLIM_SESSION_OPTIONS_ID "%s"\n' "$host_options"
} > "$host_work/session-identity.h"
"$host_cc" "$@" -I "$host_work" \
    -Dslim_print_bytes=slim_private_print_bytes \
    -Dslim_print_i64=slim_private_print_i64 \
    -Dslim_println=slim_private_println \
    -c "$host_work/slim_rt.c" -o "$host_work/runtime.o"
"$host_cc" "$@" -I "$host_work" \
    "$host_work/session.c" "$host_work/runtime.o" -o "$host_work/slim-session"
mkdir -p "$host_output"
host_publication=$(mktemp "$host_output/.slim-session.XXXXXX")
cp "$host_work/slim-session" "$host_publication"
chmod 755 "$host_publication"
mv -f "$host_publication" "$host_output/slim-session"
host_publication=
cp "$host_work/session-identity.h" "$host_output/session-identity.h"
cp "$host_work/native.c" "$host_work/native-inputs.h" "$host_output/"
cp "$host_work/build-identity" "$host_output/session-build-identity.tsv"
printf 'session host: %s (%s)\n' "$host_output/slim-session" "$host_compiler"
