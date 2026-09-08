#!/bin/sh
set -eu
copy_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-native-copy.XXXXXX")
trap 'rm -rf "$copy_work"' EXIT HUP INT TERM
python3 - "$copy_work" <<'PY'
from pathlib import Path
import hashlib,sys
source=Path('compiler/native-copy.c').read_text()
hook='''static int test_clone(int source, int destination, const char *path, int flags) {
    const char *fault = getenv("SLIM_COPY_TEST_FAULT");
    if (fault && strcmp(fault, "fallback") == 0) { errno = ENOTSUP; return -1; }
    int result = fclonefileat(source, destination, path, flags);
    if (result == 0 && fault) {
        int fd = open(path, O_WRONLY); if (fd < 0) _exit(90);
        struct stat info; if (fstat(fd, &info) != 0) _exit(90);
        if (strcmp(fault, "changed-copy") == 0) { if (pwrite(fd, "x", 1, 0) != 1) _exit(90); }
        else if (strcmp(fault, "short-copy") == 0) { if (ftruncate(fd, info.st_size-1) != 0) _exit(90); }
        else if (strcmp(fault, "grown-copy") == 0) { if (ftruncate(fd, info.st_size+1) != 0) _exit(90); }
        else _exit(90);
        if (close(fd) != 0) _exit(90);
    }
    return result;
}
'''
anchor='copied = fclonefileat(source, AT_FDCWD, separator, CLONE_NOOWNERCOPY) == 0;'
assert source.count(anchor)==1
changed=source.replace(anchor,'copied = test_clone(source, AT_FDCWD, separator, CLONE_NOOWNERCOPY) == 0;')
changed=changed.replace('int main(int argc, char **argv) {',hook+'\nint main(int argc, char **argv) {')
(Path(sys.argv[1])/'copy.c').write_text(changed)
print('native-copy-source\t'+hashlib.sha256(source.encode()).hexdigest(),flush=True)
PY
for variant in ordinary sanitized; do
    case "$variant" in ordinary) flags=; large=--large ;; sanitized) flags='-g -fsanitize=address,undefined -fno-omit-frame-pointer'; large= ;; esac
    "${CC:-cc}" -std=c11 -O1 $flags -Wall -Wextra -Werror "$copy_work/copy.c" -o "$copy_work/$variant"
    python3 -B scripts/verify-native-copy.py "$copy_work/$variant" $large
    printf 'native-copy-variant\t%s\texact\n' "$variant"
done
