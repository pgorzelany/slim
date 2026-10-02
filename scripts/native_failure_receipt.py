"""Opt-in, bounded evidence for the native corpus verifier. No acceptance rules."""
import hashlib
import json
from itertools import islice
import math
import os
from pathlib import Path
import select
import shutil
import time

TOTAL_LIMIT = 16 * 1024 * 1024
METADATA_LIMIT = 64 * 1024
FRAME_PREFIX_LIMIT = 2 * 1024 * 1024
STDERR_LIMIT = 1024 * 1024
PRIVATE_LIMIT = 2 * 1024 * 1024


class Capture:
    def __init__(self, limit):
        self.limit = limit
        self.prefix = bytearray()
        self.size = 0
        self.hash = hashlib.sha256()
        self.complete = False

    def feed(self, data):
        self.hash.update(data)
        self.size += len(data)
        remaining = self.limit - len(self.prefix)
        if remaining > 0:
            self.prefix.extend(data[:remaining])

    def metadata(self):
        return dict(observed_bytes=self.size, sha256=self.hash.hexdigest() if self.complete else None,
                    observed_extent_sha256=self.hash.hexdigest(), complete=self.complete,
                    retained_bytes=len(self.prefix), truncated=self.size > len(self.prefix))


def file_identity(path, limit=512 * 1024 * 1024):
    """Hash a fixed initial extent; a changing or over-bound file stays unknown."""
    path = Path(path)
    try:
        before = path.stat()
        if not path.is_file() or before.st_size > limit:
            return dict(path=str(path), sha256=None, reason='not-file-or-hash-bound', bytes=before.st_size)
        checksum = hashlib.sha256()
        with path.open('rb') as stream:
            remaining = before.st_size
            while remaining:
                data = stream.read(min(65536, remaining))
                if not data:
                    break
                checksum.update(data)
                remaining -= len(data)
            extra = stream.read(1)
        after = path.stat()
        stable = (before.st_dev, before.st_ino, before.st_size, before.st_mtime_ns) == \
                 (after.st_dev, after.st_ino, after.st_size, after.st_mtime_ns)
        return dict(path=str(path), bytes=before.st_size,
                    sha256=checksum.hexdigest() if stable and not remaining and not extra else None,
                    reason='' if stable and not remaining and not extra else 'changed-during-hash')
    except OSError as error:
        return dict(path=str(path), sha256=None, reason=type(error).__name__, errno=error.errno)


def summary(value, depth=0):
    if depth > 6:
        return 'metadata-depth-bound'
    if isinstance(value, bytes):
        return dict(bytes=len(value), sha256=hashlib.sha256(value).hexdigest())
    if isinstance(value, str):
        if len(value) <= 4096:
            return value
        checksum = hashlib.sha256()
        for at in range(0, len(value), 65536):
            checksum.update(value[at:at + 65536].encode('utf-8', errors='surrogatepass'))
        return dict(prefix=value[:4096], characters=len(value), sha256=checksum.hexdigest(),
                    sha256_encoding='utf8-surrogatepass')
    if isinstance(value, dict):
        return {(key[:128] if isinstance(key, str) else 'key-type-' + type(key).__name__): summary(item, depth + 1)
                for key, item in islice(value.items(), 128)}
    if isinstance(value, (tuple, list)):
        return [summary(item, depth + 1) for item in value[:128]]
    if isinstance(value, int) and value.bit_length() > 256:
        return dict(type='integer', bits=value.bit_length(), reason='metadata-scalar-bound')
    if isinstance(value, float) and not math.isfinite(value):
        return dict(type='float', reason='nonfinite-metadata')
    if value is None or isinstance(value, (int, float, bool)):
        return value
    return dict(type=type(value).__name__, reason='unsupported-metadata-type')


