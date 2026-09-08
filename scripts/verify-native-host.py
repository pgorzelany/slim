"""Public native frame oracle; executes only complete returned artifacts."""
import importlib.util
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

spec=importlib.util.spec_from_file_location('session_oracle', Path(__file__).with_name('verify-session-host.py'))
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
class Client(module.Client):
    trace=None
    def receive(self):
        header=self.read(5);size,=struct.unpack('>I',header[1:])
        maximum=116+1024+4096+1048576+67108864 if header[:1]==b'N' else 87+4096+1048576+67108864
        assert size<=maximum,(header[:1],size)
        return header[:1],self.read(size)
    def trace_rows(self):
        return self.trace.read_text().splitlines() if self.trace and self.trace.exists() else []
    def check_trace(self, previous, starts):
        if self.trace:
            counts=[0,0,0]
            for line in self.trace_rows()[previous:]:
                arguments=line.split('\t')
                if arguments[0].endswith('/toolchain/bin/clang'):
                    role=0 if 'program.c' in arguments else 1 if 'slim_rt.c' in arguments else 2
                    counts[role]+=1
            assert tuple(counts)==starts,('external exec observation',counts,starts)
    def native(self, revision, workers=0):
        previous=len(self.trace_rows())
        tag,payload=self.request(b'B', struct.pack('>QQB', *revision, workers))
        assert tag==b'N' and len(payload)>=116 and payload[0]==1,(tag,payload[:200])
        epoch,serial,status,profile,elapsed=struct.unpack('>5q',payload[1:41])
        starts=struct.unpack('>3Q',payload[41:65]);hits=tuple(payload[65:68])
        times=struct.unpack('>4Q',payload[68:100]);lengths=struct.unpack('>4I',payload[100:116])
        assert sum(lengths)+116==len(payload),(lengths,len(payload))
        at=116;strings=[]
        for size in lengths:strings.append(payload[at:at+size]);at+=size
        context,reason,diagnostics,artifact=strings
        signed=tuple(value if value<2**63 else value-2**64 for value in revision)
        assert (epoch,serial)==signed and all(h in [0,1] for h in hits)
        assert bool(artifact)==(status==0),(status,reason,diagnostics)
        self.check_trace(previous, starts)
        row=dict(revision=revision,status=status,profile=profile,ns=elapsed,starts=starts,hits=hits,times=times,context=context.decode(),reason=reason.decode(),diagnostics=diagnostics.decode(),bytes=len(artifact))
        print('native-host',json.dumps(row,sort_keys=True),sep='\t',flush=True)
        return row,artifact

def publish(path,data):
    staging=path.with_suffix('.next');staging.write_bytes(data);staging.chmod(0o700);staging.replace(path)
def execute(path,expected):
    result=subprocess.run([str(path)],capture_output=True,timeout=120)
    assert (result.returncode,result.stdout,result.stderr)==expected,(result.returncode,result.stdout,result.stderr,expected)

def framing(host, observe=False):
    with tempfile.TemporaryDirectory(prefix='slim-native-frames-') as temporary:
        trace=Path(temporary)/'executed.tsv' if observe else None
        environment={'SLIM_NATIVE_EXEC_REPORT':str(trace)} if trace else {}
        frames=[b'B'+struct.pack('>I',size) for size in [0,1,16,18,2**32-1]]
        frames += [(b'B'+struct.pack('>I',17))[:size] for size in range(1,5)]
        frames += [b'B'+struct.pack('>I',17)+bytes(size) for size in range(17)]
        for frame in frames:
            client=Client([host],environment)
            client.write(frame);client.process.stdin.close()
            assert client.receive()==(b'E',b'H0001')
            client.finish(65)
        client=Client([host],environment);client.trace=trace
        try:
            for revision in [(0,0),(0,1),(1,0),(2**63,1),(1,2**64-1)]:
                row,_=client.native(revision)
                assert row['status']==1 and row['reason']=='invalid-revision' and row['starts']==(0,0,0),row
            for workers in [2,255]:
                row,_=client.native((1,1),workers)
                assert row['status']==1 and row['reason']=='invalid-workers' and row['starts']==(0,0,0),row
            client.quit()
            assert not trace or not trace.exists(),'invalid frames started external work'
        finally:
            if client.process.poll() is None:
                client.process.stdin.close();client.process.wait(timeout=10)
    print('native-host-framing\t26 malformed/partial frames and 7 invalid selections; no native work',flush=True)

