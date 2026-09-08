# RFC-0149: Uniform moves and lexical references

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: Codex, pursuing SLIM Next M2 prerequisites
Created: 2026-09-08
DecisionDate: pending
Approver: project-maintainer
Kind: language
Primitive: uniform-ownership
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Replace RFC-0110's call-only modes with uniform explicit ownership transfer and
lexically checked reference values. Specify one checker result for initialization,
loans and cleanup obligations, consumed by code generation and retained queries.
This proposed contract depends on RFC-0148 and the complete RFC-0150/0151 memory
package. It does not authorize implementation or use the unaccepted RFC-0147
rating exception.

## Motivation

Current local/aggregate transfers are unmarked, while calls use `^`; shared
affine parameters are implicit and exclusive parameters use `@`. M0 repaired
these rules but did not create first-class escaping references. M1's canonical
control descriptions and typed place identities are foundations, not an
independent lifetime checker. The current memory planner's 64-value precision
fallback cannot justify a reference or early storage release.

## Guide-level explanation

An owning parameter receives a value; a named noncopyable source must use
`move`. A fresh owned result can flow directly to its destination. Read and
write access use `&place` and `&mut place`. There is no implicit borrowing,
reborrowing, dereference or conversion between these permissions.

The following is proposed successor syntax, not compilable SLIM 0.9:

```text
fn length['a](values: &'a Vec[I64]) -> I64:
  vec.len(values)

fn consume(values: Vec[I64]) -> Unit:
  ()

let next: Vec[I64] = move values
consume(move next)
```

RFC-0151 subsequently adds explicit allocator-lifetime arguments to owning
storage types. The spelling above describes the initial parameter-mode cutover,
whose reclamation policy remains the old conservative policy until RFC-0150/0151
are integrated. Neither stage permits an unchecked escaping byte view to dangle.

## Reference-level specification

### Types, places and operations

Reference types are `&'a T` and `&'a mut T`. A function or nominal data
declaration binds lifetimes in its existing bracketed declaration parameter
position, for example `fn first['a](...)` or `struct View['a]:`. Lifetimes
precede any later type parameters; generic source type parameters are not
implemented by this RFC. Lifetime names have an apostrophe followed by an
ordinary identifier. `'static` is reserved for static backing storage.

Every reference in a declaration signature names its lifetime. Local reference
annotations use `&T` or `&mut T`, with only the region inferred from the
lexical binding and its origins. This inference is part of reference checking;
it does not infer an omitted local value type or published signature. A returned
reference names an input lifetime or `'static`; an unconstrained output lifetime
is rejected. Multiple inputs can share a binder; the result is valid only for
their common supported borrow interval. No general lifetime inequality syntax,
implicit permission conversion or higher-rank lifetime polymorphism is added.

`*reference` is the explicit dereference place. Projection through a reference
is `(*reference).field`. Dereferencing a copyable value copies it; dereferencing
an affine value is a place for reading, borrowing or replacement, not an owning
read. A reference cannot be formed from an arbitrary rvalue or a partial owner.
Binding a fresh owned result first provides a named place and an explicit lifetime.

Assignment has the canonical form `place = expression`. Its target is a mutable
whole binding, a known field of an initialized mutable root, or a dereference
with exclusive permission. Evaluate/reserve the target once before the right
side; projection or checked access may not be reevaluated after a side effect.
Shared referents and fields of unavailable aggregates reject. An initialized
affine target releases its old value only after successful replacement under
RFC-0151; assignment does not return that old owner. `mem.replace` does.

`move name` transfers one initialized noncopyable whole binding, including an
exclusive reference. Reject `move` on copyable values, borrowed referents,
projections and arbitrary expressions. A conditional produces an owned result
only when each selected branch explicitly transfers a named source or produces
a fresh owner. General partial moves remain unsupported.

The owning destinations are bindings, assignments, function and recurrence
arguments, struct fields, enum payloads, collection insertion and returns.
All apply the same transfer rule. Calls do not synthesize moves. Copyable
arguments copy; read borrowing is explicit even for a scalar when its parameter
is a reference. A passed exclusive reference is moved unless explicitly
reborrowed as `&mut *reference`; a read reborrow is `&*reference`.

### Copy, initialization and joins

