"""Quiet public U/B/publication/run timings versus ordinary ./slimc build.

Uses the uninstrumented default host. Records raw interleaved samples and
connection setup separately; no agent-productivity or portable timing claim.
"""
import argparse
import hashlib
import importlib.util
import json
import math
import os
from pathlib import Path
import statistics
import struct
import subprocess
import tempfile
import time

spec = importlib.util.spec_from_file_location('native_oracle', Path(__file__).with_name('verify-native-host.py'))
native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(native)
now = time.perf_counter_ns
sha = lambda data: hashlib.sha256(data).hexdigest()


def decode(tag, payload):
    assert tag == b'N' and len(payload) >= 116 and payload[0] == 1
    status, = struct.unpack('>q',payload[17:25])
    lengths = struct.unpack('>4I',payload[100:116])
    assert sum(lengths)+116 == len(payload) and status == 0, (status,payload[116:500])
    starts = struct.unpack('>3Q',payload[41:65])
    times = struct.unpack('>4Q',payload[68:100])
    context = payload[116:116+lengths[0]].decode()
    diagnostics_at = 116+lengths[0]+lengths[1]
    diagnostics = payload[diagnostics_at:diagnostics_at+lengths[2]].decode(errors='replace')
    return payload[-lengths[3]:], starts, times, context, diagnostics


def pipeline(client, source, workers, target, expected_code, expected_output, environment):
    begin = now()
    tag, payload = client.request(b'U',os.fsencode(source))
    received_source = now()
    accepted = client.decode_update(tag,payload)
    assert accepted['status'] == 0, {k:v for k,v in accepted.items() if k!='code'}
    tag, payload = client.request(b'B',struct.pack('>QQB',*accepted['published'],workers))
    received_native = now()
    artifact, starts, times, context, diagnostics = decode(tag,payload)
    native.publish(target,artifact)
    published = now()
    run = subprocess.run([str(target)],capture_output=True,env=environment,timeout=120)
    finished = now()
    assert accepted['code'] == expected_code
    assert (run.returncode,run.stdout,run.stderr) == (0,expected_output,b'')
    return dict(update_ns=received_source-begin,native_ns=received_native-received_source,
        publication_ns=published-received_native,build_ns=published-begin,build_run_ns=finished-begin,
        setup_ns=times[0],starts=starts,code_bytes=len(expected_code),executable_bytes=len(artifact),
        code_sha256=sha(expected_code),executable_sha256=sha(artifact),context=context,diagnostics=diagnostics)


def baseline(source, target, expected_output, environment):
    directory = target
    directory.mkdir()
    target = directory/'program'
    begin = now()
    result = subprocess.run(['./slimc','build',str(source),'-o',str(target)],capture_output=True,env=environment,timeout=120)
    built = now()
    assert result.returncode == 0 and not result.stdout, result
    run = subprocess.run([str(target)],capture_output=True,env=environment,timeout=120)
    finished = now()
    assert (run.returncode,run.stdout,run.stderr) == (0,expected_output,b'')
    artifact = target.read_bytes()
    target.unlink()
    directory.rmdir()
    return dict(build_ns=built-begin,build_run_ns=finished-begin,executable_bytes=len(artifact),
        executable_sha256=sha(artifact),diagnostics=result.stderr.decode(errors='replace'))


def fixtures(root):
    result = []
    for size in [125,250,500,1000,2000,4000]:
        text = 'module main\n\n'+''.join(f'fn value_{i}() -> I64:\n  {i}\n\n' for i in range(size))
        text += 'fn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64(value_0())\n  io.println("")\n  0\n'
        result.append((f'helpers-{size}',0,text,text.replace('fn value_0() -> I64:\n  0','fn value_0() -> I64:\n  1')))
    for name in ['state_machine','signal_network']:
        text = Path(f'benchmarks/challenges/{name}/program.slim').read_text()
        assert text.count('io.print_i64(answer)') == 1
        for workers in [0,1]:
            result.append((f'{name}-{workers}',workers,text,text.replace('io.print_i64(answer)','io.print_i64(answer + 1)')))
    cases = []
    for name, workers, before, after in result:
        path = native.module.project(root/name,before)
        code = []
        for text in [before,after]:
            path.with_name('program.slim').write_text(text)
            code.append(subprocess.check_output(['build/toolchain/slimc',str(path)],timeout=120))
        path.with_name('program.slim').write_text(before)
        assert code[0] != code[1]
        cases.append((name,workers,path,[before,after],code))
    return cases


