#!/usr/bin/env python3
"""Early fixture admission using existing oracles and the production checker.

No native builds, performance samples, semantic oracle changes or result cache.
Data freezes check their existing pins/bounds; reference checking is not task
execution acceptance. All native and independent full campaigns remain required.
"""
import importlib.util
from contextlib import ExitStack
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def load(relative, name):
    spec = importlib.util.spec_from_file_location(name, ROOT / relative)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


def command(*args):
    subprocess.run([str(arg) for arg in args], cwd=ROOT, check=True, timeout=120)


def context_fixture(output, compiler, cc):
    """Use RFC170's existing capture, freezer and held-data validator."""
    verifier = load('scripts/verify-project-impact-context.py', 'fixture_context_verifier')
    paths = {'oracle': verifier.ORACLE_PATH, 'inherited_oracle': verifier.INHERITED_PATH,
             'verifier': verifier.VERIFIER_PATH, 'collector': verifier.COLLECTOR_PATH,
             'rfc': verifier.RFC_PATH}
    controls = {name: verifier.read(ROOT / path, verifier.MIB) for name, path in paths.items()}
    admission = {
        'sources': {name: verifier.file_identity(ROOT / name, verifier.MIB)
                    for name in verifier.SOURCE_PATHS},
        'controls': {name: verifier.identity(data) for name, data in controls.items()},
        'tools': {name: {'path_hex': os.fsencode(path).hex(), **verifier.file_identity(path)}
                  for name, path in [('compiler', compiler), ('cc', cc),
                                     ('python', Path(sys.executable).resolve())]},
    }
    oracle = verifier.captured_module(ROOT / verifier.ORACLE_PATH, controls['oracle'],
                                     admission['controls']['oracle']['sha256'],
                                     'fixture_context_oracle')
    held = oracle.freeze(output, compiler, cc, admission)
    verifier.held_data(output, {'freeze': verifier.identity(verifier.read(output / 'freeze.json', verifier.MIB)),
                                'model': held['model'], 'oracle': admission['controls']['oracle']})


def observer_anchors(source):
    manifest = load('scripts/verify-manifest-validation.py', 'fixture_manifest_observer')
    path_oracle, _ = manifest.load_path_model()
    manifest.instrument_current(source, manifest.load_oracle(), path_oracle)
    for name in ('project-lists', 'project-namespace'):
        load(f'scripts/verify-{name}.py', 'fixture_' + name.replace('-', '_')).instrument(source)
    print('fixtures: manifest/list/namespace instrumentation anchors checked', flush=True)


def references(output, compiler, cc):
    frozen = output / 'development'
    command(sys.executable, '-B', 'benchmarks/development/evaluate.py', 'freeze',
            '--compiler', compiler, '--cc', cc, '--destination', frozen)
    evaluator = load('benchmarks/development/evaluate.py', 'fixture_development_evaluator')
    tasks = json.loads((frozen / 'freeze.json').read_text())['manifest']['tasks']
    (output / 'references').mkdir()
    for task in tasks:
        project = output / 'references' / task['id']
        evaluator.install(evaluator.task_values(frozen, task, 'reference'), project)
        command(compiler, 'check', project / task['entry'])
    print(f'fixtures: {len(tasks)} frozen development reference projects checked', flush=True)


def main():
    if len(sys.argv) != 1:
        print('usage: verify-fixtures.py (run bootstrap first)', file=sys.stderr)
        return 64
    compiler = (ROOT / 'build/toolchain/slimc').resolve(strict=True)
    cc = Path('/usr/bin/cc').resolve(strict=True)
    observer_anchors((ROOT / 'build/toolchain/slimc.c').read_text())
    (ROOT / 'build').mkdir(exist_ok=True)
    with ExitStack() as cleanup:
        def workspace(parent):
            parent.mkdir(parents=True, exist_ok=True)
            return Path(cleanup.enter_context(tempfile.TemporaryDirectory(prefix='fixture-preflight-', dir=parent)))
        output = workspace(ROOT / 'build')
        impact = workspace(ROOT / 'build/overnight-project-impact') / 'frozen'
        ledger = workspace(ROOT / 'build/overnight-ledger-transaction') / 'frozen'
        plans = [
            ('manifest', ['scripts/verify-manifest-validation.py', '--prepare', '--output', output / 'manifest']),
            ('project impact', ['scripts/project-impact-oracle.py', '--freeze', impact]),
            ('ledger', ['scripts/ledger-transaction-oracle.py', 'freeze', '--output', ledger,
                        '--compiler', compiler, '--cc', cc]),
            ('source context', ['scripts/freeze-source-context.py', '--output', output / 'source-context.held',
                                '--compiler', compiler, '--cc', cc]),
        ]
        for name, args in plans:
            print(f'fixtures: admitting {name}', flush=True)
            command(sys.executable, '-B', *args)
        print('fixtures: admitting project impact context', flush=True)
        context_fixture(output / 'impact-context', compiler, cc)
        references(output, compiler, cc)
    print('fixture admission passed; native execution, performance and review remain required')
    return 0


if __name__ == '__main__':
    def deadline(_signum, _frame):
        raise TimeoutError('fixture preflight exceeded 300 seconds')
    signal.signal(signal.SIGALRM, deadline)
    signal.alarm(300)
    try:
        sys.exit(main())
    except (OSError, ValueError, subprocess.SubprocessError, TimeoutError) as error:
        print(f'fixture preflight failed: {error}', file=sys.stderr)
        sys.exit(1)
