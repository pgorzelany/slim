# RFC-0144: Retained bounded parallel analysis

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-08
DecisionDate: 2026-09-08
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Retain the existing bounded `parallel.analyze` result as a revision-owned query
in the production SLIM session. Validate its actual ordered dependency domain,
relocate its complete result, and run the sole existing producer on a miss.
Preserve every blocker, graph uncertainty, site, scheduling counter and native
execution decision. Native object/link caching remains separate required M1 work.

## Motivation

RFC-0143 exposes retained checking and C fragments, but every edited artifact
still runs global parallel analysis. The current operation considers the first
64 functions, at most 4,096 call edges and 64 reported sites. It performs an
ordered bounded graph computation and two globally ordered scheduling passes.
Its dependency is that complete bounded domain, not every body in the project.
Body edits outside it may affect the result only through consumed checked or
range/call-work facts. The total function count is an independent report field.

## Guide-level explanation

A session update may reuse the previously checked parallel result when all
consumed inputs still agree. Insertions, deletions and moved source positions
receive current identities. Changed input, invalid history or missing evidence
runs normal analysis. Bounded results retain their unknown classifications;
reuse never upgrades them to safe or executable.

This query does not expose a new command or change source language semantics.
It does not assert that every edit becomes faster. The first implementation
retains the complete bounded graph/schedule operation. It may recompute that
operation when a participating function changes; it must not fabricate a
whole-project body dependency to avoid identifying the actual 64-function scope.

## Reference-level specification

### Input and result ownership

Keep one optional result alongside the C-fragment history in the successful
snapshot, so fragment-processing loops do not copy unrelated query metadata.
Its owner is the
successful checked revision. Store the complete `parallel.View`, including all
function blockers, status and reason fields, exactness, all reported sites and
all six scheduling scalars. Retain at most 64 facts and 64 sites. Do not create
another parser, checker, graph semantics or persisted executable representation.

The previous and current prepared projects must have successful checked source,
valid declaration indices, current typed/range vectors and a validated revision
transition. Use canonical function order, not symbol spelling or storage order
invented by the cache. Derive the selected function slots and total function count from the checked
canonical declaration index. Memory plans are independent optional derived data
and must never supply this query’s function identities.

### Complete dependency check

Require the first `min(functions, 64)` function owners to map bijectively in the
same order. For each member validate the complete checked-function dependency
contract from RFC-0130/0134, including body bytes, interfaces, resolved links and
ownership/effect inputs. Require equal range facts at every corresponding node.
Caller edits outside the selected prefix may alter these facts and must miss.

The automatic scheduling pass consumes recurrence work only for selected safe
pairs. Every safe user call resolves inside the same first-64 function set;
a call outside it is unknown and cannot select a pair. Compare the presence and
complete position/bound/step profile for every selected function, including
negative lookup, using the canonical ordered recurrence profiles. Together with
unchanged checked call expressions this preserves all consumed work values.
Do not key unrelated profiles outside the selected function set. The existing
literal/proof domain bounds intermediate work arithmetic well inside I64; unused
work computations have no observable failure or effect to preserve. These key
comparisons remain visible validation work rather than avoided producer work.
No result cache may trust caller-supplied fingerprints in place of checked inputs.

Together these inputs preserve local hazards, typed-expression availability,
callee membership and graph order, the shared edge-prefix capacity, graph fixed
point order, all parallel blocker sets, explicit fork allocation capabilities,
argument binding dependence, atomic captures, leading-chain placement and the
shared lexical selection/report prefix. The analyzer's existing constants remain
part of the compiled context. No analysis or performance limit is increased.

### Import and independent report fields

Validate all history frames before importing any result. Each fact must belong
to its selected canonical function, with valid name/body extents and edge spans.
Every site and selected-until node must resolve in a selected function. Check
fixed vector bounds, scalar bounds, revision identity and a checksum over every
stored result field. The checksum detects accidental damaged optional history;
it is not cryptographic authority or proof of a semantic result. Only the normal
producer creates eligible history from a successful checked source.

Translate fact and site nodes through checked declaration owners and local
ordinals. Retain lexical fact/site order and every scalar decision. Recompute the
reported total function count from the current checked program, and compute the
existing exactness formula from that count and the retained edge completeness.
Thus adding or removing a function after the selected prefix can change the
function-limit report without invalidating an otherwise identical bounded graph.

