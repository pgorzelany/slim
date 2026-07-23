# D0069: Defer automatic parallel execution

Status: accepted
Kind: runtime
Primitive: none
Safety: 2
Compile: 2
Runtime: 1
Minimal: 2
Analysis: 2
Dogfood: 0
Score: 85

## Need

Core 1F now has a real safe application site, a deterministic non-overlapping
plan, and total-task failure semantics. That is necessary but not sufficient
to put threads in every produced binary. Execution also needs a portable worker
mechanism, capture/result lowering for all eligible types, bounded nested-task
behavior, and a target-relevant profitability gate.

The first portability probe failed before timing: the host C11 compiler does
not provide `<threads.h>`. POSIX threads are available and useful for a
reference experiment, but adopting them would make the permanent C11 backend
platform-specific. SLIM has no accepted runtime dependency or per-target worker
ABI that closes that gap.

The production compiler therefore remains serial at the Core 1F boundary. Its
report states `execution disabled` with reason
`no-portable-runtime-or-cost-model`; every site states that profitability is
unknown because target cost is unavailable.

## Alternatives

Unconditionally adopting POSIX threads excludes non-POSIX targets. Adding a
third-party scheduler creates dependency, initialization, binary-size, and
failure costs for a single demonstrated application. Emitting ad hoc worker
helpers only for the fixture would be benchmark-specific code generation.
Using a fixed iteration threshold learned on one host confuses a measurement
with a portable performance guarantee. Language-level task syntax would not
solve any backend or cost problem.

The accepted decision preserves the complete evidence and benchmark so later
work can cross the boundary without repeating research.

## Costs

Selected programs continue to execute serially and receive no speedup. In
exchange, ordinary binaries gain no worker runtime, startup, synchronization,
TLS, binary-size, or platform cost. Compilation and generated C remain
unchanged.

A permanent `parallel-runtime` benchmark compiles a manual POSIX fork/join
reference and its serial twin at geometric task sizes. It verifies identical
output and gates the two-million-iteration parallel/serial ratio at 2.00.
Because this is a reference, not production semantics, it is never linked into
SLIM output.

## Evidence

On the acceptance host, 200 zero-work fork/join pairs add about 4.2
milliseconds over the process baseline, approximately 21 microseconds per
pair. The same four-Boolean recurrence is still slower in parallel at 10,000
iterations per task, approximately even at 100,000, and faster at one and two
million. At two million iterations, the manual parallel/serial ratio is 0.761.

This proves both that a worthwhile site exists and that structural source-token
counts are not a portable profitability model. It also records the missing C11
thread facility rather than hiding it behind a POSIX-only production change.

Production execution may be reconsidered only after at least one accepted
portable or explicitly tiered worker ABI, general checked capture/result
lowering, serial fallback and join-failure tests, a bounded no-nesting policy,
and target-calibrated or profile-backed profitability evidence across more than
one substantial application.

## Removal

Do not silently enable execution, link a worker runtime, or reinterpret
`selected` as `executed`. Do not remove the manual reference or its budget.
Replace this decision only with a scored runtime decision satisfying every
listed reopening condition and the full release gate.
