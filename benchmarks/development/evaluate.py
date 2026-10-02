#!/usr/bin/env python3
"""Protocol-2 bounded transport. The production compiler alone checks SLIM."""
import argparse
from contextlib import contextmanager
from datetime import datetime, timezone
import fcntl
import hashlib
import json
import math
import os
from pathlib import Path
import platform
import re
import resource
import selectors
import shutil
import signal
import subprocess
import sys
import time
import uuid

sys.dont_write_bytecode = True
BASE = Path(__file__).resolve().parent
ROOT = BASE.parents[1]
PUBLIC = ['DESIGN.md', 'docs/CORE.md', 'design/FEATURE_POLICY.md',
          'docs/PROJECTS.md', 'docs/HOST.md', 'docs/DIAGNOSTICS.md', 'docs/CONTEXT.md']
OPERATIONS = {'check', 'interfaces', 'context', 'build', 'run'}
MAX_JSON = 1048576
MAX_LINE = 65536
MAX_LEDGER = 16777216
MAX_RECORDS = 4096
MAX_FILE = 4194304
MAX_SOURCE = 16777216
MAX_CORPUS_ENTRIES = 4096
MAX_CORPUS_BYTES = 67108864
MAX_OUTPUT = 2097152
MAX_GENERATED = 16777216
MAX_PROCESS_FILE = 134217728
ZERO = '0' * 64


class Constraint(ValueError):
    pass


def require(value, reason, error=ValueError):
    if not value:
        raise error(reason)


def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds').replace('+00:00', 'Z')


def digest(data):
    return hashlib.sha256(data).hexdigest()


def encoded(value):
    return json.dumps(value, sort_keys=True, separators=(',', ':'), ensure_ascii=True).encode('ascii')


def pairs(items):
    result = {}
    for key, value in items:
        require(key not in result, 'duplicate JSON key')
        result[key] = value
    return result


def invalid_number(value):
    raise ValueError('nonfinite JSON number')


def load_bytes(data):
    require(len(data) <= MAX_JSON, 'JSON document limit')
    return json.loads(data, object_pairs_hook=pairs, parse_constant=invalid_number)


def read(path, cap=MAX_FILE, error=ValueError):
    path = Path(path)
    require(not path.is_symlink() and path.is_file(), 'ordinary file required', error)
    fd = os.open(path, os.O_RDONLY | getattr(os, 'O_NOFOLLOW', 0))
    with os.fdopen(fd, 'rb') as stream:
        data = stream.read(cap + 1)
    require(len(data) <= cap, 'file byte limit', error)
    return data


def document(path):
    return load_bytes(read(path, MAX_JSON))


def recorded_commands(results):
    return [{key: value for key, value in result.items() if key not in {'stdout', 'stderr'}}
            for result in results]


def write_json(path, data, immutable=False):
    value = encoded(data) + b'\n'
    require(len(value) <= MAX_JSON, 'JSON document limit')
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(value)
        stream.flush()
        os.fsync(stream.fileno())
    if immutable:
        path.chmod(0o444)


def name(value):
    require(isinstance(value, str) and re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9_-]{0,95}', value), 'invalid identifier')
    return value


def relative(value):
    require(isinstance(value, str) and 0 < len(value) <= 1024, 'invalid relative path')
    path = Path(value)
    require(not path.is_absolute() and '..' not in path.parts and '.' not in path.parts
            and '\\' not in value and str(path) == value, 'unsafe relative path')
    return path


def bounded_int(value, low, high, reason):
    require(type(value) is int and low <= value <= high, reason)
    return value


def boot():
    linux = Path('/proc/sys/kernel/random/boot_id')
    if linux.is_file():
        identity = linux.read_text().strip()
    else:
        try:
            result = subprocess.run(['/usr/sbin/sysctl', '-n', 'kern.boottime'],
                                    capture_output=True, timeout=5, check=True)
            identity = result.stdout.decode('ascii').strip()
        except (OSError, subprocess.SubprocessError):
            return {'host': platform.node(), 'identity': None,
                    'evidence': 'unknown-kernel-boot-identity-unavailable'}
    return {'host': platform.node(), 'identity': digest(identity.encode()), 'evidence': 'observed-kernel-boot-identity'}


def capture(source, files, error=ValueError):
    source = Path(source)
    require(not source.is_symlink() and source.is_dir(), 'ordinary source directory required', error)
    values, total = {}, 0
    for filename in files:
        path = relative(filename)
        for ancestor in path.parents:
            require(not (source / ancestor).is_symlink(), 'symlink source directory', error)
        try:
            data = read(source / path, error=error)
        except OSError as failure:
            raise error(type(failure).__name__ + ': ' + str(failure)) from failure
        total += len(data)
        require(total <= MAX_SOURCE, 'complete source byte limit', error)
        values[filename] = data
    return values


def identity(values):
    hashes = {key: digest(value) for key, value in sorted(values.items())}
    return {'files': hashes, 'sha256': digest(encoded(hashes)),
            'bytes': sum(len(value) for value in values.values())}


def install(values, destination):
    destination = Path(destination)
    destination.mkdir(parents=True, exist_ok=False)
    for filename, data in values.items():
        path = destination / relative(filename)
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        path.chmod(0o444)
    return identity(values)


def tree(directory):
    directory = Path(directory)
    require(not directory.is_symlink() and directory.is_dir(), 'ordinary corpus directory required')
    values, pending, visited, total = {}, [directory], 0, 0
    while pending:
        parent = pending.pop()
        # scandir yields incrementally; giant flat directories and empty directory
        # trees cross the same entry bound before sorting or retaining contents.
        with os.scandir(parent) as entries:
            for entry in entries:
                visited += 1
                require(visited <= MAX_CORPUS_ENTRIES, 'corpus entry bound')
                require(not entry.is_symlink(), 'symlink corpus material')
                if entry.name == '__pycache__':
                    continue
                path = Path(entry.path)
                if entry.is_dir(follow_symlinks=False):
                    pending.append(path)
                else:
                    require(entry.is_file(follow_symlinks=False), 'ordinary corpus material required')
                    data = read(path, min(MAX_FILE, MAX_CORPUS_BYTES - total))
                    total += len(data)
                    require(total <= MAX_CORPUS_BYTES, 'corpus byte bound')
                    values[str(path.relative_to(directory))] = data
    return dict(sorted(values.items()))


