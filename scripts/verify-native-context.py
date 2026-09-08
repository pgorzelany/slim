"""Captured compiler/SDK independence after all original input paths disappear.

Only verification discovery roots are substituted. Every capture, preprocessing,
job validation and link-input check uses the production recipe and copy helper.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile


def verify(context, compiler):
    context, compiler = Path(context).resolve(), Path(compiler).resolve()
    repository = Path(__file__).resolve().parent.parent
    with tempfile.TemporaryDirectory(prefix='slim-context-independent-') as temporary:
        root = Path(temporary).resolve()
        origin, captured = root/'origin', root/'captured'
        origin.mkdir(); captured.mkdir()
        runtime = sum((context/name).stat().st_size for name in ['slim_rt.c', 'slim_rt.h'])
        relative = [row.split('\t')[1] for row in (context/'files.tsv').read_text().splitlines()]
        assert len(relative) == len(set(relative)) and all(name.startswith(('sdk/', 'toolchain/')) for name in relative)
        plan = ''.join(f'{context/name}\t{name}\n' for name in relative).encode()
        copied = subprocess.run([str(context/'copy-inputs'), str(runtime)], cwd=origin, input=plan,
                                capture_output=True, timeout=120)
        assert copied.returncode == 0 and len(copied.stdout.splitlines()) == len(relative), copied.stderr
        for name in ['slim_rt.c', 'slim_rt.h', 'copy-inputs']:
            shutil.copy2(context/name, captured/name)
        recipe = (repository/'compiler/native-context.sh').read_text()
        for old, new in [
            ('native_clang=$(xcrun --find clang)', 'native_clang='+shlex.quote(str(origin/'toolchain/bin/clang'))),
            ('native_ld=$(xcrun --find ld)', 'native_ld='+shlex.quote(str(origin/'toolchain/bin/ld'))),
            ('sdk_origin=$(CDPATH= cd "$(xcrun --show-sdk-path)" && pwd -P)', 'sdk_origin='+shlex.quote(str(origin/'sdk')))]:
            assert recipe.count(old) == 1, old
            recipe = recipe.replace(old, new)
        script = root/'capture.sh'; script.write_text(recipe)
        environment = {'PATH':'/usr/bin:/bin', 'LC_ALL':'C', 'TMPDIR':str(root)}
        result = subprocess.run(['sh', str(script), str(captured)], env=environment, capture_output=True, timeout=120)
        assert result.returncode == 0, (result.returncode, result.stdout, result.stderr)
        environment['TMPDIR'] = str(captured/'tmp')
        tool = str(captured/'toolchain/bin/clang')
        flags = ['--no-default-config', '-std=c11', '-O3', '-DNDEBUG', '-Wall', '-Wextra', '-Werror',
                 '-resource-dir', str(captured/'toolchain/lib/clang/21'), '-isysroot', str(captured/'sdk'),
                 '-nostdinc', '-isystem', str(captured/'toolchain/lib/clang/21/include'),
                 '-isystem', str(captured/'sdk/usr/include'), '-I.']
        programs = {'vector_sum': repository/'examples/vector_sum.slim',
                    **{name:repository/'benchmarks/challenges'/name/'program.slim' for name in ['state_machine','signal_network']}}
        code = {name:subprocess.check_output([str(compiler),str(source)],timeout=120) for name,source in programs.items()}
        configurations = [('vector_sum',0),('state_machine',1),('state_machine',2),('signal_network',1),('signal_network',2)]
        reference = {}
        manifest_before = (captured/'captured-sha256.tsv').read_bytes()
        for phase in ['present', 'removed']:
            if phase == 'removed':
                (origin/'toolchain').rename(origin/'unavailable-toolchain')
                (origin/'sdk').rename(origin/'unavailable-sdk')
                assert not (origin/'toolchain').exists() and not (origin/'sdk').exists()
            for name, profile in configurations:
                for file in ['program.o','runtime.o','program','link.bin']:
                    (captured/file).unlink(missing_ok=True)
                (captured/'program.c').write_bytes(code[name])
                mode = ['-DSLIM_PARALLEL=1'] if profile else []
                if profile == 2:
                    mode += ['-DSLIM_POSIX_WORKERS=1','-pthread']
                commands = [
                    [tool,*flags,*mode,'-c','program.c','-o','program.o'],
                    [tool,*flags,*mode,'-c','slim_rt.c','-o','runtime.o'],
                    [tool,'--no-default-config','-O3','-isysroot',str(captured/'sdk'),'-nostdlib','program.o','runtime.o',
                     str(captured/'sdk/usr/lib/libSystem.tbd'),str(captured/'toolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a'),
                     '-Wl,-dependency_info,link.bin','-o','program']]
                for command in commands:
                    build = subprocess.run(command,cwd=captured,env=environment,capture_output=True,timeout=120)
                    assert build.returncode == 0 and not build.stdout and not build.stderr, (phase,name,profile,build)
                artifact = (captured/'program').read_bytes()
                staging, published = root/'program.next', root/'program'
                staging.write_bytes(artifact); staging.chmod(0o700); staging.replace(published)
                outputs = []
                for arguments in ([[],['seed']] if profile else [[]]):
                    run = subprocess.run([str(published),*arguments],capture_output=True,timeout=120)
                    assert run.returncode == 0 and not run.stderr, (phase,name,profile,run)
                    outputs.append(run.stdout)
                observed = artifact, outputs
                key = name, profile
                if phase == 'present':
                    reference[key] = observed
                else:
                    assert observed == reference[key], (name,profile,'original roots affected captured build')
                print('native-context-independence\t'+json.dumps(dict(phase=phase,case=name,profile=profile,
                      executable_sha256=hashlib.sha256(artifact).hexdigest(),
                      stdout_sha256=[hashlib.sha256(value).hexdigest() for value in outputs]),sort_keys=True),flush=True)
        assert (captured/'captured-sha256.tsv').read_bytes() == manifest_before
        for row in manifest_before.decode().splitlines():
            expected, path = row.split('\t')
            assert hashlib.sha256((captured/path).read_bytes()).hexdigest() == expected, path
        print('native-context-independence\tall original tool/SDK paths removed; five fresh build/execution configurations and captured bytes exact',flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('context')
    parser.add_argument('compiler',nargs='?',default='build/toolchain/slimc')
    args = parser.parse_args()
    verify(args.context,args.compiler)
