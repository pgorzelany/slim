#!/usr/bin/env python3
"""Verify RFC170 current source through an independent fresh data hold.

The sealed model supplies semantic projections. Actual successful production
roles supply only independently measured evidence substitutions. Fault tools
and isolated monkeypatches are harness controls, never source authority.
"""
import argparse
from contextlib import redirect_stderr, redirect_stdout
from datetime import datetime, timezone
import hashlib
import io
import json
import os
from pathlib import Path
import signal
import stat
import sys
import time
import types

MIB=1048576
COLLECTOR_PATH='scripts/project-impact-context.py'
ORACLE_PATH='scripts/project-impact-context-oracle.py'
INHERITED_PATH='scripts/project-impact-oracle.py'
VERIFIER_PATH='scripts/verify-project-impact-context.py'
RFC_PATH='design/rfcs/0170-project-impact-context-export.md'
SOURCE_PATHS='''library/applications/catalog/catalog.slim
library/applications/catalog/diff_emit.slim
library/applications/catalog/emit.slim
library/applications/catalog/main.slim
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
library/catalog.project
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
scripts/project-input-inventory.py
selfhost/analysis.slim
selfhost/cache.slim
selfhost/check.slim
selfhost/codegen.slim
selfhost/context.slim
selfhost/control.slim
selfhost/driver.slim
selfhost/edit.slim
selfhost/effects.slim
selfhost/equivalence.slim
selfhost/flow.slim
selfhost/format.slim
selfhost/fragments.slim
selfhost/identity.slim
selfhost/ir.slim
selfhost/memory.slim
selfhost/nativebuild.slim
selfhost/nativecache.slim
selfhost/ownership.slim
selfhost/parallel.slim
selfhost/parallelcache.slim
selfhost/project.slim
selfhost/proof.slim
selfhost/quality.slim
selfhost/query.slim
selfhost/ranges.slim
selfhost/reduce.slim
selfhost/retained.slim
selfhost/scheduler.slim
selfhost/session.slim
selfhost/slim.project
selfhost/slimc.slim
selfhost/syntax.slim
selfhost/text.slim
selfhost/typing.slim
selfhost/validate.slim'''.splitlines()
ADAPTER_SHA='004b176cdd01aba975e53f3292cd13a759bba645831279c42fa0dabcb8003c00'
ROLES=('emit-producer','build-producer','emit-impact','build-impact','capture-before','capture-after','impact')
PAIR_NAMES=('real-unchanged-tiny','real-changed-tiny','real-deletion-rename-tiny',
 'real-manifest-widen-tiny','real-compiler-body-newline','real-catalog-body-newline')
CONTROL_COUNTS=(('cleanup-descendant',1),('invalid-before',5),('invalid-after',6),
 ('fail-emit-producer',1),('fail-build-producer',2),('fail-emit-impact',3),('fail-build-impact',4),
 ('timeout-impact',7),('output-limit-impact',7),('drift-source',7),('malformed-report',7),('fail-publication',7))
ZERO_NAMES=('missing-before','extra-positional','unknown-option','existing-output',
 'symlink-output','outside-build','tool-output-alias','input-output-alias')
PHASE_NAMES=tuple(name for index in range(1,8) for name in
 ('phase-'+str(index).zfill(2)+'.stdout.bin','phase-'+str(index).zfill(2)+'.stderr.bin','phase-'+str(index).zfill(2)+'.json'))
ARTIFACT_NAMES=PHASE_NAMES+('producer','impact','before-catalog.ns','before-graph.ns',
 'after-catalog.ns','after-graph.ns','before-inventory.json','after-inventory.json',
 'sources-before.json','sources-after.json','tools-before.json','tools-after.json','context.json','receipt.json')

