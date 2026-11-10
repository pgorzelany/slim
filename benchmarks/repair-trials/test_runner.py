"""Runner/accounting tests use controlled subprocesses, never model participants."""
import contextlib
import importlib.util
import io
import json
from pathlib import Path
import shutil
import stat
import sys
import tempfile
import time
import types
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
spec = importlib.util.spec_from_file_location('repair_runner', Path(__file__).with_name('run.py'))
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)


class RunnerTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.directory = Path(self.temporary.name)

    def tearDown(self):
        self.temporary.cleanup()

    def test_paths_and_symlinks_cannot_escape_source_set(self):
        for name in ('../outside', '/absolute', 'a/../b', './file', 'a//b'):
            with self.assertRaises(ValueError):
                runner.relative(name)
        (self.directory/'file').write_text('source')
        (self.directory/'link').symlink_to(self.directory/'file')
        with self.assertRaisesRegex(ValueError, 'symlink'):
            runner.tree(self.directory)

    def test_immutable_files_and_extra_sources_are_rejected(self):
        initial = self.directory/'tasks/d1/initial'
        initial.mkdir(parents=True)
        (initial/'app.slim').write_text('module app\n')
        (initial/'fixed.txt').write_text('fixed')
        candidate = self.directory/'candidate'
        shutil.copytree(initial, candidate)
        data = {'id': 'd1', 'initial': 'initial', 'mutable_files': ['app.slim']}
        with patch.object(runner, 'BASE', self.directory):
            runner.candidate_files(candidate, data)
            (candidate/'fixed.txt').write_text('changed')
            with self.assertRaisesRegex(ValueError, 'immutable'):
                runner.candidate_files(candidate, data)
            (candidate/'fixed.txt').write_text('fixed')
            (candidate/'extra.slim').write_text('extra')
            with self.assertRaisesRegex(ValueError, 'file set'):
                runner.candidate_files(candidate, data)

    def test_module_paths_cannot_change_even_if_manifest_mutable(self):
        initial = self.directory/'tasks/d1/initial'; initial.mkdir(parents=True)
        (initial/'app.slim').write_text('module app\n')
        (initial/'slim.project').write_text('(module app "app.slim")')
        candidate = self.directory/'candidate'; shutil.copytree(initial, candidate)
        (candidate/'slim.project').write_text('(module app "../app.slim")')
        with patch.object(runner, 'BASE', self.directory):
            with self.assertRaisesRegex(ValueError, 'boundaries'):
                runner.candidate_files(candidate, {'id': 'd1', 'initial': 'initial', 'mutable_files': ['slim.project']})

    def test_invocation_failure_and_timeout_are_durable(self):
        metadata = {'start_wall': time.time(), 'start_monotonic': time.monotonic()}
        failed = runner.invoke(self.directory, metadata, [self.directory/'missing'], self.directory, 'participant-check')
        self.assertIn('infrastructure_error', failed)
        with patch.object(runner.subprocess, 'run', side_effect=runner.subprocess.TimeoutExpired(['fake'], .01, output=b'partial')):
            timeout = runner.invoke(self.directory, metadata, ['fake'], self.directory, 'participant-check')
        self.assertTrue(timeout['timeout'])
        self.assertEqual(timeout['stdout'], 'partial')
        self.assertEqual(len(list((self.directory/'commands').glob('*.json'))), 2)
        self.assertTrue(all(runner.read(p)['status'] == 'complete' for p in (self.directory/'commands').glob('*.json')))

    def test_timebox_and_clock_discontinuity_reject_without_launch(self):
        metadata = {'start_wall': time.time()-901, 'start_monotonic': time.monotonic()-901}
        with patch.object(runner.subprocess, 'run') as run:
            with self.assertRaisesRegex(ValueError, 'expired'):
                runner.invoke(self.directory, metadata, ['fake'], self.directory, 'participant-check')
            run.assert_not_called()
        metadata['start_wall'] -= 20
        with self.assertRaisesRegex(ValueError, 'discontinuity'):
            runner.elapsed(metadata)

    def test_lock_rejects_concurrent_mutation(self):
        with runner.lock(self.directory/'lock'):
            with self.assertRaisesRegex(ValueError, 'concurrent'):
                with runner.lock(self.directory/'lock'):
                    pass

    def rows(self, raw, detailed):
        rows = {}
        for task, left, right in zip(runner.IDS[4:], raw, detailed):
            for arm, duration in zip(runner.ARMS, (left, right)):
                rows[task+'-'+arm] = {'accepted': duration is not None, 'elapsed_seconds': duration, 'finished': True}
        return rows

    def test_metric_is_median_paired_reduction_and_needs_three_pairs(self):
        rows = self.rows([1, 100, 1000, None], [.5, 80, 700, None])
        result = runner.comparison(rows)
        self.assertAlmostEqual(result['median_paired_time_reduction'], .3)
        self.assertTrue(result['gate_passed'])
        rows['h3-check-json']['accepted'] = False
        self.assertFalse(runner.comparison(rows)['gate_passed'])

    def test_missing_or_fewer_successes_never_pass_gate(self):
        rows = self.rows([10]*4, [5, 5, 5, None])
        self.assertFalse(runner.comparison(rows)['gate_passed'])
        rows = self.rows([10]*4, [5]*4)
        del rows['h4-raw']
        result = runner.comparison(rows)
        self.assertFalse(result['complete'])
        self.assertFalse(result['gate_passed'])

    def test_frozen_corpus_drift_prevents_dispatch(self):
        runner.save(self.directory/'seal.json', {'corpus': {'task': 'original'}})
        with patch.object(runner, 'corpus', return_value={'task': 'changed'}):
            with self.assertRaisesRegex(ValueError, 'frozen tasks'):
                runner.sealed(self.directory)

    def test_frozen_compiler_runtime_and_public_docs_are_checked(self):
        (self.directory/'slimc').write_text('compiler')
        (self.directory/'cc').write_text('native compiler')
        (self.directory/'runtime').mkdir(); (self.directory/'runtime/rt').write_text('runtime')
        (self.directory/'docs').mkdir(); (self.directory/'docs/CORE.md').write_text('contract')
        runner.save(self.directory/'phase.json', {'compiler_sha256': runner.sha(self.directory/'slimc'),
                    'cc': str(self.directory/'cc'), 'cc_sha256': runner.sha(self.directory/'cc'),
                    'runtime': runner.tree(self.directory/'runtime'), 'docs': runner.tree(self.directory/'docs')})
        runner.identity(self.directory)
        (self.directory/'docs/CORE.md').write_text('different')
        with self.assertRaisesRegex(ValueError, 'documents changed'):
            runner.identity(self.directory)
        (self.directory/'slimc').write_text('different compiler')
        with self.assertRaisesRegex(ValueError, 'production compiler changed'):
            runner.identity(self.directory)

    def test_initial_diagnostic_and_checks_preserve_exact_source_and_transport(self):
        candidate = self.directory/'candidate'; candidate.mkdir()
        (candidate/'app.slim').write_text('first bytes')
        metadata = {'arm': 'check-json'}
        data = {'entry': 'app.slim'}
        feedback = {'stdout': 'json feedback', 'stderr': ''}
        with patch.object(runner, 'candidate_files', return_value={'app.slim': 'hash'}), patch.object(runner, 'invoke', return_value=feedback) as invoke:
            runner.execute_check(self.directory, metadata, self.directory, data)
            (candidate/'app.slim').write_text('later edit')
            runner.execute_check(self.directory, metadata, self.directory, data)
        self.assertEqual((self.directory/'checks/001/app.slim').read_text(), 'first bytes')
        self.assertEqual((self.directory/'checks/002/app.slim').read_text(), 'later edit')
        self.assertEqual(invoke.call_args_list[0].args[2][1], 'check-json')
        self.assertTrue((self.directory/'checks/001.source.json').is_file())

    def test_incomplete_or_infrastructure_accounting_cannot_pass_gate(self):
        rows = self.rows([10]*4, [5]*4)
        rows['h1-raw']['valid_accounting'] = False
        self.assertFalse(runner.comparison(rows)['gate_passed'])

    def test_early_operator_close_leaves_outcome_incomplete(self):
        rows = self.rows([10]*4, [5]*4)
        rows['h4-raw'].update(accepted=False, elapsed_seconds=None, outcome='incomplete', finished=True)
        result = runner.comparison(rows)
        self.assertFalse(result['complete'])
        self.assertFalse(result['gate_passed'])

    def test_original_entrypoint_rejects_before_hidden_client_replacement(self):
        metadata = {'start_wall': time.time(), 'start_monotonic': time.monotonic()}
        data = {'entry': 'app.slim', 'acceptance': {'cases': []}}
        failed = {'returncode': 1, 'timeout': False}
        with patch.object(runner, 'invoke', return_value=failed) as invoke:
            cases = runner.behavior(self.directory, metadata, self.directory, {}, data, self.directory, self.directory)
        self.assertFalse(cases[0]['accepted'])
        self.assertEqual(invoke.call_count, 1)
        self.assertEqual(invoke.call_args.args[-1], 'independent-entry-compile')

    def test_prepare_archives_exact_inputs_and_does_not_retry_partial_setup(self):
        base = self.directory/'inputs'; base.mkdir()
        (base/'run.py').write_text('runner'); (base/'test_runner.py').write_text('tests')
        (self.directory/'docs').mkdir(); (self.directory/'docs/CORE.md').write_text('contract')
        (self.directory/'runtime').mkdir(); (self.directory/'runtime/rt').write_text('runtime')
        compiler = self.directory/'compiler'; compiler.write_text('compiler'); compiler.chmod(0o700)
        expected = runner.tree(base) | {'@docs/docs/CORE.md': runner.sha(self.directory/'docs/CORE.md')}
        args = types.SimpleNamespace(campaign=self.directory/'build/campaign', phase='development', compiler=compiler, model='m', reasoning_effort='high')
        with patch.object(runner, 'ROOT', self.directory.resolve()), patch.object(runner, 'BASE', base), patch.object(runner, 'DOCS', ['docs/CORE.md']), patch.object(runner, 'corpus', return_value=expected), patch.object(runner, 'production_sources', return_value={}), contextlib.redirect_stdout(io.StringIO()):
            runner.prepare(args)
            self.assertEqual(runner.tree(args.campaign/'frozen-inputs'), expected)
            self.assertEqual(runner.read(args.campaign/'seal.json')['corpus'], expected)
            with self.assertRaisesRegex(ValueError, 'phase already frozen'):
                runner.prepare(args)
            args.campaign = self.directory/'build/partial'
            with patch.object(runner.shutil, 'copyfile', side_effect=PermissionError('archive write')):
                with self.assertRaises(PermissionError):
                    runner.prepare(args)
            self.assertFalse((args.campaign/'seal.json').exists())
            with self.assertRaisesRegex(ValueError, 'must not already exist'):
                runner.prepare(args)

    def test_launch_write_normalization_preserves_fixed_files_and_original_bytes(self):
        initial = self.directory/'initial'; initial.mkdir()
        for name in ('mutable.slim', 'fixed.slim'):
            (initial/name).write_text(name); (initial/name).chmod(0o444)
        candidate = self.directory/'candidate'; shutil.copytree(initial, candidate)
        before = runner.tree(initial)
        runner.owner_writable(candidate, ['mutable.slim'])
        self.assertTrue((candidate/'mutable.slim').stat().st_mode & stat.S_IWUSR)
        self.assertFalse((candidate/'fixed.slim').stat().st_mode & stat.S_IWUSR)
        self.assertFalse((initial/'mutable.slim').stat().st_mode & stat.S_IWUSR)
        self.assertEqual(runner.tree(candidate), before)
        self.assertEqual(runner.tree(initial), before)

    def test_readonly_hidden_target_is_replaced_only_in_private_case_copy(self):
        candidate = self.directory/'candidate'; candidate.mkdir()
        (candidate/'app.slim').write_text('original'); (candidate/'app.slim').chmod(0o444)
        oracle = self.directory/'tasks/d1/oracle/client.slim'; oracle.parent.mkdir(parents=True)
        oracle.write_text('hidden client')
        data = {'id': 'd1', 'entry': 'app.slim', 'acceptance': {'cases': [
                {'name': 'client', 'replace': {'app.slim': 'oracle/client.slim'}, 'exit_code': 0, 'stdout': 'ok', 'stderr': ''}]}}
        good = {'returncode': 0, 'timeout': False, 'stdout': 'ok', 'stderr': ''}
        with patch.object(runner, 'BASE', self.directory), patch.object(runner, 'invoke', return_value=good):
            cases = runner.behavior(self.directory, {}, self.directory, {'cc': 'cc'}, data, candidate, self.directory)
        self.assertTrue(cases[0]['accepted'])
        self.assertEqual((candidate/'app.slim').read_text(), 'original')
        self.assertFalse((candidate/'app.slim').stat().st_mode & stat.S_IWUSR)
        self.assertEqual((self.directory/'case-0/app.slim').read_text(), 'hidden client')

    def test_legacy_reader_requires_exact_archive_and_unchanged_semantic_inputs(self):
        contents = {'run.py': 'old runner', 'test_runner.py': 'old tests', 'PROTOCOL.md': 'old protocol',
                    'heldout-freeze.json': 'old aggregate', 'tasks/h1/TASK.txt': 'old prompt',
                    'tasks/h1/task.json': 'descriptor', 'tasks/h1/initial/app.slim': 'source',
                    'tasks/h1/oracle/client.slim': 'client', '@docs/docs/CORE.md': 'public contract'}
        archive = self.directory/'frozen-inputs'
        for name, value in contents.items():
            path = archive/name; path.parent.mkdir(parents=True, exist_ok=True); path.write_text(value)
        expected = runner.tree(archive)
        runner.save(self.directory/'seal.json', {'corpus': expected})
        current = dict(expected)
        for name in ('run.py', 'test_runner.py', 'PROTOCOL.md', 'heldout-freeze.json', 'tasks/h1/TASK.txt'):
            current[name] = 'new hash'
        (self.directory/'run.py').write_text('new runner')
        (self.directory/'test_runner.py').write_text('new tests')
        with patch.object(runner, 'BASE', self.directory), patch.object(runner, 'corpus', return_value=current), patch.object(runner, 'definition', return_value={'prompt': 'TASK.txt'}):
            with self.assertRaisesRegex(ValueError, 'frozen tasks'):
                runner.sealed(self.directory)
            receipt = runner.sealed(self.directory, legacy_evidence=True)
            self.assertEqual(receipt['reader']['archived_runner_sha256'], expected['run.py'])
            for name in ('tasks/h1/task.json', 'tasks/h1/initial/app.slim', 'tasks/h1/oracle/client.slim', '@docs/docs/CORE.md'):
                current[name] = 'changed semantics'
                with self.assertRaisesRegex(ValueError, 'source, client, descriptor'):
                    runner.sealed(self.directory, legacy_evidence=True)
                current[name] = expected[name]
            (archive/'run.py').write_text('tampered old code')
            with self.assertRaisesRegex(ValueError, 'archived frozen inputs'):
                runner.sealed(self.directory, legacy_evidence=True)

    def test_legacy_flag_is_not_accepted_for_dispatch_or_mutation(self):
        arguments = [
            ['prepare', '--campaign', 'build/test', '--phase', 'development', '--compiler', 'compiler', '--model', 'm', '--reasoning-effort', 'high'],
            ['start', '--campaign', 'build/test', '--task', 'd1', '--arm', 'raw', '--participant', 'fresh'],
            ['check', 'session'], ['submit', 'session', '--first-response', 'response'], ['close', 'session', '--reason', 'done']]
        with contextlib.redirect_stderr(io.StringIO()):
            for argv in arguments:
                with self.assertRaises(SystemExit) as error:
                    runner.main(argv+['--legacy-evidence'])
                self.assertEqual(error.exception.code, 2)

    def test_noncommand_infrastructure_and_aborted_submission_remain_unknown(self):
        runner.save(self.directory/'submissions/001/result.json', {'outcome': 'unknown-infrastructure', 'cases': []})
        (self.directory/'submissions/002').mkdir()
        results, failures = runner.submission_evidence(self.directory)
        self.assertEqual(len(results), 1)
        self.assertEqual(failures, ['001:unknown-infrastructure', '002:aborted-submission'])
        rows = self.rows([10]*4, [5]*4)
        rows['h1-raw'].update(accepted=False, outcome='timeout', finished=True, valid_accounting=not failures)
        self.assertFalse(runner.comparison(rows)['complete'])

    def test_infrastructure_error_after_deadline_is_never_laundered_to_timeout(self):
        session = self.directory/'session'; session.mkdir()
        (session/'candidate').mkdir(); (session/'candidate/app.slim').write_text('source')
        response = self.directory/'response.txt'; response.write_text('first response')
        metadata = {'accepted': None, 'closed': None}
        data = {'id': 'd1', 'entry': 'app.slim'}
        with patch.object(runner, 'session_context', return_value=(session, metadata, self.directory, {}, data)), patch.object(runner, 'candidate_files', return_value={'app.slim': 'hash'}), patch.object(runner, 'behavior', side_effect=PermissionError('private overlay')), patch.object(runner, 'elapsed', return_value=901), contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(runner.submit(types.SimpleNamespace(session=session, first_response=response)), 1)
        result = runner.read(session/'submissions/001/result.json')
        self.assertEqual(result['outcome'], 'unknown-infrastructure')
        self.assertEqual(result['elapsed_seconds'], 901)
        self.assertIsNone(metadata['accepted'])
        self.assertTrue(runner.submission_evidence(session)[1])

    def test_replay_keeps_original_measurements_and_source_unchanged(self):
        session = self.directory/'session'; session.mkdir()
        source = session/'submissions/001/source'; source.mkdir(parents=True)
        (source/'app.slim').write_text('accepted source')
        result = session/'submissions/001/result.json'
        runner.save(result, {'source_sha256': runner.tree(source), 'elapsed_seconds': 123.5})
        runner.save(session/'session.json', {'accepted': {'submission': '001', 'elapsed_seconds': 123.5}})
        runner.save(session/'commands/0001.json', {'status': 'complete'})
        before = runner.tree(session)
        metadata = runner.read(session/'session.json')
        with patch.object(runner, 'session_context', return_value=(session, metadata, self.directory, {}, {})), patch.object(runner, 'behavior', return_value=[{'accepted': True}]), contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(runner.replay(types.SimpleNamespace(session=session, legacy_evidence=True)), 0)
        after = runner.tree(session)
        self.assertTrue(all(after[name] == digest for name, digest in before.items()))
        replay = runner.read(session/'replays/001/result.json')
        self.assertTrue(replay['legacy_evidence'])
        self.assertEqual(runner.read(session/'session.json')['accepted']['elapsed_seconds'], 123.5)

    def test_failed_submission_can_retry_and_first_response_is_preserved(self):
        session = self.directory/'session'; session.mkdir()
        (session/'candidate').mkdir()
        (session/'candidate/app.slim').write_text('broken')
        response = self.directory/'response.txt'; response.write_text('first proposed solution')
        metadata = {'accepted': None, 'closed': None, 'start_wall': time.time(), 'start_monotonic': time.monotonic()}
        data = {'id': 'd1', 'initial': 'initial', 'entry': 'app.slim', 'acceptance': {'cases': [{'name': 'client', 'exit_code': 0, 'stdout': '42\n', 'stderr': ''}]}}
        context = (session, metadata, self.directory, {'cc': 'cc'}, data)
        args = types.SimpleNamespace(session=session, first_response=response)
        rejected = {'returncode': 1, 'timeout': False, 'stdout': 'error', 'stderr': ''}
        emitted = {'returncode': 0, 'timeout': False, 'stdout': 'C code', 'stderr': ''}
        observed = {'returncode': 0, 'timeout': False, 'stdout': '42\n', 'stderr': ''}
        with patch.object(runner, 'session_context', return_value=context), patch.object(runner, 'candidate_files', return_value={'app.slim': 'hash'}), contextlib.redirect_stdout(io.StringIO()):
            with patch.object(runner, 'invoke', return_value=rejected):
                self.assertEqual(runner.submit(args), 1)
            self.assertIsNone(metadata['accepted'])
            response.write_text('second solution')
            with patch.object(runner, 'invoke', side_effect=[emitted, emitted, emitted, observed]):
                self.assertEqual(runner.submit(args), 0)
            self.assertEqual((session/'first-response.txt').read_text(), 'first proposed solution')
            self.assertEqual(len(list((session/'submissions').iterdir())), 2)
            with self.assertRaisesRegex(ValueError, 'finished'):
                runner.submit(args)


if __name__ == '__main__':
    unittest.main()