class FailureReceipt:
    def __init__(self, destination, repository, host, compiler):
        self.repository = Path(repository).resolve()
        self.destination = Path(destination).absolute()
        build = (self.repository / 'build').resolve()
        try:
            parent = self.destination.parent.resolve(strict=True)
        except OSError as error:
            raise ValueError('--failure-dir parent must already exist inside build') from error
        if not parent.is_dir() or not parent.is_relative_to(build) or \
                self.destination.name in ('', '.', '..') or self.destination.exists() or self.destination.is_symlink():
            raise ValueError('--failure-dir must be a fresh destination inside the existing build directory')
        self.destination = parent / self.destination.name
        self.frames, self.private = {}, {}
        self.stderr = Capture(STDERR_LIMIT)
        self.stderr_stop_reason = 'not-drained'
        self.context = {}
        self.client = None
        self.private_availability = 'unknown: ordinary public replies remove link.bin; no execution trace available'
        cc = shutil.which(os.environ.get('CC', 'cc'))
        paths = {'host': Path(host), 'compiler': Path(compiler), 'CC': Path(cc) if cc else Path('/missing-CC')}
        for relative in ['runtime/slim_rt.c', 'runtime/slim_rt.h', 'bootstrap/slimc-seed.c',
                         'compiler/session.c', 'compiler/native.c', 'compiler/native-context.sh',
                         'scripts/verify-native-corpus.py', 'scripts/verify-native-host.py',
                         'scripts/verify-session-host.py', 'scripts/native_failure_receipt.py',
                         'benchmarks/challenges/manifest.tsv', 'slimc']:
            paths[relative] = self.repository / relative
        for path in sorted((self.repository / 'selfhost').glob('*.slim')):
            paths[str(path.relative_to(self.repository))] = path
        paths['selfhost/slim.project'] = self.repository / 'selfhost/slim.project'
        for name in ['session-build-identity.tsv', 'session-identity.h', 'native-inputs.h']:
            paths['host-sidecar/' + name] = Path(host).parent / name
        self.identities = {name: file_identity(path) for name, path in paths.items()}
        try:
            identity_file = Path(host).parent / 'session-build-identity.tsv'
            if identity_file.stat().st_size > METADATA_LIMIT:
                raise ValueError('sidecar metadata bound')
            self.recorded_host_inputs = dict(line.split('\t') for line in identity_file.read_text().splitlines())
        except (OSError, ValueError):
            self.recorded_host_inputs = None

    def stage(self, **metadata):
        self.context = summary(metadata)

    def frame(self, name):
        capture = Capture(FRAME_PREFIX_LIMIT)
        self.frames[name] = capture
        return capture

    def capture_file(self, name, path, prefix_limit, read_limit=1024 * 1024):
        capture = Capture(prefix_limit)
        try:
            with Path(path).open('rb') as stream:
                remaining = read_limit
                while remaining:
                    data = stream.read(min(65536, remaining))
                    if not data:
                        capture.complete = True
                        break
                    capture.feed(data)
                    remaining -= len(data)
                if not remaining:
                    capture.complete = not stream.read(1)
            return capture, None
        except OSError as error:
            return capture, dict(reason=type(error).__name__, errno=error.errno)

    def capture_private(self, trace):
        if not trace or not Path(trace).exists():
            return
        capture, error = self.capture_file('trace', trace, PRIVATE_LIMIT)
        self.private['execution-trace'] = (capture, error)
        rows = bytes(capture.prefix).decode(errors='replace').splitlines()
        roots = [Path(row.split('\t')[2]) for row in rows
                 if row.startswith('/bin/sh\t') and len(row.split('\t')) >= 3]
        if len(roots) != 1:
            self.private_availability = 'unknown: capture directory not uniquely available in bounded trace'
            return
        self.private_availability = 'available trace paths sampled before host cleanup; missing files stay unknown'
        remaining = PRIVATE_LIMIT - len(capture.prefix)
        names = ['captured-sha256.tsv', 'context-inputs.sha256', 'context.sha256', 'options.txt',
                 'compiler-version.txt', 'target.txt', 'capture-size.tsv', 'link-job.txt', 'link.bin']
        names += [f'compile-job-{profile}-{role}.txt' for profile in range(3) for role in ['program', 'runtime']]
        for name in names:
            item, error = self.capture_file(name, roots[0] / name, remaining)
            self.private[name] = (item, error)
            remaining -= len(item.prefix)

    def archive(self, error, project=None):
        # Exclusive creation prevents a retry from replacing the first failure.
        self.destination.mkdir()
        remaining = TOTAL_LIMIT - METADATA_LIMIT
        data = {}

        def save(name, capture, unavailable=None):
            nonlocal remaining
            prefix = capture.prefix[:remaining]
            (self.destination / (name + '.bin')).write_bytes(prefix)
            remaining -= len(prefix)
            item = capture.metadata()
            item.update(file=name + '.bin', retained_bytes=len(prefix), truncated=capture.size > len(prefix),
                        retained_prefix_sha256=hashlib.sha256(prefix).hexdigest(), unavailable=unavailable)
            data[name] = item

        for name, capture in self.frames.items():
            save(name, capture)
        save('host-stderr', self.stderr)
        for name, (capture, unavailable) in self.private.items():
            save('private-' + name, capture, unavailable)
        if project:
            for name, path in [('project', Path(project)), ('source', Path(project).with_name('program.slim'))]:
                capture, unavailable = self.capture_file(name, path, remaining, read_limit=FRAME_PREFIX_LIMIT)
                save(name, capture, unavailable)
        client = self.client
        result = dict(schema=1, scope='failure evidence only; acceptance remains the production native oracle',
                      bounds=dict(total_bytes=TOTAL_LIMIT, stderr_prefix_bytes=STDERR_LIMIT,
                                  private_prefix_bytes=PRIVATE_LIMIT, frame_prefix_bytes=FRAME_PREFIX_LIMIT),
                      failure=dict(type=type(error).__name__, args=summary(error.args)),
                      stage=self.context, identity=self.identities, recorded_host_inputs=self.recorded_host_inputs,
                      identity_scope='Executable and checkout file bytes observed before verification; checkout source hashes do not infer loaded binary lineage. Recorded host sidecars and public greeting are separate evidence.',
                      greeting=getattr(client, 'identity', None),
                      host_returncode=client.process.poll() if client and hasattr(client, 'process') else None,
                      stderr_stop_reason=self.stderr_stop_reason,
                      private_availability=self.private_availability, data=data)
        encoded = (json.dumps(result, sort_keys=True, indent=2) + '\n').encode()
        if len(encoded) > METADATA_LIMIT:
            raise ValueError('failure receipt metadata exceeded its fixed bound')
        (self.destination / 'receipt.json').write_bytes(encoded)
        return self.destination

    def drain_stderr(self, deadline_seconds=2.0):
        """A reaped leader does not imply EOF: a descendant may retain the pipe."""
        client = self.client
        if not client or not hasattr(client, 'process'):
            self.stderr_stop_reason = 'no-process'
            return
        stream = client.process.stderr
        if stream.closed:
            self.stderr_stop_reason = 'eof-already-collected' if self.stderr.complete else 'closed-without-eof'
            return
        descriptor = stream.fileno()
        blocking = os.get_blocking(descriptor)
        deadline = time.monotonic() + deadline_seconds
        remaining = STDERR_LIMIT
        try:
            os.set_blocking(descriptor, False)
            while remaining:
                duration = deadline - time.monotonic()
                if duration <= 0 or not select.select([descriptor], [], [], max(0, duration))[0]:
                    self.stderr_stop_reason = 'deadline-open-pipe'
                    return
                try:
                    chunk = os.read(descriptor, min(65536, remaining))
                except (BlockingIOError, InterruptedError):
                    continue
                if not chunk:
                    self.stderr.complete = True
                    self.stderr_stop_reason = 'eof'
                    return
                self.stderr.feed(chunk)
                remaining -= len(chunk)
            self.stderr_stop_reason = 'byte-bound-before-eof'
        except OSError as error:
            self.stderr_stop_reason = 'read-error-' + str(error.errno)
        finally:
            os.set_blocking(descriptor, blocking)


