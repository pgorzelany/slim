#!/bin/sh
set -eu
process_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-native-process.XXXXXX")
trap 'rm -rf "$process_dir"' EXIT HUP INT TERM
process_dir=$(CDPATH= cd "$process_dir" && pwd -P)
python3 - "$process_dir" <<'PY'
from pathlib import Path
import hashlib,re,sys
root=Path(sys.argv[1]);host=Path('compiler/session.c').read_text();native=Path('compiler/native.c').read_text()
capture=host[host.index('static unsigned char *host_diagnostics;'):host.index('SlimUnit slim_print_bytes')]
clear=host[host.index('static void host_clear_capture'):host.index('static void host_u32')]
clock=native[native.index('static uint64_t native_now'):native.index('static void *native_allocate')]
process=native[native.index('static int native_group_live'):native.index('static bool native_system_identity')]
constants='\n'.join(re.search(r'^#define '+name+r' .+$',source,re.M)[0] for name,source in
    [('HOST_DIAGNOSTIC_LIMIT',host),('NATIVE_BYTE_LIMIT',native),('NATIVE_PROCESS_NS',native),
     ('NATIVE_CLEANUP_NS',native),('NATIVE_PROCESS_LIMIT',native)])
headers='''#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE 1
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <libproc.h>
#include <sys/proc.h>
static char native_directory[128];
static bool native_cleanup_failure;
'''
clock=clock.replace('native_now(void)', 'native_real_now(void)')
wrapper='''static bool test_clock_advance;
static uint64_t test_clock_origin;
static uint64_t native_now(void) {
    uint64_t now = native_real_now();
    if (test_clock_advance) {
        if (!test_clock_origin) test_clock_origin = now;
        if (now - test_clock_origin >= 150000000) return now + NATIVE_PROCESS_NS;
    }
    return now;
}
'''
(root/'fixture.c').write_text(headers+constants+'\n'+capture+clear+clock+wrapper+
    Path('tests/fixtures/native_process_probe.h').read_text()+'\n'+process+
    Path('tests/fixtures/native_process.c').read_text())
print('native-process-source\t'+hashlib.sha256(process.encode()).hexdigest(),flush=True)
print('native-process-clock\tproduction 180-second limit; test clock advances after 150 milliseconds',flush=True)
PY
"${CC:-cc}" -std=c11 -O2 -Wall -Wextra -Werror tests/fixtures/native_process_child.c -o "$process_dir/child"
for variant in ordinary sanitized; do
    case "$variant" in ordinary) flags= ;; sanitized) flags='-g -fsanitize=address,undefined -fno-omit-frame-pointer' ;; esac
    "${CC:-cc}" -std=c11 -O1 $flags -Wall -Wextra -Werror "$process_dir/fixture.c" -o "$process_dir/$variant"
    mkdir "$process_dir/work-$variant" "$process_dir/work-$variant/tmp"
    "$process_dir/$variant" "$process_dir/child" "$process_dir/work-$variant"
    printf 'native-process-variant\t%s\texact\n' "$variant"
done
