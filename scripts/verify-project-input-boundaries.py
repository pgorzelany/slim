#!/usr/bin/env python3
"""Optional finite RFC0165 whole-project boundary campaign.

`freeze` writes known literal source/manifest data and independent byte oracles.
`run` freezes the identical finite campaign before any native invocation, then
uses the production checker and ordinary/address+undefined sanitized producer.
There is no SLIM/project parser, semantic fallback, default gate, adjustable
budget, retry, or performance/productivity claim. Raw attempts stay in build/.
"""
import argparse
from dataclasses import dataclass
from datetime import datetime, timezone
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import reprlib
import shutil
import signal
import sys
import time


ROOT = Path(__file__).resolve().parents[1]
sys.dont_write_bytecode = True
OUTPUT_ROOT = ROOT / 'build/overnight-project-input-boundaries'
MIB = 1048576
GLOBAL_SECONDS = 900
CHILD_SECONDS = 60
EMIT_OUTPUT = 16 * MIB
OTHER_OUTPUT = 8 * MIB
PROCESS_FILE = 128 * MIB
TAG = b'slim-project-input-1'
CAPS = {'modules': 4095, 'edges': 65536, 'name_bytes': 64, 'path_bytes': 256,
        'payload_bytes': MIB, 'source_bytes': 4 * MIB, 'canonical_parsed_nodes': 1000000,
        'workplan_bytes': MIB, 'catalog_bytes': MIB, 'transport_bytes': 8 * MIB,
        'adapter_receipt_bytes': 4 * MIB}
VARIANTS = (('ordinary', ('-O2', '-DNDEBUG')),
            ('sanitized', ('-O1', '-g', '-fsanitize=address,undefined',
                           '-fno-sanitize-recover=all')))
NODE_REASON = 'existing producer transport/check CLI do not expose measured flattened canonical parsed-node entries; no independent source reparse'


class CampaignDeadline(ValueError):
    pass


def require(value, message):
    if not value:
        raise ValueError(message)


def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds').replace('+00:00', 'Z')


def digest(value):
    return hashlib.sha256(value).hexdigest()


def identity(value):
    return {'bytes': len(value), 'sha256': digest(value)}


def sha(path):
    return digest(Path(path).read_bytes())


def encoded(value):
    return (json.dumps(value, sort_keys=True, separators=(',', ':'), ensure_ascii=True) + '\n').encode('ascii')


def write_json(path, value):
    Path(path).write_bytes(encoded(value))


def write_frozen(path, value):
    with Path(path).open('xb') as stream:
        stream.write(value)
    require(Path(path).read_bytes() == value, 'materialized fixture/oracle bytes differ from literal data')


def load_fixed(relative, name, expected_hash):
    path = ROOT / relative
    require(sha(path) == expected_hash, 'fixed Python helper source changed before import')
    spec = importlib.util.spec_from_file_location(name, path)
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    require(sha(path) == expected_hash, 'fixed Python helper source changed after import')
    return result


def frame(value):
    return str(len(value)).encode('ascii') + b':' + value + b','


def triples(rows):
    return b''.join(frame(value) for row in rows for value in row)


@dataclass(frozen=True)
class Row:
    name: bytes
    path: bytes
    source: bytes
    imports: tuple


@dataclass(frozen=True)
class Fixture:
    name: str
    rows: tuple
    excess: str | None


def fixed_fixtures():
    # This is explicit manifest data construction, never source interpretation.
    names = tuple(f'm{i:04d}'.encode('ascii') for i in range(4095))
    rows = []
    for i, name in enumerate(names):
        imports = names[:min(i, 16)]
        if i == 4094:
            imports += names[16:168]
            source = b'module ' + name + b'\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
        else:
            source = b'module ' + name + b'\nfn value() -> I64:\n  0\n'
        rows.append(Row(name, name + b'.slim', source, imports))
    positive = tuple(rows)
    require(sum(len(row.imports) for row in positive[:-1]) == 65368, 'fixed prefix edge cardinality')
    require(sum(min(i, 16) for i in range(4095)) == 65384, 'fixed base edge cardinality')
    require(sum(len(row.imports) for row in positive) == 65536, 'fixed maximum edge cardinality')
    extra = Row(b'm4095', b'm4095.slim', b'module m4095\nfn value() -> I64:\n  0\n', ())
    edge_row = rows[-1]
    edge_row = Row(edge_row.name, edge_row.path, edge_row.source, edge_row.imports + (b'm0168',))
    return (Fixture('modules4095-edges65536', positive, None),
            Fixture('modules4096-edges65536', positive + (extra,), 'modules'),
            Fixture('modules4095-edges65537', positive[:-1] + (edge_row,), 'edges'))


