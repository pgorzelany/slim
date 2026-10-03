#!/usr/bin/env python3
"""Convert checked stopped-writer protocol-2 observations to RFC0166 data.

Only fixed repository validation modules are imported. No recorded command,
compiler, oracle, candidate or input-selected module is executed. Source bytes
are observed identities, not loaded-bytecode, ABA or atomic-snapshot proof.
"""
import argparse
import contextlib
from datetime import datetime
import hashlib
import importlib.util
import io
import json
import math
from pathlib import Path
import re
import signal
import sys
import tempfile
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
SUMMARY = ROOT / 'scripts/development-summary-input.py'
EVALUATOR = ROOT / 'benchmarks/development/evaluate.py'
FORMAT = b'slim-development-operation-cost-1'
MAX_I64 = (1 << 63) - 1
MAX_INPUT = 8 * 1048576
MAX_RECEIPT = 4 * 1048576
OPERATIONS = ('check', 'interfaces', 'context', 'build', 'run')
PHASES = ('check', 'interfaces', 'context', 'emit-c', 'native-compile', 'execute')
STATUSES = ('ok', 'compiler-error', 'timeout', 'output-limit', 'resource-limit', 'infrastructure-error')
REQUEST_STATUSES = (*STATUSES, 'constraint-error', 'denied', 'interrupted', 'unfinished')
OUTCOMES = ('accepted', 'correctness-failure', 'infrastructure-failure', 'interrupted',
            'elapsed-timeout', 'constraint-failure', 'unresolved', 'not-run')
LIFECYCLES = ('unprepared', 'prepared-undispatched', 'dispatched-unsubmitted',
              'unfinished-submission', 'submitted-awaiting-evaluation', 'unfinished-evaluation')
MONOTONIC = 'monotonic-launcher-to-direct-reap'
UNKNOWN = ['model tokens', 'model calls', 'active model time', 'native application performance',
           'general development effectiveness', 'kernel boot continuity']


class Refusal(ValueError):
    def __init__(self, category, reason):
        self.category, self.reason = category, str(reason)
        super().__init__(category + ': ' + self.reason)


def require(value, reason, category='invalid-receipt-metadata'):
    if not value:
        raise Refusal(category, reason)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def read(path, maximum):
    path = Path(path)
    require(not path.is_symlink() and path.is_file(), 'ordinary pinned file required', 'changed-identity')
    with path.open('rb') as stream:
        value = stream.read(maximum + 1)
    require(len(value) <= maximum, 'pinned file byte ceiling', 'admission')
    return value


def pairs(items):
    result = {}
    for key, value in items:
        require(key not in result, 'duplicate JSON key')
        result[key] = value
    return result


def invalid_number(value):
    raise Refusal('invalid-receipt-metadata', 'nonfinite JSON number')


def document(data):
    return json.loads(data, object_pairs_hook=pairs, parse_constant=invalid_number)


def pins(captured):
    paths = [('adapter_sha256', Path(__file__), MAX_RECEIPT),
             ('summary_adapter_sha256', SUMMARY, MAX_RECEIPT),
             ('evaluator_sha256', EVALUATOR, MAX_RECEIPT),
             ('freeze_sha256', Path(captured) / 'freeze.json', 1048576),
             ('manifest_sha256', Path(captured) / 'corpus/manifest.json', 1048576)]
    return {name: digest(read(path, cap)) for name, path, cap in paths}


def summary_adapter():
    # Fixed trusted path only. Input contains neither a module nor an import path.
    spec = importlib.util.spec_from_file_location('slim_operation_cost_summary', SUMMARY)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def load(captured, initial):
    header = document(read(Path(captured) / 'freeze.json', 1048576))
    require(initial['evaluator_sha256'] == header['corpus']['files']['evaluate.py'],
            'frozen evaluator identity differs before import', 'unsupported-evaluator')
    summary = summary_adapter()
    require(pins(captured) == initial, 'source identity changed during summary import', 'changed-identity')
    module = summary.evaluator()
    require(pins(captured) == initial, 'source identity changed during evaluator import', 'changed-identity')
    try:
        configuration = module.frozen(captured)
    except (OSError, ValueError, KeyError, TypeError) as error:
        raise Refusal('invalid-frozen-lifecycle', error) from error
    require(pins(captured) == initial, 'identity changed during frozen validation', 'changed-identity')
    return summary, module, configuration


