#!/usr/bin/env python3
"""Authoring-only finite oracles; never imports or runs a reference/candidate.

Acceptance executes frozen bytes, not this Python. Default verifies checked-in
expected bytes; --write is explicitly for prospective authoring before freeze.
Domain JSON is ordinary bounded input/golden data, never an executable program.
"""
import argparse
import bisect
import itertools
import json
from pathlib import Path
import re

HERE = Path(__file__).resolve().parent
MAX = (1 << 63) - 1
MIN = -(1 << 63)


def numbers(values):
    return ''.join('%d\n' % value for value in values).encode()


def quote(value):
    table = {34: b'\\"', 92: b'\\\\', 10: b'\\n', 13: b'\\r', 9: b'\\t'}
    return b'"' + b''.join(table[x] if x in table else bytes([x]) if 32 <= x <= 126
                            else ('\\x%02x' % x).encode() for x in value) + b'"'


def text(data):
    output = bytearray()
    for raw, start, end, budget in data['cases']:
        source = bytes.fromhex(raw)
        valid = 0 <= start <= end <= len(source) <= 1048576 and 0 <= budget <= 1048576
        value = quote(source[start:end]) if valid else b''
        valid = valid and len(value) <= budget
        output += numbers([valid]) + b'pre:' + (value if valid else b'') + b'\n'
    for length in data['large_lengths']:
        valid = length <= 1048576
        output += numbers([valid]) + b'pre:' + (b'""' if valid else b'') + b'\n'
    return bytes(output)


def identity(data):
    output = bytearray()
    for case in data['cases']:
        file, left, right = case[:3], case[3:6], case[6:9]
        a, b, c, d = case[9:]
        valid = (file[0] > 0 and file[1] > 0 and file[2] >= 0 and file == left == right
                 and 0 <= a <= b <= 5 and 0 <= c <= d <= 5)
        if not valid:
            output += b'invalid\n'
        elif max(a, c) >= min(b, d):
            output += b'empty\n'
        else:
            output += b'ready\n' + numbers([max(a, c), min(b, d), 1])
    return bytes(output)


def decimal(data):
    output = bytearray()
    for raw, start, end in data['cases']:
        source = bytes.fromhex(raw)
        if not 0 <= start < end <= len(source):
            output += b'invalid\n' + numbers([start]); continue
        first = start + (source[start] == 45)
        if first == end:
            output += b'invalid\n' + numbers([first]); continue
        if source[first] == 48:
            if first > start:
                output += b'invalid\n' + numbers([first])
            elif first + 1 < end:
                output += b'invalid\n' + numbers([first + 1])
            else:
                output += b'value\n' + numbers([0, end])
            continue
        value = 0
        limit = 1 << 63 if first > start else MAX
        for cursor in range(first, end):
            byte = source[cursor]
            if not 48 <= byte <= 57:
                output += b'invalid\n' + numbers([cursor]); break
            value = value * 10 + byte - 48
            if value > limit:
                output += b'overflow\n' + numbers([cursor]); break
        else:
            output += b'value\n' + numbers([-value if first > start else value, end])
    return bytes(output)


def frame(value):
    if isinstance(value, str):
        value = value.encode()
    return str(len(value)).encode() + b':' + value + b','


def batch(data):
    output = bytearray()
    for raw, spans, budget in data['cases']:
        source = bytes.fromhex(raw)
        valid = (0 <= budget <= 1048576 and len(spans) <= 4096
                 and all(0 <= a <= b <= len(source) for a, b in spans)
                 and sum(b - a for a, b in spans) <= budget)
        value = b''.join(frame(source[a:b]) for a, b in spans) if valid else b''
        output += numbers([valid]) + b'pre:' + value + b'\n'
    return bytes(output)


def index(data):
    ordered = sorted(zip(map(bytes.fromhex, data['keys']), data['ordinals']))
    keys = [key for key, _ in ordered]
    output = bytearray()
    for low, high in itertools.product(map(bytes.fromhex, data['queries']), repeat=2):
        a, b = (bisect.bisect_left(keys, low), bisect.bisect_right(keys, high)) if low <= high else (0, 0)
        output += numbers([a, b]) + numbers([ordinal for _, ordinal in ordered[a:b]]) + b'end\n'
    return bytes(output)


