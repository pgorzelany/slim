"""Behavioral native-query checks through generated production SLIM."""
from pathlib import Path
import hashlib
import json
import os
import shlex
import subprocess
import sys

root=Path(sys.argv[1]);scope=sys.argv[2];compiler=sys.argv[3]
identity_paths = [Path(compiler), root/'probe.c', root/'cost', root/'cost-observed',
                  Path('selfhost/nativecache.slim'), Path('runtime/slim_rt.c'),
                  Path('runtime/slim_rt.h'), Path('tests/fixtures/native_cache.slim'),
                  Path('tests/fixtures/native_cache_host.c'),
                  Path('scripts/instrument-native-cache.py'), Path(__file__)]
identities = {str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in identity_paths}
identities['cc_version'] = subprocess.check_output(
    [*shlex.split(os.environ.get('CC', 'cc')), '--version'], text=True).strip()
print('native-query-identities', json.dumps(identities, sort_keys=True), sep='\t', flush=True)
modes=['state-epoch','state-context','state-records','state-bytes','header-used','header-disabled','header-seal','key-epoch','key-context','key-role','key-workers','key-first','key-second','entry-fingerprint','entry-artifact','entry-seal','headers-remove','headers-extra','entries-remove','entries-extra']
# Header-seal replacement is deliberately repaired by resealing; do not call that
# a corrupted cache. Other scalar/frame/payload corruptions must remain misses.
resealed=[m for m in modes if m not in ['header-seal','headers-remove','headers-extra']]
for binary in ['ordinary','sanitized']:
    for arguments in [[]]+[[m] for m in modes]+[[m,'reseal'] for m in resealed]:
        r=subprocess.run([str(root/binary),*arguments],capture_output=True,timeout=60)
        assert (r.returncode,r.stdout,r.stderr)==(0,b'ok native query\n',b''),(binary,arguments,r.returncode,r.stdout,r.stderr)
    print('native-query-cases',binary,1+len(modes)+len(resealed),'exact',sep='\t',flush=True)
subprocess.run([str(root/'borrowed')],check=True,timeout=60)
print('native-query-ownership\tcontext/key/artifact remain exact after host buffers are overwritten; ASan/UBSan',flush=True)
if scope in ['full','faults']:
    for binary in ['ordinary','sanitized']:
        failed=0
        for ordinal in range(1,2049):
            r=subprocess.run([str(root/binary),'fault'],env={**os.environ,'SLIM_ALLOC_FAIL_AT':str(ordinal)},capture_output=True,timeout=30)
            if r.returncode==71:
                assert r.stdout==b'' and r.stderr==f'SLIM allocation failure: exhausted at allocation {ordinal}\n'.encode(),(binary,ordinal,r)
                failed+=1
            else:
                assert (r.returncode,r.stdout,r.stderr)==(0,b'ok native query\n',b''),(binary,ordinal,r)
                endpoint=subprocess.run([str(root/binary),'fault'],env={**os.environ,'SLIM_ALLOC_FAIL_AT':'2048'},capture_output=True,timeout=30)
                assert (endpoint.returncode,endpoint.stdout,endpoint.stderr)==(r.returncode,r.stdout,r.stderr),(binary,ordinal,endpoint)
                break
        assert 0<failed<2048,(binary,failed)
        print('native-query-faults',binary,'failed',failed,'complete',2,'cap',2048,sep='\t',flush=True)

