"""Exact complete-view differential and independent production query observations."""
from pathlib import Path
import os
import re
import subprocess
import sys

from session_host_cases import edit_cases

root = Path(sys.argv[1])
full = sys.argv[2] == 'full'
counts = {'hit': 0, 'miss': 0}
print('parallel-query-columns\tscenario\tdecision\tproducer\tqueries\timports\tkey_functions\tkey_nodes\tprofile_rows\timported_facts\timported_sites\tvalidated_facts\tvalidated_sites\tselection_declarations', flush=True)


def write_project(name, files):
    directory = root / name
    directory.mkdir()
    for path, source in files.items():
        (directory / path).write_text(source)
    return directory / 'slim.project'


def source_files(source):
    module = re.search(r'(?m)^module (\w+)$', source)[1]
    return {'slim.project': f'(project 1 (entry {module}) (module {module} "program.slim" (imports) (exports)))\n', 'program.slim': source}


def check(label, before, after, reuse=None, damage=None, reseal=False):
    paths = [write_project(label+'-before', before), write_project(label+'-after', after)]
    for binary in ['ordinary', 'observed']:
        arguments = [str(root / binary), *map(str, paths)] + ([damage] if damage else []) + (['reseal'] if reseal else [])
        result = subprocess.run(arguments, capture_output=True, timeout=120)
        assert result.returncode == 0 and result.stdout == b'ok complete parallel query\n', (label, binary, result.returncode, result.stdout, result.stderr)
        if binary == 'ordinary':
            assert result.stderr == b'', (label, result.stderr)
        else:
            fields = result.stderr.decode().strip().split('\t')
            assert fields[0] == 'query-observed' and len(fields) == 12, (label, fields)
            observed = tuple(map(int, fields[1:]))
            actual = observed[:3]
            assert all(0 <= value <= 2000000 for value in observed), (label, observed)
            assert actual in [(2, 2, 1), (3, 2, 0)], (label, actual)
            hit = actual == (2, 2, 1)
            assert hit == (observed[6] > 0), (label, observed)
            if not hit:
                assert observed[7] == 0, (label, observed)
            if reuse is not None:
                assert hit == reuse, (label, actual, reuse)
            counts['hit' if hit else 'miss'] += 1
            print('parallel-query', label, 'hit' if hit else 'miss', *observed, sep='\t', flush=True)
    return observed


basic = 'module app\n\nfn helper() -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  helper()\n'
check('same', source_files(basic), source_files(basic), True)
check('body', source_files(basic), source_files(basic.replace('  0', '  1')), False)
check('span', source_files(basic), source_files('# shifted source\n'+basic), True)
for size in [63, 64, 65]:
    source = 'module app\n\n'+''.join(f'fn helper_{i}() -> I64:\n  {i}\n\n' for i in range(size-1))+'fn main(args: Vec[Bytes]) -> I64:\n  0\n'
    check(f'bound-{size}-same', source_files(source), source_files(source), True)
    check(f'bound-{size}-last', source_files(source), source_files(source[:-4]+'  1\n'), size > 64)
    appended = source+'\nfn appended() -> I64:\n  0\n'
    check(f'bound-{size}-append', source_files(source), source_files(appended), size >= 64)
    check(f'bound-{size}-remove', source_files(appended), source_files(source), size >= 64)
# Repeated safe calls create both facts and automatic sites for complete damage.
parallel_source = 'module app\n\nfn count(index: I64) -> I64 effects[partial]:\n  if index <= 0:\n    0\n  else:\n    recur(index - 1)\n\nfn main(args: Vec[Bytes]) -> I64 effects[partial]:\n  let first: I64 = count(2000000)\n  let second: I64 = count(2000000)\n  first + second\n'
check('automatic-same', source_files(parallel_source), source_files(parallel_source), True)
for key in ['epoch', 'serial', 'seal', 'functions', 'exact', 'views-remove', 'facts-remove', 'sites-remove']:
    check('damage-'+key, source_files(parallel_source), source_files(parallel_source), False, key)
parallel_records = Path('selfhost/parallel.slim').read_text()
for typ, prefix in [('Blockers', 'blocker'), ('FunctionFact', 'fact'), ('Site', 'site'), ('Schedule', 'schedule')]:
    for field in re.findall(r'^  (\w+):', re.search(r'struct '+typ+r':\n((?:  .+\n)+)', parallel_records)[1], re.M):
        if field == 'blockers':
            continue
        key = prefix+'-'+field
        check('damage-'+key, source_files(parallel_source), source_files(parallel_source), False, key)
for key in ['epoch', 'serial', 'functions', 'facts-remove', 'sites-remove', 'fact-item', 'fact-name', 'fact-status', 'fact-reason', 'fact-first_edge', 'fact-edge_count', 'fact-body', 'fact-body_tokens', 'site-site', 'site-first', 'site-second', 'site-join', 'schedule-selected_until', 'schedule-candidates', 'schedule-selected', 'schedule-reported', 'schedule-executable', 'schedule-executed']:
    check('resealed-'+key, source_files(parallel_source), source_files(parallel_source), False, key, True)
