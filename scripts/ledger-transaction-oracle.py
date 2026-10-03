#!/usr/bin/env python3
"""Independent finite ledger data, never a SLIM/log parser or native runner.

Import defines data/functions only. Freeze is explicit, fresh, bounded and has
no compiler/CC invocation. Native checking remains the sole source authority.
"""
import argparse
from dataclasses import dataclass, replace
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import shutil
import signal
import sys
import time

MIN = -9223372036854775808
MAX = 9223372036854775807
MIB = 1048576
SOURCE = b"alice bob carol missing"
ALICE, BOB, CAROL, MISSING = (0, 5), (6, 9), (10, 15), (16, 23)
CASE_CAP, FILE_CAP = 192, 128
MODEL_CAP, FREEZE_CAP, SOURCE_CAP, TOOL_CAP = 4*MIB, 16*MIB, MIB, 128*MIB
GLOBAL_SECONDS, CHILD_SECONDS = 900, 60
HELPER_PATH = "scripts/verify-project-impact.py"
HELPER_SHA = "6e412c54726f23392003dfd82c20c159743a58ea6506c00274577f59dfb1b1be"
CONTROL_PATHS = {
    "oracle": "scripts/ledger-transaction-oracle.py",
    "verifier": "scripts/verify-ledger-transaction.py",
    "generator": "scripts/ledger-transaction-probe.py",
    "prefix": "tests/fixtures/ledger_transaction_probe_prefix.slim",
    "baseline": "tests/fixtures/ledger_transaction_baseline_state.slim",
    "rfc": "design/rfcs/0171-ledger-transaction-overflow-preflight.md",
}
BASELINE_SHA = "0316e7900060a061ec16f5859905a28dfecceec383828bd9f0a58a2955efc69e"
PREFIX_SHA = "3f3c1d7621d09fedfdfa66d461bf2c947e4321411308ba255aac5c13557a0bea"
GENERATOR_SHA = "e09de149877002ef4bd955740feb5172ca21f497fac1c5ff88cfb01483241590"
STATE_PATH = "library/applications/ledger/state.slim"
LEDGER_MANIFEST_SHA = "376145c7517aa1a65180d7d9b44949ea5a8a479752b7643701b12a14e46891b1"
LEDGER_PATHS = (
    "library/ledger.project", "library/applications/ledger/main.slim",
    "library/applications/ledger/emit.slim", "library/applications/ledger/model.slim",
    "library/applications/ledger/parser.slim", "library/applications/ledger/state.slim",
    "library/experimental/ascii.slim", "library/experimental/bytes.slim",
    "library/experimental/decimal.slim", "library/experimental/text.slim",
)
# This literal registry is the accepted RFC170 registry; no collector import.
COLLECTOR_PATHS = (
    "library/applications/catalog/catalog.slim", "library/applications/catalog/emit.slim",
    "library/applications/catalog/model.slim", "library/applications/catalog/reconcile.slim",
    "library/applications/project_impact/closure.slim", "library/applications/project_impact/data.slim",
    "library/applications/project_impact/input.slim", "library/applications/project_impact/main.slim",
    "library/applications/project_impact/model.slim", "library/applications/project_impact/prepare.slim",
    "library/applications/project_impact/report.slim", "library/applications/project_input/main.slim",
    "library/applications/workplan/load.slim", "library/applications/workplan/model.slim",
    "library/components/project_input_data.slim", "library/components/project_input_emit.slim",
    "library/components/project_input_limits.slim", "library/components/project_input_model.slim",
    "library/components/records.slim", "library/experimental/ascii.slim",
    "library/experimental/byte_index.slim", "library/experimental/bytes.slim",
    "library/experimental/decimal.slim", "library/experimental/netstring.slim",
    "library/experimental/text.slim", "library/project-impact.project", "project-input.project",
    "runtime/slim_rt.c", "runtime/slim_rt.h", "scripts/project-impact-context.py",
    "scripts/project-input-inventory.py", "selfhost/check.slim", "selfhost/codegen.slim",
    "selfhost/control.slim", "selfhost/effects.slim", "selfhost/format.slim", "selfhost/identity.slim",
    "selfhost/ir.slim", "selfhost/memory.slim", "selfhost/ownership.slim", "selfhost/parallel.slim",
    "selfhost/project.slim", "selfhost/ranges.slim", "selfhost/retained.slim",
    "selfhost/scheduler.slim", "selfhost/syntax.slim", "selfhost/text.slim",
    "selfhost/typing.slim", "selfhost/validate.slim",
)
READSET = tuple(sorted(set(LEDGER_PATHS + COLLECTOR_PATHS + (HELPER_PATH,))))

