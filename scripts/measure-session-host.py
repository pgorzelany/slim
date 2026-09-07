"""Quiet same-host public-session timings; raw samples, no inferred work counts.

Cold/reset/update responses and one-shot output are checked byte-for-byte.
This measures frontend transport, not external C compilation or agent success.
"""
import argparse
import hashlib
from pathlib import Path
import runpy
import statistics
import subprocess
import tempfile
import time

api = runpy.run_path(str(Path(__file__).with_name('verify-session-host.py')))
Client, project, compare = (api[name] for name in ['Client', 'project', 'compare'])
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--host')
parser.add_argument('--compiler', default='build/toolchain/slimc')
parser.add_argument('--samples', type=int, default=9)
options = parser.parse_args()
assert 3 <= options.samples <= 31
command = [options.host] if options.host else ['./slimc', 'session']
print('scenario\tdeclarations\tsample\tnanoseconds\tc_bytes', flush=True)
with tempfile.TemporaryDirectory(prefix='slim-host-timing-') as temporary:
    directory = Path(temporary)
    for size in [125, 250, 500, 1000, 2000, 4000]:
        source = 'module app\n\n' + ''.join(f'fn value_{i}() -> I64:\n  {i}\n\n' for i in range(size))
        source += 'fn main(args: Vec[Bytes]) -> I64:\n  value_0()\n'
        before = project(directory / f'before-{size}', source)
        after = project(directory / f'after-{size}', source.replace('fn value_0() -> I64:\n  0', 'fn value_0() -> I64:\n  1'))
        expectations = {}
        for path in [before, after]:
            result = subprocess.run([options.compiler, str(path)], capture_output=True, timeout=120, check=True)
            assert result.stderr == b''
            expectations[path] = (0, result.stdout)
        series = {name: [] for name in ['start', 'reset', 'cold', 'unchanged', 'body', 'one-shot']}
        for sample in range(options.samples + 1):
            start = time.perf_counter_ns()
            client = Client(command)
            started = time.perf_counter_ns() - start
            if sample == 0:
                print('# identity', size, client.identity, 'source_sha256', hashlib.sha256(source.encode()).hexdigest(), flush=True)
            # Alternate the one-shot operation around the retained sequence.
            def one_shot():
                start = time.perf_counter_ns()
                result = subprocess.run([options.compiler, str(after)], capture_output=True, timeout=120, check=True)
                elapsed = time.perf_counter_ns() - start
                assert result.stderr == b'' and result.stdout == expectations[after][1]
                return elapsed
            if sample % 2 == 0:
                single = one_shot()
            elapsed = {'start': started}
            start = time.perf_counter_ns()
            client.reset()
            elapsed['reset'] = time.perf_counter_ns() - start
            for label, path in [('cold', before), ('unchanged', before), ('body', after)]:
                start = time.perf_counter_ns()
                result = client.update(path)
                elapsed[label] = time.perf_counter_ns() - start
                compare(result, expectations[path])
            client.quit()
            if sample % 2:
                single = one_shot()
            elapsed['one-shot'] = single
            if sample:
                for label, value in elapsed.items():
                    series[label].append(value)
                    print(label, size, sample, value, len(expectations[after][1]), sep='\t', flush=True)
        median = {label: statistics.median(values) for label, values in series.items()}
        print('# median_ns', size, median, 'unchanged/cold', median['unchanged'] / median['cold'],
              'body/one-shot', median['body'] / median['one-shot'], flush=True)