def partial_output(host, observe=False):
    with tempfile.TemporaryDirectory(prefix='slim-native-partial-') as temporary:
        root=Path(temporary)
        source=module.project(root/'source','module main\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.println("'+('a'*1048576)+'")\n  0\n')
        trace=root/'executed.tsv' if observe else None
        client=Client([host],{'SLIM_NATIVE_EXEC_REPORT':str(trace)} if trace else {})
        client.trace=trace
        try:
            accepted=client.update(source);assert accepted['status']==0,accepted
            previous=len(client.trace_rows())
            client.write(b'B'+struct.pack('>IQQB',17,*accepted['published'],0))
            header=client.read(5);size,=struct.unpack('>I',header[1:])
            assert header[:1]==b'N' and size>1048576,(header,size)
            prefix=client.read(116)
            assert prefix[0]==1 and struct.unpack('>q',prefix[17:25])[0]==0,prefix
            assert struct.unpack('>I',prefix[112:116])[0]>1048576,prefix
            starts=struct.unpack('>3Q',prefix[41:65]);assert starts==(1,1,1),starts
            client.process.stdout.close();client.process.stdin.close()
            assert client.process.wait(timeout=15)==65
            assert client.process.stderr.read()==b''
            client.check_trace(previous,starts)
            if trace:
                rows=client.trace_rows()
                assert sum(row.startswith('/bin/rm\t') for row in rows)==1,rows
                for row in rows:
                    if row.startswith('/bin/sh\t'):
                        assert not Path(row.split('\t')[2]).exists(),'context survived partial publication'
        finally:
            if client.process.poll() is None:
                client.process.stdin.close()
                try: client.process.wait(timeout=15)
                except subprocess.TimeoutExpired: client.process.kill();client.process.wait()
    print('native-host-partial\tinterrupted native artifact frame terminates and removes context',flush=True)

def smoke(host, observe=False):
    with tempfile.TemporaryDirectory(prefix='slim-native-client-') as temporary:
        root=Path(temporary)
        before=module.project(root/'before','module main\n\nfn main(args: Vec[Bytes]) -> I64 effects[io]:\n  io.print_i64(10)\n  0\n')
        after=module.project(root/'after',before.with_name('program.slim').read_text().replace('(10)','(11)'))
        invalid=module.project(root/'invalid',before.with_name('program.slim').read_text().replace('  0\n','  true\n'))
        trace=root/'executed.tsv' if observe else None
        client=Client([host], {'SLIM_NATIVE_EXEC_REPORT':str(trace)} if trace else {})
        client.trace=trace
        try:
            rejected,_=client.native((1,1));assert rejected['status']==1 and rejected['starts']==(0,0,0)
            first=client.update(before);assert first['status']==0,first
            revision=first['published']
            cold,artifact=client.native(revision)
            assert cold['status']==0 and cold['starts']==(1,1,1) and cold['hits']==(0,0,0),cold
            if trace:
                captures=[row.split('\t')[2] for row in client.trace_rows() if row.startswith('/bin/sh\t')]
                assert len(captures)==1,captures
                subprocess.run([sys.executable,'-B',str(Path(__file__).with_name('verify-native-driver.py')),captures[0]],check=True)
                subprocess.run([sys.executable,'-B',str(Path(__file__).with_name('verify-native-context.py')),captures[0]],check=True)
            target=root/'program';publish(target,artifact);execute(target,(0,b'10',b''))
            warm,repeated=client.native(revision)
            assert repeated==artifact and warm['starts']==(0,0,0) and warm['hits']==(1,1,1),warm
            bad=client.update(invalid);assert bad['status']!=0,bad
            rejected,_=client.native(bad['attempt']);assert rejected['status']==1 and rejected['starts']==(0,0,0)
            old,retained=client.native(revision);assert retained==artifact and old['starts']==(0,0,0)
            changed=client.update(after);assert changed['status']==0,changed
            rejected,_=client.native(revision);assert rejected['status']==1 and rejected['starts']==(0,0,0)
            body,artifact=client.native(changed['published'])
            assert body['starts']==(1,0,1) and body['hits']==(0,1,0),body
            publish(target,artifact);execute(target,(0,b'11',b''))
            reverted=client.update(before);assert reverted['status']==0,reverted
            revert,artifact=client.native(reverted['published']);assert artifact==repeated and revert['starts']==(0,0,0),revert
            client.reset()
            rejected,_=client.native(reverted['published']);assert rejected['status']==1 and rejected['starts']==(0,0,0)
            renewed=client.update(before);assert renewed['status']==0,renewed
            reset,artifact=client.native(renewed['published'])
            assert reset['starts']==(1,1,1) and reset['hits']==(0,0,0) and reset['times'][0]==0,reset
            assert reset['context']==cold['context'] and artifact==repeated,reset
            client.quit()
            if trace:
                rows=client.trace_rows()
                assert sum(row.startswith('/bin/sh\t') for row in rows)==1,rows
                assert sum(row.startswith('/bin/rm\t') for row in rows)==1,rows
                print('native-host-observed\tactual backend exec attempts match every response; one capture and one teardown across reset',flush=True)
        finally:
            if client.process.poll() is None:
                client.process.stdin.close()
                try: client.process.wait(timeout=10)
                except subprocess.TimeoutExpired: client.process.kill();client.process.wait()
    print('native-host\tpublic cold/unchanged/edit/revert/reject/reset artifacts exact',flush=True)

if __name__ == "__main__":
    framing(sys.argv[1], "--observe" in sys.argv)
    smoke(sys.argv[1], "--observe" in sys.argv)
    partial_output(sys.argv[1], "--observe" in sys.argv)