@dataclass(frozen=True)
class Account:
    start: int
    end: int
    balance: int
    opened: bool
    credits: int
    debits: int

    def fields(self):
        return [self.start, self.end, self.balance, self.opened, self.credits, self.debits]

@dataclass(frozen=True)
class Command:
    kind: str
    first: tuple
    second: tuple | None
    amount: int

    def fields(self):
        return {"kind": self.kind, "first": list(self.first),
                "second": None if self.second is None else list(self.second), "amount": self.amount}

def require(condition, message):
    if not condition:
        raise ValueError(message)

def identity(data):
    return {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}

def canonical(value):
    return (json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=True) + "\n").encode("ascii")

def utc():
    return datetime.now(timezone.utc).isoformat(timespec="microseconds")

def span_name(span):
    first, last = span
    require(type(first) is int and type(last) is int and 0 <= first <= last <= len(SOURCE), "span domain")
    return SOURCE[first:last]

def checked_domain(accounts, command):
    require(command.kind in ("Open", "Credit", "Debit", "Transfer", "Close"), "command kind")
    require(type(command.amount) is int and MIN <= command.amount <= MAX, "amount domain")
    span_name(command.first)
    if command.second is not None:
        span_name(command.second)
    names = []
    for item in accounts:
        require(type(item.opened) is bool, "open domain")
        require(all(type(n) is int and MIN <= n <= MAX for n in
                    (item.balance, item.credits, item.debits)), "account scalar domain")
        names.append(span_name((item.start, item.end)))
    require(len(set(names)) == len(names), "unique names")

def mathematical_outcome(accounts, command):
    """Original priorities, then unbounded prospective values/range membership.

    This intentionally does not implement the proposed MAX-amount predicates.
    It models only explicitly constructed records/commands, never source syntax.
    """
    checked_domain(accounts, command)
    names = tuple(span_name((item.start, item.end)) for item in accounts)
    first = names.index(span_name(command.first)) if span_name(command.first) in names else None
    second = None
    if command.second is not None and span_name(command.second) in names:
        second = names.index(span_name(command.second))
    amount = command.amount
    if command.kind == "Open":
        if amount < 0:
            return 31, accounts
        if first is not None:
            return 32, accounts
        return None, accounts + (Account(*command.first, amount, True, 0, 0),)
    if command.kind == "Close":
        if first is None:
            return 41, accounts
        old = accounts[first]
        if not old.opened:
            return 42, accounts
        if old.balance > 0:
            return 43, accounts
        changes = {first: replace(old, opened=False)}
    elif command.kind == "Credit":
        if first is None or amount <= 0:
            return 33, accounts
        old = accounts[first]
        if not old.opened:
            return 34, accounts
        changes = {first: replace(old, balance=old.balance + amount, credits=old.credits + 1)}
    elif command.kind == "Debit":
        if first is None or amount <= 0:
            return 35, accounts
        old = accounts[first]
        if not old.opened:
            return 36, accounts
        if old.balance < amount:
            return 37, accounts
        changes = {first: replace(old, balance=old.balance - amount, debits=old.debits + 1)}
    else:
        if first is None or second is None or amount <= 0:
            return 38, accounts
        if first == second:
            return 39, accounts
        left, right = accounts[first], accounts[second]
        if not left.opened or not right.opened or left.balance < amount:
            return 40, accounts
        changes = {first: replace(left, balance=left.balance - amount, debits=left.debits + 1),
                   second: replace(right, balance=right.balance + amount, credits=right.credits + 1)}
    if any(n < MIN or n > MAX for item in changes.values()
           for n in (item.balance, item.credits, item.debits)):
        return 44, accounts
    return None, tuple(changes.get(index, item) for index, item in enumerate(accounts))

def base_accounts():
    return (Account(*ALICE, 10, True, 2, 3), Account(*BOB, 19, True, 6, 5),
            Account(*CAROL, 7, False, -3, 11))

