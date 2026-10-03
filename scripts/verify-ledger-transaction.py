#!/usr/bin/env python3
"""Fixed RFC171 current-source gate with a fresh independent data hold.

No SLIM/log/manifest parser, source acceptance fallback, retries or budget options.
The ordinary production compiler and matching native programs decide acceptance.
"""
import argparse
from contextlib import redirect_stdout, redirect_stderr
from datetime import datetime, timezone
import hashlib
import io
import json
import os
from pathlib import Path
import shutil
import signal
import sys
import time
import types

MIB = 1048576
ORACLE_SHA = '46e1511237cf6194fd52f2b68b25b1a79d927e75ba7ee1bc5b0e282e79b1c9bb'
HELPER_SHA = '245ccc2e41b38546bdc12bb49dae0b4331731db57174704c4d16ee805aff9c8c'
COLLECTOR_SHA = 'ccfb96dd2f0cd5fbeaeb0b4f16a1ca7e040baa1bd31fbe14ba15fe2939cfa84a'
ADAPTER_SHA = '004b176cdd01aba975e53f3292cd13a759bba645831279c42fa0dabcb8003c00'
STATE_PATH = 'library/applications/ledger/state.slim'
BASELINE_SHA = '0316e7900060a061ec16f5859905a28dfecceec383828bd9f0a58a2955efc69e'
PREFIX_SHA = '3f3c1d7621d09fedfdfa66d461bf2c947e4321411308ba255aac5c13557a0bea'
GENERATOR_SHA = 'e09de149877002ef4bd955740feb5172ca21f497fac1c5ff88cfb01483241590'
RFC_PATH = 'design/rfcs/0171-ledger-transaction-overflow-preflight.md'
HELPER_PATH = 'scripts/verify-project-impact.py'
COLLECTOR_PATH = 'scripts/project-impact-context.py'
ADAPTER_PATH = 'scripts/project-input-inventory.py'
CONTROL_PATHS = {
 'oracle':'scripts/ledger-transaction-oracle.py',
 'verifier':'scripts/verify-ledger-transaction.py',
 'generator':'scripts/ledger-transaction-probe.py',
 'prefix':'tests/fixtures/ledger_transaction_probe_prefix.slim',
 'baseline':'tests/fixtures/ledger_transaction_baseline_state.slim',
 'rfc':RFC_PATH}
ROLES = ('emit-producer', 'build-producer', 'emit-impact', 'build-impact',
         'capture-before', 'capture-after', 'impact')
LEDGER = (
 ('ledger', 'applications/ledger/main.slim', 'ledger_emit ledger_model ledger_state'),
 ('ledger_emit', 'applications/ledger/emit.slim', 'ledger_model ledger_state std_text'),
 ('ledger_model', 'applications/ledger/model.slim', ''),
 ('ledger_parser', 'applications/ledger/parser.slim', 'ledger_model std_bytes std_decimal'),
 ('ledger_state', 'applications/ledger/state.slim', 'ledger_model ledger_parser std_bytes'),
 ('std_ascii', 'experimental/ascii.slim', ''),
 ('std_bytes', 'experimental/bytes.slim', ''),
 ('std_decimal', 'experimental/decimal.slim', 'std_ascii'),
 ('std_text', 'experimental/text.slim', 'std_bytes'))
LEDGER_PATHS = ('library/ledger.project',) + tuple('library/' + row[1] for row in LEDGER)
COLLECTOR_PATHS = '''library/applications/catalog/catalog.slim
library/applications/catalog/emit.slim
library/applications/catalog/model.slim
library/applications/catalog/reconcile.slim
library/applications/project_impact/closure.slim
library/applications/project_impact/data.slim
library/applications/project_impact/input.slim
library/applications/project_impact/main.slim
library/applications/project_impact/model.slim
library/applications/project_impact/prepare.slim
library/applications/project_impact/report.slim
library/applications/project_input/main.slim
library/applications/workplan/load.slim
library/applications/workplan/model.slim
library/components/project_input_data.slim
library/components/project_input_emit.slim
library/components/project_input_limits.slim
library/components/project_input_model.slim
library/components/records.slim
library/experimental/ascii.slim
library/experimental/byte_index.slim
library/experimental/bytes.slim
library/experimental/decimal.slim
library/experimental/netstring.slim
library/experimental/text.slim
library/project-impact.project
project-input.project
runtime/slim_rt.c
runtime/slim_rt.h
scripts/project-impact-context.py
scripts/project-input-inventory.py
selfhost/check.slim
selfhost/codegen.slim
selfhost/control.slim
selfhost/diagnostics.slim
selfhost/effects.slim
selfhost/format.slim
selfhost/identity.slim
selfhost/ir.slim
selfhost/memory.slim
selfhost/ownership.slim
selfhost/parallel.slim
selfhost/project.slim
selfhost/ranges.slim
selfhost/retained.slim
selfhost/scheduler.slim
selfhost/syntax.slim
selfhost/text.slim
selfhost/typing.slim
selfhost/validate.slim'''.splitlines()
READSET = tuple(sorted(set(LEDGER_PATHS + tuple(COLLECTOR_PATHS) + (HELPER_PATH,))))
PROBE_MANIFEST = b'''(project 1
  (entry ledger_probe)
  (module ledger_model "applications/ledger/model.slim" (imports) (exports Account Applied Command Parsed Run Word))
  (module ledger_parser "applications/ledger/parser.slim" (imports ledger_model std_bytes std_decimal) (exports parse_line))
  (module ledger_probe "ledger_probe.slim" (imports ledger_model ledger_state std_bytes) (exports))
  (module ledger_state "applications/ledger/state.slim" (imports ledger_model ledger_parser std_bytes) (exports apply total))
  (module std_ascii "experimental/ascii.slim" (imports) (exports digit_value hex_value is_alpha is_alphanumeric is_digit is_hex_digit is_lower is_space is_upper))
  (module std_bytes "experimental/bytes.slim" (imports) (exports append append_range count_byte ends_with equal find_byte range_equal starts_with))
  (module std_decimal "experimental/decimal.slim" (imports std_ascii) (exports Parsed parse_i64 parse_u8)))
'''
PROBE_PATHS = tuple('library/' + path for name, path, unused in LEDGER
                    if name not in ('ledger', 'ledger_emit', 'std_text'))
FLAGS = (('ordinary', ('-O2', '-DNDEBUG')),
         ('sanitized', ('-O1', '-g', '-fsanitize=address,undefined',
                        '-fno-omit-frame-pointer')))
