#!/usr/bin/env python3
"""Permanent RFC172 current-source gate; importing defines code only.

A fresh independent literal hold precedes every helper/native invocation under
one absolute deadline. Production SLIM alone accepts sources.
"""
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import signal
import stat
import subprocess
import sys
import time
from types import ModuleType, SimpleNamespace

MIB = 1048576
SOURCE_CAP = MIB
DATA_CAP = 64*MIB
MODEL_CAP = 4*MIB
HOLD_FILE_CAP = 192
NATIVE_FILE_CAP = 128*MIB
ENTRY_CAP = 1024
RECEIPT_CAP = 8*MIB
HELPER_PATH = 'scripts/verify-project-impact.py'
HELPER_SHA = '6e412c54726f23392003dfd82c20c159743a58ea6506c00274577f59dfb1b1be'
RFC_PATH = 'design/rfcs/0172-bounded-source-record-selection.md'
APPLICATION_MANIFEST = 'library/source-context.project'
APPLICATION_BODIES = (
 'library/applications/source_context/main.slim',
 'library/applications/source_context/model.slim',
 'library/applications/source_context/data.slim',
 'library/applications/source_context/report.slim')
SELFHOST = (
 'analysis','cache','check','codegen','slimc','context','control','driver',
 'edit','effects','equivalence','flow','format','fragments','identity','ir',
 'memory','nativebuild','nativecache','ownership','parallel','parallelcache',
 'project','proof','quality','query','ranges','reduce','retained','scheduler',
 'session','syntax','text','typing','validate')
LEDGER = (
 'library/applications/ledger/main.slim','library/applications/ledger/emit.slim',
 'library/applications/ledger/model.slim','library/applications/ledger/parser.slim',
 'library/applications/ledger/state.slim','library/experimental/ascii.slim',
 'library/experimental/bytes.slim','library/experimental/decimal.slim',
 'library/experimental/text.slim')
CATALOG = (
 'library/applications/catalog/main.slim','library/applications/catalog/catalog.slim',
 'library/applications/catalog/diff_emit.slim','library/applications/catalog/emit.slim',
 'library/applications/catalog/model.slim','library/applications/catalog/reconcile.slim',
 'library/components/records.slim','library/experimental/ascii.slim',
 'library/experimental/byte_index.slim','library/experimental/bytes.slim',
 'library/experimental/decimal.slim','library/experimental/netstring.slim',
 'library/experimental/text.slim')
PRODUCER_ONLY = (
 'library/applications/project_input/main.slim',
 'library/components/project_input_data.slim',
 'library/components/project_input_emit.slim',
 'library/components/project_input_limits.slim',
 'library/components/project_input_model.slim')
MANIFESTS = ('selfhost/slim.project','library/ledger.project',
             'library/catalog.project','project-input.project')
READSET = tuple(sorted(tuple('selfhost/'+name+'.slim' for name in SELFHOST)
 +tuple(sorted(set(LEDGER+CATALOG+PRODUCER_ONLY)))+MANIFESTS
 +('runtime/slim_rt.c','runtime/slim_rt.h','scripts/project-input-inventory.py',
   HELPER_PATH,'design/rfcs/0165-checked-project-input-producer.md')))
ORACLE_SHA = 'c110ef8f8a463524b9c12364e1988e561ce96d06eef4b4b80039e3496b5939ef'
CONTROL_PATHS = {
 'rfc':RFC_PATH,'application_manifest':APPLICATION_MANIFEST,
 'application_main':APPLICATION_BODIES[0],'application_model':APPLICATION_BODIES[1],
 'application_data':APPLICATION_BODIES[2],'application_report':APPLICATION_BODIES[3],
 'oracle':'scripts/source-context-oracle.py','freezer':'scripts/freeze-source-context.py',
 'observer':'scripts/verify-source-context.py'}
REAL_LAYOUT = {
 'compiler':('selfhost/slim.project',tuple('selfhost/'+name+'.slim' for name in SELFHOST),
             ('compiler','context','project')),
 'ledger':('library/ledger.project',LEDGER,('ledger','ledger_emit','ledger_state')),
 'catalog':('library/catalog.project',CATALOG,('catalog_app','catalog_data','catalog_reconcile'))}
SNAPSHOT_PATHS = tuple(sorted(set(tuple(row[0] for row in REAL_LAYOUT.values())
 +tuple(path for row in REAL_LAYOUT.values() for path in row[1]))))
FLAGS = (('ordinary',('-O2','-DNDEBUG')),
         ('sanitized',('-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer')))
SIDECARS = (
 'application-sanitized.dSYM/Contents/Info.plist',
 'application-sanitized.dSYM/Contents/Resources/DWARF/application-sanitized',
 'application-sanitized.dSYM/Contents/Resources/Relocations/aarch64/application-sanitized.yml')
CLEANUP_SOURCE = (b'import os,sys,time\nchild=os.fork()\nif child==0:\n'
 b' os.close(0);os.close(1);os.close(2);time.sleep(120);os._exit(0)\n'
 b'print(child,flush=True)\n')
UNKNOWN = {
 'quality':'sufficient/minimal context, model use and agent efficacy unmeasured',
 'savings':'native compile-time savings and saved model work unmeasured',
 'work':'fixture/output dimensions exact; internal index operations unobserved',
 'allocation':'only ordinals1..32 for fixed C03; larger allocation domain unknown',
 'physical':'logical caps do not bound RSS/libc allocation/physical IO latency',
 'identity':'observed endpoints; loaded code, ABA, host boot and atomic capture unproved',
 'toolchain':'direct compiler/CC/Python only; standard library/SDK/linker closure unproved',
 'platform':'fixed Darwin arm64/other POSIX artifact plans; unobserved native host acceptance remains unknown'}


