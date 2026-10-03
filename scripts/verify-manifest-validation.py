#!/usr/bin/env python3
"""Bounded RFC167 production work/parity/resource and explicit same-host guards.

--current regenerates fixed portable fixtures from the sealed data oracle and
checks current production work, output signatures and bounded resource probes.
--compare additionally requires an explicitly SHA-pinned baseline executable
and enforces the permanent 1.10 ratio budget with same-session noise controls.
--baseline retains the original local custody campaign; --prepare is text-only.
Neither fixture data nor instrumentation accepts SLIM. Native children remain
sequential under 1800/900/60/5-second caps and require a coordinated quiet slot.
"""

import argparse
import datetime
import hashlib
import importlib.util
import json
import math
import os
from pathlib import Path
import re
import selectors
import signal
import statistics
import subprocess
import sys
import time


ROOT = Path(__file__).resolve().parents[1]
ORACLE_PATH = ROOT / "scripts/manifest-validation-oracle.py"
PATH_ORACLE_PATH = ROOT / "scripts/manifest-path-oracle.py"
PATH_ORACLE_SHA = "10bb9416d3589cd823302c9245fe68e66059d1076da704282a8cc7ac127baa5a"
PATH_MODEL_SHA = "1f4c8bf85ea5c457451c0184c7710a5912c385c11a9e488f285c52757ed40726"
OLD_COMPILER_SHA = "ee6264e4482e45256064079759b110b6eba99dc73bb06657cdc204b6457c97d6"
OLD_C_SHA = "d199910449a78e0ec3afb05bd4e18c357389ebe27ed198f9241b241fbd96b711"
OLD_PROJECT_SHA = "f08e6c788a25aeb8124dfb9008f0671ade45d66305714bc149db686aa925d875"
SEALED_ORACLE_SHA = "492508410de8ed99455f2ab44066449556664e2f6aa316333482a5722a768cf7"
SEALED_DATA_SHA = "8589f7476ab274ceac3273db35d0cb6fa5cd1f27405ca9a612994e8c671fa82f"
SEALED_FIXTURE_RECEIPT_SHA = "341a7cb4f399e28d09b7ab4e64f4e77615e6371b303c0c8b7d92575686dfc864"
HELD_IDENTITIES_SHA = "aff3e585051de27994022ef8eced605f206057e2a1c873a10fa9b6114aa13ace"
COUNTER_CAP = 1_000_000_000
TRACE_CAP = 262143
COUNTER_PREFIX = b"SLIM_MANIFEST_WORK "
STDOUT_CAP = 8 * 1048576
STDERR_CAP = 256 * 1024
SAMPLES = 11


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def write_json(path, value):
    Path(path).write_text(json.dumps(value, sort_keys=True, indent=2) + "\n")


def require(condition, message):
    if not condition:
        raise ValueError(message)


def load_oracle():
    require(digest(ORACLE_PATH) == SEALED_ORACLE_SHA, "independent oracle changed after hold")
    spec = importlib.util.spec_from_file_location("slim_manifest_finite_oracle", ORACLE_PATH)
    require(spec is not None and spec.loader is not None, "fixed oracle module unavailable")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def load_path_model():
    source = PATH_ORACLE_PATH.read_bytes()
    require(hashlib.sha256(source).hexdigest() == PATH_ORACLE_SHA, "independent path oracle changed before import")
    spec = importlib.util.spec_from_loader("slim_manifest_path_oracle", loader=None)
    module = importlib.util.module_from_spec(spec)
    module.__file__ = str(PATH_ORACLE_PATH)
    sys.modules[spec.name] = module
    exec(compile(source, str(PATH_ORACLE_PATH), "exec"), module.__dict__)
    encoded = module.encoded_model(ROOT)
    require(len(encoded) <= 131072 and hashlib.sha256(encoded).hexdigest() == PATH_MODEL_SHA,
            "separately held path model changed before native dispatch")
    return module, json.loads(encoded)


def held_identities(directory):
    directory = directory.resolve()
    identity_path = directory / "identities.json"
    require(digest(identity_path) == HELD_IDENTITIES_SHA, "held original identity receipt changed")
    require(identity_path.stat().st_size <= 65536, "held identity metadata admission")
    value = json.loads(identity_path.read_bytes())
    require(isinstance(value, dict) and isinstance(value.get("files"), dict), "held identities shape")
    require(len(value["files"]) == 43, "held original scope must contain all 43 files")
    for name, expected in value["files"].items():
        require(isinstance(name, str) and not Path(name).is_absolute() and ".." not in Path(name).parts, "held identity path admission")
        path = directory / name
        require(path.resolve().is_relative_to(directory) and path.is_file(), "held file missing/escaping root")
        require(isinstance(expected, dict) and type(expected.get("bytes")) is int and expected["bytes"] >= 0, "held size metadata")
        require(isinstance(expected.get("sha256"), str) and re.fullmatch("[0-9a-f]{64}", expected["sha256"]) is not None, "held hash metadata")
        require(type(expected.get("executable")) is bool, "held mode metadata")
        require(path.stat().st_size == expected["bytes"] and digest(path) == expected["sha256"], "held source/artifact drift: " + name)
        require(bool(path.stat().st_mode & 0o111) == expected["executable"], "held executable-mode drift: " + name)
    require(value["files"]["build/toolchain/slimc"]["sha256"] == OLD_COMPILER_SHA, "wrong old compiler")
    require(value["files"]["build/toolchain/slimc.c"]["sha256"] == OLD_C_SHA, "wrong old generated C")
    require(value["files"]["selfhost/project.slim"]["sha256"] == OLD_PROJECT_SHA, "wrong old project source")
    return {"receipt_sha256": digest(identity_path), "files": value["files"]}


def native(arguments, output, identity, deadline, timeout=60, stdout_cap=STDOUT_CAP, stderr_cap=STDERR_CAP, environment=None):
    """Launch/reap one process group; cap both live streams before retaining bytes."""
    remaining = deadline - time.monotonic()
    require(remaining > 0, "campaign wall deadline reached before dispatch")
    timeout = min(timeout, remaining)
    started = time.perf_counter_ns()
    process = subprocess.Popen(arguments, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                               start_new_session=True, cwd=ROOT, env=environment)
    selector = selectors.DefaultSelector()
    buffers = {"stdout": bytearray(), "stderr": bytearray()}
    caps = {"stdout": stdout_cap, "stderr": stderr_cap}
    status = "complete"
    expires = time.monotonic() + timeout

    def stop():
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass

    try:
        for label, pipe in (("stdout", process.stdout), ("stderr", process.stderr)):
            os.set_blocking(pipe.fileno(), False)
            selector.register(pipe, selectors.EVENT_READ, label)
        while selector.get_map():
            if time.monotonic() >= expires:
                status = "timeout"
                stop()
                break
            for key, _ in selector.select(min(0.1, max(0, expires - time.monotonic()))):
                room = caps[key.data] - len(buffers[key.data])
                block = os.read(key.fileobj.fileno(), min(65536, room + 1))
                if not block:
                    selector.unregister(key.fileobj)
                    continue
                buffers[key.data].extend(block[:room])
                if len(block) > room:
                    status = "output-limit"
                    stop()
                    break
            if status != "complete":
                break
        try:
            process.wait(timeout=max(0.1, expires - time.monotonic()) if status == "complete" else 5)
        except subprocess.TimeoutExpired:
            status = "timeout"
            stop()
            process.wait(timeout=5)
    finally:
        # A normally exited leader can leave descendants with closed streams.
        stop()
        process.wait(timeout=5)
        selector.close()
        process.stdout.close()
        process.stderr.close()
    elapsed = time.perf_counter_ns() - started
    stdout, stderr = bytes(buffers["stdout"]), bytes(buffers["stderr"])
    (output / (identity + ".stdout")).write_bytes(stdout)
    (output / (identity + ".stderr")).write_bytes(stderr)
    record = {"id": identity, "argv": [str(item) for item in arguments],
              "process_status": status, "returncode": process.returncode,
              "elapsed_ns": elapsed, "timeout_seconds": timeout, "leader_pid": process.pid,
              "group_cleanup": "unconditional SIGKILL then direct wait",
              "stdout_bytes": len(stdout), "stderr_bytes": len(stderr),
              "stdout_sha256": hashlib.sha256(stdout).hexdigest(),
              "stderr_sha256": hashlib.sha256(stderr).hexdigest()}
    write_json(output / (identity + ".json"), record)
    return record, stdout, stderr


def native_cleanup_control(output, deadline, environment):
    code = r"""import os, signal
read_end, write_end = os.pipe()
child = os.fork()
if child == 0:
    os.close(read_end)
    for fd in (0, 1, 2): os.close(fd)
    os.write(write_end, b'x'); os.close(write_end)
    while True: signal.pause()
os.close(write_end)
assert os.read(read_end, 1) == b'x'
os.close(read_end)
os.write(1, (str(child) + '\n').encode())
"""
    result, stdout, stderr = native([sys.executable, "-c", code], output, "leader-exit-descendant", deadline,
                                    timeout=2, stdout_cap=128, stderr_cap=4096, environment=environment)
    require(result["process_status"] == "complete" and result["returncode"] == 0 and not stderr and
            re.fullmatch(rb"[1-9][0-9]*\n", stdout) is not None, "fixed descendant cleanup leader")
    child = int(stdout)
    expires = min(deadline, time.monotonic() + 1)
    while True:
        try:
            os.kill(child, 0)
        except ProcessLookupError:
            break
        require(time.monotonic() < expires, "closed-stream descendant still exists after unconditional group cleanup")
        time.sleep(0.01)
    return {"native": result, "child_pid": child, "descendant_absent_after_cleanup": True,
            "scope": "fixed leader exits while forked descendant has closed all standard streams; 2s leader/1s absence bound"}


def namespace_function(module, name):
    escape = lambda part: part.replace("_", "_0").replace(".", "_1")
    return "slim_fn_" + ("p" + escape(module) + "__" + escape(name)).replace("_", "_95")


COUNTER_FUNCTION = """static void slim_manifest_validation_add(uint64_t *value) {
if (*value >= UINT64_C(1000000000)) abort();
++*value;
}
"""