def literal_manifest(rows):
    return (b'(project 1 (entry m4094)\n' + b''.join(
        b'  (module ' + row.name + b' "' + row.path + b'" (imports ' + b' '.join(row.imports)
        + b') (exports))\n' for row in rows) + b')\n')


def literal_oracle(fixture, adapter_identity):
    rows = fixture.rows
    manifest = literal_manifest(rows)
    graph = triples((row.name, str(len(row.source)).encode('ascii'), b','.join(row.imports)) for row in rows)
    catalog = triples([(b'@project', str(len(manifest)).encode('ascii'),
                        b'slim.project\0' + digest(manifest).encode('ascii'))]
                      + [(row.name, str(len(row.source)).encode('ascii'),
                          row.path + b'\0' + digest(row.source).encode('ascii')) for row in rows])
    edges = sum(len(row.imports) for row in rows)
    transport = (b''.join(frame(value) for value in
                         (TAG, str(len(rows)).encode('ascii'), str(edges).encode('ascii'), manifest, graph))
                 + triples((row.name, row.path, row.source) for row in rows))
    metadata = {'schema': 1, 'format': TAG.decode('ascii'), 'module_count': len(rows),
                'direct_import_edges': edges, 'source_bytes': len(manifest) + sum(len(row.source) for row in rows),
                'manifest': identity(manifest),
                'modules': [{'name': row.name.decode('ascii'), 'path_hex': row.path.hex(),
                             **identity(row.source)} for row in rows]}
    inventory_receipt = {**metadata, 'adapter': adapter_identity,
                         'capture': identity(transport), 'catalog': identity(catalog), 'workplan': identity(graph),
                         'authority': 'transport measurements only; source acceptance requires matching successful trusted producer invocation',
                         'consumer_acceptance': 'not established; existing catalog/workplan consumers remain authoritative',
                         'source_identity_scope': 'observed adapter source bytes through receipt assembly/publication; no loaded bytecode or ABA attestation',
                         'capture_scope': 'single bounded read of serialized bytes; no source-path reread or atomic live capture claim'}
    return {'manifest': manifest, 'workplan': graph, 'catalog': catalog, 'transport': transport,
            'metadata': metadata, 'inventory_receipt': encoded(inventory_receipt)}


def preflight(fixture, oracle):
    rows = fixture.rows
    names = tuple(row.name for row in rows)
    require(names == tuple(sorted(set(names))), 'known module names must be unique and byte sorted')
    require(len({row.path for row in rows}) == len(rows), 'known relative paths must be unique')
    known = set(names)
    for row in rows:
        require(row.imports == tuple(sorted(set(row.imports))), 'known imports must be unique and byte sorted')
        require(all(target in known and target < row.name for target in row.imports), 'known lower-name targets')
        require(row.path == row.name + b'.slim', 'fixed literal relative path')
    operands = {'modules': len(rows), 'edges': sum(len(row.imports) for row in rows),
                'name_bytes': max(map(len, names)), 'path_bytes': max(len(row.path) for row in rows),
                'payload_bytes': max([len(TAG), len(oracle['manifest']), len(oracle['workplan'])]
                                     + [len(row.source) for row in rows]
                                     + [len(b','.join(row.imports)) for row in rows]),
                'source_bytes': len(oracle['manifest']) + sum(len(row.source) for row in rows),
                'workplan_bytes': len(oracle['workplan']), 'catalog_bytes': len(oracle['catalog']),
                'transport_bytes': len(oracle['transport']),
                'adapter_receipt_bytes': len(oracle['inventory_receipt'])}
    crossing = [key for key, value in operands.items() if value > CAPS[key]]
    require(crossing == ([] if fixture.excess is None else [fixture.excess]), 'exactly the declared single excess')
    if fixture.excess is not None:
        require(operands[fixture.excess] == CAPS[fixture.excess] + 1, 'declared boundary must have one excess')
    else:
        require(operands['modules'] == CAPS['modules'] and operands['edges'] == CAPS['edges'], 'exact positive maxima')
    require(all(0 <= value < 2**63 for value in operands.values()), 'all fixed data operands fit I64')
    # Every sizing operand above is materialized from complete literal bytes.
    # Parsed nodes are a different representation; do not substitute byte/edge
    # counts for a production observation or invent a 1M-node crossing.
    dimensions = {key: {'classification': 'exact', 'value': value, 'limit': CAPS[key],
                        'admitted': value <= CAPS[key]} for key, value in operands.items()}
    dimensions['canonical_parsed_nodes'] = {'classification': 'unknown', 'value': None,
                                            'limit': CAPS['canonical_parsed_nodes'], 'reason': NODE_REASON}
    return dimensions