def altered(accounts, index, **fields):
    return tuple(replace(item, **fields) if cursor == index else item for cursor, item in enumerate(accounts))

def state_bytes(label, code, accounts):
    result = "accepted" if code is None else "rejected " + str(code)
    lines = ["case " + label + " " + result, "accounts " + str(len(accounts))]
    for item in accounts:
        lines.append("account " + " ".join(str(value) for value in
                     (item.start, item.end, item.balance, int(item.opened), item.credits, item.debits)))
    return ("\n".join(lines) + "\n").encode("ascii")

def state_cases():
    cases = []
    def add(label, accounts, command, forced=None):
        code, post = mathematical_outcome(accounts, command)
        if forced is not None:
            require(code == forced, "literal competing-error expectation")
        if code is not None:
            require(post == accounts, "complete rejected vector preservation")
        require(post[2] == accounts[2], "third account preservation")
        cases.append({"label": label, "accounts": [item.fields() for item in accounts],
                      "command": command.fields(), "code": code,
                      "post": [item.fields() for item in post],
                      "stdout_hex": state_bytes(label, code, post).hex(), "returncode": 0, "stderr_hex": ""})
    ordinal = 0
    for balance in (MIN, -1, 0, MAX-1, MAX):
        for amount in (1, MAX):
            for counter in (MIN, -1, 0, MAX-1, MAX):
                ordinal += 1
                accounts = altered(base_accounts(), 0, balance=balance, credits=counter, debits=MAX)
                add("C" + str(ordinal).zfill(3), accounts, Command("Credit", ALICE, None, amount))
    ordinal = 0
    for balance in (MIN, -1, 0, 1, MAX):
        for amount in (1, MAX):
            for counter in (MIN, -1, 0, MAX-1, MAX):
                ordinal += 1
                accounts = altered(base_accounts(), 0, balance=balance, debits=counter, credits=MAX)
                add("D" + str(ordinal).zfill(3), accounts, Command("Debit", ALICE, None, amount))
    ordinal = 0
    for balance in (MIN, -1, 0, MAX-1, MAX):
        for amount in (1, MAX):
            for from_debits, to_credits in ((0, 0), (MAX, 0), (0, MAX), (MAX, MAX)):
                ordinal += 1
                accounts = altered(base_accounts(), 0, balance=MAX, debits=from_debits, credits=MAX)
                accounts = altered(accounts, 1, balance=balance, credits=to_credits, debits=MAX)
                add("T" + str(ordinal).zfill(3), accounts, Command("Transfer", ALICE, BOB, amount))
    for ordinal, counters in enumerate(((MIN, MIN), (MIN, -1), (-1, MIN), (-1, -1)), 1):
        accounts = altered(base_accounts(), 0, balance=MAX, debits=counters[0], credits=MIN)
        accounts = altered(accounts, 1, balance=MIN, credits=counters[1], debits=MIN)
        add("TN" + str(ordinal).zfill(2), accounts, Command("Transfer", ALICE, BOB, MAX))
    base = base_accounts()
    closed = altered(base, 0, opened=False, balance=MAX, credits=MAX, debits=MAX)
    exhausted = altered(base, 0, balance=0, debits=MAX)
    to_closed = altered(altered(base, 0, balance=MAX, debits=MAX), 1, opened=False, balance=MAX, credits=MAX)
    earlier = (
        ("E01", base, Command("Open", ALICE, None, -1), 31),
        ("E02", base, Command("Open", MISSING, None, -1), 31),
        ("E03", base, Command("Open", ALICE, None, 0), 32),
        ("E04", base, Command("Open", MISSING, None, MAX), None),
        ("E05", base, Command("Open", MISSING, None, 0), None),
        ("E06", base, Command("Close", MISSING, None, 0), 41),
        ("E07", closed, Command("Close", ALICE, None, 0), 42),
        ("E08", altered(base, 0, balance=1), Command("Close", ALICE, None, 0), 43),
        ("E09", altered(base, 0, balance=0), Command("Close", ALICE, None, 0), None),
        ("E10", altered(base, 0, balance=MIN), Command("Close", ALICE, None, 0), None),
        ("E11", closed, Command("Credit", MISSING, None, 1), 33),
        ("E12", closed, Command("Credit", ALICE, None, 0), 33),
        ("E13", closed, Command("Credit", ALICE, None, MIN), 33),
        ("E14", closed, Command("Credit", ALICE, None, 1), 34),
        ("E15", closed, Command("Debit", MISSING, None, 1), 35),
        ("E16", closed, Command("Debit", ALICE, None, 0), 35),
        ("E17", closed, Command("Debit", ALICE, None, MIN), 35),
        ("E18", closed, Command("Debit", ALICE, None, 1), 36),
        ("E19", exhausted, Command("Debit", ALICE, None, 1), 37),
        ("E20", to_closed, Command("Transfer", MISSING, BOB, 1), 38),
        ("E21", to_closed, Command("Transfer", ALICE, MISSING, 1), 38),
        ("E22", closed, Command("Transfer", ALICE, ALICE, 0), 38),
        ("E23", closed, Command("Transfer", ALICE, ALICE, 1), 39),
        ("E24", closed, Command("Transfer", ALICE, BOB, 1), 40),
        ("E25", to_closed, Command("Transfer", ALICE, BOB, 1), 40),
        ("E26", altered(to_closed, 1, opened=True), Command("Transfer", ALICE, BOB, MAX), 44),
        ("E27", altered(altered(to_closed, 1, opened=True), 0, balance=1), Command("Transfer", ALICE, BOB, MAX), 40),
    )
    for label, accounts, command, expected in earlier:
        add(label, accounts, command, expected)
        require(cases[-1]["code"] == expected, "literal precedence code")
    require(len(cases) == 171 and len({case["label"] for case in cases}) == 171, "state case count")
    return cases

