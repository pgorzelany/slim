#!/usr/bin/env python3
"""Fixed six-case RFC165 full-project node/source boundary verification.

Freeze is data only; run opts into the held 46-child campaign. Literal bytes and
transport measurements are independent of SLIM semantics. The production prepare
API observes counts; its checker alone accepts source. No source parser, retry,
budget knob, recorded executable or compiler/API modification exists.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import shutil
import signal
import sys
import time
from types import ModuleType

MIB = 1048576
GLOBAL_SECONDS = 900
CHILD_SECONDS = 60
MODEL_CAP = 4*MIB
FREEZE_CAP = 64*MIB
FILE_COUNT_CAP = 128
HELPER_SHA = '6e412c54726f23392003dfd82c20c159743a58ea6506c00274577f59dfb1b1be'
VARIANTS = (('ordinary',('-O2','-DNDEBUG')),
            ('sanitized',('-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all')))
PRODUCER_MANIFEST_SHA = 'b31b63bab95ddac1d49e719e3537c0f202031347e794c58f7b07b7196e1a9f3f'
PROBE_PATH = 'tests/fixtures/project_input_node_observer.slim'
RFC_PATH = 'design/rfcs/0165-checked-project-input-producer.md'
COUNTER_CAP = 1000000000
NODE_CASES = (
 ('nodes-minus',512,110484,999999,1565218,(499150,500853),218),
 ('nodes-exact',508,110489,1000000,1565144,(499479,500525),219),
 ('nodes-plus',513,110483,1000001,1565240,(500835,499170),217),
)
SOURCE_CASES = (('sources-minus',-1),('sources-exact',0),('sources-plus',1))
CASES = ('nodes-minus','nodes-exact','nodes-plus','sources-minus','sources-exact','sources-plus')
SUCCESSFUL = frozenset(('nodes-minus','nodes-exact','sources-minus','sources-exact'))
# Returned fields are expectations, never observations or an independent parse.
OBSERVATIONS = {
 'nodes-minus':(0,999999,0,0,1,0), 'nodes-exact':(0,1000000,0,0,1,0),
 'nodes-plus':(65,1000001,1,65,1,0), 'sources-minus':(0,66,0,0,1,0),
 'sources-exact':(0,66,0,0,1,0), 'sources-plus':(65,0,0,0,0,10),
}
SOURCE_PATHS = (
 'selfhost/check.slim','selfhost/codegen.slim','selfhost/control.slim',
 'selfhost/effects.slim','selfhost/format.slim','selfhost/identity.slim',
 'selfhost/ir.slim','selfhost/memory.slim','selfhost/ownership.slim',
 'selfhost/parallel.slim','selfhost/project.slim',
 'library/applications/project_input/main.slim',
 'library/components/project_input_data.slim','library/components/project_input_emit.slim',
 'library/components/project_input_limits.slim','library/components/project_input_model.slim',
 'selfhost/ranges.slim','selfhost/retained.slim','selfhost/scheduler.slim',
 'library/experimental/ascii.slim','library/experimental/bytes.slim',
 'library/experimental/netstring.slim','library/experimental/text.slim',
 'selfhost/syntax.slim','selfhost/text.slim','selfhost/typing.slim','selfhost/validate.slim',
)
APP_ROW = b'  (module project_input_app "library/applications/project_input/main.slim" (imports identity project project_input_data project_input_emit project_input_limits project_input_model retained syntax) (exports))\n'
OBSERVER_ROW = b'  (module project_input_app "observer.slim" (imports identity project project_input_data project_input_emit project_input_limits retained std_text syntax) (exports))\n'
NODE_MANIFEST = (b'(project 1\n  (entry m0001)\n'
            b'  (module m0000 "m0000.slim" (imports) (exports))\n'
            b'  (module m0001 "m0001.slim" (imports) (exports))\n)\n')
SOURCE_MANIFEST = (b'(project 1\n  (entry m0003)\n'
 b'  (module m0000 "m0000.slim" (imports) (exports))\n'
 b'  (module m0001 "m0001.slim" (imports) (exports))\n'
 b'  (module m0002 "m0002.slim" (imports) (exports))\n'
 b'  (module m0003 "m0003.slim" (imports) (exports))\n)\n')
CLEANUP = (b'import os,sys,time\nchild=os.fork()\nif child==0:\n'
           b' os.close(0);os.close(1);os.close(2);time.sleep(120);os._exit(0)\nprint(child,flush=True)\n')


def root():
    for candidate in Path(__file__).resolve().parents:
        if (candidate/'design/FEATURE_POLICY.md').is_file():
            return candidate
    raise ValueError('repository root unavailable')


def require(condition, reason):
    if not condition:
        raise ValueError(reason)


def read(path, cap=128*MIB):
    path = Path(path)
    require(path.is_file() and not path.is_symlink(),'pinned ordinary file: '+str(path))
    with path.open('rb') as stream:
        value = stream.read(cap+1)
    require(len(value)<=cap,'fixed file bound: '+str(path))
    return value


def sha(value):
    return hashlib.sha256(value).hexdigest()


def identity(value):
    return {'bytes':len(value),'sha256':sha(value)}


def encoded(value):
    return (json.dumps(value,sort_keys=True,separators=(',',':'),ensure_ascii=True)+'\n').encode('ascii')


def fresh_write(path, value):
    path.parent.mkdir(parents=True,exist_ok=True)
    with path.open('xb') as stream:
        stream.write(value)
    require(read(path)==value,'fresh artifact readback')


def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds').replace('+00:00','Z')


def pins(repository):
    # Current opaque bodies are pinned per fresh campaign. Historical study
    # sources/receipts are evidence only and never a future admission condition.
    paths = {Path(__file__).resolve(),repository/PROBE_PATH,
             repository/'project-input.project',repository/'selfhost/slim.project',
             repository/'scripts/project-input-inventory.py',repository/'scripts/verify-project-impact.py',
             repository/'runtime/slim_rt.c',repository/'runtime/slim_rt.h',repository/RFC_PATH}
    paths.update(repository/path for path in SOURCE_PATHS)
    paths.update(repository.glob('selfhost/*.slim'))
    return {str(path.relative_to(repository)):identity(read(path,4*MIB)) for path in sorted(paths)}


def tool_pins(repository):
    cc = Path(shutil.which('cc') or '/usr/bin/cc').resolve(strict=True)
    return {name:{'path':str(path),**identity(read(path))} for name,path in
            (('compiler',(repository/'build/toolchain/slimc').resolve(strict=True)),
             ('cc',cc),('python',Path(sys.executable).resolve(strict=True)))}


def file_pins(folder):
    return {str(path.relative_to(folder)):identity(read(path)) for path in sorted(folder.rglob('*')) if path.is_file()}


def load_captured(path, expected, name):
    captured = read(path,4*MIB)
    require(sha(captured)==expected,'helper source pin before load')
    result = ModuleType(name)
    result.__file__ = str(path)
    sys.modules[name] = result
    exec(compile(captured,str(path),'exec',dont_inherit=True),result.__dict__)
    require(sha(read(path,4*MIB))==expected,'helper source pin after load')
    return result


def frame(value):
    return str(len(value)).encode('ascii')+b':'+value+b','


def triples(rows):
    return b''.join(frame(field) for row in rows for field in row)


def prerequisite(repository):
    raw = read(repository/RFC_PATH,MIB)
    require(b'Status: accepted' in raw.splitlines()[:24],'accepted RFC165 required')
    manifest = read(repository/'project-input.project',MIB)
    require(sha(manifest)==PRODUCER_MANIFEST_SHA,'fixed producer manifest declaration bytes')
    require(len(SOURCE_PATHS)==27 and len(set(SOURCE_PATHS))==27,'fixed producer source registry')
    return {'rfc_source':identity(raw),'manifest':identity(manifest),
            'scope':'accepted RFC165; fixed restricted constructor; current opaque tooling bytes',
            'constructor_argument':
              'Two local pure Void p functions; F includes both p and entry main; '
              'F-3 distinct fNNNN functions each contain <=219 nonterminal p() calls. '
              'The selected canonical syntax emits 11 nodes per no-argument function, '
              '7 extra main Vec[Bytes] parameter nodes, 9 per nonterminal call statement; '
              'flattening replaces local headers by one global4. Hence4+11F+7+9K. '
              'The literal canonical formatter and eight-byte private namespace extent '
              'give36F+14K+10 flattened bytes. These restricted constructor expectations '
              'never substitute for the current existing prepare API observations or '
              'normal production checker acceptance. Counts are reobserved wherever '
              'prepare runs; source-plus is declined beforehand and remains unknown.'}


def literal_case(name):
    # Fixed constructor bytes; no tokenization or source interpretation.
    for label,functions,calls,target,flat_bytes,local_targets,largest in NODE_CASES:
        if name != label:
            continue
        filled = functions-3
        quotient,remainder = divmod(calls,filled)
        cut = filled//2
        counts = [quotient+(ordinal<remainder) for ordinal in range(filled)]
        require(sum(counts)==calls and max(counts)==largest and largest<=219,'fixed complete function/call geometry')
        parts = [[b'module m0000\n\nfn p() -> Void:\n  void\n'],
                 [b'module m0001\n\nfn p() -> Void:\n  void\n']]
        for ordinal,count in enumerate(counts):
            function = b'\nfn f'+str(ordinal).zfill(4).encode()+b'() -> Void:\n'+b'  p()\n'*count+b'  void\n'
            parts[0 if ordinal<cut else 1].append(function)
        parts[1].append(b'\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
        rows = tuple((('m'+str(i).zfill(4)).encode(),('m'+str(i).zfill(4)+'.slim').encode(),b''.join(items))
                     for i,items in enumerate(parts))
        local = (4+11*(cut+1)+9*sum(counts[:cut]),4+11*(filled-cut+2)+7+9*sum(counts[cut:]))
        require(local==local_targets and sum(local)==target+4 and all(value<1000000 for value in local),'restricted local count argument')
        require(4+11*functions+7+9*calls==target and 36*functions+14*calls+10==flat_bytes,
                'accepted restricted count/byte argument')
        require(sum(len(row[2]) for row in rows)==28*functions+6*calls+29,'literal local source extent')
        return NODE_MANIFEST,rows,{'family':'fixed two-module pure Void-call constructor','functions':functions,
            'nonterminal_calls':calls,'maximum_calls_per_function':largest,'filled_functions':filled,
            'local_node_hypotheses':local,'flat_bytes_hypothesis':flat_bytes,'node_hypothesis':target,
            'authority':'constructor argument only; actual canonical count must come from prepare API'}
    for label,delta in SOURCE_CASES:
        if name != label:
            continue
        rows=[]
        lengths=(MIB,MIB,MIB,MIB-len(SOURCE_MANIFEST)+delta)
        for i,length in enumerate(lengths):
            key=('m'+str(i).zfill(4)).encode()
            data=b'module '+key+b'\n\nfn p() -> Void:\n  void\n'
            if i==3:
                data+=b'\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
            require(len(data)<=length<=MIB,'fixed trailing-LF payload extent')
            data+=b'\n'*(length-len(data))
            rows.append((key,key+b'.slim',data))
        require(len(SOURCE_MANIFEST)+sum(len(row[2]) for row in rows)==4*MIB+delta,'exact aggregate source crossing')
        return SOURCE_MANIFEST,tuple(rows),{'family':'four flat private-p modules plus entry main and trailing LF',
            'functions':5,'nonterminal_calls':0,'source_delta':delta,'node_hypothesis':66,
            'authority':'constructor argument only; whitespace extent is not a node observation'}
    raise ValueError('not one of six fixed cases')


def byte_oracle(manifest,rows,adapter):
    graph = triples((name,str(len(source)).encode(),b'') for name,path,source in rows)
    catalog = triples([(b'@project',str(len(manifest)).encode(),b'slim.project\0'+sha(manifest).encode())]+
                      [(name,str(len(source)).encode(),path+b'\0'+sha(source).encode()) for name,path,source in rows])
    transport = b''.join(frame(field) for field in (b'slim-project-input-1',str(len(rows)).encode(),b'0',manifest,graph))+triples(rows)
    metadata = {'schema':1,'format':'slim-project-input-1','module_count':len(rows),'direct_import_edges':0,
                'source_bytes':len(manifest)+sum(len(row[2]) for row in rows),'manifest':identity(manifest),
                'modules':[{'name':name.decode(),'path_hex':path.hex(),**identity(source)} for name,path,source in rows]}
    receipt = {**metadata,'adapter':adapter,'capture':identity(transport),'catalog':identity(catalog),'workplan':identity(graph),
               'authority':'transport measurements only; source acceptance requires matching successful trusted producer invocation',
               'consumer_acceptance':'not established; existing catalog/workplan consumers remain authoritative',
               'source_identity_scope':'observed adapter source bytes through receipt assembly/publication; no loaded bytecode or ABA attestation',
               'capture_scope':'single bounded read of serialized bytes; no source-path reread or atomic live capture claim'}
    return {'transport.bin':transport,'catalog.bin':catalog,'graph.bin':graph,'inventory.json':encoded(receipt)}


def labels():
    names = ['group-cleanup-control','observer-format']
    for program in ('observer','producer'):
        names.extend((program+'-check',program+'-emit'))
        names.extend(program+'-'+variant+'-build' for variant,flags in VARIANTS)
    names.extend('control-'+name for name in CASES)
    names.extend('format-'+name+'-'+str(ordinal) for name,*unused in NODE_CASES for ordinal in (0,1))
    for variant,flags in VARIANTS:
        for name in CASES:
            names.extend(('observe-'+variant+'-'+name,'capture-'+variant+'-'+name))
    require(len(names)==46 and len(set(names))==46,'fixed native label cardinality')
    return names


def bounds():
    return {'global_seconds':900,'child_seconds':60,'stdout_bytes':8*MIB,'stderr_bytes':256*1024,
            'emit_stdout_bytes':16*MIB,'child_file_bytes':128*MIB,'retries':0,
            'fixture_cases':6,'native_labels':46,'successful_inventory_conversions':8,
            'model_bytes':MODEL_CAP,'freeze_bytes':FREEZE_CAP,'freeze_files':FILE_COUNT_CAP,
            'canonical_nodes_limit':1000000,'source_bytes_limit':4*MIB,'payload_bytes_limit':MIB,
            'observer_numeric_field_cap':COUNTER_CAP,
            'observer_cap_scope':'numeric observation only; never the production node-admission limit'}


def freeze(repository,output):
    started,begin = utc(),time.monotonic_ns()
    prerequisite_record=prerequisite(repository)
    before,tools=pins(repository),tool_pins(repository)
    producer_manifest=read(repository/'project-input.project',MIB)
    require(sha(producer_manifest)==PRODUCER_MANIFEST_SHA,'literal producer manifest differs')
    require(prerequisite_record['rfc_source']==before[RFC_PATH],'accepted RFC source drift before freeze')
    require(producer_manifest.count(APP_ROW)==1,'literal app declaration exactly once')
    observer_manifest=producer_manifest.replace(APP_ROW,OBSERVER_ROW,1)
    observer=read(repository/PROBE_PATH,MIB)
    require(before['scripts/verify-project-impact.py']['sha256']==HELPER_SHA,'fixed captured process helper')
    files={'programs/producer/slim.project':producer_manifest,'programs/observer/slim.project':observer_manifest,
           'programs/observer/observer.slim':observer,'cleanup-control.py':CLEANUP,
           'accepted-rfc165.md':read(repository/RFC_PATH,MIB)}
    for path in SOURCE_PATHS:
        value=read(repository/path,MIB)
        require(identity(value)==before[path],'bounded opaque source body pin')
        files['programs/producer/'+path]=value
        if path!='library/applications/project_input/main.slim':
            files['programs/observer/'+path]=value
    dimensions=[]
    caps={'modules':4095,'edges':65536,'name_bytes':64,'path_bytes':256,'payload_bytes':MIB,
          'source_bytes':4*MIB,'catalog_bytes':MIB,'workplan_bytes':MIB,'transport_bytes':8*MIB,'adapter_receipt_bytes':4*MIB}
    for name in CASES:
        manifest,rows,constructor=literal_case(name)
        byte_values=byte_oracle(manifest,rows,before['scripts/project-input-inventory.py'])
        operands={'modules':len(rows),'edges':0,'name_bytes':max(len(row[0]) for row in rows),
            'path_bytes':max(len(row[1]) for row in rows),'payload_bytes':max(len(manifest),*(len(row[2]) for row in rows),len(byte_values['graph.bin'])),
            'source_bytes':len(manifest)+sum(len(row[2]) for row in rows),'catalog_bytes':len(byte_values['catalog.bin']),
            'workplan_bytes':len(byte_values['graph.bin']),'transport_bytes':len(byte_values['transport.bin']),
            'adapter_receipt_bytes':len(byte_values['inventory.json'])}
        require(all(type(value) is int and 0<=value for value in operands.values()),'materialized exact nonnegative operands')
        excess=[key for key,value in operands.items() if value>caps[key]]
        require(excess==(['source_bytes'] if name=='sources-plus' else []),'all other independent admission domains')
        require(name!='sources-plus' or operands['source_bytes']==caps['source_bytes']+1,'aggregate single excess')
        require(len(set(row[0] for row in rows))==len(rows) and len(set(row[1] for row in rows))==len(rows)
                and all(rows[i][0]<rows[i+1][0] for i in range(len(rows)-1)),'literal unique ordered names/paths')
        for key,path,value in rows:
            files['cases/'+name+'/'+path.decode()]=value
        files['cases/'+name+'/slim.project']=manifest
        for suffix,value in byte_values.items():
            files['expected/'+name+'/'+suffix]=value
        expected=OBSERVATIONS[name]
        files['expected/'+name+'/observer.bin']=(' '.join(map(str,expected[1:]))+'\n').encode()
        checker_stdout=b'E0102@0:1565240\n' if name=='nodes-plus' else b''
        files['expected/'+name+'/checker.bin']=checker_stdout
        files['expected/'+name+'/capture.bin']=byte_values['transport.bin'] if name in SUCCESSFUL else b'error 10 at 0\n'
        dimensions.append({'name':name,'data_operands':operands,'caps':caps,'intentional_excess':excess,
            'constructor':constructor,'canonical_nodes':{'classification':'unknown','value':None,
                 'reason':'no actual large prepare API observation'},
            'expected_observer':{'exit':expected[0],'fields':list(expected[1:])},
            'expected_checker':{'exit':1 if name=='nodes-plus' else 0,'stdout':identity(checker_stdout),'stderr':identity(b'')},
            'expected_capture':{'exit':0 if name in SUCCESSFUL else 65,'stdout':identity(files['expected/'+name+'/capture.bin']),'stderr':identity(b'')},
            'inventory_expected':name in SUCCESSFUL,
            'unemitted_oracle_scope':'negative-case transport/catalog/graph/receipt are byte dimensions only, never claimed emitted or adapter-accepted'})
    model={'schema':1,'rfc':165,'scope':'six fixed full-project node/source boundary cases only',
        'source_before':before,'tools':tools,'bounds':bounds(),'native_labels':labels(),'cases':dimensions,
        'prerequisite':prerequisite_record,
        'unknown':{'nodes':'actual canonical counts remain unknown until completed production prepare observations',
            'source_acceptance':'independent bytes never establish source acceptance',
            'generalization':'this restricted family is not a theorem for arbitrary SLIM',
            'resources':'logical/live caps do not bound RSS, libc allocation or universal CPU',
            'capture':'stopped writers and before/after pins do not prove atomic capture or exclude ABA',
            'toolchain':'direct executable hashes do not attest transitive SDK/loaded code identity'}}
    files['model.json']=encoded(model)
    require(len(files['model.json'])<=MODEL_CAP and len(files)+1<=FILE_COUNT_CAP
            and sum(map(len,files.values()))<=FREEZE_CAP-MODEL_CAP,'complete finite data preflight with receipt reserve')
    require(pins(repository)==before and tool_pins(repository)==tools,'source/tool drift before publication')
    output.mkdir(parents=True,exist_ok=False)
    for name,value in files.items():
        fresh_write(output/name,value)
    require(pins(repository)==before and tool_pins(repository)==tools,'source/tool drift after data publication')
    record={'schema':1,'rfc':165,'data_only':True,'native_invocations':0,'bounds':bounds(),
        'started_utc':started,'finished_utc':utc(),'elapsed_ns':time.monotonic_ns()-begin,
        'timing_scope':'same-process data preparation/publication through source/tool checks; freeze receipt publication excluded',
        'source_pins':before,'tools':tools,'model_sha256':sha(files['model.json']),
        'files':{name:identity(value) for name,value in sorted(files.items())},'publication_drift_checks':True}
    frozen=encoded(record)
    require(len(frozen)<=MODEL_CAP and sum(map(len,files.values()))+len(frozen)<=FREEZE_CAP,'final complete materialized cap')
    fresh_write(output/'freeze.json',frozen)
    require(file_pins(output)==record['files']|{'freeze.json':identity(frozen)},'complete held data identity')
    require(pins(repository)==before and tool_pins(repository)==tools,'source/tool drift after freeze receipt')
    print(json.dumps({'data_only':True,'native_invocations':0,'model':identity(files['model.json']),
                     'freeze':identity(frozen),'files':len(files)+1,'materialized_bytes':sum(map(len,files.values()))+len(frozen),
                     'cases':dimensions},sort_keys=True))


def observation(raw,gate):
    unknown={'classification':'unknown','canonical_nodes':None,'reason':'no complete canonical five-field observer receipt'}
    if gate.get('process_status') not in ('ok','native-error') or len(raw)>128 or not raw.endswith(b'\n'):
        return unknown
    fields=raw[:-1].split(b' ')
    if len(fields)!=5 or not all(value.isdigit() and len(value)<=20 for value in fields):
        return unknown
    count,capacity,status,called,admission=map(int,fields)
    if not (0<=count<=COUNTER_CAP and capacity in (0,1) and 0<=status<=COUNTER_CAP
            and called in (0,1) and 0<=admission<=COUNTER_CAP
            and raw==f'{count} {capacity} {status} {called} {admission}\n'.encode()):
        return unknown
    if called:
        if admission:
            return unknown
        value={'classification':'exact','canonical_nodes':count,'capacity':bool(capacity),'prepared_status':status,
            'prepare_called':True,'admission_code':0,'authority':'actual existing prepare_project_input attempt fields, no reparse'}
        if count==0 and status:
            value['whole_project_count']={'classification':'unknown','reason':'preparation returned before complete flatten count'}
        return value
    if (count,capacity,status)!=(0,0,0) or admission!=10:
        return unknown
    return {'classification':'unknown','canonical_nodes':None,'prepared_status':None,'capacity':None,
            'prepare_called':False,'admission_code':admission,'admission_classification':'exact',
            'raw_zero_fields':'reserved sentinels only, never measured count/status',
            'reason':'source admission prevented prepare'}


def verify_held(repository, folder, model_sha, freeze_sha):
    require(folder.is_dir() and not folder.is_symlink(),'held ordinary directory')
    raw_model,raw_freeze = read(folder/'model.json',MODEL_CAP),read(folder/'freeze.json',MODEL_CAP)
    require(sha(raw_model)==model_sha and sha(raw_freeze)==freeze_sha,'explicit complete model/freeze identity')
    model,record = json.loads(raw_model),json.loads(raw_freeze)
    require(record['model_sha256']==model_sha and record['data_only'] is True and record['native_invocations']==0,'data-only model association')
    require(model['bounds']==record['bounds']==bounds() and model['native_labels']==labels(),'unchanged fixed campaign bounds')
    require(len(record['files'])+1<=FILE_COUNT_CAP and sum(item['bytes'] for item in record['files'].values())+len(raw_freeze)<=FREEZE_CAP,'held finite inventory')
    for name,expected in record['files'].items():
        path = Path(name)
        require(not path.is_absolute() and '..' not in path.parts,'held relative data path')
        require(identity(read(folder/path))==expected,'held artifact pin: '+name)
    require(file_pins(folder)==record['files']|{'freeze.json':identity(raw_freeze)},'held complete inventory')
    require(model['source_before']==record['source_pins']==pins(repository),'held source readset changed before native')
    require(model['tools']==record['tools']==tool_pins(repository),'held tool readset changed before native')
    require([case['name'] for case in model['cases']]==list(CASES),'fixed case identity')
    require(model['prerequisite']==prerequisite(repository),'same accepted RFC/current manifest constructor prerequisite')
    return model,record,file_pins(folder)


def run(repository, output, held, model_sha, freeze_sha):
    started,begin = utc(),time.monotonic_ns()
    output.mkdir(parents=True,exist_ok=False)
    receipt = {'schema':1,'rfc':165,'scope':'six fixed full-project node/source boundary cases only','status':'running','started_utc':started,
               'bounds':bounds(),'commands':[],'gates':{label:{'status':'unknown','reason':'not executed'} for label in labels()},
               'observations':{},
               'boundaries':{name:{'status':'unknown','reason':'not observed'} for name in CASES},
               'unknown':{'generalization':'finite restricted fixtures do not prove arbitrary SLIM node-count geometry',
                          'physical_resources':'no RSS or universal CPU bound claim'},
               'timing_scope':'same-process whole native wrapper through final source/tool/artifact pins; final receipt publication/shutdown excluded'}
    helper = None
    def save():
        (output/'receipt.json').write_bytes(encoded(receipt))
    def expired(_signum,_frame):
        if helper is None:
            raise ValueError('fixed whole campaign alarm before process helper loaded')
        raise helper.Deadline('fixed 900-second full-project boundary alarm')
    prior = signal.signal(signal.SIGALRM,expired)
    signal.setitimer(signal.ITIMER_REAL,GLOBAL_SECONDS)
    save()
    try:
        model,record,held_before = verify_held(repository,held,model_sha,freeze_sha)
        receipt.update(source_before=pins(repository),tools_before=tool_pins(repository),held_before=held_before,
                       held_model_sha256=model_sha,held_freeze_sha256=freeze_sha)
        helper = load_captured(repository/'scripts/verify-project-impact.py',HELPER_SHA,'node_boundaries_fixed_process_helper')
        campaign = helper.Campaign(output,receipt,begin)
        adapter_pin = receipt['source_before']['scripts/project-input-inventory.py']['sha256']
        adapter = load_captured(repository/'scripts/project-input-inventory.py',adapter_pin,'node_boundaries_fixed_inventory_adapter')
        environment = dict(os.environ,LC_ALL='C',LANG='C',ASAN_OPTIONS='detect_leaks=0:abort_on_error=1',UBSAN_OPTIONS='halt_on_error=1')
        for key in ('SLIM_ALLOC_FAIL_AT','SLIM_TASK_FAIL_AT','SLIM_TASK_JOIN_FAIL_AT','SLIM_TASK_DISABLE'):
            environment.pop(key,None)
        compiler,cc = (Path(receipt['tools_before'][key]['path']) for key in ('compiler','cc'))
        plan = {'bounds':bounds(),'native_labels':labels(),'source_pins':receipt['source_before'],'tools':receipt['tools_before'],
                'held_model_sha256':model_sha,'held_freeze_sha256':freeze_sha,'held_files':held_before}
        fresh_write(output/'campaign-before-native.json',encoded(plan))
        receipt['plan_before'] = identity(read(output/'campaign-before-native.json'))
        require(pins(repository)==receipt['source_before'] and tool_pins(repository)==receipt['tools_before'] and file_pins(held)==held_before,'dispatch source/tool/data drift')
        helper.check_cleanup(campaign,held/'cleanup-control.py',environment)
        campaign.exact('observer-format',[compiler,'fmt',held/'programs/observer/observer.slim'],environment,(0,read(held/'programs/observer/observer.slim'),b''))
        programs = {}
        receipt['generated_c'],receipt['native_programs'] = {},{}
        for program in ('observer','producer'):
            manifest = held/'programs'/program/'slim.project'
            gate,unused = campaign.exact(program+'-check',[compiler,'check',manifest],environment,(0,b'',b''))
            require(gate['status']=='pass','production checker did not accept '+program)
            result,generated,error = campaign.execute(program+'-emit',[compiler,manifest],environment,cap=16*MIB)
            gate = receipt['gates'][program+'-emit'];gate.pop('reason',None)
            gate.update(process_status=result['status'],returncode=result['returncode'],stdout=identity(generated),stderr=identity(error))
            if result['status'] not in ('ok','native-error') or result['returncode'] is None:
                gate.update(status='unknown',reason='bounded emitter invocation incomplete')
            elif result['returncode']==0 and generated and not error:
                gate['status'] = 'pass'
            else:
                gate.update(status='failed',reason='completed production emitter did not emit nonempty C with empty stderr')
            campaign.save()
            require(gate['status']=='pass','production emitter did not emit '+program)
            source = output/(program+'.c');fresh_write(source,generated)
            receipt['generated_c'][program] = {'path':str(source),**identity(generated)}
            campaign.save()
            for variant,flags in VARIANTS:
                executable = output/(program+'-'+variant)
                argv = [cc,'-std=c11','-Wall','-Wextra','-Werror',*flags,'-I'+str(repository/'runtime'),source,repository/'runtime/slim_rt.c','-o',executable]
                gate,unused = campaign.exact(program+'-'+variant+'-build',argv,environment,(0,b'',b''))
                require(gate['status']=='pass','native build failed '+program+'/'+variant)
                programs[(program,variant)] = executable
                receipt['native_programs'][program+'-'+variant] = {'path':str(executable),**identity(read(executable))}
                campaign.save()
        for name in CASES:
            campaign.exact('control-'+name,[compiler,'check',held/'cases'/name/'slim.project'],environment,
                (1 if name=='nodes-plus' else 0,read(held/'expected'/name/'checker.bin'),b''))
        for name,*unused in NODE_CASES:
            for ordinal in (0,1):
                module=held/'cases'/name/('m'+str(ordinal).zfill(4)+'.slim')
                campaign.exact('format-'+name+'-'+str(ordinal),[compiler,'fmt',module],environment,(0,read(module),b''))
        receipt['inventory_artifacts']={}
        inventory_conversions=0
        for variant,flags in VARIANTS:
            for name in CASES:
                manifest=held/'cases'/name/'slim.project'
                expected=OBSERVATIONS[name]
                gate,raw=campaign.exact('observe-'+variant+'-'+name,[programs[('observer',variant)],manifest],environment,
                    (expected[0],read(held/'expected'/name/'observer.bin'),b''))
                receipt['observations'][variant+'-'+name]=observation(raw,gate)
                gate,raw=campaign.exact('capture-'+variant+'-'+name,[programs[('producer',variant)],manifest],environment,
                    (0 if name in SUCCESSFUL else 65,read(held/'expected'/name/'capture.bin'),b''))
                if name in SUCCESSFUL and gate['status']=='pass':
                    destination=output/'inventory'/(variant+'-'+name)
                    capture=destination/'capture.bin';fresh_write(capture,raw)
                    try:
                        adapter.convert(capture,destination/'catalog.bin',destination/'graph.bin',destination/'inventory.json')
                        for suffix in ('catalog.bin','graph.bin','inventory.json'):
                            require(read(destination/suffix)==read(held/'expected'/name/suffix),'independent supplied-byte inventory oracle mismatch')
                        for path in sorted(destination.iterdir()):
                            receipt['inventory_artifacts'][str(path.relative_to(output))]=identity(read(path))
                        inventory_conversions+=1
                        gate['authority']='matching successful production checker producer; adapter hashes only supplied serialized bytes'
                    except Exception as error:
                        gate.update(status='failed',reason=helper.summary(error))
                campaign.save()
        receipt['inventory_conversions']=inventory_conversions
        for name in CASES:
            labels_for_case=['control-'+name]+[prefix+'-'+variant+'-'+name for variant,flags in VARIANTS for prefix in ('observe','capture')]
            if name.startswith('nodes-'):
                labels_for_case+=['format-'+name+'-'+str(i) for i in (0,1)]
            states=[receipt['gates'][label]['status'] for label in labels_for_case]
            receipt['boundaries'][name]={'status':'pass' if all(state=='pass' for state in states)
                else 'failed' if 'failed' in states else 'unknown','gates':labels_for_case,
                'scope':'fixed supplied source family and complete paired source-admission/prepare/producer observations only'}
            if name=='sources-plus':
                receipt['boundaries'][name]['canonical_nodes']='unknown: source admission prevented prepare'
        receipt['status']='pass' if all(gate['status']=='pass' for gate in receipt['gates'].values()) and inventory_conversions==8 else 'failed'
    except Exception as error:
        receipt.update(status='timeout' if helper is not None and isinstance(error,helper.Deadline) else 'failed',failure=str(error)[:4096])
    finally:
        try:
            receipt['source_after'],receipt['tools_after'] = pins(repository),tool_pins(repository)
            receipt['held_after'] = file_pins(held)
            unchanged = receipt.get('source_before')==receipt['source_after'] and receipt.get('tools_before')==receipt['tools_after'] and receipt.get('held_before')==receipt['held_after']
            for key in ('generated_c','native_programs'):
                if key in receipt:
                    receipt[key+'_after'] = {name:{'path':row['path'],**identity(read(row['path']))} for name,row in receipt[key].items()}
                    unchanged = unchanged and receipt[key]==receipt[key+'_after']
            if 'inventory_artifacts' in receipt:
                receipt['inventory_artifacts_after'] = {name:identity(read(output/name)) for name in receipt['inventory_artifacts']}
                unchanged = unchanged and receipt['inventory_artifacts']==receipt['inventory_artifacts_after']
            if 'plan_before' in receipt:
                receipt['plan_after'] = identity(read(output/'campaign-before-native.json'))
                unchanged = unchanged and receipt['plan_before']==receipt['plan_after']
            receipt['pins_unchanged'] = unchanged
            if not unchanged:
                receipt.update(status='failed',pin_failure='source/tools/held/generated/native/inventory/plan drift')
        except Exception as error:
            receipt.update(status='failed',pin_failure=str(error)[:4096])
        finally:
            signal.setitimer(signal.ITIMER_REAL,0)
            signal.signal(signal.SIGALRM,prior)
        receipt.update(finished_utc=utc(),elapsed_ns=time.monotonic_ns()-begin)
        if receipt['elapsed_ns']>GLOBAL_SECONDS*1000000000:
            receipt.update(status='timeout',failure='fixed900s whole campaign exceeded')
        save()
    print('project node/source boundaries: '+receipt['status'].upper()+'; retained '+str(output/'receipt.json'))
    return 0 if receipt['status']=='pass' else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('mode',choices=('freeze','run'))
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--held',type=Path)
    parser.add_argument('--model-sha')
    parser.add_argument('--freeze-sha')
    args = parser.parse_args()
    repository = root()
    allowed = (repository/'build/overnight-project-input-node-boundaries').resolve()
    output = args.output.resolve()
    require(output.is_relative_to(allowed) and output!=allowed,'fresh ignored output descendant')
    if args.mode=='freeze':
        require(args.held is None and args.model_sha is None and args.freeze_sha is None,'freeze has no native/held options')
        def expired(_signum,_frame):
            raise ValueError('fixed 900-second data-only materialization alarm')
        prior=signal.signal(signal.SIGALRM,expired)
        signal.setitimer(signal.ITIMER_REAL,GLOBAL_SECONDS)
        try:
            freeze(repository,output)
        finally:
            signal.setitimer(signal.ITIMER_REAL,0)
            signal.signal(signal.SIGALRM,prior)
        return 0
    require(args.held is not None and args.model_sha and args.freeze_sha,'run requires prospectively held explicit hashes')
    return run(repository,output,args.held.resolve(strict=True),args.model_sha,args.freeze_sha)


if __name__=='__main__':
    sys.dont_write_bytecode = True
    sys.exit(main())