Scalars and shared references copy. Exclusive references, storage owners and
ordinary nominal aggregates are affine. `copy struct` and `copy enum` opt a
nominal aggregate into structural copying only if every field/payload is
copyable. No user copy hook or implicit storage clone exists. A source migration
must annotate previously copyable aggregates or supply explicit moves; the
checker does not guess that a public type was intended to copy.

Each whole binding has definite availability for source use and a separate
cleanup obligation: always initialized, never initialized, or path-dependent.
Control joins intersect availability over reachable predecessors and retain
every predecessor's cleanup obligation. Moving invalidates the source before
the next expression is checked. Whole-binding assignment evaluates its right
side while the destination's current availability and reservations remain in
force, installs a checked value and makes the binding available. Replacing an
available owner releases its old value under RFC-0151. An unavailable aggregate
cannot be restored field by field.

`match move owner` transfers the scrutinee and gives the selected arm ownership
of its payload. A shared or exclusive match borrows payload places with the
corresponding permission (`match &owner` or `match &mut owner`) and preserves
the parent loan. A fresh owned scrutinee needs no move marker. No implicit owning match
is inferred from later payload use. Branches begin from the same incoming state;
nonreturning edges do not fabricate an available result at a join.

A value moved on only one predecessor is unavailable for later source reads,
but the other predecessor still owes destruction. Preserve that distinction
through reinitialization and exits. Emit a checked initialization flag only when
path-dependent cleanup requires it; set/clear it at the actual transfer or
initialization, and release exactly when the flag is set. Do not omit cleanup
because the conservative source-use state says unavailable, or destroy the
remaining owner early at a join. Measure these flags as runtime ownership work.

### Lexical loans

A loan identifies the backing owner, checked place path, permission, source
origin and lexical end. A loan stored in a local remains live to that binding's
scope end, even after its last textual use. Temporary argument loans remain live
through all later argument evaluations and the call. A returned loan extends
to its receiving binding and keeps every necessary ancestor loan alive. Moving
a reference transfers its loan obligation; copying a shared reference retains
the origin until all lexical copies expire.

Overlapping shared loans may coexist. An overlapping exclusive loan excludes
other reads, writes, moves, replacement and destruction of its referent, except
through checked child reborrows. Parent exclusive access is suspended until
all child loans end. A shared child never restores exclusive access early.

Different known whole owners and distinct inline struct fields are disjoint.
The owner root cannot move or be destroyed while any field is borrowed.
Dereferences carry their checked origin paths; different reference variable
names alone do not prove disjointness. Unknown projections, enum alternatives
and dynamic collection indices conservatively overlap. RFC-0150 defines the
additional storage-invalidation rule for elements and slices.

Reference fields are allowed in lifetime-parameterized aggregates. Such a value
may outlive neither its referents nor the origin loans. Reject any aggregate
that would contain both movable backing ownership and a reference into that
same ownership. No self-referential movable value or untracked alias is admitted.

`mem.replace(&mut place, replacement)` is the sole exchange form: reserve the
place, evaluate the replacement once without accessing the reserved root,
install it, and return the old owner. The exchange allocates nothing and cannot
fail after operand evaluation. Named owners, known struct fields and checked
element places are supported; moving a field without replacement is rejected.

### Mandatory analysis bounds

Proposed default limits per function are 1,000,000 ownership/loan work units and
65,536 simultaneously retained state cells. One unit is charged for each
control-edge transfer, state-cell read/write/merge, origin comparison and
lifetime-constraint visit. Allocation of analysis state is charged before use.
Build configuration may select work 1–16,000,000 and cells 1–1,048,576; values
outside those ranges reject before analysis. Arithmetic checks precede addition
and multiplication. No unbounded setting or silent clamping exists.

Mandatory exhaustion rejects that function with a limit diagnostic and no
executable publication. Optional range/totality/parallel analysis keeps its
existing independent bounds and unknown behavior. Worklists use canonical lexical
node order and monotone conservative joins. A future loop child must use the
same state authority and prove its fixed-point transfer rules before adding
backedges. These proposed limits require compiler-corpus and geometric evidence
before acceptance; their availability is not a claim that the current corpus fits.

## Compiler and runtime design

