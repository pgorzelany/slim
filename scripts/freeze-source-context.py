#!/usr/bin/env python3
"""RFC172 data-only freezer; no compiler, helper or native invocation.

Import defines constants/functions only. An explicit command captures the complete
fixed readset and direct tool identities before executing exactly the pinned
independent oracle bytes. Opaque source data is never parsed or an acceptance
authority.
"""
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import signal
import stat
import sys
import time
from types import ModuleType

MIB = 1048576
SOURCE_CAP = MIB
TOOL_CAP = 128*MIB
DATA_CAP = 64*MIB
MODEL_CAP = 4*MIB
FILE_CAP = 192
DOC_CAP = 65536
RECEIPT_CAP = MIB
INPUT_ASSET_CAP = 8*MIB+1
OUTPUT_ASSET_CAP = 8*MIB
GLOBAL_SECONDS = 900
CONTROLS = {
    'rfc': 'design/rfcs/0172-bounded-source-record-selection.md',
    'application_manifest': 'library/source-context.project',
    'application_main': 'library/applications/source_context/main.slim',
    'application_model': 'library/applications/source_context/model.slim',
    'application_data': 'library/applications/source_context/data.slim',
    'application_report': 'library/applications/source_context/report.slim',
    'oracle': 'scripts/source-context-oracle.py',
    'freezer': 'scripts/freeze-source-context.py',
    'observer': 'scripts/verify-source-context.py',
}
ORACLE_SHA = 'c110ef8f8a463524b9c12364e1988e561ce96d06eef4b4b80039e3496b5939ef'
APPLICATION_MANIFEST_SHA = '71b2d510341440beedbfcd9508bcc8b979dee0840cf646c38106a0e749019d6b'
MANIFEST_SHA = {
    'selfhost/slim.project': '8f75c79f783c1942e1b1faa30867d34097ed86ee7b999cdf5153b58502dce9fe',
    'library/ledger.project': '376145c7517aa1a65180d7d9b44949ea5a8a479752b7643701b12a14e46891b1',
    'library/catalog.project': '28951ae70aaa93248fb5770ef3c8d0ec8649bd7f45456527136209c86195b16c',
    'project-input.project': 'b31b63bab95ddac1d49e719e3537c0f202031347e794c58f7b07b7196e1a9f3f',
}
FIXED_SHA = {
    'scripts/project-input-inventory.py': '004b176cdd01aba975e53f3292cd13a759bba645831279c42fa0dabcb8003c00',
    'scripts/verify-project-impact.py': '6e412c54726f23392003dfd82c20c159743a58ea6506c00274577f59dfb1b1be',
}
SELFHOST = (
    'analysis','cache','check','codegen','slimc','context','control','driver',
    'edit','effects','equivalence','flow','format','fragments','identity','ir',
    'memory','nativebuild','nativecache','ownership','parallel','parallelcache',
    'project','proof','quality','query','ranges','reduce','retained','scheduler',
    'session','syntax','text','typing','validate',
)
LEDGER = (
    'library/applications/ledger/main.slim',
    'library/applications/ledger/emit.slim',
    'library/applications/ledger/model.slim',
    'library/applications/ledger/parser.slim',
    'library/applications/ledger/state.slim',
    'library/experimental/ascii.slim','library/experimental/bytes.slim',
    'library/experimental/decimal.slim','library/experimental/text.slim',
)
CATALOG = (
    'library/applications/catalog/main.slim',
    'library/applications/catalog/catalog.slim',
    'library/applications/catalog/diff_emit.slim',
    'library/applications/catalog/emit.slim',
    'library/applications/catalog/model.slim',
    'library/applications/catalog/reconcile.slim',
    'library/components/records.slim',
    'library/experimental/ascii.slim','library/experimental/byte_index.slim',
    'library/experimental/bytes.slim','library/experimental/decimal.slim',
    'library/experimental/netstring.slim','library/experimental/text.slim',
)
PRODUCER_ONLY = (
    'library/applications/project_input/main.slim',
    'library/components/project_input_data.slim',
    'library/components/project_input_emit.slim',
    'library/components/project_input_limits.slim',
    'library/components/project_input_model.slim',
)
SOURCE_PATHS = tuple(sorted(
    tuple('selfhost/'+name+'.slim' for name in SELFHOST)
    + tuple(sorted(set(LEDGER+CATALOG+PRODUCER_ONLY)))
    + tuple(MANIFEST_SHA)
    + ('runtime/slim_rt.c','runtime/slim_rt.h',
       'scripts/project-input-inventory.py','scripts/verify-project-impact.py',
       'design/rfcs/0165-checked-project-input-producer.md')
))
REAL_LAYOUT = {
    'compiler': ('selfhost/slim.project','selfhost',
                 tuple('selfhost/'+name+'.slim' for name in SELFHOST),
                 ('compiler','context','project')),
    'ledger': ('library/ledger.project','library',LEDGER,
               ('ledger','ledger_emit','ledger_state')),
    'catalog': ('library/catalog.project','library',CATALOG,
                ('catalog_app','catalog_data','catalog_reconcile')),
}
UNKNOWN = {
    'source_acceptance': 'no compiler/checker/producer has executed in this freeze',
    'selection_acceptance': 'no source-context application or native helper has executed',
    'context_quality': 'sufficient/minimal context, model use, saved work and agent efficacy unmeasured',
    'physical_resources': 'post-read logical bounds do not bound RSS, libc allocation or filesystem IO latency',
    'identity': 'observed byte endpoints; loaded code, ABA, host boot and atomic live capture unproved',
    'toolchain': 'three direct tools byte-hashed only; standard library, SDK/linker/transitive closure unproved',
    'platform': 'native runner sidecar inventory/tier is a separate prospective condition; zero native here',
}


