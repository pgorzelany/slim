# RFC-0118: Enum match ownership and payload loans

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

An owning affine enum match consumes its named scrutinee owners. A match of
borrowed affine storage binds affine payloads as shared, read-only borrows,
retaining the origin loan throughout each payload's arm. Repair existing
match semantics without introducing partial moves or reference syntax.
Also restore assignment through existing exclusive parameters so permitted
updates after loan scope exit generate a write through the parameter pointer.

## Motivation

At c976c5c, matching an owned enum binds its vector payload as a new owner but
leaves the original enum usable. Growing that payload reallocates storage and
reading through the original enum reproduces a heap-use-after-free.
Matching an exclusive enum parameter also permits growing a value-copied payload
while leaving the enum's vector descriptor unchanged. The C backend materializes
payload values; it does not produce exclusive field references.
Positive scope-exit tests also expose invalid C for assignment to an exclusive
enum parameter: the backend assigns a struct value to its pointer. This must
write the referenced value, as the existing source assignment contract requires.

## Guide-level explanation

Matching an owned affine enum destructures and consumes it. Payload bindings
receive the selected case's ownership. The original enum cannot be used inside
or after the match. Distinct cases can each transfer their distinct payloads.
Copyable enums remain copyable and may be matched repeatedly.

Matching shared or exclusive borrowed enum storage reads its payloads. Affine
payload bindings are shared and cannot be mutated, transferred, or returned as
owners. Their origin cannot be replaced, moved, or accessed exclusively while
that arm is active. Scalar payloads copy without retaining a storage loan.
After the arm ends, the original borrow's permitted operations resume.

For exclusive field access, the successor place/lifetime work must provide an
actual checked place and suitable lowering. Treating a copied payload descriptor
as an exclusive reference is not an implementation of that contract.

## Reference-level specification

After checking a non-Boolean affine scrutinee whose existing borrow mode is
owned, apply the existing result-edge transfer traversal before checking arms.
Named owners and all possible named conditional-result owners are invalidated
in the enclosing move scope. Fresh results have no prior owner to invalidate.
Payload owners are then distinct bindings in their lexical arms. Existing
branch joins preserve consumption after a nested conditional match.

For a borrowed scrutinee, every affine payload binding has shared mode 1,
including when its source has exclusive mode 2. Register a shared loan of its
resolved origin for the canonical arm interval. Multiple payloads share the
same origin loan; unknown origins retain the conservative summary. A scalar
payload gets mode 0 and no storage loan. A case without an affine bound payload
does not reserve a loan. Pattern mutability does not turn a shared storage
borrow into an exclusive capability.

Reuse RFC-0114/0116 indexed loan records. Separate the explicit loan mode from
syntax argument markers in the internal overlap helper, so pattern mutability
cannot accidentally register an exclusive loan. The ordinary argument helper
continues to read its existing source marker.

For assignment destinations linked to an exclusive parameter, emit the existing
dereferenced binding place in both ordinary and counted-loop lowering. Local
destinations, temporaries, and function-result slots keep their existing forms.
Preserve right-hand-side evaluation, move checks, and allocation failure checks.
The counted assignment path must recursively lower its right-hand expression,
including conditionals and calls, rather than reference an unmaterialized
temporary. It uses the existing counted expression emitter and checked type.

## Compiler and runtime design

Implement in selfhost/typing.slim using canonical binding, arm, and move-scope
identities. Do not copy binding tables, add a persistent representation, duplicate
semantics in Rust, or change the runtime ABI. The current generated C payload
materialization is retained; ownership checking now constrains its legal use.
In selfhost/codegen.slim, use the existing binding-value emitter for named
assignment destinations; it already distinguishes exclusive parameters from
local values using checked links. Do not invent a second place representation.

## Compatibility and migration

Previously accepted enum alias invalidation is rejected. Consume an owned enum
when its payload must be mutated or transferred. A borrowed match supports
shared inspection. Actual exclusive payload places and explicit replacement
remain part of the successor ownership work.

