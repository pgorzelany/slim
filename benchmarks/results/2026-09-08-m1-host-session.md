# M1 public compiler-session checkpoint

Status: working RFC-0143 integration; child acceptance and M1 remain incomplete.
Baseline: `7ce3fdfe9c8a8c287f0a9a941cf2cc032f1e8300`.
Seed: `f074c6f23d33ffe6389584c9dfb0567ac3323afdfacefdf0a93e6f261dd170c6`.
Measured host identity: `996e284063ae65c9b2d2fd70fb71cc69415613fe8e35d017924335daade83cdd`.
Final host identity: `8c9d351818c59a836209334e005b09ae72b1eeb7bc103d88e50db343d64f50f8`.
The final recipe adds private executable staging; seed, adapter and native flags
are unchanged. Identity, concurrent publication and public smoke checks pass
after that build-only change. The measured recipe is preserved in the evidence.

## Implemented behavior

`./slimc session` now runs the framed, host-bound retained SLIM engine. The C
adapter owns transport, bounded diagnostic capture, immutable loaded-build
identity and physical epoch reset. Source acceptance, admission, retained queries
and deterministic C production remain in SLIM. Source failures return diagnostics
and the last-good revision without returning stale C as the failed result.
Transport/allocation failures close the connection and destroy its epoch.

The source `session.update_path` entry rejects an exhausted attempt allowance
before reading files. It passes an empty owned input through the normal update
operation for the authoritative capacity result. This prevents repeated rejected
requests from accumulating captured inputs after the 64-attempt limit. Reset
continues to work. Admitted source/node/C budgets are not peak-memory promises.

The old estimate CLI driver is now a SLIM measurement fixture. Existing estimate
work, source-map fault and geometric latency gates are preserved. Verification
Rust only builds/drives production-generated fixtures; it adds no semantics.
Bootstrap builds the adapter, and the exact release inventory now includes its
`compiler/` directory under RFC-0143. Installed-release execution is still pending.

## Observed verification

| Check | Result and domain |
| --- | --- |
| Seed/bootstrap | Exact fixed point, 4,716,052 C bytes: 512 fewer than the baseline. |
| Governance/Cargo | All hard gates, 10 unit tests and 75 integration tests pass. |
| Conformance | 338 fixtures and 2,000 deterministic malformed-input mutations pass through the production compiler, including the migrated estimate fixture. |
| Required benchmarks | Performance quick, reduction quick, parallelism, compare quick and agent pass existing gates. |
| Public differential | 96 sources across cold, unchanged and declaration insertion; 30 project paths with rejection/recovery. Raw C and diagnostics equal clean production results. Ordinary and ASan/UBSan. |
| Transport | Fragmented requests, malformed/truncated frames, size rejection, NUL/unusual paths, invalid CLI arguments, EOF and broken output. |
| Admission/reset | 64 accepted attempts, repeated capacity failures, a FIFO proving no further file read, explicit cold reset and last-good identities. |
| Actual query work | Both unchanged updates execute zero of all 23 observed producer/import entries. The basic body edit checks one of two functions. Reset restarts cold. Two root epochs are physically cleaned. |
| Source allocation faults | All 1,024 cold ordinals: 159 terminal allocation failures and 865 successful results, identically ordinary and sanitized. This is not yet an update/recovery fault sweep. |
| Diagnostic storage | Exact 1 MiB boundary, all 13 allocation-growth failure sites, overflow rejection, signed integer/embedded-NUL byte order and cleanup, under ASan/UBSan. |
| Host identity | Relocated same-input build retains identity; actual adapter/runtime/options changes alter their bound identities; native target equals the compiler-reported target. Loaded old worker retains its context. Corrupt seed cannot replace the existing executable. Other native targets are untested. |
| Concurrent builds | Two simultaneous builders publish a complete working executable using unique staging files; no temporary publication files remain. |
| Historical estimates | Existing deterministic work, geometric latency and malformed-index integration gates pass after moving the driver. |

## Public frontend costs

Quiet same-host measurements use nine samples after warmup at each of six
geometric sizes. Every C response equals the clean oracle. Session timings include
capture and response transfer; one-shot timings run the native compiler directly.
External C compilation and agent completion rates are not measured here.

| Helpers | Cold session, ms | Unchanged, ms | Body edit, ms | One-shot, ms |
| ---: | ---: | ---: | ---: | ---: |
| 125 | 1.076 | 0.114 | 1.111 | 3.758 |
| 250 | 1.875 | 0.168 | 2.072 | 4.431 |
| 500 | 3.523 | 0.298 | 4.079 | 5.857 |
| 1,000 | 6.508 | 0.549 | 8.006 | 8.933 |
| 2,000 | 12.864 | 1.014 | 15.832 | 14.658 |
| 4,000 | 25.653 | 1.921 | 31.602 | 25.804 |

Public connection startup is about 14.6–15.7 ms; explicit empty reset is about
22–25 microseconds. Unchanged requests are about 9.5–13.4 times faster than cold
requests in this family. A 4,000-helper body edit remains 22.5% slower than a fresh
one-shot compile. An earlier seed/adapter measurement showed the same limitation.
No existing performance gate is relaxed, and this is not evidence of significant
agent-productivity improvement. Global work, mapping and copying remain costs to
address in the full M1 goal.

The host adds a second native executable of 642048 bytes beside the 487456-byte
one-shot engine. Its C adapter is 9464 bytes. One additional host build took
6.78 seconds on this machine. The resource wrapper could not read
`kern.clockrate` in the sandbox, so peak build memory is unknown; the executable
was built successfully. These setup/storage costs are separate from warm
request latency.

## Remaining closure

Complete public edit/identity/limit coverage, update/recovery allocation faults,
physical resource accounting and permanent public latency gates. Keep the existing
internal detailed query and corruption oracles. Verify the committed source
package through the public launcher, then complete the full M1 release gate.
Global analysis retention and native artifact caching remain required M1 work.
RFC-0143 is deliberately still marked implementation pending.

The initial diagnostic unit fixture triggered the runtime's negative-length guard
before reaching the intended host boundary; the corrected fixture injects that
invalid C value directly. Initial governance caught a missing RFC heading,
unclassified verification helper and the new package root; the accepted contract
and exact inventories now describe those additions. Failed logs are retained.

## Evidence

The [pass ledger](2026-09-08-m1-host-session-gates.tsv) records the final
commands and outcomes. [Complete logs and source hashes](archive/2026-09-08-m1-host-session-verification.json.gz),
[current raw timing samples](archive/2026-09-08-m1-host-session-latency.tsv.gz)
and [earlier timing samples](archive/2026-09-08-m1-host-session-initial-latency.tsv.gz)
are compressed without truncation. These three new archives retain 3,881,208 original
bytes in 245,647 compressed bytes. All 289 archives at that stage verify against the manifest; the publication
follow-up brings the inventory to 290.
The source commit introducing this report is the reproducible checkpoint;
older exploratory adapter snapshots are not all retained and are not final
acceptance evidence.

[Publication follow-up evidence](archive/2026-09-08-m1-host-session-publication.json.gz)
retains both build recipes, final identity checks, the concurrent-build check and
public smoke results. It changes build publication and recipe identity; it does
not change the measured compiler or transport algorithms.
