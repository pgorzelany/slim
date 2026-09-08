# Incremental compilation status

The working implementation of `slimc session` now exposes retained parsing,
checking and C generation through the framed host protocol in
[RFC-0143](../design/rfcs/0143-host-bound-compiler-sessions.md). The
[public acceptance campaign](../benchmarks/results/2026-09-08-m1-host-closure.md)
covers real build identities, edits, default limits, allocation failures, physical
cleanup and durable latency gates. RFC-0144 also retains bounded parallel analysis.
RFC-0146 now connects native object/runtime/link retention to the default host;
its integrated cost and full M1 release closure remain under verification. The former
estimate driver
now lives in a measurement fixture; its existing gates remain passing. Ordinary `check`, `emit-c` and `build` remain
one-shot operations. The retained engine uses the function-typing entry point under [RFC-0130](../design/rfcs/0130-retained-function-typing.md),
which reuses successful function inference through checked revision maps and
complete interface dependency invalidation. Internal snapshots also retain declaration
parsing, function memory plans, range queries and C function fragments as described
below. Manifest/import validation, declaration/layout checks, termination validation
and complete C assembly remain current-revision work. Parallel analysis retains
complete bounded results through RFC-0144 when their checked dependencies agree.
Termination and emission consume the current retained range view. The former
Rust `IncrementalSession` API is not part of
the production compiler.

The retained entry point calls the same isolated SLIM function checker on misses.
[RFC-0141](../design/rfcs/0141-structured-checking-continuations.md) runs every
expression family through explicit continuations over shared canonical control
and lexical-scope descriptions. The optional flow graph consumes those same
descriptions; the normal checker remains the sole semantic authority. This
removes recursive expression inference while preserving checked facts, diagnostic
order and loan transitions. The separate name-resolution prepass retains its
recorded sanitizer depth limit.

[RFC-0131](../design/rfcs/0131-compact-retained-storage.md) stores one typed
revision/declaration owner per row and compact owner-scoped type/link words.
It reconstructs nominal type, binding and node references against the original
index, then translates them explicitly. Wrong owners and invalid storage cause
misses; all imported nodes are validated before the first write.
Unchanged bodies can survive insertion/reordering of other declarations; changed
callee signatures and transitive data interfaces invalidate their consumers.
Only full successful checks can produce eligible history. Rejected candidates and
stale revisions cannot replace it. The caller selects a token limit in
1..1,000,000; invalid limits or larger inputs execute ordinary checking and report
a capacity miss. This does not bound source
bytes or peak process memory. This internal typing layer does not complete M1's persistent service lifecycle,
global analysis retention or native backend caching.

[RFC-0132](../design/rfcs/0132-retained-project-preparation.md) connects retained
typing to shared project preparation. The attempt returns the complete prepared
project, eligible typing history and actual inference work. Both preparation modes
run current manifest/module/import/export validation and retain current diagnostic
origins. An unchanged flattened body cannot bypass a removed import or export.
This entry point originally repeated module parsing; RFC-0135 now retains eligible
declaration parses within it. Flattening and global checks still run; RFC-0140 adds retained function emission. The host adapter now calls this retained entry point.

Retained typing and source snapshot maps now use complete declaration source keys,
excluding trailing separator whitespace. A synthetic closing node is not a source
extent: its position can end at a callee before nested arguments. Literal, string,
operator and member changes are covered by permanent regressions, including
rejected updates that previously could reuse incomplete source keys.

[RFC-0133](../design/rfcs/0133-transactional-project-snapshots.md) adds internal
transactional successful snapshots and bounded epoch accounting. Unchanged complete
inputs reuse a successful snapshot, including its checked state and C. Failed
updates preserve the last-good snapshot. RFC-0143 supplies host-bound transport and actual
compiler/runtime/target/options identity binding. Explicit reset physically frees
the epoch; source admission budgets do not bound peak process memory.

[RFC-0135](../design/rfcs/0135-retained-declaration-parsing.md) retains declaration
parsing for original modules and flattened source. Exact source keys include the
lexical and lookahead dependencies required by the sole parser; imported canonical
nodes have checked owners and current positions. Native work observation separates
lexing, declaration grammar executions and parsed-node imports.

