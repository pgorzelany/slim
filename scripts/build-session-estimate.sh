#!/bin/sh
# Measurement fixture only. Never installed as a public compiler command.
set -eu
estimate_compiler=$1
estimate_output=$2
estimate_mode=${3:-ordinary}
case "$estimate_mode" in ordinary|sanitize) ;; *) exit 64 ;; esac
mkdir -p "$estimate_output/project"
cp selfhost/*.slim "$estimate_output/project/"
cp tests/fixtures/session_estimate.slim "$estimate_output/project/driver.slim"
sed 's/(module driver "driver.slim" (imports compiler)/(module driver "driver.slim" (imports session)/' \
    selfhost/slim.project > "$estimate_output/project/slim.project"
"$estimate_compiler" "$estimate_output/project/slim.project" > "$estimate_output/estimate.c"
if test "$estimate_mode" = sanitize; then
    set -- -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
else
    set -- -O2 -DNDEBUG
fi
"${CC:-cc}" -std=c11 "$@" -Wall -Wextra -Werror -I runtime \
    "$estimate_output/estimate.c" runtime/slim_rt.c -o "$estimate_output/estimate"