def client_class(base):
    """Observe the existing transport; native decoding and assertions stay inherited."""
    class Stderr:
        def __init__(self, stream, capture):
            self.stream, self.capture = stream, capture

        def read(self, size=-1):
            data = self.stream.read(size)
            self.capture.feed(data)
            if size < 0 or not data:
                self.capture.complete = True
            return data

        def __getattr__(self, name):
            return getattr(self.stream, name)

    class Client(base):
        def __init__(self, command, environment, receipt):
            self.receipt = receipt
            receipt.client = self
            self._writing = self._receiving = None
            self._request_tag = None
            self._greeted = False
            super().__init__(command, environment)

        def read(self, size):
            if not isinstance(self.process.stderr, Stderr):
                self.process.stderr = Stderr(self.process.stderr, self.receipt.stderr)
            # Same deadline and read loop as the public session transport, with
            # a bounded streamed prefix observer at the actual os.read boundary.
            pieces = []
            deadline = time.monotonic() + 120
            while size:
                remaining = deadline - time.monotonic()
                assert remaining > 0 and select.select([self.process.stdout], [], [], remaining)[0], 'response timeout'
                data = os.read(self.process.stdout.fileno(), min(size, 65536))
                assert data, ('incomplete response', size, self.process.poll())
                if self._receiving:
                    self._receiving.feed(data)
                pieces.append(data)
                size -= len(data)
            return b''.join(pieces)

        def write(self, data):
            while data:
                count = os.write(self.process.stdin.fileno(), data)
                assert count > 0
                if self._writing:
                    self._writing.feed(data[:count])
                data = data[count:]

        def request(self, tag, payload=b'', fragmented=False):
            self._request_tag = tag
            self._writing = self.receipt.frame('update-request' if tag == b'U' else 'request')
            try:
                result = super().request(tag, payload, fragmented)
                return result
            finally:
                self._writing.complete = self._writing.size == 5 + len(payload)
                self._writing = None

        def receive(self):
            name = 'greeting' if not self._greeted else 'update-response' if self._request_tag == b'U' else 'response'
            self._receiving = self.receipt.frame(name)
            try:
                result = super().receive()
                self._receiving.complete = True
                self._greeted = True
                return result
            finally:
                self._receiving = None

    return Client