[RFC-0137](../design/rfcs/0137-retained-function-memory-plans.md) retains the sole
planner's results after complete body/interface dependency validation. Saved rows
occupy snapshot-wide pools with checked contiguous slices and typed declaration
owners. Import validates and relocates both local token ordinals and byte ends,
including the legacy 64-value liveness boundary. Missing or damaged metadata runs
the ordinary planner. Independent optional history bounds preserve complete current
plans on a capacity miss. Native observation counts actual plan construction and
import; it does not infer reuse from source equality.

[RFC-0138](../design/rfcs/0138-retained-range-queries.md) retains the sole range
producer's function results within the ordinary five passes and across successful
revisions. Keys include complete checked declaration dependencies, both input facts
for each parameter and the incoming global refinement count. Sparse saved facts
and recurrence records validate and relocate against current canonical nodes;
invalid optional history causes a miss. Three reusable scratch vectors preserve
all five passes. RFC-0142 below replaces eligible global input scans
with complete retained transfers. The resulting complete view feeds
ordinary termination validation and the sole C emitter. Parallel analysis still
runs; RFC-0140 compares its current sites before reusing function fragments. Native observation separates range production,
imports, passes, scans and scratch initialization/reset work. Costs, limits and
complete differential evidence are in the
[progress report](../benchmarks/results/2026-09-05-slim-next-progress.md).

[RFC-0139](../design/rfcs/0139-stable-declaration-local-c-identifiers.md) gives
C locals and temporaries parameter-list-relative node ordinals. Private parallel
helpers also include their escaped qualified owner function name. Unrelated
preceding edits and module relocation therefore preserve unchanged function
fragments when all checked lowering inputs remain equal.

[RFC-0140](../design/rfcs/0140-retained-c-function-fragments.md) adds actual
retained function prototypes, bodies and private
parallel wrappers to the internal session. It forwards the checker’s existing
revision-bound declaration transition and compares complete checked facts/links,
consumed memory-plan fields, all local range fields, counted records and ordered
parallel sites. An unchanged caller regenerates when its callee changes consumed
recurrence work or when graph/selection limits remove an executable site.

The sole emitter records byte intervals while producing each fragment. Optional
metadata shares the snapshot’s existing immutable C artifact; it does not own a
second complete C payload. Revision/owner checks, bounded section framing and
checksums precede import. Invalid history uses normal current emission. The
checksum detects tested accidental corruption of compiler-owned state; it is not
an authentication boundary for arbitrary imported C or semantic data. Limits are
one million metadata records, 64 reported sites/two tasks per site and 67,108,864
C bytes. The ordinary and retained emitters share a constant-work counted-record
cursor; 64 counted report rows do not cap proof storage.

No-change complete inputs keep whole-artifact reuse. New artifacts charge their
complete size, including imported bytes; failed candidates retain the last-good
snapshot. Header/layout emission and C copying remain real work. Native session
observation schema 6 retains all earlier counters and adds prototype/body/wrapper
producer entries, fragment imports, copied bytes and counted-record lookups.
RFC-0142 advances this to schema 7 by appending actual input-transfer
entries; the old scan counter continues to count actual ordinary scan calls.
RFC-0143/0144/0146 expose these operations and retained parallel/native results
through the public session. Full M1 release closure remains pending.

[RFC-0142](../design/rfcs/0142-retained-parameter-input-queries.md) implements
parameter-input retention. It validates complete ordered
caller contributions, all six current argument-fact fields and recurrence
invariants, preserving both existing input modes and all four transfer rounds.
Eligible queries reuse complete results; missing, damaged or bounded-away history
uses the ordinary scans. Complete empty incoming adjacency leaves the exact
cleared defaults without storing an empty query. The full true-invariant vector
is initialized only when fallback needs it. Verified domains, allocation-fault
campaigns and contrary measurements are recorded in the
[checkpoint report](../benchmarks/results/2026-09-07-m1-input-queries.md).

## Public framed session