Extend canonical parsing, typing and the checked ownership continuation state
in selfhost/. Attach typed origin/place/region facts to revision-owned nodes.
The optional flow view describes accepted facts and cannot approve source.
Emit references from those facts using typed C pointers; lifetime parameters
erase and introduce no reference count, heap allocation or runtime borrow check.

Retained checking depends on reference permissions, lifetime signatures, copy
classification, place layouts, consumed bodies and configured mandatory bounds.
Retained cleanup/emission depends on the complete checked result. Missing or
stale origins miss/recheck; they never import a shorter loan. Preserve complete
diagnostics, source spans and clean/retained C equality across those edits.

## Compatibility and migration

At the parameter-mode cutover remove `@`, `^` and implicit shared affine
parameters from accepted source. Migrate the entire compiler/library/fixture
corpus using checked old destination facts. Add explicit moves to every owning
destination and explicit references to borrowed parameters/arguments. Preserve
the exact existing effect, eager-Boolean and recurrence contracts until their
separate successor children replace them. The later allocator and Bytes
cutovers must follow RFC-0148 rather than silently changing this interim contract.

## Diagnostics and failure cases

Required families are missing move, redundant/invalid move, use after move,
unavailable join, permission mismatch, overlapping loan, suspended parent use,
escaping reference, unknown origin, invalid copy declaration, self-reference
and mandatory work/storage exhaustion. Preserve current codes where the condition
is unchanged; allocate new codes before implementation and pin primary plus
origin/use spans in the conformance manifest. Do not reuse historical codes
for a different condition. Automatic move/borrow fixes require a checked unique
destination and cannot claim arbitrary ownership migration preserves behavior.

## Performance and complexity

Keep input processing linear and all mandatory dataflow bounded by charged work
and storage. Report actual checking and retained-import work separately, including
failure at limits. Borrowing and moving storage add no allocation or clone;
pointer/aggregate representation and compiler-size costs require measurement.
Existing memory retention is not retained as a substitute for checking lifetimes.

## Alternatives and drawbacks

Keeping call-only modes avoids a migration but perpetuates destination-dependent
transfer spelling. General partial moves or nonlexical regions would enlarge
state tracking before application demand. Lexical loans and explicit reborrows
can reject convenient programs and add source text; measure repair burden and
library utility rather than assuming familiarity improves agent outcomes.

## Test and acceptance plan

Freeze a 576-cell matrix: eight owning destinations × six source shapes
(whole owner, borrowed origin, field, fresh temporary, conditional result,
unavailable binding) × four control placements (straight, left arm, right arm,
post-join) × three loan states (none, shared, exclusive). Name every cell and
its acceptance/diagnostic oracle; invalid combinations are negative cases,
not silently omitted coverage. Add nested reborrows, reference returns/fields,
copy declarations, replacement evaluation order, path-dependent cleanup after
one-arm moves and reinitialization, and boundary exhaustion tests.

Use independent small ownership-state traces, positive escape witnesses and
ASan/UBSan execution to complement checker expectations. Cross each configured
limit below/at/above and test malformed extreme configuration without overflow.
Migrate all existing regressions; compare full cold/retained outputs and spans.
Run all AGENTS.md gates, geometric ownership workloads, memory/code-size
measurements and full compiler/library migration before adoption. No safety
claim extends beyond the named checked model and tested domains.

## Ratings and evidence

Ratings and score are zero, with benefit/cost unknown pending implementation.
Current mode asymmetry, lexical control descriptors and memory-planner fallback
are exact source observations. The proposed cross-product and limits are fixed
test specifications, not completed evidence. RFC-0147 approval and a qualifying
child evaluation decision are required before experimental acceptance.

## Decision

Proposed; not accepted. Await the policy decision, the complete memory contract
package and preimplementation workload/cost evidence. No production change is
authorized by this document's existence.

## Implementation

Pending. Implement parameter modes/moves first, then reference values and
returned lexical lifetimes, using the M2 plan's separate executable cutovers.

## Removal and supersession

On implementation this replaces RFC-0110 modes, preserving M0 repair intent from
RFC-0113/0114/0116/0118/0119/0120. Their historical evidence remains intact.
Reject or revise the proposal if sound lexical references cannot support the
two M2 applications within the unchanged budgets; never weaken loan checks.