def require(value, reason):
    if not value:
        raise ValueError(reason)


def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds').replace('+00:00','Z')


def root_path():
    for parent in Path(__file__).resolve().parents:
        if (parent/'design/FEATURE_POLICY.md').is_file():
            return parent
    raise ValueError('repository root unavailable')


def identity(data):
    return {'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()}


def canonical(value):
    return (json.dumps(value,sort_keys=True,separators=(',',':'),ensure_ascii=True)+'\n').encode('ascii')


def read(path, cap=SOURCE_CAP):
    require(not path.is_symlink() and path.is_file(),'ordinary bounded input: '+str(path))
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NONBLOCK',0)|getattr(os,'O_NOFOLLOW',0))
    with os.fdopen(descriptor,'rb') as stream:
        require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode),'ordinary opened input')
        data=stream.read(cap+1)
    require(len(data)<=cap,'post-read logical byte cap')
    return data


def file_identity(path, cutoff, cap=NATIVE_FILE_CAP):
    require(not path.is_symlink() and path.is_file(),'ordinary streamed file')
    total=0; hashed=hashlib.sha256()
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NONBLOCK',0)|getattr(os,'O_NOFOLLOW',0))
    with os.fdopen(descriptor,'rb') as stream:
        require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode),'ordinary streamed descriptor')
        while True:
            cutoff();part=stream.read(min(65536,cap-total+1))
            if not part:
                break
            total+=len(part);require(total<=cap,'streamed byte cap');hashed.update(part)
    return {'bytes':total,'sha256':hashed.hexdigest()}


def labels():
    names=['cleanup-descendant','check-application','emit-application',
           'build-application-ordinary','build-application-sanitized',
           'emit-production-producer','build-production-producer']
    names.extend(f'C{i:02d}-{v}' for i in range(1,65) for v in ('ordinary','sanitized'))
    names.extend(('argument-fifo-C08','argument-fifo-C09'))
    names.extend(f'G{n}-{v}' for n in (64,128,256,512) for v in ('ordinary','sanitized'))
    names.extend(f'oom-{i:02d}-{v}' for i in range(1,33) for v in ('ordinary','sanitized'))
    names.extend(key+'-'+role for key in ('compiler','ledger','catalog')
                 for role in ('capture','select-ordinary','select-sanitized'))
    require(len(names)==218 and len(set(names))==218,'fixed label geometry')
    return names


def artifact_plan():
    require(os.name=='posix','fixed POSIX process helper; other hosts unknown')
    machine=os.uname().machine
    require(sys.platform!='darwin' or machine=='arm64','fixed Darwin arm64 sidecar tier; other Darwin hosts unknown')
    sidecars=SIDECARS if sys.platform=='darwin' else ()
    regular={'cleanup-control.py','application.c','producer.c',
             'application-ordinary','application-sanitized','producer','receipt.json'}
    regular.update('snapshot/'+path for path in SNAPSHOT_PATHS)
    regular.update('commands/'+label+'/'+name for label in labels()
                   for name in ('stdout.bin','stderr.bin','process.json'))
    regular.update(sidecars)
    fifos={'fifos/C08','fifos/C09','commands/C04-ordinary/poison.fifo',
           'commands/C04-sanitized/poison.fifo'}
    directories={str(parent) for name in regular|fifos for parent in Path(name).parents if str(parent)!='.'}
    require(len(SNAPSHOT_PATHS)==56 and len(regular)==717+len(sidecars),'prospective fixed native file geometry')
    require(len(regular)+len(fifos)+len(directories)<=ENTRY_CAP,'prospective closed native tree geometry')
    return {'platform':sys.platform,'machine':machine,'regular_files':sorted(regular),
            'fifo_paths':sorted(fifos),'directories':sorted(directories),
            'files_including_receipt':717+len(sidecars),'nonself_hashes':716+len(sidecars),
            'sidecar_paths':list(sidecars),'entry_cap':ENTRY_CAP}


