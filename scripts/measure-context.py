#!/usr/bin/env python3
"""Freeze then measure RFC-0159 using only production compiler executables.

Run prepare after the final compiler and sources are fixed. Run run exclusively
after other builds/benchmarks stop. The stdlib runner never builds a compiler,
changes a seed, or supplies semantic acceptance through another implementation.
All timings are same-host observations, separate from deterministic assertions
and the existing repository gates. Unobserved phase costs remain unknown.
"""

import argparse
import datetime
import hashlib
import json
import math
from pathlib import Path
import platform
import re
import statistics
import subprocess
import sys
import time


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "benchmarks/context-workloads.json"
FLAGS = ["-std=c11", "-O2", "-DNDEBUG", "-Wall", "-Wextra", "-Werror"]
MODULE = re.compile(rb'\(module\s+([A-Za-z_][A-Za-z0-9_]*)\s+"([^"\\]+)"')
MAIN = "\nfn main(args: Vec[Bytes]) -> I64:\n  0\n"


class MeasurementFailure(Exception):
    pass


def require(condition, message):
    if not condition:
        raise MeasurementFailure(message)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def identity(path):
    path = Path(path).resolve()
    data = path.read_bytes()
    return {"path": str(path), "bytes": len(data), "sha256": sha(data)}


def save(path, data):
    pending = path.with_name(path.name + ".pending")
    pending.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    pending.replace(path)


def generated_source(kind, size):
    """Source fixture generation only; no SLIM parsing, linking or typing."""
    if kind == "helper_chain":
        helpers = "fn helper_0(value: I64) -> I64:\n  value\n\n"
        helpers += "".join(
            f"fn helper_{i}(value: I64) -> I64:\n  helper_{i-1}(value)\n\n"
            for i in range(1, size))
        return ("module scale\n\n" + helpers +
                f"fn main(args: Vec[Bytes]) -> I64:\n  helper_{size-1}(1)\n").encode()
    prefix = "module probe\n\n"
    if kind in ("repeated_calls", "nested_expressions"):
        prefix += "fn identity(value: I64) -> I64:\n  value\n\n"
    header = "fn selected(value: I64) -> I64:\n"
    if kind == "repeated_calls":
        body = "".join(f"  let v{i}: I64 = identity(value)\n" for i in range(size)) + "  value\n"
    elif kind == "let_continuations":
        body = "".join(f"  let v{i}: I64 = value\n" for i in range(size)) + "  value\n"
    elif kind == "nested_expressions":
        body = "  " + "identity(" * size + "value" + ")" * size + "\n"
    elif kind == "nested_let_if":
        # Each initializer owns a nested branch/let continuation. Indentation
        # contributes quadratic source bytes, independently of AST node growth.
        header = "fn selected(flag: Bool) -> I64:\n"
        body = ""
        for i in range(size):
            body += "  " * (i + 1) + f"let v{i}: I64 = if flag:\n"
        body += "  " * (size + 1) + "0\n"
        for i in reversed(range(size)):
            body += "  " * (i + 1) + "else:\n"
            body += "  " * (i + 2) + "0\n"
            body += "  " * (i + 1) + f"v{i}\n"
    elif kind == "nested_if_continuations":
        header = "fn selected(flag: Bool) -> I64:\n"
        body = "".join("  " * (i + 1) + "if flag:\n" for i in range(size))
        body += "  " * (size + 1) + "0\n"
        for i in reversed(range(size)):
            body += "  " * (i + 1) + "else:\n" + "  " * (i + 2) + "0\n"
    elif kind == "selected_source_reject":
        head, tail = b"fn selected() -> I64:\n  #", b"\n  0"
        selected = head + b"x" * (size - len(head) - len(tail)) + tail
        return b"module selected_limit\n\n" + selected + MAIN.encode()
    elif kind == "report_reject":
        providers = "".join(f"fn p{i}(" + "n" * 7900 + ": I64) -> I64:\n  0\n\n"
                            for i in range(size))
        body = "".join(f"  let v{i}: I64 = p{i}(0)\n" for i in range(size)) + "  0\n"
        return ("module report_limit\n\n" + providers +
                "fn selected() -> I64:\n" + body + MAIN).encode()
    else:
        raise MeasurementFailure("unknown frozen generator: " + kind)
    return (prefix + header + body + MAIN).encode()