Start `./slimc session` with no arguments. Bootstrap builds its host adapter from
verified seed bytes and the ordinary runtime. The initial `H` frame identifies
the loaded compiler/runtime/target/options; requests cannot supply identities or
cached facts. Send `U` with a project-manifest path, `R` to physically release the
owning epoch and restart cold, or `Q` to close. EOF between frames also closes.
Every frame has a tag, a four-byte big-endian payload length and that exact payload.
The complete frontend binary layout and bounded errors are specified in RFC-0143.
RFC-0146 adds `B` for a native build of an exact checked epoch/revision and `N` for
its result. Native tool capture survives `R`; native query records do not.

Successful `S` responses carry actual work aggregates and the current deterministic
C artifact. Failed source updates carry diagnostics and the last-good revision,
with no C payload. Accept only complete responses. `U` performs source checking and
C generation; `B` requests native compilation separately. Source capture and response transmission
remain real work even when all compiler queries are reused.

`python3 -B scripts/verify-session-host.py` drives the public launcher and compares
96 source fixtures and 30 project paths with clean production results. It also
checks recovery, physical reset, fragmented/malformed requests, unusual paths,
broken output, the 64-attempt limit and allocation failures.
`sh scripts/verify-session-host.sh` repeats these checks under ASan/UBSan and
observes actual producer entries and root cleanup. The basic unchanged update
executes zero of all 23 observed producers/imports; a body edit checks one of two
functions. These are fixture-specific observations, not an agent success rate.
`python3 -B scripts/measure-session-host.py` records quiet raw frontend timings
separately from external native compilation.

## Historical invalidation-estimate driver

`tests/fixtures/session_estimate.slim` calls the historical snapshot-comparison
operations in `selfhost/session.slim`. It loads the initial and updated projects
and builds source snapshots. `selfhost/query.slim` compares declarations, identifies
changed bodies and interfaces, and propagates dependency invalidation. The
identity is `(module, declaration kind, declared name)`.

[RFC-0125](../design/rfcs/0125-source-identity-resolution.md) and
[RFC-0126](../design/rfcs/0126-exact-revision-maps.md) add typed revision/file/node
handles, checked spans, and exact-content maps to new source positions. Aligned
keys compare directly; reordered lookup builds one lazy index and retains it in
that source state. Current source links reject duplicate names. This source-index
foundation is implemented. Completing the public lifecycle remains M1 work. [Measured work and costs](../benchmarks/results/2026-09-05-slim-next-progress.md)
are recorded separately from the historical estimates.

[RFC-0127](../design/rfcs/0127-isolated-function-checking.md) makes the existing
function body checker independently callable with fresh binding/loan/ownership
scratch and materialized source facts. Forward, reverse, and repeated checking
produce identical facts and links over the accepted regression corpus. Physical
scratch allocations still live in the caller's region. This is a prerequisite
for declaration queries; it does not yet retain checked results across revisions.

[RFC-0128](../design/rfcs/0128-checked-layout-emission-order.md) retains inline
layout completion order from the normal checker and passes it to C generation.
Aggregate definitions therefore follow their checked dependencies across source
and module order. This result belongs to its current checked source; it has no
cross-revision cache or independent validation authority.

[RFC-0129](../design/rfcs/0129-bounded-function-flow-view.md) supplies an optional
bounded per-function flow view. It uses typed source/block identities, explicit
work-stack traversal, structural branch joins, lexical boundaries, ordered call
arguments, and terminal recurrence edges. Complete describes structural normal
topology; call outcomes and feasible-path reachability are not proved. Bounded
views cannot resolve blocks as complete. The existing checker still supplies all
semantic acceptance, and ownership orchestration has not yet migrated to this
view. Default checking does not build an unused graph.

The four integers printed by the measurement fixture retain historical
field names `parsed lowered checked generated`. They are **invalidation
estimates**, not counters of operations performed. In `query.measure_update`,
`parsed` counts classified changes, `lowered` copies that count, `checked`
counts invalidation flags, and `generated` copies the invalidated count.
No retained typed bodies or generated fragments are reused by this fixture.
A zero estimate on an unchanged project does not mean zero frontend work.

## Observed execution

`cargo run --release --bin slim-bench -- work --quick` now observes actual
native compiler entry points and loop headers. It builds an instrumented copy
of the reproduced portable seed and, for legacy estimates, the SLIM measurement
fixture generated by that compiler. It compares each operation with its ordinary
executable and repeats observations to require identical counters and output.
It never installs that copy or adds counters to normal builds. Add `--sanitize`
to run the observed compiler under ASan/UBSan. Without `--quick`, the declaration
series extends through 8,000 declarations.

