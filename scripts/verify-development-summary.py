#!/usr/bin/env python3
"""Finite independent RFC-0164 byte/measurement oracle; SLIM accepts itself.
No candidate or compiler semantics are implemented in this verifier.
"""
import argparse
import contextlib
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import random
import subprocess
import time
from types import SimpleNamespace
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
MAXIMUM = 2 ** 63 - 1
OUTCOMES = [b'accepted', b'correctness-failure', b'infrastructure-failure', b'interrupted',
            b'elapsed-timeout', b'constraint-failure', b'unresolved', b'not-run']
LIFECYCLES = [b'unprepared', b'prepared-undispatched', b'dispatched-unsubmitted',
              b'unfinished-submission', b'submitted-awaiting-evaluation', b'unfinished-evaluation']


class Invalid(Exception):
    def __init__(self, code, position):
        self.code, self.position = code, position


def frame(value):
    return str(len(value)).encode() + b':' + value + b','


def triple(values):
    return b''.join(frame(value) for value in values)


def trial(identifier, outcome=b'accepted', oracle=b'true', elapsed=b'1', operations=b'0', requests=b'0', observation=None, submitted=b''):
    if observation is None:
        observation = b'unfinished-evaluation' if outcome == b'unresolved' else b'unprepared' if outcome == b'not-run' else b''
    return triple([identifier, outcome, oracle, elapsed,
                   b'same-host-monotonic-conditional' if elapsed else b'unknown', operations, requests,
                   observation, submitted])


def source(rows, identities=None):
    identities = identities or [b'1' * 64, b'2' * 64, b'3' * 64]
    return triple([b'slim-development-summary-1', b'0' * 64, triple(identities)]) + b''.join(triple(row) for row in rows)


def read_frame(data, cursor, end, maximum):
    at, value, digits = cursor, 0, 0
    while cursor < end and 48 <= data[cursor] <= 57:
        if digits and data[at] == 48:
            raise Invalid(104, cursor)
        value = value * 10 + data[cursor] - 48
        if value > maximum:
            raise Invalid(105, cursor)
        cursor += 1
        digits += 1
    if cursor == end:
        raise Invalid(103, cursor)
    if not digits or data[cursor] != 58:
        raise Invalid(102, cursor)
    start, finish = cursor + 1, cursor + 1 + value
    if finish > end:
        raise Invalid(106, end)
    if finish == end or data[finish] != 44:
        raise Invalid(107, finish)
    return (data[start:finish], start, finish), finish + 1


def read_triple(data, cursor, end, limits):
    fields = []
    for limit in limits:
        if cursor == end:
            raise Invalid(108, cursor)
        item, cursor = read_frame(data, cursor, end, limit)
        fields.append(item)
    return fields, cursor


def hash_valid(value):
    return len(value) == 64 and all(byte in b'0123456789abcdef' for byte in value)


def integer(value, position, maximum):
    if not value:
        return None
    sign = value.startswith(b'-')
    beginning = int(sign)
    if len(value) == beginning:
        raise Invalid(15, position + beginning)
    for at in range(beginning, len(value)):
        if not 48 <= value[at] <= 57:
            raise Invalid(15, position + at)
        current = int(value[:at + 1])
        if not -(2 ** 63) <= current <= MAXIMUM:
            raise Invalid(16, position + at)
    if value[beginning] == 48 and (sign or len(value) > 1):
        raise Invalid(15, position + 1)
    result = int(value)
    if not 0 <= result <= maximum:
        raise Invalid(17, position)
    return result


