#!/usr/bin/env python3
"""Append and replay operator task durations; no estimates or benchmark metrics."""

import argparse
from contextlib import contextmanager
from datetime import datetime, timezone
from decimal import Decimal, InvalidOperation
import fcntl
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time
import uuid


DEFAULT_LOG = Path(__file__).resolve().parents[1] / "build/task-times.jsonl"
MAX_EVENT_BYTES = 16384
BASE_FIELDS = {"schema", "event", "id", "at", "stamp_basis", "recorded_at", "evidence"}
END_FIELDS = {"outcome", "duration_ns", "duration_basis"}
STAMP_BASES = {"observed_utc", "file_timestamp", "unknown"}
OUTCOMES = {"succeeded", "failed", "interrupted", "unknown"}


def require(condition, message):
    if not condition:
        raise ValueError(message)


def utc_now():
    return datetime.now(timezone.utc).isoformat(timespec="microseconds").replace("+00:00", "Z")


def stamp(value):
    require(isinstance(value, str) and re.fullmatch(
        r"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d{1,6})?Z", value
    ), "expected UTC timestamp YYYY-MM-DDTHH:MM:SS[.ffffff]Z")
    return datetime.fromisoformat(value.replace("Z", "+00:00"))


def wall_ns(start, end):
    delta = stamp(end) - stamp(start)
    return (delta.days * 86400 + delta.seconds) * 1000000000 + delta.microseconds * 1000


def text_field(value, name):
    require(isinstance(value, str) and bool(value.strip()) and len(value) <= 4096,
            f"{name} must contain 1..4096 characters")


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        require(key not in result, f"duplicate JSON key: {key}")
        result[key] = value
    return result


def invalid_number(value):
    raise ValueError(f"invalid JSON number: {value}")


def apply_event(tasks, event):
    require(isinstance(event, dict), "event must be an object")
    kind = event.get("event")
    require(kind in ("start", "end"), "event must be start or end")
    fields = BASE_FIELDS | ({"task"} if kind == "start" else END_FIELDS)
    require(set(event) == fields, "unexpected or missing event fields")
    require(type(event["schema"]) is int and event["schema"] == 1, "unsupported schema")
    task_id = event["id"]
    require(isinstance(task_id, str) and str(uuid.UUID(task_id)) == task_id, "id must be a canonical UUID")
    text_field(event["evidence"], "evidence")
    stamp(event["recorded_at"])
    basis = event["stamp_basis"]
    require(isinstance(basis, str) and basis in STAMP_BASES, "invalid stamp_basis")
    if basis == "unknown":
        require(event["at"] is None, "unknown timestamp must be null")
    else:
        stamp(event["at"])
    if kind == "start":
        text_field(event["task"], "task")
        require(task_id not in tasks, f"duplicate start: {task_id}")
        tasks[task_id] = {"start": event, "end": None}
        return
    require(task_id in tasks, f"unmatched end: {task_id}")
    task = tasks[task_id]
    require(task["end"] is None, f"duplicate end: {task_id}")
    require(isinstance(event["outcome"], str) and event["outcome"] in OUTCOMES, "invalid outcome")
    duration = event["duration_ns"]
    duration_basis = event["duration_basis"]
    require(isinstance(duration_basis, str) and duration_basis in {
        "monotonic", "observed_elapsed", "wall_clock", "unknown"
    }, "invalid duration_basis")
    if duration_basis == "unknown":
        require(duration is None, "unknown duration must be null")
    else:
        require(type(duration) is int and 0 <= duration <= 2**63 - 1,
                "duration_ns must be an integer in the supported 292-year range")
    if duration_basis in ("monotonic", "wall_clock"):
        require(basis == "observed_utc" and task["start"]["stamp_basis"] == "observed_utc",
                "clock duration requires observed UTC stamps")
    if duration_basis == "wall_clock":
        require(duration == wall_ns(task["start"]["at"], event["at"]), "wall_clock duration does not match stamps")
    task["end"] = event


def replay(stream):
    tasks = {}
    stream.seek(0)
    lines = iter(lambda: stream.readline(MAX_EVENT_BYTES + 1), b"")
    for number, line in enumerate(lines, 1):
        try:
            require(len(line) <= MAX_EVENT_BYTES, "event exceeds 16384 bytes")
            require(line.endswith(b"\n"), "incomplete event (missing final newline)")
            event = json.loads(line.decode("utf-8"), object_pairs_hook=unique_object,
                               parse_constant=invalid_number)
            apply_event(tasks, event)
        except (ValueError, TypeError, AttributeError, RecursionError) as error:
            raise ValueError(f"corrupt log at line {number}: {error}") from error
    return tasks


