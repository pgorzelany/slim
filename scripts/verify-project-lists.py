#!/usr/bin/env python3
"""Verify canonical project lists through the production SLIM compiler.

--generated-c observes deterministic work in generated production C; it never
implements manifest checking or alters the compiler's accepted result.
--baseline additionally collects native elapsed samples in a coordinated quiet
host slot. Native samples describe one host; deterministic work is the portable
gate.
"""

import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shlex
import statistics
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
COUNTER_CAP = 1_000_000_000
SIZES = (16, 32, 64, 128, 256)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(compiler, command, path):
    arguments = [str(compiler)] + ([command] if command else []) + [str(path)]
    return subprocess.run(arguments, capture_output=True, timeout=30)


def check_fixtures(compiler):
    manifest = ROOT / "conformance/projects/manifest.tsv"
    before = digest(manifest)
    checked = []
    for line in manifest.read_text().splitlines():
        if not line or line.startswith("#"):
            continue
        identity, mode, name, classification, expectation, _ = line.split("\t")
        if mode not in ("check-pass", "check-fail"):
            continue
        assert classification == "parity", identity
        expected = b"" if mode == "check-pass" else (expectation.replace(",", "\n") + "\n").encode()
        status = 0 if mode == "check-pass" else 1
        path = ROOT / name
        source_before = digest(path)
        commands = ("check", None, "interfaces", "fmt") if identity.startswith("project-list-") and status else ("check",)
        for command in commands:
            first, second = run(compiler, command, path), run(compiler, command, path)
            for result in (first, second):
                assert (result.returncode, result.stdout, result.stderr) == (status, expected, b""), (
                    identity, command, result.returncode, result.stdout, result.stderr
                )
        assert digest(path) == source_before, identity
        checked.append({"id": identity, "status": status, "diagnostics": expected.decode().splitlines(), "runs_per_command": 2})
    assert digest(manifest) == before
    path = ROOT / "conformance/projects/canonical-lists/sorted.project"
    interfaces = [run(compiler, "interfaces", path) for _ in range(2)]
    assert all(result.returncode == 0 and not result.stderr for result in interfaces)
    assert interfaces[0].stdout == interfaces[1].stdout
    for line in interfaces[0].stdout.decode().splitlines():
        names = re.findall(r"\(fn (\S+) ", line)
        assert names == sorted(set(names), key=lambda name: name.encode()), names
    return checked


def audit_manifests():
    # Production/library clients and accepted conformance rows are maintained
    # manifests. Negative and frozen experiment corpora keep their own checker
    # expectations; discovering a future negative fixture must not fail this
    # independent canonical-client audit.
    paths = {ROOT / "selfhost/slim.project"}
    for directory in ("library", "examples", "tools", "applications"):
        paths.update((ROOT / directory).glob("*.project"))
    for line in (ROOT / "conformance/projects/manifest.tsv").read_text().splitlines():
        if line and not line.startswith("#"):
            row = line.split("\t")
            if row[1] != "check-fail":
                paths.add(ROOT / row[2])
    audited = []
    for path in sorted(paths):
        name = str(path.relative_to(ROOT))
        for match in re.finditer(r"\((imports|exports)\b([^()]*)\)", path.read_text()):
            names = match[2].split()
            assert names == sorted(set(names), key=lambda name: name.encode()), (name, match[1], names)
        audited.append({"path": name, "sha256": digest(path)})
    return audited


def check_client_interfaces(compiler, manifests):
    checked = []
    for manifest in manifests:
        path = ROOT / manifest["path"]
        first, second = run(compiler, "interfaces", path), run(compiler, "interfaces", path)
        assert first.returncode == second.returncode == 0 and first.stderr == second.stderr == b"", (path, first.stdout, first.stderr)
        assert first.stdout == second.stdout, path
        for line in first.stdout.decode().splitlines():
            names = re.findall(r"\((?:fn|record|variant) (\S+) ", line)
            assert names == sorted(set(names), key=lambda name: name.encode()), (path, names)
        assert digest(path) == manifest["sha256"], path
        checked.append({"path": manifest["path"], "interface_bytes": len(first.stdout), "interface_sha256": hashlib.sha256(first.stdout).hexdigest(), "runs": 2})
    return checked


def namespace_function(module, name):
    escape = lambda part: part.replace("_", "_0").replace(".", "_1")
    return "slim_fn_" + ("p" + escape(module) + "__" + escape(name)).replace("_", "_95")