# Literal, preregistered harness tools. Paths/arguments are fixed by workflow(),
# not by report data. No production CLI or fault-environment capability is added.
MOCKS={
 'fail.py':b"import os\nos.write(2,b'fixed-control\\n')\nraise SystemExit(7)\n",
 'timeout.py':b"import time\ntime.sleep(61)\n",
 'output-limit.py':(
   b"import os\nfor unused in range(128):\n chunk=memoryview(b'x'*65536)\n"
   b" while chunk:\n  chunk=chunk[os.write(1,chunk):]\nos.write(1,b'x')\n"),
 'report.py':(
   b"import os,sys\nwith open(sys.argv[1],'rb') as stream:\n data=memoryview(stream.read(8388609))\n"
   b"while data:\n data=data[os.write(1,data):]\n"),
 'descendant.py':(
   b"import os,sys,time\nread_end,write_end=os.pipe()\npid=os.fork()\n"
   b"if pid==0:\n os.close(read_end)\n with open(sys.argv[1],'x') as stream:\n  stream.write(str(os.getpid()))\n"
   b" os.write(write_end,b'1')\n os.close(write_end)\n"
   b" for descriptor in (0,1,2):\n  os.close(descriptor)\n time.sleep(61)\n os._exit(0)\n"
   b"os.close(write_end)\nassert os.read(read_end,1)==b'1'\nos.close(read_end)\nos._exit(0)\n"),
}
ZERO_TEMPLATES={
 'missing-before':('{collector}','--output','{fresh}','--compiler','{compiler}','--cc','{cc}'),
 'extra-positional':('{collector}','{before}','{after}','extra','--output','{fresh}','--compiler','{compiler}','--cc','{cc}'),
 'unknown-option':('{collector}','{before}','{after}','--output','{fresh}','--unknown','--compiler','{compiler}','--cc','{cc}'),
 'existing-output':('{collector}','{before}','{after}','--output','{existing}','--compiler','{compiler}','--cc','{cc}'),
 'symlink-output':('{collector}','{before}','{after}','--output','{symlink}','--compiler','{compiler}','--cc','{cc}'),
 'outside-build':('{collector}','{before}','{after}','--output','{outside}','--compiler','{compiler}','--cc','{cc}'),
 'tool-output-alias':('{collector}','{before}','{after}','--output','{compiler}','--compiler','{compiler}','--cc','{cc}'),
 'input-output-alias':('{collector}','{before}','{after}','--output','{before}','--compiler','{compiler}','--cc','{cc}'),
}
INJECTION_POINTS={
 'fail-emit-producer':{'role':1,'argv':('{python}','{mock}/fail.py')},
 'fail-build-producer':{'role':2,'argv':('{python}','{mock}/fail.py')},
 'fail-emit-impact':{'role':3,'argv':('{python}','{mock}/fail.py')},
 'fail-build-impact':{'role':4,'argv':('{python}','{mock}/fail.py')},
 'timeout-impact':{'role':7,'argv':('{python}','{mock}/timeout.py')},
 'output-limit-impact':{'role':7,'argv':('{python}','{mock}/output-limit.py')},
 'malformed-report':{'role':7,'argv':('{python}','{mock}/report.py','{freeze}/data/N26-outer-trailing.ns')},
 'drift-source':{'after_successful_role':7,'file':'library/experimental/ascii.slim','append_hex':'0a'},
 'fail-publication':{'method':'Session.publish','name':'receipt.json','when_status':'complete',
   'action':'raise Failure(publication-error,output,0,73); failed receipt retention remains enabled'},
 'cleanup-descendant':{'runner':'Session.process','ordinal':1,
   'argv':('{python}','{mock}/descendant.py','{output}/descendant.pid'),
   'observation':'kill(pid,0) must raise ProcessLookupError within fixed2s after runner cleanup'},
}

class CheckError(ValueError):
    pass

class Deadline(Exception):
    pass

def require(value,message):
    if not value:
        raise CheckError(message)

def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds')

def sha(data):
    return hashlib.sha256(data).hexdigest()

def identity(data):
    return {'bytes':len(data),'sha256':sha(data)}

def canonical(value):
    return json.dumps(value,sort_keys=True,separators=(',',':'),ensure_ascii=True,
                      allow_nan=False).encode('ascii')+b'\n'

def read(path,cap):
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NOFOLLOW',0)|os.O_NONBLOCK)
    with os.fdopen(descriptor,'rb') as stream:
        require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode),'ordinary admitted file')
        data=stream.read(cap+1)
    require(len(data)<=cap,'bounded read')
    return data

def file_identity(path,cap=128*MIB):
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NOFOLLOW',0)|os.O_NONBLOCK)
    with os.fdopen(descriptor,'rb') as stream:
        info=os.fstat(stream.fileno())
        require(stat.S_ISREG(info.st_mode) and info.st_size<=cap,'ordinary bounded tool')
        count=0; hashed=hashlib.sha256()
        while True:
            data=stream.read(min(65536,cap-count+1))
            if not data:
                break
            require(len(data)<=cap-count,'streamed admitted bytes')
            count+=len(data); hashed.update(data)
    return {'bytes':count,'sha256':hashed.hexdigest()}

def publish(path,data,cap):
    require(len(data)<=cap,'publication cap')
    path.parent.mkdir(parents=True,exist_ok=True)
    with path.open('xb') as stream:
        stream.write(data)

def captured_module(path,data,pin,name):
    require(sha(data)==pin and name not in sys.modules,'pin before import')
    module=types.ModuleType(name); module.__file__=str(path)
    sys.modules[name]=module
    try:
        exec(compile(data,str(path),'exec',dont_inherit=True),module.__dict__)
        require(read(path,MIB)==data,'loaded source endpoint')
    except BaseException:
        sys.modules.pop(name,None)
        raise
    return module

def root_path():
    for parent in Path(__file__).resolve().parents:
        if (parent/'design/FEATURE_POLICY.md').is_file():
            return parent
    raise CheckError('repository ancestor')

