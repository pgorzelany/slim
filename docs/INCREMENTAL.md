# Incremental compilation status

The public compiler commands do **not yet provide retained incremental parsing,
checking or C generation**. The production compiler now has an internal retained
function-typing entry point under [RFC-0130](../design/rfcs/0130-retained-function-typing.md),
which reuses successful function inference through checked revision maps and
complete interface dependency invalidation. Internal snapshots also retain declaration
parsing, function memory plans and range queries as described below. Manifest/import
validation, declaration/layout checks, termination validation, global parameter
scans, parallel analysis and changed-source C generation remain current-revision
work. Termination and emission consume the current retained range view. The public `session` command
still reports estimates. The former Rust `IncrementalSession` API is not part of
the production compiler.

The retained entry point calls the same isolated SLIM function checker on misses.
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
flow-based ownership orchestration, analysis retention or cached emission.

[RFC-0132](../design/rfcs/0132-retained-project-preparation.md) connects retained
typing to shared project preparation. The attempt returns the complete prepared
project, eligible typing history and actual inference work. Both preparation modes
run current manifest/module/import/export validation and retain current diagnostic
origins. An unchanged flattened body cannot bypass a removed import or export.
This entry point originally repeated module parsing; RFC-0135 now retains eligible
declaration parses within it. Flattening, global checks and changed-source C
generation still run. The public session protocol remains estimate-only.

Retained typing and source snapshot maps now use complete declaration source keys,
excluding trailing separator whitespace. A synthetic closing node is not a source
extent: its position can end at a callee before nested arguments. Literal, string,
operator and member changes are covered by permanent regressions, including
rejected updates that previously could reuse incomplete source keys.

[RFC-0133](../design/rfcs/0133-transactional-project-snapshots.md) adds internal
transactional successful snapshots and bounded epoch accounting. Unchanged complete
inputs reuse a successful snapshot, including its checked state and C. Failed
updates preserve the last-good snapshot. Public host-bound transport and actual
compiler/runtime/target/options identity binding are still required M1 work.

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
all five passes and eight global input scans. The resulting complete view feeds
ordinary termination validation and the sole C emitter. Parallel analysis and
changed-source C emission still run. Native observation separates range production,
imports, passes, scans and scratch initialization/reset work. Costs, limits and
complete differential evidence are in the
[progress report](../benchmarks/results/2026-09-05-slim-next-progress.md).

## What the current session command measures

`selfhost/session.slim` loads the initial and updated projects and builds
source snapshots. `selfhost/query.slim` compares declarations, identifies
changed bodies and interfaces, and propagates dependency invalidation. The
identity is `(module, declaration kind, declared name)`.

[RFC-0125](../design/rfcs/0125-source-identity-resolution.md) and
[RFC-0126](../design/rfcs/0126-exact-revision-maps.md) add typed revision/file/node
handles, checked spans, and exact-content maps to new source positions. Aligned
keys compare directly; reordered lookup builds one lazy index and retains it in
that source state. Current source links reject duplicate names. This source-index
foundation is implemented; integrating retained checking and emitted C into the
public session remains M1 work. [Measured work and costs](../benchmarks/results/2026-09-05-slim-next-progress.md)
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

The four integers printed by the internal `session` command retain historical
field names `parsed lowered checked generated`. They are **invalidation
estimates**, not counters of operations performed. In `query.measure_update`,
`parsed` counts classified changes, `lowered` copies that count, `checked`
counts invalidation flags, and `generated` copies the invalidated count.
No retained typed bodies or generated fragments are reused by this command.
A zero estimate on an unchanged project does not mean zero frontend work.

## Observed execution

`cargo run --release --bin slim-bench -- work --quick` now observes actual
native compiler entry points and loop headers. It builds an instrumented copy
of the reproduced portable seed, compares each operation with the ordinary
compiler, and repeats the observation to require identical counters and output.
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
