# RFC-0114: Call-argument loan lifetimes

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-05
DecisionDate: 2026-09-05
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 2
Score: 40

## Summary

Restore the existing borrow contract across nested argument evaluation. A
borrowed affine argument remains borrowed while later arguments are evaluated
and until its call ends. Nested evaluation cannot move or exclusively access
that owner incompatibly. Keep argument evaluation left to right and activate
the new loan after evaluating its argument.

This is an M0 repair under RFC-0112, not the successor's complete lexical
borrow checker. Field moves and longer-lived local aliases still need work.

## Motivation

The compiler accepted `observe(value, grow(^value))`: the first argument kept
vector metadata, and the nested second argument grew a transferred owner,
freeing the retained buffer. ASan confirmed an 8-byte heap-use-after-free.

The former overlap record remembered only the last call using each owner.
Nested calls overwrote an enclosing call's record. Builtin collection operands
also lacked the enclosing call's loan. A same-call-only check therefore missed
both nested moves and nested exclusive mutation.

## Guide-level explanation

Shared reads may nest. Independent owners remain independent. Mutation that
finishes before a later borrow remains valid. Copying a scalar argument does
not extend a storage loan to the surrounding call.

An affine argument passed for shared access remains unavailable for a later
nested exclusive operation or move until the surrounding call ends. An
explicit exclusive argument also prevents overlapping reads during that
interval. Compute a needed query before reserving the exclusive argument.

## Reference-level specification

Every analyzed call has its canonical node interval `[call, ast_next(call))`.
A previously recorded loan is active at a node exactly when the node lies in
that interval. Recording a compatible nested shared access retains the oldest
active enclosing loan. An expired record is replaced by the new call.

Preflight preserves the existing alias diagnostics. After argument evaluation,
resolve the result owner again, perform any ownership transfer, and register
the loan only for an affine argument. A copied scalar has no continuing loan.
Explicit transfers still invalidate their source and preserve same-call
shared/exclusive/owned overlap diagnostics.

Vector, arena, and host output operands use the same loan registration. Local
owning transfers, aggregate insertion, and freezing check pending loans before
invalidating a named owner. Named reads check pending exclusive loans.

An unknown owner origin remains conservative: a pending unknown shared loan
conflicts with a restricted access, and a pending restricted loan conflicts
with a new unknown shared origin. Shared/shared unknown accesses remain valid.
No missing owner identity is treated as evidence of independence.

## Compiler and runtime design

Reuse the checker-local binding table's existing call and mode fields. Two
reserved entries summarize unknown-owner and restricted loans. Ordinary
bindings begin after those entries; canonical source node identities do not
change. These are per-analysis records, not process-global runtime state.

For each known owner, compatible simultaneously active records form an
ancestor chain in the canonical expression tree. Keeping the oldest active
record is sufficient: all permitted nested records are shared, and its end
contains every nested loan's end. Once that outer record expires, its nested
records have also expired. Conflicting accesses are rejected before replacing
it. The same conservative lifetime argument applies to the two summary slots.

No argument-list snapshots, binding-table copies, ancestor scans, parsed IR,
production Rust semantics, new dependency, or runtime ABI is introduced.

## Compatibility and migration

The unsafe reproducer becomes E0349. Existing same-call negative diagnostic
fixtures retain their codes and spans. Five new positive loan cases emit C
byte-identical to the preserved compiler.

Two selfhost calls computed a token query after an explicit exclusive token
argument. Bind those query results before the call. The preceding argument
expressions were simple variable/reference accesses; this preserves their
runtime behavior and the relative order of observable operations.

The LZ4 decoder likewise reads a scalar byte before reserving its output vector
for `vec.push`. Its overlapping-match round-trip tests preserve the intended
copy behavior under the repaired borrow contract.

Five native challenges compute scalar call/recurrence arguments before
exclusive reservations, retaining the order of checked arithmetic and reads.
The implementation report records every changed analysis/resource baseline
row, including the existing analyzer's conservative loss of three total-site
facts after scalar controller binding. Complete blocker sets and execution
site counts are unchanged.

## Diagnostics and failure cases

Use E0349 at the conflicting argument or named access. Newly detected nested
conflicts stop dependent inference to avoid cascaded type errors. Retain the
existing recovery behavior for already diagnosed same-call conflicts.

Test nested moves, nested mutation, shared calls followed by nested mutation,
aggregate moves, collection builtins, freezing, unknown origins, and scalar
field accesses. Positive cases cover shared nesting, independent owners,
mutation before a borrow, scalar copying, and call-end expiry.

## Performance and complexity

Each active-loan lookup uses constant-time indexed node boundaries and binding
accesses. The table gains two constant-size entries. There is no per-argument
scan over earlier arguments or all live bindings. Existing result-origin
traversal remains shared with the normal ownership checker.

The current quick performance suite passes unchanged budgets, including the
owned-transfer normalized ratio. A nested-loan geometric fixture extends the
existing owned-transfer exponent ceiling independently of the original fixture.
Do not remove or relax either test to accommodate this repair.

## Alternatives and drawbacks

Rejecting every nested call would discard useful safe programs. Revalidating
only move flags after argument evaluation misses mutation without a move.
Restoring only the immediately previous call loses an older enclosing loan.
Copying all loan state at each nested call can become quadratic.

Unknown owner summaries may reject disjoint accesses whose independence is
not represented. The successor's place and lifetime facts should improve
precision while preserving these regressions. These call-local facts do not
establish the safety of arbitrary escaping references or partial field moves.

## Test and acceptance plan

Keep the ASan reproducer as a permanent production rejection fixture. Add
positive native execution cases and exact negative diagnostics, verify the
compiler against its own source, and preserve the complete bootstrap,
governance, conformance, malformed-input, Rust-test, performance, reduction,
parallelism, native-comparison, and agent-tool gates.

Run sanitizers on the positive domain and confirm that the original rejected
source emits no executable C. Record native latency separately from the
unmeasured claim of LLM productivity.

## Ratings and evidence

Safety +2: closes a reproduced native use-after-free and related pending-loan
violations. Compile 0: constant work is added, with scaling and cost gated.
Runtime 0: preserved positive sources emit identical C. Minimal 0: no new
language surface; internal state has a maintenance cost. Analysis +2: loan
lifetimes span complete argument evaluation. Dogfood +2: the repaired compiler
checks itself after two explicit argument-query migrations. Score 40.

## Decision

Accepted as a within-scope M0 implementation decision under the maintainer's
RFC-0112 authorization to implement full SLIM Next and measure its effects.
This records delegated implementation authority, not a separate maintainer
review of these details. No hard gate exception is requested or used.

## Implementation

Complete for the call-argument repair specified here. See
[the implementation report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
for the final verification, permanent nested-loan scaling, and remaining M0
ownership gaps.

## Removal and supersession

A successor place/lifetime checker may replace the interval records, but must
preserve the native UAF rejection, safe nesting cases, diagnostic contract,
and permanent scaling fixtures. Do not restore last-call-only alias tracking
or claim this repair completes the full ownership milestone.