def held_admission(folder, cutoff, model_sha, freeze_sha):
    require(folder.is_dir() and not folder.is_symlink(),'fixed ordinary hold')
    raw=read(folder/'freeze.json',MIB);require(identity(raw)['sha256']==freeze_sha,'fresh held freeze identity')
    record=json.loads(raw)
    require(record['status']=='data-held-no-native' and record['native_commands']==0,'zero-native successful independent hold')
    require(record['files_including_receipt']==len(record['files'])+1<=HOLD_FILE_CAP and
            record['observed_nonself_files']==record['files'],'complete held file geometry')
    require(sum(pin['bytes'] for pin in record['files'].values())+len(raw)<=DATA_CAP,'fresh receipt-inclusive hold bytes')
    observed=set();entries=0
    for path in folder.rglob('*'):
        cutoff();entries+=1;require(entries<=HOLD_FILE_CAP+4 and not path.is_symlink(),'bounded ordinary held tree')
        require(path.is_dir() or path.is_file(),'no held special files')
        if path.is_file():
            observed.add(str(path.relative_to(folder)))
    require(observed==set(record['files'])|{'freeze.json'},'exact complete held file set')
    files={}
    for name,pin in record['files'].items():
        cutoff();path=Path(name)
        require(not path.is_absolute() and '..' not in path.parts and
                (name in ('model.json','sources-before.json','sources-after.json','tools-before.json','tools-after.json')
                 or (name.startswith('assets/') and len(name)==75 and name.endswith('.bin')
                     and all(c in '0123456789abcdef' for c in name[7:71]))),'closed held asset/metadata path domain')
        files[name]=read(folder/path,8*MIB+1)
        require(identity(files[name])==pin,'complete held raw artifact hash')
    require(identity(files['model.json'])['sha256']==model_sha and
            identity(files['model.json'])==record['model'] and len(files['model.json'])<=MODEL_CAP,'fixed complete model')
    model=json.loads(files['model.json'])
    require(model['schema']==1 and model['format']=='rfc172-fixed-source-context-data-1','fixed measurement format')
    require((model['constructor_cards'],model['geometry_cases'],model['real_cases'],model['data_scenarios'])==(64,4,3,71),
            'finite held data domains')
    require(model['limits']=={'child_seconds':60,'constructor_cards':64,'data_bytes':DATA_CAP,
            'direct_tool_bytes':NATIVE_FILE_CAP,'files':HOLD_FILE_CAP,'geometry_cases':4,
            'global_seconds':900,'model_bytes':MODEL_CAP,'real_cases':3,'source_file_bytes':SOURCE_CAP,
            'total_scenarios':72},'unchanged complete data/native bounds')
    require(model['envelopes']['valid_capture_bytes_upper']==6627047 and
            model['envelopes']['valid_output_bytes_upper']==5578446,'fixed validator-built byte envelopes')
    require(model['native_labels']==labels() and record['native_labels']==218 and
            len(model['cards'])==64 and len(model['geometry'])==4 and len(model['real'])==3,'fixed held218 labels')
    require(tuple(row['label'] for row in model['cards'])==tuple(f'C{i:02d}' for i in range(1,65)) and
            tuple(row['label'] for row in model['geometry'])==('G64','G128','G256','G512'),'complete held card order')
    require(model['source_pins']==record['sources_before']==record['sources_after'] and
            set(model['source_pins'])==set(READSET) and len(READSET)==67,'complete held source domain/endpoints')
    require(model['control_pins']==record['controls_before']==record['controls_after'] and
            set(model['control_pins'])==set(CONTROL_PATHS) and len(model['control_pins'])==9,'complete fresh controls/endpoints')
    require(model['tools']==record['tools_before']==record['tools_after'] and
            set(model['tools'])=={'compiler','cc','python'},'complete held direct tools/endpoints')
    for field,pins in (('source_refs',model['source_pins']),('control_refs',model['control_pins'])):
        require(set(model[field])==set(pins),'exact held byte reference registry')
        for key,pin in pins.items():
            require(identity(asset(model[field][key],files))==pin,'captured source/control reference association')
    require(model['control_pins']['oracle']['sha256']==ORACLE_SHA,'unchanged literal independent oracle identity')
    for doc,pins in (('sources-before.json',{'sources':model['source_pins'],'controls':model['control_pins']}),
                     ('sources-after.json',{'sources':model['source_pins'],'controls':model['control_pins']}),
                     ('tools-before.json',model['tools']),('tools-after.json',model['tools'])):
        require(files[doc]==canonical(pins),'complete endpoint metadata association')
    for row in model['cards']+model['geometry']:
        require(type(row['status']) is int and row['status'] in (0,64,65,66) and
                row['capture_argument'] in ('input','absent','missing'),'held observation scalar domain')
        if row['input'] is not None:
            asset(row['input'],files)
        asset(row['stdout'],files);require(asset(row['stderr'],files)==b'','held literal stderr')
        queries(row)
    for key,row in zip(REAL_LAYOUT,model['real']):
        manifest,paths,requested=REAL_LAYOUT[key]
        require(row['name']==key and row['manifest_path']==manifest and set(row['body_paths'])==set(paths)
                and queries(row)==tuple(os.fsdecode(q.encode('ascii')) for q in requested),'fixed held real domains')
        require(row['status']==0 and all(type(row[field]) is int and 0<=row[field]<=1000000000
                for field in ('modules','edges','module_body_bytes','selected_body_bytes')),
                'bounded data scope counts; not a quality score')
        for field,cap in (('capture',8*MIB),('graph',MIB),('stdout',8*MIB),('stderr',0)):
            require(len(asset(row[field],files))<=cap,'held real byte domains')
    return record,model,files


def asset(reference, files):
    require(type(reference) is dict and type(reference.get('bytes')) is int and
            0<=reference['bytes']<=8*MIB+1 and type(reference.get('sha256')) is str,'bounded byte reference')
    if 'file' in reference:
        require(set(reference)=={'file','bytes','sha256'} and reference['file'] in files and
                reference['file']=='assets/'+reference['sha256']+'.bin','closed raw asset reference')
        data=files[reference['file']]
    else:
        require(set(reference)=={'inline_hex','bytes','sha256'} and
                type(reference['inline_hex']) is str and len(reference['inline_hex'])<=8192,'bounded literal hex')
        data=bytes.fromhex(reference['inline_hex'])
    require(identity(data)=={key:reference[key] for key in ('bytes','sha256')},'exact complete byte reference')
    return data


def queries(row):
    require(type(row['queries_hex']) is list and len(row['queries_hex'])<=4096,'finite modeled argv length')
    result=[]
    for value in row['queries_hex']:
        require(type(value) is str and len(value)<=130,'bounded argv hex')
        raw=bytes.fromhex(value);require(len(raw)<=65 and b'\0' not in raw,'representable modeled query bytes')
        text=os.fsdecode(raw);require(os.fsencode(text)==raw,'exact POSIX argv byte round-trip')
        result.append(text)
    return tuple(result)


def captured_module(path, data, name):
    require(identity(data)['sha256']==HELPER_SHA and read(path)==data and name not in sys.modules,'fixed captured helper bytes')
    result=ModuleType(name);result.__file__=str(path);sys.modules[name]=result
    try:
        exec(compile(data,str(path),'exec',dont_inherit=True),result.__dict__)
        require(read(path)==data,'captured helper byte endpoint')
    except BaseException:
        sys.modules.pop(name,None);raise
    return result