def source_identities():
    paths = {Path(__file__).resolve(), ROOT / 'project-input.project', ROOT / 'selfhost/slim.project',
             ROOT / 'scripts/project-input-inventory.py', ROOT / 'scripts/verify-project-input.py',
             ROOT / 'benchmarks/development/evaluate.py', ROOT / 'design/rfcs/0165-checked-project-input-producer.md'}
    for pattern in ('selfhost/*.slim', 'runtime/slim_rt.*', 'library/experimental/*.slim',
                    'library/components/project_input_*.slim', 'library/applications/project_input/*.slim'):
        paths.update(ROOT.glob(pattern))
    return {str(path.relative_to(ROOT)): identity(path.read_bytes()) for path in sorted(paths)}


def frozen_identities(folder):
    return {str(path.relative_to(folder)): identity(path.read_bytes())
            for path in sorted(folder.rglob('*')) if path.is_file()}


def safe_summary(error):
    bounded = reprlib.Repr()
    bounded.maxstring, bounded.maxother = 512, 512
    bounded.maxtuple, bounded.maxlist = 8, 8
    return (type(error).__name__ + ': ' + bounded.repr(error.args))[:4096]


class Campaign:
    def __init__(self, output, receipt, begun):
        self.output, self.receipt, self.begun = output, receipt, begun
        self.deadline = begun + GLOBAL_SECONDS * 1000000000
        self.expired = False
        self.runner = None

    def cutoff(self):
        if self.expired or time.monotonic_ns() >= self.deadline:
            raise CampaignDeadline('fixed 900-second whole-campaign deadline reached; remaining acceptance unknown')

    def expire(self, _signum, _frame):
        self.expired = True
        raise CampaignDeadline('fixed 900-second whole-campaign deadline reached')

    def save(self):
        write_json(self.output / 'receipt.json', self.receipt)

    def execute(self, label, argv, environment, cap=OTHER_OUTPUT):
        self.cutoff()
        seconds = min(CHILD_SECONDS, (self.deadline - time.monotonic_ns()) / 1000000000)
        require(seconds > 0, 'no campaign time remains for a child')
        before_utc, before_mono = utc(), time.monotonic_ns()
        result = self.runner.process(argv, self.output / 'commands' / label, seconds, environment,
                                     output_cap=cap, file_cap=PROCESS_FILE)
        observation = {key: value for key, value in result.items() if key not in ('stdout', 'stderr')}
        observation.update(label=label, wrapper_started_utc=before_utc, wrapper_finished_utc=utc(),
                           wrapper_elapsed_ns=time.monotonic_ns() - before_mono,
                           wrapper_timing_basis='same-process monotonic call-to-return; includes runner receipt work')
        write_json(self.output / 'commands' / (label + '.json'), observation)
        self.receipt['commands'].append(observation)
        self.save()
        self.cutoff()
        return result, result['stdout'].encode('latin1'), result['stderr'].encode('latin1')


def completed(result):
    return result['status'] in ('ok', 'compiler-error') and result['returncode'] is not None