def held_data(directory,pins):
    receipt_bytes=read(directory/'freeze.json',MIB)
    require(identity(receipt_bytes)==pins['freeze'],'fresh sealed freeze receipt pin')
    receipt=json.loads(receipt_bytes)
    require(receipt['status']=='complete-data-only' and receipt['native_children']==0,'data-only custody')
    require(receipt['rows']==73 and receipt['source_pairs']==6 and len(receipt['files'])+1==99,'held cardinality')
    total=len(receipt_bytes); files={}
    for name,wanted in receipt['files'].items():
        require(not Path(name).is_absolute() and '..' not in Path(name).parts,'fixed relative data path')
        data=read(directory/name,16*MIB); require(identity(data)==wanted,'held file identity '+name)
        total+=len(data); require(total<=16*MIB,'held aggregate16MiB')
        files[name]=data
    require(identity(files['model.json'])==pins['model'] and identity(files['oracle.py'])==pins['oracle'],
            'fresh sealed model/oracle pins')
    model=json.loads(files['model.json'])
    require(len(model['rows'])==73 and len(model['source_pairs'])==6,'fixed model dimensions')
    require(tuple(row['name'] for row in model['source_pairs'])==PAIR_NAMES,'source-pair order')
    return receipt,model,files

def data_controls(collector,adapter,model,files):
    observed=[]
    for row in model['rows']:
        expected=row['expected']; kind=row['kind']; result={'name':row['name'],'kind':kind}
        if kind=='report':
            data=files[row['input']]
            require(identity(data)==row['input_identity'],'sealed report input')
            try:
                impact,candidates,counts=collector.decode_report(data,adapter)
            except collector.Failure as error:
                got={'exit':error.exitcode,'stage':error.stage,'code':error.code,'position':error.position}
                require(got==expected,'exact report diagnostic '+row['name'])
                result['diagnostic']=got
            else:
                require(expected=={'exit':0},'unexpected report acceptance '+row['name'])
                wanted=bytes.fromhex(row['expected_json_hex']); fixed=json.loads(wanted)
                facts=fixed['facts']; measurements=[]
                for prefix in ('before','after'):
                    measurements.append({'module_count':facts[prefix+'_module_count'],
                      'direct_import_edges':facts[prefix+'_direct_import_edges'],
                      'manifest':{'bytes':facts[prefix+'_manifest_bytes']},
                      'source_bytes':facts[prefix+'_manifest_bytes']+facts[prefix+'_module_catalog_weight_bytes']})
                got=collector.context_json(impact,candidates,counts,measurements,fixed['evidence'])
                require(got==wanted and identity(got)==row['expected_json_identity'],
                        'exact sealed JSON '+row['name'])
                if 'geometry' in row:
                    geometry=row['geometry']
                    require(all(counts[key]==geometry[key] for key in
                      ('D','G','C','L','report_bytes','outer_frames','nested_frames')),'exact frame geometry')
                    require(len(got)<=geometry['envelope'],'exact output envelope')
                result.update(context=identity(got),report_counts=counts)
        elif kind in ('report-admission','json-admission','integer-admission'):
            got=collector.admitted(row['value'],row['cap'])
            require(got==expected['admitted'],'exact scalar admission '+row['name'])
            result['admitted']=got
        elif kind in ('addition','product'):
            method=collector.addition if kind=='addition' else collector.product
            got=method(row['left'],row['right'],row['cap'])
            require(got==expected['value'] and (got is not None)==expected['admitted'],
                    'exact guarded arithmetic '+row['name'])
            result['value']=got
        else:
            raise CheckError('fixed data kind')
        observed.append(result)
    require(len(observed)==73,'complete73 data controls')
    return observed

def unfold_bundle(data,adapter,destination):
    # Fixed harness fixture transport only. Collector never consumes this form.
    rows=adapter.Frames(data); tag,_=rows.read(64)
    require(tag==b'slim-context-fixture-bundle-1','fixed bundle tag')
    raw,position=rows.read(20); count=adapter.number(raw,4096,position)
    previous=None; total=0
    for unused in range(count):
        raw,_=rows.read(256); body,_=rows.read(MIB)
        require(raw and b'\0' not in raw and not raw.startswith(b'/'),'fixed bundle relative path')
        name=os.fsdecode(raw); path=Path(name)
        require('..' not in path.parts and (previous is None or previous<raw),'fixed bundle path/order')
        previous=raw; total+=len(body); require(total<=4*MIB,'fixed bundle bytes')
        publish(destination/path,body,MIB)
    require(rows.cursor==len(data),'complete bundle transport')
    return destination/'slim.project'

