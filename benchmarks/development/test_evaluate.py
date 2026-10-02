#!/usr/bin/env python3
"""Permanent transport tests plus an opt-in real production-compiler control."""
import contextlib
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import threading
import time
import unittest
from unittest.mock import patch

HERE = Path(__file__).resolve().parent
SPEC = importlib.util.spec_from_file_location('development_evaluate', HERE / 'evaluate.py')
E = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(E)

FAKE = '''#!{python}
from pathlib import Path
import json, os, subprocess, sys, time
args=sys.argv[1:]
operation=args[0] if args[0] in ['check','interfaces','context'] else 'emit'
source=Path(args[1] if operation!='emit' else args[0])
directory=source.parent
app=(directory/'app.slim').read_bytes()
lib=(directory/'lib.slim').read_bytes()
if b'WAIT' in lib:
    Path('ready').write_text('captured')
    time.sleep(0.6)
    lib=(directory/'lib.slim').read_bytes()
if b'HANG' in lib:
    child=subprocess.Popen([sys.executable,'-c','import time; time.sleep(20)'])
    print(child.pid,flush=True)
    time.sleep(20)
if b'FLOOD' in lib:
    os.write(1,b'x'*3000000)
    sys.exit(0)
if operation=='context':
    expected=Path(args[2]).parent
    if any((directory/name).read_bytes()!=(expected/name).read_bytes() for name in ['slim.project','app.slim','lib.slim']):
        print('E0450@project@0:0');sys.exit(1)
    print(json.dumps({{'lib':lib.decode('latin1')}}));sys.exit(0)
if b'NEG' in app or b'BAD' in lib:
    print('E0999@app@0:3');sys.exit(1)
if operation=='check':
    print(lib.decode('latin1'));sys.exit(0)
if operation=='interfaces':
    print('public-contract');sys.exit(0)
answer='accepted' if b'HIDDEN' not in app or b'RIGHT' in lib else 'wrong'
print('#include <stdio.h>\\nint main(void){{puts("'+answer+'");return 0;}}')
'''

PARALLEL_CONTROL = {
    'app.slim': b'module app\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n'
                b'  parallel:\n    let first: I64 = lib.sample(2)\n'
                b'    let second: I64 = lib.sample(4)\n'
                b'    io.print_i64(first + second)\n    io.println("")\n    0\n',
    'lib.slim': b'module lib\n\nfn sample(value: I64) -> I64 effects[io]:\n'
                b'  let now: I64 = io.monotonic_ms()\n  value\n',
    'slim.project': b'(project 1\n  (entry app)\n'
                    b'  (module app "app.slim" (imports lib) (exports))\n'
                    b'  (module lib "lib.slim" (imports) (exports sample)))\n',
}


def write(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data if isinstance(data, bytes) else data.encode())


def corpus(directory, production=False):
    root = directory / 'corpus'
    root.mkdir()
    for filename in ('evaluate.py', 'PROTOCOL.md'):
        write(root / filename, (HERE / filename).read_bytes())
    manifest = {'schema': 2, 'model': 'gpt-6.1-sol', 'reasoning_effort': 'xhigh',
                'budgets': {'elapsed_seconds': 90, 'operations': 24, 'process_seconds': 10},
                'tasks': [{'id': 'control', 'files': ['app.slim', 'lib.slim', 'slim.project'],
                           'editable': ['app.slim', 'lib.slim'], 'entry': 'slim.project', 'initial_check': 'accepted'}],
                'trials': [{'id': '01-baseline', 'task': 'control', 'condition': 'baseline'},
                           {'id': '02-context', 'task': 'control', 'condition': 'context'}]}
    write(root / 'manifest.json', json.dumps(manifest))
    task = root / 'tasks/control'
    project = '(project 1\n  (entry app)\n  (module app "app.slim" (imports lib) (exports))\n  (module lib "lib.slim" (imports) (exports twice)))\n'
    app = ('module app\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n'
           '  io.print_i64(lib.twice(21))\n  io.println("")\n  0\n') if production else 'BASIC'
    right = 'module lib\n\nfn twice(value: I64) -> I64:\n  value + value\n' if production else 'RIGHT'
    wrong = right.replace('value + value', 'value') if production else 'WRONG'
    for flavor, library in (('initial', wrong), ('reference', right)):
        for filename, data in (('app.slim', app), ('lib.slim', library), ('slim.project', project)):
            write(task / flavor / filename, data)
    write(task / 'TASK.md', 'Make twice correct for the declared finite cases.\n')
    hidden = ('module app\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n'
              '  io.print_i64(lib.twice(-3))\n  io.println("")\n'
              '  io.print_i64(lib.twice(0))\n  io.println("")\n'
              '  io.print_i64(lib.twice(7))\n  io.println("")\n  0\n') if production else 'HIDDEN'
    negative = app.replace('lib.twice(21)', 'lib.twice(true)') if production else 'NEG'
    write(task / 'fixtures/client.slim', hidden)
    write(task / 'fixtures/positive.slim', app)
    write(task / 'fixtures/negative.slim', negative)
    write(task / 'fixtures/output.bin', '-6\n0\n14\n' if production else 'accepted\n')
    write(task / 'fixtures/empty.bin', b'')
    components = [{'id': 'independent-client', 'operation': 'run', 'overlay': {'app.slim': 'fixtures/client.slim'},
                   'expect': {'returncode': 0, 'stdout': 'fixtures/output.bin', 'stderr': 'fixtures/empty.bin'},
                   'domain': 'exact inputs -3, 0, 7; only this finite test domain'}]
    if not production:
        components.append({'id': 'negative-contract', 'operation': 'check',
                           'overlay': {'app.slim': 'fixtures/negative.slim'},
                           'positive': {'app.slim': 'fixtures/positive.slim'},
                           'expect': {'returncode': 1, 'diagnostics': [['E0999', 'app', 0, 3]]},
                           'domain': 'one intentionally ill-typed client and accepted control'})
    write(task / 'acceptance.json', json.dumps({'schema': 2, 'components': components}))
    return root


