"""Actual-build identities, relocation, loaded-worker stability and bad seed rejection."""
import hashlib
import os
from pathlib import Path
import runpy
import shutil
import shlex
import subprocess
import sys
import tempfile

api = runpy.run_path(str(Path(__file__).with_name('verify-session-host.py')))
Client, compare, clean = (api[name] for name in ['Client', 'compare', 'clean'])
root = Path.cwd()
files = ['bootstrap/slimc-seed.c', 'bootstrap/slimc-seed.sha256',
         'compiler/session.c', 'compiler/native.c', 'compiler/native-context.sh', 'compiler/native-copy.c',
         'scripts/embed-native-inputs.sh', 'runtime/slim_rt.c', 'runtime/slim_rt.h',
         'scripts/build-session-host.sh']
base = Client(['./slimc', 'session'])
identity = base.identity
base.quit()
cc = os.environ.get('CC', 'cc')
target = subprocess.run([cc, '-dumpmachine'], capture_output=True, check=True).stdout.decode().strip()
assert identity['target'] == target
expected = clean(str(root / 'build/toolchain/slimc'), root / 'conformance/projects/basic/slim.project')
# Two public starters can both notice a missing host; each publication must be whole.
with tempfile.TemporaryDirectory(prefix='slim-host-publication-') as temporary:
    output = Path(temporary) / 'shared'
    env = os.environ.copy()
    env['SLIM_SESSION_SANITIZE'] = '0'
    builders = [subprocess.Popen(['sh', str(root / 'scripts/build-session-host.sh'), str(output)],
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env) for _ in range(2)]
    # Reap both builds before reporting an ordinary failure or removing output.
    results = [builder.communicate(timeout=180) for builder in builders]
    for builder, (stdout, stderr) in zip(builders, results):
        assert builder.returncode == 0, (stdout, stderr)
    concurrent = Client([str(output / 'slim-session')])
    assert concurrent.identity == identity
    compare(concurrent.update(root / 'conformance/projects/basic/slim.project'), expected)
    concurrent.quit()
    for name in ['session-identity.h','session-build-identity.tsv','native.c','native-inputs.h']:
        assert (output/name).read_bytes() == (root/'build/toolchain'/name).read_bytes(), name
    assert not list(output.glob('.slim-session.*')), 'publication left temporary files'
print('session-host-publication\texact\ttwo simultaneous builds publish one complete executable', flush=True)
if '--publication-only' in sys.argv:
    raise SystemExit(0)