DARWIN_SIDECARS = (
 'ledger-sanitized.dSYM/Contents/Info.plist',
 'ledger-sanitized.dSYM/Contents/Resources/DWARF/ledger-sanitized',
 'ledger-sanitized.dSYM/Contents/Resources/Relocations/aarch64/ledger-sanitized.yml',
 'probe-sanitized.dSYM/Contents/Info.plist',
 'probe-sanitized.dSYM/Contents/Resources/DWARF/probe-sanitized',
 'probe-sanitized.dSYM/Contents/Resources/Relocations/aarch64/probe-sanitized.yml')

def native_artifact_tier():
    machine = os.uname().machine
    require(sys.platform != 'darwin' or machine == 'arm64', 'fixed Darwin arm64 sidecar tier')
    sidecars = list(DARWIN_SIDECARS) if sys.platform == 'darwin' else []
    return {'platform':sys.platform, 'machine':machine, 'sidecar_paths':sidecars,
            'files_including_receipt':236+len(sidecars),
            'nonself_file_hashes':235+len(sidecars)}

CLEANUP_SOURCE = (b'import os,sys,time\nchild=os.fork()\nif child==0:\n'
                  b' os.close(0);os.close(1);os.close(2);time.sleep(120);os._exit(0)\n'
                  b'print(child,flush=True)\n')
UNKNOWN = {
 'application_invariants': 'not analyzed', 'agent_effectiveness': 'not measured',
 'context_sufficiency': 'declared-import selection is not a sufficiency proof',
 'saved_compiler_work': 'not measured',
 'physical_source_deduplication': 'counts and weights count supplied catalog rows',
 'atomic_live_capture': 'no atomic or final-live identity attestation',
 'loaded_code_aba_host_boot': 'before/after observed bytes only',
 'transitive_toolchain': 'direct tools pinned; SDK/linker/transitive identity not established',
 'physical_memory': 'logical caps are not RSS or libc allocation bounds'}

def labels():
    names = ['cleanup-control', 'emit-baseline-ledger', 'build-baseline-ledger-ordinary',
             'baseline-max-credit-ordinary', 'check-ledger', 'check-probe', 'emit-ledger', 'emit-probe',
             'build-ledger-ordinary', 'build-ledger-sanitized', 'build-probe-ordinary', 'build-probe-sanitized',
             'states-ordinary', 'states-sanitized']
    names.extend('replay-' + str(n).zfill(2) + '-' + variant for n in range(1, 9)
                 for variant in ('ordinary', 'sanitized'))
    names.extend('partial-' + str(n).zfill(2) + '-' + variant for n in range(1, 3)
                 for variant in ('ordinary', 'sanitized'))
    names.extend(('diagnostic-positive', 'diagnostic-missing-partial'))
    names.extend('context-before-' + name for name in ('credit_account', 'debit_account', 'transfer'))
    names.extend('impact-context-' + name for name in ROLES)
    return names

def require(value, message):
    if not value:
        raise ValueError(message)

def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds')