Whole-artifact rebasing under identical checked source/positions also rebinds
valid parallel history. Invalid optional history stays a miss on the next use.
A source rejection or allocation failure cannot publish a candidate cache.

## Compiler and runtime design

Implement the query and validation in `selfhost/`, using the existing prepared
project, checked revision maps and `parallel.View`. The C host remains framing and
lifetime glue. Rust/Python/C observation code may drive and instrument production
SLIM; it may not supply parallel facts or fallback semantics. The ordinary
one-shot analyzer remains the behavioral oracle. No runtime ABI or dependency
changes are introduced.

## Compatibility and migration

Keep diagnostics, analysis reports, generated C and native execution identical
to a clean compile of the current source. Preserve the first-64-function scope,
4,096-edge bound, all blocker bits, 64-site report limit, `posix-v1` threshold and
selected/executable/executed distinctions. Expanding any domain is outside this
RFC. Preserve all existing safety and native performance gates.

## Diagnostics and failure cases

Missing, stale, malformed or damaged optional history causes normal analysis;
it does not produce a source diagnostic or replace unknown evidence with a
negative quality score. Allocation failures retain the session's terminal
failure behavior. Invalid current source uses normal diagnostics and leaves
the last-good snapshot unchanged. A bounded result remains bounded after import.

## Performance and complexity

Select the first 64 function slots and count all functions in one linear scan
of canonical declaration metadata for each revision. These scans read kinds and
slots, not unrelated function bodies, and are reported separately. The remaining
dependency check is linear in selected source nodes with bounded profile lookup. Metadata validation, copying and relocation are
bounded by 64 facts and sites. Total-function metadata is read independently.
Record actual query attempts, producer executions, imported facts/sites and
validation work separately. Unchanged whole snapshots continue to perform zero
query producers/imports. Preserve geometric cold/unchanged/body latency gates
and add cases editing inside and outside the selected prefix. Measure compiler
and seed growth, allocations, copies and same-host latency before and after.
No latency benefit or productivity rating is assumed from a reuse label.

## Alternatives and drawbacks

Recomputing on every edit leaves a required global query unretained. Keying all
program bodies invents dependencies the bounded operation does not consume.
Per-function local summaries and component-level graph reuse could reduce misses
inside the selected prefix, but require separate treatment of shared edge and
scheduling prefixes. They are possible refinements, not a reason to duplicate
or alter graph semantics here. This result cache adds bounded metadata and
validation work even when a participating function changes.

## Test and acceptance plan

Compare every field of the complete clean and retained views and raw emitted C.
Cover body/interface/effect/ownership/range/caller-input changes, recurrence work,
source relocation, insertion/deletion/renaming/reordering, selected-prefix entry
and exit, 63/64/65 functions, edge saturation and 63/64/65 reported sites.
Exercise independent blockers, cycles and missing callees, explicit and automatic
sites, executable work thresholds, failed edits and recovery, stale owners,
malformed frames and individually damaged fields. Source rejection must retain
the last-good result. Observe producer/import calls independently of report data.

Run ordinary and ASan/UBSan differentials, bounded allocation faults, geometric
work/latency controls, complete native analysis baselines and every required
checkpoint command. Full M1 integrated release closure remains required after
native artifact caching is implemented. Do not close M1 from this child alone.

## Ratings and evidence

Analysis +2 makes complete bounded global results reusable with explicit checked
dependencies and current identities; score 15. Other dimensions remain zero
until measurements justify a claim. Baseline is checkpoint `bc32e47`; its public
large-body edit remains slower than a fresh compilation.

## Decision

Accepted under the maintainer's RFC-0112 child-decision delegation and active
M1 implementation goal. This is a compiler architecture decision within the
approved milestone. No language surface, runtime ABI, safety gate, budget or
native execution domain is changed.

## Implementation

Implemented and verified by the production SLIM compiler at seed
`9959671c67bdce89f1826230aa90827a722319dbf5fb787bb7ad2a23180df963`.
The [checkpoint report](../../benchmarks/results/2026-09-08-m1-parallel-query.md)
records complete result/corruption/fault oracles, actual work, same-host costs,
all required checkpoint checks and preserved native analysis/resource baselines.
Global-query completion does not waive native object/link caching or full M1
verification and performance obligations.

## Removal and supersession

Preserve complete current-view equivalence, explicit boundedness, positive safety
facts, all blocker sets, source-relative identities, conservative cache misses,
transactional publication and durable observation/performance gates. Replace this
query only with a measured implementation preserving those properties.