def instrument_old(source, counters):
    """Exact held-C anchors; instrumentation records calls and retains their order."""
    index = {name: position for position, name in enumerate(counters)}
    fn = lambda name: namespace_function("project", name)
    headers = {
        fn("find_unsorted_module"): "module_order_headers",
        fn("find_duplicate_module"): "duplicate_module_headers",
        fn("find_prior_module_name"): "prior_module_headers",
        fn("find_duplicate_path"): "path_module_headers",
        fn("find_prior_path"): "prior_path_headers",
        fn("find_unknown_import"): "unknown_module_headers",
        fn("find_unknown_import_in"): "unknown_import_headers",
        fn("find_reciprocal_cycle"): "cycle_module_headers",
        fn("module_cycle_imports"): "cycle_import_headers",
    }
    phase_calls = {
        (fn("report_manifest_rules"), fn("find_manifest_module")): 1,
        (fn("find_unknown_import_in"), fn("find_manifest_module")): 2,
        (fn("module_imports_name"), fn("find_manifest_module")): 3,
        (fn("module_cycle_imports"), fn("find_manifest_module")): 4,
        (fn("module_imports_name"), fn("imports_has_name")): 5,
        (fn("module_cycle_imports"), fn("imports_has_name")): 6,
    }
    observed = set(headers) | {fn("find_manifest_module"), fn("imports_has_name"), fn("parse_project_manifest")} | {parent for parent, _ in phase_calls}
    definitions = dict.fromkeys(observed, 0)
    loop_hits = dict.fromkeys(set(headers) | {fn("find_manifest_module"), fn("imports_has_name")}, 0)
    calls = dict.fromkeys(phase_calls, 0)
    pairs = {fn("find_prior_path"): "path_pairs", fn("find_prior_module_name"): "module_name_pairs"}
    pair_hits = dict.fromkeys(pairs, 0)
    comparisons, token_hits, main_hits = 0, 0, 0

    def increment(name):
        return f"slim_manifest_validation_add(&slim_manifest_validation_counts[{index[name]}]);\n"

    def switch(suffix):
        value = "switch (slim_manifest_validation_phase) {\n"
        for phase, prefix in ((1, "entry"), (2, "unknown_lookup"), (3, "cycle_unused_lookup"), (4, "cycle_actual_lookup")):
            value += f"case {phase}: " + increment(prefix + suffix) + "break;\n"
        return value + "default: abort();\n}\n"

    prefix = "#include <stdint.h>\n#include <stdio.h>\n#include <stdlib.h>\n"
    prefix += COUNTER_FUNCTION
    prefix += f"static uint64_t slim_manifest_validation_counts[{len(counters)}];\n"
    prefix += "static int slim_manifest_validation_phase;\n"
    prefix += f"static char slim_manifest_validation_trace[{TRACE_CAP + 1}];\nstatic size_t slim_manifest_validation_trace_length;\n"
    prefix += "static void slim_manifest_validation_call(int phase) {\n"
    prefix += f"if (phase < 1 || phase > 6 || slim_manifest_validation_trace_length >= {TRACE_CAP}) abort();\n"
    prefix += "slim_manifest_validation_trace[slim_manifest_validation_trace_length++] = (char)('0' + phase);\nslim_manifest_validation_phase = phase;\n}\n"
    prefix += "static void slim_manifest_validation_report(void) {\nfputs(\"SLIM_MANIFEST_WORK {\", stderr);\n"
    for position, name in enumerate(counters):
        separator = "" if not position else ","
        prefix += f'fprintf(stderr, "{separator}\\\"{name}\\\":%llu", (unsigned long long)slim_manifest_validation_counts[{position}]);\n'
    prefix += 'fprintf(stderr, ",\\\"trace\\\":\\\"%s\\\"}\\n", slim_manifest_validation_trace);\n}\n'
    output, active = [prefix], None
    for line in source.splitlines(keepends=True):
        definition = re.match(r"^static SLIM_UNUSED_FUNCTION \S+ (\w+)\(.* \{\n$", line)
        if definition:
            active = definition.group(1)
            if active in definitions:
                definitions[active] += 1
        selected = next(((key, phase) for key, phase in phase_calls.items()
                         if key[0] == active and "= " + key[1] + "(" in line), None)
        if selected:
            key, phase = selected
            calls[key] += 1
            output.append(f"slim_manifest_validation_call({phase});\n")
        spans = namespace_function("syntax", "spans_equal")
        if active in pairs and "= " + spans + "(" in line:
            pair_hits[active] += 1
            output.append(increment(pairs[active]))
        if active == fn("find_manifest_module") and "= " + spans + "(" in line:
            comparisons += 1
            output.append(switch("_comparisons"))
        output.append(line)
        if selected:
            output.append("slim_manifest_validation_phase = 0;\n")
        if active in loop_hits and line == "slim_recur: ;\n":
            loop_hits[active] += 1
            if active in headers:
                output.append(increment(headers[active]))
            elif active == fn("find_manifest_module"):
                output.append(switch("_headers"))
            else:
                output.append("switch (slim_manifest_validation_phase) {\ncase 5: " + increment("cycle_unused_list_headers") + "break;\ncase 6: " + increment("cycle_actual_list_headers") + "break;\ndefault: abort();\n}\n")
        if active == fn("parse_project_manifest") and "= " + namespace_function("syntax", "lex_data") + "(" in line:
            token_hits += 1
            output.append(f"if (slim_v_tokens_n6->len < 0 || (uint64_t)slim_v_tokens_n6->len > UINT64_C({COUNTER_CAP}) || slim_manifest_validation_counts[{index['manifest_tokens']}] != 0) abort();\n")
            output.append(f"slim_manifest_validation_counts[{index['manifest_tokens']}] = (uint64_t)slim_v_tokens_n6->len;\n")
        if line == "int main(int argc, char **argv) {\n":
            main_hits += 1
            output.append("if (atexit(slim_manifest_validation_report) != 0) abort();\n")
    require(all(value == 1 for value in definitions.values()), "old C definition anchors: " + str(definitions))
    require(all(value == 1 for value in loop_hits.values()), "old C loop anchors: " + str(loop_hits))
    require(all(value == 1 for value in calls.values()), "old C ordered call anchors: " + str(calls))
    require(all(value == 1 for value in pair_hits.values()) and comparisons == token_hits == main_hits == 1, "old C span/parse/main anchors")
    return "".join(output), {"definitions": definitions, "loops": loop_hits,
                             "ordered_calls": {str(key): value for key, value in calls.items()},
                             "pair_calls": pair_hits, "comparison_calls": comparisons,
                             "parse_calls": token_hits, "main_calls": main_hits}


def expect(row, result, stdout, stderr, command):
    if row["status"] is None:
        return                   # Held observation only, no invented old result.
    require(result["process_status"] == "complete", row["id"] + ": bounded invocation incomplete")
    require(result["returncode"] == row["status"] and not stderr, row["id"] + ": old status/stderr mismatch")
    if row["status"] or command == "check":
        require(stdout == bytes.fromhex(row["diagnostics_hex"]), row["id"] + ": old diagnostic mismatch")


def parse_work(stderr, expected):
    require(stderr.startswith(COUNTER_PREFIX) and stderr.endswith(b"\n") and stderr.count(b"\n") == 1, "work output shape")
    value = json.loads(stderr[len(COUNTER_PREFIX):])
    trace = value.pop("trace")
    require(all(type(item) is int and 0 <= item <= COUNTER_CAP for item in value.values()), "work counter type/range")
    require(isinstance(trace, str) and re.fullmatch("[1-6]{0,262143}", trace) is not None, "work trace type/range")
    require(value == expected["work"] and trace == expected["trace"], "independent old work/ordered-call oracle mismatch")
    return value