def identity(data):
    return {'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}

def canonical(value):
    return (json.dumps(value, sort_keys=True, separators=(',', ':'), ensure_ascii=True) + '\n').encode('ascii')

def read(path, cap=MIB):
    require(path.is_file() and not path.is_symlink(), 'ordinary fixed file')
    with path.open('rb') as stream:
        data = stream.read(cap + 1)
    require(len(data) <= cap, 'fixed byte cap')
    return data

def file_identity(path, cap=128*MIB):
    total = 0; hashed = hashlib.sha256()
    require(path.is_file() and len(os.fsencode(path)) <= 4096, 'fixed direct file/path domain')
    with path.open('rb') as stream:
        while True:
            part = stream.read(min(65536, cap-total+1))
            if not part:
                break
            total += len(part); require(total <= cap, 'fixed direct file cap'); hashed.update(part)
    return {'bytes': total, 'sha256': hashed.hexdigest()}

def publish(path, data, cap=MIB):
    require(type(data) is bytes and len(data) <= cap, 'publication byte cap')
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(data)

def root_path():
    for parent in Path(__file__).resolve().parents:
        if (parent / 'design/FEATURE_POLICY.md').is_file():
            return parent
    raise ValueError('repository root unavailable')

def captured_module(path, data, pin, name):
    require(identity(data)['sha256'] == pin and read(path) == data, 'fixed captured code identity')
    module = types.ModuleType(name); module.__file__ = str(path)
    sys.modules[name] = module
    try:
        exec(compile(data, str(path), 'exec', dont_inherit=True), module.__dict__)
        require(read(path) == data, 'captured code endpoint')
    except BaseException:
        sys.modules.pop(name, None)
        raise
    return module

def additional_source(model):
    inline = model.get('additional_source_hex')
    require(type(inline) is dict and set(inline) == {'selfhost/diagnostics.slim'}, 'fixed additional source domain')
    name = 'selfhost/diagnostics.slim'
    value = inline[name]
    require(type(value) is str and len(value) <= 2*MIB and len(value)%2 == 0 and
            all(byte in '0123456789abcdef' for byte in value), 'bounded complete source byte encoding')
    data = bytes.fromhex(value)
    require(identity(data) == model['source_pins'][name], 'complete additional source identity')
    return data


def additional_source_integrity():
    # These are private held-data custody controls, never source acceptance.
    name = 'selfhost/diagnostics.slim'
    def declaration(value, pin):
        return {'additional_source_hex':{name:value},'source_pins':{name:pin}}
    data = b'\x00\xff\n'
    pin = identity(data)
    require(additional_source(declaration('00ff0a',pin)) == data, 'literal complete binary source bytes')
    maximum = b'x'*MIB
    require(additional_source(declaration(maximum.hex(),identity(maximum))) == maximum, 'exact source byte bound')
    failures = (
        {'additional_source_hex':[], 'source_pins':{name:pin}},
        {'additional_source_hex':{'other.slim':'00ff0a'}, 'source_pins':{name:pin}},
        declaration(0,pin), declaration('0',pin), declaration('00ff0g',pin),
        declaration('00'* (MIB+1),pin),
        declaration('00ff0a',dict(pin,bytes=4)),
        declaration('00ff0a',dict(pin,sha256='0'*64)),
    )
    for case in failures:
        try:
            additional_source(case)
        except ValueError:
            pass
        else:
            raise ValueError('malformed held source unexpectedly admitted')
    return {'scope':'opaque held-data integrity; no SLIM parsing or source acceptance',
            'positive_controls':2,'negative_controls':len(failures)}


def held_data(folder, model_sha, freeze_sha, controls):
    raw = read(folder/'freeze.json', MIB)
    require(identity(raw)['sha256'] == freeze_sha, 'fresh independent freeze identity')
    record = json.loads(raw)
    require(record['status'] == 'data-held-no-native' and record['native_commands'] == 0, 'pure data hold')
    require(len(record['files'])+1 == 128 and
            sum(row['bytes'] for row in record['files'].values())+len(raw) <= 16*MIB, 'full receipt-inclusive hold cap')
    actual = {str(path.relative_to(folder)) for path in folder.rglob('*') if path.is_file()}
    require(actual == set(record['files']) | {'freeze.json'}, 'exact held file inventory')
    files = {}
    for name, pin in record['files'].items():
        path = Path(name)
        require(not path.is_absolute() and '..' not in path.parts, 'fixed held relative path')
        files[name] = read(folder/path, 16*MIB)
        require(identity(files[name]) == pin, 'held artifact identity')
    require(identity(files['model.json'])['sha256'] == model_sha and
            identity(files['model.json']) == record['model'] and len(files['model.json']) <= 4*MIB, 'fixed complete model')
    model = json.loads(files['model.json'])
    require(identity(files['oracle.py'])['sha256'] == ORACLE_SHA, 'accepted independent oracle bytes')
    control_pins = {name:identity(data) for name,data in controls.items()}
    require(record['controls_before'] == record['controls_after'] == model['control_sources'] == control_pins,
            'fresh complete control endpoints')
    for name, artifact in (('oracle','oracle.py'),('verifier','verifier.py'),('generator','generator.py'),
                           ('prefix','prefix.slim'),('baseline','baseline-state.slim'),('rfc','rfc.md')):
        require(files[artifact] == controls[name], 'current frozen control bytes')
    require(set(model['source_pins']) == set(READSET) and len(READSET) == 57 and len(COLLECTOR_PATHS) == 50,
            'literal source registry')
    require(record['sources_before'] == record['sources_after'] == model['source_pins'] and
            record['tools_before'] == record['tools_after'] == model['tools'], 'held endpoint equality')
    require(model['source_storage'] == {'physical_hold_files':128,'logical_hold_artifacts':129,
            'source_records':57,'standalone_source_files':56,'inline_source_records':1},
            'fixed physical and logical source storage')
    # The one additional source shares the hashed model artifact. The physical
    # hold keeps all 128 files; this ephemeral byte view adds one logical entry.
    name = 'selfhost/diagnostics.slim'
    require('sources/'+name not in files, 'additional source has no separate physical artifact')
    files['sources/'+name] = additional_source(model)
    require(model['native_labels'] == labels() and len(labels()) == 46 and model['data_cases'] == 184 and
            len(model['states']) == 171 and len(model['replays']) == 8 and len(model['partials']) == 2 and
            len(model['diagnostics']) == 2, 'fixed finite campaign')
    require(files['states.expected.bin'] == b''.join(bytes.fromhex(row['stdout_hex']) for row in model['states']),
            'complete independent batch output')
    for name in LEDGER_PATHS:
        expected = controls['baseline'] if name == STATE_PATH else files['sources/'+name]
        require(files['before/'+name] == files['expected/'+name] == expected, 'full independent baseline snapshot identity')
    return record, model, files

def source_admission(root):
    captured = {name: read(root/name) for name in READSET}
    pins = {name: identity(data) for name, data in captured.items()}
    require(len(READSET) == 57 and len(set(READSET)) == 57, 'fixed current opaque source registry')
    controls = {name:read(root/path) for name,path in CONTROL_PATHS.items()}
    for name, pin in (('oracle',ORACLE_SHA),('prefix',PREFIX_SHA),('generator',GENERATOR_SHA),('baseline',BASELINE_SHA)):
        require(identity(controls[name])['sha256'] == pin, 'fixed independent oracle/probe/baseline source')
    require(b'Status: accepted' in controls['rfc'].splitlines()[:24] and
            b'Implementation: complete' in controls['rfc'].splitlines()[:24], 'current accepted implemented RFC171')
    for name, pin in ((HELPER_PATH, HELPER_SHA), (COLLECTOR_PATH, COLLECTOR_SHA), (ADAPTER_PATH, ADAPTER_SHA)):
        require(pins[name]['sha256'] == pin, 'fixed helper/collector/adapter authority')
    compiler = (root/'build/toolchain/slimc').resolve(strict=True)
    cc_name = shutil.which('cc'); require(cc_name is not None, 'fixed CC available')
    cc = Path(cc_name).resolve(strict=True); python = Path(sys.executable).resolve(strict=True)
    tools = {name: {'path_hex': os.fsencode(path).hex(), **file_identity(path)}
             for name, path in (('compiler', compiler), ('cc', cc), ('python', python))}
    require(os.access(compiler, os.X_OK) and os.access(cc, os.X_OK), 'native executable domain')
    return captured,pins,controls,tools,compiler,cc

def admit(root, folder, model_sha, freeze_sha):
    captured,pins,controls,tools,compiler,cc = source_admission(root)
    record, model, files = held_data(folder, model_sha, freeze_sha, controls)
    for name in READSET:
        require(identity(files['sources/'+name]) == model['source_pins'][name] == pins[name],
                'fresh current complete opaque source identity')
    require(tools == model['tools'], 'same admitted direct tools')
    return record, model, files, captured, pins, controls, tools, compiler, cc

def snapshot(output, prefix, files, current=None):
    for name in LEDGER_PATHS:
        data = current[name] if current is not None else files[prefix+'/'+name]
        publish(output/prefix/name, data)
    return output/prefix/'library/ledger.project'

def probe_project(output, name, current, entry):
    directory = output/name
    for path in PROBE_PATHS:
        publish(directory/path, current[path])
    publish(directory/'library/ledger_probe.slim', entry)
    publish(directory/'library/ledger.project', PROBE_MANIFEST)
    return directory/'library/ledger.project'

def directory_pins(path):
    return {str(child.relative_to(path)): file_identity(child) for child in sorted(path.rglob('*')) if child.is_file()}

def frame(data):
    return str(len(data)).encode('ascii') + b':' + data + b','

def literal_transport(files, current):
    """Fixed manifest declarations and opaque bytes only; no grammar/checker."""
    result = []
    for bodies in ({path: files['before/'+path] for path in LEDGER_PATHS}, current):
        manifest = bodies['library/ledger.project']; graph = b''; catalog = b''
        records = {'@project': (len(manifest), b'slim.project\0'+identity(manifest)['sha256'].encode('ascii'))}
        modules = []; edges = 0
        for name, path, imports in LEDGER:
            source = bodies['library/'+path]; weight = str(len(source)).encode('ascii')
            csv = imports.replace(' ', ',').encode('ascii'); edges += len(imports.split())
            graph += b''.join(frame(field) for field in (name.encode('ascii'), weight, csv))
            records[name] = (len(source), path.encode('ascii')+b'\0'+identity(source)['sha256'].encode('ascii'))
            modules.extend((name.encode('ascii'), path.encode('ascii'), source))
        for name in sorted(records):
            weight, value = records[name]
            catalog += b''.join(frame(field) for field in (name.encode('ascii'), str(weight).encode('ascii'), value))
        capture = b''.join(frame(field) for field in
                           (b'slim-project-input-1', b'9', str(edges).encode('ascii'), manifest, graph, *modules))
        result.append({'capture': capture, 'catalog': catalog, 'graph': graph, 'records': records,
                       'manifest_bytes': len(manifest), 'module_bytes': sum(len(bodies['library/'+row[1]]) for row in LEDGER),
                       'edges': edges})
    return result

def record_bytes(name, view):
    weight, value = view['records'][name]
    return b''.join(frame(field) for field in (name.encode('ascii'), str(weight).encode('ascii'), value))

def record_json(name, view):
    weight, value = view['records'][name]; path, hashed = value.split(b'\0')
    return {'name_hex': name.encode('ascii').hex(), 'bytes': weight, 'path_hex': path.hex(),
            'source_sha256': hashed.decode('ascii')}

def literal_impact(views):
    old, new = views; selected = ('ledger', 'ledger_emit', 'ledger_state')
    fields = [b'slim-project-impact-1', b'0', record_bytes('@project', old), record_bytes('@project', new),
              b'1', b'modified', b'ledger_state', record_bytes('ledger_state', old), record_bytes('ledger_state', new), b'0']
    for names in (('ledger_state',), ('ledger_state',), selected, selected):
        fields.extend((str(len(names)).encode('ascii'), *(name.encode('ascii') for name in names)))
    fields.append(b'3'); fields.extend(record_bytes(name, new) for name in selected)
    return b''.join(frame(field) for field in fields)

def expected_cli(row):
    return row['returncode'], bytes.fromhex(row['stdout_hex']), bytes.fromhex(row['stderr_hex'])

def successful(campaign, label, argv, environment, emit=False):
    gate = campaign.receipt['gates'][label]
    row, out, err = campaign.execute(label, argv, environment, 16*MIB if emit else 8*MIB)
    gate.pop('reason', None)
    gate.update(process_status=row['status'], returncode=row['returncode'], stdout=identity(out), stderr=identity(err))
    complete = row['status'] in ('ok', 'native-error') and row['returncode'] is not None
    gate['status'] = 'pass' if complete and row['returncode'] == 0 and not err and (bool(out) if emit else not out) else ('failed' if complete else 'unknown')
    if gate['status'] != 'pass':
        gate['reason'] = 'fixed successful emission/build domain not completed'
    campaign.save(); require(gate['status'] == 'pass', label+' failed or incomplete')
    return out

def exact(campaign, label, argv, environment, expected):
    gate, out = campaign.exact(label, argv, environment, expected)
    require(gate['status'] == 'pass', label+' disagrees with held exact oracle or is incomplete')
    return out

def cleanup_control(campaign, environment):
    path = campaign.output/'cleanup-control.py'; publish(path, CLEANUP_SOURCE)
    row, out, err = campaign.execute('cleanup-control', [sys.executable, path], environment, seconds=2)
    gate = campaign.receipt['gates']['cleanup-control']; gate.pop('reason', None)
    gate.update(process_status=row['status'], returncode=row['returncode'], source=identity(CLEANUP_SOURCE))
    complete = row['status'] in ('ok', 'native-error') and row['returncode'] is not None
    gate['status'] = 'failed' if complete else 'unknown'
    if row['status'] == 'ok' and row['returncode'] == 0 and not err and out.endswith(b'\n') and out[:-1].isdigit():
        pid = int(out[:-1]); require(pid > 1, 'fixed descendant PID domain')
        until = time.monotonic()+1; gone = False
        while time.monotonic() < until:
            try:
                os.kill(pid, 0)
            except ProcessLookupError:
                gone = True; break
            time.sleep(.01)
        gate.update(status='pass' if gone else 'failed', descendant_absent=gone)
    if gate['status'] != 'pass':
        gate['reason'] = 'fixed closed-pipe descendant cleanup incomplete or failed'
    campaign.save(); require(gate['status'] == 'pass', 'mandatory process group cleanup control')

def context_observation(out, selector, baseline):
    require(len(out) <= MIB, 'context report cap')
    value = json.loads(out)
    require(value['schema'] == 1 and value['selector'] == selector and
            value['encoding'] == 'json-byte-escapes-v1' and value['identity_evidence'] == 'exact-expected-input-bytes' and
            value['input']['kind'] == 'project', 'actual checked baseline context schema/identity')
    chosen = value['selected']; span = chosen['span']
    require(chosen['module'] == 'ledger_state' and chosen['name'] == selector.split('.')[1] and
            chosen['kind'] == 'function' and chosen['evidence'] == 'exact' and
            chosen['provenance'] == 'checked-declaration', 'original checked selector declaration')
    require(type(span['start']) is int and type(span['end']) is int and
            span['file'] == 4 and 0 <= span['start'] <= span['end'] <= len(baseline) and
            chosen['source'].encode('latin1') == baseline[span['start']:span['end']], 'complete captured original declaration bytes')
    require(value['work']['report_bytes'] == len(out), 'actual context report byte counter')
    for key, cap in (('providers', 64), ('facts', 512), ('references', 512)):
        section = value[key]
        require(section['limit'] == cap and section['evidence'] in ('exact', 'bounded') and len(section['rows']) <= cap,
                'independent context section completeness labels')
    return {'selector': selector, 'raw': identity(out), 'selected_bytes': len(chosen['source'].encode('latin1')),
            'work': value['work'], 'completeness': {key: {field: value[key][field] for field in ('evidence','limit','reason')}
                                                     for key in ('providers','facts','references')},
            'meaning': 'same-byte checked baseline context; no edit diff or application invariant'}

class BoundedText(io.StringIO):
    def __init__(self):
        super().__init__(); self.used = 0; self.exceeded = False
    def write(self, value):
        require(type(value) is str and value.isascii(), 'canonical CLI ASCII stream')
        remaining = 32768-self.used
        if len(value) > remaining:
            super().write(value[:remaining]); self.used += remaining; self.exceeded = True
            raise ValueError('fixed canonical CLI stream cap exceeded; retained prefix is incomplete')
        self.used += len(value)
        return super().write(value)

def invoke_main(module, argv, streams):
    require(signal.getitimer(signal.ITIMER_REAL)[0] > 0, 'positive remaining shared parent deadline')
    previous = sys.argv
    try:
        sys.argv = argv
        with redirect_stdout(streams['stdout']), redirect_stderr(streams['stderr']):
            streams['returncode'] = module.main()
    finally:
        sys.argv = previous
    return streams['returncode'], streams['stdout'].getvalue().encode('ascii'), streams['stderr'].getvalue().encode('ascii')

def collect_roles(campaign, output):
    if not (output/'receipt.json').is_file():
        return
    raw = json.loads(read(output/'receipt.json', 2*MIB))
    campaign.receipt['collector_receipt'] = identity(read(output/'receipt.json', 2*MIB))
    for index, row in enumerate(raw['roles'], 1):
        require(index <= 7 and row['role'] == ROLES[index-1], 'actual ordered collector role prefix')
        stem = 'phase-'+str(index).zfill(2); label = 'impact-context-'+row['role']
        require(json.loads(read(output/(stem+'.json'), MIB)) == row, 'collector role/phase association')
        for channel, cap in (('stdout', row['stdout_limit']), ('stderr', 256*1024)):
            require(file_identity(output/(stem+'.'+channel+'.bin'), cap) == row[channel], 'raw collector role bytes')
        command = dict(row); command['label'] = label; command['owner'] = 'unchanged in-process e25 collector'
        campaign.receipt['commands'].append(command)
        gate = campaign.receipt['gates'][label]; gate.pop('reason', None)
        gate.update(process_status=row['status'], returncode=row['returncode'], stdout=row['stdout'], stderr=row['stderr'])
        complete = (row['stdout_complete'] and row['stderr_complete'] and row['direct_child_reaped'] and
                    (row['group_kill_issued'] or row.get('group_already_absent', False)))
        gate['status'] = ('pass' if complete and row['status'] == 'ok' and row['returncode'] == 0 and row['stderr']['bytes'] == 0
                          else 'failed' if complete and row['status'] in ('ok','native-error') else 'unknown')
        if gate['status'] != 'pass':
            gate['reason'] = 'collector role incomplete or outside successful exact domain'
    campaign.receipt['collector_unexecuted_roles'] = raw['unexecuted_roles']; campaign.save()

def verify_collector(campaign, collector, output, before, after, compiler, cc, views, pins):
    started = utc(); begin = time.monotonic_ns()
    invocation = None; original = None
    streams = {'stdout':BoundedText(), 'stderr':BoundedText(), 'returncode':None}
    try:
        invocation = invoke_main(collector, [str(root_path()/COLLECTOR_PATH), str(before), str(after),
                                 '--output', str(output), '--compiler', str(compiler), '--cc', str(cc)], streams)
    except BaseException as error:
        original = error
    finally:
        finished, elapsed = utc(), time.monotonic_ns()-begin
        mask = signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGALRM})
        try:
            stdout = streams['stdout'].getvalue().encode('ascii'); stderr = streams['stderr'].getvalue().encode('ascii')
            publish(campaign.output/'collector-wrapper.stdout.bin', stdout, 32768)
            publish(campaign.output/'collector-wrapper.stderr.bin', stderr, 32768)
            campaign.receipt['collector_wrapper'] = {'started_utc': started, 'finished_utc': finished,
              'elapsed_ns': elapsed, 'scope': 'in-process canonical main through argv restoration; overlaps seven roles; excludes wrapper artifact publication',
              'result': None if invocation is None else {'returncode': invocation[0]},
              'stdout':identity(stdout), 'stderr':identity(stderr), 'main_returned':invocation is not None,
              'stdout_complete':not streams['stdout'].exceeded and invocation is not None,
              'stderr_complete':not streams['stderr'].exceeded and invocation is not None}
            collect_roles(campaign, output); campaign.save()
        except BaseException as error:
            campaign.receipt['collector_retention_error'] = (type(error).__name__+': '+str(error))[:4096]
            if original is None:
                original = error
        finally:
            try:
                signal.pthread_sigmask(signal.SIG_SETMASK, mask)
            except BaseException as error:
                campaign.receipt['collector_retention_deadline_error'] = (type(error).__name__+': '+str(error))[:4096]
                if original is None:
                    original = error
    if original is not None:
        raise original
    require(invocation == (0, b'project-impact-context: ok\n', b''), 'fixed successful canonical collector result')
    require(all(campaign.receipt['gates']['impact-context-'+role]['status'] == 'pass' for role in ROLES), 'seven complete actual collector roles')
    for index, side in ((5, 0), (6, 1)):
        require(read(output/('phase-'+str(index).zfill(2)+'.stdout.bin'), 8*MIB) == views[side]['capture'], 'independent literal capture bytes')
    for name, side in (('before', 0), ('after', 1)):
        for kind in ('catalog', 'graph'):
            require(read(output/(name+'-'+kind+'.ns')) == views[side][kind], 'independent serialized catalog/graph bytes')
    report = read(output/'phase-07.stdout.bin', 8*MIB)
    require(report == literal_impact(views), 'independent fixed changed-record/closure/report bytes')
    context = json.loads(read(output/'context.json', 24*MIB)); old, new = views
    candidates = [record_json(name, new) for name in ('ledger', 'ledger_emit', 'ledger_state')]
    impact = {'manifest_changed': False, 'before_project': record_json('@project', old), 'after_project': record_json('@project', new),
              'module_changes': [{'change': 'modified', 'name_hex': b'ledger_state'.hex(),
                                  'before': record_json('ledger_state', old), 'after': record_json('ledger_state', new)}],
              'import_changes': [], 'before_roots_hex': [b'ledger_state'.hex()], 'after_roots_hex': [b'ledger_state'.hex()],
              'before_closure_hex': [row['name_hex'] for row in candidates], 'after_closure_hex': [row['name_hex'] for row in candidates]}
    facts = {'module_change_count': 1, 'import_change_count': 0, 'before_root_count': 1, 'after_root_count': 1,
             'before_closure_count': 3, 'after_closure_count': 3, 'current_candidate_count': 3,
             'current_candidate_catalog_weight_bytes': sum(row['bytes'] for row in candidates)}
    for prefix, view in (('before', old), ('after', new)):
        facts.update({prefix+'_module_count': 9, prefix+'_direct_import_edges': view['edges'],
                      prefix+'_manifest_bytes': view['manifest_bytes'], prefix+'_module_catalog_weight_bytes': view['module_bytes']})
    selection = {'snapshot': 'after', 'catalog_sha256': identity(new['catalog'])['sha256'],
                 'graph_sha256': identity(new['graph'])['sha256'], 'candidates': candidates}
    require(context['format'] == 'slim-project-impact-context-1' and context['impact'] == impact and
            context['facts'] == facts and context['selection'] == selection and context['unknown'] == UNKNOWN, 'independent literal JSON semantic projection')
    evidence = context['evidence']; source_set = {name: pins[name] for name in COLLECTOR_PATHS}
    expected_evidence = {'collector_sha256': COLLECTOR_SHA, 'inventory_adapter_sha256': ADAPTER_SHA,
       'source_set_sha256': identity(canonical(source_set))['sha256'], 'compiler': file_identity(compiler), 'cc': file_identity(cc),
       'tool_arguments_hex': {'compiler': os.fsencode(compiler).hex(), 'cc': os.fsencode(cc).hex()},
       'producer': {'c': file_identity(output/'phase-01.stdout.bin', 16*MIB), 'executable': file_identity(output/'producer')},
       'impact': {'c': file_identity(output/'phase-03.stdout.bin', 16*MIB), 'executable': file_identity(output/'impact')},
       'before_capture': identity(old['capture']), 'after_capture': identity(new['capture']),
       'inputs': {'before_catalog': identity(old['catalog']), 'before_graph': identity(old['graph']),
                  'after_catalog': identity(new['catalog']), 'after_graph': identity(new['graph'])},
       'report': identity(report), 'before_project_argument_hex': os.fsencode(before).hex(),
       'after_project_argument_hex': os.fsencode(after).hex(), 'successful_roles': list(ROLES)}
    require(evidence == expected_evidence and set(context) == {'format','evidence','facts','impact','selection','unknown'}, 'measured evidence only, complete JSON schema')
    require(read(output/'context.json', 24*MIB) == canonical(context), 'canonical complete JSON bytes')
    require(read(output/'sources-before.json', MIB) == read(output/'sources-after.json', MIB) == canonical(source_set), 'collector source endpoints')
    campaign.receipt['workflow_scope'] = {'current_modules': 9, 'selected_current_modules': 3,
       'selected_current_catalog_weight_bytes': facts['current_candidate_catalog_weight_bytes'],
       'all_current_module_bytes': new['module_bytes'], 'impact_report': identity(report),
       'context_json': file_identity(output/'context.json', 24*MIB), 'interpretation': 'deterministic declared-import scope proxy; no saved time or efficacy claim'}
    campaign.save()