with tempfile.TemporaryDirectory(prefix='slim-host-identity-') as temporary:
    clone = Path(temporary) / 'relocated build'
    for name in files:
        destination = clone / name
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(root / name, destination)

    def build(label, sanitized=False, compiler_override=None, expected_target=target):
        output = clone / label
        env = os.environ.copy()
        env['SLIM_SESSION_SANITIZE'] = '1' if sanitized else '0'
        if compiler_override is not None:
            env['CC'] = str(compiler_override)
        result = subprocess.run(['sh', str(clone / 'scripts/build-session-host.sh'), str(output)],
                                capture_output=True, env=env, timeout=180)
        assert result.returncode == 0, (label, result.stdout, result.stderr)
        client = Client([str(output / 'slim-session')])
        manifest = (output / 'session-build-identity.tsv').read_bytes()
        assert hashlib.sha256(manifest).hexdigest() == client.identity['compiler']
        fields = dict(line.split('\t') for line in manifest.decode().splitlines())
        for field, name in [('seed', 'bootstrap/slimc-seed.c'), ('adapter', 'compiler/session.c'),
                            ('recipe', 'scripts/build-session-host.sh'), ('native-adapter','compiler/native.c'),
                            ('native-copy-source','compiler/native-copy.c'), ('native-recipe','compiler/native-context.sh'),
                            ('native-embedding','scripts/embed-native-inputs.sh')]:
            assert fields[field] == hashlib.sha256((clone / name).read_bytes()).hexdigest()
        assert fields['runtime'] == client.identity['runtime']
        assert fields['options'] == client.identity['options']
        assert fields['target'] == client.identity['target'] == expected_target
        compare(client.update(root / 'conformance/projects/basic/slim.project'), expected)
        return client

    loaded = build('first')
    assert loaded.identity == identity, 'source relocation changed build identity'
    with (clone / 'compiler/session.c').open('a') as output:
        output.write('\n/* Build-identity test: adapter bytes differ. */\n')
    changed_adapter = build('adapter')
    assert changed_adapter.identity['compiler'] != identity['compiler']
    for key in ['runtime', 'target', 'options']:
        assert changed_adapter.identity[key] == identity[key]
    changed_adapter.quit()
    with (clone / 'runtime/slim_rt.c').open('a') as output:
        output.write('\n/* Build-identity test: runtime bytes differ. */\n')
    changed_runtime = build('runtime')
    assert changed_runtime.identity['compiler'] != changed_adapter.identity['compiler']
    assert changed_runtime.identity['runtime'] != identity['runtime']
    assert changed_runtime.identity['options'] == identity['options']
    changed_runtime.quit()
    changed_options = build('sanitized', sanitized=True)
    assert changed_options.identity['options'] != identity['options']
    assert changed_options.identity['runtime'] == changed_runtime.identity['runtime']
    assert changed_options.identity['compiler'] != changed_runtime.identity['compiler']
    changed_options.quit()
    if sys.platform == 'darwin' and target.startswith('arm64-apple-'):
        # This forwards every invocation to the real compiler with a real, runnable
        # deployment target. It does not spoof -dumpmachine or skip compilation.
        native_cc = shutil.which(cc)
        assert native_cc
        target_cc = clone / 'target-cc'
        target_cc.write_text('#!/bin/sh\nexec ' + shlex.quote(native_cc) + ' -target arm64-apple-macos15.0 "$@"\n')
        target_cc.chmod(0o755)
        target_value = subprocess.run([str(target_cc), '-dumpmachine'], capture_output=True, check=True).stdout.decode().strip()
        assert target_value != target
        changed_target = build('target', compiler_override=target_cc, expected_target=target_value)
        assert changed_target.identity['target'] != identity['target']
        assert changed_target.identity['compiler'] != changed_runtime.identity['compiler']
        assert changed_target.identity['runtime'] == changed_runtime.identity['runtime']
        changed_target.quit()
        print('session-host-target\texact\t' + target + '\t' + target_value, flush=True)
    else:
        print('session-host-target-variation\tunknown\tno configured second runnable native target on this host', flush=True)
    # A running process keeps its compiled identity and retained state after edits on disk.
    reused = loaded.update(root / 'conformance/projects/basic/slim.project')
    compare(reused, expected)
    assert loaded.identity == identity and reused['snapshot'] == reused['code_reused'] == 1
    loaded.quit()
    executable = clone / 'first/slim-session'
    prior = executable.read_bytes()
    prior_files = {name:(executable.parent/name).read_bytes() for name in
                   ['session-identity.h','session-build-identity.tsv','native.c','native-inputs.h']}
    # A verification-only cp wrapper fails while preparing auxiliary inputs.
    # None of the existing publication may change before all copies succeed.
    wrappers = clone/'publication-fault-tools'; wrappers.mkdir()
    real_cp = shutil.which('cp'); assert real_cp
    (wrappers/'cp').write_text('#!/bin/sh\npublication_dir=' + shlex.quote(str(executable.parent)) + '''
for argument; do destination=$argument; done
case "$destination" in "$publication_dir"|"$publication_dir"/*)
    for argument; do
        case "$argument" in */native-inputs.h)
            echo 'test: publication copy failure' >&2
            exit 73 ;;
        esac
    done ;;
esac
exec ''' + shlex.quote(real_cp) + ' "$@"\n')
    (wrappers/'cp').chmod(0o755)
    environment = os.environ.copy()
    environment['PATH'] = str(wrappers) + os.pathsep + environment.get('PATH', '/usr/bin:/bin')
    failed_copy = subprocess.run(['sh',str(clone/'scripts/build-session-host.sh'),str(executable.parent)],
                                 env=environment,capture_output=True,timeout=180)
    assert failed_copy.returncode == 73 and b'test: publication copy failure' in failed_copy.stderr, failed_copy
    assert executable.read_bytes() == prior, 'failed publication changed existing executable'
    assert all((executable.parent/name).read_bytes() == data for name,data in prior_files.items())
    assert not list(executable.parent.glob('.slim-session.*')), 'failed publication left temporary files'
    print('session-host-publication-fault\texact\tfailed preparation preserves executable and every auxiliary file',flush=True)
    with (clone / 'bootstrap/slimc-seed.c').open('a') as output:
        output.write('\n')
    rejected = subprocess.run(['sh', str(clone / 'scripts/build-session-host.sh'), str(executable.parent)],
                              capture_output=True, timeout=30)
    assert rejected.returncode == 1 and b'seed digest mismatch' in rejected.stderr
    assert executable.read_bytes() == prior, 'rejected seed changed the existing host artifact'
print('session-host-identity\texact\trelocation, adapter/runtime/options builds, actual native target, loaded worker stability, corrupt seed rejection', flush=True)