def exact_gate(gate, result, stdout, stderr, expected):
    gate.pop('reason', None)
    gate.update(observed_returncode=result['returncode'], process_status=result['status'],
                observed_stdout=identity(stdout), observed_stderr=identity(stderr))
    if not completed(result):
        gate.update(status='unknown', reason='trusted invocation incomplete under fixed process bound')
    elif (result['returncode'], stdout, stderr) != expected:
        gate.update(status='failed', reason='completed invocation disagrees with frozen literal byte/status oracle')
    else:
        gate['status'] = 'pass'


def build_producers(campaign, compiler, cc, environment):
    result, generated, stderr = campaign.execute('producer-emit', [compiler, ROOT / 'project-input.project'],
                                                environment, EMIT_OUTPUT)
    require(completed(result) and result['returncode'] == 0 and stderr == b'' and bool(generated),
            'production producer emission did not complete successfully')
    require(not generated.startswith(b'#define SLIM_PARALLEL 1\n'), 'fixed producer campaign is serial')
    generated_path = campaign.output / 'producer.c'
    generated_path.write_bytes(generated)
    generated_hash = sha(generated_path)
    programs = {}
    for variant, flags in VARIANTS:
        target = campaign.output / ('producer-' + variant)
        result, stdout, stderr = campaign.execute('producer-' + variant + '-build',
            [cc, '-std=c11', '-Wall', '-Wextra', '-Werror', *flags, '-I', ROOT / 'runtime',
             generated_path, ROOT / 'runtime/slim_rt.c', '-o', target], environment)
        require(completed(result) and (result['returncode'], stdout, stderr) == (0, b'', b''),
                'fixed producer native build did not complete successfully: ' + variant)
        require(sha(generated_path) == generated_hash, 'generated production C changed during native build')
        programs[variant] = target
    campaign.receipt['generated_c'] = identity(generated)
    campaign.receipt['producers'] = {variant: {'path': str(path), **identity(path.read_bytes())}
                                     for variant, path in programs.items()}
    campaign.save()
    return programs


def serialized_frames(data):
    # Decode netstring data only after an exact match with the frozen transport.
    result, cursor = [], 0
    while cursor < len(data):
        colon = data.index(b':', cursor)
        length = int(data[cursor:colon])
        end = colon + 1 + length
        require(data[end:end + 1] == b',', 'independent successful transport frame ending')
        result.append(data[colon + 1:end])
        cursor = end + 1
    return result


def verify_inventory(adapter, data, oracle, destination):
    values = serialized_frames(data)
    count = int(values[1])
    require(len(values) == 5 + 3 * count, 'independent successful transport frame cardinality')
    metadata = {'schema': 1, 'format': values[0].decode('ascii'), 'module_count': count,
                'direct_import_edges': int(values[2]), 'source_bytes': len(values[3]),
                'manifest': identity(values[3]), 'modules': []}
    catalog_rows = [(b'@project', str(len(values[3])).encode('ascii'),
                     b'slim.project\0' + digest(values[3]).encode('ascii'))]
    for i in range(count):
        name, path, source = values[5 + 3 * i:8 + 3 * i]
        metadata['source_bytes'] += len(source)
        metadata['modules'].append({'name': name.decode('ascii'), 'path_hex': path.hex(), **identity(source)})
        catalog_rows.append((name, str(len(source)).encode('ascii'), path + b'\0' + digest(source).encode('ascii')))
    expected_catalog = triples(catalog_rows)
    require(metadata == oracle['metadata'] and expected_catalog == oracle['catalog']
            and values[4] == oracle['workplan'], 'serialized byte oracle differs from pre-native literal oracle')
    catalog, graph, measured = adapter.inventory(data)
    require((catalog, graph, measured) == (expected_catalog, values[4], metadata), 'adapter inventory byte/hash disagreement')
    destination.mkdir()
    capture = destination / 'capture.ns'
    capture.write_bytes(data)
    receipt = adapter.convert(capture, destination / 'catalog.ns', destination / 'graph.ns', destination / 'inventory.json')
    require((destination / 'catalog.ns').read_bytes() == expected_catalog
            and (destination / 'graph.ns').read_bytes() == values[4], 'published adapter bytes differ')
    for key, value in metadata.items():
        require(receipt[key] == value, 'published adapter metadata differs: ' + key)
    require(receipt['capture'] == identity(data) and receipt['catalog'] == identity(expected_catalog)
            and receipt['workplan'] == identity(values[4]), 'published adapter transport/output identities differ')
    receipt_bytes = (destination / 'inventory.json').read_bytes()
    require(receipt_bytes == oracle['inventory_receipt']
            and len(receipt_bytes) <= CAPS['adapter_receipt_bytes'], 'exact independent adapter receipt bytes/cap')
    return {'capture': identity(data), 'catalog': identity(catalog), 'workplan': identity(graph),
            'inventory_receipt': identity(receipt_bytes), 'modules': count, 'edges': int(values[2]),
            'source_hash_basis': 'supplied serialized manifest/module bytes only; no source-path reread',
            'consumer_acceptance': 'unknown; catalog/workplan consumers are not executed in this campaign'}