def manifest(corpus):
    result = document(Path(corpus) / 'manifest.json')
    require(set(result) == {'schema', 'model', 'reasoning_effort', 'budgets', 'tasks', 'trials'}, 'manifest fields')
    require(result['schema'] == 2 and result['model'] == 'gpt-6.1-sol'
            and result['reasoning_effort'] == 'xhigh', 'protocol-2 model configuration')
    budgets = result['budgets']
    require(set(budgets) == {'elapsed_seconds', 'operations', 'process_seconds'}, 'budget fields')
    bounded_int(budgets['elapsed_seconds'], 1, 7200, 'elapsed budget')
    bounded_int(budgets['operations'], 1, 128, 'operation budget')
    bounded_int(budgets['process_seconds'], 1, 60, 'process budget')
    require(isinstance(result['tasks'], list) and 1 <= len(result['tasks']) <= 32, 'task count')
    tasks = {}
    for task in result['tasks']:
        required = {'id', 'files', 'editable', 'entry', 'initial_check'}
        require(required <= set(task) <= required | {'base'}, 'task fields')
        name(task['id'])
        require(task['id'] not in tasks, 'duplicate task')
        require(isinstance(task['files'], list) and 1 <= len(task['files']) <= 128
                and len(set(task['files'])) == len(task['files']), 'task file list')
        for filename in task['files']:
            relative(filename)
        require(isinstance(task.get('base', {}), dict) and set(task.get('base', {})) <= set(task['files']), 'base file mapping')
        for filename in task.get('base', {}).values():
            relative(filename)
            require(filename.endswith(('.slim', '.project')), 'base must be source data')
        require(isinstance(task['editable'], list) and bool(task['editable'])
                and set(task['editable']) <= set(task['files']), 'editable files')
        require(task['entry'] in task['files'] and task['entry'].endswith('.project')
                and task['entry'] not in task['editable'], 'fixed project entry required')
        require(task['initial_check'] in {'accepted', 'rejected'}, 'initial check expectation')
        tasks[task['id']] = task
    require(isinstance(result['trials'], list) and 1 <= len(result['trials']) <= 64, 'trial count')
    ids = set()
    for trial in result['trials']:
        require(set(trial) == {'id', 'task', 'condition'}, 'trial fields')
        name(trial['id'])
        require(trial['id'] not in ids and trial['task'] in tasks
                and trial['condition'] in {'baseline', 'context'}, 'invalid trial')
        ids.add(trial['id'])
    for task_id in tasks:
        conditions = [trial['condition'] for trial in result['trials'] if trial['task'] == task_id]
        require(sorted(conditions) == ['baseline', 'context'], 'exactly one paired trial per task')
    return result


def oracle(corpus, task):
    task_dir = Path(corpus) / 'tasks' / task['id']
    data = document(task_dir / 'acceptance.json')
    require(set(data) == {'schema', 'components'} and data['schema'] == 2, 'acceptance fields')
    require(isinstance(data['components'], list) and 1 <= len(data['components']) <= 128, 'acceptance count')
    ids = set()
    for component in data['components']:
        require(set(component) <= {'id', 'operation', 'overlay', 'expect', 'domain', 'positive'}
                and {'id', 'operation', 'overlay', 'expect', 'domain'} <= set(component), 'component fields')
        name(component['id'])
        require(component['id'] not in ids, 'duplicate component')
        ids.add(component['id'])
        require(component['operation'] in {'check', 'interfaces', 'run'}, 'acceptance operation enum')
        require(isinstance(component['domain'], str) and 0 < len(component['domain']) <= 4096, 'bounded domain required')
        overlays = [component['overlay']]
        if 'positive' in component:
            require(component['operation'] == 'check', 'positive control only for check probes')
            overlays.append(component['positive'])
        for overlay in overlays:
            require(isinstance(overlay, dict) and set(overlay) <= set(task['editable']), 'overlay file whitelist')
            for filename in overlay.values():
                read(task_dir / relative(filename))
        expected = component['expect']
        require(isinstance(expected, dict) and set(expected) <= {'returncode', 'stdout', 'stderr', 'diagnostics'}
                and 'returncode' in expected, 'expectation fields')
        bounded_int(expected['returncode'], 0, 255, 'expected return code')
        for field in ('stdout', 'stderr'):
            if field in expected:
                require(isinstance(expected[field], str), 'expected output must be text')
                read(task_dir / relative(expected[field]), MAX_OUTPUT)
        if 'diagnostics' in expected:
            require(isinstance(expected['diagnostics'], list), 'diagnostic list required')
            for diagnostic in expected['diagnostics']:
                require(isinstance(diagnostic, list) and len(diagnostic) == 4
                        and re.fullmatch(r'E[0-9]{4}', diagnostic[0])
                        and isinstance(diagnostic[1], str) and diagnostic[1].isascii()
                        and re.fullmatch(r'[^@\s]{1,1024}', diagnostic[1]), 'diagnostic identity')
                bounded_int(diagnostic[2], 0, MAX_SOURCE, 'diagnostic offset')
                bounded_int(diagnostic[3], diagnostic[2], MAX_SOURCE, 'diagnostic offset')
        if expected['returncode'] != 0:
            require(component['operation'] == 'check' and 'positive' in component
                    and 'diagnostics' in expected, 'negative probe requires positive control and exact diagnostic')
    return data['components']


@contextmanager
def lock(path):
    with Path(path).open('a+b') as stream:
        fcntl.flock(stream, fcntl.LOCK_EX)
        try:
            yield stream
        finally:
            fcntl.flock(stream, fcntl.LOCK_UN)


def replay(stream):
    stream.seek(0)
    state = {'requests': {}, 'dispatch': None, 'submission_intent': None, 'submission': None, 'evaluation_intent': None, 'evaluation': None,
             'admitted_operations': 0, 'active_request': None, 'notes': [], 'records': [], 'previous': ZERO}
    total = 0
    for index, raw in enumerate(iter(lambda: stream.readline(MAX_LINE + 1), b'')):
        total += len(raw)
        require(len(raw) <= MAX_LINE and raw.endswith(b'\n') and total <= MAX_LEDGER
                and index < MAX_RECORDS, 'ledger framing/bound')
        record = load_bytes(raw)
        require(isinstance(record, dict) and set(record) ==
                {'schema', 'seq', 'kind', 'id', 'utc', 'mono_ns', 'data', 'prev', 'sha256'}, 'ledger fields')
        require(record['schema'] == 2 and type(record['seq']) is int and record['seq'] == index
                and record['prev'] == state['previous'], 'ledger sequence/chain')
        require(record['sha256'] == digest(encoded({key: value for key, value in record.items() if key != 'sha256'})), 'ledger digest')
        require(str(uuid.UUID(record['id'])) == record['id'], 'record UUID')
        require(type(record['mono_ns']) is int and record['mono_ns'] >= 0
                and isinstance(record['utc'], str) and record['utc'].endswith('Z'), 'record time')
        datetime.fromisoformat(record['utc'].replace('Z', '+00:00'))
        require(isinstance(record['data'], dict), 'record data')
        kind, request_id, payload = record['kind'], record['id'], record['data']
        require(state['evaluation'] is None and (state['evaluation_intent'] is None or kind == 'evaluation'),
                'evaluation trial ledger is sealed')
        allowed = {
            'prepared': {'metadata_sha256'}, 'dispatch': {'boot', 'budget_seconds'},
            'request': {'operation', 'selector', 'source', 'expected'},
            'started': {'source_identity', 'expected_identity', 'capture_ns', 'queue_ns', 'quota_index'},
            'finished': {'status', 'reason', 'receipt', 'receipt_sha256', 'elapsed_ns', 'feedback_bytes'},
            'submission': {'reason', 'source_identity', 'capture_error', 'elapsed_ns'},
            'submission-intent': {'reason', 'receipt_utc', 'receipt_mono_ns', 'elapsed_ns'},
            'evaluation-intent': {'ledger_sha256'},
            'evaluation': {'outcome', 'result_sha256'}, 'note': {'category', 'reason'},
        }
        require(kind in allowed and set(payload) == allowed[kind], 'record data fields')
        if kind == 'prepared':
            require(index == 0, 'prepared must be first')
        else:
            require(index > 0, 'missing preparation')
        if kind == 'dispatch':
            require(state['dispatch'] is None and not state['requests'], 'duplicate/late dispatch')
            state['dispatch'] = record
        elif kind == 'request':
            require(request_id not in state['requests'] and payload['operation'] in OPERATIONS, 'duplicate/invalid request')
            state['requests'][request_id] = {'request': record, 'started': None, 'finished': None}
        elif kind in {'started', 'finished'}:
            require(request_id in state['requests'], 'unmatched request terminal')
            request = state['requests'][request_id]
            require(request['finished'] is None, 'duplicate terminal')
            if kind == 'started':
                cutoff = state['submission_intent']
                require(cutoff is None or request['request']['mono_ns'] < cutoff['mono_ns'], 'request received after submission cutoff')
                require(request['started'] is None and payload['quota_index'] ==
                        state['admitted_operations'] + 1,
                        'invalid admission index')
                require(state['active_request'] is None, 'overlapping execution')
                state['admitted_operations'] += 1
                state['active_request'] = request_id
            else:
                require(payload['status'] in {'ok', 'compiler-error', 'timeout', 'output-limit',
                        'infrastructure-error', 'resource-limit', 'constraint-error', 'denied', 'interrupted'}, 'terminal status')
                if request['started'] is not None:
                    require(state['active_request'] == request_id, 'active request mismatch')
                    state['active_request'] = None
            request[kind] = record
        elif kind == 'submission-intent':
            require(state['dispatch'] is not None and state['submission_intent'] is None, 'duplicate/unstarted submission intent')
            require(type(payload['receipt_mono_ns']) is int and 0 <= payload['receipt_mono_ns'] <= record['mono_ns']
                    and payload['elapsed_ns'] == record['mono_ns'] - state['dispatch']['mono_ns'], 'submission time contract')
            require(payload['reason'] in {'submitted', 'timeout', 'interrupted'}, 'submission reason')
            datetime.fromisoformat(payload['receipt_utc'].replace('Z', '+00:00'))
            state['submission_intent'] = record
        elif kind == 'submission':
            require(state['submission_intent'] is not None and state['submission'] is None, 'duplicate/unstarted submission capture')
            require(payload['elapsed_ns'] == state['submission_intent']['data']['elapsed_ns'], 'capture changed cutoff duration')
            state['submission'] = record
        elif kind == 'evaluation-intent':
            require(state['submission'] is not None and state['evaluation_intent'] is None
                    and all(r['finished'] is not None for r in state['requests'].values())
                    and payload['ledger_sha256'] == state['previous'], 'evaluation seal state')
            state['evaluation_intent'] = record
        elif kind == 'evaluation':
            require(state['evaluation_intent'] is not None and state['evaluation'] is None, 'evaluation state')
            state['evaluation'] = record
        elif kind == 'note':
            require(payload['category'] in {'constraint', 'human-intervention', 'observation'}, 'note category')
            state['notes'].append(record)
        state['records'].append(record)
        state['previous'] = record['sha256']
    return state


