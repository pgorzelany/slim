#!/usr/bin/env python3
"""Stdlib-only pilot transport, ordinary compiler execution and measurement.

This is evaluation infrastructure, not a SLIM semantic implementation. The
production compiler accepts source; independently frozen oracle data accepts
the bounded task. Participant tool mode never loads or prints oracle material.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time

sys.dont_write_bytecode = True

BASE = Path(__file__).resolve().parent
ROOT = BASE.parent.parent
PUBLIC_DOCS = ['DESIGN.md', 'design/FEATURE_POLICY.md', 'docs/CORE.md', 'docs/PROJECTS.md', 'docs/HOST.md', 'docs/DIAGNOSTICS.md', 'docs/CONTEXT.md']


class CandidateConstraint(ValueError):
    """A known fixed-file/protocol validation failure, not infrastructure."""


def now():
    return datetime.now(timezone.utc).isoformat()


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def save(path, value):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    staging = path.with_name(path.name + '.next')
    staging.write_text(json.dumps(value, indent=2, sort_keys=True) + '\n')
    staging.replace(path)


def read(path):
    return json.loads(Path(path).read_text())


def corpus():
    paths = [p for p in BASE.rglob('*') if p.is_file() and '__pycache__' not in p.parts]
    # Trial outputs and freeze receipts belong under ignored build/, not here.
    return {str(p.relative_to(ROOT)):sha(p) for p in sorted(paths)} | {
        name:sha(ROOT/name) for name in PUBLIC_DOCS}


def manifest():
    return read(BASE/'manifest.json')


def task(task_id):
    matches = [t for t in manifest()['tasks'] if t['id'] == task_id]
    if len(matches) != 1:
        raise ValueError('unknown task')
    return matches[0]


def command(argv, timeout=30):
    started = time.monotonic()
    try:
        result = subprocess.run(list(map(str, argv)), cwd=ROOT, capture_output=True, timeout=timeout)
        return {'argv':list(map(str,argv)), 'returncode':result.returncode,
                'timeout':False, 'seconds':time.monotonic()-started,
                'stdout':result.stdout.decode('latin1'), 'stderr':result.stderr.decode('latin1')}
    except subprocess.TimeoutExpired as error:
        return {'argv':list(map(str,argv)), 'returncode':None, 'timeout':True,
                'seconds':time.monotonic()-started,
                'stdout':(error.stdout or b'').decode('latin1'),
                'stderr':(error.stderr or b'').decode('latin1')}
    except OSError as error:
        return {'argv':list(map(str,argv)), 'returncode':None, 'timeout':False,
                'seconds':time.monotonic()-started,'stdout':'','stderr':'',
                'infrastructure_error':type(error).__name__+': '+str(error)}


def successful(result):
    return result['returncode'] == 0 and not result['timeout']


def toolchain_identity(compiler):
    compiler = Path(compiler).resolve(strict=True)
    if not os.access(compiler, os.X_OK):
        raise ValueError('compiler must already exist and be executable; no implicit bootstrap')
    cc = Path(shutil.which(os.environ.get('CC','cc')) or '').resolve(strict=True)
    return {'compiler_path':str(compiler), 'compiler_sha256':sha(compiler),
            'cc_path':str(cc), 'cc_sha256':sha(cc),
            'runtime':{n:sha(ROOT/'runtime'/n) for n in ['slim_rt.c','slim_rt.h']}}


def copy_sources(source, destination, definition):
    destination.mkdir(parents=True,exist_ok=False)
    for name in definition['files']:
        path = source/name
        if path.is_symlink() or not path.is_file():
            raise CandidateConstraint('candidate source must be an ordinary allowed file')
        shutil.copyfile(path,destination/name)


def candidate_files(source, definition):
    result = {}
    for name in definition['files']:
        path = source/name
        if path.is_symlink() or not path.is_file():
            raise CandidateConstraint('candidate source must be an ordinary allowed file')
        result[name] = path.read_bytes()
    declaration_paths = re.findall(rb'\(module ([A-Za-z_][A-Za-z_0-9]*) "([^"]+)"',result['slim.project'])
    expected = sorted((Path(name).stem.encode(),name.encode()) for name in definition['files'] if name.endswith('.slim'))
    if declaration_paths != expected:
        raise CandidateConstraint('candidate manifest must use the four fixed module-relative filenames')
    return result


def captured_identity(directory, metadata):
    identity = metadata['toolchain']
    return (sha(directory/'toolchain/slimc')==identity['compiler_sha256']
            and sha(identity['cc_path'])==identity['cc_sha256']
            and all(sha(directory/'runtime'/n)==s for n,s in identity['runtime'].items()))


def build(compiler, cc, runtime, source, scratch):
    scratch.mkdir(parents=True,exist_ok=True)
    emitted = command([compiler,source/'slim.project'])
    stages = [emitted]
    if not successful(emitted):
        return None, stages
    generated, executable = scratch/'program.c', scratch/'program'
    generated.write_bytes(emitted['stdout'].encode('latin1'))
    emitted.update(stdout_sha256=hashlib.sha256(emitted['stdout'].encode('latin1')).hexdigest(),
                   stdout_bytes=len(emitted['stdout']),stdout='',output_role='internal-generated-c')
    native = command([cc,'-std=c11','-O3','-DNDEBUG','-Wall','-Wextra','-Werror',
                      '-I',runtime,generated,runtime/'slim_rt.c','-o',executable])
    stages.append(native)
    return executable if successful(native) else None, stages


def oracle_module():
    spec = importlib.util.spec_from_file_location('pilot_acceptance',BASE/'oracles/acceptance.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def client_manifest(source):
    # Transport alteration of a fixed fixture manifest, not semantic resolution.
    modules = re.findall(r'\(module ([A-Za-z_][A-Za-z_0-9]*) ',source)
    if sorted(modules) != modules or len(modules) != 4 or 'app' not in modules:
        raise CandidateConstraint('task manifest module boundaries changed')
    imports = ' '.join(name for name in modules if name != 'app')
    changed,count = re.subn(r'(\(module app "app.slim" \(imports)[^)]*(\) \(exports\)\))',
                            lambda m:m[1]+' '+imports+m[2],source)
    if count != 1:
        raise CandidateConstraint('task entry manifest shape changed')
    return changed


def evaluate_candidate(task_id, candidate, compiler, cc, runtime, result=None):
    definition, oracle = task(task_id), oracle_module()
    if result is None: result = {}
    result.update({'schema':1,'task':task_id,'started':now(),'components':{},'commands':[],
              'evidence':'bounded-fixed-task-oracle','model_tokens':'unknown-not-captured',
              'model_tool_calls':'unknown-not-captured','repair_iterations':'unknown-not-captured'})
    files = candidate_files(candidate,definition)
    result['source_sha256'] = {name:hashlib.sha256(value).hexdigest() for name,value in files.items()}
    result['source_bytes'] = sum(map(len,files.values()))
    initial = BASE/'tasks'/task_id/'initial'
    edits = {}
    for name,after in files.items():
        before = (initial/name).read_bytes()
        prefix = 0
        while prefix < min(len(before),len(after)) and before[prefix] == after[prefix]: prefix += 1
        suffix = 0
        while suffix < min(len(before),len(after))-prefix and before[-1-suffix] == after[-1-suffix]: suffix += 1
        edits[name] = {'removed_bytes':len(before)-prefix-suffix,'inserted_bytes':len(after)-prefix-suffix}
    result['edit_spans'] = edits
    constraints = oracle.source_constraints(task_id,files)
    result['components']['source_form_constraints'] = {'passed':not constraints,'failures':constraints,
        'evidence':'lexical-source-form-guard; normal checker supplies semantic acceptance',
        'unproven':'full dynamic composition beyond fixed source forms and finite client matrix'}
    with tempfile.TemporaryDirectory(prefix='slim-pilot-oracle-') as temporary:
        directory = Path(temporary)
        original = directory/'original'
        copy_sources(candidate,original,definition)
        checked = command([compiler,'check',original/'slim.project'])
        result['commands'].append(checked)
        result['components']['complete_project_check'] = {'passed':successful(checked)}
        interface = command([compiler,'interfaces',original/'slim.project'])
        result['commands'].append(interface)
        result['components']['public_contract'] = {'passed':successful(interface) and interface['stdout'].encode('latin1') == oracle.INTERFACES[task_id]}
        executable,stages = build(compiler,cc,runtime,original,directory/'application-build')
        result['commands'] += stages
        app = command([executable]) if executable else None
        if app: result['commands'].append(app)
        result['components']['original_application'] = {'passed':bool(app and successful(app) and app['stderr']=='' and app['stdout'].encode('latin1')==oracle.APPLICATION_STDOUT[task_id])}
        client = directory/'client'
        copy_sources(candidate,client,definition)
        (client/'slim.project').write_text(client_manifest((client/'slim.project').read_text()))
        source,expected,domain = oracle.CLIENTS[task_id]()
        (client/'app.slim').write_bytes(source)
        executable,stages = build(compiler,cc,runtime,client,directory/'client-build')
        result['commands'] += stages
        observed = command([executable]) if executable else None
        if observed: result['commands'].append(observed)
        result['components']['independent_client_matrix'] = {'passed':bool(observed and successful(observed) and observed['stderr']=='' and observed['stdout'].encode('latin1')==expected), 'domain':domain,
            'expected_stdout_sha256':hashlib.sha256(expected).hexdigest()}
        for probe in oracle.misuse_clients(task_id):
            (client/'app.slim').write_text(probe['positive'])
            positive = command([compiler,'check',client/'slim.project'])
            result['commands'].append(positive)
            (client/'app.slim').write_text(probe['source'])
            rejected = command([compiler,'check',client/'slim.project'])
            result['commands'].append(rejected)
            issues = re.findall(r'(E[0-9]{4})@app@([0-9]+):([0-9]+)',rejected['stdout'])
            expected = [(probe['code'],*map(str,probe['span']))]
            result['components']['misuse-'+probe['name']] = {'passed':successful(positive) and rejected['returncode']==1 and not rejected['timeout'] and issues==expected,
                'positive_control_accepted':successful(positive),'required_diagnostic':probe['code'],
                'required_span':probe['span'],'observed_diagnostics':issues,'domain':probe['domain']}
    result['accepted'] = all(c['passed'] for c in result['components'].values())
    failures = [c['infrastructure_error'] for c in result['commands'] if 'infrastructure_error' in c]
    if failures:
        result['infrastructure_error'] = '; '.join(failures)
        result['accepted'] = False
    result['finished'] = now()
    return result


def verify_fixtures(args):
    identity = toolchain_identity(args.compiler)
    records = []
    for definition in manifest()['tasks']:
        task_id = definition['id']
        initial = BASE/'tasks'/task_id/'initial'
        checked = command([args.compiler,'check',initial/'slim.project'])
        expected_check = definition['initial_check']=='accepted'
        if successful(checked) != expected_check:
            raise ValueError('initial fixture checker expectation failed: '+task_id)
        reference = evaluate_candidate(task_id,BASE/'references'/task_id,Path(args.compiler).resolve(),Path(identity['cc_path']),ROOT/'runtime')
        broken = evaluate_candidate(task_id,initial,Path(args.compiler).resolve(),Path(identity['cc_path']),ROOT/'runtime')
        if not reference['accepted'] or broken['accepted']:
            save(args.output,{'passed':False,'task':task_id,'reference':reference,'initial':broken})
            raise ValueError('reference/oracle feasibility failed: '+task_id+'; inspect coordinator output')
        records.append({'task':task_id,'reference':reference,'initial':broken})
        print(json.dumps({'task':task_id,'initial_check':checked['returncode'],'initial_task_accepted':False,'reference_task_accepted':True}),flush=True)
    receipt = {'schema':1,'passed':True,'timestamp':now(),'toolchain':identity,'corpus':corpus(),'records':records}
    save(args.output,receipt)


def freeze(args):
    verified = read(args.verification)
    identity = toolchain_identity(args.compiler)
    if not verified['passed'] or verified['corpus'] != corpus() or verified['toolchain'] != identity:
        raise ValueError('freeze needs current successful exact-corpus fixture verification')
    save(args.output,{'schema':1,'frozen':now(),'corpus':corpus(),'toolchain':identity,
                      'verification_sha256':sha(args.verification),'manifest':manifest()})
    print(json.dumps({'frozen':str(Path(args.output).resolve()),'tasks':3,'trials':6,'sha256':sha(args.output)}))


def prepare(args):
    frozen = read(args.freeze)
    identity = toolchain_identity(args.compiler)
    if frozen['corpus'] != corpus() or frozen['toolchain'] != identity:
        raise ValueError('frozen corpus/compiler/runtime changed; no participant may start')
    trials = [t for t in frozen['manifest']['trials'] if t['id']==args.trial]
    if len(trials)!=1: raise ValueError('unknown frozen trial')
    trial = trials[0]
    directory = Path(args.destination).resolve()
    directory.mkdir(parents=True,exist_ok=False)
    copy_sources(BASE/'tasks'/trial['task']/'initial',directory/'candidate',task(trial['task']))
    (directory/'toolchain').mkdir()
    shutil.copyfile(args.compiler,directory/'toolchain/slimc')
    (directory/'toolchain/slimc').chmod(0o700)
    (directory/'runtime').mkdir()
    for name in identity['runtime']: shutil.copyfile(ROOT/'runtime'/name,directory/'runtime'/name)
    (directory/'docs').mkdir()
    for name in PUBLIC_DOCS: shutil.copyfile(ROOT/name,directory/'docs'/Path(name).name)
    shutil.copyfile(BASE/'tasks'/trial['task']/'TASK.md',directory/'TASK.txt')
    metadata = {'schema':1,**trial,'freeze_path':str(Path(args.freeze).resolve()),'freeze_sha256':sha(args.freeze),
                'prepared':now(),'toolchain':identity,'started':None,'submitted':None,'operations':[],
                'model':frozen['manifest']['model'],'reasoning_effort':frozen['manifest']['reasoning_effort'],
                'human_repair_interventions':[],'protocol_violations':[],
                'model_tokens':'unknown-not-captured','model_tool_calls':'unknown-not-captured'}
    save(directory/'trial.json',metadata)
    treatment = 'You may also use compiler context via the wrapper.' if trial['condition']=='context' else 'Semantic context is unavailable in this baseline. Do not invoke it by any route.'
    prompt = f'''Work only in {directory}/candidate on the task in {directory}/TASK.txt.
Use the supplied {directory}/docs public language/project/context documentation.
Read DESIGN.md, CORE.md and FEATURE_POLICY.md there before editing, as required
by repository AGENTS.md. The same documents are supplied in both conditions.
You are one fresh participant in a frozen feasibility pilot. Do not inspect the
evaluator implementation, oracles, references, other runs, repository compiler
implementation, git history or parent conversation. These files share a filesystem;
this is an advisory restriction, not a secure blind. Do not modify trial metadata,
toolchain/runtime snapshots or supplied docs. No network or model service calls.
All compiler feedback must use this command, even for your own client tests:
python3 {BASE}/evaluate.py tool {directory} OPERATION
OPERATION is check, interfaces, build, run or context MODULE.DECLARATION.
You may read/edit candidate files using ordinary tools. {treatment}
At most 24 compiler operations and 900 elapsed seconds are available. Each native
command has a 30-second timeout. Submit one final candidate when complete; do not
run the independent evaluator. Model token/tool counts are unknown unless captured.
Complete the task, check/build/run it, and report changed behavior plus any remaining
uncertainty. Participant tests and self-reported success do not decide acceptance.
'''
    (directory/'PROMPT.txt').write_text(prompt)
    print(str(directory/'PROMPT.txt'))


def start(args):
    directory = Path(args.run).resolve()
    metadata = read(directory/'trial.json')
    if metadata['started']: raise ValueError('trial already started')
    metadata.update(started=now(),started_monotonic=time.monotonic(),deadline_monotonic=time.monotonic()+manifest()['elapsed_budget_seconds'])
    save(directory/'trial.json',metadata)
    print(json.dumps({'started':metadata['started'],'elapsed_budget_seconds':900}))


def participant_tool(args):
    directory = Path(args.run).resolve()
    lock = directory/'.compiler-operation-lock'
    try:
        lock.mkdir()
    except FileExistsError:
        save(directory/'rejections'/f'{time.time_ns()}-{os.getpid()}.json',{'timestamp':now(),'reason':'concurrent compiler-wrapper operation rejected'})
        raise ValueError('concurrent compiler-wrapper operations are prohibited')
    try:
        return participant_tool_locked(args)
    except (OSError,ValueError,KeyError) as error:
        save(directory/'rejections'/f'{time.time_ns()}-{os.getpid()}.json',{'timestamp':now(),'reason':str(error)})
        raise
    finally:
        lock.rmdir()


def participant_tool_locked(args):
    directory = Path(args.run).resolve()
    metadata = read(directory/'trial.json')
    if not metadata['started'] or metadata['submitted']: raise ValueError('trial is not active')
    if time.monotonic() > metadata['deadline_monotonic']: raise ValueError('elapsed trial budget exceeded')
    if len(metadata['operations']) >= manifest()['compiler_operation_budget']: raise ValueError('compiler operation budget exceeded')
    if args.operation=='context' and metadata['condition']!='context': raise ValueError('context unavailable in baseline')
    if args.operation=='context' and len(args.operands)!=1: raise ValueError('context requires original MODULE.DECLARATION selector')
    if args.operation!='context' and args.operands: raise ValueError('operation takes no operands')
    compiler,source = directory/'toolchain/slimc',directory/'candidate'
    if not captured_identity(directory,metadata):
        raise ValueError('captured compiler/runtime changed')
    files = candidate_files(source,task(metadata['task']))
    entry = {'index':len(metadata['operations'])+1,'operation':args.operation,'started':now(),'status':'in-progress',
             'source_sha256':{name:hashlib.sha256(value).hexdigest() for name,value in files.items()}}
    metadata['operations'].append(entry); save(directory/'trial.json',metadata)
    if args.operation=='context':
        # Capture every task file in a new independent relative snapshot. The
        # compiler validates complete exact authority; no source is interpreted.
        snapshot = directory/'snapshots'/str(entry['index'])
        copy_sources(source,snapshot,task(metadata['task']))
        results = [command([compiler,'context',source/'slim.project',snapshot/'slim.project',args.operands[0]])]
    elif args.operation in ['check','interfaces']:
        results = [command([compiler,args.operation,source/'slim.project'])]
    else:
        executable,results = build(compiler,Path(metadata['toolchain']['cc_path']),directory/'runtime',source,directory/'tool-work')
        if args.operation=='run' and executable: results.append(command([executable]))
    entry.update(finished=now(),status='complete',commands=results)
    payload = {'compiler_operation':entry['index'],'operation':args.operation,'commands':results,
               'remaining_compiler_operations':24-entry['index']}
    rendered = json.dumps(payload,sort_keys=True)+'\n'
    entry['feedback_bytes'] = len(rendered.encode())
    save(directory/'trial.json',metadata)
    sys.stdout.write(rendered)


def finish(args):
    directory = Path(args.run).resolve()
    submitted,finished_monotonic = now(),time.monotonic()
    initial = read(directory/'trial.json')
    if not initial['started'] or initial['submitted'] or (directory/'finalization.json').exists():
        raise ValueError('trial cannot finish')
    finalized = {'submitted':submitted,'finish_reason':args.reason,
                 'observed_elapsed_seconds':finished_monotonic-initial['started_monotonic']}
    # This independent record survives a killed/stuck wrapper's metadata save
    # or a finalization-lock failure. The coordinator's first finish is durable.
    save(directory/'finalization.json',finalized)
    lock = directory/'.compiler-operation-lock'
    # build/run has at most three sequential30-second native commands. Root
    # invokes finalization asynchronously, retaining commentary while waiting.
    wait_deadline = time.monotonic()+95
    while True:
        try:
            lock.mkdir(); break
        except FileExistsError:
            if time.monotonic()>wait_deadline: raise ValueError('in-flight operation did not release finalization lock')
            time.sleep(0.01)
    try:
        metadata = read(directory/'trial.json')
        if not metadata['started'] or metadata['submitted']: raise ValueError('trial cannot finish')
        metadata.update(finalized)
        save(directory/'trial.json',metadata)
    finally:
        lock.rmdir()
    print(json.dumps({'finish_reason':args.reason,'observed_elapsed_seconds':metadata['observed_elapsed_seconds']}))


def evaluate_trial(args):
    directory = Path(args.run).resolve()
    metadata = read(directory/'trial.json')
    if (directory/'finalization.json').exists(): metadata.update(read(directory/'finalization.json'))
    if not metadata['submitted']: raise ValueError('coordinator must finish participant before evaluation')
    result = {'schema':1,'task':metadata['task'],'accepted':False,'components':{},'commands':[]}
    try:
        if sha(metadata['freeze_path']) != metadata['freeze_sha256'] or read(metadata['freeze_path'])['corpus'] != corpus():
            raise CandidateConstraint('frozen task/oracle/evaluator changed')
        if not captured_identity(directory,metadata):
            raise CandidateConstraint('captured compiler/runtime changed')
        if (directory/'.compiler-operation-lock').exists():
            raise RuntimeError('compiler-wrapper operation remains in flight at final evaluation')
        evaluate_candidate(metadata['task'],directory/'candidate',directory/'toolchain/slimc',Path(metadata['toolchain']['cc_path']),directory/'runtime',result=result)
    except CandidateConstraint as error:
        result.update(accepted=False,outcome='constraint-failure',evaluation_error=str(error))
    except Exception as error:
        result.update(accepted=False,outcome='infrastructure-failure',infrastructure_error=type(error).__name__+': '+str(error))
    result.update(trial=metadata['id'],condition=metadata['condition'],
                  wrapper_compiler_operations=len(metadata['operations']),
                  observed_elapsed_seconds=metadata['observed_elapsed_seconds'])
    rejections = [read(p) for p in sorted((directory/'rejections').glob('*.json'))] if (directory/'rejections').exists() else []
    result['observed_protocol_rejections'] = rejections
    result['oracle_accepted'] = result['accepted']
    result['outcome'] = ('infrastructure-failure' if result.get('infrastructure_error')
                         else 'elapsed-timeout' if metadata['finish_reason']=='timeout' or metadata['observed_elapsed_seconds']>900
                         else 'constraint-failure' if result.get('evaluation_error') or metadata['protocol_violations'] or rejections or len(metadata['operations'])>24
                         else 'accepted' if result['accepted'] else 'correctness-failure')
    result['accepted'] = result['outcome']=='accepted'
    save(directory/'result.json',result)
    print(json.dumps({k:result[k] for k in ['trial','condition','accepted','outcome','wrapper_compiler_operations','observed_elapsed_seconds']}))


def summarize(args):
    rows = []
    for trial in manifest()['trials']:
        path = Path(args.runs)/trial['id']/'result.json'
        rows.append(read(path) if path.exists() else {**trial,'outcome':'not-run','accepted':False})
    output = {'schema':1,'denominator':len(rows),'observed_trials':sum(r['outcome']!='not-run' for r in rows),
              'accepted_by_condition':{c:sum(r['outcome']=='accepted' and r['condition']==c for r in rows) for c in ['baseline','context']},
              'model_tokens':'unknown-not-captured','model_tool_calls':'unknown-not-captured',
              'general_effectiveness':'unknown-three-pair-advisory-isolation-pilot','trials':rows}
    save(args.output,output)
    print(json.dumps({k:v for k,v in output.items() if k!='trials'},sort_keys=True))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='mode',required=True)
    verify = sub.add_parser('verify-fixtures'); verify.add_argument('--compiler',required=True); verify.add_argument('--output',required=True)
    frozen = sub.add_parser('freeze'); frozen.add_argument('--compiler',required=True); frozen.add_argument('--verification',required=True); frozen.add_argument('--output',required=True)
    setup = sub.add_parser('prepare'); setup.add_argument('--freeze',required=True); setup.add_argument('--compiler',required=True); setup.add_argument('--trial',required=True); setup.add_argument('--destination',required=True)
    for mode in ['start','finish','evaluate']:
        p = sub.add_parser(mode); p.add_argument('run')
        if mode=='finish': p.add_argument('--reason',choices=['submitted','timeout'],required=True)
    tool = sub.add_parser('tool'); tool.add_argument('run'); tool.add_argument('operation',choices=['check','interfaces','context','build','run']); tool.add_argument('operands',nargs='*')
    summary = sub.add_parser('summarize'); summary.add_argument('--runs',required=True); summary.add_argument('--output',required=True)
    args = parser.parse_args()
    handlers = {'verify-fixtures':verify_fixtures,'freeze':freeze,'prepare':prepare,'start':start,'tool':participant_tool,
                'finish':finish,'evaluate':evaluate_trial,'summarize':summarize}
    try:
        handlers[args.mode](args)
    except (OSError,ValueError,KeyError) as error:
        # Tool failures must never expose oracle/reference data in tracebacks.
        message = str(error) if args.mode!='tool' else 'participant operation rejected: '+str(error)
        print(json.dumps({'status':'infrastructure-error' if args.mode!='tool' else 'tool-error','message':message}),file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