def require(condition, reason):
    if not condition:
        raise ValueError(reason)


def root_path():
    for parent in Path(__file__).resolve().parents:
        if (parent/'design/FEATURE_POLICY.md').is_file():
            return parent
    raise ValueError('repository root unavailable')


def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds').replace('+00:00','Z')


def identity(data):
    return {'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()}


def canonical(value, cap):
    data=(json.dumps(value,sort_keys=True,separators=(',',':'),ensure_ascii=True)+'\n').encode('ascii')
    require(len(data)<=cap,'canonical document byte cap')
    return data


def read(path, cap):
    require(not path.is_symlink() and path.is_file(),'fixed ordinary input file: '+str(path))
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NONBLOCK',0)|getattr(os,'O_NOFOLLOW',0))
    with os.fdopen(descriptor,'rb') as stream:
        require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode),'ordinary input descriptor')
        data=stream.read(cap+1)
    require(len(data)<=cap,'post-read logical byte cap: '+str(path))
    return data


def tool_identity(path, cutoff):
    require(path.is_file() and len(os.fsencode(path))<=4096,'resolved direct tool path')
    total=0;hashed=hashlib.sha256()
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NONBLOCK',0))
    with os.fdopen(descriptor,'rb') as stream:
        require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode),'ordinary direct tool descriptor')
        while True:
            cutoff()
            chunk=stream.read(min(65536,TOOL_CAP-total+1))
            if not chunk:
                break
            total+=len(chunk)
            require(total<=TOOL_CAP,'direct tool observed byte cap')
            hashed.update(chunk)
    return {'path_hex':os.fsencode(path).hex(),'bytes':total,'sha256':hashed.hexdigest()}


def expected_labels():
    labels=['cleanup-descendant','check-application','emit-application',
            'build-application-ordinary','build-application-sanitized',
            'emit-production-producer','build-production-producer']
    labels.extend(f'C{i:02d}-{variant}' for i in range(1,65) for variant in ('ordinary','sanitized'))
    labels.extend(('argument-fifo-C08','argument-fifo-C09'))
    labels.extend(f'G{n}-{variant}' for n in (64,128,256,512) for variant in ('ordinary','sanitized'))
    labels.extend(f'oom-{n:02d}-{variant}' for n in range(1,33) for variant in ('ordinary','sanitized'))
    labels.extend(f'{name}-{role}' for name in ('compiler','ledger','catalog')
                  for role in ('capture','select-ordinary','select-sanitized'))
    require(len(labels)==218 and len(set(labels))==218,'literal native label geometry')
    return tuple(labels)


class Assets:
    def __init__(self):
        self.files={}
        self.bytes=0

    def add(self, data, cap):
        require(type(data) is bytes and len(data)<=cap,'raw asset type/size')
        pin=identity(data);name='assets/'+pin['sha256']+'.bin'
        if name in self.files:
            require(self.files[name]==data,'same-hash assets require exact byte equality')
        else:
            require(len(self.files)<FILE_CAP-6,'asset file admission reserves model/four endpoint docs/receipt')
            require(len(data)<=DATA_CAP-self.bytes,'raw asset aggregate admission')
            self.files[name]=data;self.bytes+=len(data)
        return {'file':name,**pin}


