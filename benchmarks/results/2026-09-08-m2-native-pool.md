# M2 native pool component — 2026-09-08

RFC-0154 implements the native block component needed to evaluate RFC-0152.
Base is `95a8587` on `codex/slim-next`. Current generated programs still use
runtime ABI 1 and the unchanged 4,877,338-byte seed. The component is explicitly
compiled by its native tests and is not linked into current source programs.
M2 source integration and M3 remain incomplete.

## Verified component scope

| Obligation | Evidence domain |
| --- | --- |
| Native/oracle agreement | Six campaigns: ordinary and ASan/UBSan builds, each with plain, recursive and mixed headers. Each compares 1,015 bounded traces and 6,978 result/state lines with the independent occupied-cell oracle. |
| Native capacity and alignment | 64 KiB, 1 MiB and 256 MiB pools; both 16/64-byte headers; alignments 1/2/4/8/16; requests below/at/above every power-of-two boundary. A 1 GiB pool crosses all bitmap hierarchy boundaries. |
| Failure and identity | Invalid configuration/alignment, UINT64_MAX request/attempt boundaries, exact exhaustion, retry, stale/duplicate/foreign handles, corrupted handle/header fields, live-pool teardown rejection and startup failure. All 16 allocation positions in the named burst are injected separately, plus the no-failure control. |
| Reuse and accounting | Full minimum-block pools, alternating holes, rejected fragmented large requests, deterministic lowest-offset reuse and reverse/interleaved release restore the complete pool. |
| No host calls on valid operations | Real malloc/free wrappers assert unchanged host counters during every tested allocation/release. The self-test observes 26 hosted reservation attempts and 25 frees, including one injected reservation failure. Valid-path source inspection follows only bounded index/extent/accounting helpers. |
| Sanitizer instrumentation | The same C implementation passes with ASan/UBSan and explicit suballocation poisoning. Deliberate released-payload and one-byte-overrun writes produce source-located sanitizer errors. This supplements accounting; it does not establish universal pooled-pointer safety. |
| Permanent verification | `tests/m2_model.rs` invokes the native replay and sanitizer witnesses through `scripts/verify-pool.py`; the earlier model gates remain intact. |

These are exact comparisons within bounded named domains, not source ownership
or lifetime proofs. Raw block release does not destroy elements. The checked
cleanup driver, capability/lifetime rules, hosted adapters, worker domains and
source migration remain separate prerequisites.

The first sanitizer run passed the normal campaigns and caught the intended
bad access, then failed the verifier's source-location requirement because the
sandbox prevented the external symbolizer from starting. The authorized rerun
passed without weakening the diagnostic requirement. That failed attempt remains
identified separately. Expanded final checks add foreign-domain and burst-fault
coverage; they are not relabelled as the earlier run.

## Work and storage costs

Observed maxima in the instrumented self-test are four hierarchy lookup reads,
61 hierarchy writes, 25 availability writes, 257 within-word bit steps, 24 splits,
24 buddy tests and 24 merges per operation. The predeclared bounds remain four
lookup reads, 100 hierarchy writes, 25 availability writes, 320 bit steps and
24 split/buddy/merge steps. Instrumentation is absent from the ordinary component.

Ordinary control size is 1,496 bytes; instrumented control size is 1,552 bytes.
Index storage is 336 bytes at 64 KiB, 1,065,360 bytes at the 256 MiB default and
4,261,024 bytes at 1 GiB. The hosted reservation also charges the complete pool
capacity and 15 alignment bytes; control storage is separate. The ordinary
component object has 3,756 text bytes on this host. This is not an application
binary-size measurement.

Benchmark RSS is 2,572,288 bytes for the pool probe and 1,474,560 bytes for the
region control. Native boundary self-test RSS is 7,831,552 bytes ordinarily and
156,975,104 bytes with sanitizers. Pool reservation is address-space/resource
admission, not a claim that every reserved page is resident. Free blocks remain
in the domain until hosted teardown.

## Timing costs, not adoption evidence

Each sample performs 10,000 allocate/release cycles, writing the first and last
payload bytes. Five samples follow one warmup. The region control creates and
destroys one current child region per cycle; the candidate uses its fixed pool.
Reservation is timed separately. This compares component behavior, not complete
applications or a new source cleanup implementation. No other agent-driven build
or benchmark ran concurrently with these measurements.

| Payload bytes | Region control ms | Pool cycles ms | Pool/control |
| ---: | ---: | ---: | ---: |
| 1 | 0.341 | 2.246 | 6.59× |
| 48 | 0.314 | 2.286 | 7.28× |
| 49 | 0.318 | 2.182 | 6.86× |
| 4,096 | 0.947 | 1.508 | 1.59× |

Median pool reservation is approximately 0.010–0.011 ms in this warmed campaign.
The preimplementation 4 KiB region control took 1.352 ms, versus 0.947 ms here;
that variation limits precise attribution. Earlier samples and sandbox-denied
RSS attempts are retained. The tiny-object overhead is a real measured cost.
No performance budget is relaxed and no application speedup, source safety
improvement or agent-effectiveness gain is inferred. Full provider adoption
still requires compiler/library/application measurements under unchanged gates.

## Verification checkpoint

All eight required AGENTS.md commands passed on the captured working tree:
bootstrap, governance, Cargo, performance, reduction, parallelism, compare and
agent. Captured source stayed unchanged during that run. Formatting and Clippy
also passed. The [compact receipt](archive/2026-09-08-m2-native-pool.json.gz)
retains source identities, native streams, failure history, raw timings/resource
observations and gate logs. Final Cargo includes two additional boundary
assertions after the measurement harnesses; the native component sources are
identical. These are component/pre-commit checks, not the full M2 release gate.
Final governance and all 18 website tests also passed. The archive link and this
paragraph are reporting additions.