def parse_trial(data, raw):
    _, start, end = raw
    fields, cursor = [], start
    while cursor < end:
        if len(fields) == 9:
            raise Invalid(19, cursor)
        item, cursor = read_frame(data, cursor, end, 128)
        fields.append(item)
    if len(fields) != 9:
        raise Invalid(19, cursor)
    values = [item[0] for item in fields]
    positions = [item[1] for item in fields]
    if not values[0]:
        raise Invalid(13, positions[0])
    if values[1] not in OUTCOMES:
        raise Invalid(14, positions[1])
    if values[2] not in (b'true', b'false', b'unknown'):
        raise Invalid(14, positions[2])
    elapsed = integer(values[3], positions[3], MAXIMUM)
    if values[4] != (b'unknown' if elapsed is None else b'same-host-monotonic-conditional'):
        raise Invalid(18, positions[4])
    operations = integer(values[5], positions[5], 4096)
    requests = integer(values[6], positions[6], 4096)
    if values[7] not in [b'', *LIFECYCLES]:
        raise Invalid(14, positions[7])
    if values[8] and not hash_valid(values[8]):
        raise Invalid(12, positions[8])
    if (values[1] == b'accepted' and values[2] != b'true') or (values[1] in (b'unresolved', b'not-run') and values[2] != b'unknown'):
        raise Invalid(18, positions[2])
    if operations is not None and requests is not None and operations > requests:
        raise Invalid(18, positions[5])
    if values[1] == b'unresolved':
        valid_lifecycle = values[7] in LIFECYCLES[2:]
    elif values[1] == b'not-run':
        valid_lifecycle = values[7] in LIFECYCLES[:2]
    else:
        valid_lifecycle = values[7] == b''
    if not valid_lifecycle:
        raise Invalid(18, positions[7])
    return dict(identifier=values[0], identifier_position=positions[0], outcome=values[1],
                oracle=values[2], elapsed=elapsed, operations=operations, requests=requests)


def parse(data, maximum=32):
    if not 0 <= maximum <= 32 or len(data) > 131072:
        raise Invalid(10, 0)
    header, cursor = read_triple(data, 0, len(data), (32, 64, 256))
    if header[0][0] != b'slim-development-summary-1':
        raise Invalid(11, header[0][1])
    if not hash_valid(header[1][0]):
        raise Invalid(12, header[1][1])
    identities, after = read_triple(data, header[2][1], header[2][2], (64, 64, 64))
    if after != header[2][2]:
        raise Invalid(19, after)
    for identity in identities:
        if not hash_valid(identity[0]):
            raise Invalid(12, identity[1])
    rows = []
    while cursor < len(data):
        beginning = cursor
        fields, cursor = read_triple(data, cursor, len(data), (64, 1024, 1024))
        if len(rows) == maximum:
            raise Invalid(10, beginning)
        if not fields[0][0]:
            raise Invalid(13, fields[0][1])
        rows.append(dict(key=fields[0][0], position=fields[0][1], baseline=parse_trial(data, fields[1]), context=parse_trial(data, fields[2])))
    ordered = sorted(rows, key=lambda row: row['key'])
    for left, right in zip(ordered, ordered[1:]):
        if left['key'] == right['key']:
            raise Invalid(20, right['position'])
    trials = sorted([row[condition] for row in rows for condition in ('baseline', 'context')], key=lambda item: item['identifier'])
    for left, right in zip(trials, trials[1:]):
        if left['identifier'] == right['identifier']:
            raise Invalid(21, right['identifier_position'])
    return [header[1][0], *[item[0] for item in identities]], rows, ordered


def ratio_reason(pair):
    if any(pair[condition]['outcome'] != b'accepted' for condition in ('baseline', 'context')):
        return b'outside-accepted'
    if any(pair[condition]['elapsed'] is None for condition in ('baseline', 'context')):
        return b'missing'
    if pair['baseline']['elapsed'] == 0:
        return b'zero-denominator'
    return b'exact'


def condition_line(rows, condition):
    trials = [row[condition] for row in rows]
    terminal = [trial for trial in trials if trial['outcome'] not in (b'unresolved', b'not-run')]
    elapsed = [trial['elapsed'] for trial in terminal if trial['elapsed'] is not None]
    result = b'condition ' + condition.encode() + f' configured {len(rows)} dispatched {sum(trial["outcome"] != b"not-run" for trial in trials)} terminal {len(terminal)}'.encode()
    for outcome in OUTCOMES:
        result += b' ' + outcome + b' ' + str(sum(trial['outcome'] == outcome for trial in trials)).encode()
    for oracle in (b'true', b'false', b'unknown'):
        result += b' oracle-' + oracle + b' ' + str(sum(trial['oracle'] == oracle for trial in trials)).encode()
    for name in ('operations', 'requests'):
        known = [trial[name] for trial in trials if trial[name] is not None]
        amount = str(sum(known)).encode() if sum(known) <= MAXIMUM else b'unknown'
        result += b' ' + name.encode() + b'-known ' + str(len(known)).encode() + b' ' + name.encode() + b'-sum ' + amount + b' ' + name.encode() + b'-unknown ' + str(len(trials) - len(known)).encode()
    amount = str(sum(elapsed)).encode() if sum(elapsed) <= MAXIMUM else b'unknown'
    excluded = sum(trial['elapsed'] is not None and trial['outcome'] in (b'unresolved', b'not-run') for trial in trials)
    result += f' terminal-elapsed-known {len(elapsed)} terminal-elapsed-missing {len(terminal) - len(elapsed)} terminal-elapsed-sum '.encode() + amount + f' excluded-elapsed-known {excluded}\n'.encode()
    return result


