#!/usr/bin/env python3
"""Focused operator-log tests; no compiler or benchmark work is launched."""

import importlib.util
import json
from pathlib import Path
import subprocess
import shutil
import sys
import tempfile
import unittest
from unittest.mock import patch


sys.dont_write_bytecode = True
SCRIPT = Path(__file__).with_name("task-time.py")
SPEC = importlib.util.spec_from_file_location("task_time", SCRIPT)
TASK_TIME = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(TASK_TIME)
START = "2026-10-02T11:00:00.000000Z"
END = "2026-10-02T11:00:12.345678Z"


class TaskTimeTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.log = Path(self.temporary.name) / "tasks.jsonl"

    def cli(self, *args, expected=0):
        result = subprocess.run([sys.executable, str(SCRIPT), "--log", str(self.log), *args],
                                text=True, capture_output=True, check=False)
        self.assertEqual(result.returncode, expected, result.stderr)
        return result

    def start(self, task="validation", basis="observed_utc", at=START):
        return TASK_TIME.start_task(self.log, task, at, basis, "test receipt")

    def lines(self):
        return [json.loads(line) for line in self.log.read_text().splitlines()]

    def test_missing_log_status_is_read_only(self):
        self.assertEqual(self.cli("status").stdout, "")
        self.assertFalse(self.log.exists())

    def test_fresh_checkout_default_log_is_local_and_creates_parent(self):
        checkout = Path(self.temporary.name) / "fresh-checkout"
        script = checkout / "scripts/task-time.py"
        script.parent.mkdir(parents=True)
        shutil.copyfile(SCRIPT, script)
        status = subprocess.run([sys.executable, str(script), "status"], capture_output=True, text=True)
        self.assertEqual(status.returncode, 0, status.stderr)
        self.assertFalse((checkout / "build").exists())
        started = subprocess.run([sys.executable, str(script), "start", "fresh local scope"],
                                 capture_output=True, text=True)
        self.assertEqual(started.returncode, 0, started.stderr)
        task_id = json.loads(started.stdout)["id"]
        self.assertIn(task_id, TASK_TIME.read_tasks(checkout / "build/task-times.jsonl"))
        self.assertFalse((checkout / "design/task-times.jsonl").exists())

    def test_manual_wall_span_and_status_replay(self):
        start = json.loads(self.cli("start", "release validation", "--at", START,
                                    "--evidence", "observed start receipt").stdout)
        self.assertEqual(json.loads(self.cli("status", start["id"]).stdout)["duration_basis"], "unknown")
        end = json.loads(self.cli("end", start["id"], "--at", END,
                                  "--evidence", "observed end receipt").stdout)
        self.assertEqual(end["duration_basis"], "wall_clock")
        self.assertEqual(end["duration_ns"], 12345678000)
        status = json.loads(self.cli("status", start["id"]).stdout)
        self.assertEqual(status["outcome"], "succeeded")
        self.assertEqual(status["start"], start)
        self.assertEqual(status["end"], end)
        self.assertEqual(len(self.lines()), 2)

    def test_unknown_and_file_timestamps_do_not_make_duration(self):
        for basis, at in (("unknown", None), ("file_timestamp", START)):
            start = self.start(basis=basis, at=at)
            end = TASK_TIME.end_task(self.log, start["id"], END, "observed_utc",
                                     "unknown", "missing trustworthy start")
            self.assertIsNone(end["duration_ns"])
            self.assertEqual(end["duration_basis"], "unknown")
            self.assertIn("missing or inferred", end["evidence"])

    def test_independent_elapsed_receipt_with_incomplete_stamps(self):
        start = json.loads(self.cli("start", "historical run", "--at", "unknown",
                                    "--evidence", "receipt sha256:abc").stdout)
        end = json.loads(self.cli("end", start["id"], "--at", "unknown", "--elapsed", "12.34",
                                  "--evidence", "receipt observed elapsed seconds").stdout)
        self.assertEqual(end["duration_basis"], "observed_elapsed")
        self.assertEqual(end["duration_ns"], 12340000000)
        self.assertIsNone(end["at"])
        self.assertEqual(self.cli("status").returncode, 0)

    def test_retrospective_and_unknown_arguments_require_evidence(self):
        for arguments in (("start", "old", "--at", "unknown"),
                          ("start", "old", "--at", START),
                          ("start", "old", "--stamp-basis", "file_timestamp")):
            self.cli(*arguments, expected=2)
            self.assertFalse(self.log.exists())
        start = self.start()
        for arguments in (("--unknown",), ("--elapsed", "1")):
            result = self.cli("end", start["id"], *arguments, expected=2)
            self.assertIn("requires --evidence", result.stderr)
        self.assertEqual(len(self.lines()), 1)

    def test_unmatched_duplicate_terminal_and_start_leave_log_unchanged(self):
        start = self.start()
        original = self.log.read_bytes()
        with self.assertRaisesRegex(ValueError, "duplicate start"):
            TASK_TIME.append_event(self.log, start)
        result = self.cli("end", "00000000-0000-0000-0000-000000000000", expected=2)
        self.assertIn("unmatched end", result.stderr)
        self.assertEqual(self.log.read_bytes(), original)
        TASK_TIME.end_task(self.log, start["id"], END, "observed_utc", "succeeded", "observed end")
        finished = self.log.read_bytes()
        result = self.cli("end", start["id"], expected=2)
        self.assertIn("duplicate end", result.stderr)
        self.assertEqual(self.log.read_bytes(), finished)

    def test_corruption_blocks_replay_and_append_without_repair(self):
        start = self.start()
        valid = self.log.read_bytes()
        end = TASK_TIME.event_base("end", start["id"], END, "observed_utc", "test receipt")
        end.update(outcome="succeeded", duration_ns=12345678000, duration_basis="wall_clock")
        unmatched = dict(end, id="00000000-0000-0000-0000-000000000000")
        fixtures = [b"{", b"\n", b"\xff\n", b'{"schema":1,"schema":1}\n',
                    json.dumps(dict(start, schema=True)).encode() + b"\n",
                    valid, json.dumps(unmatched).encode() + b"\n",
                    (json.dumps(end) + "\n" + json.dumps(end) + "\n").encode(),
                    json.dumps(dict(end, duration_ns=1)).encode() + b"\n",
                    json.dumps(dict(start, evidence=float("nan"))).encode() + b"\n",
                    json.dumps(dict(start, provenance="unsupported field")).encode() + b"\n",
                    b"[" * 2000 + b"]" * 2000 + b"\n",
                    b" " * (TASK_TIME.MAX_EVENT_BYTES + 1) + b"\n"]
        for corrupt in fixtures:
            with self.subTest(corrupt=corrupt[:80]):
                original = valid + corrupt
                self.log.write_bytes(original)
                result = self.cli("status", expected=2)
                self.assertIn("corrupt log at line", result.stderr)
                result = self.cli("start", "blocked append", expected=2)
                self.assertIn("corrupt log at line", result.stderr)
                self.assertEqual(self.log.read_bytes(), original)

    def test_concurrent_starts_and_competing_endings(self):
        commands = [[sys.executable, str(SCRIPT), "--log", str(self.log), "start", f"task {index}"]
                    for index in range(12)]
        processes = [subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                     for command in commands]
        starts = []
        for process in processes:
            stdout, stderr = process.communicate(timeout=20)
            self.assertEqual(process.returncode, 0, stderr)
            starts.append(json.loads(stdout))
        self.assertEqual(len(TASK_TIME.read_tasks(self.log)), 12)
        self.assertEqual(len({start["id"] for start in starts}), 12)
        command = [sys.executable, str(SCRIPT), "--log", str(self.log), "end", starts[0]["id"],
                   "--unknown", "--evidence", "competing terminal test"]
        processes = [subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                     for _ in range(2)]
        codes = []
        for process in processes:
            stdout, stderr = process.communicate(timeout=20)
            codes.append(process.returncode)
            if process.returncode != 0:
                self.assertIn("duplicate end", stderr)
        self.assertEqual(sorted(codes), [0, 2])
        self.assertEqual(len(self.lines()), 13)
        self.assertIsNotNone(TASK_TIME.read_tasks(self.log)[starts[0]["id"]]["end"])

    def test_run_observes_monotonic_and_forwards_output_and_exit(self):
        result = self.cli("run", "failing fixture", "--", sys.executable, "-c",
                          "print('fixture output'); raise SystemExit(3)", expected=3)
        self.assertEqual(result.stdout, "fixture output\n")
        start, end = self.lines()
        self.assertEqual(start["id"], end["id"])
        self.assertEqual(end["outcome"], "failed")
        self.assertEqual(end["duration_basis"], "monotonic")
        self.assertGreaterEqual(end["duration_ns"], 0)
        self.assertIn("exit code 3", end["evidence"])

    def test_run_success_launch_failure_and_exact_clock_sample(self):
        with patch.object(TASK_TIME.time, "monotonic_ns", side_effect=[100, 600]):
            with patch("sys.stderr"):
                code = TASK_TIME.main(["--log", str(self.log), "run", "clock sample", "--",
                                       sys.executable, "-c", "pass"])
        self.assertEqual(code, 0)
        self.assertEqual(self.lines()[-1]["duration_ns"], 500)
        self.assertEqual(self.lines()[-1]["outcome"], "succeeded")
        result = self.cli("run", "launch failure", "--", str(Path(self.temporary.name) / "missing"), expected=127)
        self.assertIn("could not launch", self.lines()[-1]["evidence"])
        self.assertEqual(self.lines()[-1]["outcome"], "failed")

    def test_interrupted_run_keeps_a_terminal_observation(self):
        with patch.object(TASK_TIME.subprocess, "run", side_effect=KeyboardInterrupt):
            with patch.object(TASK_TIME.time, "monotonic_ns", side_effect=[100, 600]):
                with patch("sys.stderr"):
                    code = TASK_TIME.main(["--log", str(self.log), "run", "interrupted fixture", "--", "unused"])
        self.assertEqual(code, 130)
        self.assertEqual(self.lines()[-1]["outcome"], "interrupted")
        self.assertEqual(self.lines()[-1]["duration_basis"], "monotonic")
        self.assertEqual(self.lines()[-1]["duration_ns"], 500)

    def test_backward_wall_clock_requires_explicit_unknown(self):
        start = self.start()
        before = self.log.read_bytes()
        result = self.cli("end", start["id"], "--at", "2026-10-02T10:59:00Z",
                          "--evidence", "clock adjustment", expected=2)
        self.assertIn("wall clock moved backwards", result.stderr)
        self.assertEqual(self.log.read_bytes(), before)
        end = json.loads(self.cli("end", start["id"], "--unknown", "--evidence", "clock adjustment").stdout)
        self.assertIsNone(end["duration_ns"])

    def test_elapsed_validation_and_data_never_executed(self):
        for value in ("-1", "NaN", "Infinity", "no", "0.0000000001", "1e20",
                      "1e99999999999", "1e-99999999999", "12.12345678900000000000000000001"):
            with self.subTest(value=value), self.assertRaises(ValueError):
                TASK_TIME.elapsed_ns(value)
        self.assertEqual(TASK_TIME.elapsed_ns("0"), 0)
        self.assertEqual(TASK_TIME.elapsed_ns("12.3400000000000000000000000000"), 12340000000)
        marker = Path(self.temporary.name) / "unexpected"
        start = json.loads(self.cli("start", f"$(touch {marker})").stdout)
        self.cli("status", start["id"])
        self.assertFalse(marker.exists())


if __name__ == "__main__":
    unittest.main()
