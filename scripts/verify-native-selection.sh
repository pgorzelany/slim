#!/bin/sh
set -eu
selection_compiler=${1:-build/toolchain/slimc}
selection_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-native-selection.XXXXXX")
trap 'rm -rf "$selection_dir"' EXIT HUP INT TERM
if ! "$selection_compiler" selfhost/slim.project > "$selection_dir/slimc-seed.c"; then
    cat "$selection_dir/slimc-seed.c" >&2
    exit 1
fi
python3 - "$selection_dir" <<'PY'
from pathlib import Path
import sys
root=Path(sys.argv[1])
for name,source in [('before','module main\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'),
                    ('after','module main\n\nfn main(args: Vec[Bytes]) -> I64:\n  1\n'),
                    ('invalid','module main\n\nfn main(args: Vec[Bytes]) -> I64:\n  true\n'),
                    ('parallel',Path('benchmarks/challenges/state_machine/program.slim').read_text())]:
    directory=root/name;directory.mkdir()
    module=source.splitlines()[0].split()[1]
    (directory/'main.slim').write_text(source)
    (directory/'slim.project').write_text(f'(project 1 (entry {module}) (module {module} "main.slim" (imports) (exports)))\n')
PY
for variant in ordinary sanitized; do
    case "$variant" in ordinary) flags= ;; sanitized) flags='-g -fsanitize=address,undefined -fno-omit-frame-pointer' ;; esac
    "${CC:-cc}" -std=c11 -O1 $flags -Wall -Wextra -Werror -I runtime -I "$selection_dir" \
        tests/fixtures/native_build_selection.c runtime/slim_rt.c -o "$selection_dir/$variant"
    "$selection_dir/$variant" "$selection_dir/before/slim.project" "$selection_dir/after/slim.project" \
        "$selection_dir/invalid/slim.project" "$selection_dir/parallel/slim.project" > "$selection_dir/$variant.log"
    tail -n 1 "$selection_dir/$variant.log" | grep -qx 'native selection exact; no selection allocation'
    printf 'native-selection\t%s\texact\n' "$variant"
done
cmp "$selection_dir/ordinary.log" "$selection_dir/sanitized.log"
