"""Independent RFC0166 byte/report oracle for predeclared valid transport data.

This module models observation aggregation only. It neither parses SLIM nor
establishes evaluator/source/outcome authority. Author this before the ordinary
consumer serializer; keep the sealed original source identity in local evidence.
"""
from dataclasses import dataclass

MAX_I64 = (1 << 63) - 1
INPUT_FORMAT = b'slim-development-operation-cost-1'
REPORT_FORMAT = b'slim-development-operation-cost-report-1'
CONDITIONS = ('baseline', 'context')
OPERATIONS = ('check', 'interfaces', 'context', 'build', 'run')
PHASES = ('check', 'interfaces', 'context', 'emit-c', 'native-compile', 'execute')
REQUEST_STATUSES = ('ok', 'compiler-error', 'timeout', 'output-limit', 'resource-limit',
                    'infrastructure-error', 'constraint-error', 'denied', 'interrupted', 'unfinished')
COMMAND_STATUSES = REQUEST_STATUSES[:6]
REQUEST_METRICS = ('capture-ns', 'queue-ns', 'wrapper-ns', 'feedback-bytes')
COMMAND_METRICS = ('command-ns', 'executable-ns', 'stdout-bytes', 'stderr-bytes', 'generated-c-bytes')
MONOTONIC = 'monotonic-launcher-to-direct-reap'
SYNTHETIC_COUNTERS = (299, 11, 11, 297, 618)


@dataclass(frozen=True)
class Identities:
    freeze: str
    compiler: str
    evaluator: str
    manifest: str


@dataclass(frozen=True)
class Command:
    phase: str
    status: str = 'ok'
    elapsed: int | None = None
    executable_elapsed: int | None = None
    stdout: int | None = None
    stderr: int | None = None
    role: str | None = None
    executable: str | None = None
    basis: str = 'unknown'


@dataclass(frozen=True)
class Request:
    uuid: str
    sequence: int
    operation: str
    started: bool
    status: str
    capture: int | None = None
    queue: int | None = None
    wrapper: int | None = None
    feedback: int | None = None
    source: str | None = None
    expected: str | None = None
    receipt: str | None = None
    commands: tuple = ()


@dataclass(frozen=True)
class Trial:
    task: bytes
    condition: str
    id: bytes
    outcome: str
    lifecycle: str
    ledger: str | None
    result: str | None
    prepared: str | None
    known_count: bool = True
    requests: tuple = ()


def scalar(value):
    if value is None:
        return b''
    if type(value) is bytes:
        return value
    if type(value) is bool:
        return b'true' if value else b'false'
    return str(value).encode('ascii')


def frame(value):
    value = scalar(value)
    return str(len(value)).encode('ascii') + b':' + value + b','


def frames(values):
    return b''.join(frame(value) for value in values)


def identities_payload(identities):
    return frames((identities.compiler, identities.evaluator, identities.manifest))


def input_bytes(identities, trials):
    chunks = [frames((INPUT_FORMAT, identities.freeze, identities_payload(identities)))]
    for trial in trials:
        metadata = frames((trial.id, trial.outcome, trial.lifecycle, trial.ledger, trial.result,
                           trial.prepared, len(trial.requests) if trial.known_count else None))
        chunks.append(frames((trial.task, trial.condition, metadata)))
        for request in trial.requests:
            controls = frames((request.sequence, request.operation, request.started, request.status,
                               request.capture, request.queue, request.wrapper, request.feedback,
                               request.source, request.expected, request.receipt))
            commands = b''.join(frames((command.phase, command.status, command.elapsed,
                                        command.executable_elapsed, command.stdout, command.stderr,
                                        command.role, command.executable, command.basis))
                                for command in request.commands)
            chunks.append(frames((request.uuid, controls, commands)))
    return b''.join(chunks)


def counters(trials):
    requests = sum(len(trial.requests) for trial in trials)
    commands = sum(len(request.commands) for trial in trials for request in trial.requests)
    parsed = 6 + len(trials) * 10 + requests * 14 + commands * 9
    updates = requests * 12 + commands * 15
    return parsed, requests, commands, updates, parsed + requests + commands + updates


def request_rows(trials, operation=None):
    return [(trial, request) for trial in trials for request in trial.requests
            if operation is None or request.operation == operation]


def command_rows(trials, phase=None):
    return [(trial, request, ordinal, command) for trial in trials for request in trial.requests
            for ordinal, command in enumerate(request.commands, 1)
            if phase is None or command.phase == phase]


