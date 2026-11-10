#!/usr/bin/env python3
"""Operator transport for real fresh-chat repair participants; no simulated agents."""
import argparse
from contextlib import contextmanager
from datetime import datetime, timezone
import fcntl
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import statistics
import stat
import subprocess
import sys
import time

sys.dont_write_bytecode = True
BASE = Path(__file__).resolve().parent
ROOT = BASE.parents[1]
IDS = [f'{split}{n}' for split in 'dh' for n in range(1, 5)]
ARMS = ('raw', 'check-json')
BUDGET = 900
DOCS = ['DESIGN.md', 'docs/CORE.md', 'design/FEATURE_POLICY.md', 'docs/PROJECTS.md', 'docs/DIAGNOSTICS.md', 'docs/HOST.md']


def require(condition, reason):
    if not condition:
        raise ValueError(reason)


def now():
    return datetime.now(timezone.utc).isoformat()


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def read(path):
    return json.loads(Path(path).read_text())


def save(path, data):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + '.next')
    temporary.write_text(json.dumps(data, indent=2, sort_keys=True) + '\n')
    temporary.replace(path)


def relative(name):
    require(isinstance(name, str) and name and '\\' not in name, 'invalid relative path')
    path = PurePosixPath(name)
    require(not path.is_absolute() and all(p not in ('..', '.') for p in path.parts)
            and path.as_posix() == name, 'path escapes declared source boundary')
    return name


def tree(directory):
    require(directory.is_dir() and not directory.is_symlink(), 'source directory must be ordinary')
    result = {}
    for path in sorted(directory.rglob('*')):
        require(not path.is_symlink(), 'symlink outside source boundary')
        if path.is_file():
            result[path.relative_to(directory).as_posix()] = sha(path)
        else:
            require(path.is_dir(), 'non-ordinary source file')
    return result


def definition(task_id):
    require(task_id in IDS, 'unknown task')
    directory = BASE/'tasks'/task_id
    data = read(directory/'task.json')
    require(data['schema'] == 1 and data['id'] == task_id, 'invalid task identity')
    require(data['split'] == ('development' if task_id[0] == 'd' else 'heldout'), 'invalid task split')
    for key in ('initial', 'entry', 'prompt'):
        relative(data[key])
    sources = tree(directory/data['initial'])
    require(data['entry'] in sources, 'entry outside initial source set')
    require(bool(data['mutable_files']) and len(set(data['mutable_files'])) == len(data['mutable_files'])
            and all(relative(n) in sources for n in data['mutable_files']), 'invalid mutable source set')
    require((directory/data['prompt']).is_file(), 'missing task prompt')
    require(bool(data['acceptance']['cases']), 'missing independent behavior cases')
    for case in data['acceptance']['cases']:
        require(isinstance(case['name'], str) and bool(case['name']), 'missing case name')
        require(isinstance(case.get('args', []), list) and all(isinstance(a, str) for a in case.get('args', [])), 'invalid case args')
        require(type(case['exit_code']) is int and all(isinstance(case[k], str) for k in ('stdout', 'stderr')), 'invalid expected behavior')
        for target, origin in case.get('replace', {}).items():
            require(relative(target) in sources and (directory/relative(origin)).is_file(), 'oracle replacement outside source set')
    candidate_files(directory/data['initial'], data)
    return data


def corpus():
    # Includes all eight tasks before any development participant starts.
    for task_id in IDS:
        definition(task_id)
    return tree(BASE) | {'@docs/'+name: sha(ROOT/name) for name in DOCS}


def production_sources():
    result = {}
    for name in ('selfhost', 'runtime', 'bootstrap'):
        path = ROOT/name
        if path.is_dir():
            result.update({name+'/'+n: value for n, value in tree(path).items()})
    for name in ('bootstrap.sh',):
        result[name] = sha(ROOT/name)
    return result


def campaign_directory(value):
    directory = Path(value).resolve()
    require(directory.is_relative_to(ROOT/'build'), 'campaign outputs must be under ignored repository build/')
    return directory