def dispatch(campaign, root, output, model, files, captured, prefix, generator, collector, compiler, cc, environment, pins):
    before = snapshot(output, 'before', files)
    expected = snapshot(output, 'expected', files)
    after = snapshot(output, 'after', files, captured)
    entry = generator.construct(prefix, model['states'])
    probe = probe_project(output, 'probe', captured, entry)
    qprojects = [probe_project(output, row['label'], captured, bytes.fromhex(row['source_hex'])) for row in model['diagnostics']]
    trees = {name: directory_pins(output/name) for name in ('before','expected','after','probe','Q01','Q02')}
    campaign.receipt['snapshots_before'] = trees
    campaign.receipt['generated_probe'] = identity(entry)
    views = literal_transport(files, captured)
    for side, view in zip(('before','after'), views):
        for kind in ('capture','catalog','graph'):
            publish(output/'literal-expected'/(side+'-'+kind+'.bin'), view[kind], 8*MIB)
    publish(output/'literal-expected/impact.bin', literal_impact(views), 8*MIB)
    for name in ['baseline/B01.input.bin'] + [group+'/'+row['label']+'.input.bin' for group in ('replays','partials') for row in model[group]]:
        folder_path(files, output, name)
    campaign.receipt['literal_inputs_before'] = directory_pins(output/'literal-inputs')
    campaign.receipt['literal_expected_before'] = directory_pins(output/'literal-expected')
    campaign.save()
    cleanup_control(campaign, environment)
    cfiles = {}
    for label, project, path in (('emit-baseline-ledger', before, output/'baseline.c'),):
        body = successful(campaign, label, [compiler, project], environment, emit=True)
        publish(path, body, 16*MIB); cfiles['baseline'] = path
        campaign.receipt.setdefault('generated_artifacts', {})['baseline.c'] = identity(body)
    def build(label, cpath, binary, flags):
        require(file_identity(cpath, 16*MIB) == campaign.receipt['generated_artifacts'][str(cpath.relative_to(output))],
                'generated C identity before CC consumer')
        successful(campaign, label, [cc, '-std=c11', '-Wall', '-Wextra', '-Werror', *flags,
                    '-I', root/'runtime', '-x', 'c', cpath, root/'runtime/slim_rt.c', '-o', binary], environment)
        require(binary.is_file() and os.access(binary, os.X_OK), 'complete generated executable')
        campaign.receipt.setdefault('generated_artifacts', {})[str(binary.relative_to(output))] = file_identity(binary)
        for name in campaign.receipt['native_artifact_tier']['sidecar_paths']:
            if name.startswith(binary.name+'.dSYM/'):
                campaign.receipt.setdefault('generated_sidecars', {})[name] = file_identity(output/name)
        campaign.save()
    baseline_binary = output/'baseline-ledger'
    build('build-baseline-ledger-ordinary', cfiles['baseline'], baseline_binary, FLAGS[0][1])
    exact(campaign, 'baseline-max-credit-ordinary', [baseline_binary, folder_path(files, output, 'baseline/B01.input.bin')], environment, expected_cli(model['baseline']))
    exact(campaign, 'check-ledger', [compiler, 'check', after], environment, (0,b'',b''))
    exact(campaign, 'check-probe', [compiler, 'check', probe], environment, (0,b'',b''))
    for name, project in (('ledger', after), ('probe', probe)):
        body = successful(campaign, 'emit-'+name, [compiler, project], environment, emit=True)
        cfiles[name] = output/(name+'.c'); publish(cfiles[name], body, 16*MIB)
        campaign.receipt.setdefault('generated_artifacts', {})[name+'.c'] = identity(body)
    programs = {}
    for name in ('ledger','probe'):
        for variant, flags in FLAGS:
            binary = output/(name+'-'+variant); programs[(name,variant)] = binary
            build('build-'+name+'-'+variant, cfiles[name], binary, flags)
    for variant, unused in FLAGS:
        exact(campaign, 'states-'+variant, [programs[('probe',variant)], 'states'], environment, (0,files['states.expected.bin'],b''))
    for n, row in enumerate(model['replays'], 1):
        path = folder_path(files, output, 'replays/'+row['label']+'.input.bin')
        for variant, unused in FLAGS:
            exact(campaign, 'replay-'+str(n).zfill(2)+'-'+variant, [programs[('ledger',variant)], path], environment, expected_cli(row))
    for n, row in enumerate(model['partials'], 1):
        path = folder_path(files, output, 'partials/'+row['label']+'.input.bin')
        for variant, unused in FLAGS:
            argv = [programs[('ledger',variant)], path] if n == 1 else [programs[('probe',variant)], 'partial-total']
            exact(campaign, 'partial-'+str(n).zfill(2)+'-'+variant, argv, environment, expected_cli(row))
    for label, row, project in zip(('diagnostic-positive','diagnostic-missing-partial'), model['diagnostics'], qprojects):
        exact(campaign, label, [compiler,'check',project], environment, expected_cli(row))
    contexts = []
    for selector in model['context_selectors']:
        label = 'context-before-'+selector.split('.')[1]
        gate = campaign.receipt['gates'][label]
        row, out, err = campaign.execute(label, [compiler,'context',before,expected,selector], environment)
        gate.pop('reason', None); gate.update(process_status=row['status'], returncode=row['returncode'])
        complete = row['status'] in ('ok','native-error') and row['returncode'] is not None
        gate['status'] = 'failed' if complete else 'unknown'
        if row['status'] == 'ok' and row['returncode'] == 0 and not err:
            observation = context_observation(out, selector, files['before/'+STATE_PATH])
            contexts.append(observation); gate['status'] = 'pass'
        if gate['status'] != 'pass':
            gate['reason'] = 'checked baseline context incomplete or mismatched'
        campaign.receipt['baseline_contexts'] = contexts; campaign.save(); require(gate['status'] == 'pass', label)
    verify_collector(campaign, collector, output/'impact-context', before, after, compiler, cc, views, pins)
    after_trees = {name: directory_pins(output/name) for name in trees}
    require(after_trees == trees, 'all source snapshot endpoints')
    campaign.receipt['snapshots_after'] = after_trees
    for kind in ('inputs','expected'):
        after_pins = directory_pins(output/('literal-'+kind))
        require(after_pins == campaign.receipt['literal_'+kind+'_before'], 'complete materialized literal endpoints')
        campaign.receipt['literal_'+kind+'_after'] = after_pins
    artifacts = campaign.receipt['generated_artifacts']
    for name, pin in artifacts.items():
        require(file_identity(output/name, 128*MIB) == pin, 'generated native artifact endpoint')