# Actual external objects pass through production SLIM, are relinked, and the
# retrieved executable runs. The fixture identity is not a public host context.
if scope in ['full', 'quick']:
    import hashlib
    import shlex
    cc = shlex.split(os.environ.get('CC', 'cc'))
    flags = ['-std=c11', '-O3', '-DNDEBUG', '-Wall', '-Wextra', '-Werror']
    empty = root / 'empty'
    empty.write_bytes(b'')
    for case in ['vector_sum', 'state_machine', 'signal_network']:
        source = Path('examples/vector_sum.slim') if case == 'vector_sum' else Path('benchmarks/challenges') / case / 'program.slim'
        c_file = root / 'program.c'
        code = subprocess.check_output([compiler, str(source)], timeout=60)
        c_file.write_bytes(code)
        parallel = b'#define SLIM_PARALLEL 1\n' in code
        for tier in (['serial', 'posix'] if parallel else ['none']):
            mode = ['-DSLIM_PARALLEL=1'] if parallel else []
            if tier == 'posix':
                mode += ['-DSLIM_POSIX_WORKERS=1', '-pthread']
            objects = [root / 'program.o', root / 'runtime.o']
            for source_file, target in zip([c_file, Path('runtime/slim_rt.c')], objects):
                subprocess.run([*cc, *flags, *mode, '-I', 'runtime', '-c', str(source_file), '-o', str(target)], capture_output=True, check=True, timeout=60)
            executable = root / 'linked'
            link = [*cc, *flags, *mode, *map(str, objects), '-o', str(executable)]
            subprocess.run(link, capture_output=True, check=True, timeout=60)
            expected = subprocess.run([str(executable)], capture_output=True, timeout=60)
            assert expected.returncode == 0, (case, tier, expected)
            cases = [('object', c_file, empty, objects[0]),
                     ('runtime', Path('runtime/slim_rt.c'), Path('runtime/slim_rt.h'), objects[1]),
                     ('link', objects[0], objects[1], executable)]
            for binary in ['ordinary', 'sanitized']:
                for role, first, second, artifact in cases:
                    original = artifact.read_bytes()
                    args = [str(root / binary), role, str(first), str(second), str(artifact)]
                    if parallel:
                        args += [tier]
                    result = subprocess.run(args, capture_output=True, timeout=60)
                    assert (result.returncode, result.stdout, result.stderr) == (0, original, b''), (case, tier, binary, role, result.returncode, result.stderr)
                    # Publish a fresh inode: overwriting an executed Mach-O file
                    # in place can invalidate the kernel's cached code signature.
                    publication = artifact.with_suffix('.retrieved')
                    publication.write_bytes(result.stdout)
                    publication.chmod(0o700)
                    publication.replace(artifact)
                actual = subprocess.run([str(executable)], capture_output=True, timeout=60)
                assert (actual.returncode, actual.stdout, actual.stderr) == (expected.returncode, expected.stdout, expected.stderr), (case, tier, binary, actual.returncode, actual.stdout, actual.stderr)
                subprocess.run(link, capture_output=True, check=True, timeout=60)
                actual = subprocess.run([str(executable)], capture_output=True, timeout=60)
                assert (actual.returncode, actual.stdout, actual.stderr) == (expected.returncode, expected.stdout, expected.stderr), (case, tier, binary, 'relink')
            print('native-query-produced', case, tier, 'three roles; ordinary/sanitized; retained objects relink and execute', hashlib.sha256(executable.read_bytes()).hexdigest(), sep='\t', flush=True)

# Independent observations distinguish attempted publication from copied bytes,
# and logical admission from live allocations (including vector capacity).
if scope in ['full', 'costs']:
    import json
    import statistics
    rows = []
    for size in [1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216]:
        observed = subprocess.run([str(root/'cost-observed'), 'cost', str(size)], capture_output=True, check=True, timeout=120)
        assert observed.stderr == b'', observed.stderr
        parsed = [line.split('\t') for line in observed.stdout.decode().splitlines()]
        assert [r[0] for r in parsed] == ['publish', 'hit', 'miss', 'duplicate'], parsed
        facts = {r[0]: list(map(int, r[1:])) for r in parsed}
        # ns, live bytes, cumulative allocation attempts, lookup/publish/frames,
        # executed byte copies, hash bytes, equality bytes.
        assert facts['publish'][6] == size * 2, facts
        assert all(facts[stage][6] == 0 for stage in ['hit', 'miss', 'duplicate']), facts
        assert len({facts[stage][2] for stage in facts}) == 1, facts
        assert facts['hit'][7] >= size * 2 and facts['hit'][8] >= size, facts
        samples = {stage: [] for stage in facts}
        for trial in range(6):
            result = subprocess.run([str(root/'cost'), 'cost', str(size)], capture_output=True, check=True, timeout=60)
            assert result.stderr == b'', result.stderr
            for line in result.stdout.decode().splitlines():
                stage, ns, live, allocations = line.split('\t')
                assert [int(live), int(allocations)] == facts[stage][1:3], (size, stage, line, facts)
                if trial:
                    samples[stage].append(int(ns))
        rows.append({'input_bytes': size, 'artifact_bytes': size,
                     'observed': facts, 'ns_samples': samples,
                     'ns_median': {k: statistics.median(v) for k, v in samples.items()}})
        print('native-query-cost', json.dumps(rows[-1], separators=(',', ':')), sep='\t', flush=True)
    for binary in ['cost', 'cost-observed']:
        result = subprocess.run([str(root/binary), 'boundary', '67108864'], capture_output=True, check=True, timeout=120)
        assert result.stderr == b'', result.stderr
        print('native-query-byte-boundary', binary, '64 MiB admitted; one byte beyond rejected without allocation', sep='\t', flush=True)