@contextmanager
def locked_log(path, write=False):
    # All cooperating writers lock this inode before replay and append. Do not
    # replace or rotate the file while a task-time process is using it.
    if write:
        Path(path).parent.mkdir(parents=True, exist_ok=True)
    try:
        stream = Path(path).open("a+b" if write else "rb")
    except FileNotFoundError:
        if write:
            raise
        yield None
        return
    with stream:
        fcntl.flock(stream.fileno(), fcntl.LOCK_EX if write else fcntl.LOCK_SH)
        try:
            yield stream
        finally:
            fcntl.flock(stream.fileno(), fcntl.LOCK_UN)


def append_event(path, event):
    with locked_log(path, write=True) as stream:
        tasks = replay(stream)
        apply_event(tasks, event)
        write_event(stream, event)
    return event


def write_event(stream, event):
    encoded = (json.dumps(event, sort_keys=True, separators=(",", ":"), ensure_ascii=False) + "\n").encode("utf-8")
    require(len(encoded) <= MAX_EVENT_BYTES, "event exceeds 16384 bytes")
    stream.seek(0, os.SEEK_END)
    stream.write(encoded)
    stream.flush()
    os.fsync(stream.fileno())


def read_tasks(path):
    with locked_log(path) as stream:
        return {} if stream is None else replay(stream)


def event_base(kind, task_id, at, stamp_basis, evidence):
    return dict(schema=1, event=kind, id=task_id, at=at, stamp_basis=stamp_basis,
                recorded_at=utc_now(), evidence=evidence)


def start_task(path, task, at=None, stamp_basis="observed_utc", evidence="live manual start"):
    if at is None and stamp_basis == "observed_utc":
        at = utc_now()
    event = event_base("start", str(uuid.uuid4()), at, stamp_basis, evidence)
    event["task"] = task
    return append_event(path, event)


def end_task(path, task_id, at, stamp_basis, outcome, evidence, duration_ns=None, duration_basis=None):
    # Derivation and append happen under one exclusive lock, including the
    # duplicate-terminal check; two competing end commands cannot both win.
    with locked_log(path, write=True) as stream:
        tasks = replay(stream)
        require(task_id in tasks, f"unmatched end: {task_id}")
        start = tasks[task_id]["start"]
        if duration_basis is None:
            if start["stamp_basis"] == stamp_basis == "observed_utc":
                duration_ns = wall_ns(start["at"], at)
                require(duration_ns >= 0, "wall clock moved backwards; use --unknown with evidence")
                duration_basis = "wall_clock"
            else:
                duration_basis = "unknown"
                evidence += "; duration unknown: missing or inferred timestamp"
        event = event_base("end", task_id, at, stamp_basis, evidence)
        event.update(outcome=outcome, duration_ns=duration_ns, duration_basis=duration_basis)
        apply_event(tasks, event)
        write_event(stream, event)
        return event


def elapsed_ns(value):
    try:
        require(isinstance(value, str) and len(value) <= 128, "elapsed seconds must use at most 128 characters")
        seconds = Decimal(value)
        require(seconds.is_finite() and seconds >= 0, "elapsed seconds must be finite and nonnegative")
        # Convert the exact decimal coefficient rather than let Decimal's
        # current precision round a receipt before checking its precision.
        digits = "".join(str(digit) for digit in seconds.as_tuple().digits)
        coefficient = int(digits)
        if coefficient == 0:
            return 0
        power = seconds.as_tuple().exponent + 9
        if power < 0:
            require(-power <= len(digits) and not any(digit != "0" for digit in digits[power:]),
                    "elapsed seconds must have at most nanosecond precision")
            nanos = coefficient // 10 ** -power
        else:
            require(len(digits) + power <= 19, "elapsed seconds exceed supported 292-year range")
            nanos = coefficient * 10 ** power
        require(nanos <= 2**63 - 1, "elapsed seconds exceed supported 292-year range")
        return nanos
    except InvalidOperation as error:
        raise ValueError("invalid elapsed seconds") from error