class Freeze:
    def __init__(self, root, output, compiler, cc, begin, admission):
        require(type(begin) is int and 0<begin<=time.monotonic_ns(),
                'absolute original caller monotonic begin; no timer reset')
        self.root=root;self.output=output;self.begin=begin;self.admission=admission
        self.deadline=begin+GLOBAL_SECONDS*1000000000
        self.tool_paths={'compiler':compiler,'cc':cc,'python':Path(sys.executable).resolve(strict=True)}
        self.assets=Assets();self.inventory={};self.stage='source-admission'
        self.receipt={'schema':1,'status':'running','started_utc':utc(),'native_commands':0,
                      'global_seconds':GLOBAL_SECONDS,'unknown':UNKNOWN,'files':self.inventory,
                      'started_utc_scope':'Freeze construction; elapsed_ns uses original caller begin including prior admission'}

    def cutoff(self):
        if time.monotonic_ns()>=self.deadline:
            raise TimeoutError('fixed whole900 data deadline')

    def endpoints(self, sources, controls, tools):
        after_sources={}
        self.receipt['sources_after']=after_sources
        for path in SOURCE_PATHS:
            self.cutoff();after_sources[path]=identity(read(self.root/path,SOURCE_CAP))
        after_controls={}
        self.receipt['controls_after']=after_controls
        for label,path in CONTROLS.items():
            self.cutoff();after_controls[label]=identity(read(self.root/path,SOURCE_CAP))
        after_tools={}
        self.receipt['tools_after']=after_tools
        for label,path in self.tool_paths.items():
            after_tools[label]=tool_identity(path,self.cutoff)
        require(after_sources==sources and after_controls==controls and after_tools==tools,
                'complete source/control/tool endpoint equality')
        return after_sources,after_controls,after_tools

    def publish(self, name, data):
        self.cutoff()
        require(name not in self.inventory and len(self.inventory)<FILE_CAP-1,'nonself publication file cap')
        require(len(data)<=INPUT_ASSET_CAP,'publication per-file logical cap')
        path=self.output/name;path.parent.mkdir(parents=True,exist_ok=True)
        with path.open('xb') as stream:
            stream.write(data)
        self.inventory[name]=identity(data)
        require(read(path,INPUT_ASSET_CAP)==data,'fresh complete publication readback')

    def observed_files(self):
        observed={};entries=0
        self.receipt['observed_nonself_files']=observed
        for path in self.output.rglob('*'):
            self.cutoff();entries+=1
            require(entries<=FILE_CAP+4 and not path.is_symlink(),'bounded ordinary published tree')
            if path.is_file() and path!=self.output/'freeze.json':
                name=str(path.relative_to(self.output));observed[name]=identity(read(path,INPUT_ASSET_CAP))
        return observed

    def card(self, card):
        require(set(card)=={'label','input','queries','capture_argument','status','stdout','stderr','reason','authority'},
                'fixed card field shape')
        require(card['capture_argument'] in ('input','absent','missing'),'fixed capture argv role')
        require(type(card['status']) is int and card['status'] in (0,64,65,66),'fixed scalar status')
        require(type(card['queries']) is tuple and len(card['queries'])<=4096 and
                all(type(q) is bytes and len(q)<=65 for q in card['queries']),'finite argv fixture domain')
        require(type(card['stderr']) is bytes and card['stderr']==b'','literal empty stderr')
        require(type(card['stdout']) is bytes and len(card['stdout'])<=OUTPUT_ASSET_CAP,'expected stdout bytes')
        require(all(type(card[key]) is str and len(card[key])<=4096 for key in ('label','reason','authority')),
                'bounded card metadata')
        result={key:card[key] for key in ('label','capture_argument','status','reason','authority')}
        result['queries_hex']=[query.hex() for query in card['queries']]
        result['input']=None if card['input'] is None else self.assets.add(card['input'],INPUT_ASSET_CAP)
        if card['status']==0:
            result['stdout']=self.assets.add(card['stdout'],OUTPUT_ASSET_CAP)
        else:
            require(len(card['stdout'])<=4096,'bounded inline application diagnostic')
            result['stdout']={'inline_hex':card['stdout'].hex(),**identity(card['stdout'])}
        result['stderr']={'inline_hex':'',**identity(b'')}
        return result

    def _prepare(self):
        self.cutoff()
        require(self.root==root_path(),'fixed actual repository root')
        require(self.output.parent.resolve(strict=True).is_relative_to((self.root/'build').resolve(strict=True)) and
                self.output.is_dir() and not self.output.is_symlink() and not any(self.output.iterdir()),
                'fresh empty ordinary ignored-build data subtree')
        require(len(SELFHOST)==35 and len(LEDGER)==9 and len(CATALOG)==13,'literal real body domains')
        require(len(SOURCE_PATHS)==67 and len(set(SOURCE_PATHS))==67,'literal complete fixed readset67')
        require(type(self.admission) is dict and set(self.admission)=={'sources','controls','tools'},
                'caller-owned complete captured admission shape')
        require(type(self.admission['sources']) is dict and set(self.admission['sources'])==set(SOURCE_PATHS) and
                type(self.admission['controls']) is dict and set(self.admission['controls'])==set(CONTROLS) and
                type(self.admission['tools']) is dict and set(self.admission['tools'])==set(self.tool_paths),
                'complete67-source/nine-control/three-tool admission before oracle')
        captured=dict(self.admission['sources']);controls=dict(self.admission['controls'])
        require(all(type(data) is bytes and len(data)<=SOURCE_CAP for data in (*captured.values(),*controls.values())),
                'bounded immutable captured source/control bytes')
        require(Path(__file__).resolve()==self.root/CONTROLS['freezer'],'fixed canonical freezer location')
        require(identity(controls['oracle'])['sha256']==ORACLE_SHA,'fixed independent literal oracle identity')
        require(identity(controls['application_manifest'])['sha256']==APPLICATION_MANIFEST_SHA,
                'fixed eleven-module application registry, not a decoded manifest')
        for path,pin in {**MANIFEST_SHA,**FIXED_SHA}.items():
            require(identity(captured[path])['sha256']==pin,'fixed declaration/helper byte identity')
        require(b'Status: accepted' in captured['design/rfcs/0165-checked-project-input-producer.md'].splitlines()[:24] and
                b'Status: accepted' in controls['rfc'].splitlines()[:24],
                'current accepted producer/selector contracts')
        source_pins={path:identity(data) for path,data in captured.items()}
        control_pins={label:identity(data) for label,data in controls.items()}
        tools={label:dict(pin) for label,pin in self.admission['tools'].items()}
        require(all(set(pin)=={'path_hex','bytes','sha256'} and type(pin['bytes']) is int and
                    0<=pin['bytes']<=TOOL_CAP and type(pin['sha256']) is str and
                    len(pin['sha256'])==64 and all(c in '0123456789abcdef' for c in pin['sha256']) and
                    pin['path_hex']==os.fsencode(self.tool_paths[label]).hex()
                    for label,pin in tools.items()),'exact fixed current direct-tool pin shape/paths')
        require(sum(len(data) for data in captured.values())+sum(len(data) for data in controls.values())<=DATA_CAP,
                'complete opaque custody byte admission before oracle execution')
        self.receipt.update(sources_before=source_pins,controls_before=control_pins,tools_before=tools)
        self.endpoints(source_pins,control_pins,tools)
        # The entire fixed source/control/tool barrier precedes captured module
        # execution. Standard-library imports are not transitive attestation.
        self.stage='independent-oracle'
        oracle_path=self.root/CONTROLS['oracle'];oracle_data=controls['oracle']
        require(read(oracle_path,SOURCE_CAP)==oracle_data,'captured oracle before-exec bytes')
        name='rfc172_current_source_context_independent_data'
        require(name not in sys.modules,'fresh fixed module name')
        module=ModuleType(name);module.__file__=str(oracle_path);sys.modules[name]=module
        try:
            exec(compile(oracle_data,str(oracle_path),'exec',dont_inherit=True),module.__dict__)
            require(read(oracle_path,SOURCE_CAP)==oracle_data,'captured oracle after-exec bytes')
            require((module.CASE_CAP,module.GEOMETRY_CASES,module.REAL_CASES,module.TOTAL_CASE_CAP)==(64,4,3,72),
                    'fixed independent case domains')
            require((module.DATA_CAP,module.FILE_CAP,module.MODEL_CAP)==(DATA_CAP,FILE_CAP,MODEL_CAP),
                    'unchanged independent storage bounds')
            cards=module.campaign();geometry=module.geometry();labels=module.labels()
            require(type(cards) is tuple and len(cards)==64 and
                    tuple(row['label'] for row in cards)==tuple(f'C{i:02d}' for i in range(1,65)),'fixed64 card sequence')
            require(type(geometry) is tuple and len(geometry)==4 and
                    tuple(row['label'] for row in geometry)==('G64','G128','G256','G512'),'fixed4 geometry sequence')
            require(labels==expected_labels(),'fixed218 prospective native labels')
            require(set(module.REAL)==set(REAL_LAYOUT),'fixed3 real registry names')
            rows=[self.card(row) for row in cards]
            geometric=[self.card(row) for row in geometry]
            real=[]
            for key,(manifest_path,base,paths,queries) in REAL_LAYOUT.items():
                self.cutoff()
                declaration=module.REAL[key]
                require(type(declaration) is tuple and len(declaration)==5 and
                        declaration[:3]==(manifest_path,base,MANIFEST_SHA[manifest_path]) and
                        declaration[4]==queries,'independent fixed real custody declaration')
                require({base+'/'+row[1] for row in declaration[3]}==set(paths),
                        'independent body registry equals pre-execution fixed captured domain')
                bodies={path:captured[path] for path in paths}
                bundle=module.real_bundle(key,captured[manifest_path],bodies)
                require(bundle['status']==0 and bundle['stderr']==b'' and
                        bundle['queries']==tuple(q.encode('ascii') for q in queries),
                        'literal real bundle/status/query association')
                require(all(type(bundle[field]) is int and 0<=bundle[field]<=1000000000
                            for field in ('modules','edges','module_body_bytes','selected_body_bytes')),
                        'observed data metadata scalar admission')
                real.append({'name':key,'manifest_path':manifest_path,'body_paths':list(paths),
                             'queries_hex':[q.hex() for q in bundle['queries']],
                             'capture':self.assets.add(bundle['capture'],OUTPUT_ASSET_CAP),
                             'graph':self.assets.add(bundle['graph'],MIB),
                             'stdout':self.assets.add(bundle['stdout'],OUTPUT_ASSET_CAP),
                             'stderr':{'inline_hex':'',**identity(b'')},'status':0,
                             **{field:bundle[field] for field in ('modules','edges','module_body_bytes','selected_body_bytes','source_authority')}})
        finally:
            sys.modules.pop(name,None)
        source_refs={path:self.assets.add(data,SOURCE_CAP) for path,data in captured.items()}
        control_refs={label:self.assets.add(data,SOURCE_CAP) for label,data in controls.items()}
        model={'schema':1,'format':'rfc172-fixed-source-context-data-1',
               'cards':rows,'geometry':geometric,'real':real,'native_labels':list(labels),
               'constructor_cards':64,'geometry_cases':4,'real_cases':3,'data_scenarios':71,
               'source_refs':source_refs,'control_refs':control_refs,'source_pins':source_pins,
               'control_pins':control_pins,'tools':tools,'unknown':UNKNOWN,
               'limits':{'constructor_cards':64,'geometry_cases':4,'real_cases':3,'total_scenarios':72,
                         'files':FILE_CAP,'data_bytes':DATA_CAP,'model_bytes':MODEL_CAP,
                         'source_file_bytes':SOURCE_CAP,'direct_tool_bytes':TOOL_CAP,
                         'global_seconds':GLOBAL_SECONDS,'child_seconds':60},
               'envelopes':{'valid_capture_bytes_upper':6627047,'valid_output_bytes_upper':5578446,
                            'scope':'conservative validator-built input/output bounds; no actual work/native result'}}
        model_bytes=canonical(model,MODEL_CAP)
        files=dict(self.assets.files)
        files['model.json']=model_bytes
        files['sources-before.json']=canonical({'sources':source_pins,'controls':control_pins},DOC_CAP)
        files['tools-before.json']=canonical(tools,DOC_CAP)
        after_sources,after_controls,after_tools=self.endpoints(source_pins,control_pins,tools)
        files['sources-after.json']=canonical({'sources':after_sources,'controls':after_controls},DOC_CAP)
        files['tools-after.json']=canonical(after_tools,DOC_CAP)
        require(len(files)+1<=FILE_CAP and sum(map(len,files.values()))+RECEIPT_CAP<=DATA_CAP,
                'exact generated storage plus reserved final receipt admitted before publication')
        self.stage='publication'
        for name,data in sorted(files.items()):
            self.publish(name,data)
        observed=self.observed_files()
        require(observed==self.inventory and set(observed)==set(files),'exact complete nonself published inventory')
        # Last source/control/direct-tool check is after every published artifact
        # readback and inventory check; receipt selfwrite is explicitly excluded.
        final_sources,final_controls,final_tools=self.endpoints(source_pins,control_pins,tools)
        self.cutoff()
        self.receipt.update(status='data-held-no-native',stage='complete',model=identity(model_bytes),
                            sources_after=final_sources,controls_after=final_controls,tools_after=final_tools,
                            constructor_cards=64,geometry_cases=4,real_cases=3,data_scenarios=71,native_labels=218,
                            files_including_receipt=len(self.inventory)+1,
                            bytes_before_receipt=sum(pin['bytes'] for pin in self.inventory.values()),
                            finished_utc=utc(),elapsed_ns=time.monotonic_ns()-self.begin,
                            endpoint_scope='all recorded source/control/tool/artifact bytes through publication verification; own receipt write excluded',
                            timing_scope='data preparation through final endpoints; own receipt write/shutdown excluded')
        receipt_bytes=canonical(self.receipt,RECEIPT_CAP)
        require(sum(pin['bytes'] for pin in self.inventory.values())+len(receipt_bytes)<=DATA_CAP,'final receipt-inclusive data admission')
        self.cutoff()
        with (self.output/'freeze.json').open('xb') as stream:
            stream.write(receipt_bytes)
        require(read(self.output/'freeze.json',RECEIPT_CAP)==receipt_bytes,'final receipt readback')
        self.cutoff()
        return {'status':'data-held-no-native','model':identity(model_bytes),'freeze':identity(receipt_bytes),
                'files':len(self.inventory)+1,'bytes':self.receipt['bytes_before_receipt']+len(receipt_bytes),
                'native_commands':0,'elapsed_ns':self.receipt['elapsed_ns']}


    def retain_failure(self, error):
        self.receipt.update(status='failed-or-incomplete',stage=self.stage,
                            error=(type(error).__name__+': '+str(error))[:4096],
                            finished_utc=utc(),elapsed_ns=time.monotonic_ns()-self.begin,
                            endpoint_scope='partial retained observations only; failed hold is never source/native acceptance')
        try:
            self.observed_files()
            if all(len(self.receipt.get(key,{}))==count for key,count in
                   (('sources_before',67),('controls_before',9),('tools_before',3))):
                self.endpoints(self.receipt['sources_before'],self.receipt['controls_before'],self.receipt['tools_before'])
        except BaseException as secondary:
            self.receipt['retention_error']=(type(secondary).__name__+': '+str(secondary))[:4096]
        if not (self.output/'freeze.json').exists():
            try:
                data=canonical(self.receipt,RECEIPT_CAP)
                require(len(self.inventory)+1<=FILE_CAP and
                        sum(pin['bytes'] for pin in self.inventory.values())+len(data)<=DATA_CAP,'failed receipt-inclusive admission')
                with (self.output/'freeze.json').open('xb') as stream:
                    stream.write(data)
            except BaseException:
                pass

    def run(self):
        """Own exactly one fresh data directory under the caller's deadline."""
        self.cutoff()
        require(self.root==root_path(),'fixed actual repository root')
        build=(self.root/'build').resolve(strict=True);parent=self.output.parent.resolve(strict=True)
        require(parent.is_relative_to(build) and 6<=len(self.output.name)<=96 and
                self.output.name.isascii() and self.output.name[0].isalpha() and
                self.output.name.endswith('.held') and
                all(c.isalnum() or c in '-_.' for c in self.output.name) and
                len(os.fsencode(self.output))<=4096 and not self.output.exists() and not self.output.is_symlink(),
                'exclusive fresh ordinary ignored-build .held subtree')
        self.output=parent/self.output.name
        self.output.mkdir()
        try:
            return self._prepare()
        except BaseException as error:
            try:
                self.retain_failure(error)
            except BaseException:
                pass
            raise