def provenance(trial, request, ordinal):
    return (trial.id, request.sequence, request.uuid, ordinal)


def tie_key(provenance):
    trial, sequence, uuid, ordinal = provenance
    return sequence, uuid.encode('ascii'), ordinal, trial


def metric(name, observations):
    """Observations are eligible (optional value, exact provenance) operands.

    Unbounded Python integer addition supplies an independent overflow oracle;
    no SLIM sum or implementation-side update algorithm is reproduced here.
    """
    known = [(value, origin) for value, origin in observations if value is not None]
    total = sum(value for value, origin in known)
    if not known:
        sum_value, sum_reason, maximum, maximum_reason = None, 'empty', None, 'empty'
        origin = (None, None, None, None)
    else:
        sum_value, sum_reason = (None, 'overflow') if total > MAX_I64 else (total, 'exact')
        maximum = max(value for value, origin in known)
        origin = min((origin for value, origin in known if value == maximum), key=tie_key)
        maximum_reason = 'exact'
    return frames((name, len(observations), len(known), len(observations) - len(known),
                   sum_value, sum_reason, maximum, maximum_reason, frames(origin)))


def request_coverage(trials, rows):
    return (len(trials), sum(not trial.known_count for trial in trials), len(rows),
            sum(request.started for trial, request in rows),
            sum(request.status != 'unfinished' for trial, request in rows),
            sum(not request.started and request.status != 'unfinished' for trial, request in rows),
            sum(request.status == 'unfinished' for trial, request in rows),
            sum(request.receipt is not None for trial, request in rows),
            sum(request.started and request.receipt is None for trial, request in rows))


def command_coverage(trials, requests, rows, phase=False):
    emitted = sum(command.phase == 'emit-c' for trial, request, ordinal, command in rows)
    role_missing = sum(command.phase == 'emit-c' and command.role is None
                       for trial, request, ordinal, command in rows)
    tail = (len(rows), emitted, len(rows) - emitted, role_missing)
    if phase:
        return tail
    unknown = sum(not trial.known_count for trial in trials)
    linked = sum(request.receipt is not None for trial, request in requests)
    unassigned = sum(request.started and request.receipt is None for trial, request in requests)
    return (len(trials), unknown, linked, unassigned, len(rows),
            None if unknown or unassigned else len(rows), *tail[1:])


def request_data(trials, rows, outcome='', lifecycle=''):
    values = []
    for name, attribute in zip(REQUEST_METRICS, ('capture', 'queue', 'wrapper', 'feedback')):
        values.append(metric(name, [(getattr(request, attribute), provenance(trial, request, 0))
                                    for trial, request in rows]))
    statuses = frames(sum(request.status == status for trial, request in rows)
                      for status in REQUEST_STATUSES)
    return frames((outcome, lifecycle, frames(request_coverage(trials, rows)), statuses, frames(values)))


def command_data(trials, requests, rows, outcome='', lifecycle='', phase=False):
    values = []
    for name, attribute in zip(COMMAND_METRICS[:4], ('elapsed', 'executable_elapsed', 'stdout', 'stderr')):
        values.append(metric(name, [(getattr(command, attribute), provenance(trial, request, ordinal))
                                    for trial, request, ordinal, command in rows]))
    generated = [(command.stdout if command.role == 'generated-c' else None,
                  provenance(trial, request, ordinal))
                 for trial, request, ordinal, command in rows if command.phase == 'emit-c']
    values.append(metric('generated-c-bytes', generated))
    statuses = frames(sum(command.status == status for trial, request, ordinal, command in rows)
                      for status in COMMAND_STATUSES)
    return frames((outcome, lifecycle, frames(command_coverage(trials, requests, rows, phase)),
                   statuses, frames(values)))


