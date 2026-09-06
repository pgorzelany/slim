# RFC-0129: Bounded function flow view

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

Derive an ephemeral per-function normal-control graph from canonical checked SLIM.
Use typed source-node/block identities, explicit continuations, branch alternatives
and joins, lexical scope boundaries, call argument sequencing, and terminal recur.
This supplies the shared topology for subsequent M1 ownership/query migration.

## Motivation

The isolated function checker is available, but consumers have no bounded common
view of its canonical source's control structure. A derived view must preserve
source authority and may not become another parser, checker or input language.

## Guide-level explanation

Each block holds one source operation and its typed canonical node reference.
Edges show possible normal evaluation order; ordinal order is construction order,
not execution order. Calls evaluate arguments left to right, including eager
Boolean operators. Recur evaluates all arguments before a single simultaneous
transfer to function entry; it has no fallthrough edge. Ordinary calls remain calls.

## Reference-level specification

`flow.derive(source, tokens, checked, owner, budget)` requires the normal accepted
checker result and the same canonical token/source storage. It validates acceptance,
fact-vector length, the revision-owned declaration view, a nonempty exact function
extent, and a budget in 1..1,000,000 before construction. It is a compiler-owned API;
forged or deserialized checked facts do not establish its preconditions.

`Graph` stores its owner, budget, consumed task steps, explicit Status, blocks and
edges. Status distinguishes Complete, Bounded, InvalidSource and InvalidBudget.
A Bounded graph is a partial explanatory artifact, never a complete control result.
Consumers must require Complete. No existing language acceptance depends on this
optional view or acquires a new source-size ceiling.

A nominal BlockId contains a declaration identity and local ordinal. Each Block
contains an Operation, identity.NodeId and checked/source argument mode. Edges use
BlockIds and distinguish Next, Alternative, Join, Recur and PotentialAbrupt. Block
resolution requires a complete graph and exact owner/range equality. Entry, Return
and Abrupt blocks reference the function's canonical node. Other blocks reference
exact expression, binding or arm nodes; no new accepted source representation exists.

Use an explicit work stack, with continuations encoded as temporary block indices.
Processed tasks, block count, edge count and pending task count are independently
bounded by budget. Check before every append/step. Reuse popped task-vector slots;
C recursion depth must not depend on SLIM source nesting. All counter arithmetic
stays within the fixed domain before addition. Construction visits each structural
child once and is approximately linear in the checked declaration size.

Bindings evaluate initializers before binding and body, then leave lexical scope.
Statements retain their canonical synthetic binding form but are labelled Statement.
Assignments evaluate their RHS before mutation and continuation. Field projections
and aggregate/variant values preserve child evaluation order. Match/if evaluates
its scrutinee before a branch, enters and exits each payload scope separately, and
joins alternatives. No branch is pruned. A recur bypasses all ordinary continuations.
Recur itself marks iteration exit and the parameter transition; a future ownership
consumer must carry the checked transition rather than reset availability at its
back-edge. Parallel regions retain enter/join boundaries and their serial source order; the
existing checked worker plan remains the authority for selected/executed workers.

An Argument block retains the source's explicit borrow/move mode, and a Bind block
retains the normal checker's binding mode. Operations reference canonical nodes so
clients can resolve existing type, binding, place and ownership facts. This view
does not infer those facts or approve transfers. Typed persistent semantic handles
and orchestration of the M0 ownership rules over the view remain required M1 work.

Call blocks have a PotentialAbrupt edge to the abrupt exit. Its stable meaning is
`call-outcome-not-resolved-by-flow`: it conservatively preserves possible checked
traps/allocation failure, not proof that a hazard exists or an event occurred.
Complete means exact structural normal source topology for this supported canonical
grammar, not feasible-path reachability. Path conditions and callee outcomes are
not solved by this view;
it does not certify complete hazard analysis, ownership, termination or parallel
execution. Calls' children retain their own ordering and abrupt edges. The graph
cannot discharge an effect capability or remove a checked hazard.

## Compiler and runtime design