def measurements_evidence(output,compiler,cc,collector_pin,before,after,source_pins):
    # Provenance is independently measured from retained artifacts, never read
    # from the collector context or copied from its evidence/receipt claims.
    pins=read(output/'sources-before.json',65536)
    source_set=json.loads(pins)
    require(pins==canonical(source_pins),'independently captured source-set identity')
    tools=json.loads(read(output/'tools-before.json',65536))
    require(source_set['scripts/project-impact-context.py']['sha256']==collector_pin,'collector registered identity')
    for label,path in (('compiler',compiler),('cc',cc),('python',Path(sys.executable).resolve())):
        require(tools[label]['path_hex']==os.fsencode(path).hex(),'resolved direct tool argument')
        require({key:tools[label][key] for key in ('bytes','sha256')}==file_identity(path),'direct tool identity')
    evidence={'collector_sha256':collector_pin,'inventory_adapter_sha256':ADAPTER_SHA,
      'source_set_sha256':sha(pins),'compiler':file_identity(compiler),'cc':file_identity(cc),
      'tool_arguments_hex':{'compiler':os.fsencode(compiler).hex(),'cc':os.fsencode(cc).hex()},
      'producer':{'c':file_identity(output/'phase-01.stdout.bin',16*MIB),'executable':file_identity(output/'producer')},
      'impact':{'c':file_identity(output/'phase-03.stdout.bin',16*MIB),'executable':file_identity(output/'impact')},
      'before_capture':file_identity(output/'phase-05.stdout.bin',8*MIB),
      'after_capture':file_identity(output/'phase-06.stdout.bin',8*MIB),
      'inputs':{key:file_identity(output/name,MIB) for key,name in zip(
        ('before_catalog','before_graph','after_catalog','after_graph'),
        ('before-catalog.ns','before-graph.ns','after-catalog.ns','after-graph.ns'))},
      'report':file_identity(output/'phase-07.stdout.bin',8*MIB),
      'before_project_argument_hex':os.fsencode(before).hex(),'after_project_argument_hex':os.fsencode(after).hex(),
      'successful_roles':list(ROLES)}
    return evidence

def expected_context(pair,evidence,files):
    fixed=json.loads(bytes.fromhex(pair['expected_json_hex']))
    fixed['evidence']=evidence
    # ONLY held after-input hashes substitute into these identity fields. All
    # semantic facts, rows, roots/closures/candidates stay byte-for-byte literal.
    fixed['selection']['catalog_sha256']=sha(files['pairs/'+pair['name']+'/after-catalog.ns'])
    fixed['selection']['graph_sha256']=sha(files['pairs/'+pair['name']+'/after-graph.ns'])
    return canonical(fixed)

def workflow(case,collector_path,output,before,after,compiler,cc):
    return [str(collector_path),str(before),str(after),'--output',str(output),
            '--compiler',str(compiler),'--cc',str(cc)]

def invoke_main(module,argv):
    previous=sys.argv; stdout=io.StringIO(); stderr=io.StringIO()
    try:
        sys.argv=argv
        with redirect_stdout(stdout),redirect_stderr(stderr):
            result=module.main()
    finally:
        sys.argv=previous
    out,err=stdout.getvalue().encode('ascii'),stderr.getvalue().encode('ascii')
    require(len(out)<=32768 and len(err)<=32768,'fixed CLI stream bounds')
    return {'exit':result,'stdout_hex':out.hex(),'stderr_hex':err.hex()}

def process_records(output,count,expected):
    receipt=json.loads(read(output/'receipt.json',2*MIB))
    require(len(receipt['roles'])==count and tuple(row['role'] for row in receipt['roles'])==ROLES[:count],
            'fixed complete role prefix')
    require(receipt['unexecuted_roles']==list(ROLES[count:]),'exact unexecuted roles')
    require(all(row['direct_child_reaped'] and
      (row['group_kill_issued'] or row['group_already_absent']) for row in receipt['roles']),
      'mandatory group cleanup/direct reaping')
    for ordinal,row in enumerate(receipt['roles'],1):
        stem='phase-'+str(ordinal).zfill(2)
        require(json.loads(read(output/(stem+'.json'),128*1024))==row,'phase/receipt metadata association')
        require(file_identity(output/(stem+'.stdout.bin'),row['stdout_limit'])==row['stdout'] and
          file_identity(output/(stem+'.stderr.bin'),256*1024)==row['stderr'],'raw phase identity')
    require(all(row['status']=='ok' and row['returncode']==0 and row['stdout_complete'] and row['stderr_complete']
                for row in receipt['roles'][:-1]),'all preceding native roles completed0')
    if 'process_status' in expected:
        row=receipt['roles'][-1]
        require(row['status']==expected['process_status'],'exact raw process status')
        if 'returncode' in expected:
            require(row['returncode']==expected['returncode'],'exact raw returncode')
    else:
        require(all(row['status']=='ok' and row['returncode']==0 and row['stdout_complete'] and row['stderr_complete']
                    for row in receipt['roles']),'complete0 role domain before data/publication failure')
    return receipt