def cli_cases():
    # Explicit literal input/output bytes, never parsed or produced by a reference.
    rows = (
        ("R01", b"open alice 9223372036854775807\ncredit alice 1\n", 0,
         b"accepted 1 rejected 1\naccount alice balance 9223372036854775807 open yes credits 0 debits 0\ntotal 9223372036854775807\n"),
        ("R02", b"open alice 9223372036854775806\ncredit alice 1\n", 0,
         b"accepted 2 rejected 0\naccount alice balance 9223372036854775807 open yes credits 1 debits 0\ntotal 9223372036854775807\n"),
        ("R03", b"open alice 10\nopen bob 0\ntransfer alice bob 20\n", 0,
         b"accepted 2 rejected 1\naccount alice balance 10 open yes credits 0 debits 0\naccount bob balance 0 open yes credits 0 debits 0\ntotal 10\n"),
        ("R04", b"credit missing 1\n", 0, b"accepted 0 rejected 1\ntotal 0\n"),
        ("R05", b"open alice 1\ndebit alice 1\nclose alice\n", 0,
         b"accepted 3 rejected 0\naccount alice balance 0 open no credits 0 debits 1\ntotal 0\n"),
        ("R06", b"open alice 9223372036854775808\n", 1, b"error 24 at 29\n"),
        ("R07", b"open alice 0\nclose alice\ncredit alice 1\ndebit alice 1\ntransfer alice alice 1\n", 0,
         b"accepted 2 rejected 3\naccount alice balance 0 open no credits 0 debits 0\ntotal 0\n"),
        ("R08", b"open alice 100\nopen bob 50\ntransfer alice bob 25\ndebit bob 100\ncredit alice 5\ndebit bob 75\nclose bob\nopen alice 1\n", 0,
         b"accepted 6 rejected 2\naccount alice balance 80 open yes credits 1 debits 1\naccount bob balance 0 open no credits 1 debits 1\ntotal 80\n"),
    )
    return [{"label": label, "input_hex": data.hex(), "returncode": status,
             "stdout_hex": output.hex(), "stderr_hex": ""} for label, data, status, output in rows]

def partial_cases():
    return [
        {"label": "P01", "role": "ledger", "input_hex":
         b"open alice 9223372036854775807\nopen bob 1\n".hex(), "returncode": 70,
         "stdout_hex": "", "stderr_hex": b"SLIM runtime trap: I64 addition overflow\n".hex(),
         "classification": "retained checked partial aggregate total; not transaction rejection44"},
        {"label": "P02", "role": "probe-total-negative-index", "input_hex": "", "returncode": 70,
         "stdout_hex": "", "stderr_hex": b"SLIM runtime trap: vector index out of bounds\n".hex(),
         "classification": "retained checked partial forged total cursor; outside valid caller domain"},
    ]