def sealed(campaign, legacy_evidence=False):
    receipt = read(campaign/'seal.json')
    current, expected = corpus(), receipt['corpus']
    if not legacy_evidence:
        require(expected == current, 'frozen tasks, runner, or public documents changed')
        return receipt
    # Legacy readers never execute archived tooling or admit changed semantics.
    permitted = {'run.py', 'test_runner.py', 'PROTOCOL.md', 'heldout-freeze.json'}
    permitted.update('tasks/'+task_id+'/'+definition(task_id)['prompt'] for task_id in IDS)
    require(set(current) == set(expected) and all(current[n] == value or n in permitted
            for n, value in expected.items()), 'legacy task source, client, descriptor, or public document changed')
    require(tree(campaign/'frozen-inputs') == expected, 'archived frozen inputs do not match original seal')
    receipt['reader'] = {'legacy_evidence': True, 'seal_sha256': sha(campaign/'seal.json'),
                         'archived_runner_sha256': expected['run.py'], 'reader_sha256': sha(BASE/'run.py'),
                         'archived_tests_sha256': expected['test_runner.py'], 'reader_tests_sha256': sha(BASE/'test_runner.py'),
                         'changed_entries': sorted(n for n in expected if current[n] != expected[n])}
    return receipt


def phase_directory(campaign, task_id):
    return campaign/('development' if task_id[0] == 'd' else 'heldout')


def identity(phase):
    data = read(phase/'phase.json')
    require(sha(phase/'slimc') == data['compiler_sha256'], 'frozen production compiler changed')
    require(sha(data['cc']) == data['cc_sha256'], 'native C compiler changed')
    require(tree(phase/'runtime') == data['runtime'], 'frozen runtime changed')
    require(tree(phase/'docs') == data['docs'], 'frozen public documents changed')
    return data


def prepare(args):
    campaign = campaign_directory(args.campaign)
    current = corpus()
    if (campaign/'seal.json').exists():
        sealed(campaign)
    else:
        require(not campaign.exists(), 'new campaign directory must not already exist')
        campaign.mkdir(parents=True)
        for name in current:
            relative(name)
            origin = ROOT/name[len('@docs/'):] if name.startswith('@docs/') else BASE/name
            archived = campaign/'frozen-inputs'/name
            archived.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(origin, archived)
        require(tree(campaign/'frozen-inputs') == current, 'input changed during archive setup')
        save(campaign/'seal.json', {'schema': 1, 'sealed': now(), 'corpus': current,
             'tasks': IDS, 'heldout_pairs': IDS[4:], 'budget_seconds': BUDGET,
             'gate': {'minimum_jointly_accepted_pairs': 3, 'median_paired_time_reduction': .25,
                      'no_fewer_successes': True}, 'model': args.model, 'reasoning_effort': args.reasoning_effort})
    seal = sealed(campaign)
    require((seal['model'], seal['reasoning_effort']) == (args.model, args.reasoning_effort), 'model configuration changed')
    phase = campaign/args.phase
    require(not phase.exists(), 'phase already frozen; never replace participant inputs')
    compiler = Path(args.compiler).resolve(strict=True)
    require(os.access(compiler, os.X_OK), 'production compiler must already be executable')
    cc = Path(shutil.which(os.environ.get('CC', 'cc')) or '').resolve(strict=True)
    phase.mkdir()
    shutil.copyfile(compiler, phase/'slimc'); (phase/'slimc').chmod(0o700)
    shutil.copytree(ROOT/'runtime', phase/'runtime')
    (phase/'docs').mkdir()
    for name in DOCS:
        shutil.copyfile(ROOT/name, phase/'docs'/Path(name).name)
    save(phase/'phase.json', {'schema': 1, 'prepared': now(), 'compiler_origin': str(compiler),
         'compiler_sha256': sha(compiler), 'cc': str(cc), 'cc_sha256': sha(cc),
         'runtime': tree(phase/'runtime'), 'docs': tree(phase/'docs'), 'production_source_sha256': production_sources(),
         'seal_sha256': sha(campaign/'seal.json')})
    print(str(campaign))