def counter_boundaries(cc, output, deadline):
    source = output / "counter-boundary.c"
    source.write_text("#include <stdint.h>\n#include <stdlib.h>\n#include <stdio.h>\n#include <errno.h>\n" + COUNTER_FUNCTION + """
int main(int argc, char **argv) {
if (argc != 2) return 64;
errno = 0; char *end = NULL;
unsigned long long parsed = strtoull(argv[1], &end, 10);
if (errno || !end || *end) return 64;
uint64_t value = (uint64_t)parsed;
slim_manifest_validation_add(&value);
printf("%llu\\n", (unsigned long long)value);
return 0;
}
""")
    binary = output / "counter-boundary"
    built, _, stderr = native([cc, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", source, "-o", binary], output, "counter-build", deadline)
    require(built["process_status"] == "complete" and built["returncode"] == 0 and not stderr, "counter boundary build failed")
    rows = []
    for value in (COUNTER_CAP - 1, COUNTER_CAP, COUNTER_CAP + 1, 2**64 - 1):
        result, stdout, _ = native([binary, str(value)], output, "counter-" + str(value), deadline, timeout=5)
        require(result["process_status"] == "complete", "counter boundary incomplete")
        if value == COUNTER_CAP - 1:
            require(result["returncode"] == 0 and stdout == str(COUNTER_CAP).encode() + b"\n", "counter cap-1 behavior")
        else:
            require(result["returncode"] == -signal.SIGABRT and not stdout, "counter refused value wrapped or published success")
        rows.append(result)
    return rows


def validate_fixture_files(fixtures, rows):
    for row in rows:
        directory = fixtures / row["id"]
        require(digest(directory / "slim.project") == row["manifest_sha256"], "fixture manifest drift")
        for name, expected in row["files"].items():
            path = directory / name
            require(path.stat().st_size == expected["bytes"] and digest(path) == expected["sha256"], "fixture source drift")


def sealed_fixtures(fixtures):
    fixtures = fixtures.resolve()
    identity_path = fixtures.parent / "fixtures-held-1-identities.json"
    require(digest(identity_path) == SEALED_FIXTURE_RECEIPT_SHA, "held fixture receipt changed")
    require(identity_path.stat().st_size <= 1048576, "fixture identity metadata admission")
    identities = json.loads(identity_path.read_bytes())
    require(identities.get("oracle_py_sha256") == SEALED_ORACLE_SHA and identities.get("oracle_json_sha256") == SEALED_DATA_SHA,
            "fixture/oracle association changed")
    require(isinstance(identities.get("files"), dict) and len(identities["files"]) == 3456, "fixed held fixture file scope")
    for name, expected in identities["files"].items():
        require(isinstance(name, str) and not Path(name).is_absolute() and ".." not in Path(name).parts, "fixture path admission")
        path = fixtures / name
        require(path.resolve().is_relative_to(fixtures) and path.is_file(), "held fixture missing/escaping root")
        require(isinstance(expected, str) and re.fullmatch("[0-9a-f]{64}", expected) is not None and digest(path) == expected, "held fixture drift")
    data_path = fixtures / "oracle.json"
    require(data_path.stat().st_size <= 1048576 and digest(data_path) == SEALED_DATA_SHA, "held oracle data drift")
    payload = json.loads(data_path.read_bytes())
    require(payload.get("counts") == {"geometric": 29, "control": 12, "malformed": 25} and len(payload.get("cases", [])) == 66, "held finite matrix shape")
    validate_fixture_files(fixtures, payload["cases"])
    return payload, {"receipt_sha256": digest(identity_path), "oracle_sha256": digest(data_path), "files": identities["files"]}


def baseline(arguments, oracle):
    output = arguments.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    started = time.monotonic()
    deadline = started + 1800
    source_pins = {"oracle": digest(ORACLE_PATH), "verifier": digest(__file__)}
    held = held_identities(arguments.baseline_root)
    receipt = {"schema": 1, "status": "running", "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
               "native_deadline_seconds": 1800, "malformed_deadline_seconds": 900,
               "per_child_seconds": 60, "source_pins": source_pins, "held": held,
               "work_rows": [], "golden_rows": [], "samples": [], "unfinished": []}
    receipt_path = output / "receipt.json"
    write_json(receipt_path, receipt)
    try:
        fixtures = arguments.fixtures_root.resolve()
        payload, fixture_identities = sealed_fixtures(fixtures)
        rows = payload["cases"]
        receipt["held_fixture_identities"] = fixture_identities
        receipt["oracle_json_sha256"] = digest(fixtures / "oracle.json")
        receipt["fixture_count"] = len(rows)
        validate_fixture_files(fixtures, rows)
        compiler = arguments.baseline_root.resolve() / "build/toolchain/slimc"
        cc = Path(arguments.cc).resolve()
        require(cc.is_file(), "fixed CC executable unavailable")
        receipt["cc"] = {"path": str(cc), "sha256": digest(cc)}
        generated = arguments.baseline_root.resolve() / "build/toolchain/slimc.c"
        observed_source, anchors = instrument_old(generated.read_text(), oracle.LEGACY)
        observed_c = output / "observed-old.c"
        observed_c.write_text(observed_source)
        observed = output / "observed-old"
        runtime = arguments.baseline_root.resolve() / "runtime"
        result, _, stderr = native([cc, "-std=c11", "-O2", "-DNDEBUG", "-Wall", "-Wextra", "-Werror",
                                   "-I", runtime, observed_c, runtime / "slim_rt.c", "-o", observed], output, "observer-build", deadline)
        require(result["process_status"] == "complete" and result["returncode"] == 0 and not stderr, "old observation build failed")
        receipt["observer"] = {"generated_sha256": digest(observed_c), "binary_sha256": digest(observed), "anchors": anchors}
        receipt["counter_boundaries"] = counter_boundaries(cc, output, deadline)
        work_dir = output / "work"
        work_dir.mkdir()
        structured = [row for row in rows if row["family"] != "malformed"]
        for row in structured:
            path = fixtures / row["id"] / "slim.project"
            plain, stdout, stderr = native([compiler, "check", path], work_dir, row["id"] + "-plain", deadline)
            expect(row, plain, stdout, stderr, "check")
            observed_result, observed_stdout, observed_stderr = native([observed, "check", path], work_dir, row["id"] + "-observed", deadline)
            require(observed_result["process_status"] == "complete" and observed_result["returncode"] == plain["returncode"] and observed_stdout == stdout, row["id"] + ": observer changed accepted result")
            actual = parse_work(observed_stderr, row["legacy"])
            if row["family"] == "paths":
                n = row["extents"]["N"]
                require(actual["path_pairs"] == n * (n - 1) // 2, "old quadratic baseline not observed")
            receipt["work_rows"].append({"id": row["id"], "work": actual, "trace": row["legacy"]["trace"], "ordinary": plain, "observed": observed_result})
            write_json(receipt_path, receipt)
        golden = output / "goldens"
        golden.mkdir()
        malformed_deadline = None
        for row in rows:
            if row["family"] == "malformed" and malformed_deadline is None:
                malformed_deadline = min(deadline, time.monotonic() + 900)
            for command in ("check", "fmt", "interfaces", None):
                label = "emit-c" if command is None else command
                path = fixtures / row["id"] / "slim.project"
                argv = [compiler] + ([] if command is None else [command]) + [path]
                timeout, limit = (5, 65536) if row["family"] == "malformed" else (60, STDOUT_CAP)
                result, stdout, stderr = native(argv, golden, row["id"] + "-" + label,
                                                malformed_deadline if row["family"] == "malformed" else deadline,
                                                timeout=timeout, stdout_cap=limit,
                                                stderr_cap=65536 if row["family"] == "malformed" else STDERR_CAP)
                expect(row, result, stdout, stderr, command)
                if row["status"] == 0 and command == "fmt":
                    require(stdout == path.read_bytes(), "old project formatting identity changed")
                if row["family"] == "malformed" and result["process_status"] != "complete":
                    receipt["unfinished"].append({"id": row["id"], "command": label, "reason": "bounded malformed termination/output unknown"})
                receipt["golden_rows"].append(result)
                write_json(receipt_path, receipt)
        selfhost_row = {"id": "held-selfhost", "status": 0, "diagnostics_hex": ""}
        selfhost_path = arguments.baseline_root.resolve() / "selfhost/slim.project"
        for command in ("check", "fmt", "interfaces", None):
            label = "emit-c" if command is None else command
            argv = [compiler] + ([] if command is None else [command]) + [selfhost_path]
            result, stdout, stderr = native(argv, golden, "held-selfhost-" + label, deadline)
            expect(selfhost_row, result, stdout, stderr, command)
            if command == "fmt":
                require(stdout == selfhost_path.read_bytes(), "old held selfhost formatting changed")
            receipt["golden_rows"].append(result)
            write_json(receipt_path, receipt)
        sample_dir = output / "samples"
        sample_dir.mkdir()
        if not arguments.no_timings:
            sample_rows = [row for row in rows if row["family"] not in ("control", "malformed")]
            sample_rows.append(next(row for row in rows if row["id"] == "prefix"))
            sample_rows.append(selfhost_row)
            for row in sample_rows:
                path = selfhost_path if row["id"] == "held-selfhost" else fixtures / row["id"] / "slim.project"
                samples = {"control_a": [], "control_b": []}
                for warmup in range(2):
                    result, stdout, stderr = native([compiler, "check", path], sample_dir, f"{row['id']}-warmup-{warmup}", deadline)
                    expect(row, result, stdout, stderr, "check")
                for sample in range(SAMPLES):
                    labels = tuple(samples) if sample % 2 == 0 else tuple(reversed(samples))
                    for label in labels:
                        result, stdout, stderr = native([compiler, "check", path], sample_dir, f"{row['id']}-{label}-{sample}", deadline)
                        expect(row, result, stdout, stderr, "check")
                        samples[label].append(result["elapsed_ns"])
                medians = {label: statistics.median(values) for label, values in samples.items()}
                ratio = medians["control_b"] / medians["control_a"]
                receipt["samples"].append({"id": row["id"], "samples_ns": samples, "median_ns": medians,
                                           "control_ratio": ratio, "host_noise_admitted": 1 / 1.10 <= ratio <= 1.10,
                                           "scope": "wrapper dispatch through process-group cleanup; includes fresh process and parent drain/cleanup; filesystem cache uncontrolled"})
                write_json(receipt_path, receipt)
        require(held_identities(arguments.baseline_root) == held, "held identities changed across baseline")
        require({"oracle": digest(ORACLE_PATH), "verifier": digest(__file__)} == source_pins, "measurement code changed across baseline")
        validate_fixture_files(fixtures, rows)
        require(sealed_fixtures(fixtures)[1] == fixture_identities, "held fixture identities changed across baseline")
        receipt["status"] = "pass-with-explicit-unknowns" if receipt["unfinished"] else "pass"
        receipt["native_dimensions_unknown"] = ["isolated frontend CPU", "external backend comparisons", "generated application runtime",
                                                 "peak RSS", "allocation count/bytes", "retained-session measurements", "candidate ratios"]
        receipt["timings_collected"] = not arguments.no_timings
    except BaseException as error:
        receipt["status"] = "failed"
        receipt["failure"] = {"type": type(error).__name__, "message": str(error)}
        raise
    finally:
        receipt["elapsed_ns"] = int((time.monotonic() - started) * 1_000_000_000)
        receipt["finished_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        write_json(receipt_path, receipt)
    print(json.dumps({"status": receipt["status"], "receipt": str(receipt_path), "sha256": digest(receipt_path)}, sort_keys=True))

# Exact finite old-production observations, held before candidate native execution.
# Keys bind the fixed oracle fixture ID and one of four fixed production commands.
PORTABLE_GOLDEN_STDERR_SHA256 = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
PORTABLE_GOLDENS = {
    'paths-16-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'paths-16-fmt': (0, '56b056377bc8f2c8396eb371a2b637f9a27a4143decfa42dcde11e77b9d7a8ca'),
    'paths-16-interfaces': (0, '562956c107b6c3a430e0b7834039878c92f4080352e073d148852727af1e5108'),
    'paths-16-emit-c': (0, 'ec6b0d4183f85c2acd9f6e50a076164faadab8a4b035d95e4d9375991a7d6709'),
    'duplicate-16-check': (1, 'b46801f83f4827c0817bb3a4346fe4d90dbe2f830676bfc73f15d4add46131f1'),
    'duplicate-16-fmt': (1, 'b46801f83f4827c0817bb3a4346fe4d90dbe2f830676bfc73f15d4add46131f1'),
    'duplicate-16-interfaces': (1, 'b46801f83f4827c0817bb3a4346fe4d90dbe2f830676bfc73f15d4add46131f1'),
    'duplicate-16-emit-c': (1, 'b46801f83f4827c0817bb3a4346fe4d90dbe2f830676bfc73f15d4add46131f1'),
    'paths-32-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'paths-32-fmt': (0, '16f3d8253f61a2c8c401026c24205023904d148d77a5e2eeb2c373b80bb8819f'),
    'paths-32-interfaces': (0, 'ae20c3c01c3eb630c4b7313e1dd632c18e489fb082708dd17d7eee0f112b3e01'),
    'paths-32-emit-c': (0, 'e88b7615ecd3e94332ad74909a59d1181d5122d95bd9170a958ae2496de8d136'),
    'duplicate-32-check': (1, 'cad24881bc57e143e92177e2cb54206b85db812f348e07acacf9b06bb57cb1dd'),
    'duplicate-32-fmt': (1, 'cad24881bc57e143e92177e2cb54206b85db812f348e07acacf9b06bb57cb1dd'),
    'duplicate-32-interfaces': (1, 'cad24881bc57e143e92177e2cb54206b85db812f348e07acacf9b06bb57cb1dd'),
    'duplicate-32-emit-c': (1, 'cad24881bc57e143e92177e2cb54206b85db812f348e07acacf9b06bb57cb1dd'),
    'paths-64-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'paths-64-fmt': (0, '5e77df140c0ee23503f4ed8a12f05d38e71bdb7dd19f54c3537866421206e867'),
    'paths-64-interfaces': (0, 'e9a471b3c6afc7901a50d4c6505fc137cc693d4f0cb41eb855562074d4e16b66'),
    'paths-64-emit-c': (0, '8849c08ff8ba8e45a0c740d00f814cec37118ba358a187853920a0035ed9bf92'),
    'duplicate-64-check': (1, '73e0b5ce3e4d3f83f687c8ee6fce6b52519d6c0bd3462c77bcc4dd147c0e5344'),
    'duplicate-64-fmt': (1, '73e0b5ce3e4d3f83f687c8ee6fce6b52519d6c0bd3462c77bcc4dd147c0e5344'),
    'duplicate-64-interfaces': (1, '73e0b5ce3e4d3f83f687c8ee6fce6b52519d6c0bd3462c77bcc4dd147c0e5344'),
    'duplicate-64-emit-c': (1, '73e0b5ce3e4d3f83f687c8ee6fce6b52519d6c0bd3462c77bcc4dd147c0e5344'),
    'paths-128-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'paths-128-fmt': (0, '333f2ed250ab29a981135ff7f02b81a60319901e974c091e854246d1fd528f72'),
    'paths-128-interfaces': (0, '8e8efb09ee38001a97544039c76902b299c9661908bbee05f572cdf2b3dc8f1d'),
    'paths-128-emit-c': (0, '5421276a5f402dbf3046b295d38e34cadee700a232422e2306a0f9451d8d1335'),
    'duplicate-128-check': (1, '96bed8b4047fa9268f3eab6759a4bb4fe15b8dbe5b1b735b5a013ef840396967'),
    'duplicate-128-fmt': (1, '96bed8b4047fa9268f3eab6759a4bb4fe15b8dbe5b1b735b5a013ef840396967'),
    'duplicate-128-interfaces': (1, '96bed8b4047fa9268f3eab6759a4bb4fe15b8dbe5b1b735b5a013ef840396967'),
    'duplicate-128-emit-c': (1, '96bed8b4047fa9268f3eab6759a4bb4fe15b8dbe5b1b735b5a013ef840396967'),
    'paths-256-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'paths-256-fmt': (0, '89cf775312b7684ae13054377ecce3f23486fc147a20d91dddd253834b4b3371'),
    'paths-256-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'paths-256-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'duplicate-256-check': (1, '70aa2f482fab19f44c6bd577ccc008f4de05eac2157e42c3eb94ef3f307bc84b'),
    'duplicate-256-fmt': (1, '70aa2f482fab19f44c6bd577ccc008f4de05eac2157e42c3eb94ef3f307bc84b'),
    'duplicate-256-interfaces': (1, '70aa2f482fab19f44c6bd577ccc008f4de05eac2157e42c3eb94ef3f307bc84b'),
    'duplicate-256-emit-c': (1, '70aa2f482fab19f44c6bd577ccc008f4de05eac2157e42c3eb94ef3f307bc84b'),
    'front-16-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'front-16-fmt': (0, '6c894cd5d16ceba10739c26023865a77d5d08e3b0c41360be2eefa3283915772'),
    'front-16-interfaces': (0, '562956c107b6c3a430e0b7834039878c92f4080352e073d148852727af1e5108'),
    'front-16-emit-c': (0, 'ec6b0d4183f85c2acd9f6e50a076164faadab8a4b035d95e4d9375991a7d6709'),
    'front-32-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'front-32-fmt': (0, 'bf4a98d81335aa4d3b4fb6fccfc9f710976d0ea2599c370587e3128a192dc66d'),
    'front-32-interfaces': (0, 'ae20c3c01c3eb630c4b7313e1dd632c18e489fb082708dd17d7eee0f112b3e01'),
    'front-32-emit-c': (0, 'e88b7615ecd3e94332ad74909a59d1181d5122d95bd9170a958ae2496de8d136'),
    'front-64-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'front-64-fmt': (0, 'c58675e57f5746940d03aac4564ba3a944a1f70bb5317fdbaa3dceb4e3e35aac'),
    'front-64-interfaces': (0, 'e9a471b3c6afc7901a50d4c6505fc137cc693d4f0cb41eb855562074d4e16b66'),
    'front-64-emit-c': (0, '8849c08ff8ba8e45a0c740d00f814cec37118ba358a187853920a0035ed9bf92'),
    'front-128-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'front-128-fmt': (0, 'c3970fd8ede1f82e14f2137eb93ae33876d98905b8c1d7aa6b5993205b78d457'),
    'front-128-interfaces': (0, '8e8efb09ee38001a97544039c76902b299c9661908bbee05f572cdf2b3dc8f1d'),
    'front-128-emit-c': (0, '5421276a5f402dbf3046b295d38e34cadee700a232422e2306a0f9451d8d1335'),
    'front-256-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'front-256-fmt': (0, '43cfa24d86ae88bd547dc6ecf891672cdf78ad517efdd4206da3fd6660d17195'),
    'front-256-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'front-256-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'back-16-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'back-16-fmt': (0, 'd8c1878fb00fe6260124cb769a09fb624f20514cb28dcc6a744747ec12097539'),
    'back-16-interfaces': (0, '562956c107b6c3a430e0b7834039878c92f4080352e073d148852727af1e5108'),
    'back-16-emit-c': (0, 'ec6b0d4183f85c2acd9f6e50a076164faadab8a4b035d95e4d9375991a7d6709'),
    'back-32-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'back-32-fmt': (0, '89ef0475f28e507deaa9f8c7b704fbf9235f199a2da7bef7d665e93b607a653f'),
    'back-32-interfaces': (0, 'ae20c3c01c3eb630c4b7313e1dd632c18e489fb082708dd17d7eee0f112b3e01'),
    'back-32-emit-c': (0, 'e88b7615ecd3e94332ad74909a59d1181d5122d95bd9170a958ae2496de8d136'),
    'back-64-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'back-64-fmt': (0, 'bd48be79815a4815f546e26135910144d7e48073011b708706652952bf9c412d'),
    'back-64-interfaces': (0, 'e9a471b3c6afc7901a50d4c6505fc137cc693d4f0cb41eb855562074d4e16b66'),
    'back-64-emit-c': (0, '8849c08ff8ba8e45a0c740d00f814cec37118ba358a187853920a0035ed9bf92'),
    'back-128-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'back-128-fmt': (0, 'eff355fb269dcdb3d85a1993eb95dd2d7aa6cee94d045f83fddb3aa573db209e'),
    'back-128-interfaces': (0, '8e8efb09ee38001a97544039c76902b299c9661908bbee05f572cdf2b3dc8f1d'),
    'back-128-emit-c': (0, '5421276a5f402dbf3046b295d38e34cadee700a232422e2306a0f9451d8d1335'),
    'back-256-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'back-256-fmt': (0, '668adb9ac36eddc29c5cfc698189527b9879a45169c05e00d647b5e521b88c79'),
    'back-256-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'back-256-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'edges-16-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'edges-16-fmt': (0, 'f102a7b46d3821c70db0fa3e97fb96dfb8da5893a13eab77e62c108ef1229662'),
    'edges-16-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'edges-16-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'edges-32-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'edges-32-fmt': (0, 'fd3614aaafaa5c978c9d0642fc22d4ce793eee4598aeefaab81f7de54d1a95c7'),
    'edges-32-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'edges-32-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'edges-64-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'edges-64-fmt': (0, '8f1ce95e8ac2eaba0f2f07fb37626ddf1f7ab628c9b14001012073671e99eff0'),
    'edges-64-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'edges-64-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'edges-128-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'edges-128-fmt': (0, 'f8f69281c8fde43fd0022e5ebc648d98f72dd7e9fd40870eb35c91e4222d61cc'),
    'edges-128-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'edges-128-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'edges-256-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'edges-256-fmt': (0, '604dc3a89cda5570fd48602755b62f3ad74d646c1581ad790c3a27260545e36d'),
    'edges-256-interfaces': (0, '761cad30400165fcd9ea9888630d427f1ca916cef979cd5d823001b133469799'),
    'edges-256-emit-c': (0, '99cab3e2c770c2300bb085d8b5addeae0a373c9cac347be3d4306ca46b947302'),
    'bytes-32-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'bytes-32-fmt': (0, '56074411d28207c7bd14111c1e7f9c352948bbea106f0df6979d11d65b0ed330'),
    'bytes-32-interfaces': (0, '562956c107b6c3a430e0b7834039878c92f4080352e073d148852727af1e5108'),
    'bytes-32-emit-c': (0, 'ec6b0d4183f85c2acd9f6e50a076164faadab8a4b035d95e4d9375991a7d6709'),
    'bytes-64-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'bytes-64-fmt': (0, 'ed96934d8550bc51eb5f222f7b08a4203b0f0949c0201a709c39f653b4597d28'),
    'bytes-64-interfaces': (0, '562956c107b6c3a430e0b7834039878c92f4080352e073d148852727af1e5108'),
    'bytes-64-emit-c': (0, 'ec6b0d4183f85c2acd9f6e50a076164faadab8a4b035d95e4d9375991a7d6709'),
    'bytes-128-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'bytes-128-fmt': (0, 'd21ace119f59796427ac80079cb750e994b76e9c3b60b847842c4ba2e8f37003'),
    'bytes-128-interfaces': (0, '562956c107b6c3a430e0b7834039878c92f4080352e073d148852727af1e5108'),
    'bytes-128-emit-c': (0, 'ec6b0d4183f85c2acd9f6e50a076164faadab8a4b035d95e4d9375991a7d6709'),
    'bytes-256-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'bytes-256-fmt': (0, '92691fdeb45b17e2f265fc30c1f5dc5ee25fa92c952a093f5e81b980ad49a57f'),
    'bytes-256-interfaces': (0, '562956c107b6c3a430e0b7834039878c92f4080352e073d148852727af1e5108'),
    'bytes-256-emit-c': (0, 'ec6b0d4183f85c2acd9f6e50a076164faadab8a4b035d95e4d9375991a7d6709'),
    'prefix-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'prefix-fmt': (0, 'bd0491ef5182c44e4f13d98a130e68d7c2d54c96a2056f881caf59ba9a0da33f'),
    'prefix-interfaces': (0, '4fc9d13470c9062580cfb8b3f8f16be1567b3da10921d2fad245f54fa3db0058'),
    'prefix-emit-c': (0, '786dd730620f3a69a10e4e230e18d920d0d907390338b03028a52ec226a683ca'),
    'nonascii-path-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'nonascii-path-fmt': (0, '481ebd6eadf58e3c801f8398242e516b22afe4f6fa0694ed720af34c1300f270'),
    'nonascii-path-interfaces': (0, 'e98afd85c191ef9644ff9af45c48f6cb59baa6278bf8c72e391cb74850f1c20a'),
    'nonascii-path-emit-c': (0, '5adbff78808819bcfc4698b4a3d1a5671bb264fd615a95f19f51e178ed1973c9'),
    'unknown-entry-check': (1, '604c550f178e706c4e954c03d4f7ba36b4c6f13f69470f4da823c00fd236aa99'),
    'unknown-entry-fmt': (1, '604c550f178e706c4e954c03d4f7ba36b4c6f13f69470f4da823c00fd236aa99'),
    'unknown-entry-interfaces': (1, '604c550f178e706c4e954c03d4f7ba36b4c6f13f69470f4da823c00fd236aa99'),
    'unknown-entry-emit-c': (1, '604c550f178e706c4e954c03d4f7ba36b4c6f13f69470f4da823c00fd236aa99'),
    'unknown-first-check': (1, '6fecb92638ced2cc4a96f802ac711bafdb6798ece535e63e803c24d308e87d36'),
    'unknown-first-fmt': (1, '6fecb92638ced2cc4a96f802ac711bafdb6798ece535e63e803c24d308e87d36'),
    'unknown-first-interfaces': (1, '6fecb92638ced2cc4a96f802ac711bafdb6798ece535e63e803c24d308e87d36'),
    'unknown-first-emit-c': (1, '6fecb92638ced2cc4a96f802ac711bafdb6798ece535e63e803c24d308e87d36'),
    'unknown-last-check': (1, 'c60d7ed2c28aa81f1df7a7d19132014f8926d975ccb9f82400c0965539dd800e'),
    'unknown-last-fmt': (1, 'c60d7ed2c28aa81f1df7a7d19132014f8926d975ccb9f82400c0965539dd800e'),
    'unknown-last-interfaces': (1, 'c60d7ed2c28aa81f1df7a7d19132014f8926d975ccb9f82400c0965539dd800e'),
    'unknown-last-emit-c': (1, 'c60d7ed2c28aa81f1df7a7d19132014f8926d975ccb9f82400c0965539dd800e'),
    'unsorted-duplicate-check': (1, 'd90d4864a63b1aaa5f45bd8c4b4399634c19d8199f8dd4b6a00c3c7839256ed1'),
    'unsorted-duplicate-fmt': (1, 'd90d4864a63b1aaa5f45bd8c4b4399634c19d8199f8dd4b6a00c3c7839256ed1'),
    'unsorted-duplicate-interfaces': (1, 'd90d4864a63b1aaa5f45bd8c4b4399634c19d8199f8dd4b6a00c3c7839256ed1'),
    'unsorted-duplicate-emit-c': (1, 'd90d4864a63b1aaa5f45bd8c4b4399634c19d8199f8dd4b6a00c3c7839256ed1'),
    'unsorted-later-duplicate-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'unsorted-later-duplicate-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'unsorted-later-duplicate-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'unsorted-later-duplicate-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'duplicate-before-invalid-check': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'duplicate-before-invalid-fmt': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'duplicate-before-invalid-interfaces': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'duplicate-before-invalid-emit-c': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'duplicate-after-invalid-check': (1, 'a9dcac0b9db6978cae5568921a4fbd367ce10bda050bd2b92f325a26e17c0ee4'),
    'duplicate-after-invalid-fmt': (1, 'a9dcac0b9db6978cae5568921a4fbd367ce10bda050bd2b92f325a26e17c0ee4'),
    'duplicate-after-invalid-interfaces': (1, 'a9dcac0b9db6978cae5568921a4fbd367ce10bda050bd2b92f325a26e17c0ee4'),
    'duplicate-after-invalid-emit-c': (1, 'a9dcac0b9db6978cae5568921a4fbd367ce10bda050bd2b92f325a26e17c0ee4'),
    'duplicate-before-list-check': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'duplicate-before-list-fmt': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'duplicate-before-list-interfaces': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'duplicate-before-list-emit-c': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'self-import-check': (1, '9cc5ecddfeab6e4dd4882a496f046a01926e19830f7e1d738f45b2ad2986d659'),
    'self-import-fmt': (1, '9cc5ecddfeab6e4dd4882a496f046a01926e19830f7e1d738f45b2ad2986d659'),
    'self-import-interfaces': (1, '9cc5ecddfeab6e4dd4882a496f046a01926e19830f7e1d738f45b2ad2986d659'),
    'self-import-emit-c': (1, '9cc5ecddfeab6e4dd4882a496f046a01926e19830f7e1d738f45b2ad2986d659'),
    'reciprocal-check': (1, '94b695a70409dfb60d0c9dc99195341e0ffc8c270c9bea9350f15af69e3a0014'),
    'reciprocal-fmt': (1, '94b695a70409dfb60d0c9dc99195341e0ffc8c270c9bea9350f15af69e3a0014'),
    'reciprocal-interfaces': (1, '94b695a70409dfb60d0c9dc99195341e0ffc8c270c9bea9350f15af69e3a0014'),
    'reciprocal-emit-c': (1, '94b695a70409dfb60d0c9dc99195341e0ffc8c270c9bea9350f15af69e3a0014'),
    'malformed-unsorted-head-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-head-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-head-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-head-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-name-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-name-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-name-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-name-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-path-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-path-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-path-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-path-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-imports-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-imports-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-imports-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-imports-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-exports-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-exports-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-exports-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-exports-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-atom-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-atom-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-atom-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-atom-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-extra-close-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-extra-close-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-extra-close-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-extra-close-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-escape-check': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-escape-fmt': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-escape-interfaces': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-unsorted-escape-emit-c': (1, 'b56c78b2432e16c080bce44c42bff2515709a86d8ccbf9ab3757a23f65316653'),
    'malformed-sorted-distinct-head-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-head-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-head-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-head-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-name-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-name-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-name-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-name-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-path-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-path-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-path-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-path-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-imports-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-imports-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-imports-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-imports-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-exports-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-exports-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-exports-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-exports-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-atom-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-atom-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-atom-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-atom-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-extra-close-check': (0, 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'),
    'malformed-sorted-distinct-extra-close-fmt': (0, '411e3a86c5401ac87348ecc1b69e17f3798bae6402def47c670587368e43adf2'),
    'malformed-sorted-distinct-extra-close-interfaces': (0, 'e98afd85c191ef9644ff9af45c48f6cb59baa6278bf8c72e391cb74850f1c20a'),
    'malformed-sorted-distinct-extra-close-emit-c': (0, '5adbff78808819bcfc4698b4a3d1a5671bb264fd615a95f19f51e178ed1973c9'),
    'malformed-sorted-distinct-escape-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-escape-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-escape-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-distinct-escape-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-head-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-head-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-head-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-head-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-name-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-name-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-name-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-name-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-path-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-path-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-path-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-path-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-imports-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-imports-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-imports-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-imports-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-exports-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-exports-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-exports-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-exports-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-atom-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-atom-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-atom-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-atom-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-extra-close-check': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'malformed-sorted-duplicate-extra-close-fmt': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'malformed-sorted-duplicate-extra-close-interfaces': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'malformed-sorted-duplicate-extra-close-emit-c': (1, '4a9af5db36f589aa8c008d84fb7d69f969e56020517f236908f60ab8e30c3fc6'),
    'malformed-sorted-duplicate-escape-check': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-escape-fmt': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-escape-interfaces': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-sorted-duplicate-escape-emit-c': (1, 'ce566bd0ce0b64d40c9af09844cdecffd9c07422e737b6526a75072892e9d8a7'),
    'malformed-short-escape-check': (1, '80029e5cd3fa204d32e322ef735128002fd2ce7d6bf4a97615a51fce84192cbf'),
    'malformed-short-escape-fmt': (1, '80029e5cd3fa204d32e322ef735128002fd2ce7d6bf4a97615a51fce84192cbf'),
    'malformed-short-escape-interfaces': (1, '80029e5cd3fa204d32e322ef735128002fd2ce7d6bf4a97615a51fce84192cbf'),
    'malformed-short-escape-emit-c': (1, '80029e5cd3fa204d32e322ef735128002fd2ce7d6bf4a97615a51fce84192cbf'),
}


INDEX_PREFIX = b"SLIM_MANIFEST_INDEX_WORK "
RESOURCE_ORDINALS = tuple(1 << ordinal for ordinal in range(14))


def checked_total(values):
    total = 0
    for value in values:
        require(type(value) is int and 0 <= value <= COUNTER_CAP, "counter operand admission")
        require(total <= COUNTER_CAP - value, "counter sum admission")
        total += value
    return total


def checked_product(left, right):
    require(type(left) is int and type(right) is int and 0 <= left <= COUNTER_CAP and 0 <= right <= COUNTER_CAP,
            "counter product operands")
    require(left == 0 or right <= COUNTER_CAP // left, "counter product admission")
    return left * right



def arithmetic_controls():
    controls = []
    for value in (COUNTER_CAP - 1, COUNTER_CAP, COUNTER_CAP + 1, 2**64 - 1, -1, True):
        try:
            actual = checked_total((value, 1))
            admitted = True
        except ValueError:
            actual, admitted = None, False
        expected = type(value) is int and 0 <= value < COUNTER_CAP
        require(admitted == expected and (not admitted or actual == value + 1), "checked sum boundary")
        controls.append({"operation": "sum-plus-one", "value": value, "admitted": admitted, "result": actual})
    for left, right, expected in ((COUNTER_CAP, 1, True), (COUNTER_CAP, 2, False),
                                  (COUNTER_CAP // 800, 800, True), (COUNTER_CAP // 800 + 1, 800, False),
                                  (0, 2**64 - 1, False), (True, 1, False)):
        try:
            actual = checked_product(left, right)
            admitted = True
        except ValueError:
            actual, admitted = None, False
        require(admitted == expected and (not admitted or actual == left * right), "checked product boundary")
        controls.append({"operation": "product", "left": left, "right": right, "admitted": admitted, "result": actual})
    return controls


def instrument_current(source, oracle, path_oracle):
    """Observe only the private manifest trie; all unrelated syntax tries stay idle."""
    source, legacy_anchors = instrument_old(source, oracle.LEGACY)
    counters = path_oracle.COUNTERS + path_oracle.ELIGIBILITY
    index = {name: position for position, name in enumerate(counters)}
    project = lambda name: namespace_function("project", name)
    syntax = lambda name: namespace_function("syntax", name)
    loops = {
        project("index_manifest_paths"): "builder_module_headers",
        project("manifest_preflight_modules"): "preflight_headers",
        project("manifest_preflight_imports"): "preflight_headers",
        syntax("ensure_name_chars"): "insertion_character_headers",
        syntax("find_name_edge"): "edge_headers",
    }
    scoped = {name for name in loops if name.startswith("slim_fn_psyntax_")}
    entries = {syntax("append_name_node"): "nodes_appended",
               syntax("insert_name_chars"): "insert_calls"}
    token_reads = {project("manifest_preflight_modules"): 11,
                   project("manifest_preflight_imports"): 1,
                   project("manifest_index_eligible"): 4}
    spans = {project("manifest_preflight_modules"): ("preflight_span_checks", 2),
             project("manifest_preflight_imports"): ("preflight_span_checks", 1)}
    scopes = {(project("indexed_duplicate_path"), syntax("append_name_node")): 1,
              (project("indexed_duplicate_path"), project("index_manifest_paths")): 1}
    terminals = {syntax("insert_name_chars")}
    observed = set(loops) | set(entries) | set(token_reads) | set(spans) | terminals | {syntax("name_child")} | {parent for parent, _ in scopes}
    definitions = dict.fromkeys(observed, 0)
    loop_hits = dict.fromkeys(loops, 0)
    token_hits = dict.fromkeys(token_reads, 0)
    span_hits = dict.fromkeys(spans, 0)
    scope_hits = dict.fromkeys(scopes, 0)
    terminal_hits = dict.fromkeys(terminals, 0)
    edge_hits, main_hits = 0, 0

    def increment(name, guarded=False):
        call = f"slim_manifest_validation_add(&slim_manifest_validation_index_counts[{index[name]}]);\n"
        return "if (slim_manifest_validation_index_scope) " + call if guarded else call

    prefix = "#include <stdint.h>\n#include <stdio.h>\n#include <stdlib.h>\n"
    prefix += f"static uint64_t slim_manifest_validation_index_counts[{len(counters)}];\n"
    prefix += "static int slim_manifest_validation_index_scope;\n"
    # The original counter admission helper precedes all uses; add its prototype.
    prefix += "static void slim_manifest_validation_add(uint64_t *value);\n"
    prefix += 'static void slim_manifest_validation_index_report(void) {\nfputs("SLIM_MANIFEST_INDEX_WORK {", stderr);\n'
    for position, name in enumerate(counters):
        separator = "" if not position else ","
        prefix += f'fprintf(stderr, "{separator}\\\"{name}\\\":%llu", (unsigned long long)slim_manifest_validation_index_counts[{position}]);\n'
    prefix += 'fputs("}\\n", stderr);\n}\n'
    output, active = [prefix], None
    for line in source.splitlines(keepends=True):
        definition = re.match(r"^static SLIM_UNUSED_FUNCTION \S+ (\w+)\(.* \{\n$", line)
        if definition:
            active = definition.group(1)
            if active in definitions:
                definitions[active] += 1
        selected = next((key for key in scopes if key[0] == active and "= " + key[1] + "(" in line), None)
        if selected:
            scope_hits[selected] += 1
            output.append("if (slim_manifest_validation_index_scope) abort();\nslim_manifest_validation_index_scope = 1;\n")
        if active in token_reads and " = ((Slim_type_psyntax_95_95Token *)" in line and "slim_vec_check_index(" in line:
            token_hits[active] += 1
            output.append(increment("preflight_token_reads"))
        if active in spans and "= " + project("manifest_span_admitted") + "(" in line:
            span_hits[active] += 1
            output.append(increment(spans[active][0]))
        if active in terminals and " = ((Slim_type_psyntax_95_95NameNode *)" in line:
            terminal_hits[active] += 1
            output.append(increment("terminal_value_reads", True))
        if active == syntax("name_child") and "if (!slim_vec_push(slim_v_edges_" in line:
            edge_hits += 1
            output.append(increment("edges_appended", True))
        output.append(line)
        if selected:
            output.append("slim_manifest_validation_index_scope = 0;\n")
        if definition and active in entries:
            output.append(increment(entries[active], True))
        if active in loops and line == "slim_recur: ;\n":
            loop_hits[active] += 1
            output.append(increment(loops[active], active in scoped))
        if line == "int main(int argc, char **argv) {\n":
            main_hits += 1
            output.append("if (atexit(slim_manifest_validation_index_report) != 0) abort();\n")
    require(all(value == 1 for value in definitions.values()), "current C definition anchors: " + str(definitions))
    require(all(value == 1 for value in loop_hits.values()), "current C loop anchors: " + str(loop_hits))
    require(token_hits == token_reads, "current preflight read anchors")
    require(span_hits == {key: value[1] for key, value in spans.items()}, "current span anchors")
    require(scope_hits == scopes and all(value == 1 for value in terminal_hits.values()) and edge_hits == main_hits == 1,
            "current trie scope/terminal/edge/main anchors")
    return "".join(output), {"legacy": legacy_anchors, "definitions": definitions, "loops": loop_hits,
                             "token_reads": token_hits, "span_calls": span_hits,
                             "scopes": {str(key): value for key, value in scope_hits.items()},
                             "terminal_reads": terminal_hits, "edge_pushes": edge_hits, "main": main_hits}


def current_legacy_expected(row):
    return row["path_legacy"]


def parse_current_work(stderr, row, path_oracle):
    lines = stderr.splitlines(keepends=True)
    require(len(lines) == 2, "current observer output must contain exactly two records")
    legacy = [line for line in lines if line.startswith(COUNTER_PREFIX)]
    indexed = [line for line in lines if line.startswith(INDEX_PREFIX)]
    require(len(legacy) == len(indexed) == 1, "current observer record domains")
    old = parse_work(legacy[0], current_legacy_expected(row))
    value = json.loads(indexed[0][len(INDEX_PREFIX):])
    names = path_oracle.COUNTERS + path_oracle.ELIGIBILITY
    require(isinstance(value, dict) and set(value) == set(names), "current path observer fields")
    checked_total(value.values())
    expected = row["path_only"]
    if expected is None:
        require(value == dict.fromkeys(names, 0), "unreached path index produced work")
        return {"legacy": old, "path_only": value, "W": 0, "local_work": 0}
    require(value == expected["work"] | expected["eligibility"], "independent path/eligibility geometry mismatch")
    w = checked_total(value[name] for name in path_oracle.COUNTERS)
    local = checked_total(value.values())
    extent = row["extents"]
    n, e, bp = extent["N"], extent["E"], extent["Bp"]
    work_bound = checked_total((checked_product(260, bp), checked_product(4, n), 2))
    local_bound = checked_total((checked_product(260, bp), checked_product(20, n), checked_product(3, e), 8))
    envelope = checked_product(800, checked_total((extent["B"], n, e, 1)))
    require(w == expected["W"] and local == expected["local_work"] and w <= work_bound and local <= local_bound <= envelope,
            "current checked path work envelope")
    return {"legacy": old, "path_only": value, "W": w, "local_work": local, "work_bound": work_bound,
            "local_bound": local_bound, "ceiling": envelope}



def geometric_exponents(rows, observed_rows):
    measured = {row["id"]: row["work"]["local_work"] for row in observed_rows}
    families = {"paths": 5, "duplicate": 5, "front": 5, "back": 5, "edges": 5, "bytes": 4}
    result = {}
    for family, count in families.items():
        points = []
        for row in rows:
            if row["family"] == family:
                require(row["path_only"] is not None, "geometric family must be path eligible")
                extent = row["extents"]
                scale = checked_total((extent["B"], extent["N"], extent["E"]))
                work = measured[row["id"]]
                require(0 < scale <= COUNTER_CAP and 0 < work <= COUNTER_CAP, "geometric operands")
                points.append({"id": row["id"], "B_plus_N_plus_E": scale, "path_local_work": work})
        require(len(points) == count and all(left["B_plus_N_plus_E"] < right["B_plus_N_plus_E"]
                for left, right in zip(points, points[1:])), "geometric fixed count/order")
        first, last = points[0], points[-1]
        # Match the established project-list/namespace endpoint convention.
        exponent = math.log(last["path_local_work"] / first["path_local_work"]) / math.log(last["B_plus_N_plus_E"] / first["B_plus_N_plus_E"])
        require(math.isfinite(exponent) and exponent <= 1.15, "path-local geometric exponent crossed: " + family)
        result[family] = {"points": points, "endpoint_exponent": exponent, "budget": 1.15,
                          "scope": "observed path index plus eligibility work only; legacy name/target-degree scans excluded"}
    return result


def portable_fixture_pins(fixtures, rows):
    validate_fixture_files(fixtures, rows)
    expected = {"oracle.json": SEALED_DATA_SHA}
    for row in rows:
        expected[row["id"] + "/slim.project"] = row["manifest_sha256"]
        for name, value in row["files"].items():
            expected[row["id"] + "/" + name] = value["sha256"]
    actual = {str(path.relative_to(fixtures)): digest(path) for path in fixtures.rglob("*") if path.is_file()}
    require(len(actual) == 3456 and actual == expected, "portable fixture domain/drift")
    return actual


def portable_golden(result, stdout, stderr):
    expected = PORTABLE_GOLDENS[result["id"]]
    require(result["process_status"] == "complete" and result["returncode"] == expected[0] and
            hashlib.sha256(stdout).hexdigest() == expected[1] and hashlib.sha256(stderr).hexdigest() == PORTABLE_GOLDEN_STDERR_SHA256, "portable held output mismatch: " + result["id"])


def current_source_pins(arguments):
    paths = sorted((ROOT / "selfhost").glob("*.slim")) + [ROOT / "selfhost/slim.project", ROOT / "runtime/slim_rt.c",
             ROOT / "runtime/slim_rt.h", ORACLE_PATH, PATH_ORACLE_PATH, ROOT / "design/rfcs/0167-linear-manifest-validation.md",
             ROOT / "benchmarks/instrumentation/host_resource.h", ROOT / "benchmarks/instrumentation/host_resource.c",
             ROOT / "tests/fixtures/retained_project.slim", Path(__file__), arguments.compiler.resolve(), arguments.generated_c.resolve(), Path(arguments.cc).resolve()]
    paths.append(Path(sys.executable).resolve())
    if arguments.compare:
        paths.extend(sorted((arguments.baseline_source_root / "selfhost").glob("*.slim")))
        paths.extend([arguments.baseline_source_root / "selfhost/slim.project", arguments.baseline_source_root / "runtime/slim_rt.c",
                      arguments.baseline_source_root / "runtime/slim_rt.h", arguments.baseline_compiler, arguments.baseline_generated_c])
    require(len(paths) <= 128 and all(path.is_file() for path in paths), "current source/tool domain")
    return {str(path.resolve()): {"bytes": path.stat().st_size, "sha256": digest(path),
            "executable": bool(path.stat().st_mode & 0o111)} for path in paths}


def current_environment():
    environment = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1", UBSAN_OPTIONS="halt_on_error=1")
    for key in ("SLIM_ALLOC_FAIL_AT", "SLIM_HOST_ALLOC_FAIL_AT", "SLIM_NATIVE_ALLOC_FAIL_AT", "SLIM_TASK_FAIL_AT",
                "SLIM_TASK_JOIN_FAIL_AT", "SLIM_TASK_DISABLE"):
        environment.pop(key, None)
    return environment


def current(arguments, oracle):
    output = arguments.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    started = time.monotonic()
    deadline = started + 1800
    pins = current_source_pins(arguments)
    receipt = {"schema": 1, "mode": "compare" if arguments.compare else "current", "status": "running",
               "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(), "native_deadline_seconds": 1800,
               "malformed_deadline_seconds": 900, "per_child_seconds": 60, "source_before": pins,
               "host": list(os.uname()), "work_rows": [], "golden_rows": [], "resource_rows": [], "samples": [], "unfinished": []}
    path = output / "receipt.json"
    write_json(path, receipt)
    environment = current_environment()
    artifact_pins = {}
    try:
        receipt["native_cleanup_control"] = native_cleanup_control(output, deadline, environment)
        write_json(path, receipt)
        receipt["arithmetic_controls"] = arithmetic_controls()
        fixtures = output / "fixtures"
        payload = oracle.emit(fixtures)
        require(digest(fixtures / "oracle.json") == SEALED_DATA_SHA, "portable regenerated oracle data differs from held bytes")
        rows = payload["cases"]
        path_oracle, path_model = load_path_model()
        expectations = {value["id"]: value for value in path_model["rows"]}
        require(set(expectations) == {value["id"] for value in rows}, "path/fixture scope mismatch")
        for row in rows:
            expected = expectations[row["id"]]
            require(expected["manifest_sha256"] == row["manifest_sha256"] and expected["extents"] == row["extents"], "path/fixture identity mismatch")
            row.update(path_only=expected["path_only"], path_legacy=expected["legacy"])
        receipt["path_oracle"] = {"source_sha256": PATH_ORACLE_SHA, "model_sha256": PATH_MODEL_SHA}
        require(payload["counts"] == {"geometric": 29, "control": 12, "malformed": 25} and len(rows) == 66,
                "portable finite fixture scope")
        fixture_pins = portable_fixture_pins(fixtures, rows)
        receipt["fixture_pins"] = fixture_pins
        compiler, generated, cc = arguments.compiler.resolve(), arguments.generated_c.resolve(), Path(arguments.cc).resolve()
        require(bool(compiler.stat().st_mode & 0o111), "current compiler executable admission")
        emitted, stdout, stderr = native([compiler, ROOT / "selfhost/slim.project"], output, "self-emission", deadline, environment=environment)
        require(emitted["process_status"] == "complete" and emitted["returncode"] == 0 and not stderr and
                hashlib.sha256(stdout).hexdigest() == digest(generated), "current compiler/generated-C self-emission identity")
        receipt["self_emission"] = emitted
        owner_body, owner_proof = private_owner_cleanup(generated.read_text())
        (output / "private-owner.c.txt").write_text(owner_body)
        receipt["private_owner_cleanup"] = owner_proof
        observed_c = output / "observed-current.c"
        observed_source, anchors = instrument_current(generated.read_text(), oracle, path_oracle)
        observed_c.write_text(observed_source)
        observed = output / "observed-current"
        runtime = ROOT / "runtime"
        result, stdout, stderr = native([cc, "-std=c11", "-O2", "-DNDEBUG", "-Wall", "-Wextra", "-Werror", "-I", runtime,
                                        observed_c, runtime / "slim_rt.c", "-o", observed], output, "observer-build", deadline, environment=environment)
        require(result["process_status"] == "complete" and result["returncode"] == 0 and not stdout and not stderr, "current observer build")
        sanitized = output / "current-sanitized"
        result, stdout, stderr = native([cc, "-std=c11", "-O0", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                                        "-Wall", "-Wextra", "-Werror", "-I", runtime, generated, runtime / "slim_rt.c", "-o", sanitized],
                                       output, "sanitized-build", deadline, environment=environment)
        require(result["process_status"] == "complete" and result["returncode"] == 0 and not stdout and not stderr, "current sanitized build")
        receipt["observer"] = {"source_sha256": digest(observed_c), "binary_sha256": digest(observed), "anchors": anchors}
        receipt["sanitized"] = {"binary_sha256": digest(sanitized)}
        artifact_pins = {str(value): digest(value) for value in (observed_c, observed, sanitized)}
        receipt["artifact_before"] = artifact_pins
        receipt["counter_boundaries"] = counter_boundaries(cc, output, deadline)
        work_dir = output / "work"
        work_dir.mkdir()
        for row in [value for value in rows if value["family"] != "malformed"]:
            fixture = fixtures / row["id"] / "slim.project"
            plain, stdout, stderr = native([compiler, "check", fixture], work_dir, row["id"] + "-plain", deadline, environment=environment)
            portable_golden(plain | {"id": row["id"] + "-check"}, stdout, stderr)
            observed_result, observed_stdout, observed_stderr = native([observed, "check", fixture], work_dir, row["id"] + "-observed", deadline, environment=environment)
            require(observed_result["process_status"] == "complete" and observed_result["returncode"] == plain["returncode"] and observed_stdout == stdout,
                    "current observer changed production result: " + row["id"])
            work = parse_current_work(observed_stderr, row, path_oracle)
            receipt["work_rows"].append({"id": row["id"], "work": work, "ordinary": plain, "observed": observed_result})
            write_json(path, receipt)
        receipt["geometric_work"] = geometric_exponents(rows, receipt["work_rows"])
        golden_dir = output / "goldens"
        golden_dir.mkdir()
        malformed_deadline = None
        for row in rows:
            if row["family"] == "malformed" and malformed_deadline is None:
                malformed_deadline = min(deadline, time.monotonic() + 900)
            for command in ("check", "fmt", "interfaces", None):
                label = "emit-c" if command is None else command
                argv_tail = ([] if command is None else [command]) + [fixtures / row["id"] / "slim.project"]
                for mode, binary in (("ordinary", compiler), ("sanitized", sanitized)):
                    limit, timeout = (65536, 5) if row["family"] == "malformed" else (STDOUT_CAP, 60)
                    result, stdout, stderr = native([binary, *argv_tail], golden_dir, row["id"] + "-" + label + "-" + mode,
                                                    malformed_deadline if row["family"] == "malformed" else deadline,
                                                    timeout=timeout, stdout_cap=limit, stderr_cap=65536 if row["family"] == "malformed" else STDERR_CAP,
                                                    environment=environment)
                    if result["process_status"] != "complete":
                        receipt["unfinished"].append({"id": row["id"], "command": label, "mode": mode, "reason": "bounded process incomplete"})
                    portable_golden(result | {"id": row["id"] + "-" + label}, stdout, stderr)
                    receipt["golden_rows"].append(result)
                    write_json(path, receipt)
        resource_dir = output / "resources"
        resource_dir.mkdir()
        for identity in ("prefix", "unsorted-duplicate"):
            row = next(value for value in rows if value["id"] == identity)
            normal = bytes.fromhex(row["diagnostics_hex"])
            for mode, binary in (("ordinary", compiler), ("sanitized", sanitized)):
                for ordinal in RESOURCE_ORDINALS:
                    result, stdout, stderr = native([binary, "check", fixtures / identity / "slim.project"], resource_dir,
                                                    f"{identity}-{mode}-fault-{ordinal}", deadline,
                                                    environment=dict(environment, SLIM_ALLOC_FAIL_AT=str(ordinal)))
                    require(result["process_status"] == "complete", "resource observation incomplete")
                    if result["returncode"] == 71:
                        require(normal.startswith(stdout) and stderr == f"SLIM allocation failure: exhausted at allocation {ordinal}\n".encode(),
                                "resource failure status/publication")
                    else:
                        portable_golden(result | {"id": identity + "-check"}, stdout, stderr)
                    receipt["resource_rows"].append(result | {"ordinal": ordinal, "mode": mode, "fixture": identity})
                    write_json(path, receipt)
        if arguments.compare:
            baseline_reference_controls(arguments, compiler, output, deadline, environment, receipt, path)
            resource_observations(arguments, compiler, generated, cc, fixtures, output, deadline, environment, receipt, path)
        receipt["retained"] = retained_paired_campaign(ROOT, compiler,
            arguments.baseline_source_root if arguments.compare else None,
            arguments.baseline_compiler if arguments.compare else None,
            arguments.baseline_source_sha256 if arguments.compare else None,
            cc, fixtures, output, deadline, environment)
        write_json(path, receipt)
        if arguments.compare:
            compare_current(arguments, rows, fixtures, compiler, output, deadline, environment, receipt, path)
        require(portable_fixture_pins(fixtures, rows) == fixture_pins, "portable fixtures changed during native campaign")
        require(current_source_pins(arguments) == pins, "current source/tool identities changed during native campaign")
        require({name: digest(name) for name in artifact_pins} == artifact_pins, "current local observer/artifact drift")
        receipt["artifact_after"] = {name: digest(name) for name in artifact_pins}
        receipt["source_after"] = current_source_pins(arguments)
        receipt["status"] = "pass"
        receipt["unknown"] = ["loaded-code/ABA/host-boot custody", "cold filesystem cache", "isolated frontend CPU",
                              "external backend comparisons", "generated application runtime", "peak RSS", "RSS/libc allocation cost and copying outside runtime realloc",
                              "allocation ordinals outside fixed 1,2,4,...8192 probes", "isolated retained-update CPU"]
        if not arguments.compare:
            receipt["unknown"].extend(["same-host candidate/baseline ratios", "old/current allocation observations", "old/current retained source-paired output parity"])
    except BaseException as error:
        receipt["status"] = "inconclusive" if isinstance(error, HostNoiseInconclusive) else "failed"
        receipt["failure"] = {"type": type(error).__name__, "message": str(error)}
        raise
    finally:
        try:
            receipt["source_after"] = current_source_pins(arguments)
            receipt["artifact_after"] = {name: digest(name) for name in artifact_pins}
        except (OSError, ValueError) as error:
            receipt["identity_after_unknown"] = {"type": type(error).__name__, "message": str(error)}
            if receipt["status"] == "pass":
                receipt["status"] = "failed"
        receipt["elapsed_ns"] = int((time.monotonic() - started) * 1_000_000_000)
        receipt["finished_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        write_json(path, receipt)
    require(receipt["status"] == "pass", "current campaign ended without complete identity observations")
    print(json.dumps({"status": receipt["status"], "receipt": str(path), "sha256": digest(path)}, sort_keys=True))


RESOURCE_FIELDS = ("runtime_attempts", "requested_payload", "live_payload", "live_with_headers", "peak_payload", "peak_with_headers",
                   "runtime_realloc_copies", "host_attempts", "host_requested", "host_live", "host_peak")
RESOURCE_REPORT_CAP = 131072
RESOURCE_RUNTIME_ANCHORS = (
    ('    uint64_t attempt = atomic_fetch_add(&status->attempts, 1) + 1;\n',
     '    uint64_t attempt = atomic_fetch_add(&status->attempts, 1) + 1;\n    host_resource_attempt(size);\n', 1),
    ('    allocation->next = region->newest;\n',
     '    host_resource_add(size, offsetof(SlimAllocation, data));\n    allocation->next = region->newest;\n', 1),
    ('free(allocation);', 'host_resource_free(allocation->size, offsetof(SlimAllocation, data)); free(allocation);', 2),
    ('        memcpy(new_pointer, pointer, copied);', '        host_resource_copy(copied);\n        memcpy(new_pointer, pointer, copied);', 1),
)


def private_owner_cleanup(source):
    name = namespace_function("project", "indexed_duplicate_path")
    definition = re.search(r"^static SLIM_UNUSED_FUNCTION int64_t " + name + r"\(.* \{\n", source, re.M)
    require(definition is not None, "private scalar owner C definition")
    end = re.search(r"^(?:static SLIM_UNUSED_FUNCTION|int main)", source[definition.end():], re.M)
    require(end is not None, "private owner C boundary")
    body = source[definition.start():definition.end() + end.start()]
    require(body.count("slim_region_init(&slim_function_region, slim_region);") == 1 and
            body.count("slim_region_destroy(&slim_function_region);") == 1 and body.count("return slim_result;") == 1,
            "private owner region/return geometry")
    require("slim_region_adopt" not in body and body.index("slim_region_destroy(&slim_function_region);") < body.index("return slim_result;") and
            body.count(namespace_function("project", "index_manifest_paths") + "(") == 1,
            "private owner cleanup must precede both scalar return results")
    return body, {"function": name, "body_sha256": hashlib.sha256(body.encode()).hexdigest(),
                  "single_private_region_destroy_before_single_return": True, "no_region_adopt": True,
                  "both_success_and_duplicate_follow_same_cleanup": True}


def resource_source(generated, runtime):
    counts = []
    for anchor, replacement, count in RESOURCE_RUNTIME_ANCHORS:
        require(runtime.count(anchor) == count, "resource runtime anchor drift: " + anchor)
        runtime = runtime.replace(anchor, replacement)
        counts.append(count)
    anchor = "int main(int argc, char **argv) {\n"
    require(generated.count(anchor) == 1, "resource main anchor")
    generated = generated.replace(anchor, anchor + "host_resource_init();\n")
    anchor = "slim_rt_shutdown();\nreturn (int)slim_exit_code;"
    require(generated.count(anchor) == 1, "resource successful shutdown anchor")
    generated = generated.replace(anchor, "host_resource_snapshot(0, 1, 0);\nslim_rt_shutdown();\nhost_resource_snapshot(2, 1, 0);\nreturn (int)slim_exit_code;")
    return generated, runtime, {"runtime_anchor_counts": counts, "main": 1, "success_shutdown": 1}


def parse_resource_report(path, expected_count=2, expected_status="exact", controls=False):
    require(path.is_file() and path.stat().st_size <= RESOURCE_REPORT_CAP, "resource report physical admission")
    data = path.read_bytes()
    require(len(data) <= RESOURCE_REPORT_CAP and data.endswith(b"\n"), "resource report logical admission")
    lines = data.decode("ascii").splitlines()
    require(len(lines) <= 258 and lines[0] == f"slim-host-resources\t1\t{expected_status}\t256", "resource report scope/cap header")
    require(lines[1].split("\t") == ["event", "epoch", "serial", *RESOURCE_FIELDS], "resource field order")
    require(len(lines) == expected_count + 2, "resource exact expected snapshot count")
    rows = []
    for index, line in enumerate(lines[2:]):
        fields = line.split("\t")
        require(len(fields) == 14 and all(re.fullmatch(r"0|[1-9][0-9]*", value) is not None and len(value) <= 20 for value in fields),
                "resource scalar admission")
        values = list(map(int, fields))
        require(values[0] <= 2 and all(value <= 9223372036854775807 for value in values[1:3]) and
                all(value <= 18446744073709551615 for value in values[3:]), "resource identity/unsigned range")
        if not controls:
            require(values[:3] == [0 if index == 0 else 2, 1, 0], "resource fixed event order")
        else:
            require(values[:3] == [0, 1, index] and not any(values[3:]), "resource boundary snapshot identity/data")
        row = dict(zip(("event", "epoch", "serial", *RESOURCE_FIELDS), values))
        require(row["live_with_headers"] >= row["live_payload"], "resource live header arithmetic admission")
        row["live_header_bytes"] = row["live_with_headers"] - row["live_payload"]
        rows.append(row)
    if not controls:
        require(not any(rows[-1][key] for key in ("live_payload", "live_with_headers", "host_live")), "resource final owner cleanup")
        require(all(rows[0][key] == rows[1][key] for key in ("runtime_attempts", "requested_payload", "peak_payload", "peak_with_headers", "runtime_realloc_copies",
                                                         "host_attempts", "host_requested", "host_peak")), "resource snapshot cumulative stability")
    return {"status": expected_status, "report_sha256": digest(path), "rows": rows,
            "unknown": ["RSS", "libc bookkeeping/allocation costs", "cumulative requested header bytes", "peak header-only storage",
                        "copying outside runtime realloc", "loaded-code/ABA/host-boot custody"]}


def resource_observations(arguments, compiler, generated, cc, fixtures, output, deadline, environment, receipt, receipt_path):
    directory = output / "allocation-observations"
    directory.mkdir()
    require(arguments.compare, "old/current resource observations require explicit source-pinned comparison")
    sources = [("baseline", arguments.baseline_source_root.resolve(), arguments.baseline_compiler.resolve(), arguments.baseline_generated_c.resolve()),
               ("candidate", ROOT, compiler, generated)]
    builds, observed = {}, []
    library = ROOT / "benchmarks/instrumentation"
    for label, source_root, ordinary, c_path in sources:
        source_pins = retained_source_pins(source_root)
        if label == "baseline":
            require(retained_source_sha(source_pins) == arguments.baseline_source_sha256, "resource baseline source-set pin")
        generated_text, runtime_text, anchors = resource_source(c_path.read_text(), (source_root / "runtime/slim_rt.c").read_text())
        observed_c = directory / (label + ".c"); observed_c.write_text(generated_text)
        runtime_c = directory / (label + "-runtime.c"); runtime_c.write_text(runtime_text)
        selfhost = arguments.baseline_source_root.resolve() / "selfhost/slim.project"
        for mode in ("ordinary", "sanitized"):
            binary = directory / (label + "-" + mode)
            flags = ["-O2", "-DNDEBUG"] if mode == "ordinary" else ["-O0", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
            built, stdout, stderr = native([cc, "-std=c11", *flags, "-Wall", "-Wextra", "-Werror", "-I", source_root / "runtime",
                                           "-include", library / "host_resource.h", observed_c, runtime_c, library / "host_resource.c", "-o", binary],
                                          directory, label + "-" + mode + "-build", deadline, environment=environment)
            require(built["process_status"] == "complete" and built["returncode"] == 0 and not stdout and not stderr, "resource observer native build")
            builds[label + "-" + mode] = {"build": built, "anchors": anchors, "source_sha256": digest(observed_c), "runtime_sha256": digest(runtime_c),
                                         "binary_sha256": digest(binary), "source_pins": source_pins, "baseline_c_sha256": digest(c_path)}
            receipt["allocation_observer_builds"] = builds; write_json(receipt_path, receipt)
            for identity in ("prefix", "paths-256", "held-selfhost"):
                fixture = selfhost if identity == "held-selfhost" else fixtures / identity / "slim.project"
                report = directory / f"{label}-{mode}-{identity}.tsv"
                result, stdout, stderr = native([binary, "check", fixture], directory, f"{label}-{mode}-{identity}", deadline,
                                                environment=dict(environment, SLIM_HOST_RESOURCE_REPORT=str(report)))
                require(result["process_status"] == "complete" and result["returncode"] == 0 and not stdout and not stderr, "resource production check status/output")
                row = {"id": identity, "version": label, "mode": mode, "native": result, "resources": parse_resource_report(report)}
                observed.append(row); receipt["allocation_observations"] = observed; write_json(receipt_path, receipt)
            require(retained_source_pins(source_root) == source_pins and digest(binary) == builds[label + "-" + mode]["binary_sha256"], "resource source/artifact drift")
    require(len(observed) == 12, "resource fixed three-input artifact/variant scope")
    receipt["allocation_observation_scope"] = "three fixed inputs; old/current ordinary/sanitized"
    control = directory / "resource-boundary.c"
    control.write_text('#include "host_resource.h"\n#include <stdlib.h>\nint main(int argc, char **argv) {\nif (argc != 2) return 64;\nhost_resource_init();\nunsigned count = (unsigned)atoi(argv[1]);\nfor (unsigned i = 0; i < count; ++i) host_resource_snapshot(0, 1, i);\nreturn 0;\n}\n')
    binary = directory / "resource-boundary"
    built, stdout, stderr = native([cc, "-std=c11", "-O0", "-g", "-fsanitize=address,undefined", "-Wall", "-Wextra", "-Werror", "-I", library,
                                   control, library / "host_resource.c", "-o", binary], directory, "resource-boundary-build", deadline, environment=environment)
    require(built["process_status"] == "complete" and built["returncode"] == 0 and not stdout and not stderr, "resource bounded observer build")
    boundaries = []
    for count in (255, 256, 257):
        report = directory / f"resource-boundary-{count}.tsv"
        result, stdout, stderr = native([binary, str(count)], directory, f"resource-boundary-{count}", deadline,
                                        environment=dict(environment, SLIM_HOST_RESOURCE_REPORT=str(report)))
        require(result["process_status"] == "complete" and result["returncode"] == 0 and not stdout and not stderr, "resource observer boundary execution")
        boundaries.append({"count": count, "native": result, "report": parse_resource_report(report, min(count, 256), "exact" if count <= 256 else "bounded", controls=True)})
    receipt["allocation_report_boundaries"] = boundaries
    write_json(receipt_path, receipt)


RETAINED_PROBE_PATH = ROOT / "tests/fixtures/retained_project.slim"
RETAINED_PROBE_SHA = "aab88ea4c777dd5c0e0fffcacf1d9fd50c9e827783a4440d18fd281a4699fc57"
RETAINED_WORKLOADS = (
    ("warm", "base", "base", "work", 0),
    ("edit", "base", "edit", "work", 0),
    ("reject", "base", "rejected", "check", 1),
    ("recover", "base", "rejected", "recover", 0),
    ("stale", "base", "edit", "stale", 0),
    ("capacity", "base", "edit", "capacity", 0),
)


def retained_source_pins(source_root):
    source_root = source_root.resolve()
    paths = sorted((source_root / "selfhost").glob("*.slim")) + [source_root / "selfhost/slim.project",
             source_root / "runtime/slim_rt.c", source_root / "runtime/slim_rt.h"]
    require(0 < len(paths) <= 128 and all(value.is_file() for value in paths), "retained source scope")
    total = 0
    result = {}
    for path in paths:
        size = path.stat().st_size
        require(0 <= size <= 1048576 and total <= 4194304 - size, "retained source admission")
        total += size
        result[str(path.relative_to(source_root))] = {"bytes": size, "sha256": digest(path)}
    return result


def retained_source_sha(pins):
    # Portable caller pin: no ignored custody timestamp, modes or absolute paths.
    return hashlib.sha256(json.dumps(pins, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def retained_inputs(fixtures, output):
    directory = output / "retained-inputs"
    directory.mkdir()
    source = fixtures / "prefix"
    expected_main = b"module ma\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"
    require((source / "lib/ma.slim").read_bytes() == expected_main, "retained fixed base source")
    for label in ("base", "edit", "rejected"):
        target = directory / label
        target.mkdir()
        for name in ("slim.project", "lib/m.slim", "lib/ma.slim"):
            path = target / name
            path.parent.mkdir(exist_ok=True)
            path.write_bytes((source / name).read_bytes())
        if label != "base":
            replacement = b"1" if label == "edit" else b"false"
            (target / "lib/ma.slim").write_bytes(expected_main.replace(b"  0\n", b"  " + replacement + b"\n"))
    pins = {str(value.relative_to(directory)): digest(value) for value in directory.rglob("*") if value.is_file()}
    require(len(pins) == 9, "retained three fixed snapshots")
    return directory, pins


def build_retained_probe(source_root, source_pin, compiler, cc, output, label, deadline, environment):
    require(digest(RETAINED_PROBE_PATH) == RETAINED_PROBE_SHA, "retained client source changed")
    pins = retained_source_pins(source_root)
    require(retained_source_sha(pins) == source_pin, "retained explicit source-set pin")
    directory = output / ("retained-" + label)
    directory.mkdir()
    project_dir = directory / "source"
    project_dir.mkdir()
    for name in pins:
        if name.startswith("selfhost/") and name.endswith(".slim"):
            (project_dir / Path(name).name).write_bytes((source_root / name).read_bytes())
    (project_dir / "zzprobe.slim").write_bytes(RETAINED_PROBE_PATH.read_bytes())
    manifest = (source_root / "selfhost/slim.project").read_text()
    require(manifest.count("(entry driver)") == 1 and sum("(module driver " in value for value in manifest.splitlines()) == 1,
            "retained fixed source-manifest anchors")
    text = "\n".join(value for value in manifest.splitlines() if "(module driver " not in value).replace("(entry driver)", "(entry zzprobe)")
    require(text.endswith(")"), "retained source-manifest final close")
    text = text[:-1] + '\n  (module zzprobe "zzprobe.slim" (imports identity memory project retained syntax typing) (exports)))\n'
    project_manifest = project_dir / "slim.project"
    project_manifest.write_text(text)
    project_pins = {str(value.relative_to(project_dir)): digest(value) for value in project_dir.iterdir() if value.is_file()}
    emitted, stdout, stderr = native([compiler, project_manifest], directory, "emit", deadline, environment=environment)
    require(emitted["process_status"] == "complete" and emitted["returncode"] == 0 and stdout and not stderr, "retained native production emission")
    generated = directory / "probe.c"
    generated.write_bytes(stdout)
    binary = directory / "probe"
    runtime = source_root / "runtime"
    built, stdout, stderr = native([cc, "-std=c11", "-O2", "-DNDEBUG", "-Wall", "-Wextra", "-Werror", "-I", runtime,
                                   generated, runtime / "slim_rt.c", "-o", binary], directory, "build", deadline, environment=environment)
    require(built["process_status"] == "complete" and built["returncode"] == 0 and not stdout and not stderr, "retained probe native build")
    require(retained_source_pins(source_root) == pins, "retained source drift while building")
    require({str(value.relative_to(project_dir)): digest(value) for value in project_dir.iterdir() if value.is_file()} == project_pins,
            "retained generated fixture drift")
    return binary, {"source_set_sha256": source_pin, "source_pins": pins, "probe_source_sha256": RETAINED_PROBE_SHA,
                    "project_pins": project_pins, "emission": emitted, "build": built,
                    "generated_sha256": digest(generated), "binary_sha256": digest(binary)}


def retained_paired_campaign(candidate_root, candidate, baseline_root, baseline, baseline_source_pin,
                             cc, fixtures, output, deadline, environment):
    # Baseline caller must SHA-pin its executable separately before this function.
    # --current uses candidate only; --compare requires both old source and artifact.
    inputs, input_pins = retained_inputs(fixtures, output)
    current_pins = retained_source_pins(candidate_root)
    current_pin = retained_source_sha(current_pins)
    probes, builds = {}, {}
    progress_path = output / "retained-progress.json"
    records, ratios = [], []
    def persist():
        write_json(progress_path, {"builds": builds, "observations": records, "samples": ratios,
                                  "input_pins": input_pins, "candidate_source_pin": current_pin})
    persist()
    if baseline is not None:
        probes["baseline"], builds["baseline"] = build_retained_probe(baseline_root, baseline_source_pin, baseline, cc,
                                                                     output, "baseline", deadline, environment)
        persist()
    probes["candidate"], builds["candidate"] = build_retained_probe(candidate_root, current_pin, candidate, cc,
                                                                    output, "candidate", deadline, environment)
    persist()
    observations = output / "retained-observations"
    observations.mkdir()
    golden = {}
    # Observe all six old outcomes BEFORE the first candidate probe invocation.
    for mode, binary in probes.items():
        for name, before, after, operation, status in RETAINED_WORKLOADS:
            arguments = [binary, inputs / before / "slim.project", inputs / after / "slim.project", operation]
            result, stdout, stderr = native(arguments, observations, mode + "-" + name, deadline, environment=environment)
            records.append(result); persist()
            require(result["process_status"] == "complete" and result["returncode"] == status and not stderr,
                    "retained cold/updated/rejected/recovered probe result")
            if name == "warm":
                require(re.fullmatch(rb"0 [0-9]+ [0-9]+\n", stdout) is not None, "retained warm body execution not zero")
            if mode == "baseline":
                golden[name] = (result["returncode"], stdout, stderr)
            elif baseline is not None:
                require((result["returncode"], stdout, stderr) == golden[name], "retained old/current exact output parity")
    if baseline is not None:
        for name, before, after, operation, status in RETAINED_WORKLOADS:
            def invoke(mode, label):
                result, stdout, stderr = native([probes[mode], inputs / before / "slim.project", inputs / after / "slim.project", operation],
                                                observations, name + "-" + label, deadline, environment=environment)
                require(result["process_status"] == "complete" and (result["returncode"], stdout, stderr) == golden[name],
                        "retained timed observation changed outcome")
                return result["elapsed_ns"]
            for mode in probes:
                for warmup in range(2):
                    invoke(mode, f"{mode}-warmup-{warmup}")
            noise = {"a": [], "b": []}
            samples = {"baseline": [], "candidate": []}
            for sample in range(SAMPLES):
                for label in (("a", "b") if sample % 2 == 0 else ("b", "a")):
                    noise[label].append(invoke("baseline", f"noise-{label}-{sample}"))
                for mode in (("baseline", "candidate") if sample % 2 == 0 else ("candidate", "baseline")):
                    samples[mode].append(invoke(mode, f"pair-{mode}-{sample}"))
            medians = {key: statistics.median(value) for key, value in samples.items()}
            noise_medians = {key: statistics.median(value) for key, value in noise.items()}
            noise_ratio = noise_medians["b"] / noise_medians["a"]
            ratio = medians["candidate"] / medians["baseline"]
            row = {"id": name, "scope": "wrapper dispatch through process-group cleanup; includes fresh probe, parent drain/cleanup, cold initial load and cold comparison; isolated retained-update CPU unknown",
                   "samples_ns": samples, "median_ns": medians, "noise_samples_ns": noise, "noise_ratio": noise_ratio,
                   "host_noise_admitted": 1 / 1.10 <= noise_ratio <= 1.10, "candidate_ratio": ratio,
                   "budget": 1.10, "budget_pass": ratio <= 1.10}
            ratios.append(row)
            # Caller must persist partial rows before raising; unfinished rows
            # cannot establish parity, retention adoption or a complete timing gate.
            persist()
            if not row["host_noise_admitted"]:
                raise HostNoiseInconclusive("retained baseline noise outside admitted band: " + name)
            require(row["budget_pass"], "retained permanent same-host budget crossed: " + name)
    require({str(value.relative_to(inputs)): digest(value) for value in inputs.rglob("*") if value.is_file()} == input_pins, "retained input drift")
    require(retained_source_pins(candidate_root) == current_pins, "retained current source drift")
    for mode, binary in probes.items():
        require(digest(binary) == builds[mode]["binary_sha256"], "retained probe executable drift")
    return {"workload_count": 6, "source_pins": input_pins, "builds": builds, "observations": records, "samples": ratios,
            "unknown": ["isolated warm-update CPU", "peak RSS", "allocation count/bytes", "loaded-code/ABA/host-boot custody"]}


class HostNoiseInconclusive(ValueError):
    pass


def baseline_reference_controls(arguments, compiler, output, deadline, environment, receipt, receipt_path):
    fixture = arguments.baseline_source_root.resolve() / "selfhost/slim.project"
    records, goldens = [], {}
    for mode, binary in (("baseline", arguments.baseline_compiler.resolve()), ("candidate", compiler)):
        for command in ("check", "fmt", "interfaces", None):
            label = "emit-c" if command is None else command
            result, stdout, stderr = native([binary, *([] if command is None else [command]), fixture], output,
                                            f"held-selfhost-{mode}-{label}", deadline, environment=environment)
            require(result["process_status"] == "complete" and result["returncode"] == 0 and not stderr, "held unchanged selfhost command")
            if mode == "baseline":
                goldens[label] = (result["returncode"], stdout, stderr)
                if command is None:
                    require(hashlib.sha256(stdout).hexdigest() == arguments.baseline_c_sha256, "baseline self-emission/C identity")
            else:
                require((result["returncode"], stdout, stderr) == goldens[label], "old/current held selfhost output identity")
            records.append(result); receipt["held_selfhost_parity"] = records; write_json(receipt_path, receipt)


def compare_current(arguments, rows, fixtures, compiler, output, deadline, environment, receipt, receipt_path):
    require(arguments.baseline_compiler is not None and arguments.baseline_sha256 is not None, "compare requires explicit baseline artifact and SHA")
    baseline = arguments.baseline_compiler.resolve()
    require(re.fullmatch("[0-9a-f]{64}", arguments.baseline_sha256) is not None and digest(baseline) == arguments.baseline_sha256,
            "explicit baseline executable pin")
    selfhost_root = arguments.baseline_source_root.resolve()
    selfhost_path = selfhost_root / "selfhost/slim.project"
    selfhost_sources = sorted((selfhost_root / "selfhost").glob("*.slim")) + [selfhost_path]
    require(0 < len(selfhost_sources) <= 128 and all(value.is_file() for value in selfhost_sources), "comparison selfhost source scope")
    selfhost_pins = {str(value): digest(value) for value in selfhost_sources}
    receipt["comparison_baseline"] = {"path": str(baseline), "sha256": arguments.baseline_sha256}
    receipt["comparison_selfhost"] = selfhost_pins
    sample_dir = output / "samples"
    sample_dir.mkdir()
    sample_rows = [value for value in rows if value["family"] not in ("control", "malformed")]
    sample_rows.append(next(value for value in rows if value["id"] == "prefix"))
    sample_rows.append({"id": "comparison-selfhost", "status": 0, "diagnostics_hex": ""})
    for row in sample_rows:
        fixture = selfhost_path if row["id"] == "comparison-selfhost" else fixtures / row["id"] / "slim.project"
        def invoke(binary, label):
            result, stdout, stderr = native([binary, "check", fixture], sample_dir, row["id"] + "-" + label, deadline, environment=environment)
            if row["id"] == "comparison-selfhost":
                require(result["process_status"] == "complete" and result["returncode"] == 0 and not stdout and not stderr, "comparison selfhost check")
            else:
                portable_golden(result | {"id": row["id"] + "-check"}, stdout, stderr)
            return result["elapsed_ns"]
        for mode, binary in (("baseline", baseline), ("candidate", compiler)):
            for warmup in range(2):
                invoke(binary, f"{mode}-warmup-{warmup}")
        noise = {"a": [], "b": []}
        paired = {"baseline": [], "candidate": []}
        for sample in range(SAMPLES):
            for label in (("a", "b") if sample % 2 == 0 else ("b", "a")):
                noise[label].append(invoke(baseline, f"noise-{label}-{sample}"))
            for label in (("baseline", "candidate") if sample % 2 == 0 else ("candidate", "baseline")):
                paired[label].append(invoke(baseline if label == "baseline" else compiler, f"pair-{label}-{sample}"))
        noise_medians = {key: statistics.median(value) for key, value in noise.items()}
        medians = {key: statistics.median(value) for key, value in paired.items()}
        noise_ratio = noise_medians["b"] / noise_medians["a"]
        ratio = medians["candidate"] / medians["baseline"]
        result = {"id": row["id"], "scope": "wrapper dispatch through process-group cleanup; includes fresh process and parent drain/cleanup; filesystem cache uncontrolled",
                  "samples_ns": paired, "median_ns": medians, "candidate_ratio": ratio, "budget": 1.10,
                  "noise_samples_ns": noise, "noise_median_ns": noise_medians, "noise_ratio": noise_ratio,
                  "host_noise_admitted": 1 / 1.10 <= noise_ratio <= 1.10, "budget_pass": ratio <= 1.10}
        receipt["samples"].append(result)
        write_json(receipt_path, receipt)
        if not result["host_noise_admitted"]:
            raise HostNoiseInconclusive("same-session baseline noise outside admitted band: " + row["id"])
        require(result["budget_pass"], "permanent same-host 1.10 budget crossed: " + row["id"])
    require(digest(baseline) == arguments.baseline_sha256, "baseline executable changed during comparison")
    require({str(value): digest(value) for value in selfhost_sources} == selfhost_pins, "comparison source changed during campaign")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--prepare", action="store_true")
    action.add_argument("--baseline", action="store_true")
    action.add_argument("--current", action="store_true")
    action.add_argument("--compare", action="store_true")
    parser.add_argument("--output", type=Path, required=True, help="fresh ignored build output")
    parser.add_argument("--baseline-root", type=Path)
    parser.add_argument("--fixtures-root", type=Path, default=ROOT / "build/overnight-manifest/fixtures-held-1")
    parser.add_argument("--cc", default="/usr/bin/cc")
    parser.add_argument("--compiler", type=Path, default=ROOT / "build/toolchain/slimc")
    parser.add_argument("--generated-c", type=Path, default=ROOT / "build/toolchain/slimc.c")
    parser.add_argument("--baseline-compiler", type=Path)
    parser.add_argument("--baseline-sha256")
    parser.add_argument("--comparison-selfhost-root", type=Path)
    parser.add_argument("--baseline-source-root", type=Path)
    parser.add_argument("--baseline-source-sha256")
    parser.add_argument("--baseline-generated-c", type=Path)
    parser.add_argument("--baseline-c-sha256")
    parser.add_argument("--no-timings", action="store_true", help="leave native timing sample gate unknown")
    arguments = parser.parse_args()
    require(arguments.output.resolve().is_relative_to(ROOT / "build"), "measurement output must be ignored build storage")
    if arguments.compare:
        require(arguments.baseline_compiler is not None and arguments.baseline_sha256 is not None, "compare requires explicit baseline artifact and SHA")
        require(re.fullmatch("[0-9a-f]{64}", arguments.baseline_sha256) is not None and
                digest(arguments.baseline_compiler.resolve()) == arguments.baseline_sha256, "explicit baseline executable pin")
    if arguments.compare:
        require(arguments.baseline_source_root is not None and arguments.baseline_generated_c is not None,
                "comparison requires corresponding baseline sources and generated C")
        for value in (arguments.baseline_source_sha256, arguments.baseline_c_sha256):
            require(isinstance(value, str) and re.fullmatch("[0-9a-f]{64}", value) is not None, "baseline source/C pin admission")
        require(retained_source_sha(retained_source_pins(arguments.baseline_source_root)) == arguments.baseline_source_sha256 and
                digest(arguments.baseline_generated_c) == arguments.baseline_c_sha256, "explicit baseline source/C identity")
        require(arguments.comparison_selfhost_root is None or arguments.comparison_selfhost_root.resolve() == arguments.baseline_source_root.resolve(),
                "comparison selfhost must be the same pinned original source set")
    oracle = load_oracle()
    if arguments.prepare:
        payload = oracle.emit(arguments.output)
        print(json.dumps({"cases": len(payload["cases"]), "oracle_sha256": digest(arguments.output / "oracle.json")}, sort_keys=True))
    elif arguments.baseline:
        require(arguments.baseline_root is not None, "baseline needs held original root")
        baseline(arguments, oracle)
    else:
        require(not arguments.no_timings, "--no-timings belongs only to local --baseline")
        current(arguments, oracle)


if __name__ == "__main__":
    main()