def project_files(path):
    """Locate files only in the two named repository workload manifests.

    This deliberately narrow capture recognizer rejects escaped/absolute paths.
    It provides fixture copies, never compiler semantics or manifest acceptance;
    both production compilers still check the unchanged manifest themselves.
    """
    data = path.read_bytes()
    modules = MODULE.findall(data)
    require(modules and len(modules) == data.count(b"(module"),
            "unsupported workload manifest capture shape: " + str(path))
    files = []
    for name, raw_path in modules:
        relative = Path(raw_path.decode("ascii"))
        require(not relative.is_absolute() and ".." not in relative.parts,
                "workload module path must stay within its project directory")
        files.append((name.decode("ascii"), relative))
    require(len({str(p) for _, p in files}) == len(files), "duplicate workload module path")
    return data, files


def input_digest(manifest, files):
    # Length-framed complete bytes avoid ambiguous concatenation fingerprints.
    digest = hashlib.sha256()
    for value in ([manifest] if manifest is not None else []) + [f["data"] for f in files]:
        digest.update(len(value).to_bytes(8, "big"))
        digest.update(value)
    return digest.hexdigest()


def prepare(args):
    work = args.work.resolve()
    frozen_path = work / "frozen.json"
    require(not frozen_path.exists(), "frozen.json already exists; retain it and use a new --work directory")
    manifest_bytes = MANIFEST.read_bytes()
    manifest = json.loads(manifest_bytes)
    require(manifest["schema"] == 1 and manifest["compiler_flags"] == FLAGS, "unsupported measurement protocol")
    work.mkdir(parents=True, exist_ok=True)
    watched = {}

    def watch(path):
        value = identity(path)
        old = watched.get(value["path"])
        require(old is None or old == value, "file changed while freezing: " + value["path"])
        watched[value["path"]] = value
        return value

    toolchains = {}
    for key in ("baseline", "candidate"):
        runtime = getattr(args, key + "_runtime").resolve()
        toolchains[key] = {
            "executable": watch(getattr(args, key)),
            "compiler_c": watch(getattr(args, key + "_c")),
            "runtime": {name: watch(runtime / name) for name in ("slim_rt.c", "slim_rt.h")},
        }
    for name in ("slim_rt.c", "slim_rt.h"):
        require(Path(toolchains["baseline"]["runtime"][name]["path"]).read_bytes() ==
                Path(toolchains["candidate"]["runtime"][name]["path"]).read_bytes(),
                "baseline and candidate runtime bytes differ: " + name)
    watch(Path(__file__))
    require(watch(MANIFEST)["sha256"] == sha(manifest_bytes), "manifest changed while freezing")
    inputs = {}
    ordinary, contexts = [], []

    def capture(spec, size=None):
        key = spec["id"] if size is None else f'{spec["id"]}-{size}'
        # Maintained projects are shared between ordinary/context workloads.
        key = "project-" + spec["id"] if "project" in spec else key
        if key in inputs:
            return key
        current, expected = work / "inputs" / key / "current", work / "inputs" / key / "expected"
        require(not current.exists() and not expected.exists(), "fixture directory already exists: " + key)
        current.mkdir(parents=True)
        expected.mkdir(parents=True)
        manifest_bytes = None
        files = []
        if "project" in spec:
            origin = ROOT / spec["project"]
            watch(origin)
            manifest_bytes, located = project_files(origin)
            source_name = origin.name
            for directory in (current, expected):
                (directory / source_name).write_bytes(manifest_bytes)
                watch(directory / source_name)
            for name, relative in located:
                watch(origin.parent / relative)
                data = (origin.parent / relative).read_bytes()
                files.append({"module": name, "relative": str(relative), "data": data})
        else:
            source_name = "program.slim"
            data = generated_source(spec["generator"], size)
            files.append({"module": spec.get("selector", "scale.selected").split(".")[0],
                          "relative": source_name, "data": data})
        for file in files:
            for directory in (current, expected):
                path = directory / file["relative"]
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(file["data"])
                watch(path)
        inputs[key] = {
            "source": str(current / source_name), "expected": str(expected / source_name),
            "kind": "project" if manifest_bytes is not None else "module",
            "source_sha256": input_digest(manifest_bytes, files) if manifest_bytes is not None else sha(files[0]["data"]),
            "source_sha256_framing": "8-byte-big-endian-length-framed-manifest-and-ordered-modules" if manifest_bytes is not None else "raw-module-bytes",
            "complete_input_sha256": input_digest(manifest_bytes, files),
            "manifest_sha256": sha(manifest_bytes) if manifest_bytes is not None else None,
            "source_bytes": sum(len(f["data"]) for f in files) + len(manifest_bytes or b""),
            "files": [{"slot": i, "module": f["module"], "path": str(current / f["relative"]),
                       "expected_path": str(expected / f["relative"]),
                       "bytes": len(f["data"]), "sha256": sha(f["data"])} for i, f in enumerate(files)],
        }
        return key

    for section, output in (("ordinary", ordinary), ("context", contexts)):
        for spec in manifest[section]:
            for size in spec.get("sizes", [None]):
                output.append({**spec, "size": size, "input": capture(spec, size)})
    frozen = {
        "schema": 1, "protocol": manifest["protocol"],
        "frozen_at_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "host": {"system": platform.system(), "machine": platform.machine(),
                 "platform": platform.platform(), "python": platform.python_version()},
        "build_provenance": {"flags": FLAGS, "evidence": "caller-supplied-build-configuration",
                             "reason": "binary hashes do not independently prove compilation flags"},
        "manifest": manifest, "manifest_identity": identity(MANIFEST),
        "generator_identity": identity(Path(__file__)), "toolchains": toolchains,
        "inputs": inputs, "ordinary": ordinary, "context": contexts,
        "watched": list(watched.values()),
    }
    verify_identities(frozen)
    save(frozen_path, frozen)
    print(f"frozen {len(ordinary)} ordinary and {len(contexts)} context workloads: {frozen_path}", flush=True)
    print("No compiler was invoked; run the frozen protocol only after competing jobs stop.", flush=True)