def instrument(source):
    functions = {
        namespace_function("project", "find_unsorted_module_lists"): "module_steps",
        namespace_function("project", "find_unsorted_name_list"): "name_steps",
        namespace_function("project", "span_less_chars"): "byte_steps",
    }
    counters = tuple(functions.values())
    prefix = "#include <stdint.h>\n#include <stdio.h>\n#include <stdlib.h>\n"
    prefix += "static int slim_manifest_inside_lists;\n"
    for name in counters:
        prefix += f"static uint64_t slim_manifest_{name};\n"
    prefix += "static void slim_manifest_add(uint64_t *value) {\n"
    prefix += f"if (*value >= UINT64_C({COUNTER_CAP})) abort();\n++*value;\n}}\n"
    prefix += "static void slim_manifest_report(void) {\n"
    prefix += 'fprintf(stderr, "SLIM_PROJECT_LIST_WORK %llu %llu %llu\\n",\n'
    prefix += ", ".join(f"(unsigned long long)slim_manifest_{name}" for name in counters) + ");\n}\n"
    output = [prefix]
    active = None
    definitions, headers, exits = {name: 0 for name in functions}, {name: 0 for name in functions}, 0
    mains = 0
    for line in source.splitlines(keepends=True):
        if line.startswith("static SLIM_UNUSED_FUNCTION ") and line.rstrip().endswith(" {"):
            active = next((name for name in functions if f" {name}(" in line), None)
            if active:
                definitions[active] += 1
        if active == namespace_function("project", "find_unsorted_module_lists") and line.startswith("return "):
            output.append("slim_manifest_inside_lists = 0;\n")
            exits += 1
        output.append(line)
        if active and definitions[active] and line.startswith("static SLIM_UNUSED_FUNCTION "):
            if functions[active] == "module_steps":
                output.append("if (slim_manifest_inside_lists) abort();\nslim_manifest_inside_lists = 1;\n")
        if active and line == "slim_recur: ;\n":
            headers[active] += 1
            output.append(f"if (slim_manifest_inside_lists) slim_manifest_add(&slim_manifest_{functions[active]});\n")
        if line == "int main(int argc, char **argv) {\n":
            mains += 1
            output.append("if (atexit(slim_manifest_report) != 0) abort();\n")
    assert all(count == 1 for count in definitions.values()), definitions
    assert all(count == 1 for count in headers.values()), headers
    assert exits == 1 and mains == 1, (exits, mains)
    return "".join(output)


def fixture(directory, kind, size):
    names = [f"k{index:05}" for index in range(size)]
    if kind == "exports":
        source = "module app\n\n" + "".join(f"fn {name}() -> I64:\n  1\n\n" for name in names)
        source += "fn main(args: Vec[Bytes]) -> I64:\n  0\n"
        (directory / "app.slim").write_text(source)
        manifest = '(project 1 (entry app) (module app "app.slim" (imports) (exports ' + " ".join(names) + ")))\n"
        module_count, name_count = 1, size
    else:
        (directory / "app.slim").write_text("module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n")
        manifest = '(project 1 (entry app) (module app "app.slim" (imports ' + " ".join(names) + ") (exports))\n"
        for index, name in enumerate(names):
            filename = f"lib{index:05}.slim"
            (directory / filename).write_text(f"module {name}\n\nfn value() -> I64:\n  1\n")
            manifest += f'  (module {name} "{filename}" (imports) (exports))\n'
        manifest += ")\n"
        module_count, name_count = size + 1, size
    path = directory / "slim.project"
    path.write_text(manifest)
    byte_steps = 0
    for left, right in zip(names, names[1:]):
        byte_steps += next((index + 1 for index, pair in enumerate(zip(left, right)) if pair[0] != pair[1]), min(len(left), len(right)) + 1)
    expected = {"module_steps": module_count + 1, "name_steps": name_count + 2 * module_count, "byte_steps": byte_steps}
    return path, expected


