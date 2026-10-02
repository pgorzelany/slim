"""Observe executed native-query work in a verification-only generated C copy."""
from pathlib import Path
import re
import sys

code = Path(sys.argv[1]).read_text()
# These counters observe executed functions and loop bodies, not reported hits.
for index, name in enumerate(['lookup', 'publish', 'entry_950frame']):
    pattern = r'(?m)^(static [^\n]*\bslim_fn_pnativecache_95_95' + name + r'\([^\n]*\) \{)$'
    code, count = re.subn(pattern, lambda m: m[0] + f'\n++native_query_counts[{index}];', code)
    assert count == 1, (name, count)
for index, name, at_name, limit_name, suffix in [
    (3, 'pnativecache_95_95clone_950rows', 'at', 'source', '.len'),
    (4, 'pcache_95_95weighted_950checksum', 'index', 'end', ''),
    (5, 'pretained_95_95equal_950from', 'at', 'left', '.len'),
]:
    pattern = r'(?m)^static [^\n]*\bslim_fn_' + name + r'\([^\n]*\) \{$'
    matches = list(re.finditer(pattern, code))
    assert len(matches) == 1, (name, len(matches))
    match = matches[0]
    at = re.search(r'\bslim_v_' + at_name + r'_n[0-9]+\b', match[0])[0]
    limit = re.search(r'\bslim_v_' + limit_name + r'_n[0-9]+\b', match[0])[0] + suffix
    end = code.index('\nstatic ', match.end())
    body = code[match.start():end]
    assert body.count('slim_recur: ;') == 1, name
    body = body.replace('slim_recur: ;', f'slim_recur: ;\nif ({at} < {limit}) ++native_query_counts[{index}];')
    code = code[:match.start()] + body + code[end:]
prefix = '#include <stdint.h>\nstatic uint64_t native_query_counts[6];\n'
Path(sys.argv[2]).write_text(prefix + code)