def folder_path(files, output, name):
    path = output/'literal-inputs'/name
    if not path.exists():
        publish(path, files[name])
    require(read(path) == files[name], 'held literal CLI input copy')
    return path

def endpoints(root, folder, args, receipt, pins, tools, controls):
    after = {name:identity(read(root/name)) for name in READSET}
    after_tools = {name:{'path_hex':value['path_hex'], **file_identity(Path(os.fsdecode(bytes.fromhex(value['path_hex']))))}
                   for name,value in tools.items()}
    after_controls = {name:identity(read(root/path)) for name,path in CONTROL_PATHS.items()}
    receipt.update(sources_after=after, tools_after=after_tools, controls_after=after_controls)
    require(after == pins and after_tools == tools and after_controls == {name:identity(data) for name,data in controls.items()},
            'all current source/tool/code/contract endpoints')
    if getattr(args,'model_sha',None) is not None and getattr(args,'freeze_sha',None) is not None:
        held_data(folder, args.model_sha, args.freeze_sha, controls)
        receipt['held_endpoint'] = {'model_sha256':args.model_sha, 'freeze_sha256':args.freeze_sha, 'all128_files_rehashed':True}
    else:
        receipt['held_endpoint'] = {'status':'unknown','reason':'independent fresh data sealing did not complete'}

