# RFC-0141: Structured checking continuations

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

Move expression checking and its availability/loan orchestration onto explicit
structured continuations over canonical SLIM. Share the structural descriptions
with the optional function-flow graph. Preserve the existing semantic operations,
accepted programs, conservative joins, diagnostic ordering and checked facts.
The optional graph's reporting budget must not become a mandatory source limit.

## Motivation

At production checkpoint `49ea4a0`, `typing.check_function` owns isolated binding,
availability and loan scratch. Its inference helpers recursively select children
and resume semantic work. There are 21 direct calls to `infer_expr` in 16 functions,
including the function entry. Those call sites interleave inference with ownership
checks: replacing them with a pass over runtime-reachable graph paths would not
preserve the current checker.

`flow.derive` requires an already accepted `typing.Checked`. Its bounded result is
an explanatory normal-control graph; it cannot serve as a prerequisite for the
same acceptance. M1 requires shared control structure that actually orchestrates
checking, not an unused graph or a second ownership checker after acceptance.
The existing recursive checker also has a recorded sanitizer stack limit on deep
binding chains. Explicit continuations remove expression-dependent native stack
growth in the migrated traversal; they do not imply that every compiler helper
has become iterative.

## Guide-level explanation

The checker visits a canonical expression, requests its next child with the
expected type and lexical binding environment, and resumes the same semantic
operation when the child's result is available. A pending continuation records
that operation's current source node and remaining work. It is transient compiler
scratch, cannot be parsed or imported, and never supplies an accepted result on
its own.

Bindings evaluate their initializer before moving or borrowing its result and
checking the body. Assignments reserve the existing destination, check the new
value, and restore availability only at the existing successful transfer point.
Calls preserve left-to-right arguments and the lifetime of each earlier loan.
Branches check every source arm and retain the existing conservative join. Recur
checks every argument against the current parameters before the simultaneous
transfer; it does not run a fixed-point iteration of the checker.

## Reference-level specification

### Shared canonical structure

Add an internal `control` module depending only on canonical syntax and identity
support. Its local structural view names a half-open function extent in the
current token storage. Form descriptions distinguish atoms, bindings/statements,
assignments, calls, construction, projection, variants, branches, recurrence and
structured parallel bodies. They expose ordered canonical children and lexical
boundaries through the existing syntax accessors, without reparsing source or
copying the program into another accepted representation.

Descriptions are ephemeral values, not a second retained token tree. The normal
checker consumes them to choose child requests and continuations. The optional
`flow` materializer consumes the same form/child/boundary descriptions to build
its typed blocks and edges. It still requires accepted checked facts for semantic
annotations and retains its existing bounded/invalid statuses.

Local positions may exist inside one checking invocation. They cannot escape as
cross-revision handles. The public internal flow boundary continues to require a
validated `identity.View`; converting a local position to a `NodeId` checks its
owner and declaration-relative ordinal. Do not fabricate a revision for clean
scratch or weaken the existing retained-query ownership checks.

### Continuation machine

The inference entry retains the current source, tokens, facts, issues, binding
store, availability scopes and frames as its sole working state. It additionally
owns a reusable vector of pending frames and a live depth. Push reuses a popped
slot before extending the vector; returning a result resumes the top live frame.
No pending frame contains its own copied binding, token or fact vector.

A child request contains its canonical expression, expected type and lexical
binding parent. Function parameters and return type are invocation context.
Pending variants distinguish at least initializer/body, assignment-value/body,
argument-before/argument-after, aggregate member, projection, scrutinee,
arm-entry/arm-result, call completion, recurrence completion and parallel-body
completion. A frame stores only context/results required by its remaining phase.
The implementation may use tagged compact records rather than a wide enum when
measurements demonstrate a lower cost; tags and field interpretations must be
documented alongside the producer and consumer.

Each expression finishes through the existing `finish_type` operation exactly
once if the old entry would reach it. If `issues_block_inference` prevents entry,
return `invalid_type(expr)` without inventing a finish event. Existing enclosing
operations still perform the same post-child work, even after a child rejection.
Do not replace those rules with a global early exit or sort diagnostics after
the fact to conceal a changed checking order.