class Campaign:
    def __init__(self, root, output, begin, receipt, helper):
        self.root=root;self.output=output;self.begin=begin;self.deadline=begin+900*1000000000
        self.receipt=receipt;self.helper=helper;self.active_label=None

    def cutoff(self):
        if time.monotonic_ns()>=self.deadline:
            raise self.helper.Deadline('fixed whole900 deadline; unfinished gates unknown')

    def save(self):
        data=canonical(self.receipt);require(len(data)<=RECEIPT_CAP,'bounded native receipt')
        (self.output/'receipt.json').write_bytes(data)

    def publish(self, path, data, cap=SOURCE_CAP):
        self.cutoff();require(type(data) is bytes and len(data)<=cap,'bounded fixed publication')
        name=str(path.relative_to(self.output))
        require(name in self.receipt['artifact_plan']['regular_files'] and name!='receipt.json','closed publication path')
        path.parent.mkdir(parents=True,exist_ok=True)
        with path.open('xb') as stream:
            stream.write(data)
        require(read(path,cap)==data,'fresh complete artifact readback')
        self.receipt.setdefault('created_files',{})[name]=identity(data)
        self.save()

    def execute(self, label, argv, environment, cap=8*MIB, seconds=60):
        self.cutoff();require(label==labels()[len(self.receipt['commands'])],'exact fixed child order')
        require(len(argv)>0 and 0<len(os.fsencode(str(argv[0])))<=4096 and
                all(len(os.fsencode(str(arg)))<=4096 for arg in argv[1:]),'bounded command arguments; empty negative query admitted')
        attempted={'label':label,'status':'incomplete','reason':'dispatch result not retained',
                   'argv_hex':[os.fsencode(str(arg)).hex() for arg in argv],'wrapper_started_utc':utc()}
        self.receipt['commands'].append(attempted);self.save()
        self.active_label=label
        start=time.monotonic_ns()
        try:
            self.cutoff();remaining=(self.deadline-time.monotonic_ns())/1000000000
            row,out,err=self.helper.process(argv,self.output/'commands'/label,min(seconds,remaining),environment,cap)
            raw_process=read(self.output/'commands'/label/'process.json',MIB)
            require(json.loads(raw_process)==row,'complete original raw process/helper row association')
            process_fields=sorted(row)
            row.update(label=label,argv_hex=attempted['argv_hex'],wrapper_started_utc=attempted['wrapper_started_utc'],
                       process_receipt=identity(raw_process),process_fields=process_fields,
                       wrapper_finished_utc=utc(),wrapper_elapsed_ns=time.monotonic_ns()-start,
                       wrapper_scope='dispatch through process-group cleanup/direct reap/raw artifact publication; overlaps child elapsed')
            self.receipt['commands'][-1]=row;self.save()
        finally:
            self.active_label=None
        self.cutoff()
        require(row['direct_child_reaped'] and (row['group_kill_issued'] or row.get('group_already_absent',False)),
                'mandatory owned-group cleanup/direct reap')
        return row,out,err

    def result(self, label, row, out, err, expected):
        gate=self.receipt['gates'][label];gate.pop('reason',None)
        complete=row['status'] in ('ok','native-error') and row['returncode'] is not None
        gate.update(process_status=row['status'],returncode=row['returncode'],stdout=identity(out),stderr=identity(err))
        gate['status']='pass' if complete and (row['returncode'],out,err)==expected else 'failed' if complete else 'unknown'
        if gate['status']!='pass':
            gate['reason']='bounded child incomplete or disagrees with sealed exact status/bytes'
        self.save();require(gate['status']=='pass',label+' failed or incomplete')

    def exact(self, label, argv, environment, expected):
        row,out,err=self.execute(label,argv,environment);self.result(label,row,out,err,expected)
        return out

    def successful(self, label, argv, environment, emit=False):
        row,out,err=self.execute(label,argv,environment,16*MIB if emit else 8*MIB)
        if emit and not out:
            gate=self.receipt['gates'][label];gate.pop('reason',None)
            complete=row['status'] in ('ok','native-error') and row['returncode'] is not None
            gate.update(status='failed' if complete else 'unknown',process_status=row['status'],
                        returncode=row['returncode'],stdout=identity(out),stderr=identity(err),
                        reason='fixed emission requires complete nonempty C')
            self.save();raise ValueError(label+' empty or incomplete emission')
        self.result(label,row,out,err,(0,out,b'') if emit else (0,b'',b''))
        return out


def fifo_identity(path):
    metadata=path.lstat();require(stat.S_ISFIFO(metadata.st_mode) and not path.is_symlink(),'fixed owned FIFO type')
    return {'kind':'fifo','device':metadata.st_dev,'inode':metadata.st_ino,'mode':stat.S_IMODE(metadata.st_mode)}


def fifo(campaign, path):
    name=str(path.relative_to(campaign.output))
    require(name in campaign.receipt['artifact_plan']['fifo_paths'] and not path.exists() and not path.is_symlink(),
            'exclusive fixed FIFO path')
    path.parent.mkdir(parents=True,exist_ok=True);os.mkfifo(path,0o600)
    campaign.receipt.setdefault('created_fifos',{})[name]=fifo_identity(path);campaign.save()


def fixture_hook(campaign, helper):
    """Isolated test-only delegate: create C04 poison after mkdir, before Popen."""
    original=helper.subprocess
    def popen(*args,**kwargs):
        if campaign.active_label in ('C04-ordinary','C04-sanitized'):
            wanted=campaign.output/'commands'/campaign.active_label
            cwd=Path(kwargs['cwd'])
            require(cwd==wanted and cwd.is_dir() and not cwd.is_symlink() and
                    cwd.parent==campaign.output/'commands','fixed root-owned C04 helper working directory')
            require('poison.fifo' not in os.listdir(cwd),'fresh fixture hook path')
            fifo(campaign,cwd/'poison.fifo')
        return original.Popen(*args,**kwargs)
    helper.subprocess=SimpleNamespace(Popen=popen,TimeoutExpired=original.TimeoutExpired,PIPE=original.PIPE)
    return original


