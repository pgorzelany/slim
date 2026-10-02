#!/usr/bin/env python3
"""Independent finite oracle for RFC-0160 ordinary-source applications.

Python is verification only. Every application result comes from emitted native
code through the production SLIM compiler; no Python result accepts SLIM source.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import itertools
import bisect
import json
import math
import os
from pathlib import Path
import random
import re
import subprocess
import shutil
import time

ROOT = Path(__file__).resolve().parents[1]
MINIMUM = -(2 ** 63)
MAXIMUM = 2 ** 63 - 1
SOURCE_LIMIT = 1048576


class Invalid(Exception):
    def __init__(self, code, position):
        self.code, self.position = code, position


def frame(value):
    return str(len(value)).encode() + b':' + value + b','


def triples(records):
    return b''.join(frame(field) for record in records for field in record)


def read_frame(source, start, limit):
    cursor = start
    while cursor < len(source) and 48 <= source[cursor] <= 57:
        if cursor > start and source[start] == 48:
            raise Invalid(104, cursor)
        if int(source[start:cursor + 1]) > limit:
            raise Invalid(105, cursor)
        cursor += 1
    if cursor == len(source):
        raise Invalid(103, cursor)
    if cursor == start or source[cursor] != 58:
        raise Invalid(102, cursor)
    length = int(source[start:cursor])
    payload_start = cursor + 1
    payload_end = payload_start + length
    if payload_end > len(source):
        raise Invalid(106, len(source))
    if payload_end == len(source) or source[payload_end] != 44:
        raise Invalid(107, payload_end)
    return source[payload_start:payload_end], payload_start, payload_end + 1


def records(source, limits, maximum):
    cursor, count = 0, 0
    if len(source) > SOURCE_LIMIT or not 0 <= maximum <= 4096:
        raise Invalid(10, 0)
    while cursor < len(source):
        beginning = cursor
        fields = []
        for limit in limits:
            if cursor == len(source):
                raise Invalid(108, cursor)
            field, position, cursor = read_frame(source, cursor, limit)
            fields.append((field, position))
        if count == maximum:
            raise Invalid(10, beginning)
        count += 1
        yield fields


def integer(value, position):
    negative = value.startswith(b'-')
    start = int(negative)
    if start == len(value):
        raise Invalid(12, position + start)
    for cursor in range(start, len(value)):
        if not 48 <= value[cursor] <= 57:
            raise Invalid(12, position + cursor)
        number = int(value[:cursor + 1])
        if not MINIMUM <= number <= MAXIMUM:
            raise Invalid(13, position + cursor)
    if value[start] == 48 and (negative or len(value) > 1):
        raise Invalid(12, position + (1 if negative else 1))
    return int(value)


def error(code, position):
    return 65, f'error {code} at {position}\n'.encode(), b''


def catalog(source, mode, query):
    try:
        data = []
        for (key, position), (weight, weight_position), (value, _) in records(source, [256, 20, SOURCE_LIMIT], 4096):
            if not key:
                raise Invalid(11, position)
            data.append((key, integer(weight, weight_position), value, position))
        data.sort(key=lambda item: item[0])
        for left, right in zip(data, data[1:]):
            if left[0] == right[0]:
                raise Invalid(14, right[3])
        selected = [item for item in data if (item[0] == query if mode == 'exact' else item[0].startswith(query))]
        total = 0
        for _, weight, _, position in selected:
            total += weight
            if not MINIMUM <= total <= MAXIMUM:
                raise Invalid(16, position)
        output = f'records {len(selected)} total {total}\n'.encode()
        output += triples([(key, str(weight).encode(), value) for key, weight, value, _ in selected])
        return 0, output, b''
    except Invalid as invalid:
        return error(invalid.code, invalid.position)


def name(value, position, code):
    if not value or len(value) > 64:
        raise Invalid(code, position)
    for index, byte in enumerate(value):
        if not (48 <= byte <= 57 or 65 <= byte <= 90 or 97 <= byte <= 122 or byte in b'_.-'):
            raise Invalid(code, position + index)


def workplan(source):
    try:
        tasks = []
        for (key, position), (cost, cost_position), (dependencies, dependencies_position) in records(source, [64, 19, SOURCE_LIMIT], 4096):
            name(key, position, 11)
            amount = integer(cost, cost_position)
            if amount < 0:
                raise Invalid(12, cost_position)
            tasks.append(dict(key=key, position=position, cost=amount, raw=dependencies, deps_position=dependencies_position))
        ordered = sorted(tasks, key=lambda task: task['key'])
        for left, right in zip(ordered, ordered[1:]):
            if left['key'] == right['key']:
                raise Invalid(14, right['position'])
        by_name = {task['key']: task for task in tasks}
        edge_count = 0
        for task in tasks:
            task['dependencies'] = []
            cursor = task['deps_position']
            if task['raw']:
                for dependency in task['raw'].split(b','):
                    name(dependency, cursor, 16)
                    if dependency not in by_name:
                        raise Invalid(17, cursor)
                    if dependency == task['key']:
                        raise Invalid(18, cursor)
                    if dependency in task['dependencies']:
                        raise Invalid(19, cursor)
                    if edge_count == 65536:
                        raise Invalid(20, cursor)
                    task['dependencies'].append(dependency)
                    edge_count += 1
                    cursor += len(dependency) + 1
        completed, rows, parents = {}, [], {}
        while len(completed) < len(tasks):
            available = [task for task in ordered if task['key'] not in completed and all(dependency in completed for dependency in task['dependencies'])]
            if not available:
                blocked = next(task for task in ordered if task['key'] not in completed)
                raise Invalid(21, blocked['position'])
            task = available[0]
            start = max((completed[key] for key in task['dependencies']), default=0)
            finish = start + task['cost']
            if finish > MAXIMUM:
                raise Invalid(22, task['position'])
            completed[task['key']] = finish
            if task['dependencies']:
                parents[task['key']] = min(key for key in task['dependencies'] if completed[key] == start)
            rows.append((task['key'], start, finish))
        span = max(completed.values(), default=0)
        endpoint = min((key for key in completed if completed[key] == span), default=None)
        chain = []
        while endpoint is not None:
            chain.append(endpoint)
            endpoint = parents.get(endpoint)
        output = f'tasks {len(tasks)} edges {edge_count} span {span}\n'.encode()
        output += b''.join(b'task ' + key + f' start {start} finish {finish}\n'.encode() for key, start, finish in rows)
        output += b'critical ' + b','.join(reversed(chain)) + b'\n'
        return 0, output, b''
    except Invalid as invalid:
        return error(invalid.code, invalid.position)


def clean_environment():
    result = os.environ.copy()
    for key in ['SLIM_ALLOC_FAIL_AT', 'SLIM_HOST_ALLOC_FAIL_AT', 'SLIM_NATIVE_ALLOC_FAIL_AT']:
        result.pop(key, None)
    result['ASAN_OPTIONS'] = 'detect_leaks=0:abort_on_error=1'
    result['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    return result


def run_command(arguments, output, label, environment=None):
    before = time.monotonic_ns()
    start = datetime.now(timezone.utc).isoformat()
    run = subprocess.run([str(argument) for argument in arguments], cwd=ROOT, env=environment, capture_output=True, timeout=180)
    elapsed = time.monotonic_ns() - before
    (output / (label + '.stdout')).write_bytes(run.stdout)
    (output / (label + '.stderr')).write_bytes(run.stderr)
    with (output / 'commands.jsonl').open('a') as ledger:
        ledger.write(json.dumps(dict(label=label, command=[str(argument) for argument in arguments], start_utc=start,
                                    end_utc=datetime.now(timezone.utc).isoformat(), elapsed_ns=elapsed, returncode=run.returncode)) + '\n')
    if run.returncode:
        raise AssertionError((label, run.returncode, run.stdout, run.stderr))
    return run.stdout


def build(output, sanitize, binaries_directory):
    environment = clean_environment()
    programs = {}
    for application in ['catalog', 'workplan', 'components']:
        manifest = ROOT / 'library' / (application + '.project')
        if binaries_directory is not None and application in ['catalog', 'workplan'] and not sanitize:
            executable = binaries_directory.resolve() / application
            assert executable.is_file(), ('missing corpus executable', executable)
            programs[application] = [executable]
            continue
        run_command([ROOT / 'slimc', 'fmt', manifest, '--check'], output, application + '-format', environment)
        run_command([ROOT / 'slimc', 'check', manifest], output, application + '-check', environment)
        first = run_command([ROOT / 'slimc', manifest], output, application + '-emit-first', environment)
        second = run_command([ROOT / 'slimc', manifest], output, application + '-emit-second', environment)
        assert first == second, application + ' nondeterministic C'
        emitted = output / (application + '.c')
        emitted.write_bytes(first)
        executable = output / application
        run_command([ROOT / 'slimc', 'build', manifest, '-o', executable], output, application + '-build', environment)
        programs[application] = [executable]
        if sanitize:
            sanitized = output / (application + '-sanitized')
            run_command([os.environ.get('CC', 'cc'), '-std=c11', '-O1', '-g', '-fsanitize=address,undefined',
                         '-fno-sanitize-recover=all', '-fno-omit-frame-pointer', '-I', ROOT / 'runtime', emitted,
                         ROOT / 'runtime/slim_rt.c', '-o', sanitized], output, application + '-sanitize-build', environment)
            programs[application].append(sanitized)
    return programs


def paired_frontier(count):
    keys = [f'j{index:04}'.encode() for index in range(count)]
    split = count // 2
    return [(key, b'1', keys[index + split] if index < split else b'')
            for index, key in enumerate(keys)]


def cases(full):
    catalog_cases, graph_cases = [], []
    rng = random.Random(160)
    for size in [0, 1, 2, 3, 7, 16, 31, 64]:
        for repetition in range(3):
            keys = [f'group{index % 4}.key{index:03}'.encode() for index in range(size)]
            rng.shuffle(keys)
            data = triples([(key, str(rng.randint(-1000, 1000)).encode(), bytes(rng.randrange(256) for _ in range(rng.randrange(20)))) for key in keys])
            for mode, query in [('prefix', b''), ('prefix', b'group0'), ('prefix', b'group1.key0'), ('prefix', b'absent'), ('exact', b'group0.key000'), ('exact', b'group2.key002')]:
                catalog_cases.append((data, mode, query))
    for size in [0, 1, 2, 3, 8, 16, 32, 64]:
        for repetition in range(12):
            keys = [f'job{index:03}'.encode() for index in range(size)]
            tasks = []
            for index, key in enumerate(keys):
                dependencies = [prior for prior in keys[:index] if rng.randrange(4) == 0]
                rng.shuffle(dependencies)
                tasks.append((key, str(rng.randint(0, 1000)).encode(), b','.join(dependencies)))
            rng.shuffle(tasks)
            graph_cases.append(triples(tasks))
    good_catalog = triples([(b'z', b'-1', b'\x00,:\xff'), (b'a', b'5', b'')])
    good_graph = triples([(b'c', b'3', b'a,b'), (b'a', b'1', b''), (b'b', b'2', b'a')])
    for data, target in [(good_catalog, catalog_cases), (good_graph, graph_cases)]:
        for length in range(len(data)):
            truncated = data[:length]
            if target is catalog_cases:
                target.append((truncated, 'prefix', b''))
            else:
                target.append(truncated)
    malformed = [b':,', b'00:,', b'01:a,', b'-1:a,', b'x', b'1', b'1:', b'1:a', b'1:ax', b'1:a,', b'1:a,1:0,', b'999999999999999999999999:', b'1:a,0:,0:,x']
    for value in malformed:
        catalog_cases.append((value, 'prefix', b''))
        graph_cases.append(value)
    for value in [b'', b'-', b'+1', b'-0', b'00', b'01', b'-01', b'1x', b'9223372036854775807', b'9223372036854775808', b'-9223372036854775808', b'-9223372036854775809']:
        catalog_cases.append((triples([(b'a', value, b'value')]), 'prefix', b''))
        graph_cases.append(triples([(b'a', value, b'')]))
    catalog_cases.extend([(triples([(b'', b'1', b'')]), 'prefix', b''), (triples([(b'a', b'1', b''), (b'a', b'2', b'')]), 'prefix', b''),
                          (triples([(b'a', str(MAXIMUM).encode(), b''), (b'b', b'1', b'')]), 'prefix', b''),
                          (triples([(b'a', str(MINIMUM).encode(), b''), (b'b', b'-1', b'')]), 'prefix', b''),
                          (triples([(b'k' * 256, b'0', b'v')]), 'prefix', b''), (triples([(b'k' * 257, b'0', b'v')]), 'prefix', b''),
                          (triples([(b'\x00\xff', b'7', bytes(range(256))), (b'a', b'-7', b'')]), 'prefix', b'')])
    graph_cases.extend([triples([(b'', b'1', b'')]), triples([(b'bad name', b'1', b'')]), triples([(b'a', b'1', b''), (b'a', b'2', b'')]),
                        triples([(b'a', b'1', b'absent')]), triples([(b'a', b'1', b'a')]), triples([(b'a', b'1', b'b,b'), (b'b', b'1', b'')]),
                        triples([(b'a', b'1', b'b,'), (b'b', b'1', b'')]), triples([(b'a', b'1', b',b'), (b'b', b'1', b'')]),
                        triples([(b'a', b'1', b'b'), (b'b', b'1', b'a')]), triples([(b'a', str(MAXIMUM).encode(), b''), (b'b', b'1', b'a')]),
                        triples([(b'a', b'0', b''), (b'b', b'0', b'a')]), triples([(b'k' * 64, b'0', b'')]), triples([(b'k' * 65, b'0', b'')])])
    if full:
        for count in [4095, 4096, 4097]:
            catalog_cases.append((triples([(f'key{index:04}'.encode(), b'1', b'') for index in reversed(range(count))]), 'prefix', b''))
            graph_cases.append(triples([(f't{index:04}'.encode(), b'1', b'') for index in reversed(range(count))]))
        # Precisely 65,535 / 65,536 / 65,537 edges with valid unique names.
        base = [(f't{index:04}'.encode(), b'0', b','.join(f't{prior:04}'.encode() for prior in range(index))) for index in range(362)]
        assert sum(index for index in range(362)) == 65341
        for addition in [194, 195, 196]:
            graph_cases.append(triples(base + [(b't0362', b'0', b','.join(f't{prior:04}'.encode() for prior in range(addition)))]))
        for length in [SOURCE_LIMIT - 1, SOURCE_LIMIT, SOURCE_LIMIT + 1]:
            catalog_cases.append((b'x' * length, 'prefix', b''))
            graph_cases.append(b'x' * length)
    # Releases a lower lexical rank into a nonempty frontier. Orders change
    # source offsets while the independent model preserves named scheduling.
    for count in [1, 2, 3, 7, 16, 63, 64, 255, 256]:
        items = paired_frontier(count)
        for ordered in [items, list(reversed(items)), items[::2] + items[1::2]]:
            graph_cases.append(triples(ordered))
    graph_cases.extend([
        triples([(b'z', b'1', b'y'), (b'y', b'1', b'z'), (b'a', b'1', b'y')]),
        triples([(b'c', b'1', b'd'), (b'd', b'1', b'c'), (b'a', b'1', b'b'), (b'b', b'1', b'a')]),
        triples([(b'z', b'0', b''), (b'a', b'0', b''), (b'x', b'0', b'z,a')]),
        triples([(b'z', str(MAXIMUM).encode(), b''), (b'a', b'1', b'z')]),
    ])
    return catalog_cases, graph_cases


def execute(program, arguments, environment):
    result = subprocess.run([str(program), *arguments], capture_output=True, env=environment, timeout=60)
    return result.returncode, result.stdout, result.stderr


def verify(programs, output, full, faults):
    environment = clean_environment()
    input_file = output / 'case.bin'
    catalog_cases, graph_cases = cases(full)
    counts = {'catalog': 0, 'workplan': 0, 'allocation_faults': 0}
    digest = hashlib.sha256()
    for application, matrix in [('catalog', catalog_cases), ('workplan', graph_cases)]:
        for ordinal, case in enumerate(matrix):
            if application == 'catalog':
                source, mode, query = case
                arguments = [str(input_file), mode, query]
                expected = catalog(source, mode, query)
            else:
                source = case
                arguments = [str(input_file)]
                expected = workplan(source)
            input_file.write_bytes(source)
            for program in programs[application]:
                actual = execute(program, arguments, environment)
                assert actual == expected, (application, ordinal, str(program), source[:200], actual, expected)
                digest.update(application.encode() + b'\0' + source + b'\0' + actual[1])
                counts[application] += 1
        for program in programs[application]:
            assert execute(program, [], environment) == (64, b'error 2 at 0\n', b'')
            missing = [str(output / 'missing.bin')]
            if application == 'catalog':
                missing += ['prefix', '']
            assert execute(program, missing, environment) == (66, b'error 1 at 0\n', b'')
    if faults:
        fault_rows = []
        witnesses = [
            ('catalog', triples([(b'b', b'1', b'payload'), (b'a', b'2', b'')]), ['prefix', '']),
            ('workplan', triples([(b'b', b'2', b'a'), (b'a', b'1', b'')]), []),
            ('workplan', triples(list(reversed(paired_frontier(64)))), []),
            ('workplan', triples([(b'z', b'1', b'y'), (b'y', b'1', b'z'), (b'a', b'1', b'y')]), []),
            ('workplan', triples([(b'z', str(MAXIMUM).encode(), b''), (b'a', b'1', b'z')]), []),
        ]
        for witness, (application, source, tail) in enumerate(witnesses):
            input_file.write_bytes(source)
            for program in programs[application]:
                expected = catalog(source, 'prefix', b'') if application == 'catalog' else workplan(source)
                recovered = False
                failing_ordinals = []
                for ordinal in range(1, 257):
                    actual = execute(program, [str(input_file), *tail], dict(environment, SLIM_ALLOC_FAIL_AT=str(ordinal)))
                    if actual[0] == 71:
                        assert actual[1] == b'' and actual[2] == f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode(), (application, program, ordinal, actual)
                        counts['allocation_faults'] += 1
                        failing_ordinals.append(ordinal)
                    else:
                        assert actual == expected, (application, program, ordinal, actual, expected)
                        recovered = True
                        break
                assert recovered, (application, program, 'fault witness did not reach complete result')
                fault_rows.append(dict(application=application, witness=witness,
                    source_sha256=hashlib.sha256(source).hexdigest(), source_bytes=len(source),
                    executable_sha256=hashlib.sha256(program.read_bytes()).hexdigest(),
                    failing_ordinals=failing_ordinals, first_complete_ordinal=ordinal,
                    complete_returncode=expected[0], complete_stdout_sha256=hashlib.sha256(expected[1]).hexdigest()))
        (output / 'allocation-fault-receipt.json').write_text(json.dumps(dict(
            classification='exact for named finite witness/ordinal trials', ordinal_cap=256,
            failure_contract='exit71, empty stdout, exact exhausted-at-ordinal stderr', rows=fault_rows),
            indent=2, sort_keys=True) + '\n')
    return counts, digest.hexdigest()


def verify_components(programs, output):
    environment = clean_environment()
    input_file = output / 'component.bin'
    counts = {'frame_parse': 0, 'frame_append': 0, 'byte_index': 0}
    matrix = [(bytes(values), 0, len(values), 1048576) for length in range(5)
              for values in itertools.product(b'01:,a\x00', repeat=length)]
    matrix += [(frame(bytes([byte])), 0, 5, 1) for byte in range(256)]
    for value in [b'', b'0:,', b'1:a,', b'2:ab,', b'00:,', b'01:a,', b'1:a,x']:
        for start, end, maximum in [(-1, len(value), 1), (0, -1, 1), (0, len(value) + 1, 1),
                                    (0, len(value), -1), (0, len(value), 1048577),
                                    (0, len(value), 0), (0, len(value), 1),
                                    (MINIMUM, MAXIMUM, MAXIMUM), (len(value), len(value), 1)]:
            matrix.append((value, start, end, maximum))
    matrix += [(b'xx1:a,tail', 2, 6, 1), (b'xx1:a,tail', 2, 5, 1),
               (b'7:abcdefg,', 0, 10, 6), (b'7:abcdefg,', 0, 10, 7)]
    for source, start, end, maximum in matrix:
        input_file.write_bytes(source)
        if start < 0 or end < start or end > len(source) or not 0 <= maximum <= SOURCE_LIMIT:
            expected = f'invalid 1 at {start}\n'.encode()
        elif start == end:
            expected = b'end\n'
        else:
            try:
                payload, position, next_position = read_frame(source[start:end], 0, maximum)
                expected = f'frame {start + position} {start + position + len(payload)} {start + next_position}\n'.encode()
            except Invalid as invalid:
                expected = f'invalid {invalid.code - 100} at {start + invalid.position}\n'.encode()
        for program in programs:
            actual = execute(program, ['parse', str(input_file), str(start), str(end), str(maximum)], environment)
            assert actual == (0, expected, b''), ('frame', source, start, end, maximum, actual, expected)
            counts['frame_parse'] += 1
    append_cases = [(bytes(range(256)), 0, 256, 256), (bytes(range(256)), 1, 255, 254),
                    (b'', 0, 0, 0), (b'abc', 1, 1, 0), (b'abc', -1, 2, 3),
                    (b'abc', 1, 4, 3), (b'abc', 2, 1, 3), (b'abc', 0, 3, 2),
                    (b'abc', 0, 3, -1), (b'abc', 0, 3, SOURCE_LIMIT + 1),
                    (b'abc', MINIMUM, MAXIMUM, MAXIMUM)]
    for source, start, end, maximum in append_cases:
        input_file.write_bytes(source)
        valid = 0 <= start <= end <= len(source) and 0 <= maximum <= SOURCE_LIMIT and end - start <= maximum
        expected = b'prefix:' + (frame(source[start:end]) + b'\n' if valid else b'invalid\n')
        for program in programs:
            actual = execute(program, ['append', str(input_file), str(start), str(end), str(maximum)], environment)
            assert actual == (0, expected, b''), ('append', source, start, end, maximum, actual, expected)
            counts['frame_append'] += 1
    index_sources = [bytes(values) for values in itertools.permutations(b'abcd')]
    index_sources += [b'', b'a', b'aaa', b'zyyx', b'baba', bytes(range(256)), bytes(reversed(range(256)))]
    for source in index_sources:
        input_file.write_bytes(source)
        for query, start, end in [(b'', 0, 0), (b'a', 0, 1), (b'c', 0, 1), (b'ab', 0, 2),
                                   (b'xay', 1, 2), (b'\xff', 0, 1), (b'a', -1, 1), (b'a', 0, 2),
                                   (b'a', 1, 0), (b'a', MINIMUM, MAXIMUM)]:
            indices = sorted(range(len(source)), key=lambda index: source[index])
            repeated = next((right for left, right in zip(indices, indices[1:]) if source[left] == source[right]), None)
            if repeated is not None:
                expected = f'invalid 3 at {repeated}\n'.encode()
            else:
                selected = source.find(query[start:end]) if 0 <= start <= end <= len(query) and end - start == 1 else -1
                keys = [bytes([source[index]]) for index in indices]
                lower = bisect.bisect_left(keys, query)
                upper = lower
                while upper < len(keys) and keys[upper].startswith(query):
                    upper += 1
                expected = b''.join(f'{index},'.encode() for index in indices) + f' find {selected} prefix {lower} {upper}\n'.encode()
            for program in programs:
                actual = execute(program, ['index', str(input_file), query, str(start), str(end)], environment)
                assert actual == (0, expected, b''), ('index', source, query, start, end, actual, expected)
                counts['byte_index'] += 1
    for start, end, ordinal, maximum, expected in [(-1, 1, 99, 1, b'2 0\n'), (-1, 1, 0, 1, b'2 0\n'), (0, 258, 0, 1, b'2 0\n'),
                                                 (1, 0, 0, 1, b'2 0\n'), (0, 1, -1, 1, b'2 0\n'),
                                                 (0, 257, 0, 1, b'2 0\n'), (0, 256, 0, 1, b'ready\n'),
                                                 (0, 1, 0, 0, b'1 1\n'), (0, 1, 0, -1, b'1 1\n'),
                                                 (0, 1, 0, 4097, b'1 1\n'), (0, 0, 0, 1, b'ready\n')]:
        input_file.write_bytes(b'x' * 257)
        for program in programs:
            actual = execute(program, ['build', str(input_file), str(start), str(end), str(ordinal), str(maximum)], environment)
            assert actual == (0, expected, b''), ('invalid-build', start, end, ordinal, maximum, actual, expected)
            counts['byte_index'] += 1
    return counts


def verify_diagnostics(output):
    directory = output / 'diagnostics'
    directory.mkdir(exist_ok=True)
    manifest = (ROOT / 'library/components.project').read_text().replace('(entry component_tests)', '(entry component_bad)')
    manifest = manifest.replace('(module component_tests "tests/components.slim"', '(module component_bad "probe.slim"')
    # Transient test vendoring obeys the same manifest confinement rule. There is
    # one maintained copy; copied bytes stay under the build evidence directory.
    for source in re.findall(r'"([^\"]+\.slim)"', manifest):
        if source == 'probe.slim':
            continue
        target = directory / source
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / 'library' / source, target)
    project = directory / 'slim.project'
    project.write_text(manifest)
    rows = []
    expected_codes = {'missing_alloc': 'E0343', 'missing_partial': 'E0343', 'shared_move': 'E0347',
                      'moved_use': 'E0315', 'exclusive_temporary': 'E0348', 'wrong_type': 'E0344'}
    expected_spans = {'missing_alloc': (78, 98), 'missing_partial': (74, 93), 'shared_move': (146, 153),
                      'moved_use': (218, 225), 'exclusive_temporary': (139, 146), 'wrong_type': (115, 119)}
    for fixture, expected in expected_codes.items():
        source = (ROOT / 'library/tests/component_diagnostics' / (fixture + '.slim')).read_bytes()
        (directory / 'probe.slim').write_bytes(source)
        first = subprocess.run([str(ROOT / 'slimc'), '--message-format=json', 'check', str(project)], capture_output=True, timeout=60)
        second = subprocess.run([str(ROOT / 'slimc'), '--message-format=json', 'check', str(project)], capture_output=True, timeout=60)
        assert first.returncode == second.returncode != 0 and first.stdout == second.stdout == b'' and first.stderr == second.stderr, (fixture, first, second)
        diagnostics = [json.loads(line) for line in first.stderr.splitlines()]
        assert [item['code'] for item in diagnostics] == [expected], (fixture, diagnostics)
        assert [(item['file'], item['span']['start'], item['span']['end']) for item in diagnostics] == [
            ('component_bad', *expected_spans[fixture])], (fixture, diagnostics)
        rows.append(dict(fixture=fixture, source_sha256=hashlib.sha256(source).hexdigest(), diagnostics=diagnostics))
        (directory / (fixture + '.json')).write_bytes(first.stderr)
    (directory / 'receipt.json').write_text(json.dumps(rows, indent=2, sort_keys=True)+'\n')
    return len(rows)


def work_families():
    for count in [64, 128, 256, 512, 1024, 2048, 4096]:
        for prefix, label in [(b'key', 'catalog-reverse-short'), (b'x' * 120, 'catalog-reverse-common-prefix')]:
            keys = [prefix + f'{index:04}'.encode() for index in range(count)]
            yield label, count, 'catalog', triples([(key, b'1', b'') for key in reversed(keys)]), 0, max(map(len, keys))
        for shape in ['independent', 'chain-common-prefix', 'high-fan-in', 'high-fan-out']:
            prefix = b'm' * 48 if shape == 'chain-common-prefix' else b'job'
            keys = [prefix + f'{index:04}'.encode() for index in range(count)]
            items, edge_count = [], 0
            for index, key in enumerate(keys):
                dependencies = []
                if shape == 'chain-common-prefix' and index:
                    dependencies = [keys[index - 1]]
                elif shape == 'high-fan-in' and index == count - 1:
                    dependencies = keys[:-1]
                elif shape == 'high-fan-out' and index:
                    dependencies = keys[:1]
                items.append((key, b'1', b','.join(dependencies)))
                edge_count += len(dependencies)
            yield 'workplan-reverse-' + shape, count, 'workplan', triples(list(reversed(items))), edge_count, max(map(len, keys))
    base = [(f't{index:04}'.encode(), b'0', b','.join(f't{prior:04}'.encode() for prior in range(index))) for index in range(362)]
    for addition in [194, 195, 196]:
        edge_count = 65341 + addition
        source = triples(base + [(b't0362', b'0', b','.join(f't{prior:04}'.encode() for prior in range(addition)))])
        yield 'workplan-density-' + str(edge_count), 363, 'workplan', source, edge_count, 5
    # Preserve the original 45 rows and append these seven predeclared rows.
    for count in [64, 128, 256, 512, 1024, 2048, 4096]:
        yield 'paired-frontier', count, 'workplan', triples(list(reversed(paired_frontier(count)))), count // 2, 5


def verify_work(output):
    environment = clean_environment()
    header = ROOT / 'tests/fixtures/component_work.h'
    programs = {}
    for application in ['catalog', 'workplan']:
        emitted = output / (application + '.c')
        if not emitted.exists():
            emitted.write_bytes(run_command([ROOT / 'slimc', ROOT / 'library' / (application + '.project')],
                                           output, 'work-' + application + '-emit', environment))
        generated = emitted.read_bytes()
        assert b'slim_vec_check_index(' in generated and b'slim_vec_push(' in generated and b'slim_bytes_get(' in generated, application
        instrumented = output / (application + '-observed.c')
        instrumented.write_bytes(('#include ' + json.dumps(str(header)) + '\n').encode() + emitted.read_bytes())
        executable = output / (application + '-observed')
        run_command([os.environ.get('CC', 'cc'), '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', ROOT / 'runtime',
                     instrumented, ROOT / 'runtime/slim_rt.c', '-o', executable], output, application + '-observer-build', environment)
        programs[application] = executable
    capacity_probe = output / 'work-cap'
    run_command([os.environ.get('CC', 'cc'), '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', ROOT / 'runtime',
                 ROOT / 'tests/fixtures/component_work_cap.c', ROOT / 'runtime/slim_rt.c', '-o', capacity_probe], output, 'work-cap-build', environment)
    cap = execute(capacity_probe, [], environment)
    assert cap == (0, b'', b'component-work-v2 bytes_get=0 vector_access=0 vector_push=0\n'), cap
    saturated = execute(capacity_probe, ['saturate'], environment)
    assert saturated == (72, b'', b'component-work-v2 unknown counter-saturated\n'), saturated
    rows, source_file = [], output / 'work-input.ns'
    pattern = rb'component-work-v2 bytes_get=([0-9]+) vector_access=([0-9]+) vector_push=([0-9]+)\n'
    for family, count, application, source, edges, key_length in work_families():
        source_file.write_bytes(source)
        if application == 'catalog':
            actual = execute(programs[application], [str(source_file), 'prefix', ''], environment)
            expected = catalog(source, 'prefix', b'')
        else:
            actual = execute(programs[application], [str(source_file)], environment)
            expected = workplan(source)
        assert actual[:2] == expected[:2] and expected[2] == b'', (family, count, actual, expected)
        observation = re.fullmatch(pattern, actual[2])
        assert observation is not None, (family, count, 'unknown or malformed work observation', actual[2])
        counters = [int(value) for value in observation.groups()]
        assert all(value <= 10000000000 for value in counters) and all(value > 0 for value in counters), counters
        levels = (max(1, count) - 1).bit_length()
        if application == 'catalog':
            bound = 64 * len(source) + 32 * count * (key_length + 1) * (levels + 1) + 8192
        else:
            bound = 32 * count * count + 64 * len(source) + 32 * (count + edges) * (key_length + 1) * (levels + 1) + 8192
        assert sum(counters) <= bound, (family, count, counters, bound)
        tight_bound = None
        derived_bound = None
        if application == 'workplan':
            tight_bound = 64 * len(source) + 32 * (count + edges) * (key_length + 1) * (levels + 1) + 8192
            assert sum(counters) <= tight_bound, (family, count, counters, 'heap whole-pipeline bound', tight_bound)
            derived_bound = (64 * len(source) + count * ((2 * key_length + 11) * levels + 2 * key_length + 52)
                             + edges * ((2 * key_length + 1) * (levels + 2) + 16) + 8192)
            assert sum(counters) <= derived_bound, (family, count, counters, 'heap direct-loop bound', derived_bound)
        row = dict(family=family, classification='exact-for-named-finite-input', count=count, edges=edges, key_length=key_length,
                   source_bytes=len(source), source_sha256=hashlib.sha256(source).hexdigest(), stdout_sha256=hashlib.sha256(actual[1]).hexdigest(),
                   status=actual[0], counters=dict(zip(['bytes_get', 'vector_access', 'vector_push'], counters)),
                   deterministic_work=sum(counters), conservative_bound=bound, heap_conservative_bound=tight_bound,
                   heap_direct_loop_bound=derived_bound,
                   scope='whole generated application load/query-or-schedule/report; excludes runtime/host internal work')
        rows.append(row)
    (output / 'work-receipt.json').write_text(json.dumps(dict(schema=1, observer_schema=2, counter_cap=10000000000, saturation='unknown, gate fails',
                    rows=rows, observer_sha256=hashlib.sha256(header.read_bytes()).hexdigest(),
                    observed_executables={name: hashlib.sha256(path.read_bytes()).hexdigest() for name, path in programs.items()}), indent=2, sort_keys=True)+'\n')
    return len(rows)


def identities():
    paths = set()
    for application in ['catalog', 'workplan', 'components']:
        manifest = ROOT / 'library' / (application + '.project')
        paths.add(manifest)
        paths.update(manifest.parent / name for name in re.findall(r'"([^\"]+\.slim)"', manifest.read_text()))
    paths.update((ROOT / 'library/tests/component_diagnostics').glob('*.slim'))
    paths.update([ROOT / 'tests/fixtures/component_work.h', ROOT / 'tests/fixtures/component_work_cap.c'])
    paths.update([ROOT / 'build/toolchain/slimc', ROOT / 'runtime/slim_rt.c', ROOT / 'runtime/slim_rt.h', Path(__file__).resolve()])
    return {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in sorted(paths)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'build/overnight-components/conformance')
    parser.add_argument('--binaries-directory', type=Path, help='reuse catalog/workplan built immediately before this check by the corpus runner')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--full', action='store_true')
    parser.add_argument('--faults', action='store_true')
    parser.add_argument('--work', action='store_true', help='count deterministic direct byte/vector work over frozen geometric families')
    options = parser.parse_args()
    output = options.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    before = time.monotonic_ns()
    start = datetime.now(timezone.utc).isoformat()
    before_identities = identities()
    try:
        programs = build(output, options.sanitize, options.binaries_directory)
        counts, results = verify(programs, output, options.full, options.faults)
        counts.update(verify_components(programs['components'], output))
        counts['diagnostics'] = verify_diagnostics(output)
        if options.work:
            counts['deterministic_work_rows'] = verify_work(output)
    except BaseException as failure:
        (output / 'failure.json').write_text(json.dumps(dict(outcome='failed', stage='build-or-oracle', reason=str(failure), source_identities=before_identities,
            start_utc=start, end_utc=datetime.now(timezone.utc).isoformat(), operator_elapsed_ns=time.monotonic_ns()-before), indent=2, sort_keys=True)+'\n')
        raise
    assert identities() == before_identities, 'source changed during verification'
    receipt = dict(schema=1, classification='bounded', domain='finite RFC0160 matrices selected by flags', full=options.full,
                   sanitize=options.sanitize, faults=options.faults, work=options.work, reused_corpus_binaries=options.binaries_directory is not None, counts=counts, results_sha256=results, source_identities=before_identities,
                   executable_identities={str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path): hashlib.sha256(path.read_bytes()).hexdigest()
                                          for values in programs.values() for path in values}, start_utc=start,
                   end_utc=datetime.now(timezone.utc).isoformat(), operator_elapsed_ns=time.monotonic_ns() - before)
    (output / 'receipt.json').write_text(json.dumps(receipt, indent=2, sort_keys=True) + '\n')
    print('source-components: ' + json.dumps(counts, sort_keys=True) + '; ' + ('corpus binaries reused; component emission exact' if options.binaries_directory is not None and not options.sanitize else 'deterministic emission exact') + '; finite oracle domain bounded')


if __name__ == '__main__':
    main()
