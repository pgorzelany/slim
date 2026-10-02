"""Actual captured driver jobs, rejected input shapes, and default-config isolation."""
import argparse
from pathlib import Path
import shlex
import subprocess
import tempfile


def verify(context):
    context = Path(context).resolve()
    recipe = Path('compiler/native-context.sh').read_text()
    begin = recipe.index('check_driver_job() {')
    end = recipe.index('\nlink_inputs() {', begin)
    count = 0
    with tempfile.TemporaryDirectory(prefix='slim-driver-check-') as temporary:
        root = Path(temporary)
        checker = root / 'check.sh'
        checker.write_text('#!/bin/sh\nset -eu\nfail() { echo "$*" >&2; exit 2; }\n' +
                           recipe[begin:end] + '\ncheck_driver_job "$@"\n')
        dependency = root / 'dependencies.sh'
        begin = recipe.index('check_captured_inputs() {')
        end = recipe.index('\nfor profile in ', begin)
        dependency.write_text('#!/bin/sh\nset -eu\nfail() { echo "$*" >&2; exit 2; }\n' +
                              recipe[begin:end] + '\ncapture_root=$1\ncd "$capture_root"\ncheck_captured_inputs "$2" "$3"\n')

        def inputs(name, values, role, accepted, *, captured=context, reason=None, newline=True):
            nonlocal count
            report = root / 'inputs.txt'
            text = '\n'.join(values) + ('\n' if values and newline else '')
            report.write_text(text)
            run = subprocess.run(['sh',str(dependency),str(captured),role,str(report)],capture_output=True,timeout=10)
            assert run.returncode == (0 if accepted else 2), (name,run.returncode,run.stdout,run.stderr)
            assert not run.stdout, (name,run.stdout)
            if reason is not None:
                assert run.stderr == (reason+'\n').encode(), (name,run.stderr)
            count += 1
            print(f'native-driver-inputs\t{name}\t' + ('accepted' if accepted else 'rejected'),flush=True)

        headers = sorted(context.glob('captured-headers-*.make'))
        assert len(headers) == 6, headers
        for path in headers:
            words = shlex.split(path.read_text().replace('\\\n',' '))
            assert words[0] == 'object:', words
            inputs(path.name, words[1:], 'headers', True)
        inputs('paired-local-headers',['prefix.c','slim_rt.c','slim_rt.h','./slim_rt.h'],'headers',True)
        inputs('request-objects',['program.o','runtime.o',str(context/'program.o'),str(context/'runtime.o')],'link',True)
        inputs('unrecorded-header',[str(context/'sdk/unrecorded.h')],'headers',False)
        inputs('original-header',['/original-sdk/usr/include/stdint.h'],'headers',False)
        inputs('escaped-header',[str(context/'sdk/../sdk/usr/include/stdint.h')],'headers',False)
        inputs('object-as-header',['program.o'],'headers',False)
        inputs('header-as-link-input',['slim_rt.h'],'link',False)
        inputs('blank-input',[''],'headers',False,reason='outside-captured-input')
        inputs('root-prefix-collision',[str(context)+'-other/sdk/stdio.h'],'headers',False,
               reason='outside-captured-input')
        inputs('sdk-prefix-collision',[str(context/'sdk-other/stdio.h')],'headers',False,
               reason='outside-captured-input')
        inputs('first-input-error',['slim_rt.h',str(context/'sdk/unrecorded.h')],'link',False,
               reason='unexpected-link-input')
        inputs('reversed-first-input-error',[str(context/'sdk/unrecorded.h'),'slim_rt.h'],'link',False,
               reason='unrecorded-captured-input')
        inputs('unterminated-unrecorded-input',[str(context/'sdk/unrecorded.h')],'headers',False,
               reason='unrecorded-captured-input',newline=False)
        synthetic = root/'synthetic'; synthetic.mkdir()
        manifest = synthetic/'captured-sha256.tsv'; manifest.write_text('')
        inputs('empty-manifest-cannot-authorize',[str(synthetic/'sdk/member.h')],'headers',False,
               captured=synthetic,reason='unrecorded-captured-input')
        inputs('empty-manifest-local-headers',['slim_rt.h'],'headers',True,captured=synthetic)
        inputs('empty-report',[],'headers',True,captured=synthetic)
        members = [f'sdk/header-{i}.h' for i in range(512)]
        manifest.write_text(''.join('0'*64+'\t'+member+'\n' for member in members))
        ordered = [str(synthetic/member) for member in members]
        inputs('complete-manifest-reversed-duplicates',ordered[::-1]+ordered,'headers',True,captured=synthetic)
        inputs('manifest-exact-name',[ordered[0]+'x'],'headers',False,captured=synthetic,
               reason='unrecorded-captured-input')
        inputs('manifest-no-normalization',[str(synthetic)+'/sdk/../sdk/header-0.h'],'headers',False,
               captured=synthetic,reason='unrecorded-captured-input')
        inputs('manifest-no-trimming',[' '+ordered[0]],'headers',False,captured=synthetic,
               reason='outside-captured-input')
        inputs('manifest-whole-record',[ordered[0]+'\textra'],'headers',False,captured=synthetic,
               reason='unrecorded-captured-input')

        def check(name, data, accepted, role='program', profile=0, *, reason=None):
            nonlocal count
            report = root / 'job.txt'
            report.write_bytes(data)
            run = subprocess.run(['sh', str(checker), str(context), role, str(profile), str(report)],
                                 capture_output=True, timeout=10)
            assert run.returncode == (0 if accepted else 2), (name, run.returncode, run.stdout, run.stderr)
            assert not run.stdout, (name, run.stdout)
            if accepted:
                assert not run.stderr, (name, run.stderr)
            if reason is not None:
                assert run.stderr == (reason + '\n').encode(), (name, run.stderr)
            count += 1
            print(f'native-driver\t{name}\t' + ('accepted' if accepted else 'rejected'), flush=True)

        for profile in range(3):
            for role in ['program', 'runtime']:
                check(f'actual-{profile}-{role}', (context / f'compile-job-{profile}-{role}.txt').read_bytes(),
                      True, role, profile)
        link = (context / 'link-job.txt').read_bytes()
        check('actual-link', link, True, 'link')
        original = (context / 'compile-job-0-program.txt').read_bytes()
        lines = original.decode().splitlines()
        args = shlex.split(lines[4])

        def job(values):
            return ('\n'.join(lines[:4]) + '\n ' + ' '.join('"' + value + '"' for value in values) + '\n').encode()

        settings_option = '-fdepfile-entry=' + str(context / 'sdk/SDKSettings.json')
        without_settings = [value for value in args if not value.startswith('-fdepfile-entry=')]
        with_settings = without_settings[:-1] + [settings_option] + without_settings[-1:]
        manifest = context / 'captured-sha256.tsv'
        recorded = manifest.read_bytes()
        assert any(row.split(b'\t')[1] == b'sdk/SDKSettings.json' for row in recorded.splitlines()), recorded
        check('captured-sdk-settings', job(with_settings), True)
        check('sdk-settings-optional', job(without_settings), True)
        for name, path in [
            ('outside-sdk-settings', '/original-sdk/SDKSettings.json'),
            ('sibling-sdk-settings', str(context) + '-other/sdk/SDKSettings.json'),
            ('sdk-settings-traversal', str(context) + '/sdk/../sdk/SDKSettings.json'),
            ('other-sdk-settings-file', str(context / 'sdk/SDKSettings.plist'))]:
            values = without_settings[:-1] + ['-fdepfile-entry=' + path] + without_settings[-1:]
            check(name, job(values), False, reason='unsupported-driver-job')
        check('duplicate-sdk-settings', job(with_settings[:-1] + [settings_option] + with_settings[-1:]), False,
              reason='unsupported-driver-job')
        try:
            manifest.write_bytes(b''.join(row + b'\n' for row in recorded.splitlines()
                                         if row.split(b'\t')[1] != b'sdk/SDKSettings.json'))
            check('unrecorded-sdk-settings', job(with_settings), False, reason='unsupported-driver-job')
            check('no-sdk-settings-without-record', job(without_settings), True)
            manifest.write_bytes(b'')
            check('empty-manifest-sdk-settings', job(with_settings), False, reason='unsupported-driver-job')
            check('empty-manifest-without-sdk-settings', job(without_settings), True)
        finally:
            manifest.write_bytes(recorded)

        def replace(name, old, new):
            changed = args.copy()
            changed[changed.index(old)] = new
            check(name, job(changed), False)

        for name, extra in [
            ('implicit-include', ['-include', 'slim_rt.h']),
            ('outside-include', ['-internal-isystem', '/usr/include']),
            ('plugin', ['-load', '/tmp/unrecorded.dylib']),
            ('response-file', ['@commands.rsp']),
            ('unknown-switch', ['--slim-unknown']),
            ('missing-operand', ['-o'])]:
            check(name, job(args[:-1] + extra + args[-1:]), False)
        replace('outside-tool', args[0], '/usr/bin/clang')
        replace('sibling-tool-root', args[0], str(context) + '2/toolchain/bin/clang')
        replace('outside-resource', str(context / 'toolchain/lib/clang/21'), '/usr/lib/clang/21')
        replace('outside-sdk', str(context / 'sdk'), '/Library/Developer/SDKs')
        replace('outside-header', str(context / 'sdk/usr/include'), str(context / 'sdk/../include'))
        replace('wrong-working-directory', '-fdebug-compilation-dir=' + str(context), '-fdebug-compilation-dir=/tmp')
        replace('wrong-input', 'program.c', 'unrecorded.c')
        replace('wrong-output', 'program.o', '/tmp/output.o')
        replace('wrong-language', 'c', 'c++')
        replace('wrong-macro', 'NDEBUG', 'UNRECORDED')
        for option in ['-O3', '-std=c11', '-nostdsysteminc', '-nobuiltininc']:
            check('missing-' + option, job([value for value in args if value != option]), False)
        check('extra-job', original + lines[4].encode() + b'\n', False)
        check('ambiguous-quoting', original.replace(b' "-cc1" ', b" '-cc1' "), False)
        check('tabbed-argument', original.replace(b'"program.c"', b'"program\t.c"'), False)
        check('embedded-nul', original.replace(b'"program.c"', b'"program\0.c"'), False)
        check('config-metadata', b'\n'.join(original.splitlines()[:4]) + b'\nConfiguration file: config.cfg\n' + lines[4].encode() + b'\n', False)
        check('missing-job', b'\n'.join(original.splitlines()[:4]) + b'\n', False)
        check('wrong-profile', original, False, profile=1)
        check('invalid-profile', original, False, profile=3)
        check('wrong-role', original, False, role='runtime')
        for old, new in [(b'Apple clang version 21.', b'Apple clang version 20.'),
                         (b'Target: arm64-', b'Target: x86_64-'),
                         (b'Thread model: posix', b'Thread model: unknown')]:
            check('metadata-' + new.decode(), original.replace(old, new), False)
        crowded = args[:-1] + ['-fcommon'] * (512 - len(args)) + args[-1:]
        check('arguments-512', job(crowded), True)
        check('arguments-513', job(crowded[:-1] + ['-fcommon'] + crowded[-1:]), False)
        long = args.copy()
        long[long.index('-target-cpu') + 1] = 'x' * 4096
        check('nonfile-argument-4096', job(long), True)
        long[long.index('-target-cpu') + 1] += 'x'
        check('nonfile-argument-4097', job(long), False)
        padded = original + b' ' * (1048576 - len(original) - 1) + b'\n'
        check('report-1MiB', padded, True)
        check('report-1MiB-plus-one', padded + b'\n', False)
        for name, old, new in [
            ('link-external-lto', str(context / 'toolchain/lib/libLTO.dylib'), '/usr/lib/libLTO.dylib'),
            ('link-object-order', '"program.o" "runtime.o"', '"runtime.o" "program.o"'),
            ('link-extra-library', '"-dynamic"', '"-dynamic" "-lSystem"'),
            ('link-wrong-output', '"program"', '"other-program"'),
            ('link-wrong-report', '"link.bin"', '"outside.bin"')]:
            check(name, link.replace(old.encode(), new.encode()), False, 'link')

        # Only this private captured tool directory is changed. The ambient
        # installed compiler/SDK and the source inputs remain untouched.
        config = context / 'toolchain/bin/clang.cfg'
        output = context / 'config-check.o'
        assert not config.exists() and not output.exists()
        config.write_text('--slim-default-config-must-be-ignored\n')
        common = [str(context / 'toolchain/bin/clang'), '-std=c11', '-O3', '-DNDEBUG',
                  '-Wall', '-Wextra', '-Werror', '-resource-dir', str(context / 'toolchain/lib/clang/21'),
                  '-isysroot', str(context / 'sdk'), '-nostdinc', '-isystem', str(context / 'toolchain/lib/clang/21/include'),
                  '-isystem', str(context / 'sdk/usr/include'), '-I.', '-c', 'prefix.c', '-o', output.name]
        environment = {'PATH': '/usr/bin:/bin', 'LC_ALL': 'C', 'TMPDIR': str(context / 'tmp')}
        try:
            ambient = subprocess.run(common, cwd=context, env=environment, capture_output=True, timeout=30)
            assert ambient.returncode != 0 and b'slim-default-config-must-be-ignored' in ambient.stderr, ambient
            isolated = subprocess.run([common[0], '--no-default-config', *common[1:]], cwd=context,
                                      env=environment, capture_output=True, timeout=30)
            assert isolated.returncode == 0 and not isolated.stdout and not isolated.stderr, isolated
            assert output.stat().st_size > 0
            print('native-driver-config\tactual ambient config rejected; production option ignores it and compiles', flush=True)
        finally:
            config.unlink()
            output.unlink(missing_ok=True)
    print(f'native-driver\t{count} command-shape cases exact; default-config isolation exact', flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('context')
    args = parser.parse_args()
    verify(args.context)