def freeze(campaign):
    folder = campaign.output / 'frozen'
    folder.mkdir()
    held, fixtures = {}, []
    for fixture in fixed_fixtures():
        campaign.cutoff()
        oracle = literal_oracle(fixture, campaign.receipt['source_before']['scripts/project-input-inventory.py'])
        dimensions = preflight(fixture, oracle)
        directory = folder / fixture.name
        project = directory / 'project'
        project.mkdir(parents=True)
        write_frozen(project / 'slim.project', oracle['manifest'])
        for row in fixture.rows:
            campaign.cutoff()
            write_frozen(project / row.path.decode('ascii'), row.source)
        for kind in ('workplan', 'catalog', 'transport'):
            write_frozen(directory / ('expected-' + kind + '.ns'), oracle[kind])
        write_frozen(directory / 'expected-metadata.json', encoded(oracle['metadata']))
        write_frozen(directory / 'expected-inventory.json', oracle['inventory_receipt'])
        producer_expected = (0, oracle['transport'], b'') if fixture.excess is None else (65, b'error 10 at 0\n', b'')
        specification = {'name': fixture.name, 'entry': 'm4094', 'exports': [],
                         'literal_source_templates': ['module NAME\nfn value() -> I64:\n  0\n',
                                                     'module m4094\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'],
                         'manifest_data': [{'name': row.name.decode('ascii'), 'path': row.path.decode('ascii'),
                                            'direct_imports': [name.decode('ascii') for name in row.imports],
                                            'source': identity(row.source)} for row in fixture.rows],
                         'dimensions': dimensions, 'declared_single_excess': fixture.excess,
                         'checker_expected': {'returncode': 0, 'stdout': identity(b''), 'stderr': identity(b'')},
                         'producer_expected': {'returncode': producer_expected[0], 'stdout': identity(producer_expected[1]),
                                               'stderr': identity(producer_expected[2])},
                         'expected_data': {key: identity(oracle[key]) for key in
                                           ('manifest', 'workplan', 'catalog', 'transport', 'inventory_receipt')},
                         'node_boundary': {'classification': 'unknown', 'reason': NODE_REASON},
                         'claim_scope': 'one fixed literal whole-project fixture; no efficacy/performance or general acceptance claim'}
        write_frozen(directory / 'specification.json', encoded(specification))
        campaign.receipt['fixtures'].append({key: specification[key] for key in
            ('name', 'entry', 'dimensions', 'declared_single_excess', 'expected_data', 'checker_expected', 'producer_expected')})
        held[fixture.name] = (fixture, oracle, project / 'slim.project', producer_expected)
        fixtures.append(specification)
        campaign.save()
    # Freeze the complete finite byte oracles, data operands and all bounds
    # before loading an adapter/runner or running the compiler/CC/producer.
    plan = {'schema': 1, 'bounds': campaign.receipt['bounds'], 'source_identities': campaign.receipt['source_before'],
            'tools': campaign.receipt.get('tools'), 'fixture_specification_identities':
                {fixture['name']: identity((folder / fixture['name'] / 'specification.json').read_bytes()) for fixture in fixtures},
            'required_native_commands': 12, 'native_checks': 3, 'native_producer_invocations': 6,
            'native_build_commands': 3, 'retries': 0, 'budget_changes_after_observation': 0,
            'node_boundary': {'classification': 'unknown', 'reason': NODE_REASON}}
    write_frozen(folder / 'campaign.json', encoded(plan))
    campaign.receipt['frozen_before_native'] = frozen_identities(folder)
    campaign.receipt['freeze_completed_utc'] = utc()
    campaign.receipt['freeze_completed_elapsed_ns'] = time.monotonic_ns() - campaign.begun
    campaign.save()
    return held


