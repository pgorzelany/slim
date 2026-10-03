#!/usr/bin/env python3
"""Explicit finite RFC168 native campaign; held data never accepts SLIM source.

Fresh data is frozen before native dispatch, or supplied by an explicitly pinned
hold. No retries, adjustable budgets, recorded executable inputs, source parser,
or semantic fallback. Every child uses a cleaned process group and live bounds.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import importlib.util
import json
import math
import os
from pathlib import Path
import reprlib
import resource
import selectors
import shutil
import signal
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
sys.dont_write_bytecode = True
MIB = 1048576
GLOBAL_SECONDS = 900
CHILD_SECONDS = 60
STDOUT_CAP = 8 * MIB
STDERR_CAP = 256 * 1024
EMIT_CAP = 16 * MIB
FILE_CAP = 128 * MIB
ORACLE_SHA = '91cecce2601e0052ec28a32292b8c89fab533b6c2a6c4c85e0097f744ede4997'
VARIANTS = (('ordinary', ('-O2', '-DNDEBUG')),
            ('sanitized', ('-O1', '-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all')))
WORK_KEYS = tuple(side + '_' + field for side in ('old', 'new')
                  for field in ('queue_pushes', 'queue_pops', 'edge_visits', 'link_headers'))
OUTPUT_ROOT = ROOT / 'build/overnight-project-impact'
MODULES = (
 ('catalog_data','library/applications/catalog/catalog.slim','catalog_model framed_records std_byte_index std_decimal','load'),
 ('catalog_emit','library/applications/catalog/emit.slim','catalog_model std_byte_index std_netstring std_text','append_record'),
 ('catalog_model','library/applications/catalog/model.slim','std_byte_index','Catalog Loaded Record Selection Status'),
 ('catalog_reconcile','library/applications/catalog/reconcile.slim','catalog_model std_byte_index std_bytes','Change Diff Prepared prepare'),
 ('framed_records','library/components/records.slim','std_decimal std_netstring','Parsed Triple addition_fits integer parse'),
 ('project_impact_app','library/applications/project_impact/main.slim','project_impact_input project_impact_model std_text','error'),
 ('project_impact_closure','library/applications/project_impact/closure.slim','project_impact_model workplan_model','addition_fits admitted close product_fits zeros'),
 ('project_impact_data','library/applications/project_impact/data.slim','catalog_model project_impact_model std_ascii std_byte_index std_bytes workplan_model','associate reserved reserved_key shape'),
 ('project_impact_input','library/applications/project_impact/input.slim','catalog_data catalog_model project_impact_data project_impact_model project_impact_prepare workplan_load workplan_model','prepare'),
 ('project_impact_model','library/applications/project_impact/model.slim','catalog_model workplan_model','Association Attempt Closure Prepared Result Snapshot Work'),
 ('project_impact_prepare','library/applications/project_impact/prepare.slim','catalog_model catalog_reconcile project_impact_closure project_impact_data project_impact_model project_impact_report std_byte_index std_bytes workplan_model','prepare'),
 ('project_impact_probe','tests/fixtures/project_impact_probe.slim','project_impact_closure project_impact_input project_impact_model std_bytes std_decimal std_text',''),
 ('project_impact_report','library/applications/project_impact/report.slim','catalog_emit catalog_model catalog_reconcile project_impact_model std_byte_index std_netstring std_text workplan_model','finish'),
 ('std_ascii','library/experimental/ascii.slim','','digit_value is_alphanumeric is_digit'),
 ('std_byte_index','library/experimental/byte_index.slim','','Bounds Built Entry Index build compare find prefix'),
 ('std_bytes','library/experimental/bytes.slim','','append append_range equal find_byte range_equal'),
 ('std_decimal','library/experimental/decimal.slim','std_ascii','Parsed parse_i64'),
 ('std_netstring','library/experimental/netstring.slim','std_ascii std_bytes std_text','Frame Parsed append parse'),
 ('std_text','library/experimental/text.slim','std_bytes','append_bytes append_i64'),
 ('workplan_load','library/applications/workplan/load.slim','framed_records std_ascii std_byte_index std_bytes std_decimal workplan_model','load'),
 ('workplan_model','library/applications/workplan/model.slim','std_byte_index','Edge Graph Loaded Plan Schedule Scheduled Status Step Task'),
)


class Deadline(ValueError):
    pass


def require(value, reason):
    if not value:
        raise ValueError(reason)


def utc():
    return datetime.now(timezone.utc).isoformat(timespec='microseconds').replace('+00:00', 'Z')


def read(path, maximum=FILE_CAP):
    path = Path(path)
    require(path.is_file() and not path.is_symlink(), 'ordinary pinned file required: ' + str(path))
    with path.open('rb') as stream:
        value = stream.read(maximum + 1)
    require(len(value) <= maximum, 'pinned file byte bound: ' + str(path))
    return value


def digest(value):
    return hashlib.sha256(value).hexdigest()


def identity(value):
    return {'bytes': len(value), 'sha256': digest(value)}


def encoded(value):
    return (json.dumps(value, sort_keys=True, separators=(',', ':'), ensure_ascii=True) + '\n').encode('ascii')


def write(path, value):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(value)
    require(read(path) == value, 'fresh materialized byte readback')


def module(relative, expected, name):
    path = ROOT / relative
    captured = read(path)
    require(digest(captured) == expected, 'fixed helper source drift before import')
    from types import ModuleType
    result = ModuleType(name)
    result.__file__ = str(path)
    sys.modules[name] = result
    exec(compile(captured, str(path), 'exec', dont_inherit=True), result.__dict__)
    require(digest(read(path)) == expected, 'fixed helper source drift after import')
    return result


def source_pins():
    paths = {Path(__file__).resolve(), ROOT/'scripts/project-impact-oracle.py', ROOT/'scripts/project-input-inventory.py',
             ROOT/'library/project-impact.project', ROOT/'project-input.project', ROOT/'selfhost/slim.project',
             ROOT/'library/catalog.project', ROOT/'library/applications/catalog/main.slim',
             ROOT/'library/applications/catalog/diff_emit.slim',
             ROOT/'design/rfcs/0168-declared-import-project-impact.md', ROOT/'runtime/slim_rt.c', ROOT/'runtime/slim_rt.h'}
    paths.update(ROOT/path for _,path,_,_ in MODULES)
    paths.update(ROOT.glob('selfhost/*.slim'))
    paths.update(ROOT.glob('library/components/project_input_*.slim'))
    paths.update(ROOT.glob('library/applications/project_input/*.slim'))
    return {str(path.relative_to(ROOT)): identity(read(path)) for path in sorted(paths)}


def directory_pins(folder):
    return {str(path.relative_to(folder)): identity(read(path))
            for path in sorted(folder.rglob('*')) if path.is_file()}


def summary(error):
    small = reprlib.Repr()
    small.maxstring = small.maxother = 512
    small.maxtuple = small.maxlist = 8
    return (type(error).__name__ + ': ' + small.repr(error.args))[:4096]


def process(argv, directory, seconds, environment, stdout_cap):
    """Separate live pipe caps; always kill the group and reap the direct child."""
    require(0 < seconds <= CHILD_SECONDS and stdout_cap in (STDOUT_CAP, EMIT_CAP), 'fixed process bounds')
    directory.mkdir(parents=True, exist_ok=False)
    started_utc, begin = utc(), time.monotonic_ns()
    result = {'argv':list(map(str,argv)), 'status':'ok', 'returncode':None,
              'started_utc':started_utc, 'seconds_limit':seconds,
              'stdout_limit':stdout_cap, 'stderr_limit':STDERR_CAP, 'file_limit':FILE_CAP,
              'group_kill_issued':False, 'direct_child_reaped':False,
              'timing_scope':'dispatch through bounded drain, group cleanup and direct wait; excludes artifact publication'}
    stdout, stderr = bytearray(), bytearray()
    child = None
    deadline = time.monotonic() + seconds
    previous_mask = None
    def limits():
        if previous_mask is not None:
            signal.pthread_sigmask(signal.SIG_SETMASK, previous_mask)
        signal.pthread_sigmask(signal.SIG_UNBLOCK, {signal.SIGALRM})
        resource.setrlimit(resource.RLIMIT_CPU, (math.ceil(seconds)+1, math.ceil(seconds)+1))
        resource.setrlimit(resource.RLIMIT_FSIZE, (FILE_CAP, FILE_CAP))
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    try:
        # A pending overall alarm is delivered only after we own the child
        # handle, so its finally path can clean the newly created group.
        previous_mask = signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGALRM})
        try:
            child = subprocess.Popen(result['argv'], cwd=directory, env=environment, stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE, start_new_session=True, preexec_fn=limits)
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK, previous_mask)
        result['pid'] = child.pid
        with selectors.DefaultSelector() as selector:
            for pipe, sink, cap in ((child.stdout,stdout,stdout_cap),(child.stderr,stderr,STDERR_CAP)):
                os.set_blocking(pipe.fileno(), False)
                selector.register(pipe, selectors.EVENT_READ, (sink,cap))
            while selector.get_map() and result['status'] == 'ok':
                if time.monotonic() >= deadline:
                    result['status'] = 'timeout'
                    break
                for key,_ in selector.select(min(0.05,max(0,deadline-time.monotonic()))):
                    sink,cap = key.data
                    chunk = os.read(key.fd,min(65536,cap-len(sink)+1))
                    if not chunk:
                        selector.unregister(key.fileobj)
                    elif len(sink)+len(chunk) > cap:
                        result['status'] = 'output-limit'
                        break
                    else:
                        sink.extend(chunk)
            if result['status'] == 'ok':
                if time.monotonic() >= deadline:
                    result['status'] = 'timeout'
                else:
                    try:
                        child.wait(timeout=max(0.001,deadline-time.monotonic()))
                    except subprocess.TimeoutExpired:
                        result['status'] = 'timeout'
    except Deadline as error:
        result.update(status='timeout',reason=summary(error))
    except (OSError,ValueError) as error:
        result.update(status='infrastructure-error',reason=summary(error))
    finally:
        # Do not let the overall alarm interrupt kill/reap or publication of
        # the attempted child's receipt. A pending alarm is admitted only after
        # that bounded cleanup and retained receipt exist.
        cleanup_mask = signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGALRM})
        try:
            if child is not None:
                try:
                    os.killpg(child.pid,signal.SIGKILL)
                    result['group_kill_issued'] = True
                except ProcessLookupError:
                    result['group_already_absent'] = True
                except OSError as error:
                    result.update(status='cleanup-error',group_kill_error=summary(error))
                    try:
                        child.kill()
                    except OSError as fallback:
                        result['direct_kill_error'] = summary(fallback)
                try:
                    child.wait(timeout=1)
                    result['direct_child_reaped'] = True
                    result['returncode'] = child.returncode
                except subprocess.TimeoutExpired:
                    result.update(status='cleanup-error',reason='direct child not reaped after group kill within fixed1s cleanup bound')
                finally:
                    child.stdout.close()
                    child.stderr.close()
            result.update(finished_utc=utc(),elapsed_ns=time.monotonic_ns()-begin)
            if result['elapsed_ns'] > seconds*1000000000 and result['status'] == 'ok':
                result['status'] = 'timeout'
            if result['status'] == 'ok' and result['returncode'] != 0:
                result['status'] = 'native-error'
            for label,value in (('stdout',bytes(stdout)),('stderr',bytes(stderr))):
                write(directory/(label+'.bin'),value)
                result[label] = identity(value)
            write(directory/'process.json',encoded(result))
        finally:
            try:
                signal.pthread_sigmask(signal.SIG_SETMASK, cleanup_mask)
            except Deadline as error:
                result.update(status='timeout',reason=summary(error))
                (directory/'process.json').write_bytes(encoded(result))
    return result,bytes(stdout),bytes(stderr)


class Campaign:
    def __init__(self,output,receipt,begin):
        self.output,self.receipt,self.begin = output,receipt,begin
        self.deadline = begin + GLOBAL_SECONDS*1000000000

    def cutoff(self):
        if time.monotonic_ns() >= self.deadline:
            raise Deadline('fixed 900-second campaign limit; remaining gates unknown')

    def save(self):
        (self.output/'receipt.json').write_bytes(encoded(self.receipt))

    def execute(self,label,argv,environment,cap=STDOUT_CAP,seconds=CHILD_SECONDS):
        self.cutoff()
        remaining = (self.deadline-time.monotonic_ns())/1000000000
        before_utc,begin = utc(),time.monotonic_ns()
        attempted = {'label':label,'argv':list(map(str,argv)),'status':'incomplete',
                     'wrapper_started_utc':before_utc,'reason':'dispatch result not yet retained'}
        self.receipt['commands'].append(attempted)
        self.save()
        # The child uses the remaining whole-campaign deadline as its own cap.
        # Keep the parent alarm pending until group cleanup and both receipts
        # are retained, including when it expires between drain and finally.
        prior_mask = signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGALRM})
        try:
            result,stdout,stderr = process(argv,self.output/'commands'/label,min(seconds,remaining),environment,cap)
            result.update(label=label,wrapper_started_utc=before_utc,wrapper_finished_utc=utc(),
                          wrapper_elapsed_ns=time.monotonic_ns()-begin,
                          wrapper_scope='process dispatch, drain, cleanup, direct reap and process artifact publication')
            attempted.clear()
            attempted.update(result)
            self.save()
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK, prior_mask)
        self.cutoff()
        return result,stdout,stderr

    def exact(self,label,argv,environment,expected,cap=STDOUT_CAP):
        gate = self.receipt['gates'][label]
        result,stdout,stderr = self.execute(label,argv,environment,cap)
        gate.pop('reason',None)
        gate.update(process_status=result['status'],returncode=result['returncode'],stdout=identity(stdout),stderr=identity(stderr))
        if result['status'] not in ('ok','native-error') or result['returncode'] is None:
            gate.update(status='unknown',reason='bounded invocation incomplete')
        elif (result['returncode'],stdout,stderr) != expected:
            gate.update(status='failed',reason='completed invocation disagrees with fixed byte/status oracle')
        else:
            gate['status'] = 'pass'
        self.save()
        return gate,stdout


def verify_hold(folder,oracle,model_sha,freeze_sha):
    require(folder.is_dir() and not folder.is_symlink(),'held directory')
    freeze_bytes,model_bytes = read(folder/'freeze.json',4*MIB),read(folder/'model.json',4*MIB)
    require(digest(model_bytes) == model_sha and digest(freeze_bytes) == freeze_sha,'explicit held model/freeze identity')
    freeze,model = json.loads(freeze_bytes),json.loads(model_bytes)
    require(freeze['oracle_sha256'] == ORACLE_SHA and model['source_sha256'] == ORACLE_SHA
            and freeze['model_sha256'] == model_sha,'held oracle/model association')
    require(model['rfc'] == 168 and model['case_count'] == 124 and model['cli_case_count'] == 127,'fixed finite campaign dimensions')
    require(model['bounds'] == {'module':4095,'edges':65536,'file_bytes':MIB,'source_bytes':4*MIB,
                              'report_bytes':8*MIB,'counter':1000000000,'case_count':128,
                              'materialized_bytes':64*MIB,'model_bytes':4*MIB},'fixed model bounds')
    require(len(freeze['files']) <= 1024 and sum(row['bytes'] for row in freeze['files'].values()) <= 64*MIB,'finite held artifacts')
    for name,expected in freeze['files'].items():
        path = Path(name)
        require(not path.is_absolute() and '..' not in path.parts,'held relative artifact')
        require(identity(read(folder/path,16*MIB)) == expected,'held artifact identity: '+name)
    observed = directory_pins(folder)
    require(set(observed) == set(freeze['files']) | {'freeze.json'},'held complete artifact set')
    require(model['source_pins'] == {name:digest(value) for name,value in oracle.fixed_sources(ROOT).items()},'held source bytes differ from current stopped snapshot')
    return model,observed


def probe_project(output,pins):
    folder = output/'probe-project'
    declarations = []
    for name,path,imports,exports in MODULES:
        if name == 'project_impact_app':
            continue
        value = read(ROOT/path,MIB)
        require(identity(value) == pins[path],'probe source pin')
        write(folder/path,value)
        declarations.append(f'  (module {name} "{path}" (imports {imports}) (exports {exports}))\n')
    manifest = ('(project 1\n  (entry project_impact_probe)\n'+''.join(declarations)+')\n').encode()
    write(folder/'slim.project',manifest)
    return folder/'slim.project'


def frame(value):
    return str(len(value)).encode('ascii')+b':'+value+b','


def real_oracles(folder,oracle,adapter_pin,output):
    result = {}
    for label,declarations in (('compiler',oracle.COMPILER),('catalog',oracle.CATALOG)):
        for side in ('old','new'):
            source_folder = folder/'real'/label/side
            manifest = read(source_folder/'slim.project',MIB)
            rows = [(name.encode(),path.encode(),read(source_folder/path,MIB),tuple(x.encode() for x in imports.split()))
                    for name,path,imports in declarations]
            graph = b''.join(frame(x) for name,path,source,imports in rows
                             for x in (name,str(len(source)).encode(),b','.join(imports)))
            catalog = b''.join(frame(x) for x in (b'@project',str(len(manifest)).encode(),b'slim.project\0'+digest(manifest).encode()))
            catalog += b''.join(frame(x) for name,path,source,imports in rows
                                for x in (name,str(len(source)).encode(),path+b'\0'+digest(source).encode()))
            edges = sum(len(imports) for name,path,source,imports in rows)
            transport = b''.join(frame(x) for x in (b'slim-project-input-1',str(len(rows)).encode(),str(edges).encode(),manifest,graph))
            transport += b''.join(frame(x) for name,path,source,imports in rows for x in (name,path,source))
            metadata = {'schema':1,'format':'slim-project-input-1','module_count':len(rows),'direct_import_edges':edges,
                        'source_bytes':len(manifest)+sum(len(row[2]) for row in rows),'manifest':identity(manifest),
                        'modules':[{'name':name.decode(),'path_hex':path.hex(),**identity(source)} for name,path,source,imports in rows]}
            receipt = {**metadata,'adapter':adapter_pin,'capture':identity(transport),'catalog':identity(catalog),'workplan':identity(graph),
                       'authority':'transport measurements only; source acceptance requires matching successful trusted producer invocation',
                       'consumer_acceptance':'not established; existing catalog/workplan consumers remain authoritative',
                       'source_identity_scope':'observed adapter source bytes through receipt assembly/publication; no loaded bytecode or ABA attestation',
                       'capture_scope':'single bounded read of serialized bytes; no source-path reread or atomic live capture claim'}
            case_folder = folder/'cases'/('real-'+label+'-one-newline')
            cat_stage,graph_stage = ('before-catalog','before-graph') if side == 'old' else ('after-catalog','after-graph')
            require(catalog == read(case_folder/cat_stage,MIB) and graph == read(case_folder/graph_stage,MIB),'real byte oracle/held case relation')
            require(len(transport) <= 8*MIB and len(encoded(receipt)) <= 4*MIB,'real transport/adapter receipt bounds')
            for kind,value in (('transport',transport),('catalog',catalog),('graph',graph),('inventory',encoded(receipt))):
                write(output/'expected-real'/label/side/kind,value)
            result[(label,side)] = (source_folder/'slim.project',transport,catalog,graph,metadata,encoded(receipt))
    return result


def scope_proxies(model,real):
    rows = []
    for label in ('compiler','catalog'):
        case = next(row for row in model['cases'] if row['name'] == 'real-'+label+'-one-newline')
        selected = {bytes.fromhex(row['hex']).decode('ascii') for row in case['facts']['affected']}
        metadata = real[(label,'new')][4]
        modules = metadata['modules']
        rows.append({'project':label,'classification':'exact','selected_current_modules':len(selected),
                     'current_module_count':len(modules),
                     'selected_current_catalog_weight_bytes':sum(row['bytes'] for row in modules if row['name'] in selected),
                     'all_current_module_bytes':sum(row['bytes'] for row in modules),
                     'basis':'held affected report facts and current catalog byte weights, matched to frozen literal serialization',
                     'limit':'deterministic declared-import scope proxy; not saved compile time or agent effectiveness'})
    return rows


def expected_controls(output,model):
    # Materialize these literal model projections before the first child, too;
    # native observations never participate in constructing expected bytes.
    for case in model['cases']:
        if case['status'] == 0:
            value = (' '.join(str(case['work'][key]) for key in WORK_KEYS)+'\n').encode('ascii')
            write(output/'expected-controls'/('work-'+case['name']+'.stdout'),value)
    counters = (' '.join('1' if row['admitted'] else '0' for row in model['counter_arithmetic_controls'])+'\n').encode('ascii')
    write(output/'expected-controls'/'counters.stdout',counters)


def cleanup_control(campaign):
    # Fixed test infrastructure source, not an arbitrary recorded executable.
    source = (b'import os,sys,time\n'
              b'child=os.fork()\n'
              b'if child==0:\n'
              b' os.close(0);os.close(1);os.close(2);time.sleep(120);os._exit(0)\n'
              b'print(child,flush=True)\n')
    path = campaign.output/'cleanup-control.py'
    write(path,source)
    return path,source


def check_cleanup(campaign,path,environment):
    result,stdout,stderr = campaign.execute('group-cleanup-control',[sys.executable,path],environment,seconds=2)
    gate = campaign.receipt['gates']['group-cleanup-control']
    gate.pop('reason',None)
    gate.update(process_status=result['status'],returncode=result['returncode'])
    if result['status'] == 'ok' and result['returncode'] == 0 and stderr == b'' and stdout.endswith(b'\n'):
        try:
            require(stdout[:-1].isdigit(),'fixed cleanup child PID data')
            pid = int(stdout[:-1])
            require(pid > 1,'fixed cleanup child PID domain')
            until = time.monotonic()+1
            gone = False
            while time.monotonic() < until:
                try:
                    os.kill(pid,0)
                except ProcessLookupError:
                    gone = True
                    break
                time.sleep(0.01)
            gate.update(status='pass' if gone else 'failed',descendant_absent=gone,
                        scope='one fixed fork control closes all descendant streams; <=1s post-kill absence check')
        except Exception as error:
            gate.update(status='failed',reason=summary(error))
    else:
        gate.update(status='unknown' if result['status'] not in ('ok','native-error') else 'failed',reason='fixed cleanup control did not complete exactly')
    campaign.save()
    require(gate['status'] == 'pass','unconditional native group cleanup control failed')


def case_args(folder,case):
    return [folder/'cases'/case['name']/stage for stage in ('before-catalog','before-graph','after-catalog','after-graph')]


def planned(model):
    labels = []
    for program in ('app','probe','producer'):
        labels += [program+'-check',program+'-emit']+[program+'-'+variant+'-build' for variant,_ in VARIANTS]
    for variant,_ in VARIANTS:
        labels += ['case-'+variant+'-'+case['name'] for case in model['cases']]
        labels += ['work-'+variant+'-'+case['name'] for case in model['cases'] if case['status'] == 0]
        labels += ['args-'+variant+'-'+card['name'] for card in model['argument_controls']]
        labels += ['limit-'+variant+'-'+card['name'] for card in model['helper_output_controls']]
        labels += ['fifo-'+variant+'-'+card['name'] for card in model['fifo_precedence_controls']]
        labels += ['counters-'+variant]
        labels += ['real-'+variant+'-'+label+'-'+side for label in ('compiler','catalog') for side in ('old','new')]
        labels += ['workflow-'+variant+'-'+label for label in ('compiler','catalog')]
    labels += ['real-check-'+label+'-'+side for label in ('compiler','catalog') for side in ('old','new')]
    labels += ['group-cleanup-control']
    return labels


def build(campaign,compiler,cc,environment,probe):
    programs = {}
    for name,manifest in (('app',ROOT/'library/project-impact.project'),('probe',probe),('producer',ROOT/'project-input.project')):
        gate,_ = campaign.exact(name+'-check',[compiler,'check',manifest],environment,(0,b'',b''))
        require(gate['status'] == 'pass','production checker did not accept '+name)
        result,generated,stderr = campaign.execute(name+'-emit',[compiler,manifest],environment,EMIT_CAP)
        gate = campaign.receipt['gates'][name+'-emit']
        gate.pop('reason',None)
        gate.update(process_status=result['status'],returncode=result['returncode'],
                    stdout=identity(generated),stderr=identity(stderr))
        if result['status'] == 'ok' and result['returncode'] == 0 and not stderr and generated:
            gate.update(status='pass',generated_c=identity(generated))
        else:
            gate.update(status='failed' if result['status'] in ('ok','native-error') else 'unknown',reason='production C emission incomplete or rejected')
        campaign.save()
        require(gate['status'] == 'pass','production emission did not complete: '+name)
        require(not generated.startswith(b'#define SLIM_PARALLEL 1\n'),'fixed serial native campaign')
        source = campaign.output/(name+'.c')
        write(source,generated)
        campaign.receipt.setdefault('generated_c',{})[name] = {'path':str(source),**identity(generated)}
        campaign.save()
        for variant,flags in VARIANTS:
            target = campaign.output/(name+'-'+variant)
            gate,_ = campaign.exact(name+'-'+variant+'-build',
                [cc,'-std=c11','-Wall','-Wextra','-Werror',*flags,'-I',ROOT/'runtime',source,ROOT/'runtime/slim_rt.c','-o',target],
                environment,(0,b'',b''))
            require(gate['status'] == 'pass','native build did not complete: '+name+'/'+variant)
            require(read(source) == generated,'generated C drift')
            programs[(name,variant)] = target
            campaign.receipt.setdefault('native_programs',{})[name+'-'+variant] = {'path':str(target),**identity(read(target))}
            campaign.save()
    return programs


def native(campaign,model,folder,real,compiler,programs,environment,adapter):
    cases = {case['name']:case for case in model['cases']}
    for variant,_ in VARIANTS:
        app,probe = programs[('app',variant)],programs[('probe',variant)]
        for case in model['cases']:
            expected = read(folder/'cases'/case['name']/'stdout',STDOUT_CAP)
            require(identity(expected) == case['stdout'],'held expected report hash')
            campaign.exact('case-'+variant+'-'+case['name'],[app,*case_args(folder,case)],environment,(case['status'],expected,b''))
            if case['status'] == 0:
                expected_work = read(campaign.output/'expected-controls'/('work-'+case['name']+'.stdout'))
                gate,stdout = campaign.exact('work-'+variant+'-'+case['name'],[probe,'work',*case_args(folder,case)],environment,(0,expected_work,b''))
                if gate['status'] == 'pass':
                    observed = list(map(int,stdout.split()))
                    require(len(observed) == 8 and all(0 <= value <= 1000000000 for value in observed),'native counter observation domain')
                    gate['observed_work'] = dict(zip(WORK_KEYS,observed))
                    campaign.save()
        base = case_args(folder,cases['unchanged-chain'])
        for card in model['argument_controls']:
            count = card['input_path_count']
            argv = [app,*base[:count]] if count <= 4 else [app,*base,base[0]]
            campaign.exact('args-'+variant+'-'+card['name'],argv,environment,(64,read(folder/card['stdout_artifact']),b''))
        for card in model['helper_output_controls']:
            campaign.exact('limit-'+variant+'-'+card['name'],[probe,'limit',str(card['output_limit']),*case_args(folder,cases[card['input_case']])],
                           environment,(card['native_status'],read(folder/card['stdout_artifact']),b''))
        for card in model['fifo_precedence_controls']:
            case = cases[card['input_case']]
            args = case_args(folder,case)
            fifo = campaign.output/'fifo'/(variant+'-'+card['name'])
            fifo.parent.mkdir(parents=True,exist_ok=True)
            os.mkfifo(fifo)
            args[card['fifo_slot']] = fifo
            campaign.exact('fifo-'+variant+'-'+card['name'],[app,*args],environment,
                           (card['native_status'],read(folder/'cases'/case['name']/'stdout'),b''))
        expected_counter = read(campaign.output/'expected-controls'/'counters.stdout')
        gate,_ = campaign.exact('counters-'+variant,[probe,'counters'],environment,(0,expected_counter,b''))
        gate['scope'] = 'I64 helper domain plus ordinary decimal decoder rejection for UINT64_MAX and Boolean spelling'
        campaign.save()
    for label in ('compiler','catalog'):
        for side in ('old','new'):
            manifest,transport,catalog,graph,metadata,expected_receipt = real[(label,side)]
            campaign.exact('real-check-'+label+'-'+side,[compiler,'check',manifest],environment,(0,b'',b''))
            for variant,_ in VARIANTS:
                gate,stdout = campaign.exact('real-'+variant+'-'+label+'-'+side,[programs[('producer',variant)],manifest],environment,(0,transport,b''))
                if gate['status'] == 'pass':
                    try:
                        actual = adapter.inventory(stdout)
                        require(actual == (catalog,graph,metadata),'serialized-byte adapter inventory differs')
                        destination = campaign.output/'inventory'/variant/label/side
                        write(destination/'capture',stdout)
                        adapter.convert(destination/'capture',destination/'catalog',destination/'graph',destination/'receipt.json')
                        require(read(destination/'catalog') == catalog and read(destination/'graph') == graph
                                and read(destination/'receipt.json') == expected_receipt,'exact adapter output/receipt bytes')
                        artifacts = campaign.receipt.setdefault('inventory_artifacts',{})
                        for kind,value in (('capture',stdout),('catalog',catalog),('graph',graph),('receipt.json',expected_receipt)):
                            artifacts[str((destination/kind).relative_to(campaign.output))] = identity(value)
                        gate.update(inventory={'catalog':identity(catalog),'graph':identity(graph),'receipt':identity(expected_receipt)},
                                    authority='matching successful checked producer invocation; inventory hashes supplied serialized bytes only')
                    except Exception as error:
                        gate.update(status='failed',reason='serialized-byte adapter verification failed',failure=summary(error))
                        if isinstance(error,Deadline):
                            campaign.save()
                            raise
                    campaign.save()
        for variant,_ in VARIANTS:
            paths = [campaign.output/'inventory'/variant/label/side/kind for side in ('old','new') for kind in ('catalog','graph')]
            producers_passed = all(campaign.receipt['gates']['real-'+variant+'-'+label+'-'+side]['status'] == 'pass'
                                   for side in ('old','new'))
            if producers_passed and all(path.is_file() for path in paths):
                case = cases['real-'+label+'-one-newline']
                campaign.exact('workflow-'+variant+'-'+label,[programs[('app',variant)],*paths],environment,
                               (0,read(folder/'cases'/case['name']/'stdout'),b''))
            else:
                campaign.receipt['gates']['workflow-'+variant+'-'+label].update(
                    status='unknown',reason='required exact successful producer/adapter inputs unavailable')
                campaign.save()


def geometry(receipt,model):
    rows = []
    for variant,_ in VARIANTS:
        for family in ('chain','fanout','cycle','shared','prefix64'):
            selected = [case for case in model['cases'] if case['family'] == family]
            measurements = []
            for case in selected:
                gate = receipt['gates']['work-'+variant+'-'+case['name']]
                if gate['status'] == 'pass':
                    dims = case['facts']['dimensions'][0]
                    measurements.append((dims['modules']+dims['edges']+1,sum(gate['observed_work'].values())))
            row = {'variant':variant,'family':family,'status':'unknown','limit':1.15,'reason':'one or more fixed metrics gates incomplete or mismatched'}
            if len(measurements) == 5:
                exponent = math.log(measurements[-1][1]/measurements[0][1])/math.log(measurements[-1][0]/measurements[0][0])
                row.update(status='pass' if exponent <= 1.15 else 'failed',exponent=exponent,measurements=measurements)
                row.pop('reason',None)
            rows.append(row)
    return rows


def main(argv=None):
    started_utc,begin = utc(),time.monotonic_ns()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--held',type=Path)
    parser.add_argument('--held-model-sha')
    parser.add_argument('--held-freeze-sha')
    parser.add_argument('--compiler',type=Path,default=ROOT/'build/toolchain/slimc')
    parser.add_argument('--cc',default=os.environ.get('CC','cc'),help='one trusted executable name/path; no shell or flags')
    options = parser.parse_args(argv)
    output = options.output.resolve()
    require(output.is_relative_to(OUTPUT_ROOT.resolve()) and output != OUTPUT_ROOT.resolve(),'fresh ignored output descendant')
    output.mkdir(parents=True,exist_ok=False)
    receipt = {'schema':1,'rfc':168,'status':'running','started_utc':started_utc,'commands':[],'gates':{},
               'bounds':{'global_seconds':900,'child_seconds':60,'stdout_bytes':STDOUT_CAP,
               'stderr_bytes':STDERR_CAP,'emit_stdout_bytes':EMIT_CAP,'child_file_bytes':FILE_CAP,'retries':0,'budget_changes_after_observation':0},
               'timing_scope':'same-process monotonic whole campaign through final pin checks; final receipt publication/interpreter shutdown excluded',
               'unknown':{'semantic_recheck':'declared graph candidates confer no incremental compiler authority',
                          'parsed_node_boundary':'no measured 1000000-node crossing',
                          'physical_resources':'logical caps do not prove RSS, libc allocation, host I/O or CPU bounds',
                          'general_efficacy':'finite exact workflow only; no productivity or universal performance claim',
                          'filesystem_capture':'stopped writers and byte pins do not prove atomic capture or exclude ABA',
                          'toolchain':'direct executable pins do not attest SDK/transitive toolchain identity'}}
    campaign = Campaign(output,receipt,begin)
    oracle = folder = model = programs = cleanup = None
    tools = {}
    def expired(_signum,_frame):
        raise Deadline('fixed 900-second whole-campaign alarm')
    previous = signal.signal(signal.SIGALRM,expired)
    signal.setitimer(signal.ITIMER_REAL,max(0.001,GLOBAL_SECONDS-(time.monotonic_ns()-begin)/1000000000))
    try:
        receipt['source_before'] = source_pins()
        oracle = module('scripts/project-impact-oracle.py',ORACLE_SHA,'project_impact_finite_oracle')
        if options.held:
            require(options.held_model_sha and options.held_freeze_sha,'explicit held model and freeze hashes required')
            folder = options.held.resolve(strict=True)
            model_sha,freeze_sha = options.held_model_sha,options.held_freeze_sha
        else:
            require(not options.held_model_sha and not options.held_freeze_sha,'held hashes require held folder')
            folder = output/'frozen'
            oracle.freeze(folder)
            model_sha,freeze_sha = digest(read(folder/'model.json')),digest(read(folder/'freeze.json'))
        model,held_pins = verify_hold(folder,oracle,model_sha,freeze_sha)
        receipt['held'] = {'path':str(folder),'model_sha256':model_sha,'freeze_sha256':freeze_sha,'files':held_pins}
        compiler = options.compiler.resolve(strict=True)
        cc = Path(shutil.which(options.cc) or options.cc).resolve(strict=True)
        tools = {key:{'path':str(path),**identity(read(path))} for key,path in
                 (('compiler',compiler),('cc',cc),('python',Path(sys.executable).resolve(strict=True)))}
        receipt['tools_before'] = tools
        probe = probe_project(output,receipt['source_before'])
        real = real_oracles(folder,oracle,receipt['source_before']['scripts/project-input-inventory.py'],output)
        receipt['scope_proxies'] = scope_proxies(model,real)
        expected_controls(output,model)
        cleanup,cleanup_source = cleanup_control(campaign)
        receipt['cleanup_control_before'] = identity(cleanup_source)
        receipt['gates'] = {label:{'status':'unknown','reason':'not executed'} for label in planned(model)}
        preflight = {'schema':1,'bounds':receipt['bounds'],'source_pins':receipt['source_before'],'tools':tools,
                     'held_model_sha256':model_sha,'held_freeze_sha256':freeze_sha,'gates':list(receipt['gates']),
                     'probe_and_real_expected_files':{'probe-project/'+name:value for name,value in directory_pins(output/'probe-project').items()} | {
                         'expected-real/'+name:value for name,value in directory_pins(output/'expected-real').items()} | {
                         'expected-controls/'+name:value for name,value in directory_pins(output/'expected-controls').items()},
                     'cleanup_control':identity(cleanup_source),'scope_proxies':receipt['scope_proxies']}
        write(output/'campaign-before-native.json',encoded(preflight))
        receipt['campaign_plan_before'] = identity(read(output/'campaign-before-native.json'))
        receipt['prepared_before_native'] = directory_pins(output/'probe-project')
        receipt['real_expected_before_native'] = directory_pins(output/'expected-real')
        receipt['control_expected_before_native'] = directory_pins(output/'expected-controls')
        require(source_pins() == receipt['source_before'] and directory_pins(folder) == held_pins,'source/held drift before native')
        campaign.cutoff()
        environment = dict(os.environ,LC_ALL='C',LANG='C',ASAN_OPTIONS='detect_leaks=0:abort_on_error=1',UBSAN_OPTIONS='halt_on_error=1')
        for name in ('SLIM_ALLOC_FAIL_AT','SLIM_TASK_FAIL_AT','SLIM_TASK_JOIN_FAIL_AT','SLIM_TASK_DISABLE'):
            environment.pop(name,None)
        adapter = module('scripts/project-input-inventory.py',receipt['source_before']['scripts/project-input-inventory.py']['sha256'],'project_impact_inventory_adapter')
        receipt['producer_source'] = {'project.slim':receipt['source_before']['selfhost/project.slim'],
                                     'authority':'producer compiled from pinned current SLIM source; control compiler separately pinned'}
        campaign.save()
        check_cleanup(campaign,cleanup,environment)
        programs = build(campaign,compiler,cc,environment,probe)
        native(campaign,model,folder,real,compiler,programs,environment,adapter)
        receipt['geometry'] = geometry(receipt,model)
        receipt['status'] = 'pass' if all(gate['status'] == 'pass' for gate in receipt['gates'].values()) and all(row['status'] == 'pass' for row in receipt['geometry']) else 'failed'
    except Exception as error:
        receipt.update(status='timeout' if isinstance(error,Deadline) else 'failed',failure=summary(error))
    finally:
        try:
            receipt['source_after'] = source_pins()
            unchanged = 'source_before' in receipt and receipt['source_after'] == receipt['source_before']
            if folder is not None and 'held' in receipt:
                receipt['held_after'] = directory_pins(folder)
                unchanged = unchanged and receipt['held_after'] == receipt['held']['files']
            if tools:
                receipt['tools_after'] = {key:{'path':row['path'],**identity(read(row['path']))} for key,row in tools.items()}
                unchanged = unchanged and receipt['tools_after'] == tools
            if 'native_programs' in receipt:
                receipt['native_programs_after'] = {name:{'path':row['path'],**identity(read(row['path']))}
                                                    for name,row in receipt['native_programs'].items()}
                unchanged = unchanged and receipt['native_programs_after'] == receipt['native_programs']
            if 'generated_c' in receipt:
                receipt['generated_c_after'] = {name:{'path':row['path'],**identity(read(row['path']))}
                                                for name,row in receipt['generated_c'].items()}
                unchanged = unchanged and receipt['generated_c_after'] == receipt['generated_c']
            if 'inventory_artifacts' in receipt:
                receipt['inventory_artifacts_after'] = {name:identity(read(output/name)) for name in receipt['inventory_artifacts']}
                unchanged = unchanged and receipt['inventory_artifacts_after'] == receipt['inventory_artifacts']
            if 'prepared_before_native' in receipt:
                receipt['prepared_after'] = directory_pins(output/'probe-project')
                receipt['real_expected_after'] = directory_pins(output/'expected-real')
                receipt['control_expected_after'] = directory_pins(output/'expected-controls')
                unchanged = unchanged and receipt['prepared_after'] == receipt['prepared_before_native']
                unchanged = unchanged and receipt['real_expected_after'] == receipt['real_expected_before_native']
                unchanged = unchanged and receipt['control_expected_after'] == receipt['control_expected_before_native']
            if 'campaign_plan_before' in receipt:
                receipt['campaign_plan_after'] = identity(read(output/'campaign-before-native.json'))
                unchanged = unchanged and receipt['campaign_plan_after'] == receipt['campaign_plan_before']
            if cleanup is not None:
                receipt['cleanup_control_after'] = identity(read(cleanup))
                unchanged = unchanged and receipt['cleanup_control_after'] == receipt['cleanup_control_before']
            receipt['pins_unchanged'] = unchanged
            if not unchanged:
                receipt.update(status='failed',pin_failure='source/tool/held/generated input identities changed')
        except Exception as error:
            receipt.update(status='timeout' if isinstance(error,Deadline) else 'failed',pin_failure=summary(error))
        finally:
            signal.setitimer(signal.ITIMER_REAL,0)
            signal.signal(signal.SIGALRM,previous)
        receipt.update(finished_utc=utc(),elapsed_ns=time.monotonic_ns()-begin)
        if receipt['elapsed_ns'] > GLOBAL_SECONDS*1000000000:
            receipt.update(status='timeout',failure='whole campaign exceeded fixed 900 seconds')
        campaign.save()
    print('project-impact: '+receipt['status'].upper()+'; retained '+str(output/'receipt.json'))
    return 0 if receipt['status'] == 'pass' else 1


if __name__ == '__main__':
    sys.exit(main())