def resolve_stamp(args):
    if args.at == "unknown":
        require(args.stamp_basis == "observed_utc", "--at unknown already specifies unknown stamp_basis")
        return None, "unknown"
    at = utc_now() if args.at is None else args.at
    require(args.stamp_basis != "file_timestamp" or args.at is not None,
            "file_timestamp requires an explicit --at")
    stamp(at)
    return at, args.stamp_basis


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, epilog="""Local POSIX (macOS/Linux) JSONL log, schema 1. Locks coordinate this CLI's
writers; do not replace the log during use. Torn/corrupt records halt replay
and append; repair from original receipts rather than silently dropping data.
Open tasks have unknown final durations. No ETA, percentage, or native benchmark
claim is derived. Wall-clock spans include waiting/overlap and can be affected
by clock changes. Run observes subprocess launch-to-reap with a same-process
monotonic clock, excluding log I/O. Historical file timestamps cannot establish
a duration; independent elapsed receipts may. Event lines are limited to 16384
UTF-8 bytes; task/evidence strings to 4096 characters. Replay is linear in log
size and retains one start/end pair per task. Only the listed schema fields
are accepted; keep provenance in evidence.
Log strings are data and are never executed. SIGKILL/SIGTERM or a host crash
can leave a run open; close it with --unknown and an evidence reason.

Examples:
  python3 scripts/task-time.py start 'release validation'
  python3 scripts/task-time.py end UUID --outcome succeeded
  python3 scripts/task-time.py run 'focused Python tests' -- python3 -m unittest
  python3 scripts/task-time.py start 'old receipt' --at unknown --evidence PATH
  python3 scripts/task-time.py end UUID --at unknown --elapsed 12.34 --evidence PATH
""", formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--log", type=Path, default=DEFAULT_LOG)
    commands = parser.add_subparsers(dest="action", required=True)
    start = commands.add_parser("start", help="append a unique task start; print its JSON event")
    start.add_argument("task")
    end = commands.add_parser("end", help="append exactly one terminal event")
    end.add_argument("id")
    end.add_argument("--outcome", choices=sorted(OUTCOMES), default="succeeded")
    duration = end.add_mutually_exclusive_group()
    duration.add_argument("--elapsed", help="independently observed elapsed seconds from a receipt")
    duration.add_argument("--unknown", action="store_true", help="explicitly leave duration unknown")
    for command in (start, end):
        command.add_argument("--at", help="observed UTC event stamp, or unknown; default now")
        command.add_argument("--stamp-basis", choices=["observed_utc", "file_timestamp"], default="observed_utc")
        command.add_argument("--evidence", help="receipt/path/hash or reason; required for retrospective/unknown data")
    status = commands.add_parser("status", help="validate complete history; print each task as JSON")
    status.add_argument("id", nargs="?")
    run = commands.add_parser("run", help="observe one command without a shell; forward its exit code")
    run.add_argument("task")
    run.add_argument("command", nargs=argparse.REMAINDER, help="-- executable args...")
    args = parser.parse_args(argv)
    try:
        if args.action in ("start", "end"):
            at, basis = resolve_stamp(args)
            require(args.evidence is not None or (args.at is None and not getattr(args, "unknown", False)
                    and getattr(args, "elapsed", None) is None), "retrospective/unknown timing requires --evidence")
            evidence = args.evidence or f"live manual {args.action}"
            if args.action == "start":
                event = start_task(args.log, args.task, at, basis, evidence)
            else:
                elapsed = elapsed_ns(args.elapsed) if args.elapsed is not None else None
                duration_basis = "observed_elapsed" if elapsed is not None else ("unknown" if args.unknown else None)
                event = end_task(args.log, args.id, at, basis, args.outcome, evidence, elapsed, duration_basis)
            print(json.dumps(event, sort_keys=True))
        elif args.action == "status":
            tasks = read_tasks(args.log)
            require(args.id is None or args.id in tasks, f"unknown task: {args.id}")
            for task_id, task in tasks.items():
                if args.id is not None and task_id != args.id:
                    continue
                print(json.dumps(dict(id=task_id, task=task["start"]["task"],
                                      start=task["start"], end=task["end"],
                                      outcome=task["end"]["outcome"] if task["end"] else "running",
                                      duration_ns=task["end"]["duration_ns"] if task["end"] else None,
                                      duration_basis=task["end"]["duration_basis"] if task["end"] else "unknown"), sort_keys=True))
        else:
            command = args.command[1:] if args.command[:1] == ["--"] else args.command
            require(bool(command), "run requires -- executable args...")
            start = start_task(args.log, args.task, utc_now(), evidence="task-time run observed start")
            print(json.dumps(start, sort_keys=True), file=sys.stderr, flush=True)
            begin = time.monotonic_ns()
            outcome, evidence = "failed", "task-time run observed subprocess exit"
            try:
                code = subprocess.run(command, check=False).returncode
                outcome = "succeeded" if code == 0 else "failed"
                evidence += f"; exit code {code}"
                code = 128 - code if code < 0 else code
            except KeyboardInterrupt:
                code, outcome = 130, "interrupted"
                evidence = "task-time run observed KeyboardInterrupt"
            except OSError as error:
                code = 127
                evidence = f"task-time run could not launch subprocess: {error}"
            duration_ns = time.monotonic_ns() - begin
            end = end_task(args.log, start["id"], utc_now(), "observed_utc", outcome,
                           evidence, duration_ns, "monotonic")
            print(json.dumps(end, sort_keys=True), file=sys.stderr)
            return code
        return 0
    except (ValueError, OSError) as error:
        print(f"task-time: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
