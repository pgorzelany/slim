# M2 host and worker contract freeze — 2026-09-08

Base: `d7c05dd`, branch `codex/slim-next`. RFC-0155 and RFC-0156 are proposed;
this is a prerequisite specification, not an implemented host/worker migration.
M2 source cutovers and M3 remain incomplete.

## Inspected boundary

Current generated entry constructs argv views in a region-backed vector.
File input allocates path scratch, uses buffered stdio and grows output. TCP
input grows a temporary response before growing/copying into output. Printing
uses buffered stdio. These paths are in `runtime/slim_rt.c` and
`selfhost/codegen.slim`; no no-allocation claim follows from the present code.

The proposed hosted entry retains argv through shared host-lifetime slices and
charges one descriptor allocation to explicit startup authority. Proposed reads
use pre-reserved output with bounded native scratch. A wrapper's successful
reserve can increase capacity even if the subsequent host call fails; only the
host call itself promises unchanged output metadata on failure. Applications
needing full wrapper failure atomicity must reserve a separate result owner.

The task proposal moves one pre-reserved byte vector into each task and requires
that exact owner back on every normal return. Workers cannot allocate, release,
freeze or replace it. This preserves owned results and post-join resizing while
avoiding unimplemented child-pool quota/adoption machinery. It requires new
checked capture/cleanup evidence; native quiescence alone cannot prove source
soundness. Existing automatic CPU parallelism is independent.

## Frozen host boundary cases

These are test obligations fixed before candidate implementation, not reported
passes. Preserve the complete named state, including capacity and allocation
identity where promised. Use exact bytes rather than only lengths or status.

| Family | Required cases and independent expected result |
| --- | --- |
| Entry | No resources; allocator only; allocator+args; empty argc; argv[0]; empty/interior empty arguments; non-ASCII bytes; descriptor allocation failure; root admission failure; checked count/length/multiplication boundaries. One descriptor attempt for nonempty argv, zero payload copies, no pool for resource-free entry. |
| File path | Lengths 0, 1, 4,095 and 4,096; NUL at first/interior/last byte; absent path; directory; supported/unsupported target. Over-bound/NUL reject before opening. |
| File bounds | Limits -1, 0, 1, 63, 64 and 65; prefix lengths 0 and 3; spare capacity below/at/above limit; payload lengths 0, max(0,limit-1), limit and limit+1 for valid limits. Exact-bound EOF succeeds, extra byte fails, prefix remains unchanged. |
| File transaction | Open/read/close failure; short read; interrupted read then retry; early EOF; file growth/shrink during read. Complete observed EOF and close precede length publication; every opened descriptor closes on its defined path. No filesystem snapshot claim. |
| TCP inputs | Existing numeric IPv4/IPv6, address/NUL/port/timeout failures; insufficient capacity before transmission; response limit 0 and exact/extra byte; both request and response contain zero bytes. No DNS or hidden buffer growth. |
| TCP faults | Connect/send/shutdown/receive/close failures; timeout; interrupted/short send and receive; peer close; SIGPIPE containment; unsupported target. Output metadata and prefix unchanged; already transmitted bytes are not undone. |
| Output | I64 minimum/maximum, -1/0/1; empty bytes; binary bytes; println order; short/interrupted/zero-progress/failed write. Exact byte oracle and fixed scratch accounting. |
| Clock arithmetic | Seconds below/at/above I64/1000; fraction below/at/above remaining representable milliseconds; failed/backward read; saturated deadline. Never overflow the final fraction addition. |
| Wrapper admission | No-growth success; growth; failure at each explicit reserve; host failure after successful reserve; separately owned transactional result. Distinguish unchanged initialized bytes from changed wrapper capacity. |
| Source lifetime | Borrowed argv escape; retained-session copy; input borrowed from output; repeated output owner; missing authority/effect; invalid entry shape. Every rejection must run through the production compiler. |