def capture_admission(root, compiler, cc, cutoff, receipt=None):
    """Read fixed opaque bytes/direct identities only; no candidate execution."""
    require(compiler==(root/'build/toolchain/slimc').resolve(strict=True) and
            cc==Path('/usr/bin/cc').resolve(strict=True),'fixed current compiler and CC')
    admission={'sources':{},'controls':{},'tools':{}}
    pins={'sources_before':{},'controls_before':{},'tools_before':{}}
    if receipt is not None:
        receipt.update(pins)
    for path in SOURCE_PATHS:
        cutoff();data=read(root/path,SOURCE_CAP)
        admission['sources'][path]=data;pins['sources_before'][path]=identity(data)
    for label,path in CONTROLS.items():
        cutoff();data=read(root/path,SOURCE_CAP)
        admission['controls'][label]=data;pins['controls_before'][label]=identity(data)
    for label,path in {'compiler':compiler,'cc':cc,'python':Path(sys.executable).resolve(strict=True)}.items():
        cutoff();admission['tools'][label]=tool_identity(path,cutoff)
        pins['tools_before'][label]=admission['tools'][label]
    require(sum(map(len,admission['sources'].values()))+sum(map(len,admission['controls'].values()))<=DATA_CAP,
            'complete caller custody byte admission before oracle')
    return admission