def number(value, field):
    require(value is None or type(value) is int and 0 <= value <= MAX_I64,
            'unsupported numeric observation: ' + field)
    return b'' if value is None else str(value).encode('ascii')


def text(value, field, maximum=128, empty=False):
    require(type(value) is str and value.isascii(), 'ASCII scalar required: ' + field)
    result = value.encode('ascii')
    require((empty or len(result) > 0) and len(result) <= maximum,
            'scalar byte ceiling: ' + field, 'admission')
    return result


def hash_value(value, field, optional=False):
    if optional and value is None:
        return b''
    require(type(value) is str and re.fullmatch('[0-9a-f]{64}', value), 'invalid hash: ' + field)
    return value.encode('ascii')


def frame(value):
    return str(len(value)).encode('ascii') + b':' + value + b','


def frames(values):
    return b''.join(frame(value) for value in values)


def source_identity(module, value, files):
    require(type(value) is dict and set(value) == {'files', 'sha256', 'bytes'}, 'source identity fields')
    require(type(value['files']) is dict and set(value['files']) == set(files), 'complete source identity file set')
    for name, sha in value['files'].items():
        hash_value(sha, 'source file')
    hash_value(value['sha256'], 'source identity')
    require(value['sha256'] == module.digest(module.encoded(value['files'])), 'source identity association')
    require(type(value['bytes']) is int and 0 <= value['bytes'] <= module.MAX_SOURCE, 'source identity byte ceiling')
    return module.digest(module.encoded(value))


def authorized_launcher_paths(captured, initial):
    # Both locations contain the same pinned fixed evaluator bytes. Recorded
    # argv never provides a path to import, execute or independently trust.
    paths = (EVALUATOR.resolve(), (Path(captured) / 'corpus/evaluate.py').resolve())
    for path in paths:
        require(digest(read(path, MAX_RECEIPT)) == initial['evaluator_sha256'],
                'authorized fixed launcher source differs from pinned evaluator', 'changed-identity')
    return frozenset(str(path) for path in paths)


def launch(command, elapsed, launcher_paths):
    evidence = command.get('launch_evidence')
    executable = command.get('executable_elapsed_ns')
    if evidence is not None:
        require(type(evidence) is list and 1 <= len(evidence) <= 2, 'launcher evidence shape')
        for at, item in enumerate(evidence):
            require(type(item) is dict, 'launcher evidence object')
            if item.get('status') == 'exec-ready':
                require(at == 0 and set(item) == {'status', 'mono_ns', 'utc'}, 'exec-ready fields/order')
                number(item['mono_ns'], 'launcher monotonic')
                require(item['mono_ns'] is not None and type(item['utc']) is str and item['utc'].endswith('Z'),
                        'launcher clock metadata')
                datetime.fromisoformat(item['utc'].replace('Z', '+00:00'))
            else:
                require(item.get('status') == 'error' and at == len(evidence) - 1
                        and set(item) == {'status', 'reason'} and type(item['reason']) is str
                        and len(item['reason']) <= 2048, 'launcher error fields/order')
        if evidence[-1]['status'] == 'error':
            require(command['status'] == 'infrastructure-error' and executable is None,
                    'launcher failure association')
    if executable is not None:
        number(executable, 'executable duration')
        require(evidence is not None and len(evidence) == 1 and evidence[0]['status'] == 'exec-ready',
                'executable duration lacks positive launch evidence')
        require(elapsed is not None and executable <= elapsed, 'executable duration exceeds launcher duration')
    if command.get('launcher_argv') is not None or evidence is not None or executable is not None:
        argv, helper = command.get('argv'), command.get('launcher_argv')
        require(type(argv) is list and len(argv) > 0 and all(type(value) is str for value in argv), 'recorded argv shape')
        require(type(helper) is list and len(helper) == len(argv) + 7
                and all(type(value) is str for value in helper), 'fixed launcher argv shape')
        require(helper[0] == sys.executable and helper[1] in launcher_paths
                and helper[2] == '_exec' and helper[6] == '--' and helper[7:] == argv,
                'fixed launcher association')
        for at, field in ((3, 'cpu_seconds_limit'), (4, 'file_limit')):
            require(type(command.get(field)) is int and 0 < command[field] <= MAX_I64
                    and re.fullmatch('[1-9][0-9]*', helper[at]) and int(helper[at]) == command[field],
                    'launcher resource association')
        require(re.fullmatch('0|[1-9][0-9]*', helper[5]) and int(helper[5]) <= MAX_I64, 'launcher control descriptor')
    return executable


