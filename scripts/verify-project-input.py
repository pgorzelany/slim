#!/usr/bin/env python3
"""Finite independent RFC0165 byte/import oracles; SLIM checking stays native.

Fixtures supply known source and manifest bytes. This tool never parses SLIM or
uses transport success as evidence of source acceptance. Large helper/data
domains are explicitly separate from full capture/check/serialization domains.
"""
import argparse
from dataclasses import dataclass, replace
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import reprlib
import shutil
import subprocess
import sys
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
MIB = 1048576
TAG = b'slim-project-input-1'
CAPS = [4095, 65536, 64, 256, MIB, 4 * MIB, 1000000, MIB, MIB, 8 * MIB]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def sha(path):
    return digest(Path(path).read_bytes())


def module(relative, name):
    spec = importlib.util.spec_from_file_location(name, ROOT / relative)
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


def frame(data):
    return str(len(data)).encode() + b':' + data + b','


def triples(rows):
    return b''.join(frame(value) for row in rows for value in row)


@dataclass(frozen=True)
class Item:
    name: str
    path: str
    source: bytes
    imports: tuple = ()
    exports: tuple = ()


@dataclass(frozen=True)
class Case:
    name: str
    items: tuple
    entry: str = 'app'
    accepted: bool = True
    diagnostics: tuple = ()
    logical: bool = True
    missing: tuple = ()
    manifest_padding: int = 0
    cycle: bool = False
    admission_preempts: bool = False


def manifest(case):
    text = f'(project 1 (entry {case.entry})\n'
    for item in case.items:
        text += f'  (module {item.name} "{item.path}" (imports {" ".join(item.imports)}) (exports {" ".join(item.exports)}))\n'
    return text.encode() + b')\n' + b' ' * case.manifest_padding


def graph(items):
    return triples([(item.name.encode(), str(len(item.source)).encode(),
                     ','.join(item.imports).encode()) for item in items])


def transport(raw_manifest, rows, raw_graph=None, count=None, edges=None):
    if raw_graph is None:
        raw_graph = triples([(name, str(len(source)).encode(), imports)
                             for name, path, source, imports in rows])
    if count is None:
        count = str(len(rows)).encode()
    if edges is None:
        edges = str(sum(0 if not imports else imports.count(b',') + 1
                        for name, path, source, imports in rows)).encode()
    values = [TAG, count, edges, raw_manifest, raw_graph]
    values += [value for name, path, source, imports in rows for value in (name, path, source)]
    chunks, positions, offset = [], [], 0
    for value in values:
        positions.append({'header': offset, 'payload': offset + len(str(len(value))) + 1})
        chunk = frame(value)
        chunks.append(chunk)
        offset += len(chunk)
    return b''.join(chunks), positions


def expected_transport(case):
    rows = [(item.name.encode(), item.path.encode(), item.source,
             ','.join(item.imports).encode()) for item in case.items]
    return transport(manifest(case), rows)[0]


def expected_catalog(raw_manifest, rows):
    records = [(b'@project', str(len(raw_manifest)).encode(),
                b'slim.project\0' + digest(raw_manifest).encode())]
    records += [(name, str(len(source)).encode(), path + b'\0' + digest(source).encode())
                for name, path, source, imports in rows]
    return triples(sorted(records))


def known_catalog_records(raw_manifest, rows):
    result = {b'@project': (len(raw_manifest), b'slim.project\0' + digest(raw_manifest).encode())}
    result.update({name: (len(source), path + b'\0' + digest(source).encode())
                   for name, path, source, imports in rows})
    return result


def query_expected(records, mode, key):
    selected = [(name, weight, value) for name, (weight, value) in sorted(records.items())
                if (name == key if mode == 'exact' else name.startswith(key))]
    return (f'records {len(selected)} total {sum(weight for name, weight, value in selected)}\n'.encode()
            + triples([(name, str(weight).encode(), value) for name, weight, value in selected]))


def diff_expected(before, after):
    changes = []
    for key in sorted(before.keys() | after.keys()):
        old, new = before.get(key), after.get(key)
        if old != new:
            def nested(record):
                return triples([(key, str(record[0]).encode(), record[1])]) if record is not None else b''
            changes.append((key, nested(old), nested(new)))
    return f'changes {len(changes)}\n'.encode() + triples(changes), [row[0] for row in changes]


def compiler_schedule_expected(rows):
    """Finite graph-data oracle over the declared 36/158 fixture, not SLIM."""
    assert len(rows) == 36 and sum(0 if not imports else imports.count(b',') + 1
                                 for name, path, source, imports in rows) == 158
    costs = {name: len(source) for name, path, source, imports in rows}
    dependencies = {name: imports.split(b',') if imports else [] for name, path, source, imports in rows}
    assert len(costs) == 36 and sum(costs.values()) <= 4 * MIB
    assert all(target in costs for targets in dependencies.values() for target in targets)
    positions, cursor = {}, 0
    for name, path, source, imports in rows:
        positions[name] = cursor + len(str(len(name))) + 1
        cursor += len(triples([(name, str(len(source)).encode(), imports)]))
    completed, parents, steps = {}, {}, []
    for _ in range(36):
        available = [name for name in sorted(costs) if name not in completed
                     and all(target in completed for target in dependencies[name])]
        if not available:
            blocked = next(name for name in sorted(costs) if name not in completed)
            return 65, f'error 21 at {positions[blocked]}\n'.encode(), b''
        name = available[0]
        start = max((completed[target] for target in dependencies[name]), default=0)
        completed[name] = start + costs[name]
        if dependencies[name]:
            parents[name] = min(target for target in dependencies[name] if completed[target] == start)
        steps.append(b'task ' + name + f' start {start} finish {completed[name]}\n'.encode())
    span = max(completed.values())
    endpoint, chain = min(name for name in completed if completed[name] == span), []
    while endpoint is not None:
        chain.append(endpoint)
        endpoint = parents.get(endpoint)
    report = f'tasks 36 edges 158 span {span}\n'.encode() + b''.join(steps)
    report += b'critical ' + b','.join(reversed(chain)) + b'\n'
    return 0, report, b''


def padded(data, length):
    assert len(data) <= length
    return data + b' ' * (length - len(data))


