#!/usr/bin/env python3
"""Observe production namespace work; never parse or accept SLIM independently.

The portable gate checks exact loop work and declared byte bounds. --baseline
adds balanced launcher-to-reap timing observations only in a coordinated host
slot. The known long-owner family accounts repeated prefix output separately;
unknown families must retain compact linear output even on rejected input.
"""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shlex
import signal
import statistics
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
SIZES = (16, 32, 64, 128)
CAP = 1_000_000_000


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def symbol(qualified):
    module, name = qualified.rsplit(".", 1)
    escape = lambda part: part.replace("_", "_0").replace(".", "_1")
    return "slim_fn_" + ("p" + escape(module) + "__" + escape(name)).replace("_", "_95")


HOOKS = {
    "project.initialize_namespace_roles": "role_initialization_steps",
    "project.mark_namespace_roles": "role_node_steps",
    "project.append_namespace_segment": "namespace_byte_steps",
    "syntax.ensure_name_chars": "index_byte_steps",
    "syntax.lookup_name_node_chars": "lookup_byte_steps",
    "syntax.find_name_edge": "edge_steps",
}
COUNTERS = (*HOOKS.values(), "namespace_pairs", "namespace_lookups", "namespace_emitted_bytes", "unknown_names", "unknown_emitted_bytes")


def counter_support():
    return (
        f"static void namespace_count(uint64_t *value) {{ if (*value >= UINT64_C({CAP})) abort(); ++*value; }}\n"
        f"static void namespace_add(uint64_t *value, uint64_t amount) {{ if (*value > UINT64_C({CAP}) || amount > UINT64_C({CAP}) - *value) abort(); *value += amount; }}\n"
    )


def instrument(source):
    definitions = {name: 0 for name in (*HOOKS, "project.derive_namespace", "project.namespace_lookup", "project.append_namespace_pair", "project.append_absent_namespace")}
    headers = {name: 0 for name in HOOKS}
    returns = {name: 0 for name in ("project.derive_namespace", "project.namespace_lookup", "project.append_namespace_pair", "project.append_absent_namespace")}
    names = {symbol(name): name for name in definitions}
    output = ["#include <stdint.h>\n#include <stdio.h>\n#include <stdlib.h>\nstatic int namespace_index_active, namespace_lookup_active;\n"]
    for counter in COUNTERS:
        output.append(f"static uint64_t namespace_{counter};\n")
    output.append(counter_support())
    output.append('static void namespace_report(void) { fprintf(stderr, "SLIM_NAMESPACE_WORK' + ' %llu' * len(COUNTERS) + '\\n", ' + ', '.join('(unsigned long long)namespace_' + name for name in COUNTERS) + '); }\n')
    current = None
    output_formal = None
    mains = 0
    for line in source.splitlines(keepends=True):
        match = re.match(r'^static SLIM_UNUSED_FUNCTION .* (slim_fn_[A-Za-z_0-9]+)\(.*\) \{\n$', line)
        if match:
            current = names.get(match[1])
            if current:
                definitions[current] += 1
        if current in returns and line == 'return slim_result;\n':
            if current in ('project.append_namespace_pair', 'project.append_absent_namespace'):
                counter = 'namespace_emitted_bytes' if current == 'project.append_namespace_pair' else 'unknown_emitted_bytes'
                output.append(f'namespace_add(&namespace_{counter}, (uint64_t){output_formal}->len - namespace_before);\n')
            else:
                active = 'index' if current == 'project.derive_namespace' else 'lookup'
                output.append(f'namespace_{active}_active = 0;\n')
            returns[current] += 1
        output.append(line)
        if match and current == 'project.derive_namespace':
            output.append('if (namespace_index_active) abort(); namespace_index_active = 1;\n')
        if match and current == 'project.namespace_lookup':
            output.append('if (namespace_lookup_active) abort(); namespace_lookup_active = 1; namespace_count(&namespace_namespace_lookups);\n')
        if match and current in ('project.append_namespace_pair', 'project.append_absent_namespace'):
            formals = re.findall(r'SlimVec \* (slim_v_output(?:_n[0-9]+)?)', line)
            assert len(formals) == 1, (current, formals)
            output_formal = formals[0]
            counter = 'namespace_pairs' if current == 'project.append_namespace_pair' else 'unknown_names'
            output.append(f'namespace_count(&namespace_{counter}); uint64_t namespace_before = (uint64_t){output_formal}->len;\n')
        if line == 'slim_recur: ;\n' and current in HOOKS:
            headers[current] += 1
            guard = 'namespace_index_active' if current == 'syntax.ensure_name_chars' else 'namespace_lookup_active' if current == 'syntax.lookup_name_node_chars' else 'namespace_index_active || namespace_lookup_active' if current == 'syntax.find_name_edge' else '1'
            output.append(f'if ({guard}) namespace_count(&namespace_{HOOKS[current]});\n')
        if line == 'int main(int argc, char **argv) {\n':
            mains += 1
            output.append('if (atexit(namespace_report) != 0) abort();\n')
    assert all(count == 1 for count in definitions.values()), definitions
    assert all(count == 1 for count in headers.values()), headers
    assert all(count == 1 for count in returns.values()) and mains == 1, (returns, mains)
    return ''.join(output)