def command_fields(command, launcher_paths):
    require(type(command) is dict, 'command object required')
    phase, status = command.get('phase'), command.get('status')
    require(phase in PHASES and status in STATUSES, 'command phase/status spelling')
    rc = command.get('returncode')
    require(rc is None or type(rc) is int and -MAX_I64 <= rc <= MAX_I64, 'returncode observation')
    require(status != 'ok' or type(rc) is int and rc == 0, 'ok command requires zero returncode')
    require(rc is not None or status == 'infrastructure-error', 'missing returncode classification')
    if status == 'compiler-error':
        require(rc != 0 and phase != 'native-compile' and (phase == 'execute' or rc > 0),
                'trusted backend/signal status contradicts evaluator phase classification')
        require(rc not in {-signal.SIGXCPU, -signal.SIGXFSZ}, 'resource signal classification')
    if status == 'resource-limit':
        require(rc in {-signal.SIGXCPU, -signal.SIGXFSZ}, 'resource-limit needs observed resource signal')
    cause = command.get('failure_cause')
    require(cause in (None, 'unknown-native-nonzero-exit', 'observed-resource-signal'), 'failure cause spelling')
    if cause == 'observed-resource-signal':
        require(status == 'resource-limit', 'resource failure cause association')
    if cause == 'unknown-native-nonzero-exit':
        require(rc is not None and rc != 0 and status in ('compiler-error', 'infrastructure-error'),
                'unknown nonzero failure cause association')
    seconds = command.get('seconds_limit')
    require(type(seconds) in (int, float) and 0 < seconds <= 60 and math.isfinite(seconds),
            'recorded process time limit')
    require(type(command.get('cpu_seconds_limit')) is int and command['cpu_seconds_limit'] == math.ceil(seconds) + 1,
            'recorded CPU limit association')
    require(type(command.get('file_limit')) is int and 0 < command['file_limit'] <= 134217728,
            'recorded process file limit')
    elapsed = command.get('elapsed_ns')
    number(elapsed, 'command duration')
    basis = command.get('timing_basis')
    require(basis in (None, 'unknown', MONOTONIC), 'timing basis spelling')
    require(elapsed is None or basis == MONOTONIC, 'duration lacks recorded monotonic basis')
    executable = launch(command, elapsed, launcher_paths)
    role = command.get('output_role')
    require(role in (None, 'generated-c'), 'output role spelling')
    require(role is None or phase == 'emit-c' and status == 'ok' and rc == 0,
            'generated-C role lacks positive emit success')
    hash_value(command.get('executable_sha256'), 'executable', optional=True)
    values = (text(phase, 'phase'), text(status, 'command status'), number(elapsed, 'command duration'),
              number(executable, 'executable duration'), number(command.get('stdout_bytes'), 'stdout'),
              number(command.get('stderr_bytes'), 'stderr'), b'' if role is None else role.encode('ascii'),
              hash_value(command.get('executable_sha256'), 'executable', optional=True),
              (basis or 'unknown').encode('ascii'))
    return frames(values)