def native_endpoints(output, receipt, complete):
    """Observe bounded actual files; compare originals, never replace their pins."""
    tier = receipt['native_artifact_tier']
    require(native_artifact_tier() == tier, 'fixed observed native platform tier endpoints')
    inventory = {}; entries = 0; files = 0
    for path in output.rglob('*'):
        entries += 1; require(entries <= 1024, 'fixed native tree entry count')
        require(not path.is_symlink(), 'ordinary native artifact')
        if path.is_file():
            files += 1; require(files <= tier['files_including_receipt'], 'fixed tier native file count cap')
            if path != output/'receipt.json':
                inventory[str(path.relative_to(output))] = file_identity(path)
    receipt['native_files_observed_including_receipt'] = files
    receipt['native_inventory_before_final_receipt'] = inventory
    after_generated = {name:file_identity(output/name) for name in receipt.get('generated_artifacts',{})}
    receipt['generated_artifacts_after'] = after_generated
    require(after_generated == receipt.get('generated_artifacts',{}), 'ORIGINAL generated C/executable endpoints')
    after_sidecars = {name:file_identity(output/name) for name in receipt.get('generated_sidecars',{})}
    receipt['generated_sidecars_after'] = after_sidecars
    require(after_sidecars == receipt.get('generated_sidecars',{}), 'ORIGINAL generated sidecar endpoints')
    for field, directory in (('snapshots',None), ('literal_inputs','literal-inputs'), ('literal_expected','literal-expected')):
        before = receipt.get(field+'_before')
        if before is not None:
            after = ({name:directory_pins(output/name) for name in before} if directory is None else directory_pins(output/directory))
            receipt[field+'_after'] = after
            require(after == before, 'actual '+field+' endpoints')
    for row in receipt['commands']:
        if row.get('owner') == 'unchanged in-process e25 collector':
            stem = 'phase-'+str(ROLES.index(row['role'])+1).zfill(2)
            parent = output/'impact-context'
            channels = {channel:parent/(stem+'.'+channel+'.bin') for channel in ('stdout','stderr')}
        else:
            parent = output/'commands'/row['label']
            channels = {channel:parent/(channel+'.bin') for channel in ('stdout','stderr')}
        for channel,path in channels.items():
            require(file_identity(path) == row[channel], 'original observed native '+channel+' endpoint')
    if complete:
        observed_sidecars = {name for name in inventory if any(part.endswith('.dSYM') for part in Path(name).parts)}
        require(observed_sidecars == set(tier['sidecar_paths']) and
                set(receipt.get('generated_sidecars',{})) == set(tier['sidecar_paths']),
                'exact six Darwin sidecar paths or no-sidecar tier')
        require(files == tier['files_including_receipt'] and len(inventory) == tier['nonself_file_hashes'],
                'complete actual fixed-tier native inventory')
    receipt['native_endpoint_scope'] = 'actual bounded artifact hashes checked against original captured pins; own final receipt write excluded'

