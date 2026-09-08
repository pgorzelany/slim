# M1 native artifact query checkpoint

Status: RFC-0145 implemented and verified; public native integration and full M1
closure remain incomplete.
Baseline source: `d842ff5ae134646f4e9600aac76eff5214eac702`.
Candidate seed: `e54b3c1ed37f3548ebad1d74ac6d6088a4b1f50fbda4906769193471a0e6550a`.
Candidate host: `7a438c5c27bf717e4f63402c40fd383968f41218ce42152acea58a5e0399f6b3`.

## Scope and result

[RFC-0145](../../design/rfcs/0145-retained-native-artifact-queries.md) implements
private native artifact retention in production SLIM. Complete byte keys,
epoch/context ownership, object/runtime/link roles and worker configuration
control reuse. Admission is limited to 256 records and 64 MiB of copied input and
output bytes. Corrupt history and conflicting outputs disable retention until
reset. Borrowed host buffers are cloned on successful admission.

This library is not yet connected to public builds. The complete captured native
context, accepted source revision, external operations and atomic output
publication still need the companion host contract and implementation. Neither
this checkpoint nor the Python feasibility experiment establishes a public
build-time or agent-productivity improvement.

## Verified domains

The production compiler generates the query fixture. Ordinary and ASan/UBSan
executions pass 38 driver cases each: basic operations, all key dimensions,
intentional fingerprint collisions, record/context limits, capacity exhaustion,
20 damaged-history variants and 17 independently resealed variants. Synthetic
artifact bytes are deliberately not interpreted as source or native code.

A C ownership oracle overwrites all original context, input and output buffers;
the retained query still returns the original bytes under ASan/UBSan. The byte
boundary admits exactly 64 MiB and rejects one byte beyond it before any new
allocation. Small-byte admission, record 256/257, context 1024/1025 and extreme
scalar metadata have permanent tests. Corruption conservatively reserves the
entire admission limit; that reservation is not a physical allocation count.

The basic allocation campaign fails exactly at ordinals 1–6 with the existing
terminal failure status and no output. Ordinals 7 and 2048 reproduce complete
success, both ordinary and sanitized. This is a bounded six-allocation-path
campaign, not exhaustive fault injection over every artifact size.

Actual backend-generated object, runtime and executable bytes round-trip through
both query binaries. Retrieved objects relink and retrieved executables run with
identical status/stdout/stderr for `vector_sum` and for `state_machine` and
`signal_network` in serial and POSIX modes: five configurations, three artifact
roles and two query builds. These tests establish exact byte preservation and
execution for this domain; they do not supply a production native context.

## Observed work and costs

The verification-only generated C instrumentation counts executed lookup,
publication and entry-frame calls, byte-copy loops, hash loops and equality loops.
It does not infer work from a reported cache hit. On Darwin 25.6.0 arm64 with Apple clang 21 (`clang-2100.1.1.101`),
separate uninstrumented O3 executions provide five timing samples after one warmup. Each row has one record
and equal input/output lengths. Context length is seven bytes.

| Input bytes | Artifact bytes | Publish median | Hit median | Live region bytes |
| ---: | ---: | ---: | ---: | ---: |
| 1,024 | 1,024 | 0.006 ms | 0.002 ms | 3,280 |
| 4,096 | 4,096 | 0.019 ms | 0.008 ms | 9,424 |
| 16,384 | 16,384 | 0.061 ms | 0.030 ms | 34,000 |
| 65,536 | 65,536 | 0.229 ms | 0.121 ms | 132,304 |
| 262,144 | 262,144 | 0.868 ms | 0.475 ms | 525,520 |
| 1,048,576 | 1,048,576 | 3.232 ms | 1.851 ms | 2,098,384 |
| 4,194,304 | 4,194,304 | 15.304 ms | 8.658 ms | 8,389,840 |
| 16,777,216 | 16,777,216 | 58.049 ms | 31.928 ms | 33,555,664 |

For input/output length `n`, admission executes exactly `2n` byte copies, `3n+35`
hash steps and 14 equality steps. A hit executes zero copies, `2n+21` hash steps
and `n+21` equality steps. Hits, misses and duplicate publication perform no new
allocations. These are checked deterministic work facts for the named geometric
fixture, not estimates of the external compiler's work. Live region bytes include
allocation headers and vector capacities; they exclude caller buffers, process
memory and transient reallocation peaks. The input/output domain stops at 16 MiB
each; the separate 64 MiB admission test is not a timing gate.

The exact-key and integrity work remains visible: reading a large retained
artifact is not free. Native context capture and full build latency still need
integrated measurements before any speedup claim. Existing frontend and runtime
budgets remain unchanged.

## Feasibility observations and failures

A private captured-toolchain experiment reproduces all 20 application outputs
in serial/POSIX configurations, and repeat builds perform zero external backend
calls in that experiment. Twelve preprocessor comparisons cover token and macro
views of the generated prefix/runtime across all three worker profiles. The
snapshot has 212 files and 224,311,360 logical bytes. Its capture cost and storage
are material; this Python experiment is not production implementation.

The native test initially overwrote an already executed binary in place. The
retrieved bytes compared exactly but execution failed. Publishing a fresh inode
with atomic rename fixes the tested macOS behavior; production host publication
must use that pattern and preserve the last complete output on failure. The
initial assertion did not record the failed process status, so no particular
signal is claimed. The instrumentation's first build also failed on an incorrectly
encoded generated function name; correcting the test instrumentation fixed it.
Governance initially rejected a required RFC section heading; correcting it also
restored the governance unit test. These failed runs remain part of the evidence
rather than being counted as passes.

## Checkpoint and remaining work

Seed reproduction, governance, 10 unit tests, 75 integration tests, full
session-analysis comparisons, the required performance/reduction/parallelism/
comparison/agent commands and resource baseline all pass. The seed grows from
4,808,588 to 4,869,488 generated C bytes (+60,900, 1.27%). Native analysis/resource
baseline rows remain unchanged. The final current-seed cost campaign records
source, generated probe, executable and external compiler identities. The
permanent verification entry point is `scripts/verify-native-cache.sh`; the full
verification script includes it. Public native context capture, request handling,
external execution accounting, publication/recovery, integrated latency/locality
and the complete M1 release gate remain required.


Raw logs, failed runs, exact work counters, timing samples and command/source
identities are preserved in the [verification archive](archive/2026-09-08-m1-native-query-verification.json.gz).
The [research archive](archive/2026-09-08-m1-native-query-research.json.gz) preserves
private capture scripts, input manifests, native/preprocessor observations and
reproduction sources. Both have byte/hash entries in the archive manifest.
The full M1 release command has not been run for closure at this checkpoint.