class Campaign:
    def __init__(self,root,output,collector_path,collector_pin,compiler,cc,freeze,begin,started):
        self.root,self.output,self.collector_path,self.collector_pin=root,output,collector_path,collector_pin
        self.compiler,self.cc,self.freeze=compiler,cc,freeze
        self.begin=begin; self.started=started; self.observations=[]; self.labels=[]
        self.receipt={'schema':1,'format':'rfc170-fixed-verifier-1','status':'running',
          'started_utc':self.started,'global_seconds':900,'child_seconds':60,
          'collector':identity(read(collector_path,MIB)),'verifier':identity(read(Path(__file__),MIB)),
          'data_hold':'pending independent publication before collector load',
          'scope':'73 fixed data rows,99 fixed native labels,8 zero-native workflows; no retries',
          'expected_semantics':'held literal model only; observed evidence identity substitutions only',
          'unknown':'no loaded-code/ABA/host-boot/transitive-toolchain/RSS/context-sufficiency or agent efficacy attestation'}
    def endpoint(self):
        require(identity(read(self.collector_path,MIB))==self.receipt['collector'],'collector source endpoint')
        require(identity(read(Path(__file__),MIB))==self.receipt['verifier'],'verifier source endpoint')
        require({name:file_identity(self.root/name,MIB) for name in self.source_pins}==self.source_pins,
                'current registered source endpoints')
        require({name:file_identity(Path(os.fsdecode(bytes.fromhex(row['path_hex'])))) for name,row in self.tools.items()}
                =={name:{key:row[key] for key in ('bytes','sha256')} for name,row in self.tools.items()},'direct tool endpoints')
        require({name:identity(read(self.root/path,MIB)) for name,path in
                 (('oracle',ORACLE_PATH),('inherited_oracle',INHERITED_PATH),('verifier',VERIFIER_PATH),
                  ('collector',COLLECTOR_PATH),('rfc',RFC_PATH))}==self.controls,'control source endpoints')
        held_data(self.freeze,self.hold_pins)
    def isolated(self,name,collector_bytes,registry):
        root=self.output/'isolated'/name; root.mkdir(parents=True)
        for path in registry:
            body=collector_bytes if path=='scripts/project-impact-context.py' else read(self.root/path,MIB)
            publish(root/path,body,MIB)
        publish(root/'design/FEATURE_POLICY.md',read(self.root/'design/FEATURE_POLICY.md',MIB),MIB)
        (root/'build').mkdir()
        module=captured_module(root/'scripts/project-impact-context.py',collector_bytes,self.collector_pin,
                               'rfc170_collector_'+name.replace('-','_'))
        return root,module
    def case(self,name,collector_bytes,registry,model,files,adapter):
        root,module=self.isolated(name,collector_bytes,registry)
        basis='pairs/'+PAIR_NAMES[0]+'/'
        pair=next((row for row in model['source_pairs'] if row['name']==name),None)
        if pair is not None:
            basis='pairs/'+name+'/'
        before_data=files[basis+'before.bundle.ns']; after_data=files[basis+'after.bundle.ns']
        if name=='invalid-before':
            before_data=files[model['invalid_source_control']['bundle']]
        if name=='invalid-after':
            after_data=files[model['invalid_source_control']['bundle']]
        before=unfold_bundle(before_data,adapter,root/'fixtures/before')
        after=unfold_bundle(after_data,adapter,root/'fixtures/after')
        if name=='real-unchanged-tiny':
            require(before_data==after_data,'unchanged permits same input argument'); after=before
        output=root/'build/collection'; mock=root/'mocks'
        for path,body in MOCKS.items():
            publish(mock/path,body,32768)
        self.endpoint()
        copied_source_pins={path:file_identity(root/path,MIB) for path in registry}
        copied_tools={label:{'path_hex':os.fsencode(path).hex(),**file_identity(path)} for label,path in
                      (('compiler',self.compiler),('cc',self.cc),('python',Path(sys.executable).resolve()))}
        counts=0; original_process=module.Session.process; original_publish=module.Session.publish
        injection=INJECTION_POINTS.get(name)
        def process(session,index,argv,environment,emit=False):
            nonlocal counts
            counts+=1
            require(index==counts,'sequential fixed role ordinal')
            self.labels.append(name+'/'+str(index).zfill(2))
            actual=argv
            if injection is not None and injection.get('role')==index:
                substitutions={'python':str(Path(sys.executable).resolve()),'mock':str(mock),'freeze':str(self.freeze)}
                actual=tuple(value.format(**substitutions) for value in injection['argv'])
            result=original_process(session,index,actual,environment,emit)
            if name=='drift-source' and index==7:
                path=root/INJECTION_POINTS[name]['file']
                with path.open('ab') as stream:
                    stream.write(b'\n')
            return result
        def publication(session,path,data,cap,retain=False):
            if name=='fail-publication' and path=='receipt.json' and session.receipt['status']=='complete':
                raise module.Failure('publication-error','output',0,73)
            return original_publish(session,path,data,cap,retain)
        module.Session.process=process; module.Session.publish=publication
        result=invoke_main(module,workflow(name,module.__file__,output,before,after,self.compiler,self.cc))
        observation={'name':name,'outcome':result,'native_children':counts,
          'production_authority':pair is not None,'verification':'pending'}
        self.observations.append(observation)
        plan=next(row for row in model['native']['cases'] if row['name']==name)
        wanted=plan['expected']
        require(result=={key:wanted[key] for key in ('exit','stdout_hex','stderr_hex')},'exact CLI outcome '+name)
        require(counts==plan['children'],'exact native count '+name)
        receipt=process_records(output,counts,wanted)
        if pair is not None:
            require(receipt['status']=='complete','positive complete receipt')
            require(set(path.name for path in output.iterdir())==set(ARTIFACT_NAMES),'exact35 positive artifacts')
            require(receipt['artifacts']=={name:file_identity(output/name,384*MIB) for name in ARTIFACT_NAMES if name!='receipt.json'},
                    'independent artifact set identity')
            require(sum(file_identity(output/name,384*MIB)['bytes'] for name in ARTIFACT_NAMES)<=384*MIB,
                    'complete retained384MiB')
            require(read(output/'phase-07.stdout.bin',8*MIB)==bytes.fromhex(pair['expected_report_hex']),
                    'exact held source-pair report '+name)
            for slot in ('before-catalog','before-graph','after-catalog','after-graph'):
                require(read(output/(slot+'.ns'),MIB)==files[basis+slot+'.ns'],'four exact frozen inputs')
            evidence=measurements_evidence(output,self.compiler,self.cc,self.collector_pin,before,after,copied_source_pins)
            wanted_context=expected_context(pair,evidence,files)
            require(read(output/'context.json',24*MIB)==wanted_context,'exact provenance-substituted held JSON '+name)
            require(read(output/'sources-before.json',65536)==read(output/'sources-after.json',65536) and
                    read(output/'tools-before.json',65536)==read(output/'tools-after.json',65536),'successful source/tool pins')
            require(read(output/'tools-before.json',65536)==canonical(copied_tools),'independent complete direct-tool pin set')
            require({path:file_identity(root/path,MIB) for path in registry}==copied_source_pins,
                    'successful copied source endpoints')
            flags=('-std=c11','-Wall','-Wextra','-Werror','-O2','-DNDEBUG')
            expected_argv=((self.compiler,root/'project-input.project'),
              (self.cc,*flags,'-I',root/'runtime','-x','c',output/'phase-01.stdout.bin',root/'runtime/slim_rt.c','-o',output/'producer'),
              (self.compiler,root/'library/project-impact.project'),
              (self.cc,*flags,'-I',root/'runtime','-x','c',output/'phase-03.stdout.bin',root/'runtime/slim_rt.c','-o',output/'impact'),
              (output/'producer',before),(output/'producer',after),
              (output/'impact',*(output/(slot+'.ns') for slot in ('before-catalog','before-graph','after-catalog','after-graph'))))
            require(all(row['argv_hex']==[os.fsencode(path).hex() for path in argv] and
              row['status']=='ok' and row['returncode']==0 and row['stdout_complete'] and row['stderr_complete']
              for row,argv in zip(receipt['roles'],expected_argv)),'actual successful fixed production invocation geometry')
            context=identity(wanted_context)
        else:
            require(receipt['status']=='failed','explicit failed receipt')
            require(receipt['failure']=={key:wanted[key] for key in ('code','stage','position','exit')},'exact failure receipt')
            context=None
            if name.startswith('invalid-'):
                ordinal=plan['children']; stem='phase-'+str(ordinal).zfill(2)
                control=model['invalid_source_control']
                require(read(output/(stem+'.stdout.bin'),8*MIB)==bytes.fromhex(control['predicted_producer_stdout_hex']) and
                  read(output/(stem+'.stderr.bin'),256*1024)==bytes.fromhex(control['predicted_producer_stderr_hex']),
                  'independent invalid-source diagnostic prediction')
            if name.startswith('fail-') and name!='fail-publication':
                stem='phase-'+str(plan['children']).zfill(2)
                require(read(output/(stem+'.stdout.bin'),8*MIB)==b'' and
                        read(output/(stem+'.stderr.bin'),256*1024)==b'fixed-control\n','fixed tool failure bytes')
            if name=='fail-publication':
                require((output/'context.json').is_file(),'retained context before failed final publication')
        observation.update(context=context,receipt=file_identity(output/'receipt.json',2*MIB),verification='complete')
        self.endpoint()
    def cleanup_control(self,collector_bytes,registry):
        name='cleanup-descendant'; root,module=self.isolated(name,collector_bytes,registry)
        output=root/'build/collection'; output.mkdir()
        publish(root/'mocks/descendant.py',MOCKS['descendant.py'],32768)
        session=module.Session(root,output)
        self.labels.append(name+'/control')
        session.process(1,(Path(sys.executable).resolve(),root/'mocks/descendant.py',output/'descendant.pid'),dict(os.environ))
        record=session.receipt['roles'][0]
        require(record['status']=='ok' and record['returncode']==0 and record['direct_child_reaped'] and
          record['stdout_complete'] and record['stderr_complete'],'leader complete and runner cleanup')
        raw=read(output/'descendant.pid',32)
        require(raw.isdigit() and 0<int(raw)<=2147483647,'fixed descendant PID observation')
        pid=int(raw); until=time.monotonic()+2; absent=False
        while time.monotonic()<until:
            try:
                os.kill(pid,0)
            except ProcessLookupError:
                absent=True; break
            time.sleep(.01)
        require(absent,'closed-pipe descendant absent within2s')
        self.observations.append({'name':name,'leader_complete':True,'descendant_absent':absent,
                                  'within_deadlines':record['elapsed_ns']<=60000000000,'record':record})
        self.endpoint()
    def zero_controls(self,collector_bytes,registry,model,files,adapter):
        rows=[]
        for name in ZERO_NAMES:
            root,module=self.isolated('zero-'+name,collector_bytes,registry)
            before=unfold_bundle(files['pairs/'+PAIR_NAMES[0]+'/before.bundle.ns'],adapter,root/'fixtures/before')
            after=unfold_bundle(files['pairs/'+PAIR_NAMES[0]+'/after.bundle.ns'],adapter,root/'fixtures/after')
            existing=root/'build/existing'; existing.mkdir()
            link=root/'build/symlink'; link.symlink_to(existing)
            values={'collector':str(module.__file__),'before':str(before),'after':str(after),
              'fresh':str(root/'build/fresh'),'existing':str(existing),'symlink':str(link),
              'outside':str(root/'outside'),'compiler':str(self.compiler),'cc':str(self.cc)}
            def prohibited(*args,**kwargs):
                raise CheckError('zero control started a native child')
            module.Session.process=prohibited
            argv=[value.format(**values) for value in ZERO_TEMPLATES[name]]
            result=invoke_main(module,argv)
            wanted=next(row['expected'] for row in model['native']['zero_native'] if row['name']==name)
            require(result=={key:wanted[key] for key in ('exit','stdout_hex','stderr_hex')},'exact zero control '+name)
            require(not (root/'build/fresh').exists(),'zero control did not publish fresh output')
            rows.append({'name':name,'argv_hex':[os.fsencode(value).hex() for value in argv],
                         'native_children':0,'outcome':result})
            self.endpoint()
        return rows

