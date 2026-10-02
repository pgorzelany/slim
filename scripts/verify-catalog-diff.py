#!/usr/bin/env python3
"""Independent finite RFC0163 dictionary oracle; native SLIM remains authority."""
import argparse
from datetime import datetime, timezone
import hashlib
import importlib.util
import itertools
import json
import os
from pathlib import Path
import random
import re
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location('components', ROOT / 'scripts/verify-source-components.py')
components = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(components)
frame, triples = components.frame, components.triples


def decoded(source):
    rows = []
    for (key, key_at), (weight, weight_at), (value, _) in components.records(source, [256, 20, 1048576], 4096):
        if not key:
            raise components.Invalid(11, key_at)
        rows.append((key, components.integer(weight, weight_at), value, key_at))
    rows.sort(key=lambda row: row[0])
    for left, right in zip(rows, rows[1:]):
        if left[0] == right[0]:
            raise components.Invalid(14, right[3])
    return {key: (weight, value) for key, weight, value, _ in rows}


def difference(before, after, change_limit=8192, output_limit=8388608):
    data = {}
    for side, source in [('before', before), ('after', after)]:
        try:
            data[side] = decoded(source)
        except components.Invalid as error:
            return 65, f'error {error.code} in {side} at {error.position}\n'.encode(), b''
    if not 0 <= change_limit <= 8192 or not 0 <= output_limit <= 8388608:
        return 65, b'error 23 in diff at 0\n', b''
    changes = []
    for key in sorted(data['before'].keys() | data['after'].keys()):
        old, new = data['before'].get(key), data['after'].get(key)
        if old != new:
            def nested(record):
                return triples([(key, str(record[0]).encode(), record[1])]) if record is not None else b''
            changes.append((key, nested(old), nested(new)))
    if len(changes) > change_limit:
        return 65, b'error 23 in diff at 0\n', b''
    output = f'changes {len(changes)}\n'.encode() + triples(changes)
    if len(output) > output_limit:
        return 65, b'error 24 in diff at 0\n', b''
    return 0, output, b''