def measure(compiler, baseline, generated, directory):
    observed_c = directory / "observed.c"
    observed_c.write_text(instrument(generated.read_text()))
    observed = directory / "observed"
    command = shlex.split(os.environ.get("CC", "cc")) + ["-std=c11", "-O2", "-DNDEBUG", "-Wall", "-Wextra", "-Werror", "-I", str(ROOT / "runtime"), str(observed_c), str(ROOT / "runtime/slim_rt.c"), "-o", str(observed)]
    subprocess.run(command, check=True, timeout=120, capture_output=True)
    rows = []
    for kind in ("imports", "exports"):
        for size in SIZES:
            case = directory / f"{kind}-{size}"
            case.mkdir()
            path, expected = fixture(case, kind, size)
            plain, work = run(compiler, "check", path), run(observed, "check", path)
            assert plain.returncode == work.returncode == 0 and plain.stdout == work.stdout == b""
            assert plain.stderr == b""
            fields = work.stderr.decode().strip().split()
            assert fields[0] == "SLIM_PROJECT_LIST_WORK" and len(fields) == 4, fields
            actual = dict(zip(("module_steps", "name_steps", "byte_steps"), map(int, fields[1:])))
            assert actual == expected, (kind, size, expected, actual)
            source_bytes = sum(p.stat().st_size for p in case.iterdir())
            assert sum(actual.values()) <= 2 * path.stat().st_size
            row = {"kind": kind, "names": size, "source_bytes": source_bytes, "manifest_bytes": path.stat().st_size, "work": actual}
            if baseline:
                samples = {"baseline": [], "candidate": []}
                for binary in (baseline, compiler):
                    for _ in range(2):
                        result = run(binary, "check", path)
                        assert result.returncode == 0 and result.stdout == result.stderr == b""
                for sample in range(11):
                    pairs = (("baseline", baseline), ("candidate", compiler))
                    if sample % 2:
                        pairs = tuple(reversed(pairs))
                    for label, binary in pairs:
                        start = time.perf_counter_ns()
                        result = run(binary, "check", path)
                        elapsed = time.perf_counter_ns() - start
                        assert result.returncode == 0 and result.stdout == result.stderr == b""
                        samples[label].append(elapsed)
                medians = {name: statistics.median(values) for name, values in samples.items()}
                row.update({"samples_ns": samples, "median_ns": medians, "candidate_baseline_ratio": medians["candidate"] / medians["baseline"]})
            rows.append(row)
    exponents = {}
    for kind in ("imports", "exports"):
        subset = [row for row in rows if row["kind"] == kind]
        first, last = subset[0], subset[-1]
        exponent = math.log(sum(last["work"].values()) / sum(first["work"].values())) / math.log(last["source_bytes"] / first["source_bytes"])
        assert exponent <= 1.15, (kind, exponent)
        exponents[kind] = exponent
    native_scope = "11 balanced process launch-to-reap samples after two warmups per compiler per input on this host; timings are observations, not portable gates" if baseline else "not collected; requires --baseline in a coordinated quiet host slot"
    return {"rows": rows, "work_exponents": exponents, "work_counter_cap": COUNTER_CAP, "portable_gates": {"work_per_manifest_byte_max": 2, "work_exponent_per_total_source_byte_max": 1.15}, "native_sample_scope": native_scope, "observation_compile_command": command, "observed_generated_sha256": digest(observed_c)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, default=ROOT / "build/toolchain/slimc")
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--generated-c", type=Path)
    parser.add_argument("--receipt", type=Path)
    args = parser.parse_args()
    compiler = args.compiler.resolve()
    assert not args.baseline or args.generated_c, "--baseline requires --generated-c"
    receipt = {"schema": 1, "compiler": str(compiler), "compiler_sha256": digest(compiler), "production_source_sha256": digest(ROOT / "selfhost/project.slim"), "verification_script_sha256": digest(Path(__file__)), "conformance_manifest_sha256": digest(ROOT / "conformance/projects/manifest.tsv"), "runtime_sha256": {name: digest(ROOT / name) for name in ("runtime/slim_rt.c", "runtime/slim_rt.h")}, "fixtures": check_fixtures(compiler), "manifest_audit": audit_manifests()}
    receipt["client_interfaces"] = check_client_interfaces(compiler, receipt["manifest_audit"])
    for name in ("selfhost/slim.project", "library/slim.project"):
        result = run(compiler, "check", ROOT / name)
        assert result.returncode == 0 and result.stdout == result.stderr == b"", (name, result.stdout, result.stderr)
    if args.generated_c:
        baseline = args.baseline.resolve() if args.baseline else None
        generated = args.generated_c.resolve()
        receipt["generated_c_sha256"] = digest(generated)
        if baseline:
            receipt["baseline_sha256"] = digest(baseline)
        with tempfile.TemporaryDirectory(prefix="slim-project-lists-") as temporary:
            receipt["measurements"] = measure(compiler, baseline, generated, Path(temporary))
    if args.receipt:
        args.receipt.parent.mkdir(parents=True, exist_ok=True)
        args.receipt.write_text(json.dumps(receipt, indent=2) + "\n")
    print(f"project lists: {len(receipt['fixtures'])} deterministic check rows; {len(receipt['manifest_audit'])} maintained accepted manifests canonical; selfhost and library checks passed")
    if "measurements" in receipt:
        print("project list work exponents: " + json.dumps(receipt["measurements"]["work_exponents"], sort_keys=True))


if __name__ == "__main__":
    main()