def run(samples):
    compiler = subprocess.check_output(['xcrun','--find','clang'],text=True).strip()
    version = subprocess.check_output([compiler,'--no-default-config','--version'],text=True)
    control_cc = '/usr/bin/cc'
    identity = dict(line.split('\t') for line in Path('build/toolchain/session-build-identity.tsv').read_text().splitlines())
    print('native-timing-identity\t'+json.dumps(dict(build=identity,resolved_clang=compiler,clang_sha256=sha(Path(compiler).read_bytes()),clang_version=version,
        control_cc=control_cc,control_cc_sha256=sha(Path(control_cc).read_bytes()),
        control_environment={key:os.environ.get(key) for key in ['SDKROOT','CPATH','C_INCLUDE_PATH','LIBRARY_PATH','MACOSX_DEPLOYMENT_TARGET','CCC_OVERRIDE_OPTIONS','CLANG_CONFIG_FILE_USER_DIR','CLANG_CONFIG_FILE_SYSTEM_DIR']},
        host_sha256=sha(Path('build/toolchain/slim-session').read_bytes()),cli_sha256=sha(Path('slimc').read_bytes()),samples=samples,
        scope='Public CLI control; U/B framed session client including publication and execution; observer-free timings'),sort_keys=True),flush=True)
    with tempfile.TemporaryDirectory(prefix='slim-native-timing-') as temporary:
        root = Path(temporary)
        environment = os.environ.copy()
        for key in ['SLIM_ALLOC_FAIL_AT','SLIM_HOST_ALLOC_FAIL_AT','SLIM_NATIVE_ALLOC_FAIL_AT']:
            environment.pop(key,None)
        environment.update(PATH='/usr/bin:/bin',LC_ALL='C',TMPDIR=str(root),CC=control_cc,SLIM_WORKER_TIER='serial')
        cases = fixtures(root)
        preflight = [baseline(cases[0][2],root/('preflight-'+str(i)),b'0\n',environment) for i in range(2)]
        assert preflight[0]['executable_sha256'] == preflight[1]['executable_sha256'], 'control output identity varies across fresh directories'
        print('native-timing-preflight\tordinary CLI control builds and executes before setup sampling',flush=True)
        setup = []
        name, workers, path, texts, code = cases[0]
        for sample in range(samples+1):
            begin = now()
            client = native.Client(['./slimc','session'],environment)
            started = now()-begin
            try:
                cold = pipeline(client,path,workers,root/'setup-program',code[0],b'0\n',environment)
                warm = pipeline(client,path,workers,root/'setup-program',code[0],b'0\n',environment)
                assert cold['starts'] == (1,1,1) and cold['setup_ns'] > 0 and warm['starts'] == (0,0,0)
                if sample:
                    setup.append(cold['setup_ns'])
                    print('native-timing-setup\t'+json.dumps(dict(sample=sample,start_ns=started,cold=cold,warm=warm),sort_keys=True),flush=True)
                client.quit()
            finally:
                if client.process.poll() is None:
                    client.process.stdin.close();client.process.wait(timeout=20)
        client = native.Client(['./slimc','session'],environment)
        try:
            pipeline(client,cases[0][2],0,root/'program',cases[0][4][0],b'0\n',environment)
            for name, workers, path, texts, code in cases:
                environment['SLIM_WORKER_TIER'] = 'posix' if workers else 'serial'
                rows = []
                for sample in range(samples+1):
                    controls = {}
                    def controls_now():
                        for index in [0,1]:
                            path.with_name('program.slim').write_text(texts[index])
                            controls[index] = baseline(path,root/f'control-{name}-{sample}-{index}',f'{index}\n'.encode(),environment)
                    if sample % 2 == 0: controls_now()
                    client.reset()
                    observed = {}
                    for label,index in [('epoch-cold',0),('unchanged',0),('body',1),('revert',0)]:
                        if label != 'unchanged': path.with_name('program.slim').write_text(texts[index])
                        observed[label] = pipeline(client,path,workers,root/'program',code[index],f'{index}\n'.encode(),environment)
                    if sample % 2: controls_now()
                    assert observed['epoch-cold']['starts'] == (1,1,1)
                    assert observed['body']['starts'] == (1,0,1)
                    assert observed['unchanged']['starts'] == observed['revert']['starts'] == (0,0,0)
                    assert all(row['setup_ns'] == 0 for row in observed.values())
                    assert observed['epoch-cold']['executable_sha256'] == observed['unchanged']['executable_sha256'] == observed['revert']['executable_sha256']
                    if sample:
                        row = dict(case=name,sample=sample,workers=workers,control=controls,session=observed)
                        rows.append(row)
                        print('native-timing-sample\t'+json.dumps(row,sort_keys=True),flush=True)
                summary = dict(case=name,setup_median_ns=statistics.median(setup))
                for index in [0,1]:
                    assert len({row['control'][index]['executable_sha256'] for row in rows}) == 1, (name,index,'unstable control artifact')
                for metric in ['build_ns','build_run_ns']:
                    clean = statistics.median(row['control'][1][metric] for row in rows)
                    body = statistics.median(row['session']['body'][metric] for row in rows)
                    warm = statistics.median(row['session']['unchanged'][metric] for row in rows)
                    saving = statistics.median(row['control'][1][metric]-row['session']['body'][metric] for row in rows)
                    summary[metric] = dict(clean_median=clean,body_median=body,unchanged_median=warm,
                        paired_body_ratio_median=statistics.median(row['session']['body'][metric]/row['control'][1][metric] for row in rows),
                        estimated_setup_break_even_edits=math.ceil(statistics.median(setup)/saving) if saving>0 else None)
                print('native-timing-summary\t'+json.dumps(summary,sort_keys=True),flush=True)
            client.quit()
        finally:
            if client.process.poll() is None:
                client.process.stdin.close();client.process.wait(timeout=20)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--samples',type=int,default=5)
    args = parser.parse_args()
    assert 3 <= args.samples <= 9
    run(args.samples)