def _verify(args,begin,started):
    require(os.name=='posix' and hasattr(signal,'pthread_sigmask'),'POSIX bounded native runner')
    root=root_path(); output=args.output.resolve(); freeze=output/'fixtures'
    require(not args.output.is_symlink() and not output.exists() and output.is_relative_to(root/'build'),'fresh ignored verifier output')
    require(len(os.fsencode(output))<=4096,'bounded output path'); output.mkdir(parents=True)
    require(Path(__file__).resolve()==root/VERIFIER_PATH,'canonical current verifier location')
    collector_path=root/COLLECTOR_PATH; compiler=args.compiler.resolve(); cc=args.cc.resolve()
    collector_bytes=read(collector_path,MIB); collector_pin=sha(collector_bytes)
    campaign=Campaign(root,output,collector_path,collector_pin,compiler,cc,freeze,begin,started)
    status='failed'; error=None
    try:
        # All fixed body/control/tool bytes are admitted before executing helpers.
        # Collector loading and every data/native test follow complete data sealing.
        require(len(SOURCE_PATHS)==69 and len(set(SOURCE_PATHS))==69,'fixed69 opaque source paths')
        campaign.source_pins={name:file_identity(root/name,MIB) for name in SOURCE_PATHS}
        control_bytes={name:read(root/path,MIB) for name,path in
          (('oracle',ORACLE_PATH),('inherited_oracle',INHERITED_PATH),('verifier',VERIFIER_PATH),
           ('collector',COLLECTOR_PATH),('rfc',RFC_PATH))}
        campaign.controls={name:identity(data) for name,data in control_bytes.items()}
        require(control_bytes['collector']==collector_bytes and
                b'Status: accepted' in control_bytes['rfc'].splitlines()[:24],'accepted RFC/current collector admission')
        campaign.tools={name:{'path_hex':os.fsencode(path).hex(),**file_identity(path)} for name,path in
                         (('compiler',compiler),('cc',cc),('python',Path(sys.executable).resolve()))}
        campaign.receipt.update(source_pins=campaign.source_pins,control_sources=campaign.controls,tools=campaign.tools)
        oracle=captured_module(root/ORACLE_PATH,control_bytes['oracle'],campaign.controls['oracle']['sha256'],
                               'rfc170_current_oracle')
        require(tuple(oracle.SOURCE_PATHS)==tuple(SOURCE_PATHS) and len(oracle.REGISTRY)==49,
                'independent fixed69/49 source registries')
        admission={'sources':campaign.source_pins,'controls':campaign.controls,'tools':campaign.tools}
        generated=oracle.freeze(freeze,compiler,cc,admission)
        # Pin the returned independent publication, then revalidate every file
        # without consulting collector code, observed reports or native results.
        freeze_bytes=read(freeze/'freeze.json',MIB)
        require(json.loads(freeze_bytes)==generated,'independent freeze publication equality')
        campaign.hold_pins={'freeze':identity(freeze_bytes),'model':generated['model'],
                            'oracle':campaign.controls['oracle']}
        held,model,files=held_data(freeze,campaign.hold_pins)
        require(held['source_pins']==campaign.source_pins and held['control_sources']==campaign.controls and
                held['tools']==campaign.tools,'fresh sealed admission equality')
        held_source_pins=json.loads(files['sources-before.json'])
        require(held_source_pins==campaign.source_pins and files['sources-before.json']==files['sources-after.json'] and
                files['tools-before.json']==files['tools-after.json'],'independent complete source/tool endpoints')
        campaign.endpoint()
        campaign.receipt.update(data_hold='complete before collector load',hold_pins=campaign.hold_pins,
          model_sha256=campaign.hold_pins['model']['sha256'],freeze_sha256=campaign.hold_pins['freeze']['sha256'],
          oracle_sha256=campaign.hold_pins['oracle']['sha256'])
        adapter_bytes=read(root/'scripts/project-input-inventory.py',MIB)
        require(identity(adapter_bytes)==campaign.source_pins['scripts/project-input-inventory.py'],
                'current adapter before captured load')
        adapter=captured_module(root/'scripts/project-input-inventory.py',adapter_bytes,ADAPTER_SHA,'rfc170_fixed_adapter')
        collector=captured_module(collector_path,collector_bytes,collector_pin,'rfc170_data_collector')
        require(tuple(collector.REGISTRY)==tuple(model['source_registry']) and len(collector.REGISTRY)==49,'fixed49 source registry')
        campaign.receipt['data_rows']=data_controls(collector,adapter,model,files)
        readiness={'zero_templates':ZERO_TEMPLATES,'injection_points':INJECTION_POINTS,
          'mock_sources_hex':{name:data.hex() for name,data in MOCKS.items()},'native_labels':model['native']['labels']}
        publish(output/'control-spec.json',canonical(readiness),128*1024)
        require(len(model['native']['labels'])==99 and len(ARTIFACT_NAMES)==35,'fixed workflow cardinalities')
        for name in PAIR_NAMES:
            campaign.case(name,collector_bytes,collector.REGISTRY,model,files,adapter)
        campaign.cleanup_control(collector_bytes,collector.REGISTRY)
        for name,count in CONTROL_COUNTS[1:]:
            campaign.case(name,collector_bytes,collector.REGISTRY,model,files,adapter)
        campaign.receipt['zero_controls']=campaign.zero_controls(collector_bytes,collector.REGISTRY,model,files,adapter)
        require(campaign.labels==model['native']['labels'],'all99 labels in exact predeclared order')
        campaign.endpoint(); status='complete'
    except (CheckError,Deadline,MemoryError,OSError,ValueError) as problem:
        error={'kind':type(problem).__name__,'reason':str(problem)[:256] if isinstance(problem,CheckError) else 'incomplete fixed campaign'}
    finally:
        mask=signal.pthread_sigmask(signal.SIG_BLOCK,{signal.SIGALRM})
        try:
            if status!='complete':
                signal.setitimer(signal.ITIMER_REAL,0)
                while signal.SIGALRM in signal.sigpending():
                    signal.sigwait({signal.SIGALRM})
            campaign.receipt.update(status=status,error=error,observations=campaign.observations,native_labels=campaign.labels,
              finished_observation_utc=utc(),elapsed_ns=time.monotonic_ns()-campaign.begin,
              elapsed_scope='independent fresh hold plus fixed verifier through observations, before final receipt publication')
            publish(output/'receipt.json',canonical(campaign.receipt),4*MIB)
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK,mask)
    require(status=='complete','fixed campaign incomplete; retained receipt')
    return campaign.receipt

