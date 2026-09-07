# RFC-0138: Retained range queries

Status: accepted
Implementation: pending
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-07
DecisionDate: 2026-09-07
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

Retain the existing range analyzer's function results with complete input facts
and refinement-budget dependencies. Reuse equivalent queries within the existing
five passes and across successful revisions. Supply the resulting current view
to termination checking and C emission without a second analysis of that revision.

## Motivation

RFC-0137 retains memory plans, but changed projects still repeat global range
analysis. A retained range view is needed for correct body-derived invalidation,
parallel planning, resource facts and subsequent C-fragment reuse. Source-only
invalidation is insufficient: callers influence callee parameter ranges, and a
shared refinement budget makes lexical predecessors relevant.

## Guide-level explanation

A body edit recomputes the changed function's range result. If that edit changes
argument facts supplied to another function, that function is also recomputed in
the affected passes. Functions whose complete inputs remain equal can reuse their
results. Moving a function reconstructs its current node positions; changing the
available refinement budget causes a miss. Missing optional history runs the same
ordinary analysis. No cached range fact can bypass the normal type/effect checker.

This completes neither parallel-graph retention nor flow ownership, public session
transport, C fragments, external artifacts or the full M1 milestone.

## Reference-level specification

### Sole producer and complete inputs

The sole function producer remains `ranges.analyze_function`. Its reachable local
call graph at 6c39e40 contains 66 range functions. The producer receives source,
canonical tokens, checked typing facts, its function node, current output vectors,
parameter facts and entry facts. It has no pass-number input or hidden global state.

Use these query dependencies:

1. The complete checked declaration body, resolved references and transitive
   consumed interfaces, established by RFC-0130's exact source/shape/prior-link
   comparison and reverse interface invalidation. This includes local typing,
   nominal forms, declared parameter modes, and callee effect ceilings.
2. Every field of both input parameter and entry facts at each parameter binding,
   in canonical parameter order, with owner-local binding identities.
3. The exact incoming length of the global refinement vector, in 0..64.

The complete read argument is specific to the present producer. `expression_atom`
reads local checked types; binding lookups resolve parameters and lexical locals
of this function. Arithmetic/condition/recurrence fact reads use its local nodes.
`effects.call_requires` reads declared effect ceilings, not inferred events.
User-call argument traversal reads declared parameters, but no inferred callee
return range is consumed. `scan_parameter_calls` writes only parameter binding
positions; all other positions of its freshly initialized input vectors retain
the canonical unknown fact. Therefore the parameter rows determine all externally
supplied facts read by a function, including unknown entry facts for lexical locals.
A future additional read must extend this contract before reuse is permitted.

The range producer initializes each function's refinement parent to -1. A new
refinement can point only to -1 or a refinement created since this function began;
when the cap is reached, pushing returns the current parent. The counted-recurrence
reanalysis also starts with parent -1. Thus previous functions' refinement contents
are unreachable, but their count determines available capacity. Preserve the exact
count as a key; do not pretend the budget is independently 64 per function.

### Passes, propagation and repeated queries

Preserve `ranges.analyze`'s current five passes (indices 0 through 4), global source
order and all ordinary parameter-invariant and call-argument scans. Each next pass
uses the actual previous pass's reconstructed fact vector. Do not replace those
scans with an assumed call graph, converge early, or change precision limits.

A query does not depend on pass number. Within a candidate, an identical key can
reuse a result produced in an earlier pass. Save at most five distinct keys per
function: there are exactly five invocations in the fixed pass schedule. Use a
direct per-declaration head map and bounded lists, never an all-function search.
Search current-candidate results first, then matching prior-owner history. Import
an old result into candidate-owned storage once; later equal queries reuse it.
This avoids five unconditional saved copies for functions with unchanged inputs.

Caller-body changes reach consumers through the recomputed global input-fact
vectors, even when the callee body and declared interface are unchanged. Lexical
insertion/reordering can change the refinement-count key. Preserve those misses
and all later budget-saturated behavior. No inference is made from absent evidence.

### Saved results and position domains

Store compiler-owned, snapshot-wide pools under exact `identity.DeclarationId`
owners. Saved function entries name complete slices of input keys, output facts,
refinements, structural recurrence records and counted recurrence records.

- Save non-default output facts in strictly increasing owner-local node order.
  A default fact is exactly `unknown(false, false)`, including zero numeric fields.
  Imported functions start with the same freshly initialized output vector, so
  omitted default rows preserve every field. Never discard an analyzed or total
  fact merely because neither interval bound is known.
- A compact fact may use four explicit flag bits and two I64 values. Recover all
  six ordinary `ranges.Fact` fields exactly. Validate flags and the existing
  -1,000,000,000..1,000,000,000 proof domain before import; do not widen it.
- Refinement declarations are owner-local token ordinals. Parents are -1 or
  earlier function-local refinement ordinals; relocate through the current prefix.
- `Recurrence.item` is the owning function node. Its `position` is a parameter
  ordinal, while `bound` and `step` are numeric proof values. Do not relocate them.
- `Counted.item`, `controller`, `base` and `body` are canonical node positions.
  In particular, `base` is a branch-body node, not an initial accumulator value.
  `start`, `bound`, `step` and `iterations` are numeric values; preserve the
  current positive unit-step and at-most-16 reported-iteration contract.

