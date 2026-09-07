# RFC-0134: Retained place identities

Status: accepted
Implementation: complete
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

Add a distinct PlaceId at retained-query boundaries and a bounded query deriving
named storage and nested field-projection facts from the normal checker's retained
canonical nodes, types and bindings. Connect the function flow view to this query.
Do not add a second ownership checker or grow the 64-byte retained saved row.

## Motivation

RFC-0130 retains typed node, type and binding identities, but a client still has to
interpret raw node positions to relate a field projection to its lexical binding.
RFC-0129's operations need a typed way to request these checked facts before the
remaining ownership-orchestration migration. A field path must not be confused
with proof of unique ownership or disjointness from another lexical binding.

## Guide-level explanation

A PlaceId names one canonical occurrence under its declaration and revision.
A query can identify a binding declaration or a chain of ordinary field projections
rooted in a local/parameter/pattern binding. It returns the original occurrence,
root occurrence and binding declaration, nominal BindingId, root and value TypeId,
checked type kinds, binding borrow mode, and projection depth.

These are exact syntactic/checked binding facts, not allocation identities.
Distinct lexical bindings can alias the same owner. Borrow mode describes the
binding's checked mode, not whether mutation or transfer is currently permitted.
Availability, loans and call effects remain decisions of the sole normal checker.
Computed bases, calls, collection reads and control-result expressions are unknown
for this place query; they do not become invalid language programs.

## Reference-level specification

PlaceId contains a revision-owned canonical NodeId, distinct from TypeId and
BindingId. Query only compiler-owned accepted Cache records. Validate cache
eligibility, equal node/shape/owner lengths within 1..1,000,000, declaration/file/
revision identity, owner interval, local ordinal, saved owner and fact tag before
indexing or interpreting a result. Reject stale, foreign, negative, out-of-range
and structurally missing handles; never address a current revision using old
absolute positions. A caller may construct a new PlaceId only from a current node
or a separately established exact node map; this query does not invent remapping.

Use existing canonical shape tags and stored checked links. A named binding
occurrence resolves through its checked Local link to a binding declaration in the
same function. A binding declaration itself is a root. Its saved Binding link
provides the nominal binding identity. Nested projection follows the canonical
get operand and checks each extent before advancing. Preserve lexical shadowing
and enum/pattern binding identity. Types come from checked saved facts, not new
inference. Missing type or binding facts produce stable unknown reasons.

The explicit budget is 1..1,000,000 visited place nodes. Charge each named root or
projection once, check the bound before traversal and increment only within that
bound. A direct binding/root needs one step; each projection adds one. Exact
results include the observed steps. Exhaustion is bounded with its supplied
budget; malformed handles are invalid with a stable reason. Unknown results name
unsupported shape or missing checked evidence. No result establishes that an
unsupported operation is safe or unsafe.

No recursive lookup over alias definitions, heap objects or branch alternatives
is authorized. Projection descent strictly increases the canonical local ordinal;
binding declaration resolution is a checked terminal lookup, not another path
search. No new allocation, persistent vector, parser or semantic fallback is
needed. Existing saved-row and stored-link budgets remain unchanged.

Flow exposes a query adapter taking Graph, BlockId, Cache and a place budget.
It validates the complete flow graph and matching declaration/revision before
requesting the block's current canonical place. Binding/evaluation/projection/
argument occurrences can supply facts; unrelated operations remain unknown. This
adapter does not add a second acceptance path or claim completed ownership
orchestration. A bounded graph returns its graph budget without running a place
walk; a completed graph delegates the separate place budget to retained resolution.
Existing typechecking and diagnostics remain unchanged.

## Compiler and runtime design

Implement PlaceId, PlaceFact and a classified result in retained.slim, with checked
resolution over its existing immutable Index and Saved rows. Add the adapter in
flow.slim. Keep normal compiler/runtime semantics and ABI unchanged. The query
runs only when requested; ordinary compilation adds no place-analysis pass.