def request_fields(module, row, task, compiler, receipt, launcher_paths):
    original, started, finished = row['request'], row['started'], row['finished']
    operation = original['data']['operation']
    require(operation in OPERATIONS, 'request operation')
    sequence = original['seq']
    require(type(sequence) is int and 0 <= sequence <= 4095, 'request sequence range')
    status = 'unfinished' if finished is None else finished['data']['status']
    require(status in REQUEST_STATUSES, 'request status')
    capture = queue = wrapper = feedback = source = expected = receipt_hash = None
    if started is not None:
        quota = started['data']['quota_index']
        require(type(quota) is int and 1 <= quota <= 128, 'started admission index range')
        source = source_identity(module, started['data']['source_identity'], task['files'])
        identity = started['data']['expected_identity']
        require((identity is not None) == (operation == 'context'), 'expected identity presence')
        expected = source_identity(module, identity, task['files']) if identity is not None else None
        capture, queue = started['data']['capture_ns'], started['data']['queue_ns']
    if finished is not None:
        wrapper, feedback = finished['data']['elapsed_ns'], finished['data']['feedback_bytes']
        receipt_hash = finished['data']['receipt_sha256']
        require((finished['data']['receipt'] is not None) == (receipt_hash is not None), 'receipt path/hash association')
        hash_value(receipt_hash, 'receipt', optional=True)
    require(started is not None or receipt is None and receipt_hash is None, 'unstarted linked receipt')
    commands = []
    if receipt is not None:
        require(type(receipt) is dict and set(receipt) == {'schema', 'request', 'operation', 'source_identity',
                'expected_identity', 'compiler_sha256', 'commands'}, 'receipt fields')
        require(type(receipt['schema']) is int and receipt['schema'] == 2 and receipt['request'] == original['id']
                and receipt['operation'] == operation and receipt['compiler_sha256'] == compiler,
                'receipt request/operation/compiler association')
        require(receipt['source_identity'] == started['data']['source_identity']
                and receipt['expected_identity'] == started['data']['expected_identity'], 'receipt source/expected association')
        raw = receipt['commands']
        require(type(raw) is list and 1 <= len(raw) <= 3, 'command count', 'admission')
        phases = {'check': ('check',), 'interfaces': ('interfaces',), 'context': ('context',),
                  'build': ('emit-c', 'native-compile'), 'run': ('emit-c', 'native-compile', 'execute')}[operation]
        require(tuple(command.get('phase') if type(command) is dict else None for command in raw) == phases[:len(raw)],
                'phase prefix association')
        require(all(command.get('status') == 'ok' for command in raw[:-1])
                and (len(raw) == len(phases) or raw[-1].get('status') != 'ok'), 'positive predecessor/short prefix')
        commands = [command_fields(command, launcher_paths) for command in raw]
    controls = frames((number(sequence, 'sequence'), text(operation, 'operation'), b'true' if started is not None else b'false',
                       text(status, 'request status'), number(capture, 'capture'), number(queue, 'queue'),
                       number(wrapper, 'wrapper'), number(feedback, 'feedback'),
                       hash_value(source, 'source', optional=True), hash_value(expected, 'expected', optional=True),
                       hash_value(receipt_hash, 'receipt', optional=True)))
    require(len(controls) <= 1024 and sum(map(len, commands)) <= 2048, 'request/command payload ceiling', 'admission')
    return frames((text(original['id'], 'request UUID'), controls, b''.join(commands))), len(commands)


def snapshots(module, runs, trials):
    """Receipt/output validation covers unresolved ledgers too; no source reread."""
    observed, states = {}, {}
    for trial in trials:
        run = Path(runs) / trial['id']
        if not (run / 'ledger.jsonl').exists():
            require(not (run / 'ledger.jsonl').is_symlink(), 'dangling ledger alias', 'invalid-frozen-lifecycle')
            observed[trial['id']], states[trial['id']] = None, None
            continue
        ledger_bytes = module.read(run / 'ledger.jsonl', module.MAX_LEDGER)
        # The fixed replay validates these exact bounded bytes. A later file
        # read cannot silently supply the identity for an earlier parsed state.
        state = module.replay(io.BytesIO(ledger_bytes))
        require(bool(state['records']), 'missing prepared ledger record', 'invalid-frozen-lifecycle')
        module.run_metadata(run)
        prepared_bytes = module.read(run / 'prepared.json', module.MAX_JSON)
        require(module.digest(prepared_bytes) == state['records'][0]['data']['metadata_sha256'],
                'prepared metadata differs from captured ledger', 'changed-lifecycle')
        module.validate_receipts(run, state)
        linked, receipts = {}, {}
        for uuid, request in state['requests'].items():
            finished = request['finished']
            if finished is None or finished['data']['receipt'] is None:
                continue
            path = run / module.relative(finished['data']['receipt'])
            raw = module.read(path, module.MAX_JSON)
            require(module.digest(raw) == finished['data']['receipt_sha256'], 'receipt changed during metadata read', 'changed-identity')
            linked[uuid], receipts[uuid] = module.digest(raw), module.load_bytes(raw)
        result = module.digest(module.read(run / 'result.json', module.MAX_JSON)) if state['evaluation'] is not None else None
        require(module.read(run / 'ledger.jsonl', module.MAX_LEDGER) == ledger_bytes,
                'ledger changed during snapshot observation', 'changed-lifecycle')
        observed[trial['id']] = {'ledger_sha256': module.digest(ledger_bytes),
                                'prepared_sha256': module.digest(prepared_bytes),
                                'result_sha256': result, 'receipts': linked}
        states[trial['id']] = (state, receipts)
    return observed, states