class Transport(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='slim-development-test-')
        self.directory = Path(self.temporary.name)
        self.corpus = corpus(self.directory)
        self.compiler = self.directory / 'fake-compiler'
        write(self.compiler, FAKE.format(python=sys.executable))
        self.compiler.chmod(0o755)
        self.freeze = self.directory / 'frozen'
        self.call('freeze', '--corpus', self.corpus, '--compiler', self.compiler, '--destination', self.freeze)
        self.verification = self.directory / 'verified/verification.json'
        self.call('verify', '--freeze', self.freeze, '--output', self.verification.parent)
        self.run = self.directory / 'runs/02-context'
        self.call('prepare', '--freeze', self.freeze, '--verification', self.verification,
                  '--trial', '02-context', '--destination', self.run)
        self.call('start', self.run)

    def tearDown(self):
        self.temporary.cleanup()

    def call(self, *arguments, expected=0):
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            code = E.main(list(map(str, arguments)))
        if code != expected and arguments[0] == 'verify':
            path = Path(arguments[-1]) / 'verification.json'
            if path.exists():
                output.write(path.read_text())
                report = E.document(path)
                for task in report['tasks']:
                    controls = [task, task['initial_acceptance']]
                    for control in controls:
                        for component in control['components']:
                            for command in component['commands']:
                                if command['status'] in {'timeout', 'infrastructure-error', 'output-limit', 'resource-limit'}:
                                    for label in ('stdout', 'stderr'):
                                        output.write(Path(command[label + '_artifact']).read_bytes()[:4096].decode('latin1'))
        self.assertEqual(code, expected, output.getvalue())
        return output.getvalue()

    def participant(self, *arguments):
        return subprocess.Popen([sys.executable, str(HERE / 'evaluate.py'), 'tool', str(self.run), *arguments],
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    def waited(self, child, expected=0):
        stdout, stderr = child.communicate(timeout=12)
        self.assertEqual(child.returncode, expected, stdout + stderr)
        return json.loads(stdout)

    def readiness(self):
        deadline = time.monotonic() + 8
        while time.monotonic() < deadline:
            if list((self.run / 'receipts').rglob('ready')):
                return
            time.sleep(0.01)
        self.fail('fake compiler did not reach immutable source')

    def correct(self):
        write(self.run / 'candidate/lib.slim', 'RIGHT')

    def submit(self):
        self.call('finish', self.run, '--reason', 'submitted')

    def test_positive_controls_and_independent_failure(self):
        self.submit()
        self.call('evaluate', self.run, expected=1)
        result = E.document(self.run / 'result.json')
        self.assertEqual(result['outcome'], 'correctness-failure')
        self.assertTrue(result['components'][0]['passed'])
        self.assertFalse(result['components'][1]['passed'])
        self.assertTrue(result['components'][2]['positive_control_accepted'])

    def test_submission_is_immutable_and_accepted(self):
        self.correct()
        self.submit()
        write(self.run / 'candidate/lib.slim', 'WRONG')
        self.call('evaluate', self.run)
        result = E.document(self.run / 'result.json')
        self.assertEqual(result['outcome'], 'accepted')
        self.assertEqual(result['source_identity']['files']['lib.slim'], E.digest(b'RIGHT'))

    def test_unknown_acceptance_keeps_strict_infrastructure_outcome_separate(self):
        self.correct()
        self.submit()
        observed = {'status': 'resource-limit', 'returncode': -signal.SIGXFSZ,
                    'stdout': 'incomplete output', 'stderr': '', 'phase': 'check'}
        with patch.object(E, 'commands', return_value=[observed]):
            self.call('evaluate', self.run, expected=1)
        result = E.document(self.run / 'result.json')
        self.assertEqual(result['outcome'], 'infrastructure-failure')
        self.assertFalse(result['accepted'])
        self.assertIsNone(result['oracle_accepted'])
        self.assertTrue(result['infrastructure_failure'])
        self.assertTrue(all(component['passed'] is None for component in result['components']))

    def test_concurrent_requests_queue_without_penalty(self):
        write(self.run / 'candidate/lib.slim', 'WAIT RIGHT')
        first = self.participant('check')
        self.readiness()
        second = self.participant('interfaces')
        one, two = self.waited(first), self.waited(second)
        state = E.ledger(self.run)
        self.assertEqual(len(state['requests']), 2)
        self.assertEqual([r['finished']['data']['status'] for r in state['requests'].values()], ['ok', 'ok'])
        indexes = sorted(r['started']['data']['quota_index'] for r in state['requests'].values())
        self.assertEqual(indexes, [1, 2])
        self.assertGreater(state['requests'][two['request']]['started']['data']['queue_ns'], 100000000)
        self.assertEqual(one['source_identity'], two['source_identity'])

    def test_candidate_mutation_during_operation_uses_captured_bytes(self):
        write(self.run / 'candidate/lib.slim', 'WAIT RIGHT')
        child = self.participant('check')
        self.readiness()
        write(self.run / 'candidate/lib.slim', 'BAD')
        result = self.waited(child)
        self.assertEqual(result['feedback'][0]['stdout'], 'WAIT RIGHT\n')
        self.assertEqual(result['source_identity']['files']['lib.slim'], E.digest(b'WAIT RIGHT'))
        started = E.ledger(self.run)['requests'][result['request']]['started']
        self.assertGreaterEqual(started['data']['capture_ns'], 0)
        self.assertGreaterEqual(started['data']['queue_ns'], 0)
        self.assertGreater(result['commands'][0]['elapsed_ns'], 0)

    def test_stale_expected_and_explicit_last_good_revision(self):
        self.correct()
        first = json.loads(self.call('tool', self.run, 'check'))
        write(self.run / 'candidate/lib.slim', 'WRONG')
        second = json.loads(self.call('tool', self.run, 'context', 'lib.twice', '--expected', first['request']))
        self.assertEqual(second['commands'][0]['returncode'], 1)
        self.assertIn('E0450', second['feedback'][0]['stdout'])
        third = json.loads(self.call('tool', self.run, 'context', 'lib.twice', '--source', first['request']))
        self.assertEqual(third['commands'][0]['returncode'], 0)
        self.assertEqual(third['source_identity'], first['source_identity'])

    def test_all_rejections_preserved_and_quota_denial_is_separate(self):
        self.call('tool', self.run, 'check')
        budget = E.document(self.freeze / 'freeze.json')
        # Cannot mutate a frozen budget; exercise the boundary by issuing its exact quota.
        for _ in range(budget['manifest']['budgets']['operations'] - 1):
            self.call('tool', self.run, 'check')
        self.call('tool', self.run, 'check', expected=1)
        state = E.ledger(self.run)
        self.assertEqual(sum(r['started'] is not None for r in state['requests'].values()), 24)
        self.assertEqual(list(state['requests'].values())[-1]['finished']['data']['status'], 'denied')
        self.correct(); self.submit(); self.call('evaluate', self.run)

    def test_post_submission_request_retained(self):
        self.correct(); self.submit()
        self.call('tool', self.run, 'check', expected=1)
        state = E.ledger(self.run)
        request = next(iter(state['requests'].values()))
        self.assertIsNone(request['started'])
        self.assertEqual(request['finished']['data']['status'], 'denied')
        self.call('evaluate', self.run)

    def test_source_symlink_and_size_limits(self):
        source = self.run / 'candidate/lib.slim'
        source.unlink(); source.symlink_to(self.compiler)
        self.call('tool', self.run, 'check', expected=1)
        source.unlink(); write(source, b'x' * (E.MAX_FILE + 1))
        self.call('tool', self.run, 'check', expected=1)
        self.assertEqual(len(E.ledger(self.run)['requests']), 2)

    def test_deadline_denial_is_retained_with_no_process(self):
        dispatch = E.ledger(self.run)['dispatch']
        with patch.object(E.time, 'monotonic_ns', return_value=dispatch['mono_ns'] + 91000000000):
            self.call('tool', self.run, 'check', expected=1)
        request = next(iter(E.ledger(self.run)['requests'].values()))
        self.assertIsNone(request['started'])
        self.assertEqual(request['finished']['data']['status'], 'denied')
        self.assertIn('elapsed trial budget', request['finished']['data']['reason'])

    def test_changed_frozen_oracle_and_missing_positive_control_reject(self):
        artifact = self.freeze / 'corpus/tasks/control/fixtures/output.bin'
        artifact.chmod(0o644); artifact.write_bytes(b'changed')
        self.call('tool', self.run, 'check', expected=1)
        request = next(iter(E.ledger(self.run)['requests'].values()))
        self.assertIn('corpus identity', request['finished']['data']['reason'])
        original = E.document(self.corpus / 'tasks/control/acceptance.json')
        del original['components'][1]['positive']
        (self.corpus / 'tasks/control/acceptance.json').write_text(json.dumps(original))
        with self.assertRaisesRegex(ValueError, 'positive control'):
            E.oracle(self.corpus, E.manifest(self.corpus)['tasks'][0])

    def test_unselected_module_change_rejects_complete_expected_revision(self):
        first = json.loads(self.call('tool', self.run, 'context', 'lib.twice'))
        write(self.run / 'candidate/app.slim', 'BASIC edited')
        second = json.loads(self.call('tool', self.run, 'context', 'lib.twice', '--expected', first['request']))
        self.assertEqual(second['commands'][0]['returncode'], 1)
        self.assertNotEqual(first['source_identity'], second['source_identity'])

    def test_compiler_and_corpus_identity_change_are_infrastructure(self):
        path = self.freeze / 'toolchain/slimc'
        path.chmod(0o755); path.write_bytes(path.read_bytes() + b'\n# changed\n')
        self.call('tool', self.run, 'check', expected=1)
        request = next(iter(E.ledger(self.run)['requests'].values()))
        self.assertEqual(request['finished']['data']['status'], 'infrastructure-error')
        self.assertIn('compiler identity', request['finished']['data']['reason'])

    def test_missing_compiler_is_infrastructure_and_submission_remains_observable(self):
        (self.freeze / 'toolchain/slimc').unlink()
        self.call('tool', self.run, 'check', expected=1)
        request = next(iter(E.ledger(self.run)['requests'].values()))
        self.assertEqual(request['finished']['data']['status'], 'infrastructure-error')
        self.submit()
        submission = E.ledger(self.run)['submission']
        self.assertIsNotNone(submission['utc'])
        self.assertGreaterEqual(submission['data']['elapsed_ns'], 0)
        self.assertEqual(submission['data']['capture_error']['kind'], 'infrastructure')
        self.call('evaluate', self.run, expected=1)
        self.assertEqual(E.document(self.run / 'result.json')['outcome'], 'infrastructure-failure')

    def test_missing_candidate_is_an_explicit_constraint(self):
        (self.run / 'candidate/lib.slim').unlink()
        self.call('tool', self.run, 'check', expected=1)
        request = next(iter(E.ledger(self.run)['requests'].values()))
        self.assertEqual(request['finished']['data']['status'], 'constraint-error')
        self.submit(); self.call('evaluate', self.run, expected=1)
        self.assertEqual(E.document(self.run / 'result.json')['outcome'], 'constraint-failure')

    def test_oracle_cannot_supply_executable_data(self):
        path = self.corpus / 'tasks/control/acceptance.json'
        data = E.document(path)
        data['components'][0]['argv'] = ['sh', '-c', 'false']
        path.write_text(json.dumps(data))
        self.call('freeze', '--corpus', self.corpus, '--compiler', self.compiler,
                  '--destination', self.directory / 'illegal-freeze', expected=2)

    def test_corrupt_ledger_halts_without_repair(self):
        path = self.run / 'ledger.jsonl'
        original = path.read_bytes()
        with path.open('ab') as stream:
            stream.write(b'{"incomplete":')
        with self.assertRaises(ValueError):
            E.ledger(self.run)
        self.assertEqual(path.read_bytes(), original + b'{"incomplete":')

    def test_ledger_saturation_and_duplicate_json_do_not_silently_drop(self):
        path = self.run / 'ledger.jsonl'
        original = path.read_bytes()
        with patch.object(E, 'MAX_RECORDS', 2), self.assertRaisesRegex(ValueError, 'saturation'):
            E.append(self.run, 'note', {'category': 'observation', 'reason': 'bound crossing'})
        self.assertEqual(path.read_bytes(), original)
        with self.assertRaisesRegex(ValueError, 'duplicate JSON'):
            E.load_bytes(b'{"schema":2,"schema":2}')
        with self.assertRaisesRegex(ValueError, 'nonfinite'):
            E.load_bytes(b'{"number":NaN}')

    def test_interrupted_request_requires_explicit_recovery(self):
        request_id = '00000000-0000-4000-8000-000000000001'
        E.append(self.run, 'request', {'operation': 'check', 'selector': None, 'source': None, 'expected': None}, request_id)
        self.assertIsNone(E.ledger(self.run)['requests'][request_id]['finished'])
        self.call('recover', self.run, '--reason', 'test independently observed evaluator interruption')
        self.assertEqual(E.ledger(self.run)['requests'][request_id]['finished']['data']['status'], 'interrupted')
        self.assertIsNone(E.ledger(self.run)['requests'][request_id]['finished']['data']['elapsed_ns'])

    def test_receipt_corruption_rejected_at_acceptance(self):
        result = json.loads(self.call('tool', self.run, 'check'))
        artifact = Path(result['commands'][0]['stdout_artifact'])
        artifact.write_bytes(b'forged')
        self.correct(); self.submit(); self.call('evaluate', self.run, expected=1)
        self.assertEqual(E.document(self.run / 'result.json')['outcome'], 'infrastructure-failure')

    def test_frozen_base_sources_use_data_without_durable_duplicates(self):
        manifest = E.document(self.corpus / 'manifest.json')
        manifest['tasks'][0]['base'] = {'lib.slim': 'selfhost/context.slim'}
        (self.corpus / 'manifest.json').write_text(json.dumps(manifest))
        (self.corpus / 'tasks/control/initial/lib.slim').unlink()
        destination = self.directory / 'base-freeze'
        self.call('freeze', '--corpus', self.corpus, '--compiler', self.compiler, '--destination', destination)
        frozen_bytes = (destination / 'base-data/control/lib.slim').read_bytes()
        self.assertEqual(frozen_bytes, (E.ROOT / 'selfhost/context.slim').read_bytes())
        values = E.task_values(destination, manifest['tasks'][0], 'initial')
        self.assertEqual(values['lib.slim'], frozen_bytes)
        self.assertEqual(E.task_values(destination, manifest['tasks'][0], 'reference')['lib.slim'], b'RIGHT')


    def test_submission_cutoff_precedes_slow_capture_and_denies_late_request(self):
        self.correct()
        reached, release, failures = threading.Event(), threading.Event(), []
        original = E.candidate_values
        def slow(*args):
            reached.set()
            if not release.wait(8):
                raise ValueError('test capture barrier timeout')
            return original(*args)
        def finish():
            try:
                E.finish(argparse_namespace(run=str(self.run), reason='submitted'))
            except BaseException as error:
                failures.append(error)
        with patch.object(E, 'candidate_values', side_effect=slow):
            worker = threading.Thread(target=finish)
            worker.start()
            try:
                self.assertTrue(reached.wait(8))
                state = E.ledger(self.run)
                self.assertIsNotNone(state['submission_intent'])
                self.assertIsNone(state['submission'])
                self.call('tool', self.run, 'check', expected=1)
            finally:
                release.set(); worker.join(12)
        self.assertFalse(worker.is_alive())
        self.assertEqual(failures, [])
        state = E.ledger(self.run)
        request = next(iter(state['requests'].values()))
        self.assertGreaterEqual(request['request']['mono_ns'], state['submission_intent']['mono_ns'])
        self.assertIsNone(request['started'])
        self.assertEqual(request['finished']['data']['status'], 'denied')
        self.assertGreater(state['submission']['mono_ns'], state['submission_intent']['mono_ns'])
        self.assertEqual(state['submission']['data']['elapsed_ns'],
                         state['submission_intent']['mono_ns'] - state['dispatch']['mono_ns'])

    def test_evaluation_seal_rejects_concurrent_and_later_mutations(self):
        self.correct(); self.submit()
        reached, release, failures = threading.Event(), threading.Event(), []
        original = E.acceptance
        def slow(*args):
            reached.set()
            if not release.wait(8):
                raise ValueError('test evaluation barrier timeout')
            return original(*args)
        def evaluate():
            try:
                E.evaluate(argparse_namespace(run=str(self.run)))
            except BaseException as error:
                failures.append(error)
        with patch.object(E, 'acceptance', side_effect=slow):
            worker = threading.Thread(target=evaluate)
            worker.start()
            try:
                self.assertTrue(reached.wait(8))
                state = E.ledger(self.run)
                self.assertIsNotNone(state['evaluation_intent'])
                self.assertIsNone(state['evaluation'])
                original_ledger = (self.run / 'ledger.jsonl').read_bytes()
                self.call('tool', self.run, 'check', expected=2)
                self.call('note', self.run, '--category', 'constraint', '--reason', 'late observation', expected=2)
                self.assertEqual((self.run / 'ledger.jsonl').read_bytes(), original_ledger)
            finally:
                release.set(); worker.join(12)
        self.assertFalse(worker.is_alive())
        self.assertEqual(failures, [])
        original_ledger = (self.run / 'ledger.jsonl').read_bytes()
        self.call('tool', self.run, 'check', expected=2)
        self.call('note', self.run, '--category', 'constraint', '--reason', 'after result', expected=2)
        self.assertEqual((self.run / 'ledger.jsonl').read_bytes(), original_ledger)
        result = E.document(self.run / 'result.json')
        self.assertEqual(result['outcome'], 'accepted')
        self.assertEqual(result['requests'], 0)
        self.assertEqual(result['notes'], [])

    def test_unfinished_evaluation_and_orphan_result_remain_unknown_in_denominator(self):
        self.correct(); self.submit()
        state = E.evaluation_intent(self.run)
        E.write_json(self.run / 'result.json', {'accepted': True, 'outcome': 'accepted'})
        output = self.directory / 'summary.json'
        self.call('summarize', '--freeze', self.freeze, '--runs', self.run.parent, '--output', output)
        summary = E.document(output)
        self.assertEqual(summary['denominator'], 2)
        self.assertEqual(summary['accepted_by_condition']['context'], 0)
        row = summary['trials'][1]
        self.assertEqual(row['outcome'], 'unresolved')
        self.assertEqual(row['observation'], 'unfinished-evaluation')
        self.assertIsNone(row['oracle_accepted'])
        self.assertEqual(row['evaluation_intent']['sha256'], state['evaluation_intent']['sha256'])
        self.assertIn('dispatch', row)
        self.assertIn('submission_receipt', row)

    def test_unfinished_submission_recovery_retains_cutoff(self):
        receipt_mono, receipt_utc = time.monotonic_ns(), E.utc()
        intent = E.submission_intent(self.run, 'submitted', receipt_utc, receipt_mono)
        self.call('recover', self.run, '--reason', 'observed capture interruption')
        state = E.ledger(self.run)
        self.assertEqual(state['submission_intent'], intent)
        self.assertEqual(state['submission']['data']['capture_error']['kind'], 'infrastructure')
        self.call('evaluate', self.run, expected=1)
        result = E.document(self.run / 'result.json')
        self.assertEqual(result['outcome'], 'infrastructure-failure')
        self.assertIsNone(result['oracle_accepted'])

    def test_expected_snapshot_mutation_is_infrastructure_with_receipt(self):
        original = E.commands
        def changed(*args):
            results = original(*args)
            expected_file = args[2].parent / 'lib.slim'
            expected_file.chmod(0o644); expected_file.write_bytes(b'forged expected')
            return results
        with patch.object(E, 'commands', side_effect=changed):
            self.call('tool', self.run, 'context', 'lib.twice', expected=1)
        state = E.ledger(self.run)
        finished = next(iter(state['requests'].values()))['finished']['data']
        self.assertEqual(finished['status'], 'infrastructure-error')
        self.assertIn('expected snapshot changed', finished['reason'])
        self.assertIsNotNone(finished['receipt'])
        receipt = E.document(self.run / finished['receipt'])
        self.assertGreater(receipt['commands'][0]['stdout_bytes'], 0)
        E.validate_receipts(self.run, state)

    def test_matching_diagnostic_with_extra_or_malformed_line_is_not_acceptance(self):
        configuration = E.frozen(self.freeze)
        task = configuration['manifest']['tasks'][0]
        values = E.task_values(self.freeze, task, 'reference')
        for index, suffix in enumerate(('unrelated message\n', 'E0999 malformed\n')):
            def observations(operation, source, *args):
                negative = b'NEG' in source.parent.joinpath('app.slim').read_bytes()
                return [{'status': 'compiler-error' if negative else 'ok',
                         'returncode': 1 if negative else 0,
                         'stdout': 'E0999@app@0:3\n' + suffix if negative else
                                   'accepted\n' if operation == 'run' else '', 'stderr': ''}]
            with patch.object(E, 'commands', side_effect=observations):
                result = E.acceptance(self.freeze, task, values, self.directory / ('extra-' + str(index)), 10)
            self.assertFalse(result['oracle_accepted'])
            self.assertTrue(result['components'][2]['positive_control_accepted'])
            self.assertEqual(result['components'][2]['diagnostics'], [['E0999', 'app', 0, 3]])
            self.assertFalse(result['components'][2]['diagnostic_stream_exact'])

    def test_trusted_tool_crash_and_backend_failure_differ_from_program_trap(self):
        entry = self.run / 'candidate/slim.project'
        def observation(code=0, output=''):
            return {'status': 'ok' if code == 0 else 'compiler-error',
                    'returncode': code, 'stdout': output, 'stderr': '',
                    'failure_cause': 'unknown-native-nonzero-exit'}
        with patch.object(E, 'process', return_value=observation(-signal.SIGABRT)):
            result = E.commands('check', entry, None, None, self.freeze, self.directory / 'crash', 10)
        self.assertEqual(result[0]['status'], 'infrastructure-error')
        self.assertEqual(result[0]['phase'], 'check')
        for label, native, executed in (('backend', 1, None), ('program', 0, -signal.SIGABRT)):
            work = self.directory / label; work.mkdir()
            observations = [observation(output='int main(void){return 0;}'), observation(native)]
            if executed is not None:
                observations.append(observation(executed))
            with patch.object(E, 'process', side_effect=observations):
                result = E.commands('run', entry, None, None, self.freeze, work, 10)
            self.assertEqual(result[-1]['phase'], 'native-compile' if executed is None else 'execute')
            self.assertEqual(result[-1]['status'], 'infrastructure-error' if executed is None else 'compiler-error')

    def test_native_runtime_flags_require_canonical_generated_first_line(self):
        configuration = E.document(self.freeze / 'freeze.json')
        runtime = self.freeze / 'runtime'
        entry = self.run / 'candidate/slim.project'
        cases = (
            ('canonical', '#define SLIM_PARALLEL 1\nint main(void){return 0;}\n', True),
            ('string', 'const char *marker = "#define SLIM_PARALLEL 1\\n";\nint main(void){return 0;}\n', False),
            ('later-line', '\n#define SLIM_PARALLEL 1\nint main(void){return 0;}\n', False),
            ('other-value', '#define SLIM_PARALLEL 0\nint main(void){return 0;}\n', False),
        )
        for label, generated, structured in cases:
            with self.subTest(label=label):
                work = self.directory / ('flags-' + label); work.mkdir()
                observations = [dict(status='ok', returncode=0, stdout=generated, stderr=''),
                                dict(status='ok', returncode=0, stdout='', stderr=''),
                                dict(status='ok', returncode=0, stdout='', stderr='')]
                with patch.object(E, 'process', side_effect=observations) as process:
                    result = E.commands('run', entry, None, None, self.freeze, work, 10)
                flags = ['-DSLIM_PARALLEL=1'] if structured else []
                self.assertEqual(process.call_args_list[1].args[0],
                                 [configuration['cc']['path'], '-std=c11', '-O2', '-DNDEBUG',
                                  *flags, '-Wall', '-Wextra', '-Werror', '-I', runtime,
                                  work / 'program.c', runtime / 'slim_rt.c', '-o', work / 'program'])
                self.assertEqual((work / 'program.c').read_bytes(), generated.encode('latin1'))
                self.assertEqual(result[1]['runtime_configuration'],
                                 {'structured_workers': structured, 'platform_workers': False})


class AcceptanceEvidence(unittest.TestCase):
    """Finite command evidence models; no alternate compiler or behavioral oracle."""
    infrastructure = ('infrastructure-error', 'timeout', 'output-limit', 'resource-limit')

    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='slim-acceptance-evidence-')
        self.directory = Path(self.temporary.name)
        self.corpus = corpus(self.directory)
        self.task = E.document(self.corpus / 'manifest.json')['tasks'][0]
        self.values = {name: E.read(self.corpus / 'tasks/control/reference' / name) for name in self.task['files']}
        self.sequence = 0

    def tearDown(self):
        self.temporary.cleanup()

    def command(self, status='ok', returncode=0, stdout='', phase='check'):
        self.sequence += 1
        directory = self.directory / ('receipt-' + str(self.sequence))
        result = dict(status=status, returncode=returncode, stdout=stdout, stderr='', phase=phase)
        for field in ('stdout', 'stderr'):
            value = result[field].encode('latin1')
            artifact = directory / (field + '.bin')
            write(artifact, value)
            result[field + '_artifact'] = str(artifact)
            result[field + '_bytes'] = len(value)
            result[field + '_sha256'] = E.digest(value)
        return result

    def observations(self, operation, source, *unused):
        if b'NEG' in (source.parent / 'app.slim').read_bytes():
            return [self.command('compiler-error', 1, 'E0999@app@0:3\n')]
        if operation == 'run':
            return [self.command(stdout='/* generated C */', phase='emit-c'),
                    self.command(phase='native-compile'), self.command(stdout='accepted\n', phase='execute')]
        return [self.command()]

    def accept(self, observations):
        self.sequence += 1
        with patch.object(E, 'commands', side_effect=observations):
            return E.acceptance(self.directory, self.task, self.values,
                                self.directory / ('accepted-' + str(self.sequence)), 10)

    def assert_unknown(self, result, index):
        self.assertIsNone(result['oracle_accepted'])
        self.assertTrue(result['infrastructure_failure'])
        component = result['components'][index]
        self.assertIsNone(component['passed'])
        self.assertEqual(component['reason'], 'unknown-infrastructure')
        self.assertFalse(any(name.startswith('first_') and name.endswith('_difference') for name in component))
        for command in component['commands']:
            for field in ('stdout', 'stderr'):
                value = Path(command[field + '_artifact']).read_bytes()
                self.assertEqual(command[field + '_sha256'], E.digest(value))
                self.assertEqual(command[field + '_bytes'], len(value))
        return component

    def test_complete_checker_infrastructure_is_unknown_for_all_statuses(self):
        for status in self.infrastructure:
            def observations(operation, source, *unused):
                if source.parent.name == 'original':
                    return [self.command(status, 1, 'partial checker output')]
                return self.observations(operation, source)
            with self.subTest(status=status):
                result = self.accept(observations)
                self.assert_unknown(result, 0)
                self.assertTrue(result['components'][1]['passed'])
                self.assertTrue(result['components'][2]['passed'])

    def test_emit_backend_and_execution_infrastructure_are_unknown(self):
        for status in self.infrastructure:
            for phase in ('emit-c', 'native-compile', 'execute'):
                def observations(operation, source, *unused):
                    results = self.observations(operation, source)
                    if operation == 'run':
                        index = next(index for index, command in enumerate(results) if command['phase'] == phase)
                        return results[:index] + [self.command(status, 0, 'missing or truncated bytes', phase)]
                    return results
                with self.subTest(status=status, phase=phase):
                    result = self.accept(observations)
                    component = self.assert_unknown(result, 1)
                    self.assertEqual(component['expected_stdout_sha256'], E.digest(b'accepted\n'))
                    self.assertEqual(component['expected_stdout_bytes'], 9)
                    self.assertEqual(component['expected_stderr_sha256'], E.digest(b''))
                    self.assertTrue(result['components'][0]['passed'])
                    self.assertTrue(result['components'][2]['passed'])

    def test_negative_probe_infrastructure_has_no_diagnostic_counterexample(self):
        for status in self.infrastructure:
            def observations(operation, source, *unused):
                if b'NEG' in (source.parent / 'app.slim').read_bytes():
                    return [self.command(status, 1, 'E0999@app@0:3\ntruncated diagnostic')]
                return self.observations(operation, source)
            with self.subTest(status=status):
                result = self.accept(observations)
                component = self.assert_unknown(result, 2)
                self.assertTrue(component['positive_control_accepted'])
                self.assertNotIn('diagnostics', component)
                self.assertIsNone(component['diagnostic_stream_exact'])
                self.assertEqual(component['expected_diagnostics'], [['E0999', 'app', 0, 3]])
                self.assertEqual(component['expected_diagnostic_stdout_sha256'], E.digest(b'E0999@app@0:3\n'))

    def test_positive_control_infrastructure_is_unknown_even_with_complete_negative(self):
        for status in self.infrastructure:
            def observations(operation, source, *unused):
                if source.parent.name == 'negative-contract-positive':
                    return [self.command(status, 1, 'positive control incomplete')]
                return self.observations(operation, source)
            with self.subTest(status=status):
                result = self.accept(observations)
                component = self.assert_unknown(result, 2)
                self.assertIsNone(component['positive_control_accepted'])
                self.assertIsNone(component['diagnostic_stream_exact'])
                self.assertNotIn('diagnostics', component)
                self.assertEqual(component['commands'][-1]['stdout_sha256'], E.digest(b'E0999@app@0:3\n'))

    def test_complete_and_positive_ordinary_rejections_remain_exact_false(self):
        for selected in ('original', 'negative-contract-positive'):
            def observations(operation, source, *unused):
                if source.parent.name == selected:
                    return [self.command('compiler-error', 1, 'E0999@app@0:3\n')]
                return self.observations(operation, source)
            with self.subTest(selected=selected):
                result = self.accept(observations)
                self.assertFalse(result['oracle_accepted'])
                self.assertFalse(result['infrastructure_failure'])
                if selected == 'original':
                    self.assertFalse(result['components'][0]['passed'])
                else:
                    self.assertFalse(result['components'][2]['positive_control_accepted'])
                    self.assertFalse(result['components'][2]['passed'])
                    self.assertTrue(result['components'][2]['diagnostic_stream_exact'])

    def test_unknown_oracle_preserves_independent_known_rejection(self):
        def observations(operation, source, *unused):
            if source.parent.name == 'original':
                return [self.command('compiler-error', 1, 'E0999@app@0:3\n')]
            if operation == 'run':
                return [self.command('timeout', -signal.SIGKILL, '', 'emit-c')]
            return self.observations(operation, source)
        result = self.accept(observations)
        self.assert_unknown(result, 1)
        self.assertFalse(result['components'][0]['passed'])

    def test_complete_success_and_finite_byte_mismatch_remain_boolean(self):
        result = self.accept(self.observations)
        self.assertTrue(result['oracle_accepted'])
        self.assertFalse(result['infrastructure_failure'])
        def observations(operation, source, *unused):
            results = self.observations(operation, source)
            if operation == 'run':
                results[-1] = self.command(stdout='wrong\n', phase='execute')
            return results
        result = self.accept(observations)
        self.assertFalse(result['oracle_accepted'])
        self.assertFalse(result['infrastructure_failure'])
        self.assertEqual(result['components'][1]['first_stdout_difference'],
                         {'byte_offset': 0, 'observed': ord('w'), 'expected': ord('a')})