def hex_codec(data):
    output = bytearray()
    for mode, raw, start, end, budget in data['cases']:
        source = bytes.fromhex(raw)
        valid = 0 <= start <= end <= len(source) <= 1048576 and 0 <= budget <= 1048576
        value = b''
        if valid:
            span = source[start:end]
            if mode == 'encode':
                value = span.hex().encode()
            elif len(span) % 2 or any(x not in b'0123456789abcdefABCDEF' for x in span):
                valid = False
            else:
                value = bytes.fromhex(span.decode())
        valid = valid and len(value) <= budget
        output += numbers([valid]) + b'pre:' + (value if valid else b'') + b'\n'
    for length in data['large_lengths']:
        output += (numbers([length <= 1048576]) + b'pre:\n') * 2
    return bytes(output)


def catalog(data):
    output = bytearray()
    for case in data['cases']:
        records = [(bytes.fromhex(key), weight, bytes.fromhex(value)) for key, weight, value in case['records']]
        query = bytes.fromhex(case['query'])
        source = b''; positions = {}
        for key, weight, value in records:
            positions[key] = len(source) + len(str(len(key))) + 1
            source += frame(key) + frame(str(weight)) + frame(value)
        ordered = sorted(records)
        start = sum(key < query for key, _, _ in ordered)
        selected = [row for row in ordered if row[0] == query] if case['exact'] else [row for row in ordered if row[0].startswith(query)]
        count = total = position = 0; valid = True
        for key, weight, _ in selected:
            if not MIN <= total + weight <= MAX:
                valid = False; position = positions[key]; break
            count += 1; total += weight
        output += b'ready\n' + numbers([start, start + len(selected), valid, position, count, total])
        if valid:
            output += ('records %d total %d\n' % (count, total)).encode()
            output += b''.join(frame(key) + frame(str(weight)) + frame(value) for key, weight, value in selected)
    return bytes(output)


def workplan(data):
    output = bytearray()
    for graph in data['graphs']:
        source = b''; positions = {}
        for name, cost, dependencies in graph:
            positions[name] = len(source) + len(str(len(name))) + 1
            source += frame(name) + frame(str(cost)) + frame(','.join(dependencies))
        model = {name: (cost, dependencies) for name, cost, dependencies in graph}
        finished = {}; parents = {}; steps = []; error = None
        while len(finished) < len(model):
            ready = sorted(name for name, (_, dependencies) in model.items()
                           if name not in finished and all(item in finished for item in dependencies))
            if not ready:
                error = (21, positions[min(set(model) - set(finished))]); break
            name = ready[0]; cost, dependencies = model[name]
            start = max((finished[item] for item in dependencies), default=0)
            if start + cost > MAX:
                error = (22, positions[name]); break
            parents[name] = min((item for item in dependencies if finished[item] == start), default=None)
            finished[name] = start + cost; steps.append((name, start, start + cost))
        if error:
            output += ('error %d at %d\n' % error).encode(); continue
        span = max(finished.values(), default=0)
        current = min((name for name, value in finished.items() if value == span), default=None)
        chain = []
        while current is not None:
            chain.append(current); current = parents[current]
        output += ('tasks %d edges %d span %d\n' % (len(graph), sum(len(row[2]) for row in graph), span)).encode()
        output += ''.join('task %s start %d finish %d\n' % step for step in steps).encode()
        output += ('critical ' + ','.join(reversed(chain)) + '\n').encode()
    return bytes(output)