def native_campaign(campaign, held, compiler, cc, environment):
    sources = campaign.receipt['source_before']
    runner = load_fixed('benchmarks/development/evaluate.py', 'project_boundary_process',
                        sources['benchmarks/development/evaluate.py']['sha256'])
    adapter = load_fixed('scripts/project-input-inventory.py', 'project_boundary_inventory',
                         sources['scripts/project-input-inventory.py']['sha256'])
    campaign.runner = runner
    programs = build_producers(campaign, compiler, cc, environment)
    for name, (_fixture, oracle, manifest, expected) in held.items():
        check_gate = campaign.receipt['gates'][name + '-check']
        result, stdout, stderr = campaign.execute(name + '-check', [compiler, 'check', manifest], environment)
        exact_gate(check_gate, result, stdout, stderr, (0, b'', b''))
        check_gate['claim'] = ('production checker accepted exact supplied bytes' if check_gate['status'] == 'pass'
                               else 'expected positive source acceptance not established')
        campaign.save()
        for variant, program in programs.items():
            gate = campaign.receipt['gates'][name + '-' + variant]
            result, stdout, stderr = campaign.execute(name + '-' + variant, [program, manifest], environment)
            exact_gate(gate, result, stdout, stderr, expected)
            if gate['status'] == 'pass' and expected[0] == 0:
                try:
                    gate['inventory'] = verify_inventory(adapter, stdout, oracle, campaign.output / (name + '-' + variant + '-inventory'))
                    gate['claim'] = 'matching successful production checked producer and exact serialized-byte inventory'
                except Exception as error:
                    gate.update(status='failed', reason='adapter inventory verification failed', failure=safe_summary(error))
                    if isinstance(error, CampaignDeadline):
                        raise
            elif gate['status'] == 'pass':
                gate['claim'] = 'exact logical admission rejection; source checker acceptance recorded separately'
                gate['successful_transport_prefix_bytes'] = 0
            campaign.save()
    campaign.receipt['producer_after'] = {variant: {'path': str(path), **identity(path.read_bytes())}
                                          for variant, path in programs.items()}
    require(campaign.receipt['producer_after'] == campaign.receipt['producers'], 'producer executable bytes changed')