def baseline_case():
    return {"label": "B01", "input_hex": b"open alice 9223372036854775807\ncredit alice 1\n".hex(),
            "returncode": 70, "stdout_hex": "", "stderr_hex": b"SLIM runtime trap: I64 addition overflow\n".hex(),
            "classification": "ordinary actual before-source reproduction; prospective expectation, not observed"}

def diagnostic_cases():
    header = (b"module ledger_probe\n\nfn misuse(source: Bytes, command: ledger_model.Command, "
              b"accounts: @Vec[ledger_model.Account]) -> ledger_model.Applied effects[")
    tail = b"]:\n  ledger_state.apply(source, command, @accounts)\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
    negative, positive = header + b"alloc" + tail, header + b"alloc, partial" + tail
    start = negative.index(b"ledger_state.apply")
    require(start == 157 and start + len(b"ledger_state.apply") == 175, "literal caller byte extent")
    diagnostic = b"E0343@ledger_probe@157:175\n"
    return [{"label": "Q01", "source_hex": positive.hex(), "returncode": 0, "stdout_hex": "", "stderr_hex": ""},
            {"label": "Q02", "source_hex": negative.hex(), "returncode": 1,
             "stdout_hex": diagnostic.hex(), "stderr_hex": ""}]

def native_labels():
    labels = ["cleanup-control", "emit-baseline-ledger", "build-baseline-ledger-ordinary", "baseline-max-credit-ordinary",
              "check-ledger", "check-probe", "emit-ledger", "emit-probe",
              "build-ledger-ordinary", "build-ledger-sanitized", "build-probe-ordinary", "build-probe-sanitized",
              "states-ordinary", "states-sanitized"]
    labels.extend("replay-" + str(index).zfill(2) + "-" + variant for index in range(1, 9)
                  for variant in ("ordinary", "sanitized"))
    labels.extend("partial-" + str(index).zfill(2) + "-" + variant for index in range(1, 3)
                  for variant in ("ordinary", "sanitized"))
    labels.extend(("diagnostic-positive", "diagnostic-missing-partial"))
    labels.extend("context-before-" + name for name in ("credit_account", "debit_account", "transfer"))
    labels.extend("impact-context-" + role for role in
                  ("emit-producer", "build-producer", "emit-impact", "build-impact", "capture-before", "capture-after", "impact"))
    require(len(labels) == 46 and len(set(labels)) == 46, "native label count")
    return labels

def model():
    states, replays, partials, diagnostics = state_cases(), cli_cases(), partial_cases(), diagnostic_cases()
    require(len(states) + len(replays) + len(partials) + len(diagnostics) + 1 == 184 <= CASE_CAP, "case cap")
    require(len(LEDGER_PATHS) == 10 and len(COLLECTOR_PATHS) == 49 and len(READSET) == 56, "literal readset count")
    return {"schema": 1, "contract": "accepted-RFC-0171", "source_hex": SOURCE.hex(),
            "states": states, "replays": replays, "partials": partials, "diagnostics": diagnostics,
            "baseline": baseline_case(), "data_cases": 184, "native_labels": native_labels(),
            "collector_wrapper_label": "impact-context-cli-wrapper; in-process captured-byte canonical main, fixed sys.argv, redirected streams; overlaps seven native roles",
            "expected_candidate_names": ["ledger", "ledger_emit", "ledger_state"],
            "expected_changed_names": ["ledger_state"], "expected_manifest_changed": False,
            "context_selectors": ["ledger_state.credit_account", "ledger_state.debit_account", "ledger_state.transfer"],
            "limits": {"cases": CASE_CAP, "files": FILE_CAP, "model_bytes": MODEL_CAP,
                       "freeze_bytes": FREEZE_CAP, "source_bytes": SOURCE_CAP, "tool_bytes": TOOL_CAP,
                       "global_seconds": GLOBAL_SECONDS, "child_seconds": CHILD_SECONDS,
                       "emit_stdout_bytes": 16*MIB, "other_stdout_bytes": 8*MIB, "stderr_bytes": 256*1024},
            "unknown": ["actual source acceptance and native outputs", "general workflow effectiveness",
                        "aggregate/process-count safety outside fixed partial controls", "malformed caller totality",
                        "physical memory/leaks", "atomic live capture/ABA", "transitive toolchain/SDK",
                        "model tokens/calls/active time", "context sufficiency/saved compiler work"]}

