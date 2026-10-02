"""Complete native challenge corpus through RFC-0146 public frames.

Ordinary builds are independent production controls. These are correctness and
work observations, not a matched timing experiment or a new compiler path.
"""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import tempfile
from native_failure_receipt import FailureReceipt, client_class

spec = importlib.util.spec_from_file_location(
    'native_oracle', Path(__file__).with_name('verify-native-host.py'))
native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(native)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def result(path, arguments):
    run = subprocess.run([str(path), *arguments], capture_output=True, timeout=120)
    return run.returncode, run.stdout, run.stderr


def verify(host, compiler, observe, failure_dir=None):
    repository = Path(__file__).resolve().parent.parent
    receipt = FailureReceipt(failure_dir, repository, host, compiler) if failure_dir else None
    challenges = repository / 'benchmarks/challenges'
    names = [line.split('\t')[0] for line in (challenges / 'manifest.tsv').read_text().splitlines()
             if line and not line.startswith('#')]
    assert len(names) == len(set(names)) == 20, names
    with tempfile.TemporaryDirectory(prefix='slim-native-corpus-') as temporary:
        root = Path(temporary)
        input_file = root / 'input.bin'
        input_file.write_bytes(bytes(range(256)) * 32768)
        trace = root / 'executed.tsv' if observe else None
        client = None
        failure = None
        current_project = None
        controls, emitted, projects, fingerprints, rows = {}, {}, {}, {}, []
        parallel = set()
        target = root / 'program'
        try:
            environment = {'SLIM_NATIVE_EXEC_REPORT': str(trace)} if trace else {}
            client = client_class(native.Client)([host], environment, receipt) if receipt else native.Client([host], environment)
            client.trace = trace
            # Compile controls once, outside the session. Force both supported
            # worker tiers and execute both argument branches in parallel apps.
            for name in names:
                source = challenges / name / 'program.slim'
                projects[name] = native.module.project(root / name, source.read_text())
                current_project = projects[name]
                if receipt:
                    receipt.stage(operation='ordinary-check', case=name, source_sha256=digest(source.read_bytes()))
                status, code = native.module.clean(compiler, projects[name])
                if receipt:
                    receipt.stage(operation='ordinary-check', case=name, observed=dict(status=status, output=code), expected_status=0)
                assert status == 0, (name, status, code)
                emitted[name] = code
                if code.startswith(b'#define SLIM_PARALLEL 1\n'):
                    parallel.add(name)
                for tier in ['serial', 'posix']:
                    arguments = [[str(input_file)]] if name == 'bytefreq' else [[]]
                    if name in parallel:
                        arguments.append(['seed'])
                    staging = root / 'control.next'
                    environment = os.environ.copy()
                    environment['SLIM_WORKER_TIER'] = tier
                    for variable in ['SLIM_ALLOC_FAIL_AT', 'SLIM_HOST_ALLOC_FAIL_AT', 'SLIM_NATIVE_ALLOC_FAIL_AT']:
                        environment.pop(variable, None)
                    if receipt:
                        receipt.stage(operation='ordinary-build', case=name, tier=tier, expected_status=0)
                    build = subprocess.run([str(repository / 'slimc'), 'build', str(source), '-o', str(staging)],
                                           env=environment, capture_output=True, timeout=120)
                    if receipt:
                        receipt.stage(operation='ordinary-build', case=name, tier=tier, expected_status=0,
                                      observed=dict(status=build.returncode, stdout=build.stdout, stderr=build.stderr))
                    assert build.returncode == 0, (name, tier, build.returncode, build.stdout, build.stderr)
                    staging.replace(target)
                    expected = [result(target, args) for args in arguments]
                    if receipt:
                        receipt.stage(operation='ordinary-run', case=name, tier=tier, arguments=arguments,
                                      observed=expected, expected_status=0, expected_stderr=b'')
                    assert all(value[0] == 0 and not value[2] for value in expected), (name, tier, expected)
                    controls[name, tier] = arguments, expected
                assert controls[name, 'serial'] == controls[name, 'posix'], name
                print('native-corpus-control\t' + name + '\tserial/posix exact', flush=True)
            assert parallel == {'state_machine', 'signal_network'}, parallel

            context = None
            for epoch in range(2):
                if epoch:
                    if receipt:
                        receipt.stage(operation='reset', epoch=epoch + 1)
                    client.reset()
                seen_profiles = set()
                for name in names:
                    current_project = projects[name]
                    if receipt:
                        receipt.stage(operation='update', case=name, epoch=epoch + 1, expected_status=0,
                                      expected_code_sha256=digest(emitted[name]))
                    update = client.update(projects[name])
                    if receipt:
                        receipt.stage(operation='update', case=name, epoch=epoch + 1, observed=update,
                                      expected_status=0, expected_code_sha256=digest(emitted[name]))
                    assert update['status'] == 0 and update['code'] == emitted[name], (name, update)
                    # Reverse selection order after R: no hidden dependence on
                    # which runtime configuration was compiled first.
                    tiers = ['serial', 'posix'] if epoch == 0 else ['posix', 'serial']
                    for tier in tiers:
                        profile = (1 if tier == 'serial' else 2) if name in parallel else 0
                        warm_from_other_tier = name not in parallel and tier == tiers[1]
                        expected_starts = (0, 0, 0) if warm_from_other_tier else (1, int(profile not in seen_profiles), 1)
                        if receipt:
                            selection = dict(operation='native', case=name, tier=tier, epoch=epoch + 1,
                                             revision=update['published'], workers=int(tier == 'posix'),
                                             expected_status=0, expected_profile=profile, expected_starts=expected_starts,
                                             expected_hits=tuple(1 - value for value in expected_starts))
                            receipt.stage(**selection)
                        row, artifact = client.native(update['published'], int(tier == 'posix'))
                        if receipt:
                            receipt.stage(**selection, observed=row)
                        assert row['status'] == 0 and row['profile'] == profile, (name, tier, row)
                        assert row['starts'] == expected_starts, (name, tier, expected_starts, row)
                        assert row['hits'] == tuple(1 - value for value in expected_starts), (name, tier, row)
                        assert row['reason'] == '', (name, tier, row)
                        if row['times'][0] == 0:
                            assert row['diagnostics'] == '', (name, tier, row)
                        if context is None:
                            context = row['context']
                            assert row['times'][0] > 0, row
                        else:
                            assert row['context'] == context and row['times'][0] == 0, row
                        seen_profiles.add(profile)
                        key = name, tier
                        checksum = digest(artifact)
                        if epoch:
                            assert fingerprints[key] == checksum, (key, fingerprints[key], checksum)
                        else:
                            fingerprints[key] = checksum
                        native.publish(target, artifact)
                        arguments, expected = controls[key]
                        actual = [result(target, args) for args in arguments]
                        if receipt:
                            receipt.stage(operation='native-run', case=name, tier=tier, epoch=epoch + 1,
                                          arguments=arguments, observed=actual, expected=expected)
                        assert actual == expected, (name, tier, actual, expected)
                        if receipt:
                            receipt.stage(operation='native-repeat', case=name, tier=tier, epoch=epoch + 1,
                                          revision=update['published'], workers=int(tier == 'posix'),
                                          expected_starts=(0, 0, 0), expected_hits=(1, 1, 1),
                                          expected_artifact_sha256=checksum)
                        repeat, repeated = client.native(update['published'], int(tier == 'posix'))
                        if receipt:
                            receipt.stage(operation='native-repeat', case=name, tier=tier, epoch=epoch + 1,
                                          observed=repeat, observed_artifact_sha256=digest(repeated),
                                          expected_starts=(0, 0, 0), expected_hits=(1, 1, 1),
                                          expected_artifact_sha256=checksum)
                        assert repeat['starts'] == (0, 0, 0) and repeat['hits'] == (1, 1, 1), repeat
                        assert repeated == artifact, (name, tier, 'repeated bytes')
                        rows.append(dict(case=name, tier=tier, epoch=epoch + 1, profile=profile,
                                         starts=row['starts'], executable_sha256=checksum,
                                         stdout_sha256=[digest(value[1]) for value in actual]))
                        print('native-corpus-case\t' + json.dumps(rows[-1], sort_keys=True), flush=True)
            if receipt:
                receipt.stage(operation='quit', completed_rows=len(rows))
            client.quit()
            if trace:
                observed = client.trace_rows()
                assert sum(row.startswith('/bin/sh\t') for row in observed) == 1
                assert sum(row.startswith('/bin/rm\t') for row in observed) == 1
                # Each response also independently compared backend exec starts.
                for row in observed:
                    if row.startswith('/bin/sh\t'):
                        assert not Path(row.split('\t')[2]).exists(), 'captured context survived Q'
            assert len(rows) == 80
            print('native-corpus\t20 applications; 80 epoch/tier results; repeat/reset bytes exact; '
                  'ordinary serial/POSIX execution exact' + ('; actual exec observation exact' if trace else ''), flush=True)
        except BaseException as error:
            failure = error
            if receipt:
                try:
                    receipt.capture_private(trace)
                except Exception as capture_error:
                    receipt.private_availability = 'unknown: ' + type(capture_error).__name__
            raise
        finally:
            client = client or (receipt.client if receipt else None)
            if client and hasattr(client, 'process') and client.process.poll() is None:
                client.process.stdin.close()
                try:
                    client.process.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    client.process.kill()
                    client.process.wait()
            if receipt and failure:
                try:
                    receipt.drain_stderr()
                    destination = receipt.archive(failure, current_project)
                    print('native-corpus-failure-receipt\t' + str(destination), flush=True)
                except Exception as archive_error:
                    print('native-corpus-failure-receipt-error\t' + type(archive_error).__name__, flush=True)
            if client and hasattr(client, 'process'):
                for stream in [client.process.stdin, client.process.stdout, client.process.stderr]:
                    if stream and not stream.closed:
                        stream.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('host')
    parser.add_argument('compiler')
    parser.add_argument('--observe', action='store_true')
    parser.add_argument('--failure-dir', help='fresh failure-only evidence destination inside existing build; bounded to 16 MiB')
    arguments = parser.parse_args()
    verify(arguments.host, arguments.compiler, arguments.observe, arguments.failure_dir)
