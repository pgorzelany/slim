#!/usr/bin/env python3
"""Convert validated protocol-2 observations to RFC-0164 framed data.

The fixed repository evaluator validates source/ledger/result identities. This
adapter never evaluates candidates, invokes a compiler, or executes data. Its
receipt is separate from the statistical consumer's supplied identity fields.
Stop writers or retry explicitly if the checked lifecycle changes during read.
"""
import argparse
import contextlib
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import tempfile
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
EVALUATOR = ROOT / 'benchmarks/development/evaluate.py'
FORMAT = b'slim-development-summary-1'
MAX_INPUT = 131072
OUTCOMES = {'accepted', 'correctness-failure', 'infrastructure-failure', 'interrupted',
            'elapsed-timeout', 'constraint-failure', 'unresolved', 'not-run'}


def evaluator():
    # This path is fixed trusted repository code, never a caller-selected module.
    spec = importlib.util.spec_from_file_location('slim_development_evaluator', EVALUATOR)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def pinned_bytes(path, maximum):
    path = Path(path)
    if path.is_symlink() or not path.is_file():
        raise ValueError('ordinary pinned identity file required')
    with path.open('rb') as stream:
        value = stream.read(maximum + 1)
    if len(value) > maximum:
        raise ValueError('pinned identity file ceiling')
    return value


def pins(captured):
    paths = [('freeze_sha256', captured / 'freeze.json', 1048576),
             ('manifest_sha256', captured / 'corpus/manifest.json', 1048576),
             ('evaluator_sha256', EVALUATOR, 4194304),
             ('adapter_sha256', Path(__file__), 4194304)]
    return {name: hashlib.sha256(pinned_bytes(path, maximum)).hexdigest()
            for name, path, maximum in paths}


def frame(value):
    return str(len(value)).encode('ascii') + b':' + value + b','


def encoded_number(value):
    if value is None:
        return b''
    if type(value) is not int or not 0 <= value <= 2 ** 63 - 1:
        raise ValueError('unsupported numeric observation')
    return str(value).encode('ascii')


def observations(module, captured, runs, trials, tasks):
    records = {}
    for trial in trials:
        path = runs / trial['id']
        if not (path / 'ledger.jsonl').exists():
            records[trial['id']] = None
            continue
        raw = module.read(path / 'ledger.jsonl', module.MAX_LEDGER)
        state = module.ledger(path)
        module.run_metadata(path)
        item = {'ledger_sha256': module.digest(raw), 'record_count': len(state['records']),
                'last_record_sha256': state['records'][-1]['sha256'], 'result_sha256': None,
                'prepared_sha256': module.digest(module.read(path / 'prepared.json')),
                'submitted_source_sha256': None}
        if state['evaluation'] is not None:
            item['result_sha256'] = module.digest(module.read(path / 'result.json'))
            module.require(item['result_sha256'] == state['evaluation']['data']['result_sha256'],
                           'result identity changed')
        if state['submission'] is not None:
            identity = state['submission']['data']['source_identity']
            if identity is not None:
                item['submitted_source_sha256'] = module.digest(module.encoded(identity))
                if state['submission']['data']['capture_error'] is None:
                    values = module.capture(path / 'submitted', tasks[trial['task']]['files'])
                    module.require(module.identity(values) == identity, 'submitted source changed')
        records[trial['id']] = item
    return records