def families():
    for n in [64, 128, 256, 512, 1024, 2048, 4096]:
        for family in ['equal', 'disjoint', 'changed', 'common-prefix', 'asymmetric']:
            prefix = b'p' * 120 if family == 'common-prefix' else b'key'
            old = [(prefix + f'{i:04}'.encode(), str(i if i % 2 == 0 else -i).encode(), bytes([(i * 17) % 256]) * 32) for i in range(n)]
            if family == 'equal':
                new = old
            elif family == 'disjoint':
                new = [(b'other' + f'{i:04}'.encode(), weight, value) for i, (_, weight, value) in enumerate(old)]
            elif family == 'changed':
                new = [(key, str(-i - 1).encode(), bytes([(i * 17 + 1) % 256]) * 32) for i, (key, _, _) in enumerate(old)]
            elif family == 'common-prefix':
                new = [(key, weight, bytes([(i * 17 + 1) % 256]) * 32) for i, (key, weight, _) in enumerate(old)]
            else:
                key, weight, _ = old[n // 2]
                new = [(key, weight, b'changed')]
            yield family, n, triples(old), triples(list(reversed(new)))


def cases(full):
    states = [None, (b'0', b''), (str(components.MINIMUM).encode(), b'\x00,:\xff'), (str(components.MAXIMUM).encode(), b'\xff\x00')]
    one = [b'' if state is None else triples([(b'k', *state)]) for state in states]
    yield from itertools.product(one, repeat=2)
    states = [None, (b'0', b''), (b'1', b'\x00,:\xff')]
    two = [triples([(key, *state) for key, state in zip([b'a', b'ab'], choices) if state is not None]) for choices in itertools.product(states, repeat=2)]
    yield from itertools.product(two, repeat=2)
    items = [(b'z', str(components.MINIMUM).encode(), b'\x00'), (b'a', str(components.MAXIMUM).encode(), b'\xff'), (b'ab', b'0', b'')]
    reordered = [triples(order) for order in itertools.permutations(items)]
    yield from itertools.product(reordered, repeat=2)
    yield triples([(b'\x00', b'0', bytes(range(256))), (b'\xff', b'-1', b'')]), triples([(b'\x00', b'1', bytes(reversed(range(256)))), (b'\x00\xff', b'0', b'')])
    yield triples([(b'x' * 256, b'0', b'')]), triples([(b'x' * 255 + b'y', b'0', b'')])
    for extreme in [components.MINIMUM, components.MAXIMUM]:
        source = triples([(b'a', str(extreme).encode(), b''), (b'b', str(extreme).encode(), b'')])
        yield source, b''
        yield b'', source
    rng = random.Random(163)
    for trial in range(80):
        keys = [b'\x00key' + i.to_bytes(2, 'big') for i in range(rng.randrange(1, 80))]
        sides = []
        for side in range(2):
            records = [(key, str(rng.choice([components.MINIMUM, -1, 0, 1, components.MAXIMUM])).encode(), rng.randbytes(rng.randrange(32))) for key in keys if rng.randrange(3)]
            rng.shuffle(records)
            sides.append(triples(records))
        yield tuple(sides)
    old_cases, _ = components.cases(full)
    malformed = []
    for source, _, _ in old_cases:
        try:
            decoded(source)
        except components.Invalid:
            malformed.append(source)
    valid = triples(items)
    for source in malformed:
        yield source, valid
        yield valid, source
        yield source, b'x'
    for n in ([4095, 4096, 4097] if full else [4097]):
        source = triples([(f'key{i:04}'.encode(), b'1', b'') for i in range(n)])
        yield source, b''
        yield b'', source
    if full:
        for size in [1048575, 1048576, 1048577]:
            length = size - 20
            while len(triples([(b'k', b'0', b'x' * length)])) < size:
                length += 1
            source = triples([(b'k', b'0', b'x' * length)])
            assert len(source) == size
            yield source, b''
            yield b'', source


def identities():
    paths = {Path(__file__).resolve(), ROOT / 'scripts/verify-source-components.py', ROOT / 'build/toolchain/slimc', ROOT / 'runtime/slim_rt.c', ROOT / 'runtime/slim_rt.h', ROOT / 'tests/fixtures/component_work.h'}
    for name in ['catalog', 'catalog-diff-tests']:
        project = ROOT / 'library' / (name + '.project')
        paths.add(project)
        paths.update(project.parent / source for source in re.findall(r'"([^\"]+\.slim)"', project.read_text()))
    return {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in sorted(paths)}


def build(output, sanitize, binary):
    environment = components.clean_environment()
    result = {}
    for name in ['catalog', 'catalog-diff-tests']:
        project = ROOT / 'library' / (name + '.project')
        components.run_command([ROOT / 'slimc', 'fmt', project, '--check'], output, name + '-fmt', environment)
        components.run_command([ROOT / 'slimc', 'check', project], output, name + '-check', environment)
        source = components.run_command([ROOT / 'slimc', project], output, name + '-emit-first', environment)
        assert source == components.run_command([ROOT / 'slimc', project], output, name + '-emit-second', environment)
        generated = output / (name + '.c')
        generated.write_bytes(source)
        target = binary if name == 'catalog' and binary else output / name
        if target == output / name:
            components.run_command([ROOT / 'slimc', 'build', project, '-o', target], output, name + '-build', environment)
        result[name] = [target]
        if sanitize:
            target = output / (name + '-sanitized')
            components.run_command([os.environ.get('CC', 'cc'), '-std=c11', '-O1', '-g', '-Wall', '-Wextra', '-Werror', '-fno-omit-frame-pointer', '-fsanitize=address,undefined', '-I', ROOT / 'runtime', generated, ROOT / 'runtime/slim_rt.c', '-o', target], output, name + '-sanitizer-build', environment)
            result[name].append(target)
    return result


def verify(programs, output, full, faults, work):
    environment = components.clean_environment()
    left, right = output / 'before.ns', output / 'after.ns'
    counts = dict(diff=0, limits=0, comparison=0, invariant_guard=0, allocation_faults=0, work=0, mutation_controls=0)
    digest = hashlib.sha256()
    for ordinal, (before, after) in enumerate(cases(full)):
        left.write_bytes(before); right.write_bytes(after)
        expected = difference(before, after)
        for program in programs['catalog']:
            actual = components.execute(program, [str(left), 'diff', str(right)], environment)
            assert actual == expected, ('diff', ordinal, before[:100], after[:100], actual, expected)
            counts['diff'] += 1
            digest.update(actual[1])
    for program in programs['catalog']:
        assert components.execute(program, [], environment) == (64, b'error 2 at 0\n', b'')
        missing = str(output / 'missing.ns')
        left.write_bytes(b'x'); right.write_bytes(b'x')
        assert components.execute(program, [missing, 'diff', str(right)], environment) == (66, b'error 1 in before at 0\n', b'')
        assert components.execute(program, [str(left), 'diff', missing], environment) == difference(b'x', b'')
        left.write_bytes(b'')
        assert components.execute(program, [str(left), 'diff', missing], environment) == (66, b'error 1 in after at 0\n', b'')
        # The command and the filename have different argument positions.
        named_diff = output / 'diff'; named_diff.write_bytes(triples([(b'k', b'2', b'v')]))
        literal = subprocess.run([str(program), 'diff', 'exact', 'k'], cwd=output, capture_output=True, env=environment, timeout=60)
        assert (literal.returncode, literal.stdout, literal.stderr) == components.catalog(named_diff.read_bytes(), 'exact', b'k')
    fixtures = [(b'', b''), (triples([(b'k', str(components.MINIMUM).encode(), b'\xff\x00')]), triples([(b'k', str(components.MAXIMUM).encode(), b'v'), (b'a', b'0', b'')]))]
    for before, after in fixtures:
        left.write_bytes(before); right.write_bytes(after)
        expected = difference(before, after)
        changes = int(expected[1].split(b'\n')[0].split()[1]); size = len(expected[1])
        limits = [(n, 8388608) for n in [-1, 0, 1, changes - 1, changes, changes + 1, 8192, 8193, components.MINIMUM, components.MAXIMUM]]
        limits += [(8192, n) for n in [-1, 0, size - 1, size, size + 1, 8388608, 8388609, components.MINIMUM, components.MAXIMUM]]
        for change_limit, output_limit in limits:
            expected = difference(before, after, change_limit, output_limit)
            for program in programs['catalog-diff-tests']:
                actual = components.execute(program, [str(left), str(right), str(change_limit), str(output_limit)], environment)
                assert actual == expected, ('limits', change_limit, output_limit, actual, expected)
                counts['limits'] += 1
        for program in programs['catalog-diff-tests']:
            assert components.execute(program, [str(left), str(right), '8192', '8388608', 'corrupt'], environment) == (65, b'error 25 in diff at 0\n', b'')
            counts['invariant_guard'] += 1
    if full:
        before = triples([(f'a{i:04}'.encode(), b'0', b'') for i in range(4096)])
        after = triples([(f'b{i:04}'.encode(), b'0', b'') for i in range(4096)])
        left.write_bytes(before); right.write_bytes(after)
        for limit in [8191, 8192, 8193]:
            expected = difference(before, after, limit)
            for program in programs['catalog-diff-tests']:
                actual = components.execute(program, [str(left), str(right), str(limit), '8388608'], environment)
                assert actual == expected, ('full-change-boundary', limit, actual[:1], expected[:1])
                counts['limits'] += 1
    spans = [(b'', 0, 0), (b'a', 0, 0), (b'a', 0, 1), (b'abc', 1, 3), (bytes(range(256)), 0, 256), (b'\x00\xff', 0, 2), (b'a' * 256, 0, 256)]
    for first, second in itertools.product(spans, repeat=2):
        a, start, end = first; b, rstart, rend = second; left.write_bytes(a); right.write_bytes(b)
        expected = (0, str((a[start:end] > b[rstart:rend]) - (a[start:end] < b[rstart:rend])).encode() + b'\n', b'')
        for program in programs['catalog-diff-tests']:
            assert components.execute(program, ['compare', str(left), str(start), str(end), str(right), str(rstart), str(rend)], environment) == expected
            counts['comparison'] += 1
    left.write_bytes(b'a'); right.write_bytes(b'b')
    for start, end in [(-1, 1), (1, 0), (0, 2), (components.MINIMUM, components.MAXIMUM)]:
        for program in programs['catalog-diff-tests']:
            assert components.execute(program, ['compare', str(left), str(start), str(end), str(right), '0', '1'], environment) == (64, b'', b'')
            counts['comparison'] += 1
    # Fixed mutations are tested against the independent exact report contract.
    before = triples([(b'a', b'0', b'old'), (b'b', b'1', b'gone')])
    after = triples([(b'a', b'2', b'new'), (b'c', b'0', b'added')])
    original = difference(before, after)[1]
    malformed_reports = [original.replace(b'changes 3', b'changes 2'), original[:-1], original.replace(b'old', b'bad'), original.replace(b'new', b'bad'), original.replace(b'1:a,', b'1:z,', 1), original.replace(b'1:2,', b'1:3,')]
    # Decode and re-encode mutations to preserve their framing while changing order/omission.
    header, payload = original.split(b'\n', 1)
    outer = [tuple(field for field, _ in row) for row in components.records(payload, [256, 1048576, 1048576], 4096)]
    malformed_reports += [header + b'\n' + triples(list(reversed(outer))), b'changes 2\n' + triples(outer[:-1])]
    assert len(set(malformed_reports)) == 8 and all(value != original for value in malformed_reports)
    counts['mutation_controls'] = len(malformed_reports)
    (output / 'mutation-controls.json').write_text(json.dumps(dict(oracle_stdout_sha256=hashlib.sha256(original).hexdigest(), rejected_sha256=[hashlib.sha256(value).hexdigest() for value in malformed_reports], contract='exact independent bytes; these eight mutations must differ'), indent=2) + '\n')
    if faults:
        rows = []
        for witness, (before, after) in enumerate([(b'', b''), (before, after), (b'x', b'x')]):
            left.write_bytes(before); right.write_bytes(after); expected = difference(before, after)
            for program in programs['catalog']:
                failing = []
                for ordinal in range(1, 257):
                    actual = components.execute(program, [str(left), 'diff', str(right)], dict(environment, SLIM_ALLOC_FAIL_AT=str(ordinal)))
                    if actual[0] == 71:
                        assert actual[1] == b'' and actual[2] == f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode()
                        failing.append(ordinal); counts['allocation_faults'] += 1
                    else:
                        assert actual == expected, (witness, ordinal, actual, expected)
                        break
                else:
                    raise AssertionError('allocation ordinal cap reached without complete outcome')
                rows.append(dict(witness=witness, binary_sha256=hashlib.sha256(program.read_bytes()).hexdigest(), before_sha256=hashlib.sha256(before).hexdigest(), after_sha256=hashlib.sha256(after).hexdigest(), failing_ordinals=failing, first_complete_ordinal=ordinal))
        (output / 'fault-receipt.json').write_text(json.dumps(dict(cap=256, rows=rows), indent=2) + '\n')
    if work:
        generated = output / 'catalog.c'; observed = output / 'catalog-observed.c'; header = ROOT / 'tests/fixtures/component_work.h'
        observed.write_bytes(('#include ' + json.dumps(str(header)) + '\n').encode() + generated.read_bytes())
        binary = output / 'catalog-observed'
        components.run_command([os.environ.get('CC', 'cc'), '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', ROOT / 'runtime', observed, ROOT / 'runtime/slim_rt.c', '-o', binary], output, 'observer-build', environment)
        rows = []
        for family, n, before, after in families():
            left.write_bytes(before); right.write_bytes(after); expected = difference(before, after)
            actual = components.execute(binary, [str(left), 'diff', str(right)], environment)
            assert actual[:2] == expected[:2]
            match = re.fullmatch(rb'component-work-v2 bytes_get=([0-9]+) vector_access=([0-9]+) vector_push=([0-9]+)\n', actual[2])
            assert match, ('unknown work observation', actual[2]); counters = [int(value) for value in match.groups()]
            assert all(0 < value <= 10000000000 for value in counters)
            old, new = decoded(before), decoded(after); count = len(old) + len(new); key = max(map(len, old.keys() | new.keys())); levels = (count - 1).bit_length()
            bound = 128 * (len(before) + len(after)) + 64 * count * (key + 1) * (levels + 1) + 8192
            assert sum(counters) <= bound, (family, n, counters, bound)
            rows.append(dict(family=family, per_side_size=n, count=count, key_length=key, source_bytes=len(before) + len(after), before_sha256=hashlib.sha256(before).hexdigest(), after_sha256=hashlib.sha256(after).hexdigest(), stdout_sha256=hashlib.sha256(actual[1]).hexdigest(), status=actual[0], counters=counters, work=sum(counters), conservative_bound=bound))
        assert len(rows) == 35
        (output / 'work-receipt.json').write_text(json.dumps(dict(observer_schema=2, counter_cap=10000000000, saturation='unknown; gate fails', scope='complete two-input load/merge/preflight/report; excludes host/runtime internal operations', observer_sha256=hashlib.sha256(header.read_bytes()).hexdigest(), binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(), rows=rows), indent=2) + '\n')
        counts['work'] = len(rows)
    return counts, digest.hexdigest()


def verify_read_order(programs, output):
    rows = []
    if not hasattr(os, 'mkfifo'):
        (output / 'read-order-receipt.json').write_text(json.dumps(dict(classification='unknown', reason='host provides no mkfifo API')) + '\n')
        return 0, 0
    fifo = output / 'after.fifo'; left = output / 'order-before.ns'
    os.mkfifo(fifo)
    environment = components.clean_environment()
    try:
        for program in programs['catalog']:
            left.write_bytes(b'x')
            invalid = subprocess.run([str(program), str(left), 'diff', str(fifo)], capture_output=True, env=environment, timeout=3)
            assert (invalid.returncode, invalid.stdout, invalid.stderr) == difference(b'x', b'')
            missing = subprocess.run([str(program), str(output / 'missing-before.ns'), 'diff', str(fifo)], capture_output=True, env=environment, timeout=3)
            assert (missing.returncode, missing.stdout, missing.stderr) == (66, b'error 1 in before at 0\n', b'')
            # This control establishes that the after FIFO actually blocks when
            # reached; run() kills and waits the one non-spawning native child.
            left.write_bytes(b'')
            try:
                result = subprocess.run([str(program), str(left), 'diff', str(fifo)], capture_output=True, env=environment, timeout=3)
            except subprocess.TimeoutExpired as blocked:
                assert (blocked.stdout or b'') == (blocked.stderr or b'') == b''
            else:
                raise AssertionError(('FIFO control did not block', result.returncode, result.stdout, result.stderr))
            rows.append(dict(binary_sha256=hashlib.sha256(program.read_bytes()).hexdigest(), invalid_before_exit=65, missing_before_exit=66, valid_before_control='writerless FIFO reached; bounded timeout kills/waits child', timeout_seconds=3))
    finally:
        fifo.unlink()
    (output / 'read-order-receipt.json').write_text(json.dumps(dict(classification='exact for named POSIX FIFO trials; bounded control timeout', rows=rows), sort_keys=True, indent=2) + '\n')
    return 2 * len(rows), len(rows)


def verify_diagnostics(output):
    import shutil
    directory = output / 'diagnostics'; directory.mkdir()
    manifest = (ROOT / 'library/catalog-diff-tests.project').read_text().replace('"tests/catalog_diff.slim"', '"probe.slim"')
    for name in re.findall(r'"([^\"]+\.slim)"', manifest):
        if name == 'probe.slim':
            continue
        target = directory / name; target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / 'library' / name, target)
    project = directory / 'slim.project'; project.write_text(manifest)
    cases = [
        ('missing_alloc', 'fn misuse(before: catalog_model.Catalog, after: catalog_model.Catalog) -> catalog_reconcile.Prepared effects[partial]:\n  catalog_reconcile.prepare(before, after, 8192, 8388608)\n', 'E0343', 'catalog_reconcile.prepare'),
        ('missing_partial', 'fn misuse(before: catalog_model.Catalog, after: catalog_model.Catalog) -> catalog_reconcile.Prepared effects[alloc]:\n  catalog_reconcile.prepare(before, after, 8192, 8388608)\n', 'E0343', 'catalog_reconcile.prepare'),
        ('wrong_limit_type', 'fn misuse(before: catalog_model.Catalog, after: catalog_model.Catalog) -> catalog_reconcile.Prepared effects[alloc, partial]:\n  catalog_reconcile.prepare(before, after, 8192, true)\n', 'E0344', 'true'),
        ('compare_missing_partial', 'fn misuse(left: Bytes, right: Bytes) -> I64:\n  std_byte_index.compare(left, 0, bytes.len(left), right, 0, bytes.len(right))\n', 'E0343', 'std_byte_index.compare'),
    ]
    rows = []
    for label, body, code, span_text in cases:
        source = 'module catalog_diff_tests\n\n' + body + '\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
        (directory / 'probe.slim').write_text(source)
        first = subprocess.run([str(ROOT / 'slimc'), '--message-format=json', 'check', str(project)], capture_output=True, timeout=60)
        second = subprocess.run([str(ROOT / 'slimc'), '--message-format=json', 'check', str(project)], capture_output=True, timeout=60)
        assert first.returncode == second.returncode != 0 and first.stdout == second.stdout == b'' and first.stderr == second.stderr
        diagnostics = [json.loads(line) for line in first.stderr.splitlines()]
        assert [row['code'] for row in diagnostics] == [code], (label, diagnostics)
        start = source.index(span_text)
        assert [(row['file'], row['span']['start'], row['span']['end']) for row in diagnostics] == [('catalog_diff_tests', start, start + len(span_text))], (label, diagnostics, start, start + len(span_text))
        (directory / (label + '.json')).write_bytes(first.stderr)
        rows.append(dict(fixture=label, source_sha256=hashlib.sha256(source.encode()).hexdigest(), diagnostics=diagnostics))
    (directory / 'receipt.json').write_text(json.dumps(rows, sort_keys=True, indent=2) + '\n')
    return len(rows)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--catalog-binary', type=Path)
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--full', action='store_true')
    parser.add_argument('--faults', action='store_true')
    parser.add_argument('--work', action='store_true')
    options = parser.parse_args(); output = options.output.resolve(); output.mkdir(parents=True, exist_ok=False)
    start = datetime.now(timezone.utc).isoformat(); begin = time.monotonic_ns(); initial = identities()
    try:
        programs = build(output, options.sanitize, options.catalog_binary.resolve() if options.catalog_binary else None)
        counts, result = verify(programs, output, options.full, options.faults, options.work)
        counts['diagnostics'] = verify_diagnostics(output)
        counts['read_order_checks'], counts['fifo_open_controls'] = verify_read_order(programs, output)
        assert identities() == initial, 'source changed during verification'
        receipt = dict(outcome='passed', classification='bounded finite native oracle matrices; exact named counters', counts=counts, results_sha256=result, source_identities=initial, binary_identities={str(p): hashlib.sha256(p.read_bytes()).hexdigest() for values in programs.values() for p in values}, sanitize=options.sanitize, leak_detection='unknown on this platform; detect_leaks=0', reused_catalog_binary=options.catalog_binary is not None, start_utc=start, end_utc=datetime.now(timezone.utc).isoformat(), operator_elapsed_ns=time.monotonic_ns()-begin)
        (output / 'receipt.json').write_text(json.dumps(receipt, sort_keys=True, indent=2) + '\n')
        print('catalog-diff: ' + json.dumps(counts, sort_keys=True) + '; finite independent dictionary domain bounded')
    except BaseException as error:
        (output / 'failure.json').write_text(json.dumps(dict(outcome='failed', reason=str(error), source_identities=initial, start_utc=start, end_utc=datetime.now(timezone.utc).isoformat(), operator_elapsed_ns=time.monotonic_ns()-begin), sort_keys=True, indent=2) + '\n')
        raise


if __name__ == '__main__':
    main()
