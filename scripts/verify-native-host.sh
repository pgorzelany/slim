#!/bin/sh
set -eu
native_host=${1:-build/toolchain/slim-session}
native_compiler=${2:-build/toolchain/slimc}
sh scripts/verify-native-process.sh
sh scripts/verify-native-copy.sh
native_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-native-host-verify.XXXXXX")
trap 'rm -rf "$native_dir"' EXIT HUP INT TERM
cp "$(dirname "$native_host")/session-identity.h" "$(dirname "$native_host")/native-inputs.h" "$native_dir/"
cp compiler/session.c compiler/native.c "$native_dir/"
if ! "$native_compiler" selfhost/slim.project > "$native_dir/slimc-seed.c"; then
    cat "$native_dir/slimc-seed.c" >&2
    exit 1
fi
python3 - "$native_dir" "$(dirname "$native_host")/session-build-identity.tsv" <<'PY'
from pathlib import Path
import hashlib,sys
root=Path(sys.argv[1]);identity=dict(line.split('\t') for line in Path(sys.argv[2]).read_text().splitlines())
assert hashlib.sha256((root/'slimc-seed.c').read_bytes()).hexdigest()==identity['seed'],'observer seed differs from ordinary host'
source=(root/'native.c').read_text()
anchor='        execve(argv[0], argv, environment);'
assert source.count(anchor)==1
hook=r'''        const char *trace_path = getenv("SLIM_NATIVE_EXEC_REPORT");
        if (trace_path && *trace_path) {
            char row[8192]; size_t at = 0;
            for (unsigned i=0; argv[i]; ++i) {
                int count=snprintf(row+at,sizeof(row)-at,"%s%s",i?"\t":"",argv[i]);
                if(count<0 || (size_t)count>=sizeof(row)-at) _exit(125);
                at+=(size_t)count;
            }
            if(at+1>=sizeof(row)) _exit(125);
            row[at++]='\n';
            int trace=open(trace_path,O_WRONLY|O_APPEND|O_CREAT|O_NOFOLLOW,0600);
            if(trace<0 || write(trace,row,at)!=(ssize_t)at || close(trace)!=0) _exit(125);
        }
'''
(root/'native.c').write_text(source.replace(anchor,hook+anchor))
source=(root/'session.c').read_text()
anchor='int main(int argc, char **argv) {'
assert source.count(anchor)==1
(root/'link-fixture.c').write_text(source.replace(anchor, 'int native_test_host_main(int argc, char **argv) {') + '\n' +
    Path('tests/fixtures/native_link_inputs.c').read_text())
PY
python3 -B scripts/instrument-native-host-state.py "$native_dir/native.c"
python3 -B scripts/instrument-native-resources.py "$native_dir"
"${CC:-cc}" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror -I runtime \
    -Dslim_print_bytes=slim_private_print_bytes -Dslim_print_i64=slim_private_print_i64 -Dslim_println=slim_private_println \
    -c "$native_dir/resource-runtime.c" -o "$native_dir/runtime.o"
"${CC:-cc}" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror -I runtime -I "$native_dir" \
    "$native_dir/session.c" "$native_dir/runtime.o" -o "$native_dir/observed"
"${CC:-cc}" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Werror -I runtime -I "$native_dir" \
    "$native_dir/link-fixture.c" "$native_dir/runtime.o" -o "$native_dir/link-fixture"
"$native_dir/link-fixture"
python3 -B scripts/verify-native-host.py "$native_host"
python3 -B scripts/verify-native-host.py "$native_dir/observed" --observe
python3 -B scripts/verify-native-corpus.py "$native_host" "$native_compiler"
python3 -B scripts/verify-native-corpus.py "$native_dir/observed" "$native_compiler" --observe
python3 -B scripts/verify-native-host-faults.py "$native_host"
python3 -B scripts/verify-native-host-faults.py "$native_dir/observed" --observe
python3 -B scripts/verify-native-recovery.py "$native_dir/observed"
python3 -B scripts/verify-native-state.py "$native_dir/observed"
python3 -B scripts/verify-native-resources.py "$native_dir/observed"
"${CC:-cc}" -std=c11 -O2 -Wall -Wextra -Werror -I runtime \
    -Dslim_print_bytes=slim_private_print_bytes -Dslim_print_i64=slim_private_print_i64 -Dslim_println=slim_private_println \
    -c "$native_dir/resource-runtime.c" -o "$native_dir/runtime-resource.o"
"${CC:-cc}" -std=c11 -O2 -Wall -Wextra -Werror -I runtime -I "$native_dir" \
    "$native_dir/session.c" "$native_dir/runtime-resource.o" -o "$native_dir/resource-ordinary"
python3 -B scripts/verify-native-resources.py "$native_dir/resource-ordinary" --case history --case boundary