def projects():
    app = Item('app', 'src/app.slim', b'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  math.bits.value()\n', ('math.bits', 'util'))
    math = Item('math.bits', 'modules/math/value.slim', b'module math.bits\n\nfn value() -> I64:\n  41\n', exports=('value',))
    util = Item('util', 'modules/util.slim', b'module util\n\nfn tag() -> I64:\n  2\n')
    base = Case('nested-dotted', (app, math, util))
    yield base
    yield replace(base, name='same-size-edit', items=(app, replace(math, source=math.source.replace(b'41', b'42')), util))
    yield replace(base, name='path-move', items=(app, replace(math, path='moved/value.slim'), util))
    yield replace(base, name='import-edit', items=(replace(app, imports=('math.bits',)), math, util))
    yield replace(base, name='export-edit', items=(app, math, replace(util, exports=('tag',))))
    a = Item('a.b_c', 'a.slim', b'module a.b_c\nfn value() -> I64:\n  1\n', exports=('value',))
    b = Item('a_b.c', 'b.slim', b'module a_b.c\nfn value() -> I64:\n  2\n', exports=('value',))
    yield Case('distinct-qualified-names', (a, b, replace(app, source=b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  a.b_c.value() + a_b.c.value()\n', imports=('a.b_c', 'a_b.c'))))
    cycle = [Item(name, name + '.slim', f'module {name}\nfn value() -> I64:\n  1\n'.encode(), (target,), ('value',))
             for name, target in [('a', 'b'), ('b', 'c'), ('c', 'a')]]
    yield Case('accepted-three-module-cycle', (cycle[0], replace(app, imports=('a',), source=b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  a.value()\n'), *cycle[1:]), cycle=True)
    yield Case('rejected-reciprocal-cycle', (cycle[0], replace(app, imports=('a',), source=b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  a.value()\n'), replace(cycle[1], imports=('a',))), accepted=False, diagnostics=('E0413',))
    simple = Item('app', 'app.slim', b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
    for label, source, code in [('type', b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  true\n', 'E0344'),
                                ('effect', b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  io.println("no")\n  0\n', 'E0343'),
                                ('ownership', b'module app\nfn pair(left: @Vec[I64], right: @Vec[I64]) -> Void:\n  void\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()\n  pair(@values, @values)\n  0\n', 'E0349')]:
        yield Case(label, (replace(simple, source=source),), accepted=False, diagnostics=(code,))
    yield replace(base, name='private', items=(app, replace(math, exports=()), util), accepted=False, diagnostics=('E0415',))
    yield replace(base, name='unimported', items=(replace(app, imports=('util',)), math, util), accepted=False, diagnostics=('E0416',))
    yield Case('missing', (simple,), accepted=False, diagnostics=('E0409',), missing=('app.slim',))
    yield Case('parent-path', (replace(simple, path='../outside.slim'),), accepted=False, diagnostics=('E0407',))
    yield Case('module-mismatch', (replace(simple, source=simple.source.replace(b'module app', b'module other')),), accepted=False, diagnostics=('E0410',))
    yield Case('duplicate-module', (simple, replace(simple, path='other.slim')), accepted=False, diagnostics=('E0406', 'E0408'))
    yield Case('duplicate-path', (simple, Item('lib', 'app.slim', b'module lib\nfn value() -> I64:\n  0\n')), accepted=False, diagnostics=('E0408',))
    yield replace(base, name='module-order', items=tuple(reversed(base.items)), accepted=False, diagnostics=('E0406',))
    yield replace(base, name='import-order', items=(replace(app, imports=('util', 'math.bits')), math, util), accepted=False, diagnostics=('E0406',))
    yield replace(base, name='duplicate-import', items=(replace(app, imports=('math.bits', 'math.bits')), math, util), accepted=False, diagnostics=('E0406',))
    yield replace(base, name='duplicate-export', items=(app, replace(math, exports=('value', 'value')), util), accepted=False, diagnostics=('E0406',))
    for size in (63, 64, 65):
        name = 'a' * size
        yield Case(f'name-{size}', (replace(simple, name=name, source=simple.source.replace(b'module app', b'module ' + name.encode())),), entry=name, logical=size <= 64)
    for size in (255, 256, 257):
        yield Case(f'path-{size}', (replace(simple, path='a/' + 'p' * (size - 7) + '.slim'),), logical=size <= 256)
    for size in (MIB - 1, MIB, MIB + 1):
        yield Case(f'source-{size}', (replace(simple, source=padded(simple.source, size)),), logical=size <= MIB)
        unpadded = Case('base', (simple,))
        yield Case(f'manifest-{size}', (simple,), manifest_padding=size - len(manifest(unpadded)), logical=size <= MIB)
    modules = (simple,) + tuple(Item(name, name + '.slim', f'module {name}\nfn value() -> I64:\n  0\n'.encode()) for name in ('b', 'c', 'd'))
    raw_manifest = manifest(Case('base', modules))
    for total in (4 * MIB - 1, 4 * MIB, 4 * MIB + 1):
        sizes = [MIB, MIB, MIB, total - len(raw_manifest) - 3 * MIB]
        yield Case(f'aggregate-{total}', tuple(replace(item, source=padded(item.source, size)) for item, size in zip(modules, sizes)), logical=total <= 4 * MIB)
    # Both readable projects have the same type error. Only the oversized
    # source's explicit extent admission preempts complete source preparation.
    invalid = b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  true\n'
    for size in (MIB, MIB + 1):
        yield Case(f'source-error-precedence-{size}',
                   (replace(simple, source=padded(invalid, size)),),
                   accepted=False, diagnostics=('E0344',), logical=size <= MIB,
                   admission_preempts=size > MIB)


def materialize(case, output):
    directory = output / case.name / 'project'
    directory.mkdir(parents=True)
    (directory / 'slim.project').write_bytes(manifest(case))
    written = set()
    for item in case.items:
        if item.path in case.missing or item.path in written:
            continue
        written.add(item.path)
        path = directory / item.path
        assert path.resolve().is_relative_to(directory.parent.resolve()), 'fixture escapes isolated case folder'
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(item.source)
    return directory / 'slim.project'


def header_crossing(size, maximum):
    amount = 0
    for at, value in enumerate(str(size)):
        amount = 10 * amount + int(value)
        if amount > maximum:
            return at
    raise AssertionError('header does not exceed limit')


def csv_of_length(size):
    # Canonical opaque dependency names; they do not claim checked imports.
    count, final = divmod(size + 1, 65)
    if final == 0:
        return b','.join([b'b' * 64] * count)
    if count == 0:
        return b'b' * size
    return b','.join([b'b' * 64] * count + [b'b' * (final - 1)])


def transport_cases():
    rows = [(b'a', b'p', b'x', b'')]
    base, positions = transport(b'm', rows)
    yield 'ordinary', base, None
    yield 'binary-path-source', transport(b'\x00\xff', [(b'1', b'\x00\xff/path', bytes(range(256)), b'a,a,1')])[0], None
    yield 'zero-rows', transport(b'', [])[0], None
    yield 'cycle-data', transport(b'', [(b'a', b'a', b'', b'b'), (b'b', b'b', b'', b'a')])[0], None
    yield 'empty', b'', ('framing-truncated', 0)
    yield 'bad-header', b'x', ('framing-header', 0)
    yield 'leading-frame-zero', b'00:,', ('leading-zero', 1)
    yield 'short-payload', b'1:', ('framing-truncated', 2)
    yield 'bad-comma', b'1:x;', ('framing-comma', 3)
    changed, at = transport(b'm', rows)
    yield 'wrong-format', frame(b'wrong') + changed[len(frame(TAG)):], ('format', 2)
    changed, at = transport(b'm', rows, count=b'01')
    yield 'noncanonical-count', changed, ('number-canonical', at[1]['payload'])
    changed, at = transport(b'm', [], count=b'4096')
    yield 'module-count-cap', changed, ('number-limit', at[1]['payload'] + 3)
    changed, at = transport(b'm', rows, edges=b'65537')
    yield 'edge-count-cap', changed, ('number-limit', at[2]['payload'] + 4)
    yield 'module-count-eof', base[:positions[5]['header']], ('module-count', positions[5]['header'])
    yield 'path-frame-eof', base[:positions[6]['header']], ('framing-truncated', positions[6]['header'])
    yield 'source-frame-truncated', base[:-2], ('framing-truncated', len(base) - 2)
    yield 'trailing-byte', base + b'0:,', ('trailing-data', len(base))
    for name in (b'', b'@project', b'a\x00'):
        changed, at = transport(b'm', [(name, b'p', b'x', b'')])
        yield 'bad-name-' + name.hex(), changed, ('name', at[5]['payload'])
    for size in (63, 64, 65):
        changed, at = transport(b'm', [(b'a' * size, b'p', b'x', b'')])
        yield f'name-{size}', changed, None if size <= 64 else ('payload-limit', at[5]['header'] + header_crossing(size, 64))
    for size in (255, 256, 257):
        changed, at = transport(b'm', [(b'a', b'p' * size, b'x', b'')])
        yield f'path-{size}', changed, None if size <= 256 else ('payload-limit', at[6]['header'] + header_crossing(size, 256))
    for field in ('manifest', 'source'):
        for size in (MIB - 1, MIB, MIB + 1):
            changed, at = transport(b'm' * size if field == 'manifest' else b'm', [(b'a', b'p', b'x' * size if field == 'source' else b'x', b'')])
            index = 3 if field == 'manifest' else 7
            yield f'{field}-{size}', changed, None if size <= MIB else ('payload-limit', at[index]['header'] + header_crossing(size, MIB))
    for size in (4094, 4095, 4096):
        synthetic = [(f'm{i:04}'.encode(), b'p', b'', b'') for i in range(size)]
        changed, at = transport(b'', synthetic)
        yield f'modules-{size}', changed, None if size <= 4095 else ('number-limit', at[1]['payload'] + 3)
    for edges in (65535, 65536, 65537):
        changed, at = transport(b'', [(b'a', b'p', b'', b','.join([b'a'] * edges))])
        yield f'edges-{edges}', changed, None if edges <= 65536 else ('number-limit', at[2]['payload'] + 4)
    for extra in (-1, 0, 1):
        sources = [b'x' * MIB] * 3 + [b'x' * (MIB - 1 if extra == -1 else MIB)]
        changed, at = transport(b'x' if extra == 1 else b'', [(f'm{i}'.encode(), b'p', source, b'') for i, source in enumerate(sources)])
        yield f'aggregate-{4*MIB+extra}', changed, None if extra <= 0 else ('source-limit', at[-1]['payload'])
    for order in ([b'a', b'a'], [b'b', b'a']):
        changed, at = transport(b'', [(name, b'p', b'', b'') for name in order])
        yield 'module-order-' + b''.join(order).decode(), changed, ('module-order', at[8]['payload'])
    for label, raw_graph, code, offset in [
            ('missing', b'', 'graph-count', 0),
            ('key', triples([(b'z', b'1', b'')]), 'graph-key', 2),
            ('weight', triples([(b'a', b'2', b'')]), 'graph-weight', 6),
            ('weight-zero', triples([(b'a', b'01', b'')]), 'number-canonical', 6),
            ('weight-limit', triples([(b'a', b'1048577', b'')]), 'number-limit', 12),
            ('extra', graph((Item('a', 'p', b'x'), Item('z', 'p', b''))), 'graph-count', 11)]:
        changed, at = transport(b'm', rows, raw_graph)
        yield 'graph-' + label, changed, (code, at[4]['payload'] + offset)
    for imports, bad in [(b',a', 0), (b'a,', 2), (b'a,,b', 2), (b'a,!', 2)]:
        changed, at = transport(b'm', [(b'a', b'p', b'x', imports)])
        yield 'imports-' + imports.hex(), changed, ('import-list', at[4]['payload'] + 10 + bad)
    changed, at = transport(b'm', rows, edges=b'1')
    yield 'wrong-edge-cardinality', changed, ('edge-count', at[2]['payload'])
    for total in (MIB - 1, MIB, MIB + 1):
        # Two four-byte frames plus the comma-list frame make this exact size.
        imports = csv_of_length(total - 17)
        raw_graph = triples([(b'a', b'0', imports)])
        assert len(raw_graph) == total, (len(raw_graph), total)
        changed, at = transport(b'', [(b'a', b'p', b'', imports)], raw_graph)
        yield f'graph-bytes-{total}', changed, None if total <= MIB else ('payload-limit', at[4]['header'] + header_crossing(total, MIB))
    for delta in (-1, 0, 1):
        synthetic = [(f'm{i:04}'.encode(), b'p' * (0 if i == 0 else 179 + delta if i == 1 else 256), b'', b'') for i in range(3103)]
        assert len(expected_catalog(b'', synthetic)) == MIB + delta
        yield f'catalog-bytes-{MIB+delta}', transport(b'', synthetic)[0], None if delta <= 0 else ('catalog-limit', 0)
    # Under other admissions a valid 8MiB transport is unreachable. This tests
    # the raw capture ceiling before framing, not successful serialization.
    for size in (8 * MIB - 1, 8 * MIB, 8 * MIB + 1):
        yield f'capture-bytes-{size}', b'x' * size, ('framing-header', 0) if size <= 8 * MIB else ('transport-limit', 0)


def decoded_frames(data):
    """Independent decoding only of successful, already expected transport data."""
    result, cursor = [], 0
    while cursor < len(data):
        colon = data.index(b':', cursor)
        size = int(data[cursor:colon])
        end = colon + 1 + size
        assert data[end:end + 1] == b','
        result.append(data[colon + 1:end])
        cursor = end + 1
    return result


def check_inventory(adapter, data):
    values = decoded_frames(data)
    count = int(values[1])
    rows = [(values[5 + 3 * i], values[6 + 3 * i], values[7 + 3 * i], b'') for i in range(count)]
    catalog, workplan, metadata = adapter.inventory(data)
    assert catalog == expected_catalog(values[3], rows)
    assert workplan == values[4]
    assert metadata['manifest'] == {'bytes': len(values[3]), 'sha256': digest(values[3])}
    assert metadata['module_count'] == count and metadata['direct_import_edges'] == int(values[2])
    assert metadata['source_bytes'] == len(values[3]) + sum(len(row[2]) for row in rows)
    assert metadata['modules'] == [{'name': name.decode(), 'path_hex': path.hex(), 'bytes': len(source), 'sha256': digest(source)} for name, path, source, imports in rows]
    return catalog, workplan, metadata


def verify_transport(adapter, output):
    rows = []
    for label, data, error in transport_cases():
        if error is None:
            catalog, workplan, metadata = check_inventory(adapter, data)
            observed = {'catalog_sha256': digest(catalog), 'graph_sha256': digest(workplan)}
        else:
            try:
                adapter.inventory(data)
            except adapter.InventoryError as actual:
                assert (actual.code, actual.position) == error, (label, (actual.code, actual.position), error)
                observed = {'code': actual.code, 'position': actual.position}
            else:
                raise AssertionError(('invalid transport accepted', label))
        rows.append({'case': label, 'input_bytes': len(data), 'input_sha256': digest(data), 'observed': observed,
                     'scope': 'transport data only; does not establish SLIM source acceptance'})
    (output / 'transport-receipt.json').write_text(json.dumps({'schema': 1, 'rows': rows}, indent=2) + '\n')
    return rows


def verify_publication(adapter, output):
    folder = output / 'adapter-publication'
    folder.mkdir()
    data = transport(b'm', [(b'a', b'\x00\xff', b'x', b'')])[0]
    capture = folder / 'capture.ns'
    capture.write_bytes(data)
    outputs = [folder / name for name in ('catalog.ns', 'graph.ns', 'receipt.json')]
    expected_catalog_bytes, expected_graph, metadata = check_inventory(adapter, data)
    receipt = adapter.convert(capture, *outputs)
    assert outputs[0].read_bytes() == expected_catalog_bytes and outputs[1].read_bytes() == expected_graph
    assert receipt == json.loads(outputs[2].read_bytes())
    assert receipt['capture'] == {'bytes': len(data), 'sha256': digest(data)}
    assert receipt['adapter']['sha256'] == sha(adapter.__file__)
    assert outputs[2].stat().st_size <= 4 * MIB
    rows = [{'case': 'fresh-publication', 'receipt_sha256': sha(outputs[2])}]

    def refusal(label, source, paths, code):
        before = {path: path.read_bytes() for path in paths if path.is_file()}
        try:
            adapter.convert(source, *paths)
        except adapter.InventoryError as error:
            assert (error.code, error.position) == (code, 0), (label, error)
        else:
            raise AssertionError(('conversion should refuse', label))
        for path in paths:
            if path in before:
                assert path.read_bytes() == before[path]
            elif not path.is_symlink():
                assert not path.exists(), (label, 'preflight created output', path)
        rows.append({'case': label, 'code': code, 'position': 0})

    refusal('existing-output', capture, outputs, 'fresh-output')
    aliases = [capture, folder / 'alias-graph', folder / 'alias-receipt']
    refusal('capture-alias', capture, aliases, 'path-alias')
    refusal('adapter-output-alias', capture, [Path(adapter.__file__), folder / 'adapter-graph', folder / 'adapter-receipt'], 'path-alias')
    aliases = [folder / 'same', folder / 'same', folder / 'duplicate-receipt']
    refusal('duplicate-output', capture, aliases, 'path-alias')
    dangling = folder / 'dangling'
    dangling.symlink_to(folder / 'absent-target')
    refusal('dangling-output', capture, [dangling, folder / 'dangling-graph', folder / 'dangling-receipt'], 'fresh-output')
    linked = folder / 'capture-link'
    linked.symlink_to(capture.resolve())
    refusal('capture-symlink', linked, [folder / 'symlink-catalog', folder / 'symlink-graph', folder / 'symlink-receipt'], 'capture-file')
    directory_link = folder / 'parent-link'
    directory_link.symlink_to(folder.resolve(), target_is_directory=True)
    refusal('resolved-parent-alias', capture, [directory_link / 'capture.ns', folder / 'parent-graph', folder / 'parent-receipt'], 'path-alias')
    invalid = folder / 'invalid.ns'
    invalid.write_bytes(b'garbage')
    refusal('malformed-preflight', invalid, [folder / 'invalid-catalog', folder / 'invalid-graph', folder / 'invalid-receipt'], 'framing-header')
    fifo = folder / 'capture-fifo'
    os.mkfifo(fifo)
    fifo_outputs = [folder / ('fifo-' + name) for name in ('catalog', 'graph', 'receipt')]
    invocation = [sys.executable, str(Path(adapter.__file__).resolve()), '--capture', str(fifo),
                  '--catalog', str(fifo_outputs[0]), '--workplan', str(fifo_outputs[1]), '--receipt', str(fifo_outputs[2])]
    result = subprocess.run(invocation, capture_output=True, timeout=2, check=False)
    assert result.returncode == 1 and result.stdout == b'' and result.stderr == b'project-input-inventory: transport-limit at 0\n'
    assert not any(path.exists() for path in fifo_outputs)
    rows.append({'case': 'fifo-nonblocking-refusal', 'returncode': 1, 'seconds_limit': 2})
    for boundary in (2, 3, 4):
        copied = folder / f'copied-adapter-{boundary}.py'
        copied_bytes = Path(adapter.__file__).read_bytes()
        copied.write_bytes(copied_bytes)
        local = module(copied, f'copied_inventory_{boundary}')
        paths = [folder / f'pin-{boundary}-{name}' for name in ('catalog', 'graph', 'receipt')]
        original, calls = local.adapter_identity, 0
        def replacement_barrier():
            nonlocal calls
            calls += 1
            if calls == boundary:
                copied.write_bytes(copied_bytes + b'\n# copied identity barrier\n')
            return original()
        rejected = False
        try:
            with patch.object(local, 'adapter_identity', side_effect=replacement_barrier):
                local.convert(capture, *paths)
        except local.InventoryError as error:
            assert (error.code, error.position) == ('adapter-changed', 0)
            rejected = True
        finally:
            copied.write_bytes(copied_bytes)
        assert rejected
        assert [path.exists() for path in paths] == ([False, False, False] if boundary == 2 else [True, True, boundary == 4])
        rows.append({'case': f'copied-self-pin-{boundary}', 'code': 'adapter-changed',
                     'published_files': [path.exists() for path in paths],
                     'scope': 'observed source bytes; no interpreter bytecode/ABA or filesystem transaction claim'})
    (folder / 'controls.json').write_text(json.dumps({'schema': 1, 'rows': rows}, indent=2) + '\n')
    return rows


def boundary_expected():
    lines = []
    for maximum in CAPS:
        values = [maximum, 1, 1, 1, 0, 0, 0, maximum, maximum, -1, maximum, -1, -1, -1, -1]
        lines.append(' '.join(map(str, values)) + ' \n')
    lines += ['-1 1 1 2 19 \n', '-1 3 4 12 14 1048585 -1 -1 \n', '76 404 -1 -1 -1 -1 \n',
              '1048575 1048576 -1 -1 -1 \n']
    return ''.join(lines).encode()


def execute(argv, output, label, environment, cap=8 * MIB):
    # Fixed trusted tool paths and argv only; fixtures never select executables.
    runner = module('benchmarks/development/evaluate.py', 'project_input_bounded_process')
    receipt = runner.process(argv, output / label, 60, environment, output_cap=cap)
    (output / (label + '.json')).write_text(json.dumps({key: value for key, value in receipt.items()
         if key not in ('stdout', 'stderr')}, indent=2) + '\n')
    if receipt['status'] not in ('ok', 'compiler-error') or receipt['returncode'] is None:
        raise RuntimeError(('incomplete trusted invocation; cause unknown', label, receipt['status']))
    return receipt['returncode'], receipt['stdout'].encode('latin1'), receipt['stderr'].encode('latin1')


def build(compiler, cc, output, environment):
    boundary_source = output / 'boundary-input'
    boundary_source.mkdir()
    shutil.copyfile(ROOT / 'library/components/project_input_limits.slim', boundary_source / 'limits.slim')
    shutil.copyfile(ROOT / 'library/tests/project_input/boundaries.slim', boundary_source / 'probe.slim')
    boundary_project = boundary_source / 'slim.project'
    boundary_project.write_text('(project 1 (entry project_input_probe)\n  (module project_input_limits "limits.slim" (imports) (exports Resource add admitted catalog_record_size decimal_width frame_size maximum record_size))\n  (module project_input_probe "probe.slim" (imports project_input_limits) (exports)))\n')
    programs = {}
    for name, source in [('producer', ROOT / 'project-input.project'), ('boundary', boundary_project),
                         ('catalog', ROOT / 'library/catalog.project'), ('workplan', ROOT / 'library/workplan.project')]:
        result = execute([compiler, source], output, name + '-emit', environment, cap=16 * MIB)
        assert result[0] == 0 and result[2] == b'', (name, 'production emit', result[0], result[2])
        generated = output / (name + '.c')
        generated.write_bytes(result[1])
        assert not result[1].startswith(b'#define SLIM_PARALLEL 1\n'), 'this verifier builds serial producer/consumer fixtures'
        programs[name] = []
        for variant, flags in [('ordinary', ['-O2', '-DNDEBUG']), ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all'])]:
            target = output / (name + '-' + variant)
            result = execute([cc, '-std=c11', '-Wall', '-Wextra', '-Werror', *flags,
                    '-I', ROOT / 'runtime', generated, ROOT / 'runtime/slim_rt.c', '-o', target], output, name + '-' + variant + '-build', environment)
            assert result == (0, b'', b''), (name, variant, result)
            programs[name].append(target)
    return programs


def diagnostics(data):
    rows = data.splitlines(keepends=True)
    pattern = rb'(E[0-9]{4}|Q[0-9]{4})@([A-Za-z0-9_.-]+)@([0-9]+):([0-9]+)\n'
    parsed = [re.fullmatch(pattern, row) for row in rows]
    assert rows and all(parsed), ('complete canonical diagnostic stream required', data)
    return [{'code': found[1].decode(), 'module': found[2].decode(),
             'start': int(found[3]), 'end': int(found[4])} for found in parsed]


def project_specification(case):
    return {'case': case.name, 'checker_accepts': case.accepted,
            'producer_admitted': case.logical, 'expected_codes': list(case.diagnostics),
            'source_admission_preempts': case.admission_preempts,
            'manifest_sha256': digest(manifest(case)), 'manifest_bytes': len(manifest(case)),
            'modules': [{'name': item.name, 'path': item.path, 'bytes': len(item.source),
                         'sha256': digest(item.source), 'direct_imports': list(item.imports),
                         'exports': list(item.exports)} for item in case.items]}


def verify_native(compiler, programs, adapter, output, environment):
    observations, completed = [], []
    cases = list(projects())
    (output / 'project-specification.json').write_text(json.dumps(
        {'schema': 1, 'scope': 'predeclared finite source bytes and direct imports; no SLIM parser',
         'cases': [project_specification(case) for case in cases]}, indent=2) + '\n')
    for case in cases:
        if case.admission_preempts:
            assert not case.accepted and not case.logical, (case.name, 'explicit source-admission precedence requires an inadmissible source error')
        path = materialize(case, output)
        files = sorted(path.parent.parent.rglob('*.slim')) + [path]
        before = {str(file): sha(file) for file in files}
        checked = execute([compiler, 'check', path], output, case.name + '-check', environment)
        if case.accepted:
            assert checked == (0, b'', b''), (case.name, 'independent normal checking', checked)
            expected = (0, expected_transport(case), b'') if case.logical else (65, b'error 10 at 0\n', b'')
            reported = []
        else:
            assert checked[0] == 1 and checked[2] == b'', (case.name, 'checker rejection', checked)
            reported = diagnostics(checked[1])
            assert tuple(row['code'] for row in reported) == case.diagnostics, (case.name, reported, case.diagnostics)
            # Normal checking remains the source authority in both cases.
            # Only explicitly predeclared source-extent admission can preempt
            # this diagnostic; capture failures still precede that admission.
            expected = (65, b'error 10 at 0\n', b'') if case.admission_preempts else (65, checked[1], b'')
        modes = []
        for mode, program in enumerate(programs['producer']):
            actual = execute([program, path], output, case.name + f'-producer-{mode}', environment)
            assert actual == expected, (case.name, mode, 'producer/checker or byte oracle disagreement',
                                       actual[0], digest(actual[1]), actual[2], expected[0], digest(expected[1]))
            capture = output / case.name / f'capture-{mode}.ns'
            capture.write_bytes(actual[1])
            if case.accepted and case.logical:
                catalog, workplan, metadata = check_inventory(adapter, actual[1])
                assert workplan == graph(case.items)
                converted = adapter.convert(capture, output / case.name / f'catalog-{mode}.ns',
                    output / case.name / f'graph-{mode}.ns', output / case.name / f'inventory-{mode}.json')
                assert converted['capture']['sha256'] == digest(actual[1])
                if case.cycle:
                    consumer = execute([programs['workplan'][mode], output / case.name / f'graph-{mode}.ns'],
                        output, case.name + f'-consumer-{mode}', environment)
                    assert consumer == (65, b'error 21 at 2\n', b''), ('compiler cycle acceptance differs from planner decision', consumer)
                modes.append({'variant': ('ordinary', 'sanitized')[mode], 'capture_sha256': digest(actual[1]),
                              'catalog_sha256': digest(catalog), 'graph_sha256': digest(workplan),
                              'modules': metadata['modules'], 'edges': metadata['direct_import_edges']})
            else:
                modes.append({'variant': ('ordinary', 'sanitized')[mode], 'returncode': actual[0],
                              'diagnostic_sha256': digest(actual[1]), 'successful_prefix_bytes': 0})
            observations.append({'case': case.name, 'variant': mode, 'returncode': actual[0],
                                 'stdout_sha256': digest(actual[1]), 'stderr_sha256': digest(actual[2])})
        assert before == {str(file): sha(file) for file in files}, (case.name, 'fixture bytes changed')
        completed.append({'case': case.name, 'checked_diagnostics': reported, 'modes': modes})
        (output / 'checked-project-receipt.json').write_text(json.dumps(
            {'schema': 1, 'completed': completed, 'observations': observations}, indent=2) + '\n')
    return observations


def verify_edit_consumers(programs, output, environment):
    cases = list(projects())[:5]
    assert [case.name for case in cases] == ['nested-dotted', 'same-size-edit', 'path-move', 'import-edit', 'export-edit']
    records = {}
    for case in cases:
        rows = [(item.name.encode(), item.path.encode(), item.source, ','.join(item.imports).encode())
                for item in case.items]
        records[case.name] = known_catalog_records(manifest(case), rows)
    expected_changed = {'same-size-edit': [b'math.bits'], 'path-move': [b'@project', b'math.bits'],
                        'import-edit': [b'@project'], 'export-edit': [b'@project']}
    baseline = b'tasks 3 edges 2 span 108\ntask math.bits start 0 finish 42\ntask util start 0 finish 34\ntask app start 42 finish 108\ncritical math.bits,app\n'
    edited_imports = b'tasks 3 edges 1 span 108\ntask math.bits start 0 finish 42\ntask app start 42 finish 108\ntask util start 0 finish 34\ncritical math.bits,app\n'
    assert [len(item.source) for item in cases[0].items] == [66, 42, 34]
    planned = []
    for case in cases:
        for mode in range(2):
            for operation, key in [('exact', b'math.bits'), ('exact', b'@project'), ('prefix', b'')]:
                expected = query_expected(records[case.name], operation, key)
                planned.append({'case': case.name, 'variant': mode, 'operation': operation,
                                'key': key.decode(), 'expected_sha256': digest(expected)})
        if case.name != cases[0].name:
            expected, keys = diff_expected(records[cases[0].name], records[case.name])
            assert keys == expected_changed[case.name], ('predeclared changed-key oracle', case.name, keys)
            planned.append({'case': case.name, 'operation': 'diff', 'changed_keys': [key.decode() for key in keys],
                            'expected_sha256': digest(expected)})
    (output / 'consumer-edit-oracle.json').write_text(json.dumps({'schema': 1, 'rows': planned,
        'baseline_workplan_hex': baseline.hex(), 'import_edit_workplan_hex': edited_imports.hex()}, indent=2) + '\n')
    observations = []
    for case in cases:
        for mode in range(2):
            path = output / case.name / f'catalog-{mode}.ns'
            for at, (operation, key) in enumerate([('exact', b'math.bits'), ('exact', b'@project'), ('prefix', b'')]):
                actual = execute([programs['catalog'][mode], path, operation, key.decode()], output,
                    case.name + f'-catalog-{mode}-{at}', environment)
                assert actual == (0, query_expected(records[case.name], operation, key), b''), ('actual catalog query', case.name, mode, operation, key, actual)
                observations.append({'case': case.name, 'variant': mode, 'operation': operation,
                                     'key': key.decode(), 'stdout_sha256': digest(actual[1])})
            if case.name != cases[0].name:
                expected, keys = diff_expected(records[cases[0].name], records[case.name])
                actual = execute([programs['catalog'][mode], output / cases[0].name / f'catalog-{mode}.ns', 'diff', path],
                    output, case.name + f'-diff-{mode}', environment)
                assert actual == (0, expected, b''), ('actual catalog reconciliation', case.name, mode, actual)
                observations.append({'case': case.name, 'variant': mode, 'operation': 'diff',
                                     'changed_keys': [key.decode() for key in keys], 'stdout_sha256': digest(actual[1])})
            actual = execute([programs['workplan'][mode], output / case.name / f'graph-{mode}.ns'],
                output, case.name + f'-dag-{mode}', environment)
            expected = edited_imports if case.name == 'import-edit' else baseline
            assert actual == (0, expected, b''), ('finite declared DAG plan', case.name, mode, actual)
            observations.append({'case': case.name, 'variant': mode, 'operation': 'workplan',
                                 'cost_unit': 'exact original source bytes', 'stdout_sha256': digest(actual[1])})
    (output / 'consumer-edit-receipt.json').write_text(json.dumps({'schema': 1, 'observations': observations}, indent=2) + '\n')
    return observations


def verify_compiler_project(compiler, programs, adapter, output, environment):
    specification = json.loads((ROOT / 'library/tests/project_input/compiler-input.json').read_bytes())
    supplied = specification['modules']
    assert len(supplied) == 36 and sum(len(row['imports']) for row in supplied) == 158
    folder, manifest_path = ROOT / 'selfhost', ROOT / 'selfhost/slim.project'
    paths = [manifest_path] + [folder / row['path'] for row in supplied]
    before = {str(path.relative_to(ROOT)): sha(path) for path in paths}
    raw_manifest = manifest_path.read_bytes()
    rows = [(row['name'].encode(), row['path'].encode(), (folder / row['path']).read_bytes(),
             ','.join(row['imports']).encode()) for row in supplied]
    expected = transport(raw_manifest, rows)[0]
    # Pin the independent bytes/import fixture BEFORE any checker/producer run.
    oracle = {'schema': 1, 'modules': supplied, 'source_identities': before,
              'expected_capture_sha256': digest(expected), 'expected_capture_bytes': len(expected)}
    expected_records = known_catalog_records(raw_manifest, rows)
    planned = compiler_schedule_expected(rows)
    oracle.update(expected_catalog_records=37, expected_total_source_bytes=sum(value[0] for value in expected_records.values()),
                  expected_workplan_returncode=planned[0], expected_workplan_stdout_hex=planned[1].hex())
    (output / 'compiler-project-oracle.json').write_text(json.dumps(oracle, indent=2) + '\n')
    assert execute([compiler, 'check', manifest_path], output, 'compiler-project-check', environment) == (0, b'', b'')
    observations = []
    for mode, program in enumerate(programs['producer']):
        actual = execute([program, manifest_path], output, f'compiler-project-producer-{mode}', environment)
        assert actual == (0, expected, b''), ('complete real compiler byte/import oracle', mode,
                                           actual[0], digest(actual[1]), digest(expected), actual[2])
        catalog, workplan, metadata = check_inventory(adapter, actual[1])
        capture = output / f'compiler-project-{mode}.ns'
        capture.write_bytes(actual[1])
        receipt = adapter.convert(capture, output / f'compiler-catalog-{mode}.ns',
                                  output / f'compiler-graph-{mode}.ns', output / f'compiler-inventory-{mode}.json')
        catalog_path = output / f'compiler-catalog-{mode}.ns'
        for at, (operation, key) in enumerate([('prefix', b''), ('exact', b'project')]):
            queried = execute([programs['catalog'][mode], catalog_path, operation, key.decode()], output,
                              f'compiler-catalog-query-{mode}-{at}', environment)
            assert queried == (0, query_expected(expected_records, operation, key), b''), ('real compiler catalog', mode, queried)
        reconciled = execute([programs['catalog'][mode], catalog_path, 'diff', catalog_path], output,
                             f'compiler-catalog-identical-{mode}', environment)
        assert reconciled == (0, b'changes 0\n', b''), ('identical compiler catalog reconciliation', mode, reconciled)
        scheduled = execute([programs['workplan'][mode], output / f'compiler-graph-{mode}.ns'], output,
                            f'compiler-workplan-{mode}', environment)
        assert scheduled == planned, ('independent 36/158 graph-data plan, source-byte cost units', mode, scheduled, planned)
        observations.append({'variant': ('ordinary', 'sanitized')[mode], 'capture_sha256': digest(actual[1]),
                             'catalog_sha256': digest(catalog), 'graph_sha256': digest(workplan),
                             'modules': metadata['module_count'], 'edges': metadata['direct_import_edges'],
                             'catalog_records': 37, 'catalog_total_source_bytes': oracle['expected_total_source_bytes'],
                             'workplan_returncode': scheduled[0], 'workplan_stdout_sha256': digest(scheduled[1]),
                             'inventory_receipt_sha256': sha(output / f'compiler-inventory-{mode}.json')})
    assert before == {str(path.relative_to(ROOT)): sha(path) for path in paths}, 'held compiler input changed'
    (output / 'compiler-project-receipt.json').write_text(json.dumps(
        {'schema': 1, 'source_identities': before, 'observations': observations}, indent=2) + '\n')
    return observations


def verify_helpers_and_failures(compiler, programs, output, environment):
    observations = []
    for mode, program in enumerate(programs['boundary']):
        actual = execute([program], output, f'actual-helper-boundaries-{mode}', environment)
        assert actual == (0, boundary_expected(), b''), ('actual-used sizing helper domain', mode, actual)
        observations.append({'variant': mode, 'scope': 'actual-used logical sizing/admission helper; not whole-project boundaries',
                             'stdout_sha256': digest(actual[1])})
    for mode, program in enumerate(programs['producer']):
        for at, arguments in enumerate(([], [output / 'absent.project', output / 'extra.project'])):
            actual = execute([program, *arguments], output, f'argument-{mode}-{at}', environment)
            assert actual == (64, b'error 2 at 0\n', b''), ('invocation convention', actual)
        missing = output / 'absent.project'
        checked = execute([compiler, 'check', missing], output, f'missing-manifest-check-{mode}', environment)
        # The compiler content-sniffs an absent path as single source, whereas
        # this application explicitly calls the captured project API. Preserve
        # both existing wire forms; do not normalize either diagnostic.
        assert checked == (1, b'E0409@0:0\n', b''), ('single-source read diagnostic dispatch', checked)
        actual = execute([program, missing], output, f'missing-manifest-producer-{mode}', environment)
        assert actual == (65, b'E0409@-@0:0\n', b''), ('explicit project read diagnostic with tool exit65', actual)
    return observations


def verify_allocation(programs, output, environment):
    ordinals = [1, 2, 3, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192]
    case = Case('allocation-control', (Item('app', 'app.slim', b'module app\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'),))
    path = materialize(case, output)
    expected = (0, expected_transport(case), b'')
    observations = []
    for mode, program in enumerate(programs['producer']):
        assert execute([program, path], output, f'fault-free-{mode}', environment) == expected
        exhausted = 0
        for ordinal in ordinals:
            actual = execute([program, path], output, f'allocation-{mode}-{ordinal}',
                             dict(environment, SLIM_ALLOC_FAIL_AT=str(ordinal)))
            if actual[0] == 71:
                assert actual[1] == b'' and actual[2] == f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode(), (mode, ordinal, actual)
                exhausted += 1
            else:
                assert actual == expected, ('non-exhausted sparse ordinal must retain full exact bytes', mode, ordinal, actual)
            observations.append({'variant': mode, 'ordinal': ordinal, 'returncode': actual[0],
                                 'stdout_bytes': len(actual[1]), 'stderr_sha256': digest(actual[2])})
        assert exhausted > 0, 'positive allocation exhaustion required'
    (output / 'allocation-receipt.json').write_text(json.dumps(
        {'schema': 1, 'ordinals': ordinals, 'scope': 'sparse finite injections; skipped allocation ordinals unknown',
         'observations': observations}, indent=2) + '\n')
    return observations


def verify_hash_work(adapter, output):
    observations = []
    original = adapter.digest
    for count in (16, 32, 64, 128):
        rows = [(f'm{i:04}'.encode(), b'p', b'x' * 64, b'') for i in range(count)]
        data = transport(b'm' * 64, rows)[0]
        sizes = []
        def count_hash(value):
            sizes.append(len(value))
            return original(value)
        with patch.object(adapter, 'digest', side_effect=count_hash):
            check_inventory(adapter, data)
        assert sizes == [64] * (count + 1), ('actual hashing helper byte visits', count, sizes)
        observations.append({'modules': count, 'input_bytes': len(data), 'hash_calls': len(sizes),
                             'hash_bytes': sum(sizes), 'scope': 'actual hashing helper work, not full parser/compiler/native time'})
    (output / 'hash-work-receipt.json').write_text(json.dumps({'schema': 1, 'rows': observations}, indent=2) + '\n')
    return observations


def source_identities():
    paths = {ROOT / 'project-input.project', ROOT / 'library/workplan.project', ROOT / 'library/catalog.project',
             ROOT / 'selfhost/slim.project', ROOT / 'scripts/verify-project-input.py',
             ROOT / 'scripts/project-input-inventory.py', ROOT / 'benchmarks/development/evaluate.py',
             ROOT / 'design/rfcs/0165-checked-project-input-producer.md'}
    for pattern in ('selfhost/*.slim', 'runtime/slim_rt.*', 'library/experimental/*.slim',
                    'library/components/*.slim', 'library/applications/project_input/*.slim',
                    'library/applications/workplan/*.slim', 'library/applications/catalog/*.slim',
                    'library/tests/project_input/*'):
        paths.update(ROOT.glob(pattern))
    return {str(path.relative_to(ROOT)): sha(path) for path in sorted(paths) if path.is_file()}


def failure_summary(error):
    bounded = reprlib.Repr()
    bounded.maxstring, bounded.maxother, bounded.maxtuple, bounded.maxlist = 512, 512, 10, 10
    return (type(error).__name__ + ': ' + bounded.repr(error.args))[:4096]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', type=Path, default=ROOT / 'build/toolchain/slimc')
    parser.add_argument('--cc', default=os.environ.get('CC', 'cc'))
    parser.add_argument('--output', type=Path, default=ROOT / 'build/project-input')
    parser.add_argument('--transport-only', action='store_true', help='Python transport/FS/helper-work controls only; source acceptance unknown')
    options = parser.parse_args()
    output = options.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    before = source_identities()
    receipt = {'schema': 1, 'status': 'running', 'source_identities': before,
               'scope': 'transport only' if options.transport_only else 'finite production checker/producer and transport domains',
               'native_timing': 'operator correctness elapsed only; no comparative performance claim',
               'unknown': ['preread physical bounds', 'RSS and end-to-end CPU bounds', 'live filesystem atomicity/ABA',
                           'skipped allocation ordinals', 'full-project module count4095, edge count65536, flattened canonical parsed node entries1000000 boundaries',
                           'successful complete transport at 8 MiB and receipt at 4 MiB (unreachable with other admissions)',
                           'SDK/transitive native toolchain identities', 'model calls/tokens or development success']}
    try:
        adapter = module('scripts/project-input-inventory.py', 'project_input_inventory')
        receipt['transport_rows'] = len(verify_transport(adapter, output))
        receipt['publication_controls'] = len(verify_publication(adapter, output))
        receipt['hash_work'] = verify_hash_work(adapter, output)
        if not options.transport_only:
            compiler = options.compiler.resolve(strict=True)
            cc = Path(shutil.which(options.cc) or options.cc).resolve(strict=True)
            receipt['compiler'] = {'path': str(compiler), 'sha256': sha(compiler)}
            receipt['cc'] = {'path': str(cc), 'sha256': sha(cc)}
            environment = dict(os.environ, LC_ALL='C', LANG='C', ASAN_OPTIONS='detect_leaks=0:abort_on_error=1',
                               UBSAN_OPTIONS='halt_on_error=1')
            for key in ('SLIM_ALLOC_FAIL_AT', 'SLIM_TASK_FAIL_AT', 'SLIM_TASK_JOIN_FAIL_AT', 'SLIM_TASK_DISABLE'):
                environment.pop(key, None)
            programs = build(str(compiler), str(cc), output, environment)
            receipt['checked_project_observations'] = len(verify_native(str(compiler), programs, adapter, output, environment))
            receipt['edit_consumer_observations'] = len(verify_edit_consumers(programs, output, environment))
            receipt['compiler_project'] = verify_compiler_project(str(compiler), programs, adapter, output, environment)
            receipt['helpers'] = verify_helpers_and_failures(str(compiler), programs, output, environment)
            receipt['allocation_observations'] = len(verify_allocation(programs, output, environment))
            assert receipt['compiler']['sha256'] == sha(compiler) and receipt['cc']['sha256'] == sha(cc), 'tool bytes changed'
        assert before == source_identities(), 'held source bytes changed during verification'
        receipt['status'] = 'pass'
    except Exception as error:
        receipt.update(status='failed', failure=failure_summary(error), source_after=source_identities())
    finally:
        (output / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
    if receipt['status'] != 'pass':
        print('project-input: ' + receipt['failure'], file=sys.stderr)
        return 1
    print(f"project-input {receipt['scope']}: PASS {receipt['transport_rows']} transport rows; {receipt['publication_controls']} publication controls")
    return 0


if __name__ == '__main__':
    sys.exit(main())