class Processes(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='slim-process-test-')
        self.directory = Path(self.temporary.name)
        self.environment = {'PATH': os.environ.get('PATH', '/usr/bin:/bin'), 'LC_ALL': 'C'}

    def tearDown(self):
        self.temporary.cleanup()

    def process(self, script, **limits):
        return E.process([sys.executable, '-c', script], self.directory / 'work',
                         limits.pop('seconds', 3), self.environment, **limits)

    def test_launch_error(self):
        result = E.process(['/does/not/exist'], self.directory / 'work', 1, self.environment)
        self.assertEqual(result['status'], 'infrastructure-error')
        self.assertEqual(result['returncode'], 125)

    def test_native_exit125_is_not_a_launcher_error(self):
        result = self.process('raise SystemExit(125)')
        self.assertEqual(result['status'], 'compiler-error')
        self.assertEqual(result['returncode'], 125)
        self.assertEqual(result['launch_evidence'][0]['status'], 'exec-ready')
        self.assertGreater(result['executable_elapsed_ns'], 0)

    def test_output_limit_incremental(self):
        result = self.process('import os; os.write(1,b"x"*1000000); os.write(2,b"y"*1000000)', output_cap=1000)
        self.assertEqual(result['status'], 'output-limit')
        self.assertLessEqual(result['stdout_bytes'] + result['stderr_bytes'], 1000)

    def test_generated_c_exact_bound_and_first_crossing(self):
        result = self.process('import os;os.write(1,b"x"*16777216)', output_cap=E.MAX_GENERATED, seconds=5)
        self.assertEqual(result['status'], 'ok')
        self.assertEqual(result['stdout_bytes'], E.MAX_GENERATED)
        crossed = E.process([sys.executable, '-c', 'import os;os.write(1,b"x"*16777217)'],
                            self.directory / 'crossed', 5, self.environment, output_cap=E.MAX_GENERATED)
        self.assertEqual(crossed['status'], 'output-limit')
        self.assertLessEqual(crossed['stdout_bytes'], E.MAX_GENERATED)

    def test_timeout_kills_child_group(self):
        result = self.process('import subprocess,sys,time; p=subprocess.Popen([sys.executable,"-c","from pathlib import Path;import time;time.sleep(0.6);Path(\\\"heartbeat\\\").write_text(\\\"still running\\\");time.sleep(30)"]);print(p.pid,flush=True);time.sleep(30)', seconds=0.2)
        self.assertEqual(result['status'], 'timeout')
        pid = result['stdout'].strip()
        self.assertTrue(pid.isdecimal())
        # The descendant is not our child on macOS; a delayed output heartbeat
        # establishes no work continued without requiring a blocked inventory.
        heartbeat = self.directory / 'work/heartbeat'
        time.sleep(0.7)
        self.assertFalse(heartbeat.exists())
        self.assertLess(result['elapsed_ns'], 2000000000)

    def test_process_file_bound(self):
        result = self.process('import os; f=os.open("large",os.O_CREAT|os.O_WRONLY,0o600);os.write(f,b"x"*20000);os.write(f,b"x")', file_cap=1000)
        self.assertNotEqual(result['returncode'], 0)
        self.assertEqual(result['status'], 'compiler-error')
        self.assertEqual(result['failure_cause'], 'unknown-native-nonzero-exit')
        self.assertLessEqual((self.directory / 'work/large').stat().st_size, 1000)

    def test_argument_data_never_shell_executed(self):
        injected = '; touch ' + str(self.directory / 'unwanted')
        result = E.process([sys.executable, '-c', 'import sys;print(sys.argv[1])', injected],
                           self.directory / 'work', 1, self.environment)
        self.assertEqual(result['stdout'], injected + '\n')
        self.assertFalse((self.directory / 'unwanted').exists())