def median_line(rows, condition):
    values = [row[condition]['elapsed'] for row in rows if row[condition]['outcome'] not in (b'unresolved', b'not-run') and row[condition]['elapsed'] is not None]
    # Independent sorted order statistics; insertion work derives inversions and
    # the terminating comparisons, without using the production insertion code.
    work = sum(sum(previous > value for previous in values[:at]) + int(any(previous <= value for previous in values[:at]))
               for at, value in enumerate(values))
    ordered = sorted(values)
    lower = str(ordered[(len(ordered) - 1) // 2]).encode() if ordered else b'unknown'
    upper = str(ordered[len(ordered) // 2]).encode() if ordered else b'unknown'
    reason = b'exact' if ordered else b'empty'
    return b'stats ' + condition.encode() + f' bounded maximum 32 work {work} cap 1024 lower '.encode() + lower + b' upper ' + upper + b' reason ' + reason + b'\n'


def expected(data, statistics=False, maximum=32, report_maximum=65536):
    try:
        identities, rows, ordered = parse(data, maximum)
        report = f'development-summary-v1 tasks {len(rows)} trials {2 * len(rows)}\nidentities supplied '.encode() + b' '.join(identities) + b'\n'
        for condition in ('baseline', 'context'):
            report += condition_line(rows, condition)
        if statistics:
            for condition in ('baseline', 'context'):
                report += median_line(rows, condition)
        paired_oracles = [b''.join(row[condition]['oracle'][:1] for condition in ('baseline', 'context')) for row in rows]
        reason_counts = {reason: sum(ratio_reason(row) == reason for row in rows) for reason in
                         (b'exact', b'missing', b'zero-denominator', b'outside-accepted')}
        report += f'pairs oracle TT {paired_oracles.count(b"tt")} TF {paired_oracles.count(b"tf")} FT {paired_oracles.count(b"ft")} FF {paired_oracles.count(b"ff")} unknown {sum(b"u" in pair for pair in paired_oracles)} strict-accepted {len(rows) - reason_counts[b"outside-accepted"]} elapsed-exact {reason_counts[b"exact"]} elapsed-missing {reason_counts[b"missing"]} elapsed-zero-denominator {reason_counts[b"zero-denominator"]} elapsed-outside-accepted {reason_counts[b"outside-accepted"]}\n'.encode()
        for row in ordered:
            reason = ratio_reason(row)
            ratio = f'{row["context"]["elapsed"]}/{row["baseline"]["elapsed"]}'.encode() if reason == b'exact' else b'unknown'
            report += b'task ' + frame(row['key']) + b' baseline ' + row['baseline']['outcome'] + b' context ' + row['context']['outcome'] + b' elapsed-ratio ' + ratio + b' reason ' + reason + b'\n'
        report += b'model-tokens unknown model-calls unknown active-model-time unknown native-performance unknown general-effectiveness unknown\n'
        if not 0 <= report_maximum <= 65536 or len(report) > report_maximum:
            raise Invalid(23, 0)
        return 0, report, b''
    except Invalid as invalid:
        return 65, f'error {invalid.code} at {invalid.position}\n'.encode(), b''


def cases():
    rows = [(b'z', trial(b'z-b'), trial(b'z-c', elapsed=b'2')),
            (b'a\x00:\n', trial(b'a-b', outcome=b'correctness-failure', oracle=b'false', elapsed=b'100'),
             trial(b'a-c', outcome=b'elapsed-timeout', oracle=b'true', elapsed=b'200')),
            (b'm', trial(b'm-b', outcome=b'unresolved', oracle=b'unknown', elapsed=str(MAXIMUM).encode()),
             trial(b'm-c', outcome=b'not-run', oracle=b'unknown', elapsed=b'', operations=b'', requests=b''))]
    result = [source([]), source(rows), source(list(reversed(rows)))]
    for outcome in OUTCOMES:
        for other in OUTCOMES:
            result.append(source([(b'x', trial(b'b', outcome, b'true' if outcome == b'accepted' else b'unknown'),
                                  trial(b'c', other, b'true' if other == b'accepted' else b'false'))]))
    for elapsed in (b'', b'0', b'1', str(MAXIMUM).encode()):
        for other in (b'', b'0', b'1', str(MAXIMUM).encode()):
            result.append(source([(b'x', trial(b'b', elapsed=elapsed), trial(b'c', elapsed=other))]))
    terminal_oracles = [(b'correctness-failure', b'false', b'correctness-failure', b'false'),
                        (b'accepted', b'true', b'infrastructure-failure', b'unknown'),
                        (b'infrastructure-failure', b'unknown', b'accepted', b'true'),
                        (b'correctness-failure', b'false', b'infrastructure-failure', b'unknown'),
                        (b'infrastructure-failure', b'unknown', b'correctness-failure', b'false')]
    for left, left_oracle, right, right_oracle in terminal_oracles:
        data = source([(b'x', trial(b'b', left, left_oracle, elapsed=b''),
                       trial(b'c', right, right_oracle, elapsed=b''))])
        assert expected(data)[0] == 0, 'terminal oracle control must be admitted'
        result.append(data)
    maximum_median = source([(b'a', trial(b'a-b', elapsed=str(MAXIMUM).encode()),
                             trial(b'a-c', elapsed=str(MAXIMUM).encode())),
                            (b'b', trial(b'b-b', elapsed=str(MAXIMUM).encode()),
                             trial(b'b-c', elapsed=str(MAXIMUM).encode()))])
    assert expected(maximum_median, statistics=True)[0] == 0, 'maximum median control must be admitted'
    result.append(maximum_median)
    maximum_rows = [(f'task-{at:02}'.encode(), trial(f'b-{at}'.encode(), elapsed=str(MAXIMUM if at % 3 == 0 else 32 - at).encode(), operations=b'4096', requests=b'4096'),
                     trial(f'c-{at}'.encode(), elapsed=str(32 - at).encode())) for at in range(33)]
    result += [source(maximum_rows[:count]) for count in (1, 31, 32, 33)]
    result.append(source(maximum_rows[:32]) + frame(trial(b'extra-65th-trial')))
    result += [source([rows[0], rows[0]]), source([(b'a', trial(b'duplicate'), trial(b'duplicate'))])]
    result += [source([(b'a', trial(b'b', outcome=outcome, oracle=oracle, elapsed=b'99'),
                        trial(b'c'))]) for outcome in (b'unresolved', b'not-run') for oracle in (b'true', b'false', b'unknown')]
    for raw in (b'', b'garbage', b'00:,', b'1:x;', b'131073:' + b'x' * 131073 + b',',
                b'x' * 131071, b'x' * 131072, b'x' * 131073):
        result.append(raw)
    valid = source(rows[:1])
    result += [valid[:at] for at in [1, 2, 3, 27, 31, len(valid) - 1, len(valid) - 2]]
    result += [valid + b'0:,', valid + triple([b'extra', trial(b'b2'), trial(b'c2')])]
    for at, alternatives in {0: [b'', b'i' * 128, b'i' * 129], 1: [b'wrong'], 2: [b'wrong', b'false'],
                             3: [b'00', b'-0', b'-1', b'-9223372036854775808', b'1x', b'+1', b'9223372036854775808', b'1' * 128],
                             4: [b'unknown', b'wrong'], 5: [b'4097', b'1'], 6: [b'-1'],
                             7: [b'unprepared', b'wrong'], 8: [b'A' * 64, b'a' * 63, b'a' * 64]}.items():
        fields = [b'b', b'accepted', b'true', b'1', b'same-host-monotonic-conditional', b'0', b'0', b'', b'']
        for value in alternatives:
            changed = fields.copy()
            changed[at] = value
            result.append(source([(b'a', triple(changed), trial(b'c'))]))
    result += [source([(b'a', trial(b'b') + frame(b'extra'), trial(b'c'))]),
               source([(b'a', trial(b'b')[:-3], trial(b'c'))]),
               source([(b'a', b'x' * 1024, trial(b'c'))]),
               source([(b'a', b'x' * 1025, trial(b'c'))]),
               source([(b'x' * 65, trial(b'b'), trial(b'c'))]),
               source([], [b'1' * 64, b'A' * 64, b'3' * 64])]
    rng = random.Random(164)
    for at in range(64):
        data = bytearray(valid)
        position = rng.randrange(len(data))
        data[position] = rng.randrange(256)
        result.append(bytes(data))
    fixture = ROOT / 'library/applications/development_summary/fixtures/observations.ns'
    result.append(fixture.read_bytes())
    assert expected(fixture.read_bytes())[1] == fixture.with_suffix('.expected').read_bytes(), 'synthetic fixture golden disagrees with finite oracle'
    return result


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def execute(command, output, label, environment, timeout=120):
    begun = time.monotonic_ns()
    try:
        result = subprocess.run(list(map(str, command)), capture_output=True, env=environment, timeout=timeout)
    except subprocess.TimeoutExpired as error:
        (output / (label + '.stdout')).write_bytes(error.stdout or b'')
        (output / (label + '.stderr')).write_bytes(error.stderr or b'')
        (output / (label + '.json')).write_text(json.dumps({'argv': list(map(str, command)),
            'status': 'timeout', 'elapsed_ns': time.monotonic_ns() - begun,
            'returncode': None}, sort_keys=True) + '\n')
        raise
    (output / (label + '.stdout')).write_bytes(result.stdout)
    (output / (label + '.stderr')).write_bytes(result.stderr)
    (output / (label + '.json')).write_text(json.dumps({'argv': list(map(str, command)), 'returncode': result.returncode,
        'elapsed_ns': time.monotonic_ns() - begun, 'stdout_sha256': hashlib.sha256(result.stdout).hexdigest(),
        'stderr_sha256': hashlib.sha256(result.stderr).hexdigest()}, sort_keys=True) + '\n')
    return result.returncode, result.stdout, result.stderr


def build(compiler, cc, output, environment):
    programs = {'application': [], 'boundary': []}
    for name, project in [('application', 'development-summary.project'), ('boundary', 'development-summary-tests.project')]:
        emitted = execute([compiler, ROOT / 'library' / project], output, name + '-emit', environment)
        assert emitted[0] == 0 and emitted[2] == b'', (name, 'production emit', emitted)
        generated = output / (name + '.c')
        generated.write_bytes(emitted[1])
        for mode, flags in [('ordinary', ['-O2', '-DNDEBUG']), ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all'])]:
            program = output / (name + '-' + mode)
            result = execute([cc, '-std=c11', '-Wall', '-Wextra', '-Werror', *flags,
                              '-I', ROOT / 'runtime', generated, ROOT / 'runtime/slim_rt.c', '-o', program], output, name + '-' + mode + '-build', environment)
            assert result == (0, b'', b''), (name, mode, result)
            programs[name].append(program)
    return programs


def verify(programs, output, environment, faults):
    observations = []
    data_path = output / 'input.ns'
    for at, data in enumerate(cases()):
        data_path.write_bytes(data)
        for statistics in (False, True):
            oracle = expected(data, statistics)
            for mode, program in enumerate(programs['application']):
                actual = execute([program, data_path, *(['stats'] if statistics else [])], output,
                                 f'case-{at}-{int(statistics)}-{mode}', environment)
                assert actual == oracle, (at, statistics, mode, actual, oracle)
                observations.append((hashlib.sha256(data).hexdigest(), statistics, mode, actual[0], hashlib.sha256(actual[1]).hexdigest()))
    for program in programs['application']:
        for arguments, oracle in [([], (64, b'error 2 at 0\n', b'')), ([output / 'missing.ns'], (66, b'error 1 at 0\n', b'')),
                                  ([data_path, 'median'], (64, b'error 2 at 0\n', b''))]:
            actual = execute([program, *arguments], output, f'invocation-{len(observations)}', environment)
            assert actual == oracle, (arguments, actual, oracle)
            observations.append(('invocation', str(arguments), actual[0]))
    controls = [(['step', '1023', '1024'], b'1 1024\n'), (['step', '1024', '1024'], b'0 1024\n'),
                (['step', str(MAXIMUM), '1024'], f'0 {MAXIMUM}\n'.encode()), (['step', '0', '1025'], b'0 0\n'),
                (['step', '-1', '1024'], b'0 -1\n'), (['step', '0', '0'], b'0 0\n'),
                (['median', '1024', '0'], b'0 1 0 0 0\n'), (['median', '1024', '1'], b'1 0 1 1 0\n'),
                (['median', '1024', '32'], b'1 0 16 17 496\n'), (['median', '1024', '33'], b'0 2 0 0 0\n'),
                (['median', '1', '2'], b'1 0 1 2 1\n'), (['median', '0', '2'], b'0 2 0 0 0\n')]
    for mode, program in enumerate(programs['boundary']):
        for at, (arguments, oracle) in enumerate(controls):
            actual = execute([program, *arguments], output, f'boundary-{mode}-{at}', environment)
            assert actual == (0, oracle, b''), (arguments, actual, oracle)
        for rows in ([], [(b'a', trial(b'b'), trial(b'c'))]):
            data = source(rows)
            data_path.write_bytes(data)
            size = len(expected(data)[1])
            for cap in (0, size - 1, size, size + 1, 65536, 65537):
                actual = execute([program, 'report', data_path, str(cap), '32', 'default'], output, f'output-{mode}-{len(rows)}-{cap}', environment)
                oracle = expected(data, report_maximum=cap)
                assert actual == oracle, (cap, actual, oracle)
            for cap in (-1, 0, 1, 32, 33):
                actual = execute([program, 'report', data_path, '65536', str(cap), 'default'], output, f'tasks-{mode}-{len(rows)}-{cap}', environment)
                assert actual == expected(data, maximum=cap), (cap, actual, expected(data, maximum=cap))
    if faults:
        data = source([(b'a', trial(b'b'), trial(b'c', elapsed=b'2'))])
        data_path.write_bytes(data)
        for mode, program in enumerate(programs['application']):
            complete = False
            for ordinal in range(1, 1025):
                actual = execute([program, data_path, 'stats'], output, f'fault-{mode}-{ordinal}', dict(environment, SLIM_ALLOC_FAIL_AT=str(ordinal)))
                if actual[0] == 71:
                    assert actual[1] == b'' and actual[2] == f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode(), (mode, ordinal, actual)
                else:
                    assert actual == expected(data, True), (mode, ordinal, actual)
                    observations.append(('complete_fault_ordinal', mode, ordinal))
                    complete = True
                    break
            assert complete, ('bounded fault campaign did not reach complete report', mode)
    return observations


def fixed_module(relative, name):
    spec = importlib.util.spec_from_file_location(name, ROOT / relative)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def verify_adapter(compiler, cc, output):
    """Real ledger APIs, supplied synthetic acceptance facts; transport only.

    These local fixture dispatch records are not model participant trials. The
    explicitly synthetic terminal facts establish no SLIM source correctness.
    """
    adapter = fixed_module('scripts/development-summary-input.py', 'summary_adapter_fixture')
    fixtures = fixed_module('benchmarks/development/test_evaluate.py', 'summary_transport_helpers')
    evaluator = fixtures.E
    folder = output / 'adapter-transport-fixture'
    folder.mkdir()
    corpus = fixtures.corpus(folder, production=True)
    captured = folder / 'freeze'
    with contextlib.redirect_stdout(io.StringIO()):
        evaluator.freeze(SimpleNamespace(corpus=corpus, compiler=compiler, cc=cc, destination=captured))
    proof = folder / 'synthetic-preparation-fixture.json'
    evaluator.write_json(proof, {'passed': True, 'freeze_sha256': sha(captured / 'freeze.json'),
        'scope': 'synthetic preparation gate for transport-only tests; no actual source acceptance or model trial claim'})
    rows = []
    for oracle, infrastructure in [(True, False), (False, False), (None, True)]:
        runs = folder / ('oracle-' + str(oracle))
        run = runs / '02-context'
        with contextlib.redirect_stdout(io.StringIO()):
            evaluator.prepare(SimpleNamespace(freeze=captured, verification=proof, trial='02-context', destination=run))
            evaluator.start(SimpleNamespace(run=run))
            evaluator.finish(SimpleNamespace(run=run, reason='submitted'))
            with patch.object(evaluator, 'acceptance', return_value={
                    'oracle_accepted': oracle, 'infrastructure_failure': infrastructure, 'components': []}):
                evaluator.evaluate(SimpleNamespace(run=run))
        framed, receipt = adapter.convert(captured, runs)
        _, pairs, _ = parse(framed)
        assert len(pairs) == 1 and pairs[0]['baseline']['outcome'] == b'not-run'
        assert pairs[0]['context']['oracle'] == (b'unknown' if oracle is None else b'true' if oracle else b'false')
        assert pairs[0]['context']['outcome'] == (b'infrastructure-failure' if infrastructure else b'accepted' if oracle else b'correctness-failure')
        rows.append({'supplied_synthetic_oracle': oracle, 'supplied_synthetic_infrastructure': infrastructure,
                     'input_sha256': receipt['input_sha256'], 'evidence': 'transport only; acceptance was explicitly supplied by fixture'})
        mutations = [run / 'result.json', run / 'ledger.jsonl', run / 'prepared.json',
                     run / 'submitted/lib.slim', captured / 'freeze.json', captured / 'corpus/evaluate.py']
        for path in mutations:
            original = path.read_bytes()
            path.chmod(0o644)
            path.write_bytes(original + b' ')
            rejected = False
            try:
                adapter.convert(captured, runs)
            except (OSError, ValueError, KeyError, TypeError):
                rejected = True
            finally:
                path.write_bytes(original)
            assert rejected, ('adapter accepted changed identity', path)
        framed_again, _ = adapter.convert(captured, runs)
        assert framed_again == framed
    for stage in ('undispatched', 'dispatched', 'submitted', 'evaluation-intent'):
        runs = folder / stage
        run = runs / '02-context'
        with contextlib.redirect_stdout(io.StringIO()):
            evaluator.prepare(SimpleNamespace(freeze=captured, verification=proof, trial='02-context', destination=run))
            if stage != 'undispatched':
                evaluator.start(SimpleNamespace(run=run))
            if stage in ('submitted', 'evaluation-intent'):
                evaluator.finish(SimpleNamespace(run=run, reason='submitted'))
            if stage == 'evaluation-intent':
                evaluator.evaluation_intent(run)
                evaluator.write_json(run / 'result.json', {'orphan': 'not an authoritative terminal result'})
        framed, receipt = adapter.convert(captured, runs)
        _, pairs, _ = parse(framed)
        assert pairs[0]['context']['outcome'] == (b'not-run' if stage == 'undispatched' else b'unresolved')
        assert pairs[0]['context']['oracle'] == b'unknown'
        report = expected(framed, True)[1]
        assert b'stats context bounded maximum 32 work 0 cap 1024 lower unknown upper unknown reason empty\n' in report
        rows.append({'lifecycle': stage, 'input_sha256': receipt['input_sha256'],
                     'evidence': 'real ledger lifecycle; no participant/model/acceptance execution'})
    original_metadata = (captured / 'freeze.json').read_bytes()
    changed_metadata = json.loads(original_metadata)
    changed_metadata['created'] = '2026-10-02T00:00:00.000000Z'
    original_summary = evaluator.summarize
    def replacement_barrier(arguments):
        # Valid independent freeze metadata, all-not-run ledger set; old code
        # could combine its initial metadata with this later summary identity.
        (captured / 'freeze.json').chmod(0o644)
        (captured / 'freeze.json').write_bytes(evaluator.encoded(changed_metadata) + b'\n')
        evaluator.frozen(captured)
        return original_summary(arguments)
    rejected = False
    try:
        with patch.object(adapter, 'evaluator', return_value=evaluator), patch.object(evaluator, 'summarize', side_effect=replacement_barrier):
            adapter.convert(captured, folder / 'all-not-run-replacement')
    except ValueError:
        rejected = True
    finally:
        (captured / 'freeze.json').write_bytes(original_metadata)
    assert rejected, 'adapter accepted a valid freeze replacement during an all-not-run summary'
    rows.append({'barrier': 'valid-freeze-replacement-during-all-not-run-summary', 'rejected': True})
    copied_source = folder / 'adapter-copy.py'
    copied_bytes = (ROOT / 'scripts/development-summary-input.py').read_bytes()
    copied_source.write_bytes(copied_bytes)
    copied_adapter = fixed_module(copied_source, 'summary_adapter_self_identity_fixture')
    copied_adapter.EVALUATOR = adapter.EVALUATOR
    original_observations = copied_adapter.observations
    def adapter_replacement_barrier(*arguments):
        result = original_observations(*arguments)
        copied_source.write_bytes(copied_bytes + b'\n# copied source replacement barrier\n')
        return result
    rejected = False
    try:
        with patch.object(copied_adapter, 'observations', side_effect=adapter_replacement_barrier):
            copied_adapter.convert(captured, folder / 'all-not-run-adapter-replacement')
    except ValueError:
        rejected = True
    finally:
        copied_source.write_bytes(copied_bytes)
    assert rejected, 'adapter accepted changed observed adapter source bytes during conversion'
    _, copied_receipt = copied_adapter.convert(captured, folder / 'all-not-run-adapter-replacement')
    assert copied_receipt['adapter_sha256'] == sha(copied_source)
    rows.append({'barrier': 'copied-adapter-source-replacement-during-observation', 'rejected': True,
                 'scope': 'observed source bytes over conversion; no loaded bytecode or ABA attestation'})
    (folder / 'receipt.json').write_text(json.dumps({'schema': 1, 'scope': 'isolated transport fixtures only', 'rows': rows}, indent=2) + '\n')
    return len(rows)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', type=Path, default=ROOT / 'build/toolchain/slimc')
    parser.add_argument('--cc', default='cc')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/development-summary')
    parser.add_argument('--no-faults', action='store_true')
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    sources = [*sorted((ROOT / 'library/components').glob('development_summary*.slim')),
               ROOT / 'library/applications/development_summary/main.slim', ROOT / 'library/tests/development_summary.slim',
               ROOT / 'library/applications/development_summary/fixtures/observations.ns',
               ROOT / 'library/applications/development_summary/fixtures/observations.expected',
               ROOT / 'library/development-summary.project', ROOT / 'library/development-summary-tests.project',
               *sorted((ROOT / 'library/experimental').glob('*.slim')), ROOT / 'library/components/records.slim',
               ROOT / 'runtime/slim_rt.c', ROOT / 'runtime/slim_rt.h', args.compiler.resolve(), Path(__file__)]
    sources += [ROOT / 'scripts/development-summary-input.py', ROOT / 'benchmarks/development/evaluate.py',
                ROOT / 'benchmarks/development/test_evaluate.py', ROOT / 'design/rfcs/0164-bounded-development-summary.md']
    identities = {str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path): sha(path) for path in sources}
    environment = {key: value for key, value in os.environ.items() if key != 'SLIM_ALLOC_FAIL_AT'}
    programs = build(args.compiler.resolve(), args.cc, args.output, environment)
    observations = verify(programs, args.output, environment, not args.no_faults)
    adapter_cases = verify_adapter(args.compiler.resolve(), args.cc, args.output)
    assert identities == {str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path): sha(path) for path in sources}, 'verification input changed'
    receipt = {'schema': 1, 'identities': identities, 'observations': len(observations),
               'observations_sha256': hashlib.sha256(json.dumps(observations, sort_keys=True).encode()).hexdigest(),
               'cases': len(cases()), 'native_variants': ['ordinary', 'address+undefined sanitized'],
               'adapter_transport_cases': adapter_cases,
               'bounds': {'tasks': 32, 'trials': 64, 'source_bytes': 131072, 'report_bytes': 65536, 'stats_work_per_condition': 1024},
               'classification': 'exact finite outputs and diagnostics, bounded native fault/work controls',
               'timing': 'uncoordinated verification elapsed, no comparative performance claim'}
    (args.output / 'receipt.json').write_text(json.dumps(receipt, indent=2, sort_keys=True) + '\n')
    print(json.dumps({'cases': receipt['cases'], 'observations': len(observations), 'receipt': str(args.output / 'receipt.json')}))


if __name__ == '__main__':
    main()