def fixture(directory, family, size):
    directory.mkdir()
    modules = {}
    if family == 'modules':
        names = [f'k{at:05}' for at in range(size)]
        for name in names:
            modules[name] = f'module {name}\n\nstruct Number:\n  value: I64\n\nfn value() -> I64:\n  1\n'
        modules['app'] = 'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n' + ''.join(f'  let r{at:05}: {name}.Number = {name}.Number(value: 1)\n  let v{at:05}: I64 = {name}.value()\n' for at, name in enumerate(names)) + '  0\n'
        owner = 'app'
        expected = {'index_byte_steps': 40 * size + 9, 'lookup_byte_steps': 82 * size, 'namespace_pairs': 5 * size, 'namespace_lookups': 12 * size, 'namespace_byte_steps': 68 * size, 'namespace_emitted_bytes': 73 * size, 'unknown_names': 0, 'unknown_emitted_bytes': 0}
        module_count = size + 1
    else:
        owner = 'long.part_' + 'x' * size
        name = 'value' if family == 'known-owner' else 'missing_name' if family == 'unknown-call-owner' else 'Missing_name'
        prelude = 'fn value() -> I64:\n  1\n\n' if family == 'known-owner' else ''
        body = ''.join(f'  let v{at:05}: ' + (f'{name} = 0\n' if family == 'unknown-type-owner' else f'I64 = {name}()\n') for at in range(size))
        modules[owner] = f'module {owner}\n\n' + prelude + 'fn main(args: Vec[Bytes]) -> I64:\n' + body + '  0\n'
        known = family == 'known-owner'
        pairs = size + 1 if known else 0
        # Exact absence stops at the first missing trie edge: lower-case m
        # shares main's first edge, while upper-case M is absent at the root.
        lookup_steps = len(name) + 1 if known else 2 if family == 'unknown-call-owner' else 1
        expected = {'index_byte_steps': len(owner) + (12 if known else 6), 'lookup_byte_steps': size * lookup_steps, 'namespace_pairs': pairs, 'namespace_lookups': size, 'namespace_byte_steps': pairs * (len(owner) + len(name) + 2) if known else size * (len(name) + 1), 'namespace_emitted_bytes': pairs * (len(owner) + len(name) + 5), 'unknown_names': 0 if known else size, 'unknown_emitted_bytes': 0 if known else size * (len(name) + 2)}
        module_count = 1
    for module, source in modules.items():
        (directory / (module + '.slim')).write_text(source)
    manifest = '(project 1 (entry ' + owner + ')\n'
    for module in sorted(modules):
        imports = ' '.join(names) if family == 'modules' and module == 'app' else ''
        exports = 'Number value' if family == 'modules' and module != 'app' else ''
        manifest += f'  (module {module} "{module}.slim" (imports {imports}) (exports {exports}))\n'
    manifest += ')\n'
    path = directory / 'slim.project'
    path.write_text(manifest)
    status = 1 if family.startswith('unknown') else 0
    return path, expected, module_count, status


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', type=Path, default=ROOT / 'build/toolchain/slimc')
    parser.add_argument('--generated-c', type=Path, default=ROOT / 'build/toolchain/slimc.c')
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--receipt', type=Path)
    parser.add_argument('--work-dir', type=Path)
    args = parser.parse_args()
    directory = args.work_dir or ROOT / 'build/verification' / ('project-namespace-' + str(time.time_ns()))
    directory.mkdir(parents=True, exist_ok=False)
    receipt_path = args.receipt or directory / 'receipt.json'
    receipt_path.parent.mkdir(parents=True, exist_ok=True)
    receipt = {'schema': 1, 'status': 'running', 'compiler_sha256': sha(args.compiler), 'generated_c_sha256': sha(args.generated_c), 'production_source_sha256': sha(ROOT / 'selfhost/project.slim'), 'observer_sha256': sha(Path(__file__)), 'runtime_sha256': {name: sha(ROOT / name) for name in ('runtime/slim_rt.c', 'runtime/slim_rt.h')}, 'counter_cap': CAP, 'commands': [], 'rows': []}
    pinned = [args.compiler, args.generated_c, Path(__file__), ROOT / 'runtime/slim_rt.c', ROOT / 'runtime/slim_rt.h', ROOT / 'selfhost/slim.project']
    pinned.extend(sorted((ROOT / 'selfhost').glob('*.slim')))
    if args.baseline:
        receipt['baseline_sha256'] = sha(args.baseline)
        pinned.append(args.baseline)
    identities = {str(path.resolve()): sha(path) for path in pinned}
    receipt['identities_before'] = identities

    def save():
        receipt_path.write_text(json.dumps(receipt, indent=2) + '\n')

    def run(command, label, timeout=30):
        start = time.perf_counter_ns()
        result = subprocess.run(list(map(str, command)), capture_output=True, timeout=timeout)
        receipt['commands'].append({'label': label, 'argv': list(map(str, command)), 'elapsed_ns': time.perf_counter_ns() - start, 'returncode': result.returncode, 'stdout': result.stdout.decode(errors='replace'), 'stderr': result.stderr.decode(errors='replace')})
        save()
        return result

    try:
        # These controls invoke the identical helpers included in observed.c.
        # Fixed cases cross the real cap without iterating one billion times.
        controls_c = directory / 'counter-controls.c'
        controls_c.write_text('#include <stdint.h>\n#include <stdio.h>\n#include <stdlib.h>\n' + counter_support() + f'''
int main(int argc, char **argv) {{
  if (argc != 2 || argv[1][1] != '\\0') return 64;
  uint64_t value = 0;
  switch (argv[1][0]) {{
    case '0': value = UINT64_C({CAP - 1}); namespace_count(&value); namespace_add(&value, 0); break;
    case '1': value = UINT64_C({CAP}); namespace_count(&value); break;
    case '2': namespace_add(&value, UINT64_C({CAP + 1})); break;
    case '3': value = 1; namespace_add(&value, UINT64_MAX); break;
    case '4': namespace_add(&value, UINT64_C({CAP})); namespace_add(&value, 0); break;
    case '5': value = UINT64_C({CAP - 1}); namespace_add(&value, 2); break;
    case '6': value = UINT64_MAX; namespace_add(&value, 1); break;
    case '7': value = UINT64_C({CAP + 1}); namespace_add(&value, 0); break;
    default: return 64;
  }}
  printf("%llu\\n", (unsigned long long)value);
  return 0;
}}
''')
        controls = directory / 'counter-controls'
        result = run(shlex.split(os.environ.get('CC', 'cc')) + ['-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', controls_c, '-o', controls], 'counter-controls-build')
        assert result.returncode == 0, (result.returncode, result.stderr)
        receipt['counter_controls'] = []
        for mode, name in enumerate(('increment-to-cap', 'increment-crossing', 'add-crossing', 'add-overflow-attempt', 'maximum-admitted-add', 'near-cap-add-crossing', 'corrupt-counter-overflow-attempt', 'corrupt-counter-zero-add')):
            result = run([controls, str(mode)], 'counter-' + name)
            admitted = mode in (0, 4)
            expected_status = 0 if admitted else -signal.SIGABRT
            expected_stdout = (str(CAP) + '\n').encode() if admitted else b''
            assert (result.returncode, result.stdout, result.stderr) == (expected_status, expected_stdout, b''), (name, result)
            receipt['counter_controls'].append({'name': name, 'expected_returncode': expected_status, 'observed_returncode': result.returncode, 'expected_value': CAP if admitted else None})
            save()
        receipt['counter_controls_sha256'] = sha(controls_c)
        observed_c = directory / 'observed.c'
        observed_c.write_text(instrument(args.generated_c.read_text()))
        observed = directory / 'observed'
        command = shlex.split(os.environ.get('CC', 'cc')) + ['-std=c11', '-O2', '-DNDEBUG', '-Wall', '-Wextra', '-Werror', '-I', ROOT / 'runtime', observed_c, ROOT / 'runtime/slim_rt.c', '-o', observed]
        result = run(command, 'observer-build', 120)
        assert result.returncode == 0, (result.returncode, result.stderr)
        receipt.update({'observed_c_sha256': sha(observed_c), 'observed_binary_sha256': sha(observed)})
        for family in ('modules', 'known-owner', 'unknown-call-owner', 'unknown-type-owner'):
            for size in SIZES:
                path, expected, module_count, status = fixture(directory / f'{family}-{size}', family, size)
                sources = {name.name: sha(name) for name in path.parent.iterdir()}
                source_bytes = sum(name.stat().st_size for name in path.parent.iterdir())
                plain = run([args.compiler, 'check', path], f'{family}-{size}-plain')
                work = run([observed, 'check', path], f'{family}-{size}-work')
                assert plain.returncode == work.returncode == status and plain.stdout == work.stdout and plain.stderr == b'', (family, size, plain, work)
                fields = work.stderr.decode().strip().split()
                assert fields[0] == 'SLIM_NAMESPACE_WORK' and len(fields) == len(COUNTERS) + 1, fields
                actual = dict(zip(COUNTERS, map(int, fields[1:])))
                assert all(actual[key] == value for key, value in expected.items()), (family, size, expected, actual)
                assert actual['role_node_steps'] == actual['role_initialization_steps'] - 3 * module_count, actual
                assert actual['role_initialization_steps'] <= source_bytes + module_count, actual
                assert actual['edge_steps'] <= 65 * (actual['index_byte_steps'] + actual['lookup_byte_steps']), actual
                assert actual['index_byte_steps'] + actual['lookup_byte_steps'] <= 8 * source_bytes, actual
                if family.startswith('unknown'):
                    assert actual['namespace_pairs'] == 0 and actual['namespace_byte_steps'] <= source_bytes and actual['unknown_emitted_bytes'] <= 2 * source_bytes, actual
                row = {'family': family, 'size': size, 'source_bytes': source_bytes, 'source_sha256': sources, 'expected_work': expected, 'work': actual, 'strict_returncode': status}
                if args.baseline:
                    samples = {'baseline': [], 'candidate': []}
                    for sample in range(5):
                        sequence = (('baseline', args.baseline), ('candidate', args.compiler))
                        if sample % 2: sequence = tuple(reversed(sequence))
                        for label, compiler in sequence:
                            result = run([compiler, 'check', path], f'{family}-{size}-{label}-{sample}')
                            assert result.returncode == status and result.stderr == b'', (family, size, label, result)
                            samples[label].append(receipt['commands'][-1]['elapsed_ns'])
                    medians = {key: statistics.median(values) for key, values in samples.items()}
                    row.update({'samples_ns': samples, 'median_ns': medians, 'candidate_baseline_ratio': medians['candidate'] / medians['baseline']})
                receipt['rows'].append(row)
                assert sources == {name.name: sha(name) for name in path.parent.iterdir()}, path
                save()
        exponents = {}
        for family in ('modules', 'known-owner', 'unknown-call-owner', 'unknown-type-owner'):
            rows = [row for row in receipt['rows'] if row['family'] == family]
            # Prefix bytes in the known family are accounted separately, not
            # silently included in a raw-source-linear claim.
            metric = lambda row: row['work']['role_node_steps'] + row['work']['index_byte_steps'] + row['work']['lookup_byte_steps']
            exponent = math.log(metric(rows[-1]) / metric(rows[0])) / math.log(rows[-1]['source_bytes'] / rows[0]['source_bytes'])
            assert exponent <= 1.15, (family, exponent)
            exponents[family] = exponent
        receipt['identities_after'] = {path: sha(Path(path)) for path in identities}
        assert receipt['identities_after'] == identities, 'source/tool/runtime identity changed during observation'
        receipt.update({'status': 'passed', 'work_exponents': exponents, 'portable_gates': {'role_initialization_per_source_byte_plus_module': 1, 'trie_work_per_source_byte': 8, 'edge_steps_per_trie_step': 65, 'role_and_trie_source_exponent': 1.15, 'unknown_namespace_loop_steps_per_source_byte': 1, 'unknown_namespace_emitted_bytes_per_source_byte': 2}, 'timing_scope': 'five balanced launcher-to-reap observations; no portable timing gate' if args.baseline else 'not collected; requires coordinated --baseline', 'known_prefix_scope': 'exact namespace byte loop observed; owner bytes per known occurrence remain explicit'})
        save()
        print('project namespace: exact role/trie/prefix work passed for 16 geometric inputs; known owner expansion separate; rejected unknown expansion bounded')
    except BaseException as error:
        receipt.update({'status': 'failed', 'error': str(error), 'error_type': type(error).__name__})
        save()
        raise


if __name__ == '__main__':
    main()