def cleanup_control(campaign, environment):
    path=campaign.output/'cleanup-control.py';campaign.publish(path,CLEANUP_SOURCE)
    row,out,err=campaign.execute('cleanup-descendant',[sys.executable,path],environment,seconds=2)
    gate=campaign.receipt['gates']['cleanup-descendant'];gate.pop('reason',None)
    gate.update(process_status=row['status'],returncode=row['returncode'],source=identity(CLEANUP_SOURCE))
    complete=row['status'] in ('ok','native-error') and row['returncode'] is not None
    gate['status']='failed' if complete else 'unknown'
    if row['status']=='ok' and row['returncode']==0 and err==b'' and out.endswith(b'\n') and out[:-1].isdigit():
        require(len(out)<=32,'bounded descendant PID');pid=int(out[:-1]);require(pid>1,'positive descendant PID')
        end=min(time.monotonic()+1,campaign.deadline/1000000000);gone=False
        while time.monotonic()<end:
            campaign.cutoff()
            try:
                os.kill(pid,0)
            except ProcessLookupError:
                gone=True;break
            time.sleep(.01)
        gate.update(status='pass' if gone else 'failed',descendant_absent=gone)
    if gate['status']!='pass':
        gate['reason']='fixed closed-pipe descendant cleanup incomplete or failed'
    campaign.save();require(gate['status']=='pass','mandatory descendant cleanup control')


def generated_pin(campaign, path):
    name=str(path.relative_to(campaign.output))
    pin=file_identity(path,campaign.cutoff)
    require(name not in campaign.receipt.setdefault('generated_artifacts',{}),'capture generated artifact once')
    campaign.receipt['generated_artifacts'][name]=pin;campaign.save()


def program(campaign, binary):
    name=str(binary.relative_to(campaign.output))
    require(file_identity(binary,campaign.cutoff)==campaign.receipt['generated_artifacts'][name] and
            os.access(binary,os.X_OK),'original generated executable pin before use')
    return binary


def snapshot(campaign, model, files):
    for path in SNAPSHOT_PATHS:
        campaign.publish(campaign.output/'snapshot'/path,asset(model['source_refs'][path],files))


def native(campaign, model, files, folder, compiler, cc, environment):
    root=campaign.root;output=campaign.output
    snapshot(campaign,model,files)
    cleanup_control(campaign,environment)
    campaign.exact('check-application',[compiler,'check',root/APPLICATION_MANIFEST],environment,(0,b'',b''))
    body=campaign.successful('emit-application',[compiler,root/APPLICATION_MANIFEST],environment,emit=True)
    campaign.publish(output/'application.c',body,16*MIB);generated_pin(campaign,output/'application.c')
    programs={}
    for variant,flags in FLAGS:
        binary=output/('application-'+variant)
        require(file_identity(output/'application.c',campaign.cutoff,16*MIB)==campaign.receipt['generated_artifacts']['application.c'],
                'original emitted C before trusted CC')
        campaign.successful('build-application-'+variant,[cc,'-std=c11','-Wall','-Wextra','-Werror',*flags,
                            '-I',root/'runtime',output/'application.c',root/'runtime/slim_rt.c','-o',binary],environment)
        generated_pin(campaign,binary);programs[variant]=binary
        if variant=='sanitized':
            for name in campaign.receipt['artifact_plan']['sidecar_paths']:
                generated_pin(campaign,output/name)
    body=campaign.successful('emit-production-producer',[compiler,root/'project-input.project'],environment,emit=True)
    campaign.publish(output/'producer.c',body,16*MIB);generated_pin(campaign,output/'producer.c')
    require(file_identity(output/'producer.c',campaign.cutoff,16*MIB)==campaign.receipt['generated_artifacts']['producer.c'],
            'original producer C before trusted CC')
    campaign.successful('build-production-producer',[cc,'-std=c11','-Wall','-Wextra','-Werror',*FLAGS[0][1],
                        '-I',root/'runtime',output/'producer.c',root/'runtime/slim_rt.c','-o',output/'producer'],environment)
    generated_pin(campaign,output/'producer')
    def card_argv(row, variant):
        argv=[program(campaign,programs[variant])]
        if row['capture_argument']=='input':
            require(row['input'] is not None and 'file' in row['input'],'literal capture asset exists')
            argv.append(folder/row['input']['file'])
        elif row['capture_argument']=='missing':
            missing=output/'missing-capture.ns';require(not missing.exists() and not missing.is_symlink(),'fixed missing capture remains absent')
            argv.append(missing)
        else:
            require(row['capture_argument']=='absent' and row['input'] is None,'literal absent capture argument')
        argv.extend(queries(row));return argv
    for row in model['cards']:
        expected=(row['status'],asset(row['stdout'],files),asset(row['stderr'],files))
        for variant,_ in FLAGS:
            campaign.exact(row['label']+'-'+variant,card_argv(row,variant),environment,expected)
    for identifier in ('C08','C09'):
        row=model['cards'][int(identifier[1:])-1];path=output/'fifos'/identifier;fifo(campaign,path)
        campaign.exact('argument-fifo-'+identifier,[program(campaign,programs['ordinary']),path,*queries(row)],
                       environment,(row['status'],asset(row['stdout'],files),asset(row['stderr'],files)))
    for row in model['geometry']:
        for variant,_ in FLAGS:
            campaign.exact(row['label']+'-'+variant,card_argv(row,variant),environment,
                           (row['status'],asset(row['stdout'],files),asset(row['stderr'],files)))
    fault=model['cards'][2];full=asset(fault['stdout'],files)
    for ordinal in range(1,33):
        for variant,_ in FLAGS:
            label=f'oom-{ordinal:02d}-{variant}'
            row,out,err=campaign.execute(label,card_argv(fault,variant),dict(environment,SLIM_ALLOC_FAIL_AT=str(ordinal)))
            failure=(71,b'',f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode('ascii'))
            success=(0,full,b'')
            actual=(row['returncode'],out,err)
            expected=failure if ordinal==1 or actual==failure else success
            campaign.result(label,row,out,err,expected)
            campaign.receipt['gates'][label]['allocation_observation']='failed at requested ordinal' if actual==failure else 'ordinal not reached; whole exact result'
            campaign.save()
    for row in model['real']:
        key=row['name'];project=output/'snapshot'/REAL_LAYOUT[key][0]
        captured=campaign.exact(key+'-capture',[program(campaign,output/'producer'),project],environment,
                                (0,asset(row['capture'],files),b''))
        capture_path=output/'commands'/(key+'-capture')/'stdout.bin'
        require(file_identity(capture_path,campaign.cutoff,8*MIB)==identity(captured),'actual checked producer raw capture pin')
        campaign.receipt.setdefault('source_scope',[]).append({
            'project':key,'source_authority':'actual production producer0 plus exact independent capture bytes',
            'producer_executable':campaign.receipt['generated_artifacts']['producer'],
            'raw_capture':identity(captured),'declared_graph':{k:row['graph'][k] for k in ('bytes','sha256')},
            'modules':row['modules'],'edges':row['edges'],'module_body_bytes':row['module_body_bytes'],
            'selected_modules':len(row['queries_hex']),'selected_body_bytes':row['selected_body_bytes'],
            'expected_selection_output':{k:row['stdout'][k] for k in ('bytes','sha256')},
            'selector_gate_labels':[key+'-select-'+variant for variant,_ in FLAGS],
            'interpretation':'producer authority independent of selector outcomes; requested opaque byte scope/quality and savings unknown'})
        campaign.save()
        for variant,_ in FLAGS:
            campaign.exact(key+'-select-'+variant,[program(campaign,programs[variant]),capture_path,*queries(row)],environment,
                           (0,asset(row['stdout'],files),b''))


