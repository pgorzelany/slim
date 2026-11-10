"""Black-box workflow controls; fake tools test orchestration, not SLIM semantics."""
import os
import importlib.util
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class ObserverAdmission(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        spec = importlib.util.spec_from_file_location('fixture_preflight', ROOT / 'scripts/verify-fixtures.py')
        cls.preflight = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(cls.preflight)
        cls.source = (ROOT / 'bootstrap/slimc-seed.c').read_text()
        observer = cls.preflight.load('scripts/verify-manifest-validation.py', 'fixture_mutation_observer')
        cls.symbol = observer.namespace_function('project', 'parse_project_manifest_report')

    def test_renamed_observation_anchor_is_rejected_before_native_work(self):
        self.assertIn(self.symbol, self.source)
        broken = self.source.replace(self.symbol, 'removed_manifest_reporter')
        with self.assertRaisesRegex(ValueError, 'anchor'):
            self.preflight.observer_anchors(broken)

    def test_ambiguous_observation_anchor_is_rejected(self):
        header = next(line for line in self.source.splitlines(keepends=True)
                      if line.startswith('static SLIM_UNUSED_FUNCTION ') and line.rstrip().endswith(' {')
                      and f' {self.symbol}(' in line)
        with self.assertRaisesRegex(ValueError, 'anchor'):
            self.preflight.observer_anchors(self.source + header + 'slim_recur: ;\n}\n')


class ValidationWorkflow(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='slim workflow ')
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        (self.root / 'scripts').mkdir()
        (self.root / 'bin').mkdir()
        shutil.copyfile(ROOT / 'scripts/verify-fast.sh', self.root / 'scripts/verify-fast.sh')
        bootstrap = self.root / 'bootstrap.sh'
        bootstrap.write_text('#!/bin/sh\nprintf "bootstrap\\n" >> "$VALIDATION_EVENTS"\n'
                             'exit "${FAIL_BOOTSTRAP:-0}"\n')
        bootstrap.chmod(0o755)
        for filename, stage in [('verify-repair-diagnostics.py', 'diagnostics'),
                                ('test-native-session-diagnostics.py', 'frame-tests'),
                                ('verify-fixtures.py', 'fixtures')]:
            (self.root / 'scripts' / filename).write_text(
                'import os,sys\n'
                f'with open(os.environ["VALIDATION_EVENTS"],"a") as out: out.write({stage!r}+"\\n")\n'
                f'sys.exit(int(os.environ.get("FAIL_{stage.upper().replace("-", "_")}","0")))\n')
        cargo = self.root / 'bin/cargo'
        cargo.write_text(f'#!{sys.executable}\nimport os,sys\n'
                         'with open(os.environ["VALIDATION_EVENTS"],"a") as out: out.write("cargo "+sys.argv[1]+"\\n")\n'
                         'sys.exit(int(os.environ.get("FAIL_CARGO_"+sys.argv[1].upper(),"0")))\n')
        cargo.chmod(0o755)
        self.events = self.root / 'events'
        self.environment = {**os.environ, 'VALIDATION_EVENTS': str(self.events),
                            'PATH': str(self.root / 'bin') + os.pathsep + os.environ['PATH']}

    def run_workflow(self, *args, **fault):
        result = subprocess.run(['sh', str(self.root / 'scripts/verify-fast.sh'), *args],
                                cwd='/', env={**self.environment, **fault}, capture_output=True, timeout=10)
        events = self.events.read_text().splitlines() if self.events.exists() else []
        return result, events

    def test_unknown_or_extra_arguments_do_no_work(self):
        for args in [('unknown',), ('fixtures', 'extra')]:
            with self.subTest(args=args):
                result, events = self.run_workflow(*args)
                self.assertEqual((result.returncode, events), (64, []))

    def test_bootstrap_failure_never_checks_a_stale_compiler(self):
        result, events = self.run_workflow(FAIL_BOOTSTRAP='71')
        self.assertEqual((result.returncode, events), (71, ['bootstrap']))
        self.assertNotIn(b'checks passed', result.stdout)

    def test_diagnostic_failure_stops_before_fixture_work(self):
        result, events = self.run_workflow('fixtures', FAIL_DIAGNOSTICS='1')
        self.assertEqual(result.returncode, 1)
        self.assertEqual(events, ['bootstrap', 'diagnostics'])
        self.assertNotIn(b'checks passed', result.stdout)

    def test_fixture_failure_stops_before_cargo(self):
        result, events = self.run_workflow('fixtures', FAIL_FIXTURES='65')
        self.assertEqual(result.returncode, 65)
        self.assertEqual(events[-1], 'fixtures')
        self.assertFalse(any(event.startswith('cargo') for event in events))
        self.assertNotIn(b'checks passed', result.stdout)

    def test_observer_failure_prevents_governance_and_success(self):
        result, events = self.run_workflow('fixtures', FAIL_CARGO_TEST='101')
        self.assertEqual(result.returncode, 101)
        self.assertEqual(events[-1], 'cargo test')
        self.assertNotIn('cargo run', events)
        self.assertNotIn(b'checks passed', result.stdout)

    def test_default_feedback_is_focused_and_explicitly_incomplete(self):
        result, events = self.run_workflow()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(events, ['bootstrap', 'diagnostics', 'frame-tests'])
        self.assertIn(b'full validation and review remain required', result.stdout)

    def test_fixture_success_does_not_claim_full_acceptance(self):
        result, events = self.run_workflow('fixtures')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('fixtures', events)
        self.assertEqual(events[-1], 'cargo run')
        self.assertIn(b'full validation and review remain required', result.stdout)


if __name__ == '__main__':
    unittest.main()