def verify_identities(frozen):
    changes = []
    for expected in frozen["watched"]:
        try:
            actual = identity(expected["path"])
        except OSError as error:
            changes.append({"path": expected["path"], "error": str(error)})
            continue
        if actual != expected:
            changes.append({"expected": expected, "actual": actual})
    require(not changes, "frozen identity changed: " + json.dumps(changes))
    return {"evidence": "exact", "checked_files": len(frozen["watched"]), "changes": []}


def invocation(executable, command, fixture, selector=None, timed=False, timeout=60):
    source = fixture["source"]
    if command == "emit":
        argv = [executable, source]
    elif command == "context":
        argv = [executable, "context", source, fixture["expected"], selector]
    else:
        argv = [executable, "check", source]
    started = time.perf_counter_ns() if timed else None
    result = subprocess.run(argv, capture_output=True, timeout=timeout)
    elapsed = time.perf_counter_ns() - started if timed else None
    return result, elapsed


def exact_output(result):
    return result.returncode, result.stdout, result.stderr


def output_identity(result):
    return {"returncode": result.returncode, "stdout_bytes": len(result.stdout),
            "stdout_sha256": sha(result.stdout), "stderr_bytes": len(result.stderr),
            "stderr_sha256": sha(result.stderr)}


def check_success(result, label):
    require(result.returncode == 0 and not result.stderr,
            label + ": normal compiler rejected source: " +
            repr((result.returncode, result.stdout[:300], result.stderr[:300])))


def section_summary(report):
    return {key: {"evidence": report[key]["evidence"], "limit": report[key]["limit"],
                  "reason": report[key]["reason"], "rows": len(report[key]["rows"])}
            for key in ("providers", "facts", "references")}