def source_admission(root, compiler, cc, cutoff, receipt):
    captured={};pins={}
    receipt['sources_before']=pins
    for path in READSET:
        cutoff();captured[path]=read(root/path);pins[path]=identity(captured[path])
    require(len(pins)==67 and set(pins)==set(READSET),'complete literal current source registry')
    require(len(APPLICATION_BODIES)>0 and len(APPLICATION_BODIES)<=8 and
            all(path.startswith('library/applications/source_context/') and path.endswith('.slim') for path in APPLICATION_BODIES)
            and len(set(APPLICATION_BODIES))==len(APPLICATION_BODIES),'exact accepted implementation body registry')
    controls={};receipt['controls_before']={}
    for label,path in CONTROL_PATHS.items():
        cutoff();controls[label]=read(root/path)
        receipt['controls_before'][label]=identity(controls[label])
    require(len(controls)==9 and len(set(CONTROL_PATHS.values()))==9,'complete distinct canonical controls')
    require(b'Status: accepted' in controls['rfc'].splitlines()[:24] and
            b'Status: accepted' in captured['design/rfcs/0165-checked-project-input-producer.md'].splitlines()[:24],
            'current accepted producer/selector contracts')
    require(identity(controls['oracle'])['sha256']==ORACLE_SHA and
            identity(captured[HELPER_PATH])['sha256']==HELPER_SHA,'fixed pure oracle/process helper bytes before code loading')
    tools={}
    receipt['tools_before']=tools
    for label,path in {'compiler':compiler,'cc':cc,'python':Path(sys.executable).resolve(strict=True)}.items():
        require(len(os.fsencode(path))<=4096,'direct tool path bound')
        tools[label]={'path_hex':os.fsencode(path).hex(),**file_identity(path,cutoff)}
    require(sum(map(len,captured.values()))+sum(map(len,controls.values()))<=DATA_CAP,
            'complete opaque custody byte admission before any captured code execution')
    return captured,pins,controls,tools


def captured_control_module(root, label, data, name):
    path=root/CONTROL_PATHS[label]
    require(label=='freezer' and read(path)==data and name not in sys.modules,'fixed fresh captured freezer identity')
    module=ModuleType(name);module.__file__=str(path);sys.modules[name]=module
    try:
        exec(compile(data,str(path),'exec',dont_inherit=True),module.__dict__)
        require(read(path)==data,'captured freezer byte endpoint')
    except BaseException:
        sys.modules.pop(name,None);raise
    return module


def admitted_hold(record, model, pins, controls, tools, fresh):
    require(model['source_pins']==pins and model['control_pins']=={key:identity(data) for key,data in controls.items()}
            and model['tools']==tools,'complete independent fresh hold binds pre-exec custody')
    require(fresh['status']=='data-held-no-native' and fresh['native_commands']==0 and
            fresh['model']==record['model'] and fresh['files']==record['files_including_receipt'] and
            fresh['bytes']==record['bytes_before_receipt']+fresh['freeze']['bytes']<=DATA_CAP,
            'complete fresh freezer return/receipt association')