Validate complete pool framing with subtraction before addition, owner/source
extent, slice lengths, bounded list traversal, key positions, sparse ordering,
flags, all local node ranges and proof-record scalar domains before importing any
part of a function. Invalid framing declines old range history. Invalid individual
metadata or a changed key is a miss. Missing or damaged facts never approve a
program and are never interpreted as a negative quality score.

### Storage and failure bounds

Use the caller's existing optional retained-node limit in 1..1,000,000 as a shared
admission bound for the sum of saved query entries, key rows, non-default output
facts, refinements, recurrence records and counted records. Check remaining capacity
before appending any slice. Direct declaration head maps remain independently
bounded by the checked canonical declaration count. At most five entries belong
to one function. No unbounded variant list or cross-revision chain is retained.

An admission failure declines the complete optional new range history while
preserving the complete current ordinary range result and source acceptance.
Already valid current-candidate results may still serve equivalent queries in
that computation. Required candidate service bounds remain separate; record
admission is not a claim about peak RSS or physical epoch reclamation. Allocation
failure retains status 71. Only complete successful candidates publish history.

## Compiler and runtime design

Implement orchestration and saved history in production SLIM using existing typed
owners and declaration maps. `retained` can import `ranges` without a module cycle:
`ranges` depends on `typing`, which depends on `memory`, not on `retained`.
Do not make `typing.Checked` import `ranges`, which would introduce a cycle.
Carry an explicitly present current range view through `retained.Analyzed`,
`check.Attempt` and `project.PreparedProject`, or equivalent acyclic wrappers.

The retained path computes this view after complete successful typing, passes it
to the ordinary termination validation and retains it for generation. Fork and
call-cycle checks remain mandatory before publication. Ordinary checks retain their
current lazy behavior: ranges are needed for termination only when that check asks
for them. Ordinary emission still computes the same range view when none is present.
Factor one emitter body that consumes a supplied current view; do not maintain two
semantic emitters or parse emitted C to reconstruct analysis dependencies.

Keep parallel analysis and scheduling unchanged and current for now. They consume
the exact supplied range view. Semantic result history remains compiler-owned
in-memory data. This contract grants no trust to persisted proof text, serialized
facts or imported executable caches, and introduces no dependency or runtime ABI.

## Compatibility and migration

Preserve source acceptance, diagnostics and their order, every range fact and
refinement summary, every recurrence/counting field, native hazard/resource rows,
parallel candidates/selected/executable sites, and byte-deterministic C. Keep the
existing proof domain, pass count, refinement cap and unknown reasons unchanged.

## Diagnostics and failure cases

There is no new diagnostic or proof rule. A rejected candidate cannot publish
facts or artifacts; public actual-host identity binding is still separate M1 work.

## Performance and complexity

Default analysis retains its existing bounded approximately linear schedule.
Each candidate performs five lexical passes and current global input scans.
Query matching examines at most five keys per declaration. Pool checks, key
comparisons, copying and reconstruction remain explicit linear work, not free
cache hits. Shared admission bounds prevent unbounded accumulated history.

Native counters must observe actual range invocations, function-producer calls,
imports, pass transitions and parameter scans separately from typing, memory plans,
parallel analysis and C generation. Measure cold/unchanged/body/interface costs,
record sizes and geometric work. Preserve all existing gates and dated contrary
measurements. No speedup or performance exception is presumed.

## Alternatives and drawbacks

Source-only reuse misses caller-derived inputs. Treating the refinement budget as
per-function silently changes analysis. Saving only the last pass loses otherwise
reusable earlier keys; saving five unconditional copies adds needless cold cost.
Whole-source range caching alone does not satisfy local query reuse. A second range
analyzer would duplicate semantic authority. Saved metadata and imported facts add
real storage and scanning costs that must be measured before this child is complete.

## Test and acceptance plan

Compare every range-view field with ordinary analysis across the accepted corpus,
full native/resource/parallel baselines and generated C. Exercise caller argument
body edits with unchanged callee source, multiple callers and recurrence inputs,
transitive interfaces/effects/modes, definition reordering, deletion/reinsertion,
source/node relocation and rejected candidates with recovery.

Cross global refinement counts 63/64/65, five-pass input propagation boundaries,
the integer proof-domain edges, recurrence/counting shape limits and sparse default
facts. Corrupt each pool/slice/head/key/tag/position category, including extreme
counts, stale owners and invalid parent references. Verify optional storage at and
below exact need. Check complete no-change reuse and actual per-function work in
scalar, dense and multi-application edits, including reuse within a candidate.

Run ordinary/ASan/UBSan differentials, bounded allocation-fault campaigns, all
required per-commit checks and paired geometric latency. Run full parent release
closure only when every M1 obligation is implemented and verified.

## Ratings and evidence

Analysis +2 specifies complete source, caller-input and global-budget dependencies
and exact reconstruction of current range evidence. Other ratings remain zero
pending measurements. Weighted score 15. No stronger range proof or safety claim.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit complete M1 goal.
This approves the stated internal implementation contract, not external review,
completed implementation, changed precision or relaxed performance requirements.

## Implementation

Pending. The producer read/call audit starts at validated checkpoint 6c39e40.
M1 remains open for this integration, parallel/global analysis retention, actual
flow ownership/loans, public host-bound sessions, stable C fragments/backend
artifacts and complete locality/differential/release closure.

## Removal and supersession

Preserve sole producer authority, complete input/budget keys, exact current evidence,
typed node relocation, bounded history, transactional misses, actual work counters,
all compatibility/performance gates and historical cost evidence.
