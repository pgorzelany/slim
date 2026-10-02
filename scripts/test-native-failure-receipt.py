"""Receipt transport/bounds tests; fake hosts never establish native acceptance."""
import contextlib
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import select
import struct
import subprocess
import sys
import tempfile
import time
from types import SimpleNamespace
import unittest
from unittest import mock

from native_failure_receipt import Capture, FailureReceipt, FRAME_PREFIX_LIMIT, METADATA_LIMIT, PRIVATE_LIMIT, STDERR_LIMIT, TOTAL_LIMIT

ROOT = Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location('corpus', ROOT / 'scripts/verify-native-corpus.py')
corpus = importlib.util.module_from_spec(spec)
spec.loader.exec_module(corpus)

HOST = r'''#!PYTHON
import os,struct,sys
def send(tag,payload):
 sys.stdout.buffer.write(tag+struct.pack('>I',len(payload))+payload);sys.stdout.buffer.flush()
def read(size):
 result=b''
 while len(result)<size:
  value=sys.stdin.buffer.read(size-len(result))
  if not value:return result
  result+=value
 return result
send(b'H',b'slim-session\t1\ncompiler\t'+b'1'*64+b'\nruntime\t'+b'2'*64+b'\ntarget\tfixture-target\noptions\t'+b'3'*64+b'\n')
epoch=1;serial=1;profiles=set();cache=set();setup=True;context=b'test-only-context';name=''
while True:
 head=read(5)
 if not head:break
 size,=struct.unpack('>I',head[1:]);body=read(size);tag=head[:1]
 if tag==b'U':
  name=os.path.basename(os.path.dirname(os.fsdecode(body)));serial+=1
  code=b'#define SLIM_PARALLEL 1\n' if name in ['state_machine','signal_network'] else b'control-code'
  payload=struct.pack('>8q',epoch,serial,epoch,serial,0,1,0,0)+bytes([0,0])+struct.pack('>q',len(code))+bytes([0])+struct.pack('>3I',0,0,len(code))+code
  send(b'S',payload)
 elif tag==b'B':
  e,s,workers=struct.unpack('>QQB',body);profile=(1 if workers==0 else 2) if name in ['state_machine','signal_network'] else 0
  key=name,profile;starts=(0,0,0) if key in cache else (1,int(profile not in profiles),1)
  cache.add(key);profiles.add(profile);hits=bytes(1-v for v in starts)
  status=3 if FAIL else 0;reason=b'native-build-failed' if FAIL else b'';artifact=b'' if FAIL else b'fixture-'+name.encode()+bytes([profile])
  times=(1 if setup else 0,10*starts[0],10*starts[1],10*starts[2]);setup=False
  payload=b'\x01'+struct.pack('>5q',e,s,status,profile,123)+struct.pack('>3Q',*starts)+hits+struct.pack('>4Q',*times)+struct.pack('>4I',len(context),len(reason),0,len(artifact))+context+reason+artifact
  if FAIL:sys.stderr.write('receipt-test-stderr\n');sys.stderr.flush()
  send(b'N',payload)
 elif tag==b'R':
  epoch+=1;serial=1;profiles=set();cache=set();send(b'A',struct.pack('>Q',epoch))
 elif tag==b'Q':send(b'Q',b'');break
 else:raise AssertionError(tag)
'''