class CorpusBounds(unittest.TestCase):
    def test_incremental_flat_and_empty_directory_entry_bound(self):
        with tempfile.TemporaryDirectory(prefix='slim-corpus-bound-') as temporary:
            directory = Path(temporary)
            for index in range(E.MAX_CORPUS_ENTRIES):
                (directory / str(index)).mkdir()
            self.assertEqual(E.tree(directory), {})
            (directory / 'crossing').mkdir()
            with self.assertRaisesRegex(ValueError, 'corpus entry bound'):
                E.tree(directory)

    def test_corpus_combined_byte_exact_boundary_and_crossing(self):
        with tempfile.TemporaryDirectory(prefix='slim-corpus-bytes-') as temporary:
            directory = Path(temporary)
            for index in range(E.MAX_CORPUS_BYTES // E.MAX_FILE):
                write(directory / str(index), b'x' * E.MAX_FILE)
            self.assertEqual(sum(map(len, E.tree(directory).values())), E.MAX_CORPUS_BYTES)
            write(directory / 'crossing', b'x')
            with self.assertRaisesRegex(ValueError, 'file byte limit|corpus byte bound'):
                E.tree(directory)


class Production(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('SLIM_DEVELOPMENT_COMPILER'), 'set SLIM_DEVELOPMENT_COMPILER to an existing production binary')
    def test_real_compiler_explicit_parallel_runtime_configuration(self):
        with tempfile.TemporaryDirectory(prefix='slim-production-parallel-control-') as temporary:
            directory = Path(temporary)
            source = corpus(directory, production=True)
            captured = directory / 'frozen'
            with contextlib.redirect_stdout(io.StringIO()):
                E.freeze(argparse_namespace(corpus=str(source), compiler=os.environ['SLIM_DEVELOPMENT_COMPILER'],
                                            cc='cc', destination=str(captured)))
            E.install(PARALLEL_CONTROL, directory / 'source')
            entry = directory / 'source/slim.project'
            checked = E.commands('check', entry, None, None, captured, directory / 'checked', 10)
            work = directory / 'run'; work.mkdir()
            results = E.commands('run', entry, None, None, captured, work, 10)
            self.assertEqual([result['status'] for result in checked + results], ['ok'] * 4,
                             [(result['phase'], result['status'], result['stderr']) for result in checked + results])
            self.assertTrue((work / 'program.c').read_bytes().startswith(b'#define SLIM_PARALLEL 1\n'))
            self.assertEqual(results[1]['argv'].count('-DSLIM_PARALLEL=1'), 1)
            self.assertNotIn('-DSLIM_POSIX_WORKERS=1', results[1]['argv'])
            self.assertNotIn('-pthread', results[1]['argv'])
            self.assertEqual(results[1]['runtime_configuration'],
                             {'structured_workers': True, 'platform_workers': False})
            self.assertEqual((results[2]['returncode'], results[2]['stdout'], results[2]['stderr']), (0, '6\n', ''))

    @unittest.skipUnless(os.environ.get('SLIM_DEVELOPMENT_COMPILER'), 'set SLIM_DEVELOPMENT_COMPILER to an existing production binary')
    def test_real_compiler_independent_finite_client(self):
        with tempfile.TemporaryDirectory(prefix='slim-production-control-') as temporary:
            directory = Path(temporary)
            source = corpus(directory, production=True)
            args = argparse_namespace(corpus=str(source), compiler=os.environ['SLIM_DEVELOPMENT_COMPILER'],
                                      cc='cc', destination=str(directory / 'frozen'))
            with contextlib.redirect_stdout(io.StringIO()):
                E.freeze(args)
                code = E.verify(argparse_namespace(freeze=str(directory / 'frozen'), output=str(directory / 'verified')))
            self.assertEqual(code, 0)
            report = E.document(directory / 'verified/verification.json')
            self.assertTrue(report['passed'])
            self.assertTrue(report['tasks'][0]['components'][1]['passed'])


def argparse_namespace(**values):
    import argparse
    return argparse.Namespace(**values)


if __name__ == '__main__':
    unittest.main()