### Ownership and diagnostic sequencing

Preserve these concrete transitions:

| Operation | Required sequence |
| --- | --- |
| Binding/statement | Check initializer with its existing expected type; report stored/discarded Void; determine source ownership; perform result move or lexical loan; install binding; check body; combine validity; finish enclosing type. |
| Assignment | Resolve destination and mutability; reject outstanding destination loan; check RHS while a moved destination remains unavailable; check storage transfer; restore availability only with valid type/transfer and no new issues; check body; combine validity. |
| User-call argument | Check mode/capability and prior overlap; enforce existing recurrence identity when applicable; infer allowed argument; validate transfer; register the completed argument's loan only under the existing success conditions; advance to the next argument. |
| Built-in argument | Preserve that built-in's current arity gates, expected types, collection reservation, operand order and conditional evaluation of checker operations. Eager source Boolean operators remain eager. |
| Match/if | Check scrutinee and control kind; perform owning scrutinee transfer; begin the branch with every source arm; resolve each ordered pattern/payload; begin its availability arm; check its body; end its arm; compare results; end the branch. |
| Replacement | Validate and check the actual place; reserve its root before checking the replacement; preserve the existing exact-type and transfer gates. |
| Recur | Preserve ordinary argument validation plus exclusive-parameter identity and the current owned-argument rule; retain earlier argument reservations through later evaluation; complete the simultaneous parameter transition without following a runtime back-edge. |

The existing indexed availability algorithm remains the state-join operation.
Untouched arms retain entry state. Every syntactic arm contributes under the
current conservative rules, including an arm whose normal flow ends in recur.
No reachability refinement, conditional move feature or more permissive borrow
rule is introduced. Existing unknown-owner and restricted-loan summaries remain
independent of known-root reservations.

Shared lexical lifetime descriptions name the canonical call, binding or payload
arm and its checked half-open extent. Activation remains after the same semantic
success point as today. Expiration follows the corresponding scope boundary;
nested arguments must not clear an outer reservation. Source intervals remain a
valid compact implementation of these lexical lifetimes, provided the checker
and graph obtain the boundary from the shared structural description.

### Bounds and termination

The machine does not follow a recurrence back-edge and performs no mandatory
fixed point. Each child request descends into a proper canonical child, each
sibling advance moves forward to a distinct canonical member/argument/arm, and
each frame's finite phase sequence progresses toward completion. Explicitly
document the phase order for each frame variant. This is a structural termination
argument for orchestration, not a termination claim about the checked program.

The number of simultaneously pending expression frames is bounded by the current
function's canonical node count. Check vector depth before append and reuse
released slots. Avoid calculating an overflowing multiple of the source size to
establish a work bound: each finite phase has its own monotonic state, and each
child/sibling transition has a checked source bound. Node-based capacity is an
internal invariant of valid canonical structure, not a new configurable language
size ceiling. Allocation failure retains the existing status-71 boundary.

Optional graph task/block/edge limits remain exactly the RFC-0129 reporting
contract. A large checked function does not fail merely because a separately
requested graph would be bounded. Existing ownership/result-origin scans retain
their separate complexity obligations; do not count repeated scans as single
continuation steps to claim linear total checking.

## Compiler and runtime design

Implement in production SLIM, principally `control`, `typing` and `flow`.
Reuse the existing availability and semantic predicates. No new source form,
primitive, runtime ABI, dependency, semantic Rust path or persisted proof authority
is introduced. Clean and retained checking call the same inference entry.

Migration can replace one expression family at a time, with its old orchestration
removed when the new producer is installed. Temporary mixed traversal is explicitly
incomplete: it cannot establish complete stack independence or complete ownership
orchestration. Do not retain two selectable checkers, a semantic fallback, or a
permanent mode that accepts source through an unmigrated alternate path. This RFC
is complete only when all expression families, branch/loan transitions and the
optional graph use the shared structure and pass the full acceptance plan.

## Compatibility and migration

