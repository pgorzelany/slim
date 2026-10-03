#!/usr/bin/env python3
"""Hash RFC-0165 serialized source bytes into bounded catalog/workplan data.

This fixed measurement adapter parses transport data only. A matching successful
trusted producer invocation, retained separately, is required for source
acceptance; a capture tag or these hashes cannot establish that authority.
The supplied capture's final path cannot be a symlink. Parent paths are resolved;
this is an ordinary-file policy, not a filesystem containment guarantee.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import stat

FORMAT = b'slim-project-input-1'
MAX_MODULES = 4095
MAX_EDGES = 65536
MAX_FILE = 1048576
MAX_SOURCES = 4194304
MAX_TRANSPORT = 8388608
MAX_RECEIPT = 4194304
MAX_ADAPTER = 4194304
NAME_BYTES = frozenset(b'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-')


class InventoryError(ValueError):
    def __init__(self, code, position=0):
        self.code, self.position = code, position
        super().__init__(f'{code} at {position}')


def require(value, code, position=0):
    if not value:
        raise InventoryError(code, position)


def digest(value):
    return hashlib.sha256(value).hexdigest()


def frame(value):
    return str(len(value)).encode('ascii') + b':' + value + b','


class Frames:
    def __init__(self, data, origin=0):
        self.data, self.origin, self.cursor = data, origin, 0

    def read(self, maximum=MAX_FILE):
        data, cursor, start = self.data, self.cursor, self.cursor
        length, digits = 0, 0
        while cursor < len(data) and 48 <= data[cursor] <= 57:
            require(not (digits and data[start] == 48), 'leading-zero', self.origin + cursor)
            digit = data[cursor] - 48
            require(digit <= maximum and length <= (maximum - digit) // 10,
                    'payload-limit', self.origin + cursor)
            length = length * 10 + digit
            cursor += 1
            digits += 1
        require(cursor < len(data), 'framing-truncated', self.origin + cursor)
        require(digits and data[cursor] == 58, 'framing-header', self.origin + cursor)
        beginning, end = cursor + 1, cursor + 1 + length
        require(end <= len(data), 'framing-truncated', self.origin + len(data))
        require(end < len(data) and data[end] == 44, 'framing-comma', self.origin + end)
        self.cursor = end + 1
        return data[beginning:end], self.origin + beginning


def number(value, maximum, position):
    require(bool(value) and (len(value) == 1 or value[0] != 48), 'number-canonical', position)
    result = 0
    for offset, byte in enumerate(value):
        require(48 <= byte <= 57, 'number-canonical', position + offset)
        digit = byte - 48
        require(digit <= maximum and result <= (maximum - digit) // 10,
                'number-limit', position + offset)
        result = result * 10 + digit
    return result


def valid_name(value):
    return 0 < len(value) <= 64 and all(byte in NAME_BYTES for byte in value)


def import_count(value, position):
    if not value:
        return 0
    count, start = 0, 0
    while start <= len(value):
        end = value.find(b',', start)
        if end < 0:
            end = len(value)
        require(valid_name(value[start:end]), 'import-list', position + start)
        count += 1
        require(count <= MAX_EDGES, 'edge-count', position + start)
        if end == len(value):
            return count
        start = end + 1
    raise InventoryError('import-list', position + start)


def validate_graph(graph, origin, modules, edges, edge_position):
    rows, counted = Frames(graph, origin), 0
    for module in modules:
        require(rows.cursor < len(graph), 'graph-count', origin + rows.cursor)
        key, key_position = rows.read(64)
        require(key == module['name'].encode('ascii'), 'graph-key', key_position)
        weight, weight_position = rows.read(20)
        measured = number(weight, MAX_FILE, weight_position)
        require(measured == module['bytes'], 'graph-weight', weight_position)
        imports, imports_position = rows.read()
        counted += import_count(imports, imports_position)
        require(counted <= MAX_EDGES, 'edge-count', imports_position)
    require(rows.cursor == len(graph), 'graph-count', origin + rows.cursor)
    require(counted == edges, 'edge-count', edge_position)


def inventory(data):
    """Return catalog bytes, unchanged graph bytes, and raw-byte measurements.

    No manifest or SLIM source syntax is interpreted. Graph checks concern only
    transport association, framing, names, byte weights and edge cardinality;
    dependency resolution, cycles and scheduling remain consumer decisions.
    """
    require(type(data) is bytes and len(data) <= MAX_TRANSPORT, 'transport-limit')
    rows = Frames(data)
    tag, tag_position = rows.read(64)
    require(tag == FORMAT, 'format', tag_position)
    count_bytes, count_position = rows.read(20)
    count = number(count_bytes, MAX_MODULES, count_position)
    edge_bytes, edge_position = rows.read(20)
    edges = number(edge_bytes, MAX_EDGES, edge_position)
    manifest, _ = rows.read()
    graph, graph_position = rows.read()
    modules, catalog_rows = [], []
    total, previous = len(manifest), None
    manifest_hash = digest(manifest)
    project_row = (b'@project', len(manifest), b'slim.project\0' + manifest_hash.encode('ascii'))
    for _ in range(count):
        require(rows.cursor < len(data), 'module-count', rows.cursor)
        key, key_position = rows.read(64)
        require(valid_name(key) and key != b'@project', 'name', key_position)
        require(previous is None or previous < key, 'module-order', key_position)
        path, _ = rows.read(256)
        source, source_position = rows.read()
        total += len(source)
        require(total <= MAX_SOURCES, 'source-limit', source_position)
        source_hash = digest(source)
        modules.append({'name': key.decode('ascii'), 'path_hex': path.hex(),
                        'bytes': len(source), 'sha256': source_hash})
        catalog_rows.append((key, len(source), path + b'\0' + source_hash.encode('ascii')))
        previous = key
    require(rows.cursor == len(data), 'trailing-data', rows.cursor)
    validate_graph(graph, graph_position, modules, edges, edge_position)
    # Module rows are already strict byte ordered; insert the one reserved row
    # in a linear pass instead of sorting a second interpretation of the data.
    catalog, inserted = bytearray(), False
    for row in catalog_rows + [None]:
        if not inserted and (row is None or project_row[0] < row[0]):
            for field in (project_row[0], str(project_row[1]).encode('ascii'), project_row[2]):
                catalog.extend(frame(field))
            inserted = True
        if row is not None:
            for field in (row[0], str(row[1]).encode('ascii'), row[2]):
                catalog.extend(frame(field))
        require(len(catalog) <= MAX_FILE, 'catalog-limit')
    metadata = {'schema': 1, 'format': FORMAT.decode('ascii'), 'module_count': count,
                'direct_import_edges': edges, 'source_bytes': total,
                'manifest': {'bytes': len(manifest), 'sha256': manifest_hash}, 'modules': modules}
    return bytes(catalog), graph, metadata


def read_once(path, maximum, code):
    path = Path(path)
    require(not path.is_symlink(), code)
    descriptor = os.open(path, os.O_RDONLY | getattr(os, 'O_NOFOLLOW', 0) | os.O_NONBLOCK)
    with os.fdopen(descriptor, 'rb') as stream:
        require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode), code)
        value = stream.read(maximum + 1)
    require(len(value) <= maximum, code)
    return value


def adapter_identity():
    value = read_once(__file__, MAX_ADAPTER, 'adapter-limit')
    return {'sha256': digest(value), 'bytes': len(value)}


def fresh_paths(capture, outputs):
    require(not Path(capture).is_symlink(), 'capture-file')
    capture = Path(capture).resolve(strict=True)
    paths = [Path(path).resolve() for path in outputs]
    require(len(set(paths)) == len(paths) and capture not in paths
            and Path(__file__).resolve() not in paths, 'path-alias')
    for original, path in zip(outputs, paths):
        require(not Path(original).is_symlink() and not path.exists(), 'fresh-output')
    return capture, paths


def convert(capture, catalog, workplan, receipt):
    """Validate completely before fresh data/receipt publication; return receipt.

    Observed source bytes do not attest loaded bytecode or exclude ABA changes.
    Host I/O failures may leave fresh partial files; no transactional publication
    or arbitrary-capture source acceptance is claimed.
    """
    initial_adapter = adapter_identity()
    capture_path, outputs = fresh_paths(capture, [catalog, workplan, receipt])
    data = read_once(capture_path, MAX_TRANSPORT, 'transport-limit')
    catalog_bytes, graph_bytes, measurements = inventory(data)
    result = {**measurements, 'adapter': initial_adapter,
              'capture': {'bytes': len(data), 'sha256': digest(data)},
              'catalog': {'bytes': len(catalog_bytes), 'sha256': digest(catalog_bytes)},
              'workplan': {'bytes': len(graph_bytes), 'sha256': digest(graph_bytes)},
              'authority': 'transport measurements only; source acceptance requires matching successful trusted producer invocation',
              'consumer_acceptance': 'not established; existing catalog/workplan consumers remain authoritative',
              'source_identity_scope': 'observed adapter source bytes through receipt assembly/publication; no loaded bytecode or ABA attestation',
              'capture_scope': 'single bounded read of serialized bytes; no source-path reread or atomic live capture claim'}
    receipt_bytes = json.dumps(result, sort_keys=True, separators=(',', ':'), ensure_ascii=True).encode('ascii') + b'\n'
    require(len(receipt_bytes) <= MAX_RECEIPT, 'receipt-limit')
    require(adapter_identity() == initial_adapter, 'adapter-changed')
    for path in outputs:
        path.parent.mkdir(parents=True, exist_ok=True)
    for path, value in zip(outputs[:2], (catalog_bytes, graph_bytes)):
        with path.open('xb') as stream:
            stream.write(value)
    require(adapter_identity() == initial_adapter, 'adapter-changed')
    with outputs[2].open('xb') as stream:
        stream.write(receipt_bytes)
    require(adapter_identity() == initial_adapter, 'adapter-changed')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', required=True, type=Path)
    parser.add_argument('--catalog', required=True, type=Path)
    parser.add_argument('--workplan', required=True, type=Path)
    parser.add_argument('--receipt', required=True, type=Path)
    args = parser.parse_args()
    try:
        result = convert(args.capture, args.catalog, args.workplan, args.receipt)
    except InventoryError as error:
        parser.exit(1, f'project-input-inventory: {error}\n')
    except (OSError, RuntimeError, ValueError) as error:
        # Do not serialize source/input contents or unbounded path exception text.
        parser.exit(1, f'project-input-inventory: io-error {type(error).__name__} errno {getattr(error, "errno", None)}\n')
    print(json.dumps({'catalog': str(Path(args.catalog).resolve()), 'workplan': str(Path(args.workplan).resolve()),
                      'receipt': str(Path(args.receipt).resolve()), 'modules': result['module_count'],
                      'edges': result['direct_import_edges'], 'source_acceptance': 'unknown-without-trusted-producer-invocation'}))


if __name__ == '__main__':
    main()