def repository_root():
    for parent in Path(__file__).resolve().parents:
        if (parent / "design/FEATURE_POLICY.md").is_file():
            return parent
    raise ValueError("repository root unavailable")

def read_bounded(path, cap):
    require(path.is_file() and not path.is_symlink(), "ordinary source file")
    with path.open("rb") as stream:
        data = stream.read(cap + 1)
    require(len(data) <= cap, "source/file byte cap")
    return data

def tool_identity(path):
    require(path.is_file() and len(os.fsencode(path)) <= 4096, "direct tool path domain")
    count, hashed = 0, hashlib.sha256()
    with path.open("rb") as stream:
        while True:
            chunk = stream.read(min(65536, TOOL_CAP-count+1))
            if not chunk:
                break
            count += len(chunk)
            require(count <= TOOL_CAP, "direct tool byte cap")
            hashed.update(chunk)
    return {"bytes": count, "sha256": hashed.hexdigest(), "path_hex": os.fsencode(path).hex()}

def freeze(output, compiler=None, cc=None, admission=None):
    """Fresh bounded data-only hold of current bytes and one fixed baseline body."""
    started, begin = utc(), time.monotonic_ns()
    root = repository_root()
    destination = Path(output).absolute()
    parent = destination.parent.resolve(strict=True)
    build = (root / "build/overnight-ledger-transaction").resolve(strict=True)
    require(parent == build or build in parent.parents, "fixed ignored output containment")
    destination = parent / destination.name
    require(not destination.exists() and not destination.is_symlink(), "fresh output")
    compiler = Path(compiler).resolve(strict=True) if compiler else (root / "build/toolchain/slimc").resolve(strict=True)
    cc = Path(cc or shutil.which("cc") or "").resolve(strict=True)
    python = Path(sys.executable).resolve(strict=True)
    tools = {name: tool_identity(path) for name, path in (("compiler", compiler), ("cc", cc), ("python", python))}
    own_path = Path(__file__).resolve()
    require(own_path == root/CONTROL_PATHS["oracle"], "canonical oracle location")
    controls = {name: read_bounded(root/path, SOURCE_CAP) for name,path in CONTROL_PATHS.items()}
    control_pins = {name:identity(data) for name,data in controls.items()}
    own_bytes = controls["oracle"]
    require(b"Status: accepted" in controls["rfc"].splitlines()[:24] and
            b"Implementation: complete" in controls["rfc"].splitlines()[:24], "current accepted implemented RFC171")
    for name,pin in (("baseline",BASELINE_SHA),("prefix",PREFIX_SHA),("generator",GENERATOR_SHA)):
        require(control_pins[name]["sha256"] == pin, "fixed baseline/probe source identity")
    sources = {name: read_bounded(root / name, SOURCE_CAP) for name in READSET}
    require(identity(sources[HELPER_PATH])["sha256"] == HELPER_SHA, "fixed process helper identity")
    require(identity(sources["library/ledger.project"])["sha256"] == LEDGER_MANIFEST_SHA, "fixed ledger declaration identity")
    source_pins = {name: identity(data) for name, data in sources.items()}
    if admission is not None:
        require(admission == {"sources":source_pins,"controls":control_pins,"tools":tools},
                "complete caller admission before captured oracle execution")
    expected = model()
    expected.update(source_pins=source_pins, tools=tools, oracle=identity(own_bytes),
                    control_sources=control_pins, baseline_state=control_pins["baseline"])
    model_bytes = canonical(expected)
    require(len(model_bytes) <= MODEL_CAP, "model cap")
    files = {"oracle.py":own_bytes,"verifier.py":controls["verifier"],"generator.py":controls["generator"],
             "prefix.slim":controls["prefix"],"rfc.md":controls["rfc"],"baseline-state.slim":controls["baseline"],
             "model.json": model_bytes,
             "sources-before.json": canonical(source_pins), "tools-before.json": canonical(tools),
             "states.expected.bin": b"".join(bytes.fromhex(case["stdout_hex"]) for case in expected["states"])}
    for name, data in sources.items():
        files["sources/" + name] = data
    # Two independently retained complete before trees; fixed paths, opaque bytes.
    for prefix in ("before", "expected"):
        for name in LEDGER_PATHS:
            files[prefix + "/" + name] = controls["baseline"] if name == STATE_PATH else sources[name]
    for field, suffix in (("input_hex", "input"), ("stdout_hex", "stdout"), ("stderr_hex", "stderr")):
        files["baseline/B01." + suffix + ".bin"] = bytes.fromhex(expected["baseline"][field])
    for group, rows in (("replays", expected["replays"]), ("partials", expected["partials"]),
                        ("diagnostics", expected["diagnostics"])):
        for case in rows:
            stem = group + "/" + case["label"]
            files[stem + ".input.bin"] = bytes.fromhex(case.get("input_hex", case.get("source_hex", "")))
            files[stem + ".stdout.bin"] = bytes.fromhex(case["stdout_hex"])
            files[stem + ".stderr.bin"] = bytes.fromhex(case["stderr_hex"])
    require(len(files)+3 <= FILE_CAP, "prospective file cap including endpoint files and receipt")
    require(sum(map(len, files.values())) + 3*MIB <= FREEZE_CAP, "prospective byte cap reserves final files")
    destination.mkdir()
    inventory = {}
    for name, data in files.items():
        path = destination / name
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("xb") as stream:
            stream.write(data)
        inventory[name] = identity(data)
    after = {name: identity(read_bounded(root / name, SOURCE_CAP)) for name in READSET}
    after_tools = {name: tool_identity(path) for name, path in (("compiler", compiler), ("cc", cc), ("python", python))}
    require(after == source_pins and after_tools == tools, "source/tool drift")
    for name, data in (("sources-after.json", canonical(after)), ("tools-after.json", canonical(after_tools))):
        with (destination / name).open("xb") as stream:
            stream.write(data)
        inventory[name] = identity(data)
    after_controls = {name:identity(read_bounded(root/path,SOURCE_CAP)) for name,path in CONTROL_PATHS.items()}
    require(after_controls == control_pins, "current oracle/verifier/probe/baseline/contract drift")
    for name, value in inventory.items():
        require(identity(read_bounded(destination / name, FREEZE_CAP)) == value, "published artifact drift")
    # Final direct-tool/source endpoint follows publication/readback; ABA remains unknown.
    require({name: identity(read_bounded(root / name, SOURCE_CAP)) for name in READSET} == source_pins, "final source drift")
    require({name:identity(read_bounded(root/path,SOURCE_CAP)) for name,path in CONTROL_PATHS.items()} == control_pins,
            "final current control drift")
    require({name: tool_identity(path) for name, path in (("compiler", compiler), ("cc", cc), ("python", python))} == tools,
            "final tool drift")
    receipt = {"schema": 1, "status": "data-held-no-native", "started_utc": started, "finished_utc": utc(),
               "elapsed_ns": time.monotonic_ns()-begin, "model": identity(model_bytes), "files": inventory,
               "sources_before": source_pins, "sources_after": after, "tools_before": tools, "tools_after": after_tools,
               "oracle":identity(own_bytes),"controls_before":control_pins,"controls_after":after_controls,
               "native_commands": 0,
               "timing_scope": "data freeze through endpoint checks; receipt write/shutdown excluded"}
    receipt_bytes = canonical(receipt)
    require(len(receipt_bytes) <= MIB and len(inventory)+1 <= FILE_CAP and
            sum(value["bytes"] for value in inventory.values())+len(receipt_bytes) <= FREEZE_CAP, "final freeze caps")
    with (destination / "freeze.json").open("xb") as stream:
        stream.write(receipt_bytes)
    return {"model": identity(model_bytes), "freeze": identity(receipt_bytes), "files": len(inventory)+1,
            "bytes": sum(value["bytes"] for value in inventory.values())+len(receipt_bytes), "native_commands": 0}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("freeze",))
    parser.add_argument("--output", required=True)
    parser.add_argument("--compiler")
    parser.add_argument("--cc")
    args = parser.parse_args()
    def alarm(_signum, _frame):
        raise TimeoutError("fixed data preparation deadline")
    signal.signal(signal.SIGALRM, alarm)
    signal.setitimer(signal.ITIMER_REAL, GLOBAL_SECONDS)
    try:
        print(json.dumps(freeze(args.output, args.compiler, args.cc), sort_keys=True))
    finally:
        signal.setitimer(signal.ITIMER_REAL, 0)

if __name__ == "__main__":
    main()