def run(args):
    begin = time.monotonic_ns(); started = utc(); root = root_path()
    require(Path(__file__).resolve() == root/CONTROL_PATHS['verifier'], 'canonical current verifier location')
    require(0 < len(os.fsencode(args.output)) <= 4096, 'fixed output operand bound')
    output = Path(args.output).absolute(); base = (root/'build/overnight-ledger-transaction').resolve(strict=True)
    parent = output.parent.resolve(strict=True)
    require((parent == base or base in parent.parents) and not output.exists() and not output.is_symlink(), 'fresh fixed ignored output')
    output = parent/output.name; output.mkdir()
    folder = output.with_name(output.name+'.held')
    require(not folder.exists() and not folder.is_symlink(), 'fresh independent sibling hold')
    receipt = {'schema':1, 'status':'running', 'started_utc':started, 'global_seconds':900, 'child_seconds':60,
      'fixed_data_cases':184, 'native_labels':labels(), 'commands':[],
      'native_artifact_tier':native_artifact_tier(),
      'gates':{name:{'status':'unknown','reason':'not executed'} for name in labels()},
      'unknown':['general workflow efficacy/context sufficiency/model active time/saved compile time',
                 'physical resources/transitive tools/ABA', 'larger malformed caller or aggregate/process safety'],
      'timing_scope':'whole preparation, children, cleanup and publication through final endpoints; own receipt write/shutdown excluded'}
    def save():
        data = canonical(receipt); require(len(data) <= 8*MIB, 'bounded retained receipt')
        (output/'receipt.json').write_bytes(data)
    save(); campaign = None; controls = None; pins = None; tools = None; module_names = []
    def alarm(signum, frame):
        if campaign is not None:
            raise helper.Deadline('fixed whole900 deadline; later gates unknown')
        raise TimeoutError('fixed whole900 preparation deadline')
    remaining = 900-(time.monotonic_ns()-begin)/1000000000
    require(remaining > 0, 'fixed whole preparation deadline')
    old_handler = signal.signal(signal.SIGALRM, alarm); signal.setitimer(signal.ITIMER_REAL, remaining)
    code = 1
    try:
        # Fresh opaque source/control/direct-tool admission precedes even the
        # independent oracle's captured-byte execution. No historical hold is loaded.
        before_captured,before_pins,before_controls,before_tools,compiler,cc = source_admission(root)
        receipt['additional_source_integrity'] = additional_source_integrity()
        pins,controls,tools = before_pins,before_controls,before_tools
        receipt.update(sources_before=before_pins,tools_before=before_tools,
                       controls_before={name:identity(data) for name,data in before_controls.items()})
        save()
        oracle = captured_module(root/CONTROL_PATHS['oracle'],before_controls['oracle'],ORACLE_SHA,
                                 'rfc171_current_independent_oracle'); module_names.append(oracle.__name__)
        require(tuple(oracle.READSET) == READSET and oracle.CONTROL_PATHS == CONTROL_PATHS,
                'independent literal source/control registries')
        generated = oracle.freeze(folder,compiler,cc,
            {'sources':before_pins,'controls':receipt['controls_before'],'tools':before_tools})
        args.model_sha = generated['model']['sha256']; args.freeze_sha = generated['freeze']['sha256']
        receipt['fresh_hold'] = generated
        record, model, files, captured, pins, controls, tools, compiler, cc = admit(root, folder, args.model_sha, args.freeze_sha)
        require(pins == before_pins and controls == before_controls and tools == before_tools,
                'full current admission unchanged through data sealing')
        receipt.update(hold={'model':record['model'],'freeze':file_identity(folder/'freeze.json',MIB),'files':128,
                            'physical_files':128,'logical_artifacts':129,'inline_source_records':1},
                       sources_before=pins, tools_before=tools, controls_before={name:identity(data) for name,data in controls.items()})
        save()
        # All complete independent data, current source, prospective code and
        # direct-tool admissions precede every captured-byte module execution.
        helper = captured_module(root/HELPER_PATH, captured[HELPER_PATH], HELPER_SHA, 'rfc171_fixed_process_helper'); module_names.append(helper.__name__)
        generator = captured_module(root/CONTROL_PATHS['generator'], controls['generator'], GENERATOR_SHA, 'rfc171_fixed_probe_constructor'); module_names.append(generator.__name__)
        collector = captured_module(root/COLLECTOR_PATH, captured[COLLECTOR_PATH], COLLECTOR_SHA, 'rfc171_fixed_collector'); module_names.append(collector.__name__)
        campaign = helper.Campaign(output, receipt, begin)
        environment = dict(os.environ)
        for name in ('SLIM_ALLOC_FAIL_AT','SLIM_TASK_FAIL_AT','SLIM_TASK_JOIN_FAIL_AT','SLIM_TASK_DISABLE'):
            environment.pop(name, None)
        environment.update(ASAN_OPTIONS='detect_leaks=0:halt_on_error=1', UBSAN_OPTIONS='halt_on_error=1')
        dispatch(campaign, root, output, model, files, captured, controls['prefix'], generator, collector, compiler, cc, environment, pins)
        require([row['label'] for row in receipt['commands']] == labels() and len(receipt['commands']) == 46,
                'fixed complete native observation sequence')
        require(all(gate['status'] == 'pass' for gate in receipt['gates'].values()), 'complete finite exact gates')
        require(all(row['direct_child_reaped'] and (row['group_kill_issued'] or row.get('group_already_absent',False))
                    for row in receipt['commands']), 'all46 owned process cleanup/direct reaping')
        endpoints(root, folder, args, receipt, pins, tools, controls)
        native_endpoints(output, receipt, True)
        campaign.cutoff(); receipt['status'] = 'pass'; code = 0
    except BaseException as error:
        receipt.update(status='failed-or-incomplete', error=(type(error).__name__+': '+str(error))[:4096])
    finally:
        mask = signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGALRM})
        try:
            signal.setitimer(signal.ITIMER_REAL, 0)
            while signal.SIGALRM in signal.sigpending():
                signal.sigwait({signal.SIGALRM})
            if controls is not None and pins is not None and tools is not None:
                try:
                    endpoints(root, folder, args, receipt, pins, tools, controls)
                    native_endpoints(output, receipt, code == 0)
                except BaseException as error:
                    code = 1
                    receipt.update(status='failed-or-incomplete', endpoint_error=(type(error).__name__+': '+str(error))[:4096])
            if time.monotonic_ns()-begin > 900*1000000000:
                code = 1; receipt.update(status='failed-or-incomplete', error='fixed whole900 elapsed cap exceeded')
            receipt.update(finished_utc=utc(), elapsed_ns=time.monotonic_ns()-begin,
                           unexecuted_labels=[name for name in labels() if name not in {row['label'] for row in receipt['commands']}])
            save()
            for name in module_names:
                sys.modules.pop(name, None)
            signal.signal(signal.SIGALRM, old_handler)
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK, mask)
    print(json.dumps({'status':receipt['status'],'receipt':file_identity(output/'receipt.json',8*MIB),
                      'elapsed_ns':receipt['elapsed_ns'],'native_observations':len(receipt['commands'])},sort_keys=True))
    return code

def main():
    parser = argparse.ArgumentParser(description=__doc__, allow_abbrev=False)
    parser.add_argument('--current',action='store_true',required=True)
    parser.add_argument('--output',required=True)
    return run(parser.parse_args())

if __name__ == '__main__':
    sys.dont_write_bytecode = True
    raise SystemExit(main())