def report_bytes(identities, trials):
    chunks = [frames((REPORT_FORMAT, identities.freeze, identities_payload(identities)))]

    def group(kind, trial, selector, condition, data):
        chunks.append(frames((kind, frames((trial, selector, condition)), data)))

    for trial in trials:
        requests = request_rows((trial,))
        commands = command_rows((trial,))
        group('trial-request', trial.id, trial.task, trial.condition,
              request_data((trial,), requests, trial.outcome, trial.lifecycle))
        group('trial-command', trial.id, trial.task, trial.condition,
              command_data((trial,), requests, commands, trial.outcome, trial.lifecycle))
    for condition in CONDITIONS:
        selected = tuple(trial for trial in trials if trial.condition == condition)
        requests = request_rows(selected)
        group('condition-request', b'', b'', condition, request_data(selected, requests))
        group('condition-command', b'', b'', condition,
              command_data(selected, requests, command_rows(selected)))
    for condition in CONDITIONS:
        selected = tuple(trial for trial in trials if trial.condition == condition)
        for operation in OPERATIONS:
            group('operation-request', b'', operation, condition,
                  request_data(selected, request_rows(selected, operation)))
    for condition in CONDITIONS:
        selected = tuple(trial for trial in trials if trial.condition == condition)
        for phase in PHASES:
            group('phase-command', b'', phase, condition,
                  command_data(selected, (), command_rows(selected, phase), phase=True))
    chunks.append(frames(('work', frames(counters(trials)), frames(('unknown',) * 6))))
    return b''.join(chunks)


def request_uuid(value):
    return f'00000000-0000-0000-0000-{value:012x}'


def synthetic():
    """Frozen fixture mixes observations, roles, unknown domains and overflow."""
    ids = Identities('a' * 64, 'b' * 64, 'c' * 64, 'd' * 64)
    source, expected, receipt = 'e' * 64, 'f' * 64, '1' * 64
    baseline = (
        Request(request_uuid(1), 7, 'check', True, 'ok', 0, None, 13, 35, source,
                receipt=receipt, commands=(Command('check', elapsed=7, stdout=4, stderr=0, basis=MONOTONIC),)),
        Request(request_uuid(2), 3, 'interfaces', True, 'compiler-error', 10, 1, 20, 40, source,
                receipt=receipt, commands=(Command('interfaces', 'compiler-error', 15, 5, 0, 12, executable=expected, basis=MONOTONIC),)),
        Request(request_uuid(3), 3, 'build', True, 'ok', MAX_I64, 9, 30, 50, source,
                receipt=receipt, commands=(Command('emit-c', elapsed=8, executable_elapsed=7, stdout=123, stderr=0, role='generated-c', executable=expected, basis=MONOTONIC),
                                          Command('native-compile', elapsed=6, stdout=0, stderr=0, basis=MONOTONIC))),
        Request(request_uuid(4), 1, 'run', True, 'timeout', 1, MAX_I64, 31, 60, source,
                receipt=receipt, commands=(Command('emit-c', elapsed=8, stdout=90, stderr=0, basis=MONOTONIC),
                                          Command('native-compile', elapsed=8, stdout=0, stderr=0, basis=MONOTONIC),
                                          Command('execute', 'timeout', 20, 19, 7, 1, executable=expected, basis=MONOTONIC))),
        Request(request_uuid(5), 8, 'check', False, 'denied', wrapper=0),
    )
    context = (
        Request(request_uuid(1), 4, 'context', True, 'ok', 0, 0, 0, 0, source, expected, receipt,
                (Command('context', elapsed=0, executable_elapsed=0, stdout=0, stderr=0, executable=expected, basis=MONOTONIC),)),
        Request(request_uuid(2), 2, 'check', True, 'infrastructure-error', queue=0, wrapper=MAX_I64,
                feedback=MAX_I64, source=source),
        Request(request_uuid(3), 0, 'check', False, 'unfinished'),
        Request(request_uuid(4), 1, 'build', True, 'compiler-error', source=source, receipt=receipt,
                commands=(Command('emit-c', 'compiler-error', stdout=12, stderr=8),)),
        Request(request_uuid(5), 6, 'run', True, 'resource-limit', source=source, receipt=receipt,
                commands=(Command('emit-c', stdout=MAX_I64, stderr=0, role='generated-c'),
                          Command('native-compile', 'resource-limit'))),
        Request(request_uuid(6), 5, 'interfaces', False, 'constraint-error', wrapper=15, feedback=0),
    )
    trials = (
        Trial(b'alpha', 'baseline', b'z-baseline', 'accepted', '', '2' * 64, '3' * 64, '4' * 64, requests=baseline),
        Trial(b'alpha', 'context', b'y-context', 'unresolved', 'dispatched-unsubmitted', '5' * 64, None, '6' * 64, requests=context),
        Trial(b'beta', 'baseline', b'b-unprepared', 'not-run', 'unprepared', None, None, None, known_count=False),
        Trial(b'beta', 'context', b'a-prepared', 'not-run', 'prepared-undispatched', '7' * 64, None, '8' * 64),
    )
    return ids, trials