def convert(captured, runs):
    captured, runs = Path(captured).resolve(), Path(runs).resolve()
    initial = pins(captured)
    summary_adapter_module, module, configuration = load(captured, initial)
    launcher_paths = authorized_launcher_paths(captured, initial)
    manifest = configuration['manifest']
    try:
        require(manifest == module.manifest(captured / 'corpus'),
                'frozen manifest differs from validated captured manifest', 'invalid-frozen-lifecycle')
    except (OSError, ValueError, KeyError, TypeError, IndexError) as error:
        if isinstance(error, Refusal):
            raise
        raise Refusal('invalid-frozen-lifecycle', error) from error
    require(len(manifest['tasks']) <= 32 and len(manifest['trials']) <= 64, 'task/trial ceiling', 'admission')
    tasks = {task['id']: task for task in manifest['tasks']}
    trials = sorted(manifest['trials'], key=lambda trial: (trial['task'].encode('ascii'), trial['condition'] != 'baseline'))
    try:
        inherited_before = summary_adapter_module.observations(module, captured, runs, trials, tasks)
        observed, states = snapshots(module, runs, trials)
        with tempfile.TemporaryDirectory(prefix='slim-operation-cost-') as temporary:
            output = Path(temporary) / 'summary.json'
            with contextlib.redirect_stdout(io.StringIO()):
                module.summarize(SimpleNamespace(freeze=captured, runs=runs, output=output))
            summary = module.document(output)
    except (OSError, ValueError, KeyError, TypeError, IndexError) as error:
        if isinstance(error, Refusal):
            raise
        raise Refusal('invalid-frozen-lifecycle', error) from error
    require(summary['freeze_sha256'] == initial['freeze_sha256'], 'summary belongs to different freeze', 'changed-identity')
    outcomes = {row['id']: row for row in summary['trials']}
    require(len(outcomes) == len(trials), 'planned summary denominator', 'invalid-frozen-lifecycle')
    identity = frames((hash_value(configuration['compiler_sha256'], 'compiler'),
                       initial['evaluator_sha256'].encode('ascii'), initial['manifest_sha256'].encode('ascii')))
    chunks = []
    input_bytes = 0

    def append(chunk):
        nonlocal input_bytes
        require(len(chunk) <= MAX_INPUT - input_bytes, 'framed input ceiling', 'admission')
        chunks.append(chunk)
        input_bytes += len(chunk)

    append(frames((FORMAT, initial['freeze_sha256'].encode('ascii'), identity)))
    request_count = started_count = command_count = unknown_trials = 0
    for trial in trials:
        row, item = outcomes[trial['id']], observed[trial['id']]
        outcome, lifecycle = row['outcome'], row.get('observation', '')
        require(outcome in OUTCOMES and lifecycle in ('', *LIFECYCLES), 'outcome/lifecycle control', 'invalid-frozen-lifecycle')
        raw_requests = [] if item is None else sorted(states[trial['id']][0]['requests'].items())
        require(len(raw_requests) <= 4096, 'per-trial request ceiling', 'admission')
        count = None if item is None else len(raw_requests)
        unknown_trials += int(count is None)
        metadata = frames((text(trial['id'], 'trial'), outcome.encode('ascii'), lifecycle.encode('ascii'),
                           b'' if item is None else item['ledger_sha256'].encode('ascii'),
                           b'' if item is None or item['result_sha256'] is None else item['result_sha256'].encode('ascii'),
                           b'' if item is None else item['prepared_sha256'].encode('ascii'), number(count, 'request count')))
        require(len(metadata) <= 1024, 'trial metadata ceiling', 'admission')
        append(frames((text(trial['task'], 'task', 64), text(trial['condition'], 'condition'), metadata)))
        starts = 0
        for uuid, request in raw_requests:
            require(request_count < 8192, 'global request ceiling', 'admission')
            is_started = request['started'] is not None
            require(not is_started or starts < 128 and started_count + starts < 8192,
                    'per-trial/global started ceiling', 'admission')
            require(re.fullmatch('[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}', uuid), 'canonical request UUID')
            encoded, count_commands = request_fields(module, request, tasks[trial['task']], configuration['compiler_sha256'],
                                                       states[trial['id']][1].get(uuid), launcher_paths)
            require(count_commands <= 24576 - command_count, 'global command ceiling', 'admission')
            append(encoded)
            starts += int(is_started)
            request_count += 1
            command_count += count_commands
        require(starts <= 128, 'per-trial started ceiling', 'admission')
        started_count += starts
    require(request_count <= 8192 and started_count <= 8192 and command_count <= 24576,
            'global row ceiling', 'admission')
    framed = b''.join(chunks)
    require(len(framed) <= MAX_INPUT, 'framed input ceiling', 'admission')
    try:
        inherited_after = summary_adapter_module.observations(module, captured, runs, trials, tasks)
        after, _ = snapshots(module, runs, trials)
        module.frozen(captured)
    except (OSError, ValueError, KeyError, TypeError, IndexError) as error:
        raise Refusal('changed-lifecycle', error) from error
    require(observed == after and inherited_before == inherited_after,
            'lifecycle/receipt/output changed during conversion', 'changed-lifecycle')
    require(pins(captured) == initial, 'source identity changed during conversion', 'changed-identity')
    receipt = {'schema': 1, **initial, 'compiler_sha256': configuration['compiler_sha256'],
               'input_sha256': digest(framed), 'input_bytes': len(framed), 'tasks': len(tasks), 'trials': len(trials),
               'requests': request_count, 'started_requests': started_count, 'commands': command_count,
               'unknown_request_count_trials': unknown_trials, 'lifecycle_observations': observed,
               'evidence': 'fixed evaluator validation and metadata association; writers stopped; no acceptance rerun',
               'source_identity_encoding': 'SHA256 of evaluator canonical JSON source identity; no operation source reread',
               'timing': 'same-host/boot-conditional monotonic observations; dimensions are independent',
               'executable_metric': 'fixed launcher pre-exec marker to direct-child reap; not executable main duration',
               'unknown': UNKNOWN}
    encoded_receipt = module.encoded(receipt) + b'\n'
    require(len(encoded_receipt) <= MAX_RECEIPT, 'receipt byte ceiling', 'admission')
    require(pins(captured) == initial, 'source identity changed during receipt assembly', 'changed-identity')
    return framed, receipt