def http_response(source):
    def invalid(code, position):
        return [0, code, position, 0, position, position]
    status_end = source.find(b'\r\n')
    if status_end < 12 or not source.startswith(b'HTTP/1.1 '):
        return invalid(10, 0)
    try:
        status = int(source[9:12])
    except ValueError:
        return invalid(10, 9)
    if not 100 <= status <= 999:
        return invalid(10, 9)
    beginning = cursor = status_end + 2; seen = False; declared = 0
    token = b"!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    while True:
        end = source.find(b'\r\n', cursor, min(len(source), beginning + 8192))
        if end < 0:
            return invalid(13, beginning + 8192) if len(source) - beginning >= 8192 else invalid(11, cursor)
        if end == cursor:
            body = end + 2; break
        colon = source.find(b':', cursor, end)
        if colon < 0:
            return invalid(12, cursor)
        bad = next((i for i in range(cursor, colon) if source[i] not in token), colon if colon == cursor else None)
        if bad is not None:
            return invalid(18, bad)
        name = source[cursor:colon].lower(); first = colon + 1; last = end
        while first < end and source[first] in b' \t': first += 1
        while last > first and source[last - 1] in b' \t': last -= 1
        if name == b'transfer-encoding': return invalid(15, cursor)
        if name == b'content-length':
            if seen: return invalid(16, cursor)
            value = source[first:last]
            if not re.fullmatch(rb'-?[0-9]+', value): return invalid(14, first)
            declared = int(value)
            if not 0 <= declared <= 65536: return invalid(14, first)
            seen = True
        cursor = end + 2
    if seen and len(source) - body != declared: return invalid(17, body)
    return [1, 0, len(source), status, body, len(source)]


def http(data):
    return b''.join(numbers(http_response(bytes.fromhex(raw))) for raw in data['cases'])


def ledger(data):
    output = bytearray()
    for mode, a, b, fc, fd, tc, td, amount, ao, bo in data['cases']:
        accounts = [[0, 1, a, ao, fc, fd], [1, 2, b, bo, tc, td]]; code = 0
        if mode == 'credit':
            if amount <= 0: code = 33
            elif not ao: code = 34
            elif a + amount > MAX or fc == MAX: code = 44
            else: accounts[0][2] += amount; accounts[0][4] += 1
        elif mode == 'debit':
            if amount <= 0: code = 35
            elif not ao: code = 36
            elif a < amount: code = 37
            elif fd == MAX: code = 44
            else: accounts[0][2] -= amount; accounts[0][5] += 1
        elif mode == 'same': code = 39
        elif mode == 'missing': code = 38
        else:
            if amount <= 0: code = 38
            elif not (ao and bo) or a < amount: code = 40
            elif b + amount > MAX or tc == MAX or fd == MAX: code = 44
            else:
                accounts[0][2] -= amount; accounts[0][5] += 1
                accounts[1][2] += amount; accounts[1][4] += 1
        output += numbers([code]) + b''.join(numbers(account) for account in accounts)
    return bytes(output)


def formatting(data):
    # Hand-specified canonical modules are independent finite golden data.
    return b''.join(b'1\n0\n' + data['canonical'][index].encode() + b'1\n0\n'
                    + data['canonical'][index].encode() for index in data['indices'])


def effects(data):
    # Existing builtin/declared ceilings are specified finite truth tables.
    return b''.join(numbers([1, *ceilings]) for _, ceilings in data['classification']) + b''.join(expected.encode() for _, expected in data['checker'])


ORACLES = {'text-budgeted-quote': text, 'identity-span-intersection': identity,
           'decimal-exact': decimal, 'netstring-batch-atomic': batch,
           'byte-index-range': index, 'hex-atomic-codec': hex_codec,
           'catalog-ordinal-sum': catalog, 'workplan-critical-ties': workplan,
           'http-header-window': http, 'ledger-overflow-transaction': ledger,
           'format-call-modes': formatting, 'effects-call-ceilings': effects}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write', action='store_true')
    args = parser.parse_args()
    for name, oracle in ORACLES.items():
        directory = HERE / name
        data = json.loads((directory / 'fixtures/domain.json').read_text())
        expected = oracle(data)
        path = directory / 'fixtures/expected.bin'
        if args.write:
            path.write_bytes(expected)
        elif path.read_bytes() != expected:
            raise SystemExit(name + ': independent expected bytes differ')
        print(name + ': ' + str(len(expected)) + ' exact expected bytes')


if __name__ == '__main__':
    main()