Implement in `selfhost/flow.slim` using existing vectors, syntax accessors, identity
resolution and checked facts. No parser, Rust semantics, runtime change, dependency
or public command is added. The normal checker remains the sole semantic producer.
Regenerate the seed and exercise the actual production module through a SLIM probe.

## Compatibility and migration

No source acceptance, diagnostics, C generation, runtime ABI or native analysis row
changes. The initial operation is available to subsequent compiler queries; default
checking does not build an unused optional graph. This child does not complete the
parent's ownership-orchestration or retained-query obligations.

## Diagnostics and failure cases

Wrong extents, missing checked facts and rejected source return InvalidSource.
Invalid budgets return InvalidBudget. Exhaustion returns Bounded explicitly and
cannot resolve blocks as complete. Allocation exhaustion preserves status 71.
Unsupported canonical forms do not silently become ordinary calls or atoms.

## Performance and complexity

Measure actual construction steps, output sizes, budget boundaries and geometric
source families. Record opt-in view allocation cost separately from ordinary check
cost. Existing ordinary work/performance gates remain intact. No physical scratch
reclamation beyond the current caller-region rules is claimed.

## Alternatives and drawbacks

A second parsed IR would duplicate source authority. Recursive derivation could
consume unbounded native stack on nested source. A mandatory dataflow fixed point
would need a separate measured compatibility contract; none is introduced here.
Single-operation blocks use more storage than basic blocks but make exact source
operation boundaries and future grouping explicit without adding semantics.

## Test and acceptance plan

Compare exact expected paths for branches, eager arguments, scopes and recur.
Exercise all accepted conformance/native sources, deterministic repeat derivation,
node/edge ownership, stale block handles, exact extents, missing/rejected facts,
budget boundaries and deep/geometric source. Compare ordinary and sanitized probe
execution and 512 bounded allocation-fault ordinals. A measurement-only native
observer counts derive entries, walk entries/loop headers, result statuses and
returned step counts, with checked saturation at 1,000,000,000. On successful
probe runs, native walk headers must equal reported task steps plus walk entries;
repeated observations and ordinary output must match. The observer uses fixed
checked C anchors and is never installed in the compiler/runtime. Preserve all required checkpoint gates.

## Ratings and evidence

Analysis +2 supplies shared explicit topology with checked limits. Other dimensions
are zero pending measurements; weighted score 15. No ownership or reuse claim is
inferred from this representation alone.

## Decision

Accepted under the maintainer's RFC-0112 delegation and explicit M1 completion
request, following RFC-0123 and RFC-0124. This is not independent external review.

## Implementation

Implemented in `selfhost/flow.slim` with a production SLIM probe and separate
native observation hooks. All 93 accepted conformance/native files produce valid,
deterministic bounded views. Permanent checks cover exact evaluation paths,
recurrence, revision ownership, rejected/missing facts, invalid extents, and budget
boundaries. A geometric binding family through N=4,000 gives exactly 3N+4 blocks,
3N+2 edges and 2N+1 task steps. The 512-ordinal ordinary/sanitized fault campaign
observes 71 failures, including 34 after task walking begins, and 441 successes.

The preceding recursive checker has a confirmed baseline sanitizer stack limit at
N=4,000; that diagnostic measurement uses a recorded larger process stack. The
ordinary deep test and unchanged standard sanitizer corpus pass. This child does
not claim that all checker traversal is iterative.

Bootstrap reaches 3,687,073 C bytes, SHA-256
`86eadc2e41200ad66cd64f5497bb81c706e7ecaa248b6d4706077aa4b58e8a46`.
All 10 unit and 65 integration tests, 332 conformance fixtures, 2,000 malformed
mutations and required checkpoint gates pass. The dated
[progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
records work, fault, timing and storage evidence. The parent remains pending for
ownership orchestration, persistent semantic identities and retained queries.

## Removal and supersession

Any replacement must preserve canonical-source authority, evaluation order, terminal
recurrence, explicit uncertainty, budget/stale-handle regressions and all M1 gates.
