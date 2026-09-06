# RFC-0119: Checked field replacement

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: language
Primitive: affine-replacement
Safety: 2
Compile: 0
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 0
Score: 45

## Summary

Make affine struct projections shared reads and provide one checked operation,
`mem.replace(@place, replacement)`, to transfer an affine value out while
installing a valid replacement. This closes the M0 projected-ownership gap
without introducing partially initialized aggregate states. Definite whole-name
reinitialization remains a separate M0 repair.

## Motivation

The production compiler at de031be accepts a local vector initialized from an
owned struct field. Growing that local reallocates its buffer while the struct
retains the old descriptor; reading the field reproduces heap-use-after-free
under ASan. The compiler itself extracts affine fields, including checker issue
vectors. Merely rejecting field extraction would strand those legitimate uses.

RFC-0112 calls for exclusive replacement rather than general partial moves.
An ordinary 0.9 library cannot implement it: reading an exclusive parameter
creates a shared borrow, which cannot be returned as ownership, and holding that
borrow blocks replacement. A primitive is necessary to check the atomic ownership
transition without exposing an uninitialized place or raw pointer.

## Guide-level explanation

`owner.values` reads an affine field as a shared borrow, even for an owned or
exclusive root. Locals retain its origin loan through their lexical scope.
Scalar fields continue to copy. Plain projections cannot produce affine owners
for returns, aggregates, collection insertion, or owning arguments.

To extract a field, write `mem.replace(@owner.values, vec.new())`. The operation
returns the old vector as an owner and leaves an empty vector in the field.
The replacement expression runs before the exchange. Its checked effects and
failure behavior remain visible. There is no intermediate uninitialized field.

## Reference-level specification

The operation has exactly two arguments. The first is marked `@` and denotes a
whole named affine owner or a chain of struct fields rooted in a named owned or
exclusive binding. The second has no marker, has exactly the place's affine
type, and transfers ownership as an ordinary owning destination. The result has
that same type and owns the former value. Copyable places are rejected: ordinary
copy and assignment already express their operation.

Shared roots, moved roots, borrowed local aliases, unknown origins, temporaries,
conditional places, collection reads, and enum payload projections cannot grant
replacement access. Each field is normally linked and type checked. Reserve
exclusive access to the entire root through both arguments and the operation;
replacement expressions cannot read, mutate, move, or replace that same root.
Independent roots remain usable. Existing lexical and call loans must be absent.
Disjoint-field loan precision and indexed replacement are successor work, not
implicit exceptions to the root-level exclusivity rule.

All affine field reads have shared mode, including fields of computed owned
values. A computed root without a persistent owner has conservative unknown
origin. Scalar copies create no storage loan. Unknown evidence never permits
exclusive access or ownership escape.

## Compiler and runtime design

Implement checking, lowering, and analysis in selfhost SLIM. Reuse canonical
field/name nodes, their normal types and links, and existing loan records. The
only extra derived place view is the bounded-by-source root/field chain; no
second parsed representation or persistent place model is introduced.

Lower the exchange to typed C temporaries and assignments to the actual named
or dereferenced parameter place. Evaluate the replacement exactly once, preserve
left-to-right source evaluation and allocation-failure checks, save the old
value, install the replacement, then deliver the old value to its destination.
This ordering also handles a result destination that names the replaced owner.
Do not materialize a copied field as the write target. Ordinary and counted
lowering share the same operation. No runtime ABI change or allocation is added.

Memory planning must track replacement storage into the root destination and
the extracted result's escape conservatively. Parallel and quality analysis
must retain mutation/exclusive-access blockers. Missing semantic summaries in
bounded analyses remain unknown; replacement is never silently treated as pure
arithmetic or a source reduction.

Retain each checked binding's type and borrow mode on its canonical name fact.
The existing analysis binding report must use that fact rather than infer local
ownership from storage type or label every enum payload shared. Unavailable
binding facts report unknown. This corrects an existing reporting defect exposed
by field borrows; it adds no second ownership checker. A fact retains the existing
two-I64 layout: a tag and source type form. Expression tags encode `kind + 2`;
binding tags encode `8 * (mode + 1) + kind + 2`. Type kinds are -2 through 4 and
binding modes are 0 through 2, so tags lie in 0 through 30, and the encoder's
arithmetic cannot overflow. A tag modulo 8 recovers the type category; its
quotient identifies a checked binding mode or unknown. The normal fact accessor
decodes types for every consumer. One final linear binding traversal records
these facts. Measure its cost explicitly and record every changed native row.

