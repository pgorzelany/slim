# RFC-0116: Local borrow loan lifetimes

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 35

## Summary

Extend the existing checker-local loan records to the lexical lifetime of
local affine borrows. This is an M0 prerequisite for safe explicit field
replacement, not a new reference syntax or a complete place/lifetime checker.

## Motivation

The production compiler at d6eeede accepts a local vector alias of an exclusive
parameter, then permits growing that parameter before reading the alias.
ASan reproduces an eight-byte heap-use-after-free when growth reallocates its
buffer. Call-argument protection ends before the local alias's lifetime ends.

## Guide-level explanation

A local initialized from borrowed affine storage remains a shared, read-only
borrow. Its owner cannot be moved, mutated exclusively, or replaced while the
local remains in its lexical scope. Shared reads and independent known owners
remain usable. A copied scalar does not keep a storage loan alive.

After the borrowed local's scope ends, mutation is allowed again. This first
repair does not infer a last-use endpoint before lexical scope exit. Unknown
conditional origins remain conservative, as in RFC-0114.

## Reference-level specification

After checking a local initializer, resolve its existing borrow mode and
origin. For a valid borrowed affine initializer, register a shared loan whose
extent is the canonical local-binding expression. That extent contains its
initializer and continuation; registration occurs only after initializer
evaluation, so it does not retroactively prohibit earlier operations.

Reuse the same known-owner records and unknown-owner summary as call loans.
An active enclosing shared loan is retained. When initializer-local records
are superseded by the enclosing local scope, the longer local loan replaces
them. Canonical expression intervals are nested or disjoint, so the oldest
active enclosing record covers every compatible nested use and expires after
all of them. Mutually exclusive scopes do not create loans in sibling arms.

Affine assignment is a restricted access to its target. Reject it when the
target's owner has a live shared/unknown loan, using the same conflict query as
an owning transfer. Scalar assignment does not acquire this restriction.
Existing move, exclusive call, builtin mutation, freeze, and shared-read
checks continue to consume the same records.

## Compiler and runtime design

Implement in selfhost/typing.slim. Reuse canonical node boundaries and existing
Binding loan fields, without table copies, per-binding scans, a new parsed
representation, production Rust semantics, runtime instrumentation, or an ABI
change. The ordinary checker remains the sole acceptance authority.

## Compatibility and migration

Previously accepted alias invalidation is rejected. If a scalar query is all
that is needed, copy the scalar before mutation. Otherwise put the read borrow
in an inner lexical expression and mutate after it ends. No automatic copy or
inferred lifetime extension may hide a source storage operation.

Owned-field extraction remains a separate known gap: this change registers
only initializers already classified as borrowed. General partial moves,
explicit replacement, borrowed enum payload scopes, and ownership reinitialization
still need their own implementation evidence. Do not claim this repair closes
M0 ownership safety.

## Diagnostics and failure cases

Use E0349 at the conflicting restricted access or assignment target. Preserve
existing E0347 escape/shared-mutation and E0315 use-after-move diagnostics.
Unresolved owners retain the RFC-0114 conservative summary; absence of an
owner identity does not prove independence.

## Performance and complexity

Each local borrow performs constant indexed loan bookkeeping after the
existing result-origin traversal. No all-binding or earlier-argument scan is
added. Preserve previous scaling fixtures and add geometric nested local loans
and disjoint lexical scopes under the existing 1.25 ownership exponent ceiling.
Measure native compiler latency independently from unmeasured agent outcomes.
Programs have no new runtime operation or allocation from this checker repair.

## Alternatives and drawbacks

Rejecting all local aliases would remove valid shared access. Ending a loan at
the initializer's call reproduces the UAF. Per-use last-use analysis is broader
than this lexical repair and belongs to the successor place/lifetime work.
Per-branch table snapshots would introduce superlinear work. Conservative
unknown-origin summaries can reject independent accesses not represented by
the current checker; missing evidence must not enable mutation.

## Test and acceptance plan

Retain the ASan witness as a production rejection fixture. Cover direct and
nested aliases, exclusive calls, builtin growth, ownership transfers, affine
assignment, unknown origins, scalar copies, independent owners, shared nesting,
and scope exit. Execute the positive fixtures natively and with ASan/UBSan.
Check deterministic diagnostics and the permanent scaling fixtures. Run the
required bootstrap, governance, conformance/malformed-input, Cargo, performance,
reduction, parallelism, comparison, and agent gates before the checkpoint.

## Ratings and evidence

Safety +2 closes a reproduced native UAF. Compile 0 records added indexed work
without a speed claim. Runtime 0 adds no runtime mechanism. Minimal 0 changes
no source surface. Analysis +2 retains the complete lexical borrow extent.
Dogfood 0 makes no unmeasured productivity claim. Weighted score: 35.

## Decision

Accepted under the maintainer's RFC-0112 implementation authorization and
request to commit validated checkpoints. This is delegated implementation
authority, not a separate review of these details. No hard gate is relaxed.

## Implementation

Complete for local initializers already classified as borrowed. The production
checker rejects the ASan witness and seven additional invalidation fixtures;
eight positive native fixtures are formatter-stable, emit unchanged C, and
pass ASan/UBSan. Bootstrap, governance, all required checkpoint gates, and
248 conformance fixtures plus 2,000 malformed-input mutations pass.

Nested-local and disjoint-scope check exponents are 0.342 and 0.692 under the
existing 1.25 ceiling. The implementation report records paired native latency,
unchanged analysis baselines, exact allocation-fault coverage, and the remaining
ownership and formatter gaps. This checkpoint does not close M0.

## Removal and supersession

A successor place/lifetime checker may replace these interval records while
retaining the alias-invalidation witness, scope-exit positives, unknown-origin
rejections, and permanent scaling gates. New syntax cannot stand in for proof
that the underlying loan lifetime is enforced.
