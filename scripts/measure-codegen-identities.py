#!/usr/bin/env python3
"""Same-host C identifier costs; run after other validation jobs finish.

Supply O2 compilers built from the recorded baseline and candidate C. Measurements
separate the complete frontend, external C backend and native execution. These
process timings are evidence, not portable regression budgets.
"""
import argparse
import datetime
import hashlib
import os
from pathlib import Path
import platform
import statistics
import subprocess
import time


def timed(argv):
    start = time.perf_counter_ns()
    result = subprocess.run(list(map(str, argv)), capture_output=True)
    elapsed = time.perf_counter_ns() - start
    assert result.returncode == 0 and not result.stderr, (argv, result)
    return elapsed, result.stdout


def pairs(left, right, count=11, warm=2):
    for _ in range(warm):
        timed(left)
        timed(right)
    rows = []
    for pair in range(count):
        if pair % 2 == 0:
            before, before_output = timed(left)
            after, after_output = timed(right)
        else:
            after, after_output = timed(right)
            before, before_output = timed(left)
        rows.append([pair, before, after, after / before,
                     len(before_output), len(after_output)])
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('baseline')
    parser.add_argument('candidate')
    parser.add_argument('--baseline-revision', required=True)
    parser.add_argument('--candidate-c', required=True)
    parser.add_argument('--work', required=True)
    parser.add_argument('--results', default='benchmarks/results')
    args = parser.parse_args()
    work, results = Path(args.work), Path(args.results)
    work.mkdir(parents=True, exist_ok=True)
    results.mkdir(parents=True, exist_ok=True)
    old, new = Path(args.baseline).resolve(), Path(args.candidate).resolve()
    digest = hashlib.sha256(Path(args.candidate_c).read_bytes()).hexdigest()
    date = datetime.date.today().isoformat()
    posix = platform.system() in ('Darwin', 'Linux', 'FreeBSD', 'NetBSD', 'OpenBSD')
    metadata = (f'# date={date}; baseline={args.baseline_revision}; '
                f'candidate_seed_sha256={digest}; same-host O2; process totals '
                f'include startup and I/O; POSIX worker backend={posix}; '
                'no portable timing claim\n')

    def save(kind, protocol, columns, rows):
        path = results / f'{date}-m1-stable-c-{kind}.tsv'
        path.write_text(metadata + f'# {protocol}\n' + columns + '\n' +
                        ''.join('\t'.join(map(str, row)) + '\n' for row in rows))

    frontend = []
    for size in (125, 250, 500, 1000, 2000):
        path = work / f'geometry-{size}.slim'
        path.write_text('module geometry\n\n' + ''.join(
            f'fn helper_{i}(value: I64) -> I64 effects[partial]:\n'
            f'  let doubled: I64 = value + value\n  doubled + {i}\n\n'
            for i in range(size)) + 'fn main(args: Vec[Bytes]) -> I64:\n  0\n')
        rows = pairs([old, path], [new, path])
        frontend.extend([[size, *row] for row in rows])
        print('frontend', size, statistics.median(row[1] for row in rows) / 1e6,
              statistics.median(row[2] for row in rows) / 1e6, flush=True)
    save('frontend', 'two warmups, eleven alternating pairs; complete frontend including deterministic C emission',
         'helpers\tpair\tbaseline_ns\tcandidate_ns\tratio\tbaseline_C_bytes\tcandidate_C_bytes', frontend)

    backend, runtime, identities = [], [], []
    input_file = work / 'input.bin'
    input_file.write_bytes(bytes(range(256)) * 256)
    for path in sorted(Path('benchmarks/challenges').glob('*/program.slim')):
        name = path.parent.name
        files, binaries, commands = [], [], []
        for label, compiler in (('before', old), ('after', new)):
            _, code = timed([compiler, path])
            c_file = work / f'{name}-{label}.c'
            c_file.write_bytes(code)
            binary = work / f'{name}-{label}'
            files.append(c_file)
            binaries.append(binary)
            flags = []
            if b'#define SLIM_PARALLEL 1' in code:
                flags = ['-DSLIM_PARALLEL=1']
                if posix:
                    flags += ['-DSLIM_POSIX_WORKERS=1', '-pthread']
            commands.append([os.environ.get('CC', 'cc'), '-std=c11', '-O2', '-DNDEBUG',
                             '-Wall', '-Wextra', '-Werror', *flags, '-I', 'runtime',
                             c_file, 'runtime/slim_rt.c', '-o', binary])
        rows = pairs(*commands, count=5, warm=1)
        sizes = [binary.stat().st_size for binary in binaries]
        backend.extend([[name, *row[:4], *sizes] for row in rows])
        arguments = [input_file] if name == 'bytefreq' else []
        before = timed([binaries[0], *arguments])[1]
        after = timed([binaries[1], *arguments])[1]
        assert before == after, (name, 'native output changed')
        rows = pairs([binaries[0], *arguments], [binaries[1], *arguments])
        runtime.extend([[name, *row[:4]] for row in rows])
        identities.append([name, *[len(file.read_bytes()) for file in files],
                           *sizes, hashlib.sha256(before).hexdigest(), 'exact'])
        print('native', name, 'runtime_ms',
              round(statistics.median(row[1] for row in rows) / 1e6, 3),
              round(statistics.median(row[2] for row in rows) / 1e6, 3), flush=True)
        # Publish complete rows after each application so interrupted measurements
        # retain evidence and are visibly shorter than the 20-application domain.
        save('backend', 'one warmup, five alternating pairs; external C compilation and link',
             'application\tpair\tbaseline_ns\tcandidate_ns\tratio\tbaseline_binary_bytes\tcandidate_binary_bytes', backend)
        save('runtime', 'two warmups, eleven alternating pairs; exact output agreement before sampling; default input except bytefreq=65536 bytes 0..255 repeated',
             'application\tpair\tbaseline_ns\tcandidate_ns\tratio', runtime)
        save('native', 'Each changed C row separately verified as injective private identifier renaming with exact complete analysis in identities.tsv',
             'application\tbaseline_C_bytes\tcandidate_C_bytes\tbaseline_binary_bytes\tcandidate_binary_bytes\tstdout_sha256\tstatus_stdout_stderr', identities)


if __name__ == '__main__':
    main()