General owned field projections remain a separately recorded M0 gap. This
repair applies the existing transfer traversal and does not claim that it
implements safe arbitrary partial moves or closes every affine destination.
Borrow escape, reinitialization, and projected extraction still require their
remaining checkpoint evidence before M0 closes.

## Diagnostics and failure cases

Use existing E0315 for uses of consumed names, E0347 for shared payload
mutation/transfer/escape, and E0349 for overlapping origin access. Keep exact
spans in conformance fixtures. Missing owner identity is not independence.

## Performance and complexity

Payload binding gains constant indexed bookkeeping per affine payload. Named
scrutinee transfer uses existing indexed moves; conditional results use the
existing result traversal. Preserve all earlier geometric fixtures and add a
many-case payload loan series under the existing 1.25 ownership check exponent.
Measure baseline/candidate native latency separately from unmeasured agent
outcomes. No runtime allocation, synchronization, or copy is introduced.

## Alternatives and drawbacks

Leaving the scrutinee live duplicates affine ownership. Treating copied
exclusive payloads as mutable references reproduces invalidation. Disallowing
all enum payloads removes valid shared and owned destructuring. General partial
move state and exclusive place lowering are broader than the current repair.
The lexical loan may outlast a payload's last read; last-use shortening requires
its own proof and representative need.

## Test and acceptance plan

Retain both native invalidation witnesses as production rejections. Cover
consumption inside/after a match, conditional owners and branch joins, borrowed
payload mutation, origin replacement and calls, unknown origins, nested payload
borrows, independent owners, scalar and empty cases, owned transfer, and scope
exit. Execute positive fixtures with ASan/UBSan and compare generated C.
Include exclusive assignment from a builtin, user call, conditional, owned
name, and enum constructor, plus counted-loop assignment coverage.
Run bootstrap, governance, Cargo, conformance/malformed-input, permanent scaling,
reduction, parallelism, native comparison, and agent gates. Preserve exact native
analysis/resource baselines or explain every changed row. Record bounded
allocation-fault injection and the remaining M0 gaps.

## Ratings and evidence

Safety +2 closes reproduced ownership invalidation. Compile 0 adds indexed work
without a speed claim. Runtime 0 adds no runtime mechanism. Minimal 0 introduces
no source form. Analysis +2 maintains ownership and loan scope across enum arms.
Dogfood 0 makes no unmeasured productivity claim. Weighted score: 35.

## Decision

Accepted under the maintainer's RFC-0112 implementation authorization and
request for validated checkpoint commits. This records delegated authority,
not a separate maintainer review. No hard gate is relaxed.

## Implementation

Implemented in `selfhost/typing.slim` and `selfhost/codegen.slim`, with the
portable seed regenerated to the verified 2,957,449-byte fixed point.
Twenty-four added conformance rows retain the two native invalidation witnesses,
exact ownership/loan diagnostics, valid payload transfers, scalar copies, and
exclusive assignment. A dedicated integration test verifies that the counted
assignment fixture executes the specialized three-stage lowering.

Bootstrap, governance, formatting, Clippy, all 7 unit and 54 integration tests,
280 conformance fixtures and 2,000 malformed-input mutations, quick performance,
quick reduction, parallelism, resources, quick native comparison, agent checks,
and quick parallel runtime pass. ASan/UBSan and bounded allocation-fault checks
cover the compiler and positive fixtures. The complete analysis reports for all
20 native challenges are byte-identical to the retained pre-repair compiler.

The permanent enum-match scaling exponent measures 0.421 against the unchanged
1.25 ceiling. The same-host paired frontend measurements, exact fixture evidence,
allocation-fault bounds, and remaining M0 gaps are recorded in the
[enum checkpoint](../../benchmarks/results/2026-09-05-slim-next-progress.md#enum-match-ownership-checkpoint-2026-09-06).
No performance budget or analysis/resource baseline is relaxed. Completion of
this repair does not establish complete ownership safety or close M0.

## Removal and supersession

A successor place/lifetime checker may replace these records while preserving
the invalidation witnesses, exact move/borrow rejections, legitimate payload
transfers and scalar copies, and permanent scaling gates.