@contextmanager
def lock(path):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('a') as stream:
        try:
            fcntl.flock(stream, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as error:
            raise ValueError('concurrent operation rejected; retry after current operation') from error
        try:
            yield
        finally:
            fcntl.flock(stream, fcntl.LOCK_UN)


def owner_writable(directory, names):
    for name in names:
        path = directory/relative(name)
        require(path.is_file() and not path.is_symlink(), 'write target must be an ordinary source file')
        path.chmod(stat.S_IMODE(path.stat().st_mode) | stat.S_IWUSR)


def start(args):
    campaign = campaign_directory(args.campaign)
    with lock(campaign/'operator.lock'):
        sealed(campaign)
        data = definition(args.task)
        phase = phase_directory(campaign, args.task)
        identity(phase)
        sessions = campaign/'sessions'
        require(args.participant.strip() and not any(read(p)['participant'] == args.participant
                for p in sessions.glob('*/session.json')), 'each participant must be a distinct fresh chat')
        session = sessions/(args.task+'-'+args.arm)
        require(not session.exists(), 'predeclared task/arm already started; reruns cannot replace it')
        session.mkdir(parents=True)
        metadata = {'schema': 1, 'task': args.task, 'arm': args.arm, 'participant': args.participant,
                    'fresh_chat_attested': True, 'started': now(), 'start_wall': time.time(),
                    'start_monotonic': time.monotonic(), 'accepted': None, 'closed': None,
                    'phase_sha256': sha(phase/'phase.json'), 'seal_sha256': sha(campaign/'seal.json')}
        save(session/'session.json', metadata)
        shutil.copytree(BASE/'tasks'/args.task/data['initial'], session/'candidate')
        owner_writable(session/'candidate', data['mutable_files'])
        shutil.copyfile(BASE/'tasks'/args.task/data['prompt'], session/'TASK.md')
        prompt = f'''You are a fresh independent repair participant. Work only in {session}/candidate.
Read {session}/TASK.md and {phase}/docs/DESIGN.md, CORE.md, FEATURE_POLICY.md.
Your model/reasoning settings are fixed by the operator. You have 900 elapsed seconds,
including reading, editing, checks and independent acceptance, from the recorded start.
All compiler invocations must use: python3 {BASE}/run.py check {session}
Edit only the task's declared mutable files: {', '.join(data['mutable_files'])}.
Do not inspect task descriptors, hidden clients/oracles, other participants, compiler
implementation, git history, evaluation implementation, parent chat or network sources.
Do not access frozen compiler/runtime directly or alter metadata/docs. Filesystem
separation is advisory, not secure isolation. Before your first edit, write one sentence describing the diagnosed problem and proposed repair
to {session}/interpretation.txt; preserve those first response bytes. Submit your candidate with:
python3 {BASE}/run.py submit {session} --first-response {session}/interpretation.txt
Only the independent behavior checks decide acceptance. All later submissions are
preserved too; a rejected submission may be repaired within the original deadline.
'''
        record = execute_check(session, metadata, phase, data)
        (session/'initial-feedback.txt').write_bytes((record['stdout']+record['stderr']).encode('latin1'))
        metadata['initial_feedback_sha256'] = sha(session/'initial-feedback.txt')
        save(session/'session.json', metadata)
        prompt += f'Read the preserved first diagnostic in {session}/initial-feedback.txt before editing.\n'
        (session/'PROMPT.txt').write_text(prompt)
        print(str(session/'PROMPT.txt'))


def session_context(value, legacy_evidence=False):
    session = Path(value).resolve()
    campaign = campaign_directory(session.parent.parent)
    require(session.parent == campaign/'sessions', 'invalid session directory')
    sealed(campaign, legacy_evidence)
    metadata = read(session/'session.json')
    require(session.name == metadata['task']+'-'+metadata['arm'], 'session identity changed')
    phase = phase_directory(campaign, metadata['task'])
    require(sha(phase/'phase.json') == metadata['phase_sha256'] and
            sha(campaign/'seal.json') == metadata['seal_sha256'], 'frozen phase/campaign identity changed')
    return session, metadata, phase, identity(phase), definition(metadata['task'])


def elapsed(metadata):
    duration = time.monotonic() - metadata['start_monotonic']
    require(duration >= 0 and abs((time.time()-metadata['start_wall'])-duration) < 5,
            'clock discontinuity; timing unknown, close with an evidence reason')
    return duration


def candidate_files(directory, data):
    initial = BASE/'tasks'/data['id']/data['initial']
    actual, expected = tree(directory), tree(initial)
    require(set(actual) == set(expected), 'candidate source file set changed')
    require(all(actual[n] == digest for n, digest in expected.items() if n not in data['mutable_files']),
            'immutable task source changed')
    # Fixed project paths are checked lexically, never interpreted as SLIM semantics.
    for name in expected:
        if name.endswith('.project'):
            before = re.findall(rb'\(module\s+\w+\s+"([^"]+)"', (initial/name).read_bytes())
            after = re.findall(rb'\(module\s+\w+\s+"([^"]+)"', (directory/name).read_bytes())
            require(before == after, 'project module source boundaries changed')
            for origin in after:
                require(relative(origin.decode()) in expected, 'project references outside candidate source set')
    return actual


def invoke(session, metadata, argv, cwd, role):
    remaining = BUDGET-elapsed(metadata)
    require(remaining > 0, 'participant timebox expired')
    directory = session/'commands'
    directory.mkdir(exist_ok=True)
    index = len(list(directory.glob('*.json'))) + 1
    path = directory/f'{index:04}.json'
    record = {'index': index, 'role': role, 'argv': list(map(str, argv)), 'cwd': str(cwd),
              'started': now(), 'elapsed_start': elapsed(metadata), 'status': 'in-progress'}
    save(path, record)
    begin = time.monotonic()
    try:
        result = subprocess.run(record['argv'], cwd=cwd, capture_output=True, timeout=min(30, remaining))
        record.update(returncode=result.returncode, timeout=False, stdout=result.stdout.decode('latin1'), stderr=result.stderr.decode('latin1'))
    except subprocess.TimeoutExpired as error:
        record.update(returncode=None, timeout=True, stdout=(error.stdout or b'').decode('latin1'), stderr=(error.stderr or b'').decode('latin1'))
    except OSError as error:
        record.update(returncode=None, timeout=False, stdout='', stderr='', infrastructure_error=str(error))
    record.update(status='complete', seconds=time.monotonic()-begin, elapsed_end=elapsed(metadata), finished=now())
    save(path, record)
    return record


def success(record):
    return record['returncode'] == 0 and not record['timeout']


def execute_check(session, metadata, phase, data):
    hashes = candidate_files(session/'candidate', data)
    checks = session/'checks'
    checks.mkdir(exist_ok=True)
    snapshot = checks/f'{len([p for p in checks.iterdir() if p.is_dir()])+1:03}'
    shutil.copytree(session/'candidate', snapshot)
    record = invoke(session, metadata, [phase/'slimc', 'check' if metadata['arm'] == 'raw' else 'check-json',
                    snapshot/data['entry']], snapshot, 'participant-check')
    save(snapshot.parent/(snapshot.name+'.source.json'), hashes)
    return record


def preserve_response(session, metadata, response):
    response = Path(response)
    require(response.is_file() and not response.is_symlink(), 'response must be an ordinary captured participant response')
    first = session/'first-response.txt'
    if not first.exists():
        shutil.copyfile(response, first)
        metadata['first_response_sha256'] = sha(first)
        save(session/'session.json', metadata)
    require(sha(first) == metadata['first_response_sha256'], 'preserved first response changed')


def check(args):
    session = Path(args.session).resolve()
    with lock(session/'operation.lock'):
        session, metadata, phase, _, data = session_context(session)
        require(not metadata['closed'] and not metadata['accepted'], 'session already finished')
        preserve_response(session, metadata, session/'interpretation.txt')
        record = execute_check(session, metadata, phase, data)
        sys.stdout.write(record['stdout']); sys.stderr.write(record['stderr'])
        return record['returncode'] if record['returncode'] is not None and record['returncode'] >= 0 else 1


def behavior(session, metadata, phase, toolchain, data, candidate, destination):
    cases = []
    original = invoke(session, metadata, [phase/'slimc', candidate/data['entry']], candidate, 'independent-entry-compile')
    if not success(original):
        return [{'name': 'original-entrypoint', 'accepted': False,
                 'infrastructure_error': 'infrastructure_error' in original}]
    infrastructure_error = False
    for index, case in enumerate(data['acceptance']['cases']):
        source = destination/f'case-{index}'
        shutil.copytree(candidate, source)
        for target, origin in case.get('replace', {}).items():
            owner_writable(source, [target])
            shutil.copyfile(BASE/'tasks'/data['id']/origin, source/target)
        emitted = invoke(session, metadata, [phase/'slimc', source/data['entry']], source, 'independent-compile')
        accepted = success(emitted)
        infrastructure_error |= 'infrastructure_error' in emitted
        if accepted:
            generated, executable = source/'program.c', source/'program'
            generated.write_bytes(emitted['stdout'].encode('latin1'))
            native = invoke(session, metadata, [toolchain['cc'], '-std=c11', '-O2', '-I', phase/'runtime', generated,
                            phase/'runtime/slim_rt.c', '-o', executable], source, 'independent-native-build')
            accepted = success(native)
            infrastructure_error |= 'infrastructure_error' in native
            if accepted:
                observed = invoke(session, metadata, [executable, *case.get('args', [])], source, 'independent-behavior')
                infrastructure_error |= 'infrastructure_error' in observed
                accepted = (not observed['timeout'] and observed['returncode'] == case['exit_code']
                            and observed['stdout'] == case['stdout'] and observed['stderr'] == case['stderr'])
        cases.append({'name': case['name'], 'accepted': accepted, 'infrastructure_error': infrastructure_error})
    return cases


def submit(args):
    session = Path(args.session).resolve()
    with lock(session/'operation.lock'):
        session, metadata, phase, toolchain, data = session_context(session)
        require(not metadata['closed'] and not metadata['accepted'], 'session already finished')
        response = Path(args.first_response)
        preserve_response(session, metadata, response)
        submissions = session/'submissions'; submissions.mkdir(exist_ok=True)
        destination = submissions/f'{len(list(submissions.iterdir()))+1:03}'
        destination.mkdir()
        result = {'schema': 1, 'started': now(), 'accepted': False, 'cases': [], 'evidence': 'bounded-fixed-independent-clients'}
        try:
            shutil.copyfile(response, destination/'response.txt')
            result['source_sha256'] = candidate_files(session/'candidate', data)
            shutil.copytree(session/'candidate', destination/'source')
            result['cases'] = behavior(session, metadata, phase, toolchain, data, destination/'source', destination)
            result['elapsed_seconds'] = elapsed(metadata)
            result['accepted'] = all(c['accepted'] for c in result['cases']) and result['elapsed_seconds'] <= BUDGET
            result['outcome'] = 'accepted' if result['accepted'] else ('timeout' if result['elapsed_seconds'] > BUDGET else 'rejected')
            if any(c.get('infrastructure_error') for c in result['cases']):
                result['outcome'] = 'unknown-infrastructure'
        except (ValueError, OSError) as error:
            result.update(outcome='unknown-infrastructure' if isinstance(error, OSError) else 'unknown-or-constraint-failure', reason=str(error), elapsed_seconds=None)
            try:
                result['elapsed_seconds'] = elapsed(metadata)
                if not isinstance(error, OSError) and str(error) == 'participant timebox expired':
                    result['outcome'] = 'timeout'
            except ValueError:
                pass
        result['response_sha256'] = sha(destination/'response.txt') if (destination/'response.txt').is_file() else None
        result['finished'] = now()
        save(destination/'result.json', result)
        if result['accepted']:
            metadata['accepted'] = {'submission': destination.name, 'elapsed_seconds': result['elapsed_seconds']}
            save(session/'session.json', metadata)
        print(json.dumps({'accepted': result['accepted'], 'outcome': result['outcome'], 'elapsed_seconds': result['elapsed_seconds']}))
        return 0 if result['accepted'] else 1


def replay(args):
    session, metadata, phase, toolchain, data = session_context(args.session, getattr(args, 'legacy_evidence', False))
    require(metadata['accepted'], 'replay requires an accepted preserved submission')
    submission = session/'submissions'/metadata['accepted']['submission']
    original = read(submission/'result.json')
    require(tree(submission/'source') == original['source_sha256'], 'preserved submission source changed')
    replays = session/'replays'; replays.mkdir(exist_ok=True)
    with lock(session/'operation.lock'):
        directory = replays/f'{len(list(replays.iterdir()))+1:03}'; directory.mkdir()
        replay_clock = {'start_wall': time.time(), 'start_monotonic': time.monotonic()}
        cases = behavior(directory, replay_clock, phase, toolchain, data, submission/'source', directory)
        result = {'schema': 1, 'cases': cases, 'accepted': all(c['accepted'] for c in cases),
                  'replayed': now(), 'original_result_sha256': sha(submission/'result.json'),
                  'reader_sha256': sha(BASE/'run.py'), 'legacy_evidence': getattr(args, 'legacy_evidence', False),
                  'metric_role': 'independent-replay-only; original participant elapsed unchanged'}
        save(directory/'result.json', result)
        print(json.dumps({'replayed': result['accepted']}))
        return 0 if result['accepted'] else 1


def close(args):
    session = Path(args.session).resolve()
    with lock(session/'operation.lock'):
        _, metadata, _, _, _ = session_context(session)
        require(not metadata['accepted'] and not metadata['closed'], 'session already finished')
        require(args.reason.strip(), 'missing evidence reason')
        try:
            duration = elapsed(metadata)
        except ValueError:
            duration = None
        metadata['closed'] = {'at': now(), 'reason': args.reason, 'elapsed_seconds': duration,
                              'outcome': 'timeout' if duration is not None and duration >= BUDGET else 'incomplete'}
        save(session/'session.json', metadata)


def submission_evidence(session):
    results, failures = [], []
    for directory in sorted((session/'submissions').glob('*')):
        path = directory/'result.json'
        if directory.is_symlink() or not directory.is_dir() or not path.is_file() or path.is_symlink():
            failures.append(directory.name+':aborted-submission')
            continue
        try:
            result = read(path)
            if result['outcome'].startswith('unknown') or any(c.get('infrastructure_error') for c in result.get('cases', [])):
                failures.append(directory.name+':'+result['outcome'])
            results.append(result)
        except (ValueError, OSError, KeyError, TypeError, AttributeError):
            failures.append(directory.name+':unreadable-submission-result')
    return results, failures


def comparison(rows):
    paired, successes = [], {arm: 0 for arm in ARMS}
    for task_id in IDS[4:]:
        pair = {arm: rows.get(task_id+'-'+arm) for arm in ARMS}
        for arm, row in pair.items():
            successes[arm] += bool(row and row['accepted'])
        if all(row and row['accepted'] for row in pair.values()):
            raw, detailed = (pair[arm]['elapsed_seconds'] for arm in ARMS)
            require(raw > 0 and detailed >= 0, 'invalid accepted timing')
            paired.append({'task': task_id, 'raw_seconds': raw, 'check_json_seconds': detailed,
                           'time_reduction': 1-detailed/raw})
    median = statistics.median(p['time_reduction'] for p in paired) if paired else None
    complete = all(task_id+'-'+arm in rows and rows[task_id+'-'+arm]['finished'] and rows[task_id+'-'+arm].get('valid_accounting', True) and rows[task_id+'-'+arm].get('outcome', 'accepted') in ('accepted', 'timeout') for task_id in IDS[4:] for arm in ARMS)
    passed = complete and len(paired) >= 3 and median >= .25 and successes['check-json'] >= successes['raw']
    return {'predeclared_pairs': 4, 'jointly_accepted_pairs': paired, 'successes': successes,
            'median_paired_time_reduction': median, 'complete': complete, 'gate_passed': passed,
            'effectiveness': 'bounded-heldout-gate-passed' if passed else 'unknown-gate-not-established'}


def report(args):
    campaign = campaign_directory(args.campaign)
    legacy_evidence = getattr(args, 'legacy_evidence', False)
    receipt = sealed(campaign, legacy_evidence)
    rows = {}
    for task_id in IDS:
        for arm in ARMS:
            key = task_id+'-'+arm
            session = campaign/'sessions'/key
            if not (session/'session.json').exists():
                continue
            _, metadata, _, _, _ = session_context(session, legacy_evidence)
            commands = [read(p) for p in sorted((session/'commands').glob('*.json'))]
            accepted = metadata['accepted']
            submissions, submission_failures = submission_evidence(session)
            if metadata.get('first_response_sha256'):
                require(sha(session/'first-response.txt') == metadata['first_response_sha256'], 'preserved first response changed')
            if accepted:
                original = session/'submissions'/accepted['submission']
                result = read(original/'result.json')
                require(result['accepted'] and result['elapsed_seconds'] == accepted['elapsed_seconds']
                        and 0 < result['elapsed_seconds'] <= BUDGET
                        and tree(original/'source') == result['source_sha256'], 'preserved accepted submission changed')
            duration = accepted['elapsed_seconds'] if accepted else None
            active_elapsed = None
            if not accepted and not metadata['closed']:
                try:
                    active_elapsed = elapsed(metadata)
                except ValueError:
                    pass
            expired = active_elapsed is not None and active_elapsed >= BUDGET
            outcome = 'accepted' if accepted else metadata['closed']['outcome'] if metadata['closed'] else 'timeout' if expired else 'incomplete'
            initial_preserved = ((session/'initial-feedback.txt').is_file() and
                                 sha(session/'initial-feedback.txt') == metadata.get('initial_feedback_sha256') and
                                 bool(commands) and commands[0]['role'] == 'participant-check')
            rows[key] = {'accepted': bool(accepted), 'elapsed_seconds': duration, 'outcome': outcome,
                         'finished': bool(accepted or metadata['closed'] or expired),
                         'initial_observation_preserved': initial_preserved,
                         'submission_accounting_failures': submission_failures,
                         'valid_accounting': initial_preserved and not submission_failures and all(c['status'] == 'complete' and 'infrastructure_error' not in c for c in commands), 'participant': metadata['participant'],
                         'participant_checks_including_initial': sum(c['role'] == 'participant-check' for c in commands),
                         'independent_compiler_invocations': sum(c['role'] in ('independent-entry-compile', 'independent-compile') for c in commands),
                         'compiler_invocations': sum(c['role'] in ('participant-check', 'independent-entry-compile', 'independent-compile') for c in commands),
                         'incomplete_commands': sum(c['status'] != 'complete' for c in commands),
                         'submission_count': len(submissions),
                         'first_response_preserved': (session/'first-response.txt').is_file()}
    result = {'schema': 1, 'reported': now(), 'rows': rows, 'missing': [i+'-'+a for i in IDS for a in ARMS if i+'-'+a not in rows],
              'heldout': comparison(rows), 'limitations': 'Advisory blindness and fresh-chat operator attestation; model actions outside wrapper and token counts unknown. Times are operator wall elapsed, not native performance.'}
    if receipt.get('reader'):
        result['reader'] = receipt['reader']
    if args.output:
        output = Path(args.output).resolve()
        require(output.is_relative_to(ROOT/'build'), 'report output must be under ignored build/')
        require(not output.exists(), 'report output already exists; preserve previous evidence')
        save(output, result)
    print(json.dumps(result, indent=2, sort_keys=True))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='action', required=True)
    prepare_parser = commands.add_parser('prepare', help='seal all eight tasks; pin one production compiler per phase')
    prepare_parser.add_argument('--campaign', required=True)
    prepare_parser.add_argument('--phase', choices=('development', 'heldout'), required=True)
    prepare_parser.add_argument('--compiler', required=True)
    prepare_parser.add_argument('--model', required=True)
    prepare_parser.add_argument('--reasoning-effort', required=True)
    start_parser = commands.add_parser('start', help='start timer before exposing a task to one fresh chat')
    start_parser.add_argument('--campaign', required=True)
    start_parser.add_argument('--task', choices=IDS, required=True)
    start_parser.add_argument('--arm', choices=ARMS, required=True)
    start_parser.add_argument('--participant', required=True, help='unique fresh chat ID; attests no inherited task context')
    check_parser = commands.add_parser('check'); check_parser.add_argument('session')
    submit_parser = commands.add_parser('submit'); submit_parser.add_argument('session')
    submit_parser.add_argument('--first-response', required=True, help='actual participant response; first captured bytes never overwritten')
    replay_parser = commands.add_parser('replay'); replay_parser.add_argument('session')
    close_parser = commands.add_parser('close'); close_parser.add_argument('session'); close_parser.add_argument('--reason', required=True)
    report_parser = commands.add_parser('report'); report_parser.add_argument('--campaign', required=True); report_parser.add_argument('--output')
    for reader in (report_parser, replay_parser):
        reader.add_argument('--legacy-evidence', action='store_true', help='read/replay archived exact evidence after tooling or prompt maintenance; never dispatch')
    args = parser.parse_args(argv)
    try:
        return globals()[args.action](args) or 0
    except (ValueError, OSError, KeyError) as error:
        print('repair-trial: '+str(error), file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