def ledger(run):
    with lock(Path(run) / 'ledger.jsonl') as stream:
        return replay(stream)


def validate_receipts(run, state):
    run = Path(run).resolve()
    for request in state['requests'].values():
        finished = request['finished']
        if finished is None or finished['data']['receipt'] is None:
            continue
        receipt = run / relative(finished['data']['receipt'])
        require(digest(read(receipt)) == finished['data']['receipt_sha256'], 'operation receipt changed')
        for command in document(receipt)['commands']:
            for field in ('stdout', 'stderr'):
                artifact = Path(command[field + '_artifact'])
                require(artifact.resolve().is_relative_to(run), 'process output outside trial')
                bounded_int(command['output_limit'], 1, MAX_GENERATED, 'recorded process output limit')
                data = read(artifact, command['output_limit'])
                require(len(data) == command[field + '_bytes'] and digest(data) == command[field + '_sha256'], 'process output changed')


def append_locked(stream, state, kind, payload, request_id=None, at=None, monotonic=None):
    record = {'schema': 2, 'seq': len(state['records']), 'kind': kind,
              'id': request_id or str(uuid.uuid4()), 'utc': at or utc(),
              'mono_ns': time.monotonic_ns() if monotonic is None else monotonic,
              'data': payload, 'prev': state['previous']}
    record['sha256'] = digest(encoded(record))
    raw = encoded(record) + b'\n'
    require(len(raw) <= MAX_LINE and len(state['records']) < MAX_RECORDS, 'ledger saturation')
    stream.seek(0, os.SEEK_END)
    require(stream.tell() + len(raw) <= MAX_LEDGER, 'ledger byte saturation')
    # Validate the proposed transition before any durable mutation.
    import io
    stream.seek(0)
    replay(io.BytesIO(stream.read() + raw))
    stream.seek(0, os.SEEK_END)
    stream.write(raw)
    stream.flush()
    os.fsync(stream.fileno())
    return record


def append(run, kind, payload, request_id=None, at=None, monotonic=None):
    with lock(Path(run) / 'ledger.jsonl') as stream:
        return append_locked(stream, replay(stream), kind, payload, request_id, at, monotonic)


def submission_intent(run, reason, receipt_utc, receipt_mono_ns):
    # Publication and its authoritative cutoff are atomic with ledger admission.
    # This lock never waits for an executing compiler/native operation.
    with lock(Path(run) / 'ledger.jsonl') as stream:
        state = replay(stream)
        require(state['dispatch'] is not None and state['submission_intent'] is None, 'trial cannot submit')
        cutoff_utc, cutoff_mono = utc(), time.monotonic_ns()
        payload = {'reason': reason, 'receipt_utc': receipt_utc, 'receipt_mono_ns': receipt_mono_ns,
                   'elapsed_ns': cutoff_mono - state['dispatch']['mono_ns']}
        return append_locked(stream, state, 'submission-intent', payload, at=cutoff_utc, monotonic=cutoff_mono)


def evaluation_intent(run):
    # Seal the exact completed request/note set in the same critical section.
    with lock(Path(run) / 'ledger.jsonl') as stream:
        state = replay(stream)
        require(state['submission'] is not None and state['evaluation_intent'] is None,
                'unsubmitted/already sealed trial')
        require(all(request['finished'] is not None for request in state['requests'].values()),
                'unfinished request; explicitly recover first')
        append_locked(stream, state, 'evaluation-intent', {'ledger_sha256': state['previous']})
        return replay(stream)