def convert(captured, runs):
    captured, runs = Path(captured).resolve(), Path(runs).resolve()
    initial_pins = pins(captured)
    header = json.loads(pinned_bytes(captured / 'freeze.json', 1048576))
    if initial_pins['evaluator_sha256'] != header['corpus']['files']['evaluate.py']:
        raise ValueError('frozen evaluator identity differs before import')
    module = evaluator()
    if pins(captured) != initial_pins:
        raise ValueError('identity changed during evaluator import')
    metadata = module.frozen(captured)
    if pins(captured) != initial_pins:
        raise ValueError('identity changed during frozen validation')
    manifest = metadata['manifest']
    if len(manifest['tasks']) > 32 or len(manifest['trials']) > 64:
        raise ValueError('summary task/trial ceiling')
    tasks = {task['id']: task for task in manifest['tasks']}
    before = observations(module, captured, runs, manifest['trials'], tasks)
    with tempfile.TemporaryDirectory(prefix='slim-development-summary-') as temporary:
        output = Path(temporary) / 'validated-summary.json'
        with contextlib.redirect_stdout(io.StringIO()):
            module.summarize(SimpleNamespace(freeze=captured, runs=runs, output=output))
        summary = module.document(output)
    if summary['freeze_sha256'] != initial_pins['freeze_sha256']:
        raise ValueError('summary belongs to different pinned freeze')
    after = observations(module, captured, runs, manifest['trials'], tasks)
    module.frozen(captured)
    if before != after:
        raise ValueError('lifecycle changed during summary conversion')
    if pins(captured) != initial_pins:
        raise ValueError('identity changed during lifecycle observation')
    manifest_sha = initial_pins['manifest_sha256']
    evaluator_sha = initial_pins['evaluator_sha256']
    identity = b''.join(frame(value.encode('ascii')) for value in
                        (metadata['compiler_sha256'], evaluator_sha, manifest_sha))
    framed = frame(FORMAT) + frame(summary['freeze_sha256'].encode('ascii')) + frame(identity)
    grouped = {task['id']: {} for task in manifest['tasks']}
    for row in summary['trials']:
        if row['outcome'] not in OUTCOMES:
            raise ValueError('unsupported outcome')
        if row['condition'] in grouped[row['task']]:
            raise ValueError('duplicate configured condition')
        oracle = row['oracle_accepted']
        if oracle is not None and type(oracle) is not bool:
            raise ValueError('unsupported oracle evidence')
        elapsed = encoded_number(row.get('elapsed_ns'))
        receipt = before[row['id']]
        submitted = receipt['submitted_source_sha256'] if receipt is not None else None
        fields = [row['id'].encode('ascii'), row['outcome'].encode('ascii'),
                  b'unknown' if oracle is None else b'true' if oracle else b'false', elapsed,
                  b'same-host-monotonic-conditional' if elapsed else b'unknown',
                  encoded_number(row.get('operations')), encoded_number(row.get('requests')),
                  row.get('observation', '').encode('ascii'),
                  b'' if submitted is None else submitted.encode('ascii')]
        trial = b''.join(frame(value) for value in fields)
        if not fields[0] or len(fields[0]) > 128 or len(trial) > 1024:
            raise ValueError('framed trial ceiling')
        if any(len(value) > 128 for value in fields):
            raise ValueError('framed field ceiling')
        grouped[row['task']][row['condition']] = trial
    for task in manifest['tasks']:
        key = task['id'].encode('ascii')
        if not 0 < len(key) <= 64 or set(grouped[task['id']]) != {'baseline', 'context'}:
            raise ValueError('framed task/condition contract')
        framed += frame(key) + frame(grouped[task['id']]['baseline']) + frame(grouped[task['id']]['context'])
    if len(framed) > MAX_INPUT:
        raise ValueError('framed input ceiling')
    receipt = {'schema': 1, 'adapter_sha256': initial_pins['adapter_sha256'],
               'evaluator_sha256': evaluator_sha, 'freeze_sha256': summary['freeze_sha256'],
               'compiler_sha256': metadata['compiler_sha256'], 'manifest_sha256': manifest_sha,
               'input_sha256': module.digest(framed), 'input_bytes': len(framed),
               'tasks': len(manifest['tasks']), 'trials': len(summary['trials']),
               'lifecycle_observations': before,
               'evidence': 'fixed evaluator validation, lifecycle stable over this read; no acceptance rerun',
               'source_identity_encoding': 'SHA256 of evaluator canonical JSON submitted source identity',
               'unknown': ['model tokens/calls/active time', 'native application performance', 'kernel boot continuity']}
    if pins(captured) != initial_pins:
        raise ValueError('identity changed during framed receipt assembly')
    return framed, receipt


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--freeze', required=True, type=Path)
    parser.add_argument('--runs', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--receipt', required=True, type=Path)
    args = parser.parse_args()
    if args.output.exists() or args.receipt.exists() or args.output.resolve() == args.receipt.resolve():
        parser.error('fresh distinct output and receipt paths required')
    try:
        framed, receipt = convert(args.freeze, args.runs)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.receipt.parent.mkdir(parents=True, exist_ok=True)
        with args.output.open('xb') as stream:
            stream.write(framed)
        with args.receipt.open('x', encoding='utf-8') as stream:
            stream.write(json.dumps(receipt, indent=2, sort_keys=True) + '\n')
    except (OSError, ValueError, KeyError, TypeError) as error:
        parser.exit(1, type(error).__name__ + ': ' + str(error) + '\n')
    print(json.dumps({'input': str(args.output), 'receipt': str(args.receipt),
                      'tasks': receipt['tasks'], 'trials': receipt['trials']}))


if __name__ == '__main__':
    main()