def main(argv=None):
    started_utc, begun = utc(), time.monotonic_ns()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('mode', choices=('freeze', 'run'), help='run explicitly opts into the fixed native campaign')
    parser.add_argument('--compiler', type=Path, default=ROOT / 'build/toolchain/slimc')
    parser.add_argument('--cc', default=os.environ.get('CC', 'cc'), help='one trusted executable path/name; no flags or shell')
    parser.add_argument('--output', type=Path, default=OUTPUT_ROOT / 'campaign')
    options = parser.parse_args(argv)
    output = options.output.resolve()
    require(output.is_relative_to(OUTPUT_ROOT.resolve()) and output != OUTPUT_ROOT.resolve(), 'fresh output must be a descendant of ignored build/overnight-project-input-boundaries')
    output.mkdir(parents=True, exist_ok=False)
    receipt = {'schema': 1, 'status': 'running', 'mode': options.mode, 'started_utc': started_utc,
               'timing_basis': 'UTC plus same-process monotonic whole-wrapper elapsed; operator correctness timing only',
               'bounds': {'global_seconds': GLOBAL_SECONDS, 'child_seconds': CHILD_SECONDS,
                          'live_combined_output_bytes': {'emit': EMIT_OUTPUT, 'other': OTHER_OUTPUT},
                          'child_file_bytes': PROCESS_FILE, 'caps': CAPS,
                          'retries': 0, 'budget_changes_after_observation': 0},
               'source_before': source_identities(), 'commands': [], 'fixtures': [],
               'gates': {name + '-' + operation: {'status': 'unknown', 'reason': 'not executed'}
                         for name in ('modules4095-edges65536', 'modules4096-edges65536', 'modules4095-edges65537')
                         for operation in ('check', 'ordinary', 'sanitized')},
               'unknown': [NODE_REASON, 'whole-project one-million-node boundary', 'preread physical/RSS/CPU bounds',
                           'filesystem atomicity, loaded Python bytecode and ABA', 'SDK/transitive native toolchain identity',
                           'catalog/workplan consumer acceptance', 'performance improvement and general efficacy']}
    campaign = Campaign(output, receipt, begun)
    previous_handler = signal.signal(signal.SIGALRM, campaign.expire)
    signal.setitimer(signal.ITIMER_REAL, max(0.001, GLOBAL_SECONDS - (time.monotonic_ns() - begun) / 1000000000))
    try:
        compiler = cc = None
        if options.mode == 'run':
            compiler = options.compiler.resolve(strict=True)
            cc = Path(shutil.which(options.cc) or options.cc).resolve(strict=True)
            receipt['tools'] = {key: {'path': str(path), **identity(path.read_bytes())}
                                for key, path in (('compiler', compiler), ('cc', cc), ('python', Path(sys.executable).resolve(strict=True)))}
        held = freeze(campaign)
        require(receipt['source_before'] == source_identities(), 'held source bytes changed during freeze')
        require(receipt['frozen_before_native'] == frozen_identities(output / 'frozen'), 'fixture/oracle bytes changed before native')
        if options.mode == 'run':
            environment = dict(os.environ, LC_ALL='C', LANG='C', ASAN_OPTIONS='detect_leaks=0:abort_on_error=1', UBSAN_OPTIONS='halt_on_error=1')
            for key in ('SLIM_ALLOC_FAIL_AT', 'SLIM_TASK_FAIL_AT', 'SLIM_TASK_JOIN_FAIL_AT', 'SLIM_TASK_DISABLE'):
                environment.pop(key, None)
            native_campaign(campaign, held, compiler, cc, environment)
            campaign.cutoff()
            receipt['status'] = 'pass' if all(gate['status'] == 'pass' for gate in receipt['gates'].values()) else 'failed'
        else:
            receipt['status'] = 'frozen'
    except Exception as error:
        receipt.update(status='timeout' if isinstance(error, CampaignDeadline) else 'failed', failure=safe_summary(error))
    finally:
        signal.setitimer(signal.ITIMER_REAL, 0)
        signal.signal(signal.SIGALRM, previous_handler)
        try:
            receipt['source_after'] = source_identities()
            receipt['frozen_after'] = frozen_identities(output / 'frozen') if (output / 'frozen').exists() else {}
            receipt['pins_unchanged'] = (receipt['source_before'] == receipt['source_after']
                                         and receipt.get('frozen_before_native') == receipt['frozen_after'])
            if 'tools' in receipt:
                receipt['tools_after'] = {key: {'path': value['path'], **identity(Path(value['path']).read_bytes())}
                                          for key, value in receipt['tools'].items()}
                receipt['pins_unchanged'] = receipt['pins_unchanged'] and receipt['tools'] == receipt['tools_after']
            if not receipt['pins_unchanged']:
                receipt.update(status='failed', pin_failure='held source/tool/fixture bytes changed; acceptance cannot be attributed to frozen identities')
        except Exception as error:
            receipt.update(status='failed', pin_failure=safe_summary(error))
        receipt.update(finished_utc=utc(), elapsed_ns=time.monotonic_ns() - begun)
        if receipt['elapsed_ns'] > GLOBAL_SECONDS * 1000000000 and receipt['status'] in ('pass', 'frozen'):
            receipt.update(status='timeout', failure='whole-wrapper receipt/identity completion exceeded fixed 900-second bound')
        campaign.save()
    if receipt['status'] in ('pass', 'frozen'):
        print('project-input-boundaries: ' + receipt['status'].upper() + '; fixed 3 fixtures; parsed-node boundary unknown')
        return 0
    print('project-input-boundaries: ' + receipt['status'].upper() + '; retained receipt ' + str(output / 'receipt.json'), file=sys.stderr)
    return 1


if __name__ == '__main__':
    sys.exit(main())