class ReceiptTests(unittest.TestCase):
    def setUp(self):
        (ROOT / 'build').mkdir(exist_ok=True)
        self.temporary = tempfile.TemporaryDirectory(prefix='native-receipt-test-', dir=ROOT / 'build')
        self.root = Path(self.temporary.name)
        self.host = self.root / 'host'

    def tearDown(self):
        self.temporary.cleanup()

    def receipt(self, name='failure'):
        return FailureReceipt(self.root / name, ROOT, self.host, ROOT / 'build/toolchain/slimc')

    def fake_host(self, fail):
        self.host.write_text(HOST.replace('PYTHON', sys.executable, 1).replace('FAIL', str(fail)))
        self.host.chmod(0o700)

    def drive(self, fail, destination):
        self.fake_host(fail)
        def clean(compiler, project):
            code = b'#define SLIM_PARALLEL 1\n' if project.parent.name in ['state_machine', 'signal_network'] else b'control-code'
            return 0, code
        def build(command, **unused):
            Path(command[-1]).write_bytes(b'ordinary-test-artifact')
            return SimpleNamespace(returncode=0, stdout=b'', stderr=b'')
        with mock.patch.object(corpus.native.module, 'clean', side_effect=clean), \
                mock.patch.object(corpus.subprocess, 'run', side_effect=build), \
                mock.patch.object(corpus, 'result', return_value=(0, b'expected-test-output', b'')), \
                contextlib.redirect_stdout(io.StringIO()):
            corpus.verify(str(self.host), str(ROOT / 'build/toolchain/slimc'), False, destination)

    def test_public_failure_receipt(self):
        destination = self.root / 'failure'
        with self.assertRaises(AssertionError):
            self.drive(True, destination)
        receipt = json.loads((destination / 'receipt.json').read_text())
        self.assertEqual(receipt['stage']['case'], 'gcd_fib')
        self.assertEqual(receipt['stage']['operation'], 'native')
        self.assertEqual(receipt['stage']['expected_status'], 0)
        self.assertEqual(receipt['stage']['observed']['status'], 3)
        self.assertEqual(receipt['host_returncode'], 0)
        self.assertEqual(receipt['identity']['host']['sha256'], hashlib.sha256(self.host.read_bytes()).hexdigest())
        cc = Path(receipt['identity']['CC']['path'])
        self.assertEqual(receipt['identity']['CC']['sha256'], hashlib.sha256(cc.read_bytes()).hexdigest())
        self.assertEqual(receipt['greeting']['compiler'], '1' * 64)
        self.assertIn('unknown', receipt['private_availability'])
        request = (destination / 'request.bin').read_bytes()
        self.assertEqual(request, b'B' + struct.pack('>IQQB', 17, 1, 2, 0))
        response = (destination / 'response.bin').read_bytes()
        self.assertEqual(response[:1], b'N')
        self.assertEqual(struct.unpack('>q', response[22:30])[0], 3)
        stderr = (destination / 'host-stderr.bin').read_bytes()
        self.assertEqual(stderr, b'receipt-test-stderr\n')
        for name, item in receipt['data'].items():
            data = (destination / item['file']).read_bytes()
            self.assertEqual(item['retained_prefix_sha256'], hashlib.sha256(data).hexdigest())
            self.assertTrue(item['complete'], name)
            self.assertEqual(item['sha256'], hashlib.sha256(data).hexdigest())
        self.assertLessEqual(sum(path.stat().st_size for path in destination.iterdir()), TOTAL_LIMIT)
        with self.assertRaises(ValueError):
            self.receipt()

    def test_success_creates_no_archive(self):
        destination = self.root / 'success'
        self.drive(False, destination)
        self.assertFalse(destination.exists())

    def test_partial_response_retains_observed_bytes_and_complete_request(self):
        original = HOST
        partial = original.replace("send(b'N',payload)",
                                   "sys.stdout.buffer.write(b'N'+struct.pack('>I',116)+b'\\x01');sys.stdout.buffer.flush();break")
        destination = self.root / 'partial'
        with mock.patch.dict(globals(), HOST=partial), self.assertRaises(AssertionError):
            self.drive(True, destination)
        receipt = json.loads((destination / 'receipt.json').read_text())
        self.assertEqual((destination / 'response.bin').read_bytes(), b'N' + struct.pack('>I', 116) + b'\x01')
        self.assertFalse(receipt['data']['response']['complete'])
        self.assertIsNone(receipt['data']['response']['sha256'])
        self.assertTrue(receipt['data']['request']['complete'])
        self.assertEqual(receipt['data']['request']['sha256'],
                         hashlib.sha256(b'B' + struct.pack('>IQQB', 17, 1, 2, 0)).hexdigest())

    def test_default_success_creates_no_archive(self):
        self.drive(False, None)
        self.assertEqual(sorted(path.name for path in self.root.iterdir()), ['host'])

    def test_malformed_destinations_before_host_work(self):
        existing = self.root / 'exists';existing.mkdir()
        file = self.root / 'file';file.write_bytes(b'preserve')
        link = self.root / 'link';link.symlink_to(existing, target_is_directory=True)
        cases = [existing, file, link, self.root / 'missing-parent/failure', ROOT / 'outside-build-failure']
        with mock.patch.object(corpus.native, 'Client') as client:
            for destination in cases:
                with self.subTest(destination=destination), self.assertRaises(ValueError):
                    corpus.verify(str(self.host), '/no-compiler', False, destination)
            client.assert_not_called()
        self.assertEqual(file.read_bytes(), b'preserve')

    def test_streamed_prefix_boundaries_and_full_hash(self):
        for bound in [FRAME_PREFIX_LIMIT, STDERR_LIMIT]:
            for length in [bound - 1, bound, bound + 1]:
                with self.subTest(bound=bound, length=length):
                    capture = Capture(bound)
                    expected = hashlib.sha256()
                    left = length
                    while left:
                        data = b'x' * min(left, 65536);expected.update(data);capture.feed(data);left -= len(data)
                    capture.complete = True
                    item = capture.metadata()
                    self.assertEqual(item['sha256'], expected.hexdigest())
                    self.assertEqual(item['retained_bytes'], min(length, bound))
                    self.assertEqual(item['truncated'], length > bound)

    def test_combined_archive_bound_and_stderr_prefix(self):
        receipt = self.receipt()
        block = b'x' * 65536
        for index in range(9):
            capture = receipt.frame('frame-' + str(index))
            for unused in range(FRAME_PREFIX_LIMIT // len(block) + 1):
                capture.feed(block)
            capture.complete = True
        for unused in range(STDERR_LIMIT // len(block) + 1):
            receipt.stderr.feed(block)
        receipt.stderr.complete = True
        destination = receipt.archive(AssertionError('bounded test'))
        result = json.loads((destination / 'receipt.json').read_text())
        self.assertLessEqual(sum(p.stat().st_size for p in destination.iterdir()), TOTAL_LIMIT)
        self.assertEqual(sum(v['retained_bytes'] for v in result['data'].values()), TOTAL_LIMIT - METADATA_LIMIT)
        self.assertTrue(result['data']['frame-8']['truncated'])
        self.assertLessEqual(result['data']['host-stderr']['retained_bytes'], STDERR_LIMIT)

    def test_private_inventory_bound_and_missing_link_inputs(self):
        receipt = self.receipt()
        context = self.root / 'captured';context.mkdir()
        (context / 'captured-sha256.tsv').write_bytes(b'x' * PRIVATE_LIMIT)
        (context / 'context-inputs.sha256').write_bytes(b'y' * PRIVATE_LIMIT)
        (context / 'options.txt').write_bytes(b'z' * PRIVATE_LIMIT)
        trace = self.root / 'trace';trace.write_text('/bin/sh\tcontext.sh\t' + str(context) + '\n')
        receipt.capture_private(trace)
        self.assertLessEqual(sum(len(item.prefix) for item, error in receipt.private.values()), PRIVATE_LIMIT)
        self.assertFalse(receipt.private['captured-sha256.tsv'][0].complete)
        self.assertIsNone(receipt.private['captured-sha256.tsv'][0].metadata()['sha256'])
        self.assertEqual(receipt.private['link.bin'][1]['errno'], 2)

    def test_held_open_stderr_has_deadline_and_unknown_full_hash(self):
        receipt = self.receipt()
        read_fd, write_fd = os.pipe()
        keeper = subprocess.Popen([sys.executable, '-c',
                                   'import os,sys,time;os.write(int(sys.argv[1]),b"held-open");print("ready",flush=True);time.sleep(30)',
                                   str(write_fd)], pass_fds=[write_fd], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        os.close(write_fd)
        stream = os.fdopen(read_fd, 'rb', buffering=0)
        receipt.client = SimpleNamespace(process=SimpleNamespace(stderr=stream, poll=lambda: 0))
        try:
            self.assertTrue(select.select([keeper.stdout], [], [], 2)[0])
            self.assertEqual(keeper.stdout.readline(), b'ready\n')
            started = time.monotonic()
            receipt.drain_stderr(deadline_seconds=0.1)
            self.assertLess(time.monotonic() - started, 1)
            self.assertEqual(receipt.stderr_stop_reason, 'deadline-open-pipe')
            self.assertEqual(receipt.stderr.prefix, b'held-open')
            self.assertFalse(receipt.stderr.complete)
            self.assertIsNone(receipt.stderr.metadata()['sha256'])
            self.assertTrue(os.get_blocking(read_fd))
        finally:
            keeper.terminate()
            try:
                keeper.wait(timeout=2)
            except subprocess.TimeoutExpired:
                keeper.kill();keeper.wait()
            keeper.stdout.close();stream.close()

    def test_stderr_byte_bound_is_incomplete_until_eof_observed(self):
        receipt = self.receipt()
        path = self.root / 'large-stderr';path.write_bytes(b'x' * (STDERR_LIMIT + 1))
        with path.open('rb', buffering=0) as stream:
            receipt.client = SimpleNamespace(process=SimpleNamespace(stderr=stream, poll=lambda: 0))
            receipt.drain_stderr()
        self.assertEqual(len(receipt.stderr.prefix), STDERR_LIMIT)
        self.assertEqual(receipt.stderr.size, STDERR_LIMIT)
        self.assertEqual(receipt.stderr_stop_reason, 'byte-bound-before-eof')
        self.assertFalse(receipt.stderr.complete)
        self.assertIsNone(receipt.stderr.metadata()['sha256'])

    def test_exception_summary_never_calls_unbounded_repr(self):
        class NoRepr:
            def __str__(self):
                raise AssertionError('must not format arbitrary argument')
            def __repr__(self):
                raise AssertionError('must not format arbitrary argument')
        receipt = self.receipt()
        data = b'x' * (2 * 1024 * 1024)
        error = AssertionError(data, 'y' * (2 * 1024 * 1024), NoRepr(), 1 << 4096)
        destination = receipt.archive(error)
        saved = json.loads((destination / 'receipt.json').read_text())
        args = saved['failure']['args']
        self.assertEqual(args[0]['bytes'], len(data))
        self.assertEqual(args[0]['sha256'], hashlib.sha256(data).hexdigest())
        self.assertEqual(len(args[1]['prefix']), 4096)
        self.assertEqual(args[2]['reason'], 'unsupported-metadata-type')
        self.assertEqual(args[3]['bits'], 4097)


if __name__ == '__main__':
    unittest.main()