## Compatibility and migration

No syntax, ownership, effect, accepted-source or generated-C change is intended.
Persistent places are newly nominal; scratch array positions remain private.
The remaining RFC-0124 migration of availability and loan orchestration onto flow,
public sessions and granular retained queries remains required.

## Diagnostics and failure cases

Tooling results distinguish exact, bounded, unknown and invalid. Stable reasons
cover invalid budget, cache/owner/node mismatch, malformed projection, missing
checked type or binding, unsupported computed base and inapplicable flow operation.
These results do not replace or create source diagnostics.

## Performance and complexity

The query is O(projection depth) within its checked budget, with constant auxiliary
storage and no allocations. Default compilation does not execute it. Measure
native query work on geometric projection chains and retain before/after ordinary
compiler timings, exact native analysis/C baselines and fixed storage budgets.

## Alternatives and drawbacks

A raw canonical position loses its revision owner. A separately allocated path
vector increases every retained row or duplicates the source projection chain.
Following lexical aliases as though they were unique allocation roots would invent
ownership evidence. Re-running typing or ownership in this query would create a
second semantic authority. This read-only access path deliberately does none of
those operations; shared flow orchestration remains additional required work.
Repeated queries over every nested projection can repeat path walks, so a future
batch consumer must bound total work or memoize its ephemeral results rather than
introduce a default quadratic pass.

## Test and acceptance plan

Exercise parameters, locals, shadowing, owned/shared/exclusive bindings, nested
fields, enum payloads and binding declarations. Cover computed bases and collection
reads without upgrading unknown to an ownership claim. Validate stale revisions,
wrong declaration/file/ordinal, missing facts, malformed shapes and exact/beyond
budget boundaries. Compare current facts after explicit node mapping, insertion,
reordering and relocation with a fresh checked cache. Test the flow adapter with
matching and mismatched owners and bounded graphs. Run sanitizer/fault checks,
geometric work and all required checkpoint gates. Preserve full M1 exit criteria.

## Ratings and evidence

Analysis +2, score 15, records a typed access path to existing checked place facts.
Other ratings remain zero pending evidence. No ownership precision or overall
incremental latency improvement is claimed.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit M1 goal. This is
not external review. No semantic or performance gate is weakened.

## Implementation

Implemented in retained.slim and flow.slim with no added retained row or default
analysis pass. The production probe compares all current node queries against
fresh checked caches and verifies mapped identities across edits. All 94 accepted
conformance/native sources, exact/beyond-budget boundaries, missing individual
type/binding facts, malformed metadata, nominal-type diagnostics and flow-owner
checks pass. Native work is projection depth + 1 through depth 256; a one-step
budget observes exactly one visit. Both repetitions agree under ASan/UBSan.

The 2,048 allocation-fault ordinals produce 153 status-71 failures with empty
stdout and 1,895 successes, with ordinary/sanitized status and output agreement.
All 193 rejected fixtures retain prior status/diagnostics; 20 native applications
retain exact analysis/resource rows and byte-identical C relative to 07d721d.
Bootstrap, governance, 10 unit/69 integration tests, 333 conformance fixtures and
2,000 mutations, formatting, Clippy and all required checkpoint gates pass.

The portable fixed-point seed is 4,033,892 bytes, SHA-256
`dddb3865357a149932bc4c2cd2b09e05e822c65cb16d0bee6413d0412a320b82`.
Saved remains at most 64 bytes and StoredLink at most eight bytes. Paired default
project checks at 4,000 helpers measure 20.383 ms before and 21.112 ms after;
these process totals are dated observations, not a speedup claim or portable gate.
Detailed matrices, work, fault and timing records are linked from the
[SLIM Next progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md).
Full M1 ownership orchestration and retained build integration remain required.

## Removal and supersession

A replacement must preserve nominal revision ownership, complete checked binding
and type provenance, unknown computed bases, bounded traversal and all permanent
safety, diagnostic and storage gates.