The maintained two-module fixture gives these exact per-process counts:

| Operation | Program parses | Checker calls | C-generation calls | Artifact-cache hits |
| --- | ---: | ---: | ---: | ---: |
| Clean C emission | 3 | 1 | 1 | 0 |
| Cold artifact-cache miss | 3 | 1 | 1 | 0 |
| Unchanged artifact-cache hit | 0 | 0 | 0 | 1 |
| Changed body with an old artifact | 3 | 1 | 1 | 0 |
| Unchanged snapshot comparison | 4 | 0 | 0 | 0 |

The third parse on a clean build is the existing flattened-source reparse after
the two module parses. A cache hit still reads the key manifest, both source
files, and the artifact frame, lexes the manifest, and validates the checksum.
The snapshot comparison parses each module in both input projects even when
its four printed invalidation estimates are zero. These counts demonstrate
whole-artifact reuse and snapshot-model work; they do not demonstrate retained
incremental checking or emission.

The session recovery variant performs clean checks of its initial, rejected, and
recovered inputs and compares snapshots after recovery. It does not establish
transactional retention of a previous checked compilation.

The observed campaign compares clean and cached C through body, signature,
layout, effect, borrow-mode, insertion, deletion, rename, reordering, and
relocation cases. It also checks corruption and failed-then-recovered inputs.
Only successful native miss frames are published by the harness; a rejected
input leaves the last good artifact unchanged. The recovered original source
can hit that artifact while session recovery still performs clean checks.
See [the work-measurement contract](PERFORMANCE.md#observed-compiler-work) and
[RFC-0121](../design/rfcs/0121-observed-compiler-work.md) for the observation
boundary, counter caps, and failure rules.

## Historical measurements

`cargo run --release --bin slim-bench -- incremental` exercises this dependency
invalidation model. It remains useful as a regression test for the selected
change set and its scaling, but cannot establish incremental compiler latency
or avoided parse/check/code-generation work.

The [2026-07-21 measurements](../benchmarks/results/2026-07-21-incremental.tsv)
are retained as historical evidence. Their operation-labelled columns must
not be cited as demonstrated reuse by today's production SLIM compiler.

## Acceptance boundary for SLIM Next

M1 must retain checked results attached to stable canonical SLIM identities,
invalidate their actual dependencies, and compare every successful update with
a clean compilation. It must instrument work where parsing, checking, and
emission actually occur. Tests must include body, signature, layout, effect,
borrow-mode, deletion, insertion, relocation, and failed-then-recovered edits.

Source indexing, metadata updates, output assembly, external C compilation,
and queueing must remain visible as separate costs. A fast invalidation model
is useful infrastructure; only a measured working compiler can establish a
fast compilation cycle. See [the SLIM Next implementation report](../benchmarks/results/2026-09-05-slim-next-progress.md).

## Native artifact query foundation

RFC-0145 adds a private production SLIM query for native object, runtime and link
artifacts. Exact keys and cloned storage pass ordinary, sanitizer, corruption,
capacity and real native-byte round-trip tests. RFC-0146 now connects this library
to the public session through a checked-source `B` request and a complete framed
`N` artifact response. The reference provider captures Apple clang 21 on Darwin/arm64,
its dependencies, SDK inputs and the paired runtime into immutable connection-owned
storage. Unsupported contexts return an explicit unavailable result. Clients publish
complete executable bytes to a fresh inode and rename it atomically. Reset clears
retained query history while preserving the captured tools; a new connection refreshes
them. Ordinary `build` remains a one-shot operation.

The complete default native campaign passes, including clean/edit/revert builds,
independently observed backend starts, all 20 native applications, input isolation,
allocation and I/O failures, corruption, actual capacity, concurrency and physical
cleanup. [Initial public costs](../benchmarks/results/2026-09-08-m1-native-session.md)
show faster edited builds and an expensive setup cost; startup improvements and
the full release gate remain under verification. The
[query report](../benchmarks/results/2026-09-08-m1-native-query.md) records measured
copy/validation costs and the remaining checks.