def context_evidence(result, spec, fixture, protocol):
    if spec["outcome"] == "rejected":
        require(result.returncode == 1 and not result.stderr and
                result.stdout == (spec["diagnostic"] + "@0:0\n").encode() and
                not result.stdout.lstrip().startswith(b"{"),
                spec["id"] + ": expected explicit limit diagnostic without partial JSON")
        return {"outcome": "rejected", "diagnostic": result.stdout.decode("ascii").strip(),
                "reason": spec["rejection_reason"], "work": None, "sections": None,
                "report_bytes": None, "diagnostic_bytes": len(result.stdout)}
    check_success(result, spec["id"])
    require(result.stdout.endswith(b"\n"), "context report must include its final newline")
    report = json.loads(result.stdout)
    require(set(report) == {"schema", "encoding", "identity_evidence", "selector", "input", "limits",
                            "selected", "providers", "facts", "references", "work"}, "unexpected context root fields")
    require(report["schema"] == 1 and report["encoding"] == "json-byte-escapes-v1" and
            report["identity_evidence"] == "exact-expected-input-bytes", "unsupported context schema")
    require(report["selector"] == spec["selector"] and report["limits"] == protocol["limits"],
            "selector or fixed limits differ from the frozen contract")
    require(report["input"]["kind"] == fixture["kind"], "wrong captured input kind")
    require(len(report["input"]["files"]) == len(fixture["files"]), "captured module count mismatch")
    for actual, frozen in zip(report["input"]["files"], fixture["files"]):
        require(actual["slot"] == frozen["slot"] and actual["module"] == frozen["module"] and
                actual["bytes"] == frozen["bytes"], "captured module provenance mismatch")
    work = report["work"]
    require(set(work) == {"inspected_nodes", "fact_rows", "reference_rows", "provider_rows",
                          "source_bytes_copied", "report_bytes"}, "unexpected work counter fields")
    require(all(type(value) is int and value >= 0 for value in work.values()), "invalid work counter")
    require(work["report_bytes"] == len(result.stdout) <= protocol["limits"]["report_bytes"],
            "report-byte accounting or limit mismatch")
    sections = section_summary(report)
    for key, count in (("providers", "provider_rows"), ("facts", "fact_rows"), ("references", "reference_rows")):
        section = report[key]
        require(section["limit"] == protocol["limits"][key] and
                len(section["rows"]) == work[count] <= section["limit"], "section row accounting mismatch")
        require((section["evidence"] == "exact" and section["reason"] == "") or
                (section["evidence"] == "bounded" and section["reason"] == "row-limit" and
                 len(section["rows"]) == section["limit"]), "incorrect section completeness")
    selected = report["selected"]
    span = selected["span"]
    require(0 <= span["file"] < len(fixture["files"]), "invalid selected source file")
    source = Path(fixture["files"][span["file"]]["path"]).read_bytes()
    require(0 <= span["start"] <= span["end"] <= len(source), "invalid selected original source span")
    selected_bytes = selected["source"].encode("latin-1")
    require(selected_bytes == source[span["start"]:span["end"]] and
            len(selected_bytes) <= protocol["limits"]["selected_source_bytes"],
            "selected source is not complete byte-exact original source")
    sources = [Path(file["path"]).read_bytes() for file in fixture["files"]]

    def validate_spans(value):
        if isinstance(value, list):
            for child in value:
                validate_spans(child)
        elif isinstance(value, dict):
            if set(value) == {"file", "start", "end"}:
                require(0 <= value["file"] < len(sources) and
                        0 <= value["start"] <= value["end"] <= len(sources[value["file"]]),
                        "invalid original source span")
            for child in value.values():
                validate_spans(child)
            for text_key, span_key in (("source", "span"), ("signature", "signature_span"), ("name", "name_span")):
                if value.get(text_key) is not None and value.get(span_key) is not None:
                    location = value[span_key]
                    require(value[text_key].encode("latin-1") ==
                            sources[location["file"]][location["start"]:location["end"]],
                            "reported text differs from its original source span")
            if value.get("kind") == "source-form" and value.get("evidence") == "exact":
                location = value["span"]
                require(value["text"].encode("latin-1") == sources[location["file"]][location["start"]:location["end"]],
                        "source-backed type text differs from its original source span")

    validate_spans(report)
    span_lengths = []
    for fact in report["facts"]["rows"]:
        value = fact["span"]
        if value is not None:
            require(0 <= value["file"] < len(fixture["files"]) and
                    0 <= value["start"] <= value["end"] <= fixture["files"][value["file"]]["bytes"],
                    "invalid reported fact span")
            span_lengths.append(value["end"] - value["start"])
    if "reference_count" in spec:
        total = spec["size"] if spec["reference_count"] == "size" else spec["reference_count"]
        require(work["reference_rows"] == min(total, 512) and
                report["references"]["evidence"] == ("bounded" if total > 512 else "exact"),
                "generated fixture reference count/completeness mismatch")
    if "provider_count" in spec:
        require(work["provider_rows"] == spec["provider_count"] and
                report["providers"]["evidence"] == "exact", "generated fixture provider mismatch")
    if spec.get("size", 0) is not None and spec.get("size", 0) >= 1024:
        require(work["fact_rows"] == 512 and report["facts"]["evidence"] == "bounded",
                "large generated selected fixture must cross the fact cap")
    return {"outcome": "accepted", "diagnostic": None, "work": work, "sections": sections,
            "report_bytes": len(result.stdout), "selected_source_bytes": len(selected_bytes),
            "selected_source_sha256": sha(selected_bytes), "selected_node": selected["node"],
            "fact_span_bytes_sum": sum(span_lengths), "max_fact_span_bytes": max(span_lengths, default=0)}