Host-bound cross-products are finite products of the listed values, deduplicated
where values coincide. Fault sequences are named separately; no unbounded search
or claim of every possible host schedule is made. The implementation receipt
must state actual case counts, excluded combinations and first mismatch.

## Frozen worker cases and gates

Use two distinct buffers from the same root pool, both zero/nonzero capacity
choices, both completion orders, and ordinary/declined/forced-serial/portable
execution. Check exact output bytes, buffer identities, live/requested/rounded
accounting, immutable allocator metadata during tasks, exact joins and release
after the join. Follow each successful run with parent growth and complete
reuse. Inject each preparation allocation ordinal separately, spawn failure and
join failure. A live-worker trap must not free accessible storage.

Source negative rows include wrong/missing/duplicated returned origin, transient
cleanup before result construction, hidden transitive reserve, captured allocator,
input/output overlap, owner shared by tasks, captured non-U8 owner, escaping
reference, unsupported placement and exhausted mandatory facts. Check all return
branches and empty owners. A mere effect-list match is insufficient.

Retain exact dual_fetch requests LEFT/RIGHT, 64-byte response bounds and 2,000 ms
timeout; dual_health requests A/B, 8-byte response bounds and the same timeout.
Keep each 0.75 parallel/serial budget. Keep state_machine and signal_network
automatic parallel budgets separately. Allocation failures deliberately move
before launch; record the old/new ordinal and network-side-effect map instead of
relabeling the old sticky failure. The post-join owned-result/resizing/release
gate remains mandatory even when obsolete region-adopt C assertions migrate.

## Evidence and remaining acceptance

The lexical inventory identifies tracked SLIM source inputs and native source
hashes. Counts are text occurrences, including possible occurrences in strings;
they are not semantic call counts or a complete migration proof. Actual
compiler/library/fixture callers and their accepted spans must be enumerated
by the production checker at cutover. Preserve native timing baselines before
changing adapters, every existing failure witness, and the complete release gate.

The frozen inventory covers 406 affected tracked SLIM files: 400 main signature
occurrences, 48 file reads, 8 TCP exchanges, 11 clock reads, 103 integer prints,
107 byte prints and 145 println occurrences. These are lexical counts only.

`scripts/verify-pool-workers.py` now executes the existing native task runtime
with the independently compiled RFC-0154 pool. Its 192 bounded cases cover two
ordinary/sanitized builds, portable serial and POSIX tiers, normal/disabled/
declined modes where applicable, four empty/nonempty buffer combinations, two
requested completion orders and three preparation-fault choices. Actual POSIX
execution verifies both completion orders; inline execution preserves second-
then-first order. Two additional injected-join-failure runs retain exit 70 and
the exact existing diagnostic. Every successful normal run checks returned
identities, exact bytes, unchanged pool control/index/header metadata during
tasks, parent growth and complete release/reuse. Preparation failure starts no
task. A permanent Cargo test retains this witness.

The harness's atomics only force test completion order; they are not new source
operations. The task payload loop has no provider access. Host-call counters
observe pool reservation/free only, not internal pthread or OS resource work.
ASan/UBSan passes are distinct from a race-detector proof. This native witness
does not implement host reads, source loan/origin/cleanup checking or safe
live-worker trap teardown. Those named acceptance obligations remain pending.

Both RFCs remain proposed. Native target audits, source ownership/cleanup tests,
the complete memory package and substantial application oracles still precede
adoption. No source, runtime ABI, default behavior or budget changes here.

All eight AGENTS.md commands passed with unchanged captured sources, including
75 production integration tests and all five M2 verification tests. Formatting,
Clippy, final governance and all 18 website tests passed. The final standalone
native witness also includes empty-owner domain identity assertions. The
[compact receipt](archive/2026-09-08-m2-host-worker-contracts.json.gz) retains the
inventory, both native source checkpoints and complete gate logs. Two draft
wording corrections and this reporting paragraph follow the eight-command run;
compiler/runtime/test sources stayed unchanged. This is prerequisite verification,
not the full M2 or M3 milestone release gate.
