#!/bin/sh
# Known reference provider must pass positive tests; other hosts must decline B.
set -eu
native_host=${1:-build/toolchain/slim-session}
native_compiler=${2:-build/toolchain/slimc}
native_scope=${3:-full}
case "$native_scope" in full|smoke) ;; *) exit 64 ;; esac
native_supported=0
if test "$(uname -s):$(uname -m)" = Darwin:arm64; then
    if native_clang=$(xcrun --find clang); then
        native_version=$("$native_clang" --no-default-config --version | sed -n '1p')
        case "$native_version" in 'Apple clang version 21.'*) native_supported=1 ;; esac
    fi
fi
if test "$native_supported" -eq 0; then
    python3 -B scripts/verify-native-unavailable.py "$native_host"
elif test "$native_scope" = full; then
    sh scripts/verify-native-host.sh "$native_host" "$native_compiler"
    python3 -B scripts/test-native-session-diagnostics.py
    python3 -B scripts/measure-native-session.py
else
    python3 -B - "$native_host" <<'PY'
import runpy,sys
runpy.run_path('scripts/verify-native-host.py')['smoke'](sys.argv[1])
PY
fi