def process(argv, directory, seconds, environment, output_cap=MAX_OUTPUT, file_cap=MAX_PROCESS_FILE):
    """Drain bounded pipes; never run a shell or inherit participant environment."""
    directory = Path(directory)
    require(0 < seconds <= 60 and 0 < output_cap <= MAX_GENERATED and 0 < file_cap <= MAX_PROCESS_FILE, 'process bounds')
    directory.mkdir(parents=True, exist_ok=False)
    begin, reaped = None, None
    result = {'argv': list(map(str, argv)), 'started': None, 'status': 'ok',
              'returncode': None, 'seconds_limit': seconds, 'output_limit': output_cap,
              'file_limit': file_cap, 'cpu_seconds_limit': math.ceil(seconds) + 1,
              'timing_basis': 'monotonic-launcher-to-direct-reap'}
    stdout, stderr = bytearray(), bytearray()
    child, control_read, control_write = None, None, None
    try:
        control_read, control_write = os.pipe()
        helper = [sys.executable, str(Path(__file__).resolve()), '_exec', str(math.ceil(seconds) + 1),
                  str(file_cap), str(control_write), '--', *result['argv']]
        result['launcher_argv'] = helper
        executable = Path(result['argv'][0]).resolve()
        if executable.is_file():
            result['executable_sha256'] = digest(read(executable, MAX_PROCESS_FILE))
        result['started'], begin = utc(), time.monotonic_ns()
        child = subprocess.Popen(helper, cwd=directory, env=environment,
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE, start_new_session=True,
                                 pass_fds=(control_write,))
        os.close(control_write)
        control_write = None
        with selectors.DefaultSelector() as selector:
            for pipe, sink in ((child.stdout, stdout), (child.stderr, stderr)):
                os.set_blocking(pipe.fileno(), False)
                selector.register(pipe, selectors.EVENT_READ, sink)
            deadline = time.monotonic() + seconds
            while selector.get_map():
                if time.monotonic() >= deadline:
                    result['status'] = 'timeout'
                    break
                for key, _ in selector.select(min(0.05, max(0, deadline - time.monotonic()))):
                    chunk = os.read(key.fd, min(65536, output_cap - len(stdout) - len(stderr) + 1))
                    if not chunk:
                        selector.unregister(key.fileobj)
                    elif len(stdout) + len(stderr) + len(chunk) > output_cap:
                        result['status'] = 'output-limit'
                        break
                    else:
                        key.data.extend(chunk)
                if result['status'] != 'ok':
                    break
            if result['status'] == 'ok':
                try:
                    child.wait(timeout=max(0.001, deadline - time.monotonic()))
                    reaped = time.monotonic_ns()
                except subprocess.TimeoutExpired:
                    result['status'] = 'timeout'
        # Also kill descendants holding no pipe after their leader exits.
        try:
            os.killpg(child.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        child.wait()
        if reaped is None:
            reaped = time.monotonic_ns()
        result['returncode'] = child.returncode
        control = os.read(control_read, 8193)
        require(len(control) <= 8192, 'launcher control bound')
        launch = [load_bytes(line) for line in control.splitlines()]
        require(bool(launch), 'native launch evidence unavailable')
        result['launch_evidence'] = launch
        if launch[-1]['status'] == 'error':
            result.update(status='infrastructure-error', reason=launch[-1]['reason'])
        else:
            require(len(launch) == 1 and launch[0]['status'] == 'exec-ready', 'launcher control framing')
            result['executable_elapsed_ns'] = reaped - launch[0]['mono_ns']
        if 'executable_sha256' in result and digest(read(executable, MAX_PROCESS_FILE)) != result['executable_sha256']:
            result.update(status='infrastructure-error', reason='process executable changed during operation')
        if result['status'] == 'ok' and child.returncode != 0:
            result['status'] = 'compiler-error'
            result['failure_cause'] = 'unknown-native-nonzero-exit'
            if child.returncode in {-signal.SIGXCPU, -signal.SIGXFSZ}:
                result['status'] = 'resource-limit'
                result['failure_cause'] = 'observed-resource-signal'
    except (OSError, ValueError) as error:
        result.update(status='infrastructure-error', reason=type(error).__name__ + ': ' + str(error))
    finally:
        if child is not None:
            if child.poll() is None:
                try:
                    os.killpg(child.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
            child.wait()
            if reaped is None:
                reaped = time.monotonic_ns()
            child.stdout.close()
            child.stderr.close()
        result['elapsed_ns'] = ((reaped or time.monotonic_ns()) - begin) if begin is not None else None
        result['finished'] = utc()
        for fd in (control_read, control_write):
            if fd is not None:
                os.close(fd)
    for label, data in (('stdout', stdout), ('stderr', stderr)):
        (directory / (label + '.bin')).write_bytes(data)
        result[label + '_bytes'] = len(data)
        result[label + '_sha256'] = digest(data)
        result[label + '_artifact'] = str(directory / (label + '.bin'))
        result[label] = bytes(data).decode('latin1')
    return result


def ok(result):
    return result['status'] == 'ok' and result['returncode'] == 0


def phase(result, operation):
    result['phase'] = operation
    if result['status'] == 'compiler-error' and (operation == 'native-compile' or
                                                (operation != 'execute' and result['returncode'] < 0)):
        result.update(status='infrastructure-error',
                      reason='trusted tool/backend failed; cause unknown')
    return result


def commands(operation, source, expected, selector, captured, work, seconds):
    compiler, runtime = captured / 'toolchain/slimc', captured / 'runtime'
    configuration = document(captured / 'freeze.json')
    # Caller passes the fixed project path inside the operation's private snapshot.
    entry = source
    environment = configuration['environment']
    if operation in {'check', 'interfaces'}:
        result = process([compiler, operation, entry], work / 'check', seconds, environment)
        phase(result, operation)
        return [result]
    if operation == 'context':
        result = process([compiler, 'context', entry, expected, selector], work / 'context', seconds, environment)
        phase(result, 'context')
        return [result]
    emitted = process([compiler, entry], work / 'emit', seconds, environment, output_cap=MAX_GENERATED)
    results = [emitted]
    phase(emitted, 'emit-c')
    if not ok(emitted):
        return results
    generated = work / 'program.c'
    generated_bytes = emitted['stdout'].encode('latin1')
    structured_workers = generated_bytes.startswith(b'#define SLIM_PARALLEL 1\n')
    runtime_flags = ['-DSLIM_PARALLEL=1'] if structured_workers else []
    generated.write_bytes(generated_bytes)
    generated.chmod(0o444)
    emitted['stdout'] = ''
    emitted['output_role'] = 'generated-c'
    native_inputs = {str(path): digest(read(path, MAX_GENERATED))
                     for path in (generated, runtime / 'slim_rt.c', runtime / 'slim_rt.h')}
    native = process([configuration['cc']['path'], '-std=c11', '-O2', '-DNDEBUG',
                      *runtime_flags, '-Wall', '-Wextra', '-Werror', '-I', runtime, generated,
                      runtime / 'slim_rt.c', '-o', work / 'program'], work / 'native', seconds, environment)
    results.append(native)
    phase(native, 'native-compile')
    native['input_sha256'] = native_inputs
    native['runtime_configuration'] = {'structured_workers': structured_workers, 'platform_workers': False}
    require(all(digest(read(path, MAX_GENERATED)) == value for path, value in native_inputs.items()), 'native compilation input changed')
    if operation == 'run' and ok(native):
        execution = process([work / 'program'], work / 'execute', seconds, environment)
        execution['phase'] = 'execute'
        results.append(execution)
    return results


def frozen(path, check_self=True):
    path = Path(path).resolve()
    metadata = document(path / 'freeze.json')
    require(metadata['schema'] == 2, 'freeze schema')
    require(identity(tree(path / 'corpus')) == metadata['corpus'], 'frozen corpus identity changed')
    require(identity(tree(path / 'public-docs')) == metadata['public_docs'], 'frozen public docs identity changed')
    require(digest(read(path / 'toolchain/slimc', 134217728)) == metadata['compiler_sha256'], 'compiler identity changed')
    require(identity(tree(path / 'runtime')) == metadata['runtime'], 'runtime identity changed')
    require(identity(tree(path / 'base-data')) == metadata['base_data'], 'frozen base data changed')
    require(digest(read(metadata['cc']['path'], 134217728)) == metadata['cc']['sha256'], 'native compiler identity changed')
    if check_self:
        require(digest(read(__file__)) == metadata['corpus']['files']['evaluate.py'], 'evaluator identity changed')
    return metadata


def run_metadata(run):
    metadata = document(Path(run) / 'prepared.json')
    require(ledger(run)['records'][0]['data']['metadata_sha256'] == digest(read(Path(run) / 'prepared.json')), 'prepared metadata changed')
    return metadata


def task_for(configuration, task_id):
    matches = [task for task in configuration['manifest']['tasks'] if task['id'] == task_id]
    require(len(matches) == 1, 'unknown frozen task')
    return matches[0]


def task_values(captured, task, flavor):
    values = capture(Path(captured) / 'base-data' / task['id'], list(task.get('base', {}))) if task.get('base') else {}
    overlays = Path(captured) / 'corpus/tasks' / task['id'] / flavor
    require(not overlays.is_symlink() and (not overlays.exists() or overlays.is_dir()), 'ordinary task overlay directory required')
    for filename in task['files']:
        if (overlays / filename).exists():
            values[filename] = capture(overlays, [filename])[filename]
    require(set(values) == set(task['files']), 'incomplete frozen task source')
    require(identity(values)['bytes'] <= MAX_SOURCE, 'complete task source limit')
    return values


def candidate_values(run, task, configuration):
    values = capture(Path(run) / 'candidate', task['files'], error=Constraint)
    original = task_values(configuration['freeze_path'], task, 'initial')
    require(all(values[key] == original[key] for key in task['files'] if key not in task['editable']), 'fixed candidate file changed', Constraint)
    return values


def freeze(args):
    corpus = Path(args.corpus).resolve()
    values = tree(corpus)
    compiler = Path(args.compiler).resolve(strict=True)
    require(os.access(compiler, os.X_OK), 'existing executable production compiler required')
    cc = Path(shutil.which(args.cc) or args.cc).resolve(strict=True)
    docs = {key: read(ROOT / key) for key in PUBLIC}
    runtime = {key: read(ROOT / 'runtime' / key) for key in ('slim_rt.c', 'slim_rt.h')}
    destination = Path(args.destination).resolve()
    destination.mkdir(parents=True, exist_ok=False)
    install(values, destination / 'corpus')
    # Interpret only the retained bytes. Live author edits after this point
    # cannot change manifest/fixture association within this freeze.
    definition = manifest(destination / 'corpus')
    for task in definition['tasks']:
        read(destination / 'corpus/tasks' / task['id'] / 'TASK.md')
        oracle(destination / 'corpus', task)
    install(docs, destination / 'public-docs')
    install(runtime, destination / 'runtime')
    bases = {}
    for task in definition['tasks']:
        for candidate_name, repository_name in task.get('base', {}).items():
            bases[task['id'] + '/' + candidate_name] = capture(ROOT, [repository_name])[repository_name]
    install(bases, destination / 'base-data')
    for task in definition['tasks']:
        task_values(destination, task, 'initial')
        task_values(destination, task, 'reference')
    (destination / 'toolchain').mkdir()
    shutil.copyfile(compiler, destination / 'toolchain/slimc')
    (destination / 'toolchain/slimc').chmod(0o555)
    environment = {'PATH': os.environ.get('PATH', '/usr/bin:/bin'), 'LC_ALL': 'C', 'LANG': 'C',
                   'TMPDIR': str(destination / 'tmp')}
    (destination / 'tmp').mkdir()
    # Only declared build variables are captured; no secret-bearing environment dump.
    for variable in ('SDKROOT', 'MACOSX_DEPLOYMENT_TARGET'):
        if variable in os.environ:
            environment[variable] = os.environ[variable]
    metadata = {'schema': 2, 'created': utc(), 'manifest': definition, 'corpus': identity(values),
                'public_docs': identity(docs), 'runtime': identity(runtime), 'base_data': identity(bases),
                'compiler_sha256': digest(read(destination / 'toolchain/slimc', 134217728)),
                'cc': {'path': str(cc), 'sha256': digest(read(cc, 134217728))},
                'environment': environment, 'host': {'platform': platform.platform(), 'boot': boot()}}
    write_json(destination / 'freeze.json', metadata, immutable=True)
    print(json.dumps({'freeze': str(destination), 'sha256': digest(read(destination / 'freeze.json'))}))
    return destination


def prepare(args):
    captured = Path(args.freeze).resolve()
    configuration = frozen(captured)
    verification = document(args.verification)
    require(verification['passed'] is True and verification['freeze_sha256'] ==
            digest(read(captured / 'freeze.json')), 'successful frozen fixture verification required')
    matches = [trial for trial in configuration['manifest']['trials'] if trial['id'] == args.trial]
    require(len(matches) == 1, 'unknown trial')
    trial = matches[0]
    task = task_for(configuration, trial['task'])
    run = Path(args.destination).resolve()
    run.mkdir(parents=True, exist_ok=False)
    values = task_values(captured, task, 'initial')
    install(values, run / 'candidate')
    for path in (run / 'candidate').rglob('*'):
        if path.is_file():
            path.chmod(0o644)
    (run / 'snapshots').mkdir()
    (run / 'receipts').mkdir()
    install(tree(captured / 'public-docs'), run / 'docs')
    shutil.copyfile(captured / 'corpus/tasks' / task['id'] / 'TASK.md', run / 'TASK.md')
    metadata = {'schema': 2, **trial, 'freeze_path': str(captured),
                'freeze_sha256': digest(read(captured / 'freeze.json')), 'prepared': utc(),
                'verification_path': str(Path(args.verification).resolve()),
                'verification_sha256': digest(read(args.verification))}
    frozen(captured)
    write_json(run / 'prepared.json', metadata, immutable=True)
    append(run, 'prepared', {'metadata_sha256': digest(read(run / 'prepared.json'))})
    treatment = ('Semantic context is available.' if trial['condition'] == 'context'
                 else 'Semantic context is unavailable; do not invoke it by another route.')
    budget = configuration['manifest']['budgets']
    prompt = f'''Work in {run}/candidate on {run}/TASK.md. Read supplied docs/DESIGN.md,
docs/docs/CORE.md and docs/design/FEATURE_POLICY.md before editing. Edit only:
{', '.join(task['editable'])}. Fixed project files must retain their frozen bytes.
You are a fresh isolated Sol 6.1 xhigh participant. Do not inspect evaluator code,
oracle/reference files, other trials, compiler implementation outside the supplied candidate, Git history or
the parent conversation. Do not change ledger, snapshots, metadata or supplied
docs. This same-filesystem restriction is advisory, not secure blinding.
No network, purchases, dependencies or other model services. All compiler
feedback, including temporary test edits in these same candidate files, uses:
python3 {BASE}/evaluate.py tool {run} OPERATION [MODULE.DECLARATION]
Operations: check, interfaces, build, run, context. {treatment}
Concurrent wrapper requests serialize without penalty; their queue wait counts
toward {budget['elapsed_seconds']} elapsed seconds. At most {budget['operations']}
admitted operations; each subprocess at most {budget['process_seconds']} seconds.
Context --source REQUEST_ID selects an explicitly retained earlier source;
--expected REQUEST_ID supplies exact expected bytes and rejects stale input.
Do not run the independent evaluator. Submit one final candidate; compilation,
your tests and self-reported success do not decide task acceptance. Actual model
calls/tokens are unknown unless independently observed. Report changed behavior
and remaining uncertainty. Do not learn from another trial.
'''
    (run / 'PROMPT.txt').write_text(prompt)
    print(str(run / 'PROMPT.txt'))


def configuration_for(run):
    metadata = run_metadata(run)
    captured = Path(metadata['freeze_path'])
    require(digest(read(captured / 'freeze.json')) == metadata['freeze_sha256'], 'freeze metadata changed')
    require(digest(read(metadata['verification_path'])) == metadata['verification_sha256'], 'fixture verification changed')
    configuration = frozen(captured)
    require(configuration['host']['boot'] == boot(), 'host boot evidence changed; monotonic trial unavailable')
    state = ledger(run)
    if state['dispatch'] is not None:
        require(time.monotonic_ns() >= state['dispatch']['mono_ns'], 'monotonic epoch moved backwards')
    return metadata, configuration, captured


def start(args):
    run = Path(args.run).resolve()
    _, configuration, _ = configuration_for(run)
    append(run, 'dispatch', {'boot': configuration['host']['boot'],
                            'budget_seconds': configuration['manifest']['budgets']['elapsed_seconds']})


def terminal(run, request_id, status, reason, receipt=None, begun=None, feedback_bytes=None):
    append(run, 'finished', {'status': status, 'reason': reason,
                            'receipt': str(receipt.relative_to(run)) if receipt else None,
                            'receipt_sha256': digest(read(receipt)) if receipt else None,
                            'elapsed_ns': time.monotonic_ns() - begun if begun is not None else None,
                            'feedback_bytes': feedback_bytes}, request_id)


def tool(args):
    run = Path(args.run).resolve()
    request_id, begun, requested_utc = str(uuid.uuid4()), time.monotonic_ns(), utc()
    append(run, 'request', {'operation': args.operation, 'selector': args.selector,
                          'source': args.source, 'expected': args.expected}, request_id, at=requested_utc, monotonic=begun)
    receipt, status = None, 'infrastructure-error'
    try:
        metadata, configuration, captured = configuration_for(run)
        task = task_for(configuration, metadata['task'])
        require(args.operation != 'context' or metadata['condition'] == 'context', 'context unavailable', Constraint)
        require((args.operation == 'context') == bool(args.selector), 'context selector required only for context', Constraint)
        require(args.operation == 'context' or (args.source is None and args.expected is None), 'retained revisions only for context', Constraint)
        if args.selector:
            require(bool(re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*\.[A-Za-z_][A-Za-z_0-9]*', args.selector)), 'qualified declaration required', Constraint)
        state = ledger(run)
        dispatch = state['dispatch']
        cutoff = state['submission_intent']
        if dispatch is None or (cutoff is not None and begun >= cutoff['mono_ns']):
            status = 'denied'
            raise Constraint('trial not active')
        def retained(request):
            require(request in state['requests'] and state['requests'][request]['started'] is not None, 'unknown retained source', Constraint)
            path = run / 'snapshots' / request / 'source'
            values = capture(path, task['files'])
            require(identity(values) == state['requests'][request]['started']['data']['source_identity'], 'retained source changed')
            return values
        values = retained(args.source) if args.source else candidate_values(run, task, {**configuration, 'freeze_path': str(captured)})
        expected_values = retained(args.expected) if args.expected else values
        private = run / 'snapshots' / request_id
        source_identity = install(values, private / 'source')
        expected_identity = install(expected_values, private / 'expected') if args.operation == 'context' else None
        # All candidate reads completed before the operation queue is entered.
        queued = time.monotonic_ns()
        with lock(run / 'operation.lock'):
            state = ledger(run)
            quota = sum(request['started'] is not None for request in state['requests'].values())
            budget = configuration['manifest']['budgets']
            if state['submission_intent'] is not None and begun >= state['submission_intent']['mono_ns']:
                status = 'denied'
                raise Constraint('request received after submission')
            if time.monotonic_ns() - dispatch['mono_ns'] > budget['elapsed_seconds'] * 1000000000:
                status = 'denied'
                raise Constraint('elapsed trial budget exceeded')
            if quota >= budget['operations']:
                status = 'denied'
                raise Constraint('operation budget exceeded')
            append(run, 'started', {'source_identity': source_identity, 'expected_identity': expected_identity,
                                   'capture_ns': queued - begun, 'queue_ns': time.monotonic_ns() - queued,
                                   'quota_index': quota + 1}, request_id)
            try:
                work = run / 'receipts' / request_id
                work.mkdir()
                results = commands(args.operation, private / 'source' / task['entry'],
                                   private / 'expected' / task['entry'], args.selector, captured,
                                   work, budget['process_seconds'])
                receipt = work / 'result.json'
                result = {'schema': 2, 'request': request_id, 'operation': args.operation,
                          'source_identity': source_identity, 'expected_identity': expected_identity,
                          'compiler_sha256': configuration['compiler_sha256'], 'commands': recorded_commands(results)}
                write_json(receipt, result, immutable=True)
                # Identity checked before and after native work; changed bytes invalidate it.
                configuration_for(run)
                require(capture(private / 'source', task['files']) == values, 'operation snapshot changed')
                if args.operation == 'context':
                    require(capture(private / 'expected', task['files']) == expected_values, 'expected snapshot changed')
                status = next((command['status'] for command in results if command['status'] in
                               {'infrastructure-error', 'timeout', 'output-limit', 'resource-limit'}),
                              'ok' if all(ok(command) for command in results) else 'compiler-error')
                # Bounded feedback previews; exact bytes remain in the participant's own receipts.
                result['feedback'] = [{field: command[field][:65536] for field in ('stdout', 'stderr')}
                                      | {'preview_limit': 65536, 'complete': all(len(command[field]) <= 65536 for field in ('stdout', 'stderr'))}
                                      for command in results]
                rendered = json.dumps(result, sort_keys=True) + '\n'
                terminal(run, request_id, status, '', receipt, begun, len(rendered.encode()))
                sys.stdout.write(rendered)
            except (OSError, ValueError, KeyError, TypeError) as error:
                terminal(run, request_id, 'infrastructure-error', type(error).__name__ + ': ' + str(error), receipt, begun)
                print(json.dumps({'request': request_id, 'status': 'infrastructure-error', 'reason': str(error)}))
                return 1
    except Constraint as error:
        if status != 'denied':
            status = 'constraint-error'
        terminal(run, request_id, status, str(error), receipt, begun)
        print(json.dumps({'request': request_id, 'status': status, 'reason': str(error)}))
        return 1
    except (OSError, ValueError, KeyError, TypeError) as error:
        terminal(run, request_id, 'infrastructure-error', type(error).__name__ + ': ' + str(error), receipt, begun)
        print(json.dumps({'request': request_id, 'status': 'infrastructure-error', 'reason': str(error)}))
        return 1
    return 0


def finish(args):
    run = Path(args.run).resolve()
    submitted, stamp = time.monotonic_ns(), utc()
    intent = submission_intent(run, args.reason, stamp, submitted)
    source_identity, error = None, None
    try:
        metadata, configuration, captured = configuration_for(run)
        task = task_for(configuration, metadata['task'])
        values = candidate_values(run, task, {**configuration, 'freeze_path': str(captured)})
        source_identity = install(values, run / 'submitted')
    except Constraint as failure:
        error = {'kind': 'constraint', 'reason': str(failure)}
    except (OSError, ValueError, KeyError, TypeError) as failure:
        error = {'kind': 'infrastructure', 'reason': type(failure).__name__ + ': ' + str(failure)}
    append(run, 'submission', {'reason': args.reason, 'source_identity': source_identity,
                              'capture_error': error, 'elapsed_ns': intent['data']['elapsed_ns']})


def acceptance(captured, task, values, destination, seconds):
    infrastructure_statuses = {'infrastructure-error', 'timeout', 'output-limit', 'resource-limit'}

    def checked(commands):
        return None if any(command['status'] in infrastructure_statuses for command in commands) else all(ok(command) for command in commands)

    components = []
    initial_source = destination / 'original'
    install(values, initial_source)
    initial = commands('check', initial_source / task['entry'], None, None, captured,
                       destination / 'original-work', seconds)
    input_identity = {key: value for key, value in identity(values).items() if key != 'files'}
    initial_pass = checked(initial)
    components.append({'id': 'complete-project', 'passed': initial_pass,
                       'input_identity': input_identity,
                       'domain': 'complete submitted fixed project', 'commands': initial})
    if initial_pass is None:
        components[-1]['reason'] = 'unknown-infrastructure'
    task_dir = captured / 'corpus/tasks' / task['id']
    for component in oracle(captured / 'corpus', task):
        observed = {'id': component['id'], 'domain': component['domain'], 'commands': []}
        positive_pass = True
        if 'positive' in component:
            positive = dict(values)
            positive.update({key: read(task_dir / path) for key, path in component['positive'].items()})
            source = destination / (component['id'] + '-positive')
            install(positive, source)
            results = commands('check', source / task['entry'], None, None, captured,
                               destination / (component['id'] + '-positive-work'), seconds)
            observed['commands'].extend(results)
            positive_pass = checked(results)
            observed['positive_control_accepted'] = positive_pass
            observed['positive_input_identity'] = {key: value for key, value in identity(positive).items() if key != 'files'}
        candidate = dict(values)
        candidate.update({key: read(task_dir / path) for key, path in component['overlay'].items()})
        observed['input_identity'] = {key: value for key, value in identity(candidate).items() if key != 'files'}
        source = destination / component['id']
        install(candidate, source)
        results = commands(component['operation'], source / task['entry'], None, None, captured,
                           destination / (component['id'] + '-work'), seconds)
        observed['commands'].extend(results)
        actual, expected = results[-1], component['expect']
        observed['expected_returncode'] = expected['returncode']
        unknown = any(command['status'] in infrastructure_statuses for command in observed['commands'])
        passed = None if unknown else positive_pass and actual['returncode'] == expected['returncode']
        if unknown:
            observed['reason'] = 'unknown-infrastructure'
        for field in ('stdout', 'stderr'):
            if field in expected:
                expected_bytes = read(task_dir / expected[field], MAX_OUTPUT)
                observed['expected_' + field + '_sha256'] = digest(expected_bytes)
                observed['expected_' + field + '_bytes'] = len(expected_bytes)
                if unknown:
                    continue
                observed_bytes = actual[field].encode('latin1')
                passed &= observed_bytes == expected_bytes
                if observed_bytes != expected_bytes:
                    limit = min(len(observed_bytes), len(expected_bytes))
                    offset = next((index for index in range(limit) if observed_bytes[index] != expected_bytes[index]), limit)
                    observed['first_' + field + '_difference'] = {'byte_offset': offset,
                        'observed': observed_bytes[offset] if offset < len(observed_bytes) else None,
                        'expected': expected_bytes[offset] if offset < len(expected_bytes) else None}
        if 'diagnostics' in expected:
            canonical = ''.join(f'{code}@{module}@{start}:{end}\n' for code, module, start, end in expected['diagnostics'])
            observed['expected_diagnostics'] = expected['diagnostics']
            observed['expected_diagnostic_stdout_sha256'] = digest(canonical.encode())
            observed['expected_diagnostic_stdout_bytes'] = len(canonical.encode())
            observed['diagnostic_stream_exact'] = None if unknown else actual['stdout'] == canonical
            if not unknown:
                diagnostics = re.findall(r'(E[0-9]{4})@([^@\s]+)@([0-9]+):([0-9]+)', actual['stdout'])
                observed['diagnostics'] = [[code, module, int(start), int(end)] for code, module, start, end in diagnostics]
                passed &= observed['diagnostic_stream_exact']
        observed['passed'] = None if unknown else bool(passed)
        components.append(observed)
    infrastructure = any(command['status'] in infrastructure_statuses
                         for component in components for command in component['commands'])
    for component in components:
        component['commands'] = recorded_commands(component['commands'])
    return {'oracle_accepted': None if any(component['passed'] is None for component in components)
            else all(component['passed'] for component in components),
            'infrastructure_failure': infrastructure, 'components': components}


def verify(args):
    captured = Path(args.freeze).resolve()
    configuration = frozen(captured)
    destination = Path(args.output).resolve()
    destination.mkdir(parents=True, exist_ok=False)
    results = []
    for task in configuration['manifest']['tasks']:
        initial = task_values(captured, task, 'initial')
        source = destination / (task['id'] + '-initial')
        install(initial, source)
        check = commands('check', source / task['entry'], None, None, captured,
                         destination / (task['id'] + '-initial-work'), configuration['manifest']['budgets']['process_seconds'])
        initial_pass = ((all(ok(command) for command in check)) == (task['initial_check'] == 'accepted')
                        and all(command['status'] not in {'infrastructure-error', 'timeout', 'output-limit', 'resource-limit'} for command in check))
        initial_acceptance = acceptance(captured, task, initial, destination / (task['id'] + '-initial-acceptance'),
                                        configuration['manifest']['budgets']['process_seconds'])
        values = task_values(captured, task, 'reference')
        require(all(initial[key] == values[key] for key in task['files'] if key not in task['editable']), 'reference fixed-file mismatch')
        result = acceptance(captured, task, values, destination / (task['id'] + '-reference'),
                            configuration['manifest']['budgets']['process_seconds'])
        result.update(task=task['id'], initial_expected=task['initial_check'], initial_pass=initial_pass,
                      initial_commands=recorded_commands(check), initial_acceptance=initial_acceptance)
        results.append(result)
    frozen(captured)
    report = {'schema': 2, 'freeze_sha256': digest(read(captured / 'freeze.json')),
              'passed': all(result['initial_pass'] and not result['initial_acceptance']['oracle_accepted']
                            and not result['initial_acceptance']['infrastructure_failure']
                            and result['oracle_accepted'] and not result['infrastructure_failure'] for result in results),
              'tasks': results}
    write_json(destination / 'verification.json', report, immutable=True)
    print(json.dumps({'passed': report['passed'], 'verification': str(destination / 'verification.json')}))
    return 0 if report['passed'] else 1


def evaluate(args):
    run = Path(args.run).resolve()
    with lock(run / 'operation.lock'):
        state = evaluation_intent(run)
        metadata = run_metadata(run)
        submission = state['submission']
        intent = state['submission_intent']
        result = {'schema': 2, **metadata, 'oracle_accepted': None, 'components': [],
                  'source_identity': submission['data']['source_identity'],
                  'elapsed_ns': submission['data']['elapsed_ns'],
                  'dispatch': {key: state['dispatch'][key] for key in ('utc', 'mono_ns')},
                  'submission_receipt': {key: intent['data'][key] for key in ('receipt_utc', 'receipt_mono_ns')},
                  'submission_cutoff': {key: intent[key] for key in ('utc', 'mono_ns')},
                  'capture_completed': {key: submission[key] for key in ('utc', 'mono_ns')},
                  'evaluation_seal': {key: state['evaluation_intent'][key] for key in ('utc', 'mono_ns', 'sha256')},
                  'model_tokens': 'unknown-not-observed', 'model_calls': 'unknown-not-observed',
                  'repair_iterations': 'unknown-not-observed', 'notes': state['notes'],
                  'operations': sum(request['started'] is not None for request in state['requests'].values()),
                  'requests': len(state['requests']), 'outcome': 'infrastructure-failure'}
        try:
            validate_receipts(run, state)
            _, configuration, captured = configuration_for(run)
            task = task_for(configuration, metadata['task'])
            if submission['data']['capture_error'] is not None:
                failure = submission['data']['capture_error']
                raise (Constraint if failure['kind'] == 'constraint' else ValueError)(failure['reason'])
            values = capture(run / 'submitted', task['files'])
            require(identity(values) == result['source_identity'], 'submitted source changed')
            result.update(acceptance(captured, task, values, run / 'evaluation',
                                     configuration['manifest']['budgets']['process_seconds']))
            configuration_for(run)
            require(capture(run / 'submitted', task['files']) == values, 'submitted snapshot changed during evaluation')
            wrapper_infrastructure = any(request['finished']['data']['status'] in
                                         {'infrastructure-error', 'timeout', 'output-limit', 'resource-limit'}
                                         for request in state['requests'].values())
            result['outcome'] = ('infrastructure-failure' if result['infrastructure_failure'] or wrapper_infrastructure
                                 else 'interrupted' if submission['data']['reason'] == 'interrupted'
                                 else 'elapsed-timeout' if submission['data']['reason'] == 'timeout' or
                                      result['elapsed_ns'] > configuration['manifest']['budgets']['elapsed_seconds'] * 1000000000
                                 else 'constraint-failure' if any(note['data']['category'] == 'constraint' for note in state['notes'])
                                 else 'accepted' if result['oracle_accepted'] else 'correctness-failure')
        except Constraint as error:
            result.update(outcome='constraint-failure', reason=str(error))
        except (OSError, ValueError, KeyError, TypeError) as error:
            result['reason'] = type(error).__name__ + ': ' + str(error)
        result['accepted'] = result['outcome'] == 'accepted'
        write_json(run / 'result.json', result, immutable=True)
        append(run, 'evaluation', {'outcome': result['outcome'], 'result_sha256': digest(read(run / 'result.json'))})
    print(json.dumps({key: result[key] for key in ('id', 'condition', 'outcome', 'accepted', 'oracle_accepted', 'operations', 'elapsed_ns')}))
    return 0 if result['accepted'] else 1


def recover(args):
    run = Path(args.run).resolve()
    with lock(run / 'operation.lock'):
        for request_id, request in ledger(run)['requests'].items():
            if request['finished'] is None:
                terminal(run, request_id, 'interrupted', args.reason)
        state = ledger(run)
        if state['submission_intent'] is not None and state['submission'] is None:
            intent = state['submission_intent']
            append(run, 'submission', {'reason': 'interrupted', 'source_identity': None,
                    'capture_error': {'kind': 'infrastructure', 'reason': args.reason + '; unfinished submission capture'},
                    'elapsed_ns': intent['data']['elapsed_ns']})


def note(args):
    require(0 < len(args.reason) <= 4096, 'note text limit')
    append(Path(args.run).resolve(), 'note', {'category': args.category, 'reason': args.reason})


def summarize(args):
    captured = Path(args.freeze).resolve()
    configuration = frozen(captured)
    rows = []
    for trial in configuration['manifest']['trials']:
        path = Path(args.runs).resolve() / trial['id']
        state = ledger(path) if (path / 'ledger.jsonl').exists() else None
        if state is not None:
            metadata = run_metadata(path)
            require(metadata['freeze_sha256'] == digest(read(captured / 'freeze.json')) and
                    all(metadata[key] == trial[key] for key in trial), 'different experiment lifecycle')
        if state is not None and state['evaluation'] is not None:
            validate_receipts(path, state)
            require(digest(read(path / 'result.json')) == state['evaluation']['data']['result_sha256'], 'result identity changed')
            result = document(path / 'result.json')
            require(result['freeze_sha256'] == digest(read(captured / 'freeze.json')) and
                    all(result[key] == trial[key] for key in trial), 'different experiment result')
            rows.append({key: result[key] for key in ('id', 'task', 'condition', 'outcome', 'oracle_accepted',
                                                     'elapsed_ns', 'operations', 'requests')})
        elif state is not None:
            observed = ('unfinished-evaluation' if state['evaluation_intent'] is not None
                        else 'submitted-awaiting-evaluation' if state['submission'] is not None
                        else 'unfinished-submission' if state['submission_intent'] is not None
                        else 'dispatched-unsubmitted' if state['dispatch'] is not None else 'prepared-undispatched')
            row = {**trial, 'outcome': 'unresolved' if state['dispatch'] is not None else 'not-run',
                   'oracle_accepted': None, 'observation': observed, 'acceptance_evidence': 'unknown-no-terminal-evaluation',
                   'operations': state['admitted_operations'], 'requests': len(state['requests'])}
            for key in ('dispatch', 'submission_intent', 'submission', 'evaluation_intent'):
                if state[key] is not None:
                    row[key] = {field: state[key][field] for field in ('utc', 'mono_ns', 'sha256')}
            if state['submission_intent'] is not None:
                row['elapsed_ns'] = state['submission_intent']['data']['elapsed_ns']
                row['submission_receipt'] = {field: state['submission_intent']['data'][field]
                                             for field in ('receipt_utc', 'receipt_mono_ns')}
            rows.append(row)
        else:
            rows.append({**trial, 'outcome': 'not-run', 'oracle_accepted': None, 'observation': 'unprepared'})
    report = {'schema': 2, 'freeze_sha256': digest(read(captured / 'freeze.json')), 'denominator': len(rows),
              'accepted_by_condition': {condition: sum(row['condition'] == condition and row['outcome'] == 'accepted' for row in rows)
                                        for condition in ('baseline', 'context')},
              'model_tokens': 'unknown-not-observed', 'model_calls': 'unknown-not-observed',
              'general_effectiveness': 'unknown-single-model-bounded-advisory-isolation', 'trials': rows}
    write_json(args.output, report, immutable=True)
    print(json.dumps({key: value for key, value in report.items() if key != 'trials'}))


def main(argv=None):
    arguments = sys.argv[1:] if argv is None else argv
    if arguments and arguments[0] == '_exec':
        # Trusted internal launcher. It has no public data-driven command field.
        try:
            cpu, file_size, control = int(arguments[1]), int(arguments[2]), int(arguments[3])
            require(arguments[4] == '--' and len(arguments) >= 6, 'launcher framing')
            resource.setrlimit(resource.RLIMIT_CPU, (cpu, cpu))
            resource.setrlimit(resource.RLIMIT_FSIZE, (file_size, file_size))
            resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
            os.set_inheritable(control, False)
            os.write(control, encoded({'status': 'exec-ready', 'mono_ns': time.monotonic_ns(), 'utc': utc()}) + b'\n')
            os.execvpe(arguments[5], arguments[5:], os.environ)
        except (OSError, ValueError) as error:
            if 'control' in locals():
                os.write(control, encoded({'status': 'error', 'reason': (type(error).__name__ + ': ' + str(error))[:2048]}) + b'\n')
            sys.stderr.write('launcher failure: ' + str(error) + '\n')
            return 125
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='mode', required=True)
    p = sub.add_parser('freeze')
    p.add_argument('--corpus', default=str(BASE)); p.add_argument('--compiler', required=True)
    p.add_argument('--cc', default='cc'); p.add_argument('--destination', required=True)
    for mode in ('verify', 'prepare', 'summarize'):
        p = sub.add_parser(mode); p.add_argument('--freeze', required=True)
        if mode == 'prepare':
            p.add_argument('--trial', required=True); p.add_argument('--destination', required=True)
            p.add_argument('--verification', required=True)
        else:
            p.add_argument('--output', required=True)
        if mode == 'summarize':
            p.add_argument('--runs', required=True)
    for mode in ('start', 'finish', 'tool', 'evaluate', 'recover', 'note'):
        p = sub.add_parser(mode); p.add_argument('run')
        if mode == 'tool':
            p.add_argument('operation', choices=sorted(OPERATIONS)); p.add_argument('selector', nargs='?')
            p.add_argument('--source'); p.add_argument('--expected')
        if mode in ('finish', 'recover', 'note'):
            p.add_argument('--reason', required=True, **({'choices': ['submitted', 'timeout', 'interrupted']} if mode == 'finish' else {}))
        if mode == 'note':
            p.add_argument('--category', choices=['constraint', 'human-intervention', 'observation'], required=True)
    args = parser.parse_args(arguments)
    try:
        result = globals()[args.mode](args)
        return result if type(result) is int else 0
    except (OSError, ValueError, KeyError, TypeError, RecursionError, subprocess.SubprocessError) as error:
        print(json.dumps({'status': 'infrastructure-error', 'reason': type(error).__name__ + ': ' + str(error)}), file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