Preserve source acceptance and rejection, exact ordered diagnostics and current
spans, complete checked facts/links, memory/range/parallel evidence and generated
C. Retained parsing/typing/planning/range/emission keys and their trust boundaries
remain intact. Allocation counts and compiler size may change and must be measured;
runtime behavior of compiled programs and allocation failure status cannot change.

## Diagnostics and failure cases

No new source diagnostic is introduced. Preserve skipped inference after blocking
issues and the post-child diagnostic sequence. An invalid internal continuation
cannot supply facts or publish a good snapshot. Malformed source is rejected by
the existing parser/shape/checker pipeline, not accepted by a permissive cursor.
Fault tests must distinguish successful completion from status-71 termination and
verify that a failed session never publishes a partially resumed candidate.

## Performance and complexity

Record continuation entry, push, resume, high-water depth and complete/failed
inference work with separate native observation. Counters have an explicit checked
cap and never appear in normal timing binaries. Measure geometric binding,
assignment, nested-call, branch, payload and recurrence fixtures and nested mixes.
Count source scans and existing ownership work independently.

Pair uninstrumented old/new frontend and retained-session latency, allocation,
peak process memory, compiler/seed size, external C compilation and native program
behavior. Preserve the pre-change compiler as the differential baseline. A
reproducible regression outside the measured noise band blocks adoption under
FEATURE_POLICY; this RFC grants no budget exception or assumed speedup.

## Alternatives and drawbacks

Running ownership only after accepted typing is circular if typing depends on
those checks. Removing ownership from typing and independently rechecking a graph
would duplicate semantic authority and change error ordering. Making the existing
bounded graph mandatory adds an unsupported source limit. Thin wrappers around
the old calls do not establish shared orchestration. Explicit continuations add
state-machine code and dynamic storage; implementation and measurement must show
that those costs are contained before closure.

## Test and acceptance plan

Compare complete checker results, ordered diagnostics, facts/links, full analysis
and raw C against `49ea4a0` for the accepted/rejected corpus and malformed-input
campaign. Retain the independent bounded branch-action oracle and extend it with
assignment restoration, lexical/call/payload loans, owned/exclusive recurrence
arguments, unknown origins and nested constructs. Cross every frame phase with
positive, negative and diagnostic cases, including a child error followed by
enclosing cleanup/diagnostics and prior errors that prevent child entry.

Verify shared structural descriptions and typed graph edges against independently
expected branch, eager argument, lexical and recur paths. Keep exact/beyond optional
budget and stale-owner cases. Demonstrate bounded native expression-check stack
depth after the complete migration; identify other recursive helpers separately.
Exercise continuation capacity/reuse, ASan/UBSan and bounded allocation-fault
campaigns over cold, unchanged, changed, rejected and recovered session candidates.

Run bootstrap, governance, Cargo, all required checkpoint performance/reduction/
parallelism/comparison/agent commands and every applicable durable gate. Preserve
the complete session differential matrix and native application baselines. Record
costs, limits and identified checkpoint hashes in the M1 progress report. This
child does not complete public sessions, global-analysis/native caches or M1's
integrated release closure.

## Ratings and evidence

Analysis +2, weighted score 15, specifies actual shared orchestration and its
observable sequence. Other ratings remain zero pending measured implementation.
The 21-site audit establishes the current direct expression-call boundary, not
proof that all transitive recursion or ownership scans have been removed.

## Decision

Accepted under the maintainer's RFC-0112 delegation and explicit full M1 goal,
following RFC-0123/0124. This is an in-scope implementation contract, not independent
external review, completed implementation or permission to relax hard gates.

## Implementation

Pending. The baseline is `49ea4a0`, with the closure ledger at `17373e5`.
No expression family has migrated under this RFC yet. Shared source descriptions,
all continuation phases, ownership/loan integration and full validation remain.

## Removal and supersession

A replacement must preserve sole canonical/checker authority, complete diagnostic
sequencing, conservative joins and lexical loans, recurrence transitions,
source-relative traversal bounds, retained-query validity and durable measured
work/performance gates. Do not restore duplicate recursive and iterative checkers.