def summarize(samples):
    return {"samples_seconds": [value / 1e9 for value in samples],
            "median_seconds": statistics.median(samples) / 1e9,
            "min_seconds": min(samples) / 1e9, "max_seconds": max(samples) / 1e9}


def exponents(rows, metric):
    rows = sorted(rows, key=lambda row: row["size"])
    values = []
    for left, right in zip(rows, rows[1:]):
        a, b = metric(left), metric(right)
        if a > 0 and b > 0:
            values.append({"from_size": left["size"], "to_size": right["size"],
                           "size_exponent": math.log(b / a) / math.log(right["size"] / left["size"]),
                           "source_byte_exponent": math.log(b / a) /
                           math.log(right["source_bytes"] / left["source_bytes"])})
    return values


def run(args):
    work = args.work.resolve()
    frozen_path = work / "frozen.json"
    frozen = json.loads(frozen_path.read_text())
    frozen_identity = identity(frozen_path)
    report_path = args.report.resolve() if args.report else work / "results.json"
    require(not report_path.exists(), "result already exists; retain it and use a distinct --report path")
    report_path.parent.mkdir(parents=True, exist_ok=True)
    protocol = frozen["manifest"]
    timing = protocol["timing"]
    executables = {"A": frozen["toolchains"]["baseline"]["executable"]["path"],
                   "B": frozen["toolchains"]["candidate"]["executable"]["path"]}
    result = {"schema": 1, "protocol": frozen["protocol"], "status": "running",
              "started_at_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
              "frozen_identity": frozen_identity, "toolchains": frozen["toolchains"],
              "host": frozen["host"], "build_provenance": frozen["build_provenance"],
              "metric_definitions": protocol["metrics"], "phase_isolation": protocol["phase_isolation"],
              "timing_protocol": timing, "ordinary": [], "context": [],
              "timing_gates": {"evidence": "unknown", "reason": "observations-only; existing gates remain authoritative"}}

    def checkpoint():
        save(report_path, result)

    def stable_sample(key, command, fixture, expected, selector=None):
        observed, elapsed = invocation(executables[key], command, fixture, selector,
                                       timed=True, timeout=timing["timeout_seconds"])
        require(exact_output(observed) == exact_output(expected), "sample changed output: " + command)
        return elapsed

    try:
        result["identities_before"] = verify_identities(frozen)
        checkpoint()
        # Preserve parity outputs for shared project/context inputs rather than
        # repeatedly checking/emitting the same immutable fixture at preflight.
        preflight = {}
        for spec in frozen["ordinary"]:
            fixture = frozen["inputs"][spec["input"]]
            row = {"id": spec["id"], "size": spec["size"], "source_bytes": fixture["source_bytes"],
                   "source_sha256": fixture["source_sha256"], "input": spec["input"], "commands": {}}
            result["ordinary"].append(row)
            for command in ("check", "emit"):
                outputs = {}
                for key in ("A", "B"):
                    outputs[key], _ = invocation(executables[key], command, fixture,
                                                timeout=timing["timeout_seconds"])
                    check_success(outputs[key], spec["input"] + " " + command + " " + key)
                require(exact_output(outputs["A"]) == exact_output(outputs["B"]),
                        "ordinary baseline/candidate output mismatch: " + spec["input"] + " " + command)
                preflight[(spec["input"], command)] = outputs["B"]
                data = {"output": output_identity(outputs["B"]), "output_equality": "exact", "pairs": []}
                row["commands"][command] = data
                warmups = {key: stable_sample(key, command, fixture, outputs[key]) for key in ("A", "B")}
                data["warmup_seconds"] = {key: value / 1e9 for key, value in warmups.items()}
                for pair in range(timing["ordinary_pairs"]):
                    order = timing["ordinary_orders"][pair % 2]
                    ns = {key: stable_sample(key, command, fixture, outputs[key]) for key in order}
                    data["pairs"].append({"pair": pair, "order": order, "baseline_ns": ns["A"],
                                          "candidate_ns": ns["B"], "ratio": ns["B"] / ns["A"]})
                    checkpoint()
                for name, key in (("baseline", "baseline_ns"), ("candidate", "candidate_ns")):
                    data[name] = summarize([pair[key] for pair in data["pairs"]])
                data["median_paired_ratio"] = statistics.median(pair["ratio"] for pair in data["pairs"])
                print(f'{spec["input"]} {command}: paired median {data["median_paired_ratio"]:.3f}', flush=True)
            verify_identities(frozen)
            checkpoint()
        for spec in frozen["context"]:
            fixture = frozen["inputs"][spec["input"]]
            normal = {}
            parity = {}
            for command in ("check", "emit"):
                cached = preflight.get((spec["input"], command))
                if cached is None:
                    old, _ = invocation(executables["A"], command, fixture, timeout=timing["timeout_seconds"])
                    new, _ = invocation(executables["B"], command, fixture, timeout=timing["timeout_seconds"])
                    check_success(old, spec["input"] + " " + command + " A")
                    check_success(new, spec["input"] + " " + command + " B")
                    require(exact_output(old) == exact_output(new), "context fixture ordinary output differs: " + spec["input"])
                    cached = new
                normal[command] = cached
                parity[command] = output_identity(cached)
            first, _ = invocation(executables["B"], "context", fixture, spec["selector"],
                                  timeout=timing["timeout_seconds"])
            second, _ = invocation(executables["B"], "context", fixture, spec["selector"],
                                   timeout=timing["timeout_seconds"])
            require(exact_output(first) == exact_output(second), "context output is not deterministic: " + spec["input"])
            evidence = context_evidence(first, spec, fixture, protocol)
            row = {"id": spec["id"], "size": spec["size"], "selector": spec["selector"],
                   "input": spec["input"], "source_sha256": fixture["source_sha256"],
                   "source_bytes": fixture["source_bytes"], "ordinary_output_equality": "exact",
                   "ordinary_outputs": parity, "determinism": "exact", "output": output_identity(first),
                   **evidence, "samples": []}
            if spec.get("nested_suffix_items"):
                row["nested_suffix_items"] = spec["size"] * (spec["size"] + 1) // 2
                row["nested_suffix_evidence"] = "generator-structural-proxy; not observed compiler work"
            result["context"].append(row)
            outputs = {"check": normal["check"], "context": first}
            row["warmup_seconds"] = {
                command: stable_sample("B", command, fixture, outputs[command], spec["selector"]) / 1e9
                for command in ("check", "context")}
            for sample in range(timing["context_samples"]):
                order = timing["context_check_orders"][sample % 2].split("/")
                ns = {command: stable_sample("B", command, fixture, outputs[command], spec["selector"])
                      for command in order}
                row["samples"].append({"sample": sample, "order": order,
                                       "check_ns": ns["check"], "context_ns": ns["context"]})
                checkpoint()
            row["native_process_seconds"] = summarize([sample["context_ns"] for sample in row["samples"]])
            row["ordinary_check_seconds"] = summarize([sample["check_ns"] for sample in row["samples"]])
            # Raw bounded/rejected data remain rows, including expensive report
            # and selected-source rejections. They are never silently excluded.
            print(f'{spec["input"]} context: {row["outcome"]}, '
                  f'{row["native_process_seconds"]["median_seconds"]:.6f}s, '
                  f'report={row["report_bytes"]}', flush=True)
            verify_identities(frozen)
            checkpoint()
        geometries = []
        for family in protocol["context"]:
            rows = [row for row in result["context"] if row["id"] == family["id"] and
                    row["outcome"] == "accepted" and row["size"] is not None]
            if len(rows) < 2:
                continue
            rows.sort(key=lambda row: row["size"])
            for left, right in zip(rows, rows[1:]):
                require(right["size"] == 2 * left["size"] and
                        right["work"]["inspected_nodes"] <= 2 * left["work"]["inspected_nodes"] + 64,
                        "selected lexical scan geometric assertion failed: " + family["id"])
            geometries.append({"id": family["id"], "assertion": "selected lexical scan only; 2x plus 64 on size doubling",
                               "inspected_nodes": exponents(rows, lambda row: row["work"]["inspected_nodes"]),
                               "fact_span_bytes_sum": exponents(rows, lambda row: row["fact_span_bytes_sum"]),
                               "native_process_seconds": exponents(rows, lambda row: row["native_process_seconds"]["median_seconds"]),
                               "timing_gate": None})
        result["context_geometry"] = geometries
        result["ordinary_geometry"] = {
            command: {key: exponents([row for row in result["ordinary"] if row["id"] == "helper_chain"],
                                    lambda row: row["commands"][command][key]["median_seconds"])
                      for key in ("baseline", "candidate")}
            for command in ("check", "emit")}
        result["status"] = "complete"
    except BaseException as error:
        result["status"] = "interrupted" if isinstance(error, KeyboardInterrupt) else "failed"
        result["failure"] = {"type": type(error).__name__, "message": str(error)}
        raise
    finally:
        try:
            result["identities_after"] = verify_identities(frozen)
            require(identity(frozen_path) == frozen_identity, "frozen.json changed during measurements")
        except (MeasurementFailure, OSError) as error:
            result["status"] = "failed"
            result["identity_failure"] = str(error)
        result["finished_at_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        checkpoint()
    require(result["status"] == "complete", "identity verification failed; see " + str(report_path))
    print(f"complete: {report_path}", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    freeze = commands.add_parser("prepare", help="capture and hash every workload; invokes no compiler")
    for key in ("baseline", "candidate"):
        freeze.add_argument("--" + key, type=Path, required=True)
        freeze.add_argument("--" + key + "-c", type=Path, required=True)
        freeze.add_argument("--" + key + "-runtime", type=Path, required=True)
    freeze.add_argument("--work", type=Path, required=True)
    execute = commands.add_parser("run", help="execute the already frozen protocol exclusively")
    execute.add_argument("--work", type=Path, required=True)
    execute.add_argument("--report", type=Path)
    args = parser.parse_args()
    try:
        (prepare if args.command == "prepare" else run)(args)
    except (MeasurementFailure, OSError, ValueError, subprocess.TimeoutExpired) as error:
        print("measure-context: " + str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