## Compatibility and migration

Replace unsafe affine extraction in the compiler and maintained corpus with
explicit replacement or ordinary shared inspection. Never copy storage to
preserve an invalid program. Remove current migration advice that claims a
plain local field extraction safely enables an owning transfer. The final seed
and selfhost fixed point must use the same checked semantics; a bootstrap
transition compiler is only an ignored build artifact, not a production fallback.

## Diagnostics and failure cases

Use existing E0351 for arity, E0360 for argument markers, E0344 for type mismatch
or a copyable place, E0348 for an unsupported place shape, E0347 for a shared
capability or borrowed replacement, E0315 for moved roots, and E0349 for loans.
Keep exact diagnostic spans in permanent conformance fixtures.

## Performance and complexity

Checking and emitting a place chain costs linear work in that chain's source
size; root loan access remains indexed. Do not scan all bindings per replacement.
Preserve every scaling fixture and add a geometric replacement/field-loan series
under the unchanged 1.25 ownership check ceiling. Compare baseline and candidate
on the same host with warmup and interleaved samples. Replacement introduces no
buffer copy, allocation, synchronization, or runtime metadata.

## Alternatives and drawbacks

Implicit projected transfers require partial-move tracking or invalidate an
entire aggregate and prevent extraction of its other fields. General partial
moves contradict the accepted initial successor direction. Blanket rejection
without an extraction operation removes required compiler functionality.
An ordinary library lacks permission to return the old borrowed storage.
Root-level loans conservatively reject simultaneous access to disjoint fields;
later precision needs independent evidence and an accepted specification.

## Test and acceptance plan

Retain the UAF witness as a production rejection. Cover all owning destinations,
borrow lifetimes, nested fields, scalar copies, computed/unknown/shared/moved
roots, marker and type errors, arity, replacement order and failure, result
aliasing, and ordinary/counted lowering. Migrate and bootstrap the compiler;
execute positive cases under ASan/UBSan and bounded allocation-fault injection.
Preserve complete native analysis/resource baselines or explain each changed row.
Run governance, conformance/malformed inputs, required benchmark and test gates,
and record the exact supported domain and remaining M0 work.

## Ratings and evidence

Safety +2 removes reproduced affine alias invalidation. Compile and runtime 0
make no speed claim. Minimal +1 replaces ambiguous implicit field extraction
with one explicit ownership-preserving operation and rejects general partial
moves. Analysis +2 makes the exclusive transition and its origin explicit.
Dogfood 0 makes no unmeasured productivity claim. Weighted score: 45.

## Decision

Accepted under the maintainer's RFC-0112 implementation authorization and active
goal to complete M0. This records delegated authority, not separate maintainer
review. No hard gate or performance budget is relaxed.

## Implementation

Implemented in the production SLIM checker, C generator, memory planner, and
ownership/quality/parallel reports. Compiler field extraction uses replacement;
the portable seed and selfhost reach the same fixed point. Thirty-two permanent
conformance rows cover the reproduced alias invalidation and replacement's
positive and negative contract. Integration tests check counted lowering,
caller-visible mutation, checked ownership modes, and the 64-binding report
boundary. All eight positive fixtures pass ASan/UBSan, and bounded compiler and
generated-program allocation-fault campaigns fail cleanly.

The first wider fact representation caused a measured frontend regression and
was replaced by the two-word tagged representation before committing. Paired
measurements retain both results. The replacement scaling exponent is 0.814
under the existing 1.25 ceiling. Full checkpoint tests and required benchmark
gates pass. Exactly two native payload-binding report rows are corrected from
unknown/shared to I64/copy; all native summary/resource and blocker rows remain
unchanged. The dated progress report records exact hashes, measurements, changed
rows, and bounded validation domains:
[field replacement evidence](../../benchmarks/results/2026-09-05-slim-next-progress.md#checked-field-replacement-checkpoint-2026-09-06).
Definite reinitialization, actual-work counters, and full M0 closure remain open.

## Removal and supersession

The successor place/lifetime model may extend this same canonical operation
after independent evidence. Preserve the invalidation witness, replacement
ordering/failure contract, exact negative cases, and permanent scaling gates.