def source_endpoints(campaign, folder, pins, controls, tools, fresh):
    after={};after_controls={};after_tools={}
    campaign.receipt.update(sources_after=after,controls_after=after_controls,tools_after=after_tools)
    for path in READSET:
        campaign.cutoff();after[path]=identity(read(campaign.root/path))
    for label,path in CONTROL_PATHS.items():
        campaign.cutoff();after_controls[label]=identity(read(campaign.root/path))
    for label,value in tools.items():
        path=Path(os.fsdecode(bytes.fromhex(value['path_hex'])))
        after_tools[label]={'path_hex':value['path_hex'],**file_identity(path,campaign.cutoff)}
    require(after==pins and after_controls=={key:identity(data) for key,data in controls.items()} and after_tools==tools,
            'all current source/hold-control/implementation/direct-tool original endpoints')
    record,model,_files=held_admission(folder,campaign.cutoff,fresh['model']['sha256'],fresh['freeze']['sha256'])
    admitted_hold(record,model,pins,controls,tools,fresh)
    campaign.receipt['held_endpoint']={'model_sha256':fresh['model']['sha256'],'freeze_sha256':fresh['freeze']['sha256'],
                                      'complete_observed_files_rehashed':record['files_including_receipt'],
                                      'scope':'complete unchanged independent fresh hold; no source acceptance fallback'}


def native_endpoints(campaign, complete):
    plan=campaign.receipt['artifact_plan'];require(artifact_plan()==plan,'fixed platform tier endpoints')
    inventory={};fifos={};directories=set();entries=0
    campaign.receipt['native_inventory_before_final_receipt']=inventory
    for path in campaign.output.rglob('*'):
        campaign.cutoff();entries+=1;require(entries<=ENTRY_CAP and not path.is_symlink(),'native tree bound/no symlink')
        name=str(path.relative_to(campaign.output));mode=path.lstat().st_mode
        if stat.S_ISDIR(mode):
            require(name in plan['directories'],'closed native directory');directories.add(name)
        elif stat.S_ISFIFO(mode):
            require(name in plan['fifo_paths'],'closed native FIFO');fifos[name]=fifo_identity(path)
        else:
            require(stat.S_ISREG(mode) and name in plan['regular_files'],'closed ordinary native artifact')
            if name!='receipt.json':
                inventory[name]=file_identity(path,campaign.cutoff)
    for name,pin in campaign.receipt.get('created_files',{}).items():
        require(inventory[name]==pin,'original copied/fixture artifact endpoint')
    for name,pin in campaign.receipt.get('generated_artifacts',{}).items():
        require(inventory[name]==pin,'original generated C/executable/sidecar endpoint')
    require(fifos==campaign.receipt.get('created_fifos',{}),'original FIFO type/inode endpoints')
    for row in campaign.receipt['commands']:
        if 'stdout' in row and 'stderr' in row:
            for channel in ('stdout','stderr'):
                require(inventory['commands/'+row['label']+'/'+channel+'.bin']==row[channel],'original raw child channel endpoint')
            process_bytes=read(campaign.output/'commands'/row['label']/'process.json',MIB)
            require(identity(process_bytes)==row['process_receipt'],'ORIGINAL raw process receipt byte endpoint')
            require(json.loads(process_bytes)=={key:row[key] for key in row['process_fields']},
                    'complete original process field set/value association')
    if complete:
        require(set(inventory)==set(plan['regular_files'])-{'receipt.json'} and
                set(fifos)==set(plan['fifo_paths']) and directories==set(plan['directories']),
                'complete exact native file/FIFO/directory inventory')
        require(len(inventory)==plan['nonself_hashes'] and entries==len(plan['regular_files'])+len(plan['fifo_paths'])+len(plan['directories']),
                'complete exact native artifact dimensions')
    campaign.receipt.update(native_files_including_receipt=len(inventory)+1,native_fifo_endpoints=fifos,
                           native_entries=entries,native_endpoint_scope='all observed original byte/type endpoints; own final receipt write excluded')


