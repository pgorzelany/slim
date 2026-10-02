#!/bin/sh
set -eu
query_compiler=${1:-build/toolchain/slimc}
query_scope=${2:-full}
case "$query_scope" in full|quick|faults) ;; *) exit 64 ;; esac
query_dir=$(mktemp -d "${TMPDIR:-/tmp}/slim-parallel-query.XXXXXX")
trap 'rm -rf "$query_dir"' EXIT HUP INT TERM
mkdir "$query_dir/probe"
cp selfhost/*.slim "$query_dir/probe/"
cp tests/fixtures/parallel_query.slim "$query_dir/probe/zzprobe.slim"
python3 -B scripts/prepare-probe-manifest.py selfhost/slim.project "$query_dir/probe/slim.project" \
  --export parallelcache "metadata_seal" <<'MANIFEST'
  (module zzprobe "zzprobe.slim" (imports identity parallel parallelcache project retained) (exports))
MANIFEST
if ! "$query_compiler" "$query_dir/probe/slim.project" > "$query_dir/probe.c"; then
    cat "$query_dir/probe.c" >&2
    exit 1
fi
python3 - "$query_dir" <<'PY'
from pathlib import Path
import re
import sys
root = Path(sys.argv[1])
# Verify that the field-by-field oracle covers the complete production records.
source = Path('selfhost/parallel.slim').read_text()
fixture = Path('tests/fixtures/parallel_query.slim').read_text()
for name in ['Blockers', 'FunctionFact', 'Site', 'Schedule']:
    fields = set(re.findall(r'^  (\w+):', re.search(r'struct '+name+r':\n((?:  .+\n)+)', source)[1], re.M))
    body = fixture.split('fn same_'+name+'(')[1].split('\nfn ')[0]
    assert fields == set(re.findall(r'left\.(\w+)', body)), name
fields = set(re.findall(r'^  (\w+):', re.search(r'struct View:\n((?:  .+\n)+)', source)[1], re.M))
body = fixture.split('fn same_view(')[1].split('\nfn ')[0]
assert fields == set(re.findall(r'left\.(\w+)', body)), 'View'
code = (root / 'probe.c').read_text()
# Fixed two-query fixture: counts cannot exceed three producer calls and one
# import. These independent native observations never infer reuse from a report.
for index, name in enumerate(['pparallel_95_95analyze', 'pparallelcache_95_95analyze', 'pparallelcache_95_95import_950view']):
    pattern = r'(?m)^(static [^\n]*\bslim_fn_'+name+r'\([^\n]*\) \{)$'
    code, count = re.subn(pattern, lambda m: m[0]+f'\n++parallel_query_counts[{index}];', code)
    assert count == 1, (name, count)
# Count real validation/import loop bodies, excluding terminal loop entries.
for index, name, collection, suffix in [
    (3, 'same_950inputs', 'count', ''),
    (4, 'same_950ranges', 'owner', '.slim_field_count'),
    (5, 'profile_950at', 'profiles', '.len'),
    (6, 'import_950facts', 'old', '.len'),
    (7, 'import_950sites', 'old', '.slim_field_sites.len'),
    (8, 'fact_950frames', 'view', '.slim_field_facts.len'),
    (9, 'site_950frames', 'view', '.slim_field_sites.len'),
    (10, 'select_950rows', 'declarations', '.len'),
]:
    pattern = r'(?m)^(static [^\n]*\bslim_fn_pparallelcache_95_95'+name+r'\([^\n]*\) \{)$'
    matches = list(re.finditer(pattern, code))
    assert len(matches) == 1, (name, len(matches))
    match = matches[0]
    at = re.search(r'\bslim_v_at_n[0-9]+\b', match[0])[0]
    limit = re.search(r'\bslim_v_'+collection+r'_n[0-9]+\b', match[0])[0] + suffix
    end = code.index('\nstatic ', match.end())
    body = code[match.start():end]
    assert body.count('slim_recur: ;') == 1, name
    body = body.replace('slim_recur: ;', f'slim_recur: ;\nif ({at} < {limit}) ++parallel_query_counts[{index}];')
    code = code[:match.start()] + body + code[end:]
code, count = re.subn(r'(?m)^(int main\([^\n]*\) \{)$', lambda m: m[0]+'\nif (atexit(parallel_query_report) != 0) abort();', code)
assert count == 1
prefix = '#include <stdio.h>\n#include <stdlib.h>\nstatic unsigned parallel_query_counts[11];\nstatic void parallel_query_report(void) { fprintf(stderr, "query-observed\\t%u\\t%u\\t%u\\t%u\\t%u\\t%u\\t%u\\t%u\\t%u\\t%u\\t%u\\n", parallel_query_counts[0], parallel_query_counts[1], parallel_query_counts[2], parallel_query_counts[3], parallel_query_counts[4], parallel_query_counts[5], parallel_query_counts[6], parallel_query_counts[7], parallel_query_counts[8], parallel_query_counts[9], parallel_query_counts[10]); }\n'
(root / 'observed.c').write_text(prefix + code)
PY
"${CC:-cc}" -std=c11 -O1 -Wall -Wextra -Werror -I runtime \
    "$query_dir/probe.c" runtime/slim_rt.c -o "$query_dir/ordinary"
"${CC:-cc}" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Wall -Wextra -Werror -I runtime "$query_dir/observed.c" runtime/slim_rt.c -o "$query_dir/observed"
python3 -B scripts/verify-parallel-query.py "$query_dir" "$query_scope"