def verify(args):
    require(os.name=='posix' and all(hasattr(signal,name) for name in
            ('pthread_sigmask','sigpending','sigwait')),'POSIX bounded native runner')
    begin=time.monotonic_ns(); started=utc()
    old_handler=signal.getsignal(signal.SIGALRM); old_timer=signal.getitimer(signal.ITIMER_REAL)
    def alarm(signum,frame):
        raise Deadline()
    seconds=min(900,old_timer[0]) if old_timer[0]>0 else 900
    signal.signal(signal.SIGALRM,alarm); signal.setitimer(signal.ITIMER_REAL,seconds)
    try:
        result=_verify(args,begin,started)
        require(time.monotonic_ns()-begin<=seconds*1000000000,'whole900s completion including receipt publication')
        return result
    finally:
        mask=signal.pthread_sigmask(signal.SIG_BLOCK,{signal.SIGALRM})
        try:
            signal.setitimer(signal.ITIMER_REAL,0)
            while signal.SIGALRM in signal.sigpending():
                signal.sigwait({signal.SIGALRM})
            signal.signal(signal.SIGALRM,old_handler)
            if old_timer[0]>0:
                signal.setitimer(signal.ITIMER_REAL,max(.000001,old_timer[0]-(time.monotonic_ns()-begin)/1000000000),old_timer[1])
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK,mask)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--current',action='store_true',required=True)
    parser.add_argument('--output',required=True,type=Path)
    parser.add_argument('--compiler',required=True,type=Path); parser.add_argument('--cc',required=True,type=Path)
    args=parser.parse_args()
    try:
        result=verify(args)
    except (CheckError,OSError,ValueError,Deadline,MemoryError) as error:
        print('rfc170 verifier: fixed campaign incomplete',file=sys.stderr); return 1
    print(canonical({'status':result['status'],'data_rows':len(result['data_rows']),
                     'native_labels':len(result['native_labels']),'elapsed_ns':result['elapsed_ns']}).decode(),end='')
    return 0

if __name__=='__main__':
    sys.exit(main())