def main():
    begin=time.monotonic_ns();root=root_path();args=sys.argv[1:]
    require(Path(__file__).resolve()==root/CONTROL_PATHS['observer'],'canonical observer location; ignored candidate is source-only')
    require((len(args)==3 and args[:2]==['--current','--output']) or
            (len(args)==7 and args[:2]==['--current','--output'] and args[3]=='--compiler' and args[5]=='--cc'),
            'one fixed current command; no historical hold or budget modes')
    operands=[args[2]]+([] if len(args)==3 else [args[4],args[6]])
    require(all(0<len(os.fsencode(arg))<=4096 for arg in operands),'bounded fixed path arguments')
    build=(root/'build').resolve(strict=True);output=Path(args[2]).absolute();parent=output.parent.resolve(strict=True)
    require((parent==build or build in parent.parents) and 1<=len(output.name)<=91 and
            output.name.isascii() and output.name[0].isalpha() and
            all(c.isalnum() or c in '-_.' for c in output.name) and
            not output.exists() and not output.is_symlink(),
            'fresh output inside ignored repository build')
    output=parent/output.name;folder=output.with_name(output.name+'.held')
    require(len(os.fsencode(output))<=4096 and len(os.fsencode(folder))<=4096 and
            not folder.exists() and not folder.is_symlink(),'bounded fresh independent sibling hold')
    compiler=Path(args[4] if len(args)==7 else root/'build/toolchain/slimc').resolve(strict=True)
    cc=Path(args[6] if len(args)==7 else '/usr/bin/cc').resolve(strict=True)
    require(compiler==(root/'build/toolchain/slimc').resolve(strict=True) and cc==Path('/usr/bin/cc').resolve(strict=True),
            'fixed current direct compiler and CC')
    plan=artifact_plan();output.mkdir()
    receipt={'schema':1,'status':'running','started_utc':utc(),'global_seconds':900,'child_seconds':60,
             'constructor_cards':64,'geometry_cases':4,
             'real_cases':3,'data_scenarios':71,'native_labels':labels(),'commands':[],
             'gates':{label:{'status':'unknown','reason':'not executed'} for label in labels()},'artifact_plan':plan,
             'unknown':UNKNOWN,'fixture_hook':'isolated helper subprocess namespace; only two fixed C04 Popen calls create exclusive cwd poison FIFO',
             'timing_scope':'whole admission/materialization/native/cleanup/publication/endpoints; final receipt/shutdown excluded'}
    def save():
        data=canonical(receipt);require(len(data)<=RECEIPT_CAP,'bounded root native receipt')
        (output/'receipt.json').write_bytes(data)
    save();helper=None;campaign=None;original_namespace=None;module_name='rfc172_fixed_process_helper'
    freezer_name='rfc172_current_data_freezer';fresh=None
    def cutoff():
        if time.monotonic_ns()>=begin+900*1000000000:
            raise TimeoutError('fixed whole900 preparation deadline')
    def alarm(_signal,_frame):
        if helper is not None:
            raise helper.Deadline('fixed whole900 deadline; later observations unknown')
        raise TimeoutError('fixed whole900 admission deadline')
    old_handler=signal.signal(signal.SIGALRM,alarm)
    signal.setitimer(signal.ITIMER_REAL,max(.000001,900-(time.monotonic_ns()-begin)/1000000000))
    code=1;pins=None;controls=None;tools=None
    try:
        captured,pins,controls,tools=source_admission(root,compiler,cc,cutoff,receipt)
        save();cutoff()
        freezer=captured_control_module(root,'freezer',controls['freezer'],freezer_name)
        require(tuple(freezer.SOURCE_PATHS)==READSET and freezer.CONTROLS==CONTROL_PATHS,
                'canonical freezer/source/control registry association')
        require((freezer.DATA_CAP,freezer.MODEL_CAP,freezer.FILE_CAP,freezer.GLOBAL_SECONDS)==(DATA_CAP,MODEL_CAP,HOLD_FILE_CAP,900),
                'unchanged freezer geometry and whole bound')
        fresh=freezer.Freeze(root,folder,compiler,cc,begin,{'sources':captured,'controls':controls,'tools':tools}).run()
        receipt['fresh_hold']=fresh;save();cutoff()
        record,model,files=held_admission(folder,cutoff,fresh['model']['sha256'],fresh['freeze']['sha256'])
        admitted_hold(record,model,pins,controls,tools,fresh)
        receipt.update(held_model_sha256=fresh['model']['sha256'],held_freeze_sha256=fresh['freeze']['sha256'],
                       held_admission={'files':record['files_including_receipt'],'bytes':fresh['bytes'],'native_commands':0,'model':record['model']})
        save();cutoff()
        helper=captured_module(root/HELPER_PATH,captured[HELPER_PATH],module_name)
        campaign=Campaign(root,output,begin,receipt,helper);original_namespace=fixture_hook(campaign,helper)
        environment=dict(os.environ)
        for key in ('SLIM_ALLOC_FAIL_AT','SLIM_HOST_ALLOC_FAIL_AT','SLIM_NATIVE_ALLOC_FAIL_AT',
                    'SLIM_TASK_FAIL_AT','SLIM_TASK_JOIN_FAIL_AT','SLIM_TASK_DISABLE'):
            environment.pop(key,None)
        environment.update(ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1')
        native(campaign,model,files,folder,compiler,cc,environment)
        require([row['label'] for row in receipt['commands']]==labels() and
                all(gate['status']=='pass' for gate in receipt['gates'].values()),'all218 fixed leaves completed exactly')
        native_endpoints(campaign,True)
        source_endpoints(campaign,folder,pins,controls,tools,fresh)
        campaign.cutoff();receipt.update(status='pass',finished_utc=utc(),elapsed_ns=time.monotonic_ns()-begin,
                                       later_labels_unknown=[]);campaign.save();code=0
    except BaseException as error:
        receipt.update(status='failed-or-incomplete',error=(type(error).__name__+': '+str(error))[:4096],
                       later_labels_unknown=[label for label in labels() if receipt['gates'][label]['status']=='unknown'])
        if campaign is not None and time.monotonic_ns()<campaign.deadline:
            try:
                native_endpoints(campaign,False)
                if pins is not None and fresh is not None:
                    source_endpoints(campaign,folder,pins,controls,tools,fresh)
            except BaseException as secondary:
                receipt['retention_error']=(type(secondary).__name__+': '+str(secondary))[:4096]
        receipt.update(finished_utc=utc(),elapsed_ns=time.monotonic_ns()-begin)
        try:
            save()
        except BaseException as secondary:
            receipt['receipt_write_error']=(type(secondary).__name__+': '+str(secondary))[:4096]
        message='source-context-verify: failed or incomplete; '+receipt['error']
        print(message.encode('utf-8','backslashreplace')[:4096].decode('utf-8','ignore'),file=sys.stderr)
    finally:
        mask=signal.pthread_sigmask(signal.SIG_BLOCK,{signal.SIGALRM})
        try:
            signal.setitimer(signal.ITIMER_REAL,0)
            while signal.SIGALRM in signal.sigpending():
                signal.sigwait({signal.SIGALRM})
            if helper is not None and original_namespace is not None:
                helper.subprocess=original_namespace
            sys.modules.pop(module_name,None);sys.modules.pop(freezer_name,None);signal.signal(signal.SIGALRM,old_handler)
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK,mask)
    if code==0:
        print('source-context-verify: ok')
    return code


if __name__=='__main__':
    sys.dont_write_bytecode=True
    try:
        raise SystemExit(main())
    except (ValueError,OSError) as error:
        print(('source-context-verify: '+str(error)).encode('utf-8','backslashreplace')[:4096].decode('utf-8','ignore'),file=sys.stderr)
        raise SystemExit(64)
