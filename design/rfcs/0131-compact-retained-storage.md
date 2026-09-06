# RFC-0131: Compact retained storage

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 1
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 10

## Summary

Compact RFC-0130's immutable in-memory storage while preserving typed semantic
handles and the same checked relocation, dependency and publication decisions.
Store one typed declaration owner per saved row and owner-scoped storage words;
materialize TypeId, BindingId and NodeId values only at query boundaries.

## Motivation

The first retained implementation stores 264 bytes per canonical node plus a
104-byte temporary pre-inference link. Most of those words repeat revision and
owner information. Native measurements establish actual inference reuse but show
snapshot construction and copying outweighing its savings in a two-revision run.

## Guide-level explanation

A saved row belongs to one declaration in one revision. Its compact positions are
meaningful only inside that owner's immutable source index. They never directly
index a new revision. Reading a saved type or link reconstructs a complete typed
handle against the old index, then uses the existing explicit checked map.

## Reference-level specification

`Saved` retains a nominal DeclarationId owner, the fact tag, a StoredType word, a
StoredLink word for the post-check link, and a StoredLink word for the pre-check
resolution outcome. StoredType and StoredLink are distinct nominal storage types;
they are not cross-revision semantic handles or externally accepted input data.
The expected fixed payload is 64 bytes per node on the current host.

Before reading any saved row for comparison or import, require exact equality with
the expected old declaration owner, including epoch, revision, file and slot.
Validate fact tags against the existing 0..30 packing domain, excluding low-bit
values of seven. A stored type
position is resolved in the old index into TypeId(NodeId). A stored source link
is resolved similarly. A binding slot becomes BindingId using the saved owner.
Packed local links are decoded against the old token count into full typed binding
and optional type nodes. Every resulting handle passes the existing checked
whole-declaration or interface-prefix relocation before any new-buffer write.

Source-node materialization validates position, owner-table bounds, owner view,
revision/file/slot consistency and containment. Missing or stale metadata remains
a miss. No raw stored position may be applied to the current revision. All saved
rows of a function must pass validation before its first imported fact/link write.
The same pre-inference outcomes, including negative lookup and local shadowing,
remain dependency inputs. Their transient capture vector uses StoredLink words;
comparison and dependency discovery materialize the same typed Link values.

Only the normal production checker can produce new accepted semantic facts. The
storage is compiler-owned, immutable history; it is not a serialized cache schema
or an authentication mechanism for fabricated facts. The 1..1,000,000 token limit
bounds packed-link arithmetic below I64 overflow. Reject a negative packed word
below the maximum representable local-link magnitude before negating it; even
I64 minimum cannot overflow the decoder. Invalid source positions and decoded
links produce an explicit Invalid result rather than a Missing reference.
Existing capacity, stale-revision,
failed-candidate and missing-metadata behavior is unchanged.

## Compiler and runtime design

Replace the old dense row representation in `retained.slim`; keep one canonical
storage form. The normal checker, source parser, inference rules and runtime ABI
do not change. Full semantic identity records remain the relocation interface.
Use ordinary SLIM records/enums and checked vectors; no primitive or dependency.

## Compatibility and migration

Internal storage only. Preserve acceptance, diagnostics, all token fields and
facts, layout/memory/analysis results, generated C, public commands, and every
existing work/reuse and failure gate. `imported` keeps its actual canonical-node
import meaning and its permanent geometric budgets. Persistent service and C
fragment retention remain parent M1 work.

## Diagnostics and failure cases

Wrong row owners, stale revisions, missing index entries, out-of-range positions,
invalid tags and invalid mappings cannot authorize an import. Recompute normally
on a miss. Allocation failures retain status 71 and cannot publish a partial check.

## Performance and complexity

Measure fixed storage, geometric actual work and paired uninstrumented timings
against 4dfd31d. Preserve the two-clean-check reference as well as old/new retained
comparisons, so reducing snapshot overhead cannot be mistaken for an overall
latency win. Keep default work approximately linear and all existing gates.
Record source copying, capacity slack and scratch-lifetime limitations honestly.

## Alternatives and drawbacks

Dropping ownership information would permit accidental cross-revision indexing.
Keeping every expanded union repeats immutable context and consumes unnecessary
memory. A new serialized IR or unchecked pointer representation is disallowed.
Materializing typed handles adds local checked work; actual measurements decide
whether the smaller storage is worthwhile.

## Test and acceptance plan

Run the complete retained differential corpus and geometric gates unchanged.
Add a wrong-owner saved-row regression, including revision mismatch and a different
declaration in the same revision. Exercise missing and out-of-range type positions,
I64 minimum packed pre/post links, I64 maximum source links and invalid tags.
An early otherwise-importable altered fact followed by a bad final row must leave
the fresh facts untouched before the ordinary checker runs. Compare all
facts/links/C with clean checking.
The native observer compilation permanently gates Saved at no more than 64 bytes
and StoredLink at no more than eight bytes. These fixed-payload budgets exclude
vector capacity and index/source storage.
Run sanitizer and allocation-fault campaigns, before/after storage and latency
measurements, and every required checkpoint gate before completion.

## Ratings and evidence

Compile +1 targets measured reduction of retained memory traffic; score 10. The
baseline is 264 bytes per saved node and 104 per temporary link. Keep the rating
only if final measurements support the improvement; no speedup is presumed for
ordinary checking or the completed M1 service.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit M1 completion goal.
This is not independent external review. No existing budget or semantic gate changes.

## Implementation

Implemented in `selfhost/retained.slim`, with native fixed-payload gates in the
retained observer and storage-corruption regressions in the production probe.
The [progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
records the complete campaign and raw measurements. Saved is 64 bytes versus 264;
transient links are eight versus 104. At 4,000 helpers the paired old/new retained
unchanged medians are 59.125/49.764 ms; the separate two-clean-check comparison
still favors clean checking. Compile +1 is retained for the measured improvement,
not a claim that full incremental latency or M1 is complete.

All required checkpoint gates, storage-owner/range/atomic-import regressions,
94-file sanitizer differentials and 512 allocation-fault ordinals pass. All 20
native C/analysis rows and 193 rejected diagnostics are unchanged from 4dfd31d.
Bootstrap is 3,861,132 C bytes, SHA-256
`e8accfbc15ab9436213e8648ba31b434c2124bdc87a8578a5572f98d36c1caff`.

## Removal and supersession

Any replacement must preserve typed ownership at query boundaries, old-index-only
storage decoding, checked relocation, complete dependencies and all durable gates.