if full:
    for size in [125, 250, 500, 1000, 2000, 4000]:
        source = 'module app\n\n'+''.join(f'fn helper_{i}() -> I64:\n  {i}\n\n' for i in range(size))+'fn main(args: Vec[Bytes]) -> I64:\n  0\n'
        check(f'geometry-{size}-inside', source_files(source), source_files(source.replace('fn helper_0() -> I64:\n  0', 'fn helper_0() -> I64:\n  1')), False)
        outside = check(f'geometry-{size}-outside', source_files(source), source_files(source[:-4]+'  1\n'), True)
        assert outside[3:] == (64, 704, 0, 64, 0, 64, 0, 2 * (size + 1)), (size, outside)
    padding = ''.join(f'fn idle_{i}() -> I64:\n  {i}\n\n' for i in range(63))
    caller = 'module app\n\nfn increment(value: I64) -> I64:\n  value + 1\n\n'+padding+'fn main(args: Vec[Bytes]) -> I64:\n  increment(1)\n'
    check('outside-caller-input', source_files(caller), source_files(caller.replace('  increment(1)', '  increment(2)')), False)
    for sites in [63, 64, 65]:
        header = 'module app\n\nfn count(index: I64) -> I64 effects[partial]:\n  if index <= 0:\n    0\n  else:\n    recur(index - 1)\n\n'
        chain = 'fn many() -> I64 effects[partial]:\n'+''.join(f'  let first{i}: I64 = count(1000000)\n  let second{i}: I64 = count(1000000)\n' for i in range(sites))+f'  first{sites-1} + second{sites-1}\n\n'
        source = header+chain+'fn main(args: Vec[Bytes]) -> I64:\n  0\n'
        shifted = source.replace('fn count', 'struct Shifted:\n  value: I64\n\nfn count')
        check(f'sites-{sites}-shift', source_files(source), source_files(shifted), True)
        check(f'sites-{sites}-threshold', source_files(source), source_files(source.replace('count(1000000)', 'count(999999)')), False)
    for edges in [4095, 4096, 4097]:
        functions = []
        for index in range(62):
            count = 66 if index < 61 else edges-61*66
            functions.append(f'fn worker_{index}() -> I64:\n'+''.join(f'  let v{i}: I64 = leaf(0)\n' for i in range(count))+'  0\n\n')
        source = 'module app\n\nfn leaf(value: I64) -> I64:\n  value\n\n'+''.join(functions)+'fn main(args: Vec[Bytes]) -> I64:\n  0\n'
        check(f'edges-{edges}-same', source_files(source), source_files(source), True)
        check(f'edges-{edges}-append', source_files(source), source_files(source+'\nfn outside() -> I64:\n  0\n'), True)
    for label, before, after, accepted in edit_cases():
        if accepted:
            check('edit-'+label, before, after)
    paths = sorted(Path('conformance/pass').glob('*.slim'))
    paths += sorted(Path('benchmarks/challenges').glob('*/program.slim'))
    paths.append(Path('tests/fixtures/retained_inputs_calls.slim'))
    for at, path in enumerate(paths):
        source = path.read_text()
        check(f'corpus-{at}', source_files(source), source_files(source), True)
        check(f'corpus-{at}-layout-shift', source_files(source), source_files(source.replace('\n\n', '\n\nstruct Shifted:\n  field: I64\n\n', 1)))
assert counts['hit'] > 0 and counts['miss'] > 0
print('parallel-query-exact', counts, sep='\t', flush=True)

if sys.argv[2] in ['full', 'faults']:
    paths = [write_project('fault-before', source_files(basic)), write_project('fault-after', source_files(basic))]
    for binary in ['ordinary', 'observed']:
        failed = complete = imported_failures = 0
        for ordinal in range(1, 2049):
            result = subprocess.run([str(root / binary), *map(str, paths)], capture_output=True, timeout=30,
                                    env={**os.environ, 'SLIM_ALLOC_FAIL_AT': str(ordinal)})
            error = result.stderr.decode()
            if binary == 'observed':
                lines = error.splitlines()
                trace = lines.pop()
                assert trace.startswith('query-observed\t'), (binary, ordinal, error)
                counters = list(map(int, trace.split('\t')[1:]))
                assert len(counters) == 11 and all(value >= 0 for value in counters)
                error = '\n'.join(lines) + ('\n' if lines else '')
            if result.returncode == 71:
                assert complete == 0, ('fault domain is not a deterministic allocation prefix', binary, ordinal)
                assert error == f'SLIM allocation failure: exhausted at allocation {ordinal}\n', (binary, ordinal, error)
                assert result.stdout == b'', (binary, ordinal, result.stdout)
                failed += 1
                if binary == 'observed' and counters[2] > 0:
                    imported_failures += 1
            else:
                assert result.returncode == 0 and result.stdout == b'ok complete parallel query\n' and not error, (binary, ordinal, result.returncode, result.stdout, result.stderr)
                if binary == 'observed':
                    assert counters[:3] == [2, 2, 1], counters
                complete += 1
                # With deterministic source and allocation order, every later
                # ordinal follows this complete no-failure path. Cross the fixed
                # cap as a second control instead of repeating the same path.
                endpoint = subprocess.run([str(root / binary), *map(str, paths)], capture_output=True, timeout=30,
                                          env={**os.environ, 'SLIM_ALLOC_FAIL_AT': '2048'})
                assert (endpoint.returncode, endpoint.stdout, endpoint.stderr) == (result.returncode, result.stdout, result.stderr), (binary, ordinal, endpoint)
                complete += 1
                break
        assert failed > 0 and complete > 0, (binary, failed, complete)
        if binary == 'observed':
            assert imported_failures > 0, 'allocation faults did not cross result import'
        print('parallel-query-faults', binary, 'cap', 2048, 'trials', failed+complete, 'failed', failed, 'complete', complete, 'failed-during-import', imported_failures if binary == 'observed' else 'unobserved', sep='\t', flush=True)
