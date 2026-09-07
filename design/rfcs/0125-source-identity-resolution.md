# RFC-0125: Source identity resolution

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
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Implement RFC-0124's first identity boundary in the production snapshot index.
Introduce nominal revision, file, declaration, node, and byte-span records in
`selfhost/identity.slim`, with checked resolution into the current flat view.
This stage does not retain checked queries or claim incremental compilation.

## Motivation

Snapshot declarations currently carry unqualified absolute token positions and
byte offsets. Before retaining any result across updates, make ownership explicit
and reject stale or malformed handles before indexing.

## Guide-level explanation

A node handle names one declaration in one source revision and a local ordinal.
An old handle cannot address a new revision, even if its absolute position happens
to be unchanged. A span names one file revision and a half-open byte interval.
Original source bytes remain available for exact comparisons.

## Reference-level specification

`Revision(epoch, serial)` uses positive I64 components. `successor` increments
serial only below 9223372036854775807; `reset` increments epoch only below that
limit and starts serial at one. Invalid components and exhaustion return `None`.
There is no wraparound or implicit reset. These are session-local identities,
not persistent or process-global credentials.

`FileId(revision, slot)` and `DeclarationId(file, slot)` use nonnegative slots.
`NodeId(declaration, ordinal)` uses a declaration-local canonical token ordinal.
`Span(file, start, end)` retains original half-open byte positions. The snapshot's
existing full `(module, kind, name)` key remains the logical declaration key;
the slot is a version-local address, never a replacement for full key equality.

`View(declaration, first, count)` is an explicit import adapter to one current
canonical token vector. `resolve_node(view, handle, total)` validates the full
identity, nonnegative positions, `first <= total`, `count <= total - first`, and
`ordinal < count` before adding. `resolve_span(file, bytes, span)` validates full
file identity and `0 <= start <= end <= bytes.len(bytes)` before returning the
start. Both return `Index::Invalid` or `Index::Found(I64)`; invalid is never zero.
Zero-width EOF spans are valid. Empty views contain no resolvable node.

The session command assigns initial revision (1,1), changed revision (1,2), and
recovery revision (1,3); its invocation is the ownership boundary. No handle is
exported. This finite snapshot command does not allocate retained service history.
The later service allocator must use these checked successors and specify its
storage limits under RFC-0124; this change adds no source-size ceiling.

Snapshots store a typed declaration view, body span, and interface span. Dependency
scanning resolves the declaration's local root in the current view before reading
tokens. State construction validates all imported views/spans; invalid state
returns command status 65 and tooling reason `Q0001: invalid source identity`.
The session validates the existing manifest token shape in linear work before
calling the module loader, and propagates input read/parse failure instead of
measuring a partial index. This is validation of tooling data, not a second SLIM
program parser. Existing valid-session output remains four historical estimate fields.

Revision-safe cross-update node translation is not enabled in this stage. Exact
key/body comparison still drives historical invalidation estimates; it cannot
authorize checked result reuse. Retained mapping, typed semantic identities,
dependency observation, and semantic reuse remain RFC-0124 obligations.

## Compiler and runtime design

Use ordinary SLIM records, enums, and checked arithmetic. The only accepted parser
and checker remain unchanged. No runtime ABI, built-in, dependency, or Rust semantic
implementation is added. Regenerate and reproduce the portable C seed.

## Compatibility and migration

Valid source and estimate output retain their behavior. Invalid imported handles
fail explicitly with Q0001 and no successful estimate; source failures retain
existing diagnostics. Nominal type mismatches use existing type diagnostics.

## Diagnostics and failure cases

Invalid source ownership uses Q0001; no partial index is reported as success.

## Performance and complexity

Each resolver uses constant work and no allocation. Index validation visits each
declaration once; declaration IDs and spans increase snapshot storage. Ordinary
clean checking does not allocate a new per-node identity table. Record seed cost
and preserve existing performance gates; no budget relaxation is authorized.

## Alternatives and drawbacks

Migrating every current scalar node position at once would make this boundary
harder to verify. The explicit flat-view adapter is transitional, with a checked
boundary, while retained compiler stages proceed. Typed records are not unforgeable
capabilities; resolution must validate even manually constructed invalid records.

## Test and acceptance plan

Compile and execute the actual production source-identity module with positive
and adversarial handles: stale revisions/epochs, wrong files/declarations, negative
and extreme positions, EOF/empty spans, relocation of a view, local ordinals, and
revision exhaustion. Reject interchange of nominal handle types. Preserve session
body/interface/recovery fixtures and check malformed input rejection. Run bootstrap,
governance, Cargo, required benchmark gates, and sanitizers before checkpointing.

## Ratings and evidence

Analysis +2 introduces explicit checked provenance at the source-index boundary.
All other ratings are zero; no semantic safety or speedup claim. Weighted score 15.

## Decision

Accepted under the maintainer's RFC-0112 delegation and explicit M1 completion
goal, following RFC-0123 and RFC-0124. This is not independent external review.

## Implementation

Implemented in the production `identity`, `query`, and `session` modules.
The 7,350-result independent boundary oracle, source-index integration tests,
reproduced seed, required compiler gates, and sanitizer/fault campaign pass.
See the [M1 source identity checkpoint](../../benchmarks/results/2026-09-05-slim-next-progress.md)
and [same-host measurements](../../benchmarks/results/archive/2026-09-06-m1-source-identities.tsv.gz).
This completes this child boundary only; RFC-0124 and M1 remain incomplete.

## Removal and supersession

Later retained-query stages must preserve these failure cases and replace raw
positions at their own boundaries. The snapshot estimator is not a permanent
alternative compiler path and does not satisfy the M1 reuse exit criteria.
