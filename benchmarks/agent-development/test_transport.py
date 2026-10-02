"""Transport/budget/failure-accounting tests; these do not test SLIM semantics."""
import contextlib
import importlib.util
import io
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
import types
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True

spec = importlib.util.spec_from_file_location('pilot_transport',Path(__file__).with_name('evaluate.py'))
pilot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pilot)


class TransportTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='slim-pilot-transport-')
        self.run = Path(self.temporary.name)
        pilot.copy_sources(pilot.BASE/'tasks/effects-report/initial',self.run/'candidate',pilot.task('effects-report'))
        (self.run/'toolchain').mkdir(); (self.run/'runtime').mkdir()
        self.compiler = self.run/'toolchain/slimc'
        self.compiler.write_text('#!/bin/sh\nsleep 0.15\nexit 0\n')
        self.compiler.chmod(0o700)
        for name in ['slim_rt.c','slim_rt.h']:
            shutil.copyfile(pilot.ROOT/'runtime'/name,self.run/'runtime'/name)
        self.metadata = {'id':'01-effects-baseline','task':'effects-report','condition':'baseline',
                         'started':pilot.now(),'started_monotonic':time.monotonic(),
                         'deadline_monotonic':time.monotonic()+900,'submitted':None,
                         'operations':[],'protocol_violations':[],
                         'toolchain':{'compiler_sha256':pilot.sha(self.compiler),
                                      'cc_path':str(self.compiler),'cc_sha256':pilot.sha(self.compiler),
                                      'runtime':{name:pilot.sha(self.run/'runtime'/name) for name in ['slim_rt.c','slim_rt.h']}}}
        pilot.save(self.run/'trial.json',self.metadata)

    def tearDown(self):
        self.temporary.cleanup()

    def tool(self, operation='check'):
        with contextlib.redirect_stdout(io.StringIO()):
            return pilot.participant_tool(types.SimpleNamespace(run=str(self.run),operation=operation,operands=[]))

    def test_fixed_provider_paths_and_symlinks_are_rejected(self):
        project = self.run/'candidate/slim.project'
        project.write_text(project.read_text().replace('"metrics.slim"','"other.slim"'))
        with self.assertRaisesRegex(ValueError,'four fixed'):
            pilot.candidate_files(self.run/'candidate',pilot.task('effects-report'))
        path = self.run/'candidate/metrics.slim'
        original = self.run/'external.slim'; path.rename(original); path.symlink_to(original)
        with self.assertRaisesRegex(ValueError,'ordinary allowed'):
            pilot.candidate_files(self.run/'candidate',pilot.task('effects-report'))

    def test_twenty_fourth_operation_and_twenty_fifth_rejection_are_durable(self):
        self.metadata['operations'] = [{'status':'complete'} for _ in range(23)]
        pilot.save(self.run/'trial.json',self.metadata)
        self.tool()
        with self.assertRaisesRegex(ValueError,'operation budget exceeded'): self.tool()
        metadata = pilot.read(self.run/'trial.json')
        self.assertEqual(len(metadata['operations']),24)
        self.assertEqual(metadata['operations'][-1]['index'],24)
        self.assertEqual(len(list((self.run/'rejections').glob('*.json'))),1)

    def test_baseline_context_and_expired_deadline_rejections_are_recorded(self):
        with self.assertRaisesRegex(ValueError,'context unavailable'): self.tool('context')
        self.metadata['deadline_monotonic'] = time.monotonic()-1
        pilot.save(self.run/'trial.json',self.metadata)
        with self.assertRaisesRegex(ValueError,'elapsed trial budget exceeded'): self.tool()
        self.assertEqual(len(list((self.run/'rejections').glob('*.json'))),2)

    def test_overlapping_process_cannot_overwrite_reserved_operation(self):
        argv = [sys.executable,str(pilot.BASE/'evaluate.py'),'tool',str(self.run),'check']
        first = subprocess.Popen(argv,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        deadline = time.monotonic()+3
        while not (self.run/'.compiler-operation-lock').exists() and time.monotonic()<deadline:
            time.sleep(0.005)
        self.assertTrue((self.run/'.compiler-operation-lock').exists())
        second = subprocess.run(argv,capture_output=True,timeout=3)
        stdout,stderr = first.communicate(timeout=3)
        self.assertEqual(first.returncode,0,(stdout,stderr))
        self.assertEqual(second.returncode,1,second.stdout)
        self.assertIn(b'concurrent compiler-wrapper',second.stderr)
        metadata = pilot.read(self.run/'trial.json')
        self.assertEqual(len(metadata['operations']),1)
        self.assertEqual(metadata['operations'][0]['status'],'complete')
        self.assertEqual(len(list((self.run/'rejections').glob('*.json'))),1)

    def test_finish_serializes_with_in_flight_operation(self):
        argv = [sys.executable,'-B',str(pilot.BASE/'evaluate.py'),'tool',str(self.run),'check']
        operation = subprocess.Popen(argv,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        deadline = time.monotonic()+3
        while not (self.run/'.compiler-operation-lock').exists() and time.monotonic()<deadline:
            time.sleep(0.005)
        self.assertTrue((self.run/'.compiler-operation-lock').exists())
        with contextlib.redirect_stdout(io.StringIO()):
            pilot.finish(types.SimpleNamespace(run=str(self.run),reason='submitted'))
        stdout,stderr = operation.communicate(timeout=3)
        self.assertEqual(operation.returncode,0,(stdout,stderr))
        metadata = pilot.read(self.run/'trial.json')
        self.assertIsNotNone(metadata['submitted'])
        self.assertEqual(metadata['finish_reason'],'submitted')
        self.assertEqual(metadata['operations'][0]['status'],'complete')

    def test_stuck_operation_keeps_durable_finalization_after_wait_bound(self):
        self.metadata['started_monotonic']=1000
        pilot.save(self.run/'trial.json',self.metadata)
        (self.run/'.compiler-operation-lock').mkdir()
        with patch.object(pilot.time,'monotonic',side_effect=[1001,1001,1097]):
            with self.assertRaisesRegex(ValueError,'did not release'):
                pilot.finish(types.SimpleNamespace(run=str(self.run),reason='timeout'))
        finalized=pilot.read(self.run/'finalization.json')
        self.assertEqual(finalized['finish_reason'],'timeout')
        self.assertEqual(finalized['observed_elapsed_seconds'],1)

    def test_command_start_infrastructure_error_is_an_explicit_record(self):
        argv=[str(self.run/'missing-executable')]
        result=pilot.command(argv)
        self.assertEqual(result['argv'],argv)
        self.assertIsNone(result['returncode'])
        self.assertIn('FileNotFoundError',result['infrastructure_error'])

    def finish_metadata(self):
        frozen = self.run/'freeze.json'
        pilot.save(frozen,{'corpus':pilot.corpus()})
        self.metadata.update(submitted=pilot.now(),finish_reason='submitted',observed_elapsed_seconds=1,
                             freeze_path=str(frozen),freeze_sha256=pilot.sha(frozen))
        pilot.save(self.run/'trial.json',self.metadata)

    def evaluate(self):
        with contextlib.redirect_stdout(io.StringIO()):
            pilot.evaluate_trial(types.SimpleNamespace(run=str(self.run)))
        return pilot.read(self.run/'result.json')

    def test_malformed_candidate_records_final_constraint_failure(self):
        self.finish_metadata()
        (self.run/'candidate/slim.project').write_text('(project broken)\n')
        result = self.evaluate()
        self.assertFalse(result['accepted'])
        self.assertEqual(result['outcome'],'constraint-failure')
        self.assertIn('four fixed',result['evaluation_error'])

    def test_final_identity_change_records_failure_before_oracle_execution(self):
        self.finish_metadata()
        (self.run/'runtime/slim_rt.h').write_bytes(b'changed')
        result = self.evaluate()
        self.assertEqual(result['outcome'],'constraint-failure')
        self.assertEqual(result['commands'],[])
        self.assertIn('compiler/runtime changed',result['evaluation_error'])

    def test_missing_external_native_tool_records_infrastructure_failure(self):
        self.metadata['toolchain']['cc_path'] = str(self.run/'missing-cc')
        self.finish_metadata()
        result = self.evaluate()
        self.assertEqual(result['outcome'],'infrastructure-failure')
        self.assertFalse(result['accepted'])
        self.assertIn('FileNotFoundError',result['infrastructure_error'])


if __name__=='__main__':
    unittest.main()