def publication_barrier(captured, runs, receipt):
    initial = {key: receipt[key] for key in pins(captured)}
    require(pins(captured) == initial, 'source identity changed before publication', 'changed-identity')
    _, module, configuration = load(captured, initial)
    try:
        observed, _ = snapshots(module, runs, configuration['manifest']['trials'])
    except (OSError, ValueError, KeyError, TypeError, IndexError) as error:
        raise Refusal('changed-lifecycle', error) from error
    require(observed == receipt['lifecycle_observations'], 'lifecycle changed before publication', 'changed-lifecycle')
    require(pins(captured) == initial, 'source identity changed at publication barrier', 'changed-identity')


def publish(captured, runs, output, receipt_path):
    output, receipt_path = Path(output), Path(receipt_path)
    require(not output.exists() and not output.is_symlink() and not receipt_path.exists() and not receipt_path.is_symlink()
            and output.resolve() != receipt_path.resolve(), 'fresh distinct output and receipt paths required', 'admission')
    framed, receipt = convert(captured, runs)
    raw = json.dumps(receipt, sort_keys=True, separators=(',', ':'), ensure_ascii=True).encode('ascii') + b'\n'
    require(len(raw) <= MAX_RECEIPT and len(framed) <= MAX_INPUT, 'publication byte ceiling', 'admission')
    publication_barrier(captured, runs, receipt)
    output.parent.mkdir(parents=True, exist_ok=True)
    receipt_path.parent.mkdir(parents=True, exist_ok=True)
    with output.open('xb') as stream:
        stream.write(framed)
    with receipt_path.open('xb') as stream:
        stream.write(raw)
    return receipt


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('freeze', 'runs', 'output', 'receipt'):
        parser.add_argument('--' + name, required=True, type=Path)
    args = parser.parse_args()
    try:
        receipt = publish(args.freeze, args.runs, args.output, args.receipt)
    except (OSError, ValueError, KeyError, TypeError, IndexError, RecursionError) as error:
        category = error.category if isinstance(error, Refusal) else 'invalid-frozen-lifecycle'
        reason = error.reason if isinstance(error, Refusal) else type(error).__name__ + ': ' + str(error)
        parser.exit(1, category + ': ' + reason + '\n')
    print(json.dumps({'input': str(args.output), 'receipt': str(args.receipt),
                      'tasks': receipt['tasks'], 'trials': receipt['trials'], 'requests': receipt['requests']}))


if __name__ == '__main__':
    main()
