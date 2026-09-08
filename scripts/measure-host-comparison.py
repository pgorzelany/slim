"""Interleaved public-session comparison with two identical baseline controls."""
import argparse
import hashlib
import itertools
from pathlib import Path
import runpy
import statistics
import subprocess
import tempfile
import time

api = runpy.run_path(str(Path(__file__).with_name('verify-session-host.py')))
Client, project, compare = (api[name] for name in ['Client', 'project', 'compare'])
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('baseline_host')
parser.add_argument('candidate_host')
parser.add_argument('--shape', choices=['scalar', 'graph'], default='scalar')
parser.add_argument('--baseline-compiler', required=True)
parser.add_argument('--candidate-compiler', required=True)
options = parser.parse_args()
engines = {'a': options.baseline_host, 'b': options.baseline_host, 'c': options.candidate_host}
identities = {name: hashlib.sha256(Path(path).read_bytes()).hexdigest() for name, path in engines.items()}
print('# schema=1; 18 groups; two warmups; six rotating orders; a/b identical baseline; c candidate', flush=True)
print('# shape', options.shape, 'native_host_sha256', identities, flush=True)
print('size\tscenario\tgroup\torder\ta_ns\tb_ns\tc_ns', flush=True)
orders = list(itertools.permutations('abc'))
with tempfile.TemporaryDirectory(prefix='slim-session-compare-') as temporary:
    root = Path(temporary)
    for size in [125, 250, 500, 1000, 2000, 4000]:
        if options.shape == 'scalar':
            header = 'module app\n\n'
            functions = [f'fn value_{i}() -> I64:\n  {i}\n\n' for i in range(size)]
            changed = functions[0].replace('  0\n', '  1\n')
        else:
            header = 'module app\n\nfn leaf(value: I64) -> I64:\n  value\n\n'
            functions = [f'fn value_{i}() -> I64:\n'+(''.join(f'  let v{j}: I64 = leaf({j})\n' for j in range(16))+'  v0\n\n' if i < 63 else f'  {i}\n\n') for i in range(size)]
            changed = functions[0].replace('  v0\n', '  v0 + 1\n')
        main = 'fn main(args: Vec[Bytes]) -> I64:\n  value_0()\n'
        source = header+''.join(functions)+main
        inside = header+changed+''.join(functions[1:])+main
        contents = [source, inside, source.replace('  value_0()', '  value_0() + 1')]
        print('# sources_sha256', size, [hashlib.sha256(text.encode()).hexdigest() for text in contents], flush=True)
        paths = [project(root/f'{size}-{index}', text) for index, text in enumerate(contents)]
        expected = []
        for path in paths:
            old = subprocess.run([options.baseline_compiler, str(path)], capture_output=True, check=True, timeout=120)
            new = subprocess.run([options.candidate_compiler, str(path)], capture_output=True, check=True, timeout=120)
            assert old.stdout == new.stdout and old.stderr == new.stderr == b''
            expected.append((0, new.stdout))
        clients = {key: Client([path]) for key, path in engines.items()}
        series = {kind: [] for kind in ['cold', 'unchanged', 'inside', 'outside']}
        for group in range(-2, 18):
            order = orders[group % 6]
            timings = {}
            for key in order:
                client = clients[key]
                client.reset()
                for kind, variant in [('cold', 0), ('unchanged', 0), ('inside', 1), ('restore', 0), ('outside', 2)]:
                    start = time.perf_counter_ns()
                    actual = client.update(paths[variant])
                    elapsed = time.perf_counter_ns()-start
                    compare(actual, expected[variant])
                    timings[(key, kind)] = elapsed
            for kind in series:
                values = [timings[(key, kind)] for key in 'abc']
                print(size, kind, group, ''.join(order), *values, sep='\t', flush=True)
                if group >= 0:
                    series[kind].append(values)
        for client in clients.values():
            client.quit()
        for kind, rows in series.items():
            controls = [b/a for a, b, c in rows]
            ratios = [c/((a+b)/2) for a, b, c in rows]
            medians = [statistics.median(row[index] for row in rows) for index in range(3)]
            quartiles = statistics.quantiles(controls, n=4)
            print('# comparison', size, kind, 'median_ns', *medians, 'candidate_ratio', statistics.median(ratios), 'control_iqr', quartiles[0], quartiles[2], flush=True)
for name, path in engines.items():
    assert hashlib.sha256(Path(path).read_bytes()).hexdigest() == identities[name]