def main():
    begin=time.monotonic_ns();root=root_path()
    args=sys.argv[1:]
    require(len(args)==6 and args[0]=='--output' and args[2]=='--compiler' and args[4]=='--cc',
            'exact output/compiler/cc argument pairs required')
    require(all(0<len(os.fsencode(value))<=4096 for value in (args[1],args[3],args[5])),'fixed argument byte bounds')
    compiler=Path(args[3]).resolve(strict=True);cc=Path(args[5]).resolve(strict=True)
    require(compiler==(root/'build/toolchain/slimc').resolve(strict=True) and
            cc==Path('/usr/bin/cc').resolve(strict=True),'fixed current compiler and CC identities only')
    build=root/'build';build.mkdir(parents=True,exist_ok=True);build=build.resolve(strict=True)
    output=Path(args[1]).absolute();parent=output.parent.resolve(strict=True)
    require(parent.is_relative_to(build) and 6<=len(output.name)<=96 and
            output.name.isascii() and output.name[0].isalpha() and output.name.endswith('.held') and
            all(c.isalnum() or c in '-_.' for c in output.name),'fixed fresh ignored-build .held subtree')
    output=parent/output.name
    require(not output.exists() and not output.is_symlink(),'fresh ignored data output')
    freeze=Freeze(root,output,compiler,cc,begin,{'sources':{},'controls':{},'tools':{}})
    def alarm(_signum,_frame):
        raise TimeoutError('fixed whole900 data deadline')
    old_handler=signal.signal(signal.SIGALRM,alarm)
    signal.setitimer(signal.ITIMER_REAL,max(0.000001,(freeze.deadline-time.monotonic_ns())/1000000000))
    try:
        freeze.admission=capture_admission(root,compiler,cc,freeze.cutoff,freeze.receipt)
        result=freeze.run()
    except BaseException as error:
        if not output.exists():
            # Source/tool admission failed before run owned a data directory.
            # A fixed fresh failure receipt does not regenerate or retry data.
            try:
                output.mkdir()
                freeze.retain_failure(error)
            except BaseException:
                pass
        message='source-context-freeze: failed or incomplete; stage='+freeze.stage+'; '+type(error).__name__+': '+str(error)
        print(message.encode('utf-8','backslashreplace')[:4096].decode('utf-8','ignore'),file=sys.stderr)
        return 1
    finally:
        previous_mask=signal.pthread_sigmask(signal.SIG_BLOCK,{signal.SIGALRM})
        try:
            signal.setitimer(signal.ITIMER_REAL,0)
            while signal.SIGALRM in signal.sigpending():
                signal.sigwait({signal.SIGALRM})
            signal.signal(signal.SIGALRM,old_handler)
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK,previous_mask)
    print(json.dumps(result,sort_keys=True))
    return 0


if __name__=='__main__':
    sys.dont_write_bytecode=True
    try:
        raise SystemExit(main())
    except (ValueError,OSError) as error:
        message='source-context-freeze: '+str(error)
        print(message.encode('utf-8','backslashreplace')[:4096].decode('utf-8','ignore'),file=sys.stderr)
        raise SystemExit(64)
