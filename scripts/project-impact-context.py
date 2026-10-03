#!/usr/bin/env python3
"""Run the fixed checked SLIM capture/impact pipeline and export context data.

Report paths are byte labels. This collector never opens caller project source,
parses SLIM/manifests, reconciles catalogs or computes an impact closure.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import math
import os
from pathlib import Path
try:
    import resource
except ImportError:
    resource=None
import selectors
import shutil
import signal
import stat
import subprocess
import sys
import time
import types

MIB=1048576
REPORT_CAP=8*MIB
JSON_CAP=24*MIB
TOOL_CAP=128*MIB
COUNTER_CAP=1000000000
GLOBAL_SECONDS=900
CHILD_SECONDS=60
STDERR_CAP=256*1024
RETAINED_CAP=384*MIB
ADAPTER_SHA='004b176cdd01aba975e53f3292cd13a759bba645831279c42fa0dabcb8003c00'
MANIFEST_PINS={
 'project-input.project':'b31b63bab95ddac1d49e719e3537c0f202031347e794c58f7b07b7196e1a9f3f',
 'library/project-impact.project':'86a2c62e3e631b35b7b38b23d6f69c994fea38b70e0ab5cbc1dc7d0fec9f5b68',
}
ROLES=('emit-producer','build-producer','emit-impact','build-impact',
       'capture-before','capture-after','impact')
FAULT_NAMES=('SLIM_ALLOC_FAIL_AT','SLIM_TASK_FAIL_AT','SLIM_TASK_JOIN_FAIL_AT','SLIM_TASK_DISABLE')
REGISTRY=('library/applications/catalog/catalog.slim','library/applications/catalog/emit.slim',
 'library/applications/catalog/model.slim','library/applications/catalog/reconcile.slim',
 'library/applications/project_impact/closure.slim','library/applications/project_impact/data.slim',
 'library/applications/project_impact/input.slim','library/applications/project_impact/main.slim',
 'library/applications/project_impact/model.slim','library/applications/project_impact/prepare.slim',
 'library/applications/project_impact/report.slim','library/applications/project_input/main.slim',
 'library/applications/workplan/load.slim','library/applications/workplan/model.slim',
 'library/components/project_input_data.slim','library/components/project_input_emit.slim',
 'library/components/project_input_limits.slim','library/components/project_input_model.slim',
 'library/components/records.slim','library/experimental/ascii.slim','library/experimental/byte_index.slim',
 'library/experimental/bytes.slim','library/experimental/decimal.slim','library/experimental/netstring.slim',
 'library/experimental/text.slim','library/project-impact.project','project-input.project',
 'runtime/slim_rt.c','runtime/slim_rt.h','scripts/project-impact-context.py',
 'scripts/project-input-inventory.py','selfhost/check.slim','selfhost/codegen.slim',
 'selfhost/control.slim','selfhost/effects.slim','selfhost/format.slim','selfhost/identity.slim',
 'selfhost/ir.slim','selfhost/memory.slim','selfhost/ownership.slim','selfhost/parallel.slim',
 'selfhost/project.slim','selfhost/ranges.slim','selfhost/retained.slim','selfhost/scheduler.slim',
 'selfhost/syntax.slim','selfhost/text.slim','selfhost/typing.slim','selfhost/validate.slim')
UNKNOWN={
 'application_invariants':'not analyzed','agent_effectiveness':'not measured',
 'context_sufficiency':'declared-import selection is not a sufficiency proof',
 'saved_compiler_work':'not measured',
 'physical_source_deduplication':'counts and weights count supplied catalog rows',
 'atomic_live_capture':'no atomic or final-live identity attestation',
 'loaded_code_aba_host_boot':'before/after observed bytes only',
 'transitive_toolchain':'direct tools pinned; SDK/linker/transitive identity not established',
 'physical_memory':'logical caps are not RSS or libc allocation bounds',
}

class Failure(ValueError):
    def __init__(self,code,stage,position=0,exitcode=65):
        self.code,self.stage,self.position,self.exitcode=code,stage,position,exitcode
        super().__init__(code)

class WholeDeadline(Exception):
    pass

def require(value,code,stage,position=0,exitcode=65):
    if not value:
        raise Failure(code,stage,position,exitcode)

def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds')

def digest(data):
    return hashlib.sha256(data).hexdigest()

def identity(data):
    return {'bytes':len(data),'sha256':digest(data)}

def admitted(value,cap):
    return type(value) is int and 0<=value<=cap

def addition(left,right,cap=COUNTER_CAP):
    if not admitted(left,cap) or not admitted(right,cap) or right>cap-left:
        return None
    return left+right

def product(left,right,cap=COUNTER_CAP):
    if not admitted(left,cap) or not admitted(right,cap) or (left and right>cap//left):
        return None
    return left*right

def guarded_sum(values):
    total=0
    for value in values:
        next_value=addition(total,value)
        require(next_value is not None,'arithmetic-limit','report')
        total=next_value
    return total

def canonical(value,cap,code='json-limit',stage='output'):
    data=json.dumps(value,sort_keys=True,separators=(',',':'),ensure_ascii=True,
                    allow_nan=False).encode('ascii')+b'\n'
    require(len(data)<=cap,code,stage)
    return data

def bounded_path(value,stage='arguments'):
    require(isinstance(value,(str,Path)) and len(os.fsencode(value))<=4096,
            'invalid-arguments',stage,exitcode=64)
    try:
        result=Path(value).resolve()
    except (OSError,ValueError,RuntimeError):
        raise Failure('invalid-arguments',stage,exitcode=64) from None
    require(len(os.fsencode(result))<=4096,'invalid-arguments',stage,exitcode=64)
    return result

def repository_root():
    for parent in Path(__file__).resolve().parents:
        if (parent/'design/FEATURE_POLICY.md').is_file():
            return bounded_path(parent)
    raise Failure('source-error','sources',exitcode=66)

def read_file(path,cap,stage='sources',code='source-error'):
    try:
        descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NOFOLLOW',0)|os.O_NONBLOCK)
        with os.fdopen(descriptor,'rb') as stream:
            require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode),code,stage,exitcode=66)
            data=stream.read(cap+1)
    except OSError:
        raise Failure(code,stage,exitcode=66) from None
    require(len(data)<=cap,'source-identity' if stage=='sources' else 'arithmetic-limit',stage)
    return data

def file_identity(path,cap=TOOL_CAP,stage='tools'):
    try:
        descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NOFOLLOW',0)|os.O_NONBLOCK)
        with os.fdopen(descriptor,'rb') as stream:
            info=os.fstat(stream.fileno())
            require(stat.S_ISREG(info.st_mode),'tool-error',stage,exitcode=66)
            require(info.st_size<=cap,'source-identity',stage)
            count=0; hashed=hashlib.sha256()
            while True:
                chunk=stream.read(min(65536,cap-count+1))
                if not chunk:
                    break
                require(len(chunk)<=cap-count,'source-identity',stage)
                count+=len(chunk); hashed.update(chunk)
    except OSError:
        raise Failure('tool-error',stage,exitcode=66) from None
    return {'bytes':count,'sha256':hashed.hexdigest()}

def adapter_module(root,captured):
    require(digest(captured)==ADAPTER_SHA,'source-identity','sources')
    name='slim_project_impact_inventory_'+str(os.getpid())
    require(name not in sys.modules,'source-identity','sources')
    module=types.ModuleType(name); module.__file__=str(root/'scripts/project-input-inventory.py')
    sys.modules[name]=module
    try:
        exec(compile(captured,module.__file__,'exec',dont_inherit=True),module.__dict__)
        require(read_file(Path(module.__file__),MIB)==captured,'source-identity','sources')
        return module
    except BaseException:
        sys.modules.pop(name,None)
        raise

def decode_report(data,adapter):
    require(type(data) is bytes and admitted(len(data),REPORT_CAP),'report-limit','report')
    counts={'outer_frames':0,'nested_frames':0}
    outer=adapter.Frames(data)
    def field(reader,cap,nested=False):
        key='nested_frames' if nested else 'outer_frames'
        next_value=addition(counts[key],1)
        require(next_value is not None,'arithmetic-limit','report')
        counts[key]=next_value
        return reader.read(cap)
    def number(reader,cap,nested=False):
        value,position=field(reader,20,nested)
        return adapter.number(value,cap,position)
    def name(reader):
        value,position=field(reader,64)
        require(adapter.valid_name(value),'report-record','report',position)
        return value,position
    def record(value,position,reserved=False):
        if not value:
            return None,None
        inner=adapter.Frames(value,position)
        key,key_position=field(inner,64,True)
        require(key==b'@project' if reserved else adapter.valid_name(key),
                'report-record','report',key_position)
        weight=number(inner,MIB,True)
        raw,raw_position=field(inner,MIB,True)
        require(inner.cursor==len(value),'report-end','report',position+inner.cursor)
        split=raw.find(b'\0')
        require(1<=split<=256 and len(raw)==split+65,'report-record','report',raw_position)
        path,hashed=raw[:split],raw[split+1:]
        require(all(byte in b'0123456789abcdef' for byte in hashed),
                'report-record','report',raw_position)
        require(not reserved or path==b'slim.project','report-record','report',raw_position)
        return {'name_hex':key.hex(),'bytes':weight,'path_hex':path.hex(),
                'source_sha256':hashed.decode('ascii')},key_position
    def nested(reserved=False):
        value,position=field(outer,MIB)
        result,key_position=record(value,position,reserved)
        return result,position,key_position
    def ordered(previous,key,position):
        require(previous is None or previous<key,'report-order','report',position)
        return key
    try:
        tag,position=field(outer,64)
        require(tag==b'slim-project-impact-1','report-format','report',position)
        flag,position=field(outer,1)
        require(flag in (b'0',b'1'),'report-control','report',position)
        before,position,_=nested(True)
        require(before is not None,'report-control','report',position)
        after,position,_=nested(True)
        require(after is not None,'report-control','report',position)
        d=number(outer,8190); changes=[]; previous=None
        for _ in range(d):
            kind,kind_position=field(outer,8)
            require(kind in (b'added',b'removed',b'modified'),'report-control','report',kind_position)
            key,key_position=name(outer); previous=ordered(previous,key,key_position)
            old,old_position,old_key=nested(); new,new_position,new_key=nested()
            require(old is None or old['name_hex']==key.hex(),'report-record','report',old_key)
            require(new is None or new['name_hex']==key.hex(),'report-record','report',new_key)
            require((old is None)==(kind==b'added'),'report-control','report',old_position)
            require((new is None)==(kind==b'removed'),'report-control','report',new_position)
            changes.append({'change':kind.decode('ascii'),'name_hex':key.hex(),'before':old,'after':new})
        g=number(outer,4095); graph=[]; previous=None
        for _ in range(g):
            key,key_position=name(outer); previous=ordered(previous,key,key_position)
            old,_=field(outer,MIB); new,_=field(outer,MIB)
            graph.append({'name_hex':key.hex(),'before_dependencies_hex':old.hex(),'after_dependencies_hex':new.hex()})
        lists=[]
        for _ in range(4):
            size=number(outer,4095); items=[]; previous=None
            for _ in range(size):
                key,key_position=name(outer); previous=ordered(previous,key,key_position)
                items.append(key.hex())
            lists.append(items)
        c=number(outer,4095); candidates=[]; previous=None
        for _ in range(c):
            row,position,key_position=nested()
            require(row is not None,'report-control','report',position)
            key=bytes.fromhex(row['name_hex']); previous=ordered(previous,key,key_position)
            candidates.append(row)
        require(outer.cursor==len(data),'report-end','report',outer.cursor)
    except adapter.InventoryError as error:
        raise Failure(error.code,'report',error.position) from None
    impact={'manifest_changed':flag==b'1','before_project':before,'after_project':after,
            'module_changes':changes,'import_changes':graph}
    impact.update({key:items for key,items in zip(('before_roots_hex','after_roots_hex',
                   'before_closure_hex','after_closure_hex'),lists)})
    counts.update(D=d,G=g,C=c,L=guarded_sum(tuple(len(items) for items in lists)),report_bytes=len(data))
    return impact,candidates,counts

def context_json(impact,candidates,counts,measurements,evidence):
    before,after=measurements
    facts={}
    for prefix,view in (('before',before),('after',after)):
        require(admitted(view['module_count'],4095) and admitted(view['direct_import_edges'],65536),
                'arithmetic-limit','report')
        require(admitted(view['manifest']['bytes'],MIB) and admitted(view['source_bytes'],4*MIB),
                'arithmetic-limit','report')
        require(view['source_bytes']>=view['manifest']['bytes'],'arithmetic-limit','report')
        facts[prefix+'_module_count']=view['module_count']
        facts[prefix+'_direct_import_edges']=view['direct_import_edges']
        facts[prefix+'_manifest_bytes']=view['manifest']['bytes']
        facts[prefix+'_module_catalog_weight_bytes']=view['source_bytes']-view['manifest']['bytes']
    facts.update(module_change_count=counts['D'],import_change_count=counts['G'],
      before_root_count=len(impact['before_roots_hex']),after_root_count=len(impact['after_roots_hex']),
      before_closure_count=len(impact['before_closure_hex']),after_closure_count=len(impact['after_closure_hex']),
      current_candidate_count=len(candidates),
      current_candidate_catalog_weight_bytes=guarded_sum(tuple(row['bytes'] for row in candidates)))
    require(all(admitted(value,COUNTER_CAP) for value in facts.values()),'arithmetic-limit','report')
    value={'format':'slim-project-impact-context-1','evidence':evidence,'facts':facts,'impact':impact,
      'selection':{'snapshot':'after','catalog_sha256':evidence['inputs']['after_catalog']['sha256'],
          'graph_sha256':evidence['inputs']['after_graph']['sha256'],'candidates':candidates},'unknown':dict(UNKNOWN)}
    data=canonical(value,JSON_CAP)
    envelope=guarded_sum((product(2,counts['report_bytes']),product(256,counts['D']),
      product(128,counts['G']),product(128,counts['C']),product(16,counts['L']),262144))
    require(len(data)<=envelope,'json-limit','output')
    return data

class Session:
    def __init__(self,root,output,begin=None,started_utc=None,budget_seconds=GLOBAL_SECONDS):
        self.root,self.output=root,output
        self.begin=time.monotonic_ns() if begin is None else begin
        self.deadline=self.begin+int(budget_seconds*1000000000)
        self.stage=ROLES[0]; self.expired=False; self.artifacts={}
        self.receipt={'schema':1,'format':'slim-project-impact-collector-1','status':'running',
          'started_utc':utc() if started_utc is None else started_utc,'roles':[],'unexecuted_roles':list(ROLES),
          'global_seconds':GLOBAL_SECONDS,'child_seconds':CHILD_SECONDS,
          'source_authority':'matching actual successful production producer invocations only',
          'report_authority':'matching actual successful fixed SLIM impact invocation only',
          'source_identity_scope':'observed source/tool and artifact bytes through context/phase publication; before final receipt write; no loaded-code/ABA/host-boot attestation',
          'timing_scope':'wrapper dispatch through process-group cleanup; includes fresh process and parent drain/cleanup; filesystem cache uncontrolled',
          'cleared_environment':list(FAULT_NAMES),'unknown':dict(UNKNOWN)}
    def cutoff(self):
        if self.expired or time.monotonic_ns()>=self.deadline:
            raise Failure('native-incomplete',self.stage,exitcode=74)
    def alarm(self,signum,frame):
        self.expired=True
        raise WholeDeadline()
    def publish(self,name,data,cap,retain=False):
        if not retain:
            self.cutoff()
        require(name not in self.artifacts and len(self.artifacts)<35,'publication-error','output',exitcode=73)
        require(len(data)<=cap,'publication-error','output',exitcode=73)
        try:
            with (self.output/name).open('xb') as stream:
                stream.write(data)
        except OSError:
            raise Failure('publication-error','output',exitcode=73) from None
        self.artifacts[name]=identity(data)
    def register(self,name,cap=TOOL_CAP):
        require(name not in self.artifacts and len(self.artifacts)<35,'publication-error','output',exitcode=73)
        self.artifacts[name]=file_identity(self.output/name,cap,'output')
    def pin_sources(self):
        captured={name:read_file(bounded_path(self.root/name),MIB) for name in REGISTRY}
        for name,pin in MANIFEST_PINS.items():
            require(digest(captured[name])==pin,'source-identity','sources')
        require(digest(captured['scripts/project-input-inventory.py'])==ADAPTER_SHA,'source-identity','sources')
        require(captured['scripts/project-impact-context.py']==read_file(Path(__file__),MIB),
                'source-identity','sources')
        return captured,{name:identity(data) for name,data in captured.items()}
    def process(self,index,argv,environment,emit=False):
        self.cutoff(); self.stage=ROLES[index-1]
        seconds=min(CHILD_SECONDS,max(0,(self.deadline-time.monotonic_ns())/1000000000))
        self.cutoff()
        cap=16*MIB if emit else REPORT_CAP
        record={'role':self.stage,'ordinal':index,'argv_hex':[os.fsencode(item).hex() for item in argv],
          'status':'ok','returncode':None,'started_utc':utc(),'seconds_limit':seconds,
          'stdout_limit':cap,'stderr_limit':STDERR_CAP,'file_limit':TOOL_CAP,
          'group_kill_issued':False,'group_already_absent':False,'direct_child_reaped':False,
          'stdout_complete':False,'stderr_complete':False,
          'timing_scope':'dispatch through drain/group cleanup/direct wait; excludes artifact writes'}
        self.receipt['roles'].append(record); self.receipt['unexecuted_roles']=list(ROLES[index:])
        started=time.monotonic_ns(); deadline=time.monotonic()+seconds
        child=None; out=bytearray(); err=bytearray(); previous_mask=None
        def limits():
            if previous_mask is not None:
                signal.pthread_sigmask(signal.SIG_SETMASK,previous_mask)
            signal.pthread_sigmask(signal.SIG_UNBLOCK,{signal.SIGALRM})
            resource.setrlimit(resource.RLIMIT_CPU,(math.ceil(seconds)+1,math.ceil(seconds)+1))
            resource.setrlimit(resource.RLIMIT_FSIZE,(TOOL_CAP,TOOL_CAP))
            resource.setrlimit(resource.RLIMIT_CORE,(0,0))
        try:
            previous_mask=signal.pthread_sigmask(signal.SIG_BLOCK,{signal.SIGALRM})
            try:
                child=subprocess.Popen(list(map(str,argv)),cwd=self.output,env=environment,
                  stdout=subprocess.PIPE,stderr=subprocess.PIPE,start_new_session=True,preexec_fn=limits)
            finally:
                signal.pthread_sigmask(signal.SIG_SETMASK,previous_mask)
            record['pid']=child.pid
            with selectors.DefaultSelector() as selector:
                for pipe,sink,limit,label in ((child.stdout,out,cap,'stdout'),(child.stderr,err,STDERR_CAP,'stderr')):
                    os.set_blocking(pipe.fileno(),False)
                    selector.register(pipe,selectors.EVENT_READ,(sink,limit,label))
                while selector.get_map() and record['status']=='ok':
                    if time.monotonic()>=deadline:
                        record['status']='timeout'; break
                    for key,_ in selector.select(min(.05,max(0,deadline-time.monotonic()))):
                        sink,limit,label=key.data
                        chunk=os.read(key.fd,min(65536,limit-len(sink)+1))
                        if not chunk:
                            selector.unregister(key.fileobj); record[label+'_complete']=True
                        elif len(chunk)>limit-len(sink):
                            record['status']='output-limit'; record['limited_stream']=label
                            record['observed_at_least_bytes']=limit+1; break
                        else:
                            sink.extend(chunk)
                if record['status']=='ok':
                    try:
                        child.wait(timeout=max(.001,deadline-time.monotonic()))
                    except subprocess.TimeoutExpired:
                        record['status']='timeout'
        except WholeDeadline:
            record['status']='timeout'; record['whole_deadline']=True
        except (OSError,ValueError):
            record['status']='infrastructure-error'
        finally:
            mask=signal.pthread_sigmask(signal.SIG_BLOCK,{signal.SIGALRM})
            try:
                if child is not None:
                    try:
                        os.killpg(child.pid,signal.SIGKILL); record['group_kill_issued']=True
                    except ProcessLookupError:
                        record['group_already_absent']=True
                    except OSError:
                        record['status']='cleanup-error'
                        try:
                            child.kill()
                        except OSError:
                            pass
                    try:
                        child.wait(timeout=1); record['direct_child_reaped']=True
                        record['returncode']=child.returncode
                    except subprocess.TimeoutExpired:
                        record['status']='cleanup-error'
                    for pipe in (child.stdout,child.stderr):
                        try:
                            pipe.close()
                        except OSError:
                            record['status']='cleanup-error'
                record.update(finished_utc=utc(),elapsed_ns=time.monotonic_ns()-started)
                if record['status']=='ok' and (record['elapsed_ns']>seconds*1000000000 or self.expired):
                    record['status']='timeout'
                if record['status']=='ok' and record['returncode']!=0:
                    record['status']='native-error'
            finally:
                try:
                    signal.pthread_sigmask(signal.SIG_SETMASK,mask)
                except WholeDeadline:
                    record['status']='timeout'; record['whole_deadline']=True
        stdout,stderr=bytes(out),bytes(err)
        record['stdout']=identity(stdout); record['stderr']=identity(stderr)
        stem='phase-'+str(index).zfill(2)
        self.publish(stem+'.stdout.bin',stdout,cap,True)
        self.publish(stem+'.stderr.bin',stderr,STDERR_CAP,True)
        self.publish(stem+'.json',canonical(record,128*1024),128*1024,True)
        require(record['status']=='ok' and record['returncode']==0 and record['stdout_complete']
                and record['stderr_complete'] and record['direct_child_reaped'],
                'native-incomplete',self.stage,exitcode=74)
        self.cutoff()
        return stdout
    def finish(self,status,error=None):
        self.receipt.update(status=status,finished_observation_utc=utc(),
          elapsed_ns=time.monotonic_ns()-self.begin,
          elapsed_scope='whole collector through observation; before final receipt write/shutdown',
          artifacts=dict(self.artifacts))
        if error is not None:
            self.receipt['failure']={'code':error.code,'stage':error.stage,'position':error.position,'exit':error.exitcode}
        self.publish('receipt.json',canonical(self.receipt,2*MIB),2*MIB,status!='complete')

def fresh_output(root,value,inputs,tools):
    original=Path(value)
    require(not original.is_symlink(),'unsafe-output','output',exitcode=73)
    output=bounded_path(original,'output')
    require(output.is_relative_to((root/'build').resolve()) and output!=(root/'build').resolve()
            and not output.exists(),'unsafe-output','output',exitcode=73)
    forbidden=tuple(inputs)+tuple(tools)
    require(all(output!=item and not item.is_relative_to(output) for item in forbidden),
            'unsafe-output','output',exitcode=73)
    try:
        output.parent.mkdir(parents=True,exist_ok=True)
        output.mkdir()
    except OSError:
        raise Failure('unsafe-output','output',exitcode=73) from None
    return output

def _run(before,after,output,compiler,cc,begin,started_utc,state,budget_seconds):
    require(os.name=='posix' and hasattr(signal,'pthread_sigmask'),'unsupported-host','tools',exitcode=64)
    root=repository_root(); before=bounded_path(before); after=bounded_path(after)
    compiler=bounded_path(compiler if compiler is not None else root/'build/toolchain/slimc','tools')
    cc_name=cc if cc is not None else shutil.which('cc')
    require(cc_name is not None,'tool-error','tools',exitcode=66)
    cc=bounded_path(cc_name,'tools'); python=bounded_path(sys.executable,'tools')
    tools={label:{'path_hex':os.fsencode(path).hex(),**file_identity(path)}
           for label,path in (('compiler',compiler),('cc',cc),('python',python))}
    require(os.access(compiler,os.X_OK) and os.access(cc,os.X_OK),'tool-error','tools',exitcode=66)
    out=fresh_output(root,output,(before,after),(compiler,cc,python))
    session=Session(root,out,begin,started_utc,budget_seconds); state['session']=session
    adapter=None
    try:
        captured,pins=session.pin_sources()
        adapter=adapter_module(root,captured['scripts/project-input-inventory.py'])
        source_bytes=canonical(pins,65536)
        session.publish('sources-before.json',source_bytes,65536)
        session.publish('tools-before.json',canonical(tools,65536),65536)
        environment=dict(os.environ)
        for name in FAULT_NAMES:
            environment.pop(name,None)
        flags=('-std=c11','-Wall','-Wextra','-Werror','-O2','-DNDEBUG')
        runtime=bounded_path(root/'runtime/slim_rt.c'); include=bounded_path(root/'runtime')
        def execute(index,argv,emit=False,nonempty=False,empty=False):
            require(all(len(os.fsencode(value))<=4096 for value in argv),'invalid-arguments','tools',exitcode=64)
            stdout=session.process(index,argv,environment,emit)
            require(session.receipt['roles'][-1]['stderr']['bytes']==0 and
              (not nonempty or bool(stdout)) and (not empty or not stdout),
              'native-incomplete',ROLES[index-1],exitcode=74)
            return stdout
        execute(1,(compiler,bounded_path(root/'project-input.project')),True,True)
        execute(2,(cc,*flags,'-I',include,'-x','c',out/'phase-01.stdout.bin',runtime,'-o',out/'producer'),empty=True)
        session.register('producer')
        execute(3,(compiler,bounded_path(root/'library/project-impact.project')),True,True)
        execute(4,(cc,*flags,'-I',include,'-x','c',out/'phase-03.stdout.bin',runtime,'-o',out/'impact'),empty=True)
        session.register('impact')
        measurements=[]
        for index,label,path in ((5,'before',before),(6,'after',after)):
            execute(index,(out/'producer',path))
            try:
                measured=adapter.convert(out/('phase-'+str(index).zfill(2)+'.stdout.bin'),
                  out/(label+'-catalog.ns'),out/(label+'-graph.ns'),out/(label+'-inventory.json'))
            except adapter.InventoryError as error:
                raise Failure(error.code,ROLES[index-1],error.position) from None
            except (OSError,ValueError,RuntimeError):
                raise Failure('publication-error','output',exitcode=73) from None
            require(measured['capture']==session.artifacts['phase-'+str(index).zfill(2)+'.stdout.bin'],
                    'source-identity','sources')
            for suffix,cap in (('catalog.ns',MIB),('graph.ns',MIB),('inventory.json',4*MIB)):
                session.register(label+'-'+suffix,cap)
            require(measured['catalog']==session.artifacts[label+'-catalog.ns'] and
                    measured['workplan']==session.artifacts[label+'-graph.ns'],'source-identity','sources')
            measurements.append(measured)
        input_names=('before-catalog.ns','before-graph.ns','after-catalog.ns','after-graph.ns')
        report=execute(7,(out/'impact',*(out/name for name in input_names)))
        impact,candidates,counts=decode_report(report,adapter)
        evidence={'collector_sha256':pins['scripts/project-impact-context.py']['sha256'],
          'inventory_adapter_sha256':ADAPTER_SHA,'source_set_sha256':digest(source_bytes),
          'compiler':{key:tools['compiler'][key] for key in ('bytes','sha256')},
          'cc':{key:tools['cc'][key] for key in ('bytes','sha256')},
          'tool_arguments_hex':{key:tools[key]['path_hex'] for key in ('compiler','cc')},
          'producer':{'c':session.artifacts['phase-01.stdout.bin'],'executable':session.artifacts['producer']},
          'impact':{'c':session.artifacts['phase-03.stdout.bin'],'executable':session.artifacts['impact']},
          'before_capture':session.artifacts['phase-05.stdout.bin'],'after_capture':session.artifacts['phase-06.stdout.bin'],
          'inputs':{key:session.artifacts[name] for key,name in zip(
            ('before_catalog','before_graph','after_catalog','after_graph'),input_names)},
          'report':session.artifacts['phase-07.stdout.bin'],
          'before_project_argument_hex':os.fsencode(before).hex(),'after_project_argument_hex':os.fsencode(after).hex(),
          'successful_roles':list(ROLES)}
        context=context_json(impact,candidates,counts,measurements,evidence)
        session.publish('context.json',context,JSON_CAP)
        _,after_pins=session.pin_sources()
        require(after_pins==pins,'source-identity','sources')
        after_tools={key:{'path_hex':value['path_hex'],**file_identity(Path(os.fsdecode(bytes.fromhex(value['path_hex']))))}
                     for key,value in tools.items()}
        require(after_tools==tools,'source-identity','sources')
        session.publish('sources-after.json',canonical(after_pins,65536),65536)
        session.publish('tools-after.json',canonical(after_tools,65536),65536)
        require(all(file_identity(out/name,RETAINED_CAP,'output')==value for name,value in session.artifacts.items()),
                'source-identity','sources')
        _,final_pins=session.pin_sources()
        require(final_pins==pins,'source-identity','sources')
        require(all(file_identity(Path(os.fsdecode(bytes.fromhex(value['path_hex']))))==
                    {field:value[field] for field in ('bytes','sha256')} for value in tools.values()),
                'source-identity','sources')
        total=guarded_sum(tuple(value['bytes'] for value in session.artifacts.values()))
        require(len(session.artifacts)==34 and total<=RETAINED_CAP-2*MIB,'publication-error','output',exitcode=73)
        session.receipt.update(report_counts=counts,context=identity(context),source_set_sha256=digest(source_bytes),
          producer_source_acceptance='observed complete0 for both matching captures',
          impact_acceptance='observed complete0 for matching four frozen inputs and report')
        session.finish('complete')
        return out
    except WholeDeadline:
        error=Failure('native-incomplete',session.stage,exitcode=74)
        try:
            session.finish('failed',error)
        except (Failure,WholeDeadline,OSError):
            pass
        raise error from None
    except (Failure,MemoryError,OSError) as original:
        error=original if isinstance(original,Failure) else Failure('memory-error',session.stage,exitcode=71)
        if isinstance(original,OSError):
            error=Failure('publication-error','output',exitcode=73)
        try:
            session.finish('failed',error)
        except (Failure,WholeDeadline,OSError,MemoryError):
            pass
        raise error from None
    finally:
        if adapter is not None:
            sys.modules.pop(adapter.__name__,None)

def run(before,after,output,compiler=None,cc=None):
    require(os.name=='posix' and resource is not None and all(hasattr(signal,name) for name in
            ('pthread_sigmask','sigpending','sigwait')),'unsupported-host','tools',exitcode=64)
    begin=time.monotonic_ns(); started_utc=utc(); state={'session':None}
    old_handler=signal.getsignal(signal.SIGALRM); old_timer=signal.getitimer(signal.ITIMER_REAL)
    def alarm(signum,frame):
        if state['session'] is not None:
            state['session'].expired=True
        raise WholeDeadline()
    budget_seconds=min(GLOBAL_SECONDS,old_timer[0]) if old_timer[0]>0 else GLOBAL_SECONDS
    signal.signal(signal.SIGALRM,alarm); signal.setitimer(signal.ITIMER_REAL,budget_seconds)
    try:
        result=_run(before,after,output,compiler,cc,begin,started_utc,state,budget_seconds)
        stage=ROLES[0] if state['session'] is None else state['session'].stage
        require(time.monotonic_ns()-begin<=budget_seconds*1000000000,'native-incomplete',stage,exitcode=74)
        return result
    except WholeDeadline:
        stage=ROLES[0] if state['session'] is None else state['session'].stage
        raise Failure('native-incomplete',stage,exitcode=74) from None
    except MemoryError:
        raise Failure('memory-error','tools',exitcode=71) from None
    finally:
        mask=signal.pthread_sigmask(signal.SIG_BLOCK,{signal.SIGALRM})
        try:
            signal.setitimer(signal.ITIMER_REAL,0)
            while signal.SIGALRM in signal.sigpending():
                signal.sigwait({signal.SIGALRM})
            signal.signal(signal.SIGALRM,old_handler)
            if old_timer[0]>0:
                remaining=old_timer[0]-(time.monotonic_ns()-begin)/1000000000
                signal.setitimer(signal.ITIMER_REAL,max(.000001,remaining),old_timer[1])
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK,mask)

class Arguments(argparse.ArgumentParser):
    def error(self,message):
        raise Failure('invalid-arguments','arguments',exitcode=64)

def main():
    try:
        parser=Arguments(description=__doc__,add_help=False,allow_abbrev=False)
        parser.add_argument('before'); parser.add_argument('after'); parser.add_argument('--output',required=True)
        parser.add_argument('--compiler'); parser.add_argument('--cc')
        args=parser.parse_args()
        run(args.before,args.after,args.output,args.compiler,args.cc)
    except Failure as error:
        sys.stderr.write('project-impact-context: '+error.code+' in '+error.stage+' at '+str(error.position)+'\n')
        return error.exitcode
    except MemoryError:
        sys.stderr.write('project-impact-context: memory-error in arguments at 0\n'); return 71
    sys.stdout.write('project-impact-context: ok\n'); return 0

if __name__=='__main__':
    sys.exit(main())
