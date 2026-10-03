#!/usr/bin/env python3
"""Independent finite RFC0166 framing, report and measurement verification.

The expected report oracle was sealed before the SLIM serializer. Python never
implements SLIM parsing, checking, ownership, effects or candidate acceptance.
All native observations and failures are retained in the fresh output directory.
"""
import argparse
import contextlib
from dataclasses import replace
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import signal
import shutil
import subprocess
import sys
import tempfile
import time
from types import SimpleNamespace
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
ORACLE_PATH = ROOT / 'library/tests/development_operation_cost/oracle.py'
ORACLE_SHA = '0e303fd514e3cba3c4828ab8a59ee65a4d2e14ec7c1ebe111a330fbc188ebc39'
ADAPTER_PATH = ROOT / 'scripts/development-operation-cost-input.py'
RFC = ROOT / 'design/rfcs/0166-bounded-development-operation-cost.md'
MAX_FAULT_ORDINAL = 8192


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def module(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    result = importlib.util.module_from_spec(spec)
    sys.modules[name] = result
    spec.loader.exec_module(result)
    return result


def oracle():
    assert sha(ORACLE_PATH) == ORACLE_SHA, 'independent pre-serializer oracle identity changed'
    return module(ORACLE_PATH, 'operation_cost_independent_oracle')


def parsed_frames(data):
    """Transport-only reader used for independent known wire operands."""
    values, at = [], 0
    while at < len(data):
        colon = data.index(b':', at)
        spelling = data[at:colon]
        assert spelling and spelling.isdigit() and (spelling == b'0' or not spelling.startswith(b'0'))
        length = int(spelling)
        start, end = colon + 1, colon + 1 + length
        assert end < len(data) and data[end] == 44
        values.append((data[start:end], start))
        at = end + 1
    assert at == len(data)
    return values


def decode_input(data, O):
    """Decode exact wire operands without importing the adapter's serializer."""
    fields = parsed_frames(data)
    assert fields[0][0] == O.INPUT_FORMAT
    ids = [value.decode('ascii') for value, position in parsed_frames(fields[2][0])]
    identities = O.Identities(fields[1][0].decode('ascii'), *ids)
    trials, at = [], 3
    while at < len(fields):
        task, condition, raw = (fields[at + offset][0] for offset in range(3))
        trial = [value for value, position in parsed_frames(raw)]
        assert len(trial) == 7
        count = None if not trial[6] else int(trial[6])
        requests = []
        at += 3
        for unused in range(count or 0):
            uuid, controls, commands = (fields[at + offset][0] for offset in range(3))
            controls = [value for value, position in parsed_frames(controls)]
            assert len(controls) == 11
            scalars = [value for value, position in parsed_frames(commands)]
            assert len(scalars) % 9 == 0
            observed = []
            for start in range(0, len(scalars), 9):
                values = scalars[start:start + 9]
                observed.append(O.Command(values[0].decode(), values[1].decode(),
                    *[None if not value else int(value) for value in values[2:6]],
                    *[None if not value else value.decode() for value in values[6:8]], values[8].decode()))
            requests.append(O.Request(uuid.decode(), int(controls[0]), controls[1].decode(), controls[2] == b'true',
                controls[3].decode(), *[None if not value else int(value) for value in controls[4:8]],
                *[None if not value else value.decode() for value in controls[8:11]], tuple(observed)))
            at += 3
        trials.append(O.Trial(task, condition.decode(), trial[0], trial[1].decode(), trial[2].decode(),
            *[None if not value else value.decode() for value in trial[3:6]], count is not None, tuple(requests)))
    return identities, tuple(trials)


def simple_pair(O, requests=(), context=()):
    ids, seed = O.synthetic()
    return ids, (replace(seed[0], requests=tuple(requests)), replace(seed[1], requests=tuple(context)))


def edit_field(data, path, value, O):
    fields = [payload for payload, position in parsed_frames(data)]
    at, *tail = path
    fields[at] = edit_field(fields[at], tail, value, O) if tail else value
    return b''.join(O.frame(payload) for payload in fields)


def field_position(data, path):
    fields = parsed_frames(data)
    at, *tail = path
    payload, position = fields[at]
    return position + field_position(payload, tail) if tail else position


def frame_position(data, path):
    payload, unused = parsed_frames(data)[path[0]]
    return field_position(data, path) - len(str(len(payload)).encode()) - 1


def negative_cases(O):
    identities, seed = O.synthetic()
    identities, trials = simple_pair(O, (seed[0].requests[0],))
    data = O.input_bytes(identities, trials)
    # Expected positions come from framing offsets, never consumer diagnostics.
    controls = [
        ('format', (0,), b'unsupported', 11, 0),
        ('freeze-hash', (1,), b'A' * 64, 12, 0),
        ('nested-identity', (2, 1), b'g' * 64, 12, 0),
        ('empty-task', (3,), b'', 13, 0),
        ('condition', (4,), b'Context', 14, 0),
        ('trial-outcome', (5, 1), b'good', 14, 0),
        ('terminal-lifecycle', (5, 2), b'unprepared', 18, 0),
        ('trial-ledger', (5, 3), b'?', 12, 0),
        ('request-count-range', (5, 6), b'4097', 17, 0),
        ('operation', (7, 1), b'evaluate', 14, 0),
        ('started-spelling', (7, 2), b'1', 14, 0),
        ('request-status', (7, 3), b'accepted', 14, 0),
        ('numeric-boolean', (7, 4), b'true', 15, 0),
        ('numeric-negative', (7, 4), b'-1', 17, 0),
        ('numeric-leading-zero', (7, 4), b'01', 15, 1),
        ('numeric-overflow', (7, 4), b'9223372036854775808', 16, 18),
        ('source-hash', (7, 8), b'g' * 64, 12, 0),
        ('command-phase', (8, 0), b'compile', 14, 0),
        ('command-status', (8, 1), b'denied', 14, 0),
        ('command-basis', (8, 8), b'wall-clock', 14, 0),
        ('known-time-unknown-basis', (8, 8), b'unknown', 18, 0),
        ('duplicate-trial', (11, 0), trials[0].id, 21, 0),
    ]
    for name, path, value, code, delta in controls:
        altered = edit_field(data, path, value, O)
        yield name, altered, code, field_position(altered, path) + delta
    paired = simple_pair(O, seed[0].requests[:2])
    duplicate = edit_field(O.input_bytes(*paired), (9,), seed[0].requests[0].uuid.encode(), O)
    yield 'duplicate-request', duplicate, 22, field_position(duplicate, (9,))
    reversed_uuid = edit_field(O.input_bytes(*paired), (6,), O.request_uuid(99).encode(), O)
    yield 'request-order', reversed_uuid, 22, field_position(reversed_uuid, (9,))
    yield 'input-admission', b'x' * (8 * 1048576 + 1), 10, 0
    yield 'framing-leading-zero', b'00:,', 104, 1
    yield 'framing-colon', b'1x', 102, 1
    yield 'framing-comma', data[:-1] + b';', 107, len(data) - 1
    yield 'framing-truncated', data[:-1], 107, len(data) - 1
    started = tuple(replace(seed[0].requests[0], uuid=O.request_uuid(at + 1), sequence=at) for at in range(129))
    excess_starts = O.input_bytes(*simple_pair(O, started))
    yield 'started-129', excess_starts, 10, field_position(excess_starts, (6 + 128 * 3,))
    trials33 = []
    for at in range(33):
        key = f'task-{at:02}'.encode()
        trials33.extend((replace(seed[0], task=key, id=b'b-' + key, requests=()),
                         replace(seed[1], task=key, id=b'c-' + key, requests=())))
    task_excess = O.input_bytes(identities, tuple(trials33))
    yield 'task-33-trial-65', task_excess, 10, frame_position(task_excess, (3 + 64 * 3,))
    trial65 = O.input_bytes(identities, tuple(trials33[:-1]))
    yield 'trial-65-missing-final-pair', trial65, 10, frame_position(trial65, (3 + 64 * 3,))
    denied = tuple(O.Request(O.request_uuid(at + 1), at, 'check', False, 'denied') for at in range(4096))
    first = simple_pair(O, denied, denied)[1]
    second = (replace(seed[0], task=b'beta', id=b'b-beta', requests=denied[:1]),
              replace(seed[1], task=b'beta', id=b'c-beta', requests=()))
    global_excess = O.input_bytes(identities, first + second)
    yield 'global-request-8193', global_excess, 10, frame_position(global_excess, (3 + 2 * 3 + 8192 * 3 + 3,))
    run = seed[0].requests[3]
    four = replace(run, commands=run.commands + (run.commands[-1],))
    command_excess = O.input_bytes(*simple_pair(O, (four,)))
    outer = parsed_frames(command_excess)[8][1]
    commands = parsed_frames(parsed_frames(command_excess)[8][0])
    yield 'command-four', command_excess, 10, outer + commands[27][1] - len(str(len(commands[27][0])).encode()) - 1


def positive_cases(O):
    ids, seed = O.synthetic()
    yield 'sealed-synthetic', ids, seed
    yield 'empty-planned', ids, ()
    yield 'known-zero-pair', *simple_pair(O)
    # Every supplied terminal outcome and every unresolved lifecycle is separate
    # from the operation/command status observations.
    terminal = ('accepted', 'correctness-failure', 'infrastructure-failure', 'interrupted',
                'elapsed-timeout', 'constraint-failure')
    for outcome in terminal:
        yield 'outcome-' + outcome, ids, (replace(seed[0], outcome=outcome, requests=()), replace(seed[1], requests=()))
    for lifecycle in ('dispatched-unsubmitted', 'unfinished-submission', 'submitted-awaiting-evaluation', 'unfinished-evaluation'):
        yield 'lifecycle-' + lifecycle, ids, (replace(seed[0], requests=()), replace(seed[1], lifecycle=lifecycle, requests=()))
    for status in O.REQUEST_STATUSES:
        unstarted = status in ('denied', 'constraint-error', 'interrupted', 'unfinished')
        request = O.Request(O.request_uuid(1), 0, 'check', not unstarted, status,
                            wrapper=None if status == 'unfinished' else 0,
                            source=None if unstarted else '1' * 64)
        yield 'request-status-' + status, *simple_pair(O, (request,))
    for status in O.COMMAND_STATUSES:
        command = O.Command('check', status, 0, stdout=0, stderr=0, basis=O.MONOTONIC)
        request = O.Request(O.request_uuid(1), 0, 'check', True, status,
                            source='1' * 64, receipt='2' * 64, commands=(command,))
        yield 'command-status-' + status, *simple_pair(O, (request,))
    for value in (0, 1, O.MAX_I64 - 1, O.MAX_I64):
        for other in (None, 0, 1, O.MAX_I64):
            requests = tuple(O.Request(O.request_uuid(at + 1), at, 'check', True, 'ok', capture=item,
                                      source='1' * 64) for at, item in enumerate((value, other, 2)))
            yield f'sum-{value}-{other}', *simple_pair(O, requests)
    # Four independent tie dimensions, including reused sequence/UUID across
    # trials and ordinal distinctions. Non-emits are ineligible for C bytes.
    tied = tuple(O.Request(O.request_uuid(at + 1), sequence, 'run', True, 'ok', capture=7,
                         source='1' * 64, receipt='2' * 64,
                         commands=tuple(O.Command(phase, elapsed=7, stdout=7, role='generated-c' if phase == 'emit-c' else None,
                                                  basis=O.MONOTONIC) for phase in ('emit-c', 'native-compile', 'execute')))
                 for at, sequence in enumerate((3, 1, 1, 3)))
    pair = simple_pair(O, tied, tied)
    yield 'ties-all-levels', *pair
    for role, stdout in ((None, None), (None, 0), ('generated-c', None), ('generated-c', 0), ('generated-c', O.MAX_I64)):
        command = O.Command('emit-c', stdout=stdout, role=role)
        request = O.Request(O.request_uuid(1), 0, 'build', True, 'ok', source='1' * 64,
                            receipt='2' * 64, commands=(command, O.Command('native-compile')))
        yield f'emit-role-{role}-{stdout}', *simple_pair(O, (request,))
    for number in (1, 2, 4, 8, 16, 32, 64, 128):
        requests = tuple(O.Request(O.request_uuid(at + 1), at, 'run', True, 'ok', capture=at,
                         source='1' * 64, receipt='2' * 64,
                         commands=tuple(O.Command(phase, elapsed=at, stdout=at, stderr=0,
                                      role='generated-c' if phase == 'emit-c' else None, basis=O.MONOTONIC)
                                        for phase in ('emit-c', 'native-compile', 'execute'))) for at in range(number))
        yield f'geometric-started-{number}', *simple_pair(O, requests)
    for count in (1, 2, 4, 8, 16, 32):
        trials = []
        for at in range(count):
            key = f'task-{at:02}'.encode()
            trials.extend((replace(seed[0], task=key, id=b'b-' + key, requests=()),
                           replace(seed[1], task=key, id=b'c-' + key, requests=())))
        yield f'geometric-tasks-{count}', ids, tuple(trials)
    maximum_trials = []
    for at in range(32):
        key = f'{at:02}'.encode() + b'x' * 62
        for condition in ('baseline', 'context'):
            identifier = f'{at:02}-{condition}'.encode().ljust(128, b'z')
            requests = tuple(O.Request(O.request_uuid(index + 1), 4095, 'run', True, 'ok',
                            O.MAX_I64, O.MAX_I64, O.MAX_I64, O.MAX_I64, '1' * 64, receipt='2' * 64,
                            commands=tuple(O.Command(phase, elapsed=O.MAX_I64, stdout=O.MAX_I64, stderr=O.MAX_I64,
                               role='generated-c' if phase == 'emit-c' else None, basis=O.MONOTONIC)
                               for phase in ('emit-c', 'native-compile', 'execute'))) for index in range(128))
            maximum_trials.append(O.Trial(key, condition, identifier, 'accepted', '', '3' * 64, '4' * 64,
                                          '5' * 64, requests=requests))
    yield 'all-row-maxima-64-trials', ids, tuple(maximum_trials)
    denied = tuple(O.Request(O.request_uuid(at + 1), at, 'check', False, 'denied') for at in range(4096))
    yield 'maximum-denied-8192', *simple_pair(O, denied, denied)


def execute(command, output, label, environment, timeout=180):
    began = time.monotonic_ns()
    try:
        result = subprocess.run(list(map(str, command)), capture_output=True, env=environment, timeout=timeout)
    except subprocess.TimeoutExpired as error:
        (output / (label + '.stdout')).write_bytes(error.stdout or b'')
        (output / (label + '.stderr')).write_bytes(error.stderr or b'')
        (output / (label + '.json')).write_text(json.dumps({'argv': list(map(str, command)),
            'status': 'timeout', 'elapsed_ns': time.monotonic_ns() - began, 'returncode': None}, sort_keys=True) + '\n')
        raise
    (output / (label + '.stdout')).write_bytes(result.stdout)
    (output / (label + '.stderr')).write_bytes(result.stderr)
    (output / (label + '.json')).write_text(json.dumps({'argv': list(map(str, command)),
        'returncode': result.returncode, 'elapsed_ns': time.monotonic_ns() - began,
        'stdout_sha256': hashlib.sha256(result.stdout).hexdigest(), 'stderr_sha256': hashlib.sha256(result.stderr).hexdigest()}, sort_keys=True) + '\n')
    return result.returncode, result.stdout, result.stderr


def build(compiler, cc, output, environment):
    programs = {'application': [], 'boundary': []}
    for kind, project in [('application', 'development-operation-cost.project'),
                          ('boundary', 'development-operation-cost-tests.project')]:
        emitted = execute([compiler, ROOT / 'library' / project], output, kind + '-emit', environment)
        assert emitted[0] == 0 and emitted[2] == b'', ('production emit', emitted)
        generated = output / (kind + '.c')
        generated.write_bytes(emitted[1])
        for mode, flags in [('ordinary', ['-O2', '-DNDEBUG']),
                            ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all'])]:
            program = output / (kind + '-' + mode)
            actual = execute([cc, '-std=c11', '-Wall', '-Wextra', '-Werror', *flags, '-I', ROOT / 'runtime',
                              generated, ROOT / 'runtime/slim_rt.c', '-o', program], output, kind + '-' + mode + '-build', environment)
            assert actual == (0, b'', b''), (kind, mode, actual)
            programs[kind].append(program)
    return programs


def verify_boundaries(programs, output, environment, O):
    rows, path = [], output / 'boundary.ns'
    families = [('empty', O.Identities('1' * 64, '2' * 64, '3' * 64, '4' * 64), ()),
                ('synthetic', *O.synthetic()), list(positive_cases(O))[-2]]
    for name, identities, trials in families:
        data, expected = O.input_bytes(identities, trials), O.report_bytes(identities, trials)
        path.write_bytes(data)
        work, size = O.counters(trials)[-1], len(expected)
        for mode, program in enumerate(programs):
            for cap in (-1, 0, size - 1, size, size + 1, 524288, 524289):
                result = execute([program, 'report', path, cap, 1000000], output, f'report-cap-{name}-{mode}-{cap}', environment)
                wanted = (0, expected, b'') if size <= cap <= 524288 else (65, b'error 23 at 0\n', b'')
                assert result == wanted, ('report cap', name, mode, cap, result, wanted)
                rows.append({'family': name, 'variant': mode, 'report_cap': cap, 'outcome': result[0]})
            for cap in (-1, 0, work - 1, work, work + 1, 1000000, 1000001):
                result = execute([program, 'report', path, 524288, cap], output, f'work-cap-{name}-{mode}-{cap}', environment)
                if not 0 <= cap <= 1000000:
                    wanted = (65, b'error 10 at 0\n', b'')
                    assert result == wanted, ('invalid work parameter', name, mode, cap, result, wanted)
                elif cap >= work:
                    assert result == (0, expected, b''), ('exact work cap', name, mode, cap, result)
                else:
                    assert result[0] == 65 and result[2] == b'' and result[1].startswith(b'error 24 at ')
                    assert result[1].endswith(b'\n') and result[1].count(b'\n') == 1
                    position = int(result[1][len(b'error 24 at '):-1])
                    assert position in source_positions(data), ('work exhaustion does not identify original span', name, cap, position)
                rows.append({'family': name, 'variant': mode, 'work_cap': cap, 'outcome': result[0],
                             'exhaustion_position': 'validated original span; unique exact position unknown'})
    return rows


def source_positions(data):
    """Original framing/record/scalar spans; no implementation visit order."""
    positions = {0, len(data)}
    fields = parsed_frames(data)
    for payload, position in fields:
        positions.update((position, position - len(str(len(payload))) - 1))
    def nested(payload, base):
        for value, position in parsed_frames(payload):
            positions.update((base + position, base + position - len(str(len(value))) - 1))
    nested(fields[2][0], fields[2][1])
    at = 3
    while at < len(fields):
        metadata = parsed_frames(fields[at + 2][0])
        nested(fields[at + 2][0], fields[at + 2][1])
        count = int(metadata[6][0]) if metadata[6][0] else 0
        at += 3
        for unused in range(count):
            nested(fields[at + 1][0], fields[at + 1][1])
            nested(fields[at + 2][0], fields[at + 2][1])
            at += 3
    return positions


def verify_native(programs, output, environment, O, faults):
    path, rows = output / 'input.ns', []
    for at, (name, identities, trials) in enumerate(positive_cases(O)):
        data, report = O.input_bytes(identities, trials), O.report_bytes(identities, trials)
        assert len(data) <= 8 * 1048576 and len(report) <= 524288, ('predeclared valid family admission', name)
        if name == 'sealed-synthetic':
            assert O.counters(trials) == O.SYNTHETIC_COUNTERS
        path.write_bytes(data)
        for mode, program in enumerate(programs):
            actual = execute([program, path], output, f'valid-{at}-{mode}', environment)
            assert actual == (0, report, b''), (name, mode, actual, hashlib.sha256(report).hexdigest())
            rows.append({'name': name, 'variant': mode, 'input_sha256': hashlib.sha256(data).hexdigest(),
                         'report_sha256': hashlib.sha256(report).hexdigest(), 'report_bytes': len(report), 'work': O.counters(trials)})
    for at, (name, data, code, position) in enumerate(negative_cases(O)):
        path.write_bytes(data)
        expected = (65, f'error {code} at {position}\n'.encode(), b'')
        for mode, program in enumerate(programs):
            actual = execute([program, path], output, f'invalid-{at}-{mode}', environment)
            assert actual == expected, (name, mode, actual, expected)
            rows.append({'name': name, 'variant': mode, 'error': code, 'position': position,
                         'input_sha256': hashlib.sha256(data).hexdigest()})
    for mode, program in enumerate(programs):
        for at, (arguments, expected) in enumerate([([], (64, b'error 2 at 0\n', b'')),
            ([output / 'missing'], (66, b'error 1 at 0\n', b'')), ([path, 'detail'], (64, b'error 2 at 0\n', b''))]):
            actual = execute([program, *arguments], output, f'invocation-{mode}-{at}', environment)
            assert actual == expected, (mode, arguments, actual, expected)
    if faults:
        identities, trials = O.synthetic()
        path.write_bytes(O.input_bytes(identities, trials))
        expected = (0, O.report_bytes(identities, trials), b'')
        for mode, program in enumerate(programs):
            completed = False
            for ordinal in range(1, MAX_FAULT_ORDINAL + 1):
                actual = execute([program, path], output, f'fault-{mode}-{ordinal}', dict(environment, SLIM_ALLOC_FAIL_AT=str(ordinal)))
                if actual[0] == 71:
                    assert actual[1] == b'' and actual[2] == f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode(), (mode, ordinal, actual)
                else:
                    assert actual == expected, (mode, ordinal, actual)
                    rows.append({'name': 'complete-fault-ordinal', 'variant': mode, 'ordinal': ordinal})
                    completed = True
                    break
            assert completed, 'bounded allocation campaign did not reach unmodified full success'
    return rows


def raw_expected(module, captured, runs, O):
    """Read independently checked operands; never use adapter encoding helpers."""
    configuration = module.frozen(captured)
    header = O.Identities(sha(captured / 'freeze.json'), configuration['compiler_sha256'],
                         sha(ROOT / 'benchmarks/development/evaluate.py'), sha(captured / 'corpus/manifest.json'))
    with tempfile.TemporaryDirectory(prefix='slim-cost-oracle-summary-') as temporary:
        path = Path(temporary) / 'summary.json'
        with contextlib.redirect_stdout(io.StringIO()):
            module.summarize(SimpleNamespace(freeze=captured, runs=runs, output=path))
        summary = {row['id']: row for row in module.document(path)['trials']}
    trials = []
    for trial in sorted(configuration['manifest']['trials'], key=lambda value: (value['task'].encode(), value['condition'] != 'baseline')):
        run = runs / trial['id']
        row = summary[trial['id']]
        if not (run / 'ledger.jsonl').exists():
            trials.append(O.Trial(trial['task'].encode(), trial['condition'], trial['id'].encode(),
                                  row['outcome'], row.get('observation', ''), None, None, None, False))
            continue
        state = module.ledger(run)
        module.validate_receipts(run, state)
        requests = []
        for uuid, request in sorted(state['requests'].items()):
            original, started, finished = request['request'], request['started'], request['finished']
            begin = None if started is None else started['data']
            end = None if finished is None else finished['data']
            commands = []
            if end is not None and end['receipt'] is not None:
                receipt = module.document(run / module.relative(end['receipt']))
                for command in receipt['commands']:
                    commands.append(O.Command(command['phase'], command['status'], command.get('elapsed_ns'),
                        command.get('executable_elapsed_ns'), command.get('stdout_bytes'), command.get('stderr_bytes'),
                        command.get('output_role'), command.get('executable_sha256'), command.get('timing_basis') or 'unknown'))
            source = None if begin is None else module.digest(module.encoded(begin['source_identity']))
            expected = None if begin is None or begin['expected_identity'] is None else module.digest(module.encoded(begin['expected_identity']))
            requests.append(O.Request(uuid, original['seq'], original['data']['operation'], begin is not None,
                'unfinished' if end is None else end['status'], None if begin is None else begin['capture_ns'],
                None if begin is None else begin['queue_ns'], None if end is None else end['elapsed_ns'],
                None if end is None else end['feedback_bytes'], source, expected,
                None if end is None else end['receipt_sha256'], tuple(commands)))
        trials.append(O.Trial(trial['task'].encode(), trial['condition'], trial['id'].encode(), row['outcome'],
            row.get('observation', ''), sha(run / 'ledger.jsonl'), sha(run / 'result.json') if state['evaluation'] is not None else None,
            sha(run / 'prepared.json'), True, tuple(requests)))
    return header, tuple(trials)


def adapter_parity(adapter, E, captured, runs, O):
    identities, trials = raw_expected(E, captured, runs, O)
    framed, receipt = adapter.convert(captured, runs)
    assert framed == O.input_bytes(identities, trials), 'adapter disagrees with independent raw-observation framing'
    assert receipt['input_sha256'] == hashlib.sha256(framed).hexdigest() and receipt['input_bytes'] == len(framed)
    assert receipt['trials'] == len(trials) and receipt['requests'] == sum(len(trial.requests) for trial in trials)
    assert receipt['commands'] == sum(len(request.commands) for trial in trials for request in trial.requests)
    return framed, receipt, O.report_bytes(identities, trials)


def rewrite_receipt(E, run, relative, mutation):
    path = run / relative
    receipt = E.document(path)
    mutation(receipt)
    path.chmod(0o644)
    path.write_bytes(E.encoded(receipt) + b'\n')
    records = [E.load_bytes(line) for line in (run / 'ledger.jsonl').read_bytes().splitlines()]
    previous = E.ZERO
    for record in records:
        if record['kind'] == 'finished' and record['data']['receipt'] == relative:
            record['data']['receipt_sha256'] = sha(path)
        record['prev'] = previous
        record['sha256'] = E.digest(E.encoded({key: value for key, value in record.items() if key != 'sha256'}))
        previous = record['sha256']
    (run / 'ledger.jsonl').write_bytes(b''.join(E.encoded(record) + b'\n' for record in records))
    return path


def synthetic_command(E, run, folder, phase, status='ok', returncode=0, role=None, known=True):
    """Declared metadata fixture, never an executed command or quality claim."""
    path = run / 'receipts' / folder
    path.mkdir(parents=True)
    result = {'argv': ['/fixed-observation-tool'], 'phase': phase, 'status': status, 'returncode': returncode,
              'seconds_limit': 10, 'cpu_seconds_limit': 11, 'file_limit': E.MAX_PROCESS_FILE,
              'output_limit': E.MAX_OUTPUT, 'elapsed_ns': 11 if known else None,
              'timing_basis': 'monotonic-launcher-to-direct-reap', 'executable_sha256': '9' * 64}
    if known:
        result['launcher_argv'] = [sys.executable, str((ROOT / 'benchmarks/development/evaluate.py').resolve()),
                                  '_exec', '11', str(E.MAX_PROCESS_FILE), '99', '--', *result['argv']]
        result['launch_evidence'] = [{'status': 'exec-ready', 'mono_ns': 1, 'utc': '2026-10-03T00:00:00Z'}]
        result['executable_elapsed_ns'] = 7
    if role is not None:
        result['output_role'] = role
    for name, data in (('stdout', b'x'), ('stderr', b'')):
        artifact = path / (name + '.bin')
        artifact.write_bytes(data)
        result.update({name + '_artifact': str(artifact), name + '_bytes': len(data), name + '_sha256': E.digest(data)})
    return result


def verify_adapter(compiler, cc, output, O):
    """Manual isolated ledgers and receipts verify transport, not SLIM acceptance."""
    adapter = module(ADAPTER_PATH, 'operation_cost_adapter_fixture')
    helpers = module(ROOT / 'benchmarks/development/test_evaluate.py', 'operation_cost_transport_helpers')
    E = helpers.E
    folder = output / 'adapter-transport'
    folder.mkdir()
    corpus = helpers.corpus(folder, production=True)
    captured = folder / 'freeze'
    with contextlib.redirect_stdout(io.StringIO()):
        E.freeze(SimpleNamespace(corpus=corpus, compiler=compiler, cc=cc, destination=captured))
    proof = folder / 'supplied-transport-preparation.json'
    E.write_json(proof, {'passed': True, 'freeze_sha256': sha(captured / 'freeze.json'),
                        'scope': 'supplied isolated transport preparation only; no semantic acceptance'})
    runs, run = folder / 'runs', folder / 'runs/02-context'
    with contextlib.redirect_stdout(io.StringIO()):
        E.prepare(SimpleNamespace(freeze=captured, verification=proof, trial='02-context', destination=run))
        E.start(SimpleNamespace(run=run))
    identity = E.identity({filename: (run / 'candidate' / filename).read_bytes()
                           for filename in ('app.slim', 'lib.slim', 'slim.project')})
    commands = [synthetic_command(E, run, 'run-emit', 'emit-c', role='generated-c'),
                synthetic_command(E, run, 'run-native', 'native-compile'),
                synthetic_command(E, run, 'run-execute', 'execute')]
    identifier = O.request_uuid(1)
    E.append(run, 'request', {'operation': 'run', 'selector': None, 'source': None, 'expected': None}, identifier)
    E.append(run, 'started', {'source_identity': identity, 'expected_identity': None, 'capture_ns': 0,
                             'queue_ns': None, 'quota_index': 1}, identifier)
    relative = 'receipts/run.json'
    E.write_json(run / relative, {'schema': 2, 'request': identifier, 'operation': 'run', 'source_identity': identity,
                                'expected_identity': None, 'compiler_sha256': sha(captured / 'toolchain/slimc'), 'commands': commands})
    E.append(run, 'finished', {'status': 'ok', 'reason': '', 'receipt': relative, 'receipt_sha256': sha(run / relative),
                              'elapsed_ns': 100, 'feedback_bytes': None}, identifier)
    # A terminal started/no receipt retains wrapper separately and an unfinished
    # unstarted request supplies no fabricated zero timing or command count.
    identifier2 = O.request_uuid(2)
    E.append(run, 'request', {'operation': 'check', 'selector': None, 'source': None, 'expected': None}, identifier2)
    E.append(run, 'started', {'source_identity': identity, 'expected_identity': None, 'capture_ns': None,
                             'queue_ns': 0, 'quota_index': 2}, identifier2)
    E.append(run, 'finished', {'status': 'infrastructure-error', 'reason': 'supplied fixture', 'receipt': None,
                              'receipt_sha256': None, 'elapsed_ns': 0, 'feedback_bytes': None}, identifier2)
    E.append(run, 'request', {'operation': 'interfaces', 'selector': None, 'source': None, 'expected': None}, O.request_uuid(3))
    with patch.object(subprocess, 'run', side_effect=AssertionError('adapter executed a subprocess')), \
         patch.object(subprocess, 'Popen', side_effect=AssertionError('adapter launched a subprocess')):
        framed, receipt, report = adapter_parity(adapter, E, captured, runs, O)
    assert receipt['trials'] == 2 and receipt['requests'] == 3 and receipt['commands'] == 3
    checks = [{'name': 'declared-isolated-receipts', 'input_sha256': receipt['input_sha256'],
               'scope': 'manual data observations; subprocess launch disabled; no command/model/source acceptance claim'}]
    original_receipt, original_ledger = (run / relative).read_bytes(), (run / 'ledger.jsonl').read_bytes()
    mutations = [
        ('schema', lambda value: value.__setitem__('schema', True)),
        ('request', lambda value: value.__setitem__('request', O.request_uuid(99))),
        ('operation', lambda value: value.__setitem__('operation', 'build')),
        ('compiler', lambda value: value.__setitem__('compiler_sha256', 'a' * 64)),
        ('source', lambda value: value['source_identity'].__setitem__('bytes', value['source_identity']['bytes'] + 1)),
        ('expected', lambda value: value.__setitem__('expected_identity', identity)),
        ('phase', lambda value: value['commands'][0].__setitem__('phase', 'execute')),
        ('status', lambda value: value['commands'][0].__setitem__('status', 'denied')),
        ('ok-returncode', lambda value: value['commands'][0].__setitem__('returncode', 1)),
        ('returncode-boolean', lambda value: value['commands'][0].__setitem__('returncode', False)),
        ('known-time-basis', lambda value: value['commands'][0].__setitem__('timing_basis', 'unknown')),
        ('invalid-basis', lambda value: value['commands'][0].__setitem__('timing_basis', 'wall-time')),
        ('duration-boolean', lambda value: value['commands'][0].__setitem__('elapsed_ns', True)),
        ('duration-negative', lambda value: value['commands'][0].__setitem__('elapsed_ns', -1)),
        ('duration-float', lambda value: value['commands'][0].__setitem__('elapsed_ns', 1.0)),
        ('duration-overflow', lambda value: value['commands'][0].__setitem__('elapsed_ns', O.MAX_I64 + 1)),
        ('stdout-boolean', lambda value: value['commands'][0].__setitem__('stdout_bytes', True)),
        ('role-spelling', lambda value: value['commands'][0].__setitem__('output_role', 'source')),
        ('role-non-emit', lambda value: value['commands'][1].__setitem__('output_role', 'generated-c')),
        ('launch-absent', lambda value: value['commands'][0].pop('launch_evidence')),
        ('launch-shape', lambda value: value['commands'][0].__setitem__('launch_evidence', [])),
        ('launch-boolean', lambda value: value['commands'][0]['launch_evidence'][0].__setitem__('mono_ns', True)),
        ('launch-error-positive-duration', lambda value: value['commands'][0].__setitem__('launch_evidence', [{'status': 'error', 'reason': 'fixture'}])),
        ('executable-duration-range', lambda value: value['commands'][0].__setitem__('executable_elapsed_ns', 12)),
        ('executable-hash', lambda value: value['commands'][0].__setitem__('executable_sha256', 'G' * 64)),
        ('fixed-helper-path', lambda value: value['commands'][0]['launcher_argv'].__setitem__(1, '/input-selected-module.py')),
        ('fixed-helper-tail', lambda value: value['commands'][0]['launcher_argv'].__setitem__(-1, '/different-command')),
        ('cpu-association', lambda value: value['commands'][0].__setitem__('cpu_seconds_limit', 1)),
        ('cpu-boolean', lambda value: value['commands'][0].__setitem__('cpu_seconds_limit', True)),
        ('file-association', lambda value: value['commands'][0].__setitem__('file_limit', 1)),
        ('native-false-compiler-error', lambda value: value['commands'][1].update(status='compiler-error', returncode=1)),
        ('trusted-negative-compiler-error', lambda value: value['commands'][0].update(status='compiler-error', returncode=-signal.SIGTERM)),
        ('false-resource', lambda value: value['commands'][2].update(status='resource-limit', returncode=1)),
        ('positive-predecessor', lambda value: value['commands'][0].update(status='timeout')),
        ('extra-command', lambda value: value['commands'].append(value['commands'][-1])),
    ]
    for name, mutation in mutations:
        try:
            rewrite_receipt(E, run, relative, mutation)
            # All these tampered metadata cases retain valid receipt/ledger and
            # artifact hashes. Rejection cannot rely only on changed identities.
            E.validate_receipts(run, E.ledger(run))
            rejected = None
            try:
                adapter.convert(captured, runs)
            except adapter.Refusal as error:
                rejected = error.category
            assert rejected is not None, ('adapter accepted rehashed contradictory metadata', name)
            checks.append({'name': name, 'rehash': True, 'category': rejected})
        finally:
            (run / relative).write_bytes(original_receipt)
            (run / 'ledger.jsonl').write_bytes(original_ledger)
    def frozen_copy_launcher(value):
        for command in value['commands']:
            command['launcher_argv'][1] = str((captured / 'corpus/evaluate.py').resolve())
    try:
        rewrite_receipt(E, run, relative, frozen_copy_launcher)
        counterpart_input, unused_receipt, counterpart_report = adapter_parity(adapter, E, captured, runs, O)
        assert counterpart_input != framed and counterpart_report == report
        checks.append({'name': 'hash-validated-fixed-frozen-launcher-counterpart', 'accepted': True,
                       'scope': 'same fixed pinned evaluator bytes, no recorded module execution'})
    finally:
        (run / relative).write_bytes(original_receipt)
        (run / 'ledger.jsonl').write_bytes(original_ledger)
    # Supported failure prefixes preserve evaluator classification, independent
    # request status, unknown timing and positive-only generated-C evidence.
    prefixes = [
        ('emit-compiler-error', 1, 'compiler-error', 1, None),
        ('emit-resource', 1, 'resource-limit', -signal.SIGXCPU, None),
        ('native-infrastructure', 2, 'infrastructure-error', 1, 'generated-c'),
        ('native-launch-failure', 2, 'infrastructure-error', None, 'generated-c'),
        ('execute-nonzero', 3, 'compiler-error', 1, 'generated-c'),
        ('execute-negative', 3, 'compiler-error', -signal.SIGTERM, 'generated-c'),
        ('execute-timeout-zero-reaped', 3, 'timeout', 0, 'generated-c'),
        ('execute-output-limit', 3, 'output-limit', -signal.SIGKILL, 'generated-c'),
    ]
    for name, count, status, rc, role in prefixes:
        def change(value):
            value['commands'] = value['commands'][:count]
            value['commands'][0].pop('output_role', None)
            if role is not None:
                value['commands'][0]['output_role'] = role
            last = value['commands'][-1]
            last.update(status=status, returncode=rc)
            if rc is None:
                last['elapsed_ns'] = None
                last.pop('executable_elapsed_ns', None)
                last.pop('launch_evidence', None)
                last.pop('launcher_argv', None)
        try:
            rewrite_receipt(E, run, relative, change)
            adapter_parity(adapter, E, captured, runs, O)
            checks.append({'name': name, 'classification': 'supported partial prefix; request status remains independent'})
        finally:
            (run / relative).write_bytes(original_receipt)
            (run / 'ledger.jsonl').write_bytes(original_ledger)
    # Identity and lifecycle replacement barriers are valid alternate states,
    # rather than malformed blobs that would make the check vacuous.
    alternate_summary = adapter.summary_adapter()
    original_summary = E.summarize
    def lifecycle_barrier(arguments):
        E.append(run, 'note', {'category': 'observation', 'reason': 'supplied read barrier'})
        return original_summary(arguments)
    try:
        with patch.object(adapter, 'summary_adapter', return_value=alternate_summary), \
             patch.object(alternate_summary, 'evaluator', return_value=E), \
             patch.object(E, 'summarize', side_effect=lifecycle_barrier):
            rejected = False
            try:
                adapter.convert(captured, runs)
            except adapter.Refusal as error:
                rejected = error.category == 'changed-lifecycle'
            assert rejected, 'valid concurrent lifecycle addition accepted'
            checks.append({'name': 'valid-lifecycle-barrier', 'rejected': True})
    finally:
        (run / 'ledger.jsonl').write_bytes(original_ledger)
    original_metadata = E.run_metadata
    metadata_calls = 0
    def state_identity_barrier(arguments):
        nonlocal metadata_calls
        metadata_calls += 1
        # Inherited observations calls run_metadata once before snapshots.
        # The second call is exactly between snapshot replay and identity read.
        if metadata_calls == 2:
            E.append(run, 'note', {'category': 'observation', 'reason': 'valid snapshot identity read barrier'})
        return original_metadata(arguments)
    try:
        rejected = False
        with patch.object(adapter, 'summary_adapter', return_value=alternate_summary), \
             patch.object(alternate_summary, 'evaluator', return_value=E), \
             patch.object(E, 'run_metadata', side_effect=state_identity_barrier):
            try:
                adapter.convert(captured, runs)
            except adapter.Refusal as error:
                rejected = error.category == 'changed-lifecycle'
        assert rejected, 'old parsed state was bound to new valid ledger identity'
        checks.append({'name': 'snapshot-state-identity-barrier', 'rejected': True})
    finally:
        (run / 'ledger.jsonl').write_bytes(original_ledger)
    original_freeze = (captured / 'freeze.json').read_bytes()
    changed_freeze = E.load_bytes(original_freeze)
    changed_freeze['created'] = '2026-10-03T00:00:01Z'
    def freeze_barrier(arguments):
        (captured / 'freeze.json').chmod(0o644)
        (captured / 'freeze.json').write_bytes(E.encoded(changed_freeze) + b'\n')
        E.frozen(captured)
        return original_summary(arguments)
    try:
        with patch.object(adapter, 'summary_adapter', return_value=alternate_summary), \
             patch.object(alternate_summary, 'evaluator', return_value=E), \
             patch.object(E, 'summarize', side_effect=freeze_barrier):
            rejected = False
            try:
                adapter.convert(captured, folder / 'unprepared-replacement')
            except adapter.Refusal:
                rejected = True
            assert rejected, 'valid freeze replacement in all-unprepared conversion accepted'
            checks.append({'name': 'valid-freeze-barrier', 'rejected': True})
    finally:
        (captured / 'freeze.json').write_bytes(original_freeze)
    for path in (run / 'prepared.json', run / 'ledger.jsonl', run / relative,
                 Path(commands[0]['stdout_artifact']), captured / 'freeze.json', captured / 'corpus/evaluate.py'):
        original = path.read_bytes()
        try:
            path.chmod(0o644)
            path.write_bytes(original + b' ')
            rejected = False
            try:
                adapter.convert(captured, runs)
            except (adapter.Refusal, OSError, ValueError, KeyError, TypeError):
                rejected = True
            assert rejected, ('changed validated material accepted', path)
            checks.append({'name': 'changed-material-' + path.name, 'rejected': True})
        finally:
            path.write_bytes(original)
    # Orphan results have no terminal authority and cannot change observations.
    E.write_json(run / 'result.json', {'orphan': True, 'outcome': 'accepted'})
    orphan_input, unused_receipt, unused_report = adapter_parity(adapter, E, captured, runs, O)
    assert orphan_input == framed, 'orphan result changed unresolved observations'
    checks.append({'name': 'orphan-result-ignored', 'unchanged': True})
    (run / 'result.json').unlink()
    # All source replacement probes use copies; repository evaluator/adapter
    # source remains untouched. The test hook is not an input-selected module.
    copied_path = folder / 'copied-adapter.py'
    copied_source = ADAPTER_PATH.read_bytes()
    copied_path.write_bytes(copied_source)
    copied_adapter = module(copied_path, 'operation_cost_source_pin_fixture')
    copied_adapter.ROOT, copied_adapter.SUMMARY, copied_adapter.EVALUATOR = ROOT, adapter.SUMMARY, adapter.EVALUATOR
    original_snapshots = copied_adapter.snapshots
    def copied_source_barrier(*arguments):
        result = original_snapshots(*arguments)
        copied_path.write_bytes(copied_source + b'\n# changed observed source\n')
        return result
    try:
        rejected = False
        with patch.object(copied_adapter, 'snapshots', side_effect=copied_source_barrier):
            try:
                copied_adapter.convert(captured, runs)
            except copied_adapter.Refusal as error:
                rejected = error.category == 'changed-identity'
        assert rejected, 'changed observed new-adapter source accepted'
        checks.append({'name': 'new-adapter-source-barrier', 'rejected': True})
    finally:
        copied_path.write_bytes(copied_source)
    for field, original_path in (('SUMMARY', adapter.SUMMARY), ('EVALUATOR', adapter.EVALUATOR)):
        copy = folder / ('copied-' + field.lower() + '.py')
        original = original_path.read_bytes()
        copy.write_bytes(original)
        original_loader = adapter.summary_adapter
        def source_import_barrier():
            copy.write_bytes(original + b'\n# source replacement barrier\n')
            return original_loader()
        rejected = False
        with patch.object(adapter, field, copy), patch.object(adapter, 'summary_adapter', side_effect=source_import_barrier):
            try:
                adapter.convert(captured, folder / 'all-unprepared-source-barrier')
            except adapter.Refusal as error:
                rejected = error.category == 'changed-identity'
        assert rejected, ('source pin changed before import accepted', field)
        checks.append({'name': field.lower() + '-source-import-barrier', 'rejected': True})
    # Publication must preflight both files, and recheck lifecycle after convert.
    framed_again, receipt_again, unused = adapter_parity(adapter, E, captured, runs, O)
    assert framed_again == framed and receipt_again['input_sha256'] == receipt['input_sha256']
    fresh_output, fresh_receipt = folder / 'published.ns', folder / 'published.json'
    adapter.publish(captured, runs, fresh_output, fresh_receipt)
    assert fresh_output.read_bytes() == framed and E.load_bytes(fresh_receipt.read_bytes())['input_sha256'] == receipt['input_sha256']
    for label, target, receipt_target in [('existing-output', fresh_output, folder / 'unused-receipt'),
        ('existing-receipt', folder / 'unused-output', fresh_receipt), ('same-path', folder / 'same', folder / 'same')]:
        rejected = False
        try:
            adapter.publish(captured, runs, target, receipt_target)
        except adapter.Refusal as error:
            rejected = error.category == 'admission'
        assert rejected, ('publication accepted invalid paths', label)
        checks.append({'name': label, 'rejected': True})
    dangling = folder / 'dangling-output'
    dangling.symlink_to(folder / 'nonexistent-target')
    try:
        rejected = False
        try:
            adapter.publish(captured, runs, dangling, folder / 'dangling-receipt')
        except adapter.Refusal as error:
            rejected = error.category == 'admission'
        assert rejected and not (folder / 'dangling-receipt').exists(), 'dangling output alias accepted'
        checks.append({'name': 'dangling-output-symlink', 'rejected': True})
    finally:
        dangling.unlink()
    oversized = dict(receipt, extra='x' * (4 * 1048576))
    oversized_output, oversized_receipt = folder / 'oversized.ns', folder / 'oversized.json'
    with patch.object(adapter, 'convert', return_value=(framed, oversized)):
        rejected = False
        try:
            adapter.publish(captured, runs, oversized_output, oversized_receipt)
        except adapter.Refusal as error:
            rejected = error.category == 'admission'
        assert rejected and not oversized_output.exists() and not oversized_receipt.exists(), 'oversized receipt created prefix'
    checks.append({'name': 'receipt-ceiling-before-publication', 'rejected': True})
    original_convert = adapter.convert
    def publication_lifecycle(*arguments):
        result = original_convert(*arguments)
        E.append(run, 'note', {'category': 'observation', 'reason': 'before publication'})
        return result
    target, receipt_target = folder / 'race.ns', folder / 'race.json'
    try:
        rejected = False
        with patch.object(adapter, 'convert', side_effect=publication_lifecycle):
            try:
                adapter.publish(captured, runs, target, receipt_target)
            except adapter.Refusal as error:
                rejected = error.category == 'changed-lifecycle'
        assert rejected and not target.exists() and not receipt_target.exists(), 'publication accepted changed lifecycle or created prefix'
        checks.append({'name': 'publication-lifecycle-barrier', 'rejected': True})
    finally:
        (run / 'ledger.jsonl').write_bytes(original_ledger)
    (folder / 'receipt.json').write_text(json.dumps({'scope': 'isolated transport-only data, no participant quality', 'checks': checks}, indent=2) + '\n')
    return checks


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', type=Path, default=ROOT / 'build/toolchain/slimc')
    parser.add_argument('--cc', default='cc')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--freeze', type=Path)
    parser.add_argument('--runs', type=Path)
    parser.add_argument('--no-faults', action='store_true')
    parser.add_argument('--python-only', action='store_true')
    args = parser.parse_args()
    assert (args.freeze is None) == (args.runs is None), 'freeze/runs must be supplied together'
    args.output.mkdir(parents=True, exist_ok=False)
    O = oracle()
    assert O.counters(O.synthetic()[1]) == O.SYNTHETIC_COUNTERS
    cases = list(positive_cases(O))
    for name, identities, trials in cases:
        data = O.input_bytes(identities, trials)
        decoded = decode_input(data, O)
        assert decoded == (identities, trials), ('oracle transport round trip', name)
        assert len(O.report_bytes(identities, trials)) <= 524288
    environment = {key: value for key, value in os.environ.items() if key != 'SLIM_ALLOC_FAIL_AT'}
    inputs = [ORACLE_PATH, ADAPTER_PATH, Path(__file__), RFC, ROOT / 'scripts/development-summary-input.py',
              ROOT / 'benchmarks/development/evaluate.py', ROOT / 'runtime/slim_rt.c', ROOT / 'runtime/slim_rt.h']
    inputs += sorted((ROOT / 'library/components').glob('*.slim'))
    inputs += sorted((ROOT / 'library/experimental').glob('*.slim'))
    inputs += [ROOT / 'benchmarks/development/test_evaluate.py']
    inputs += [ROOT / 'library/applications/development_operation_cost/main.slim', ROOT / 'library/development-operation-cost.project',
               ROOT / 'library/development-operation-cost-tests.project', ROOT / 'library/tests/development_operation_cost.slim']
    if not args.python_only:
        inputs.append(args.compiler.resolve())
        inputs.append(Path(shutil.which(args.cc) or args.cc).resolve())
    identities = {str(path): sha(path) for path in inputs if path.exists()}
    (args.output / 'source-identities-before.json').write_text(json.dumps(identities, indent=2, sort_keys=True) + '\n')
    programs = {'application': [], 'boundary': []} if args.python_only else build(args.compiler.resolve(), args.cc, args.output, environment)
    rows = [] if args.python_only else verify_native(programs['application'], args.output, environment, O, not args.no_faults)
    boundary_rows = [] if args.python_only else verify_boundaries(programs['boundary'], args.output, environment, O)
    adapter_checks = verify_adapter(args.compiler.resolve(), args.cc, args.output, O)
    real = None
    if args.freeze is not None:
        adapter = module(ADAPTER_PATH, 'operation_cost_adapter')
        E = adapter.summary_adapter().evaluator()
        framed, receipt, report = adapter_parity(adapter, E, args.freeze.resolve(), args.runs.resolve(), O)
        path = args.output / 'actual-cohort.ns'
        path.write_bytes(framed)
        (args.output / 'actual-cohort.expected').write_bytes(report)
        for mode, program in enumerate(programs['application']):
            assert execute([program, path], args.output, f'actual-cohort-{mode}', environment) == (0, report, b'')
        real = receipt
    assert identities == {str(path): sha(path) for path in inputs if path.exists()}, 'verification source/artifact changed'
    receipt = {'schema': 1, 'identities': identities, 'oracle_sealed_sha256': ORACLE_SHA,
               'positive_families': len(cases), 'native_observations': rows, 'actual_cohort': real,
               'adapter_checks': adapter_checks,
               'boundary_observations': boundary_rows,
               'allocation_campaign': {'maximum_ordinal_per_variant': MAX_FAULT_ORDINAL,
                   'stop': 'first exact complete report', 'later_ordinals': 'unknown; not exercised'},
               'classification': 'finite exact bytes and observed operands; no SLIM or outcome semantic authority',
               'timing': 'verification elapsed only; no native performance comparison'}
    (args.output / 'receipt.json').write_text(json.dumps(receipt, indent=2, sort_keys=True) + '\n')
    print(json.dumps({'positive_families': len(cases), 'native_observations': len(rows), 'receipt': str(args.output / 'receipt.json')}))


if __name__ == '__main__':
    main()
