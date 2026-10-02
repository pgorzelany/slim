# RFC-0157: Native bounded host component

Status: accepted
Implementation: pending
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-08
DecisionDate: 2026-09-08
Approver: project-maintainer
Kind: runtime
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Implement the private C host component needed to test RFC-0155/0156: bounded
file/TCP input into caller-reserved buffers, fixed-scratch output, pool-backed
argument descriptors and fail-fast termination without pool traversal. Compile
it independently in native verification; do not link it into current generated
programs. Source signatures, effects, ownership and task eligibility remain
proposed under their children. This experiment does not complete M2 or M3.

## Motivation

The frozen host matrix needs real native I/O, failure and allocation evidence.
The current adapters grow storage and use buffered stdio. A model cannot show
whether their replacement preserves bytes on native failure, closes descriptors
or leaves running workers' storage intact. Private native component verification
allows those checks before the coupled source cutover.

## Guide-level explanation

An internal C caller supplies a validated byte view and mutable buffer descriptor.
The adapter receives no allocator for ordinary I/O and cannot grow the buffer.
Success publishes complete appended bytes; failure preserves its initialized
prefix, length, capacity and data identity. Only explicit argument preparation
receives a pool and may allocate descriptor storage. Source code cannot call
these private C functions or forge their pointers.

## Reference-level specification

Use `runtime/slim_host.h` and `.c`. Byte views contain const data and I64 length;
buffer views contain mutable data, I64 length and capacity. Native objects are
trusted caller-owned extents, not deserialized or hostile pointers. Check signs,
length<=capacity, SIZE_MAX/PTRDIFF_MAX representation, and non-null backing for
positive extents before arithmetic. A numeric extent alone is not proof that
an arbitrary C pointer owns that storage. The eventual source checker supplies
type, lifetime, disjointness and initialization proofs.

Retain RFC-0155's fixed 4,095-byte path bound, 46-byte numeric address buffer,
explicit I64 read/response bounds and timeout. Each native transfer is at most
1 MiB, additionally bounded by remaining capacity and the target's signed count.
Read into spare bytes; probe at most one extra byte at the limit. EOF plus
successful close precedes length publication. No heap scratch, response copy,
provider call or hidden retry allocation occurs. Retry interrupted read/write/
poll and transient send/receive operations within the existing deadline contract.
An interrupted connect may return ordinary failure, preserving the current rule.

POSIX descriptor support is enabled explicitly with `SLIM_HOST_POSIX` on audited
Darwin/Linux targets. Without it, file/TCP/output operations return false without
I/O or an allocating stdio fallback. No installed target is removed by this
experiment; source adoption still needs complete supported-target evidence.
Socket construction, numeric parsing, nonblocking connect, bounded send,
write-shutdown, receive and close retain the current protocol. SIGPIPE is
contained with the supported target mechanism and setup failure returns false.

The target close protocol performs one close attempt and never retries a
possibly reused descriptor. Linux documents early descriptor release even on
later close failure. Apple's inspected XNU `fp_close_and_unlock` releases the
descriptor before returning `fg_drop`'s status. These are named implementation
assumptions, not a claim about every POSIX version. No source cancellation,
guarded descriptors or foreign descriptor reuse is permitted in this component.
Test wrappers may inject a reported close error after an actual close; separately
record that this does not simulate every kernel implementation. Audit and
retain target identity before source adoption.

Output takes an internal descriptor, writes complete bytes with fixed scratch,
and returns success/failure. On Linux, a thread-scoped SIGPIPE mask surrounds a
nonempty write: preserve the old mask and any already pending signal, consume
only a newly generated synchronous EPIPE signal, then restore the old mask.
Darwin's inspected kernel pipe-write error signals the process, so that scheme
does not protect another unmasked worker. On Darwin, admitting an output
descriptor sets `F_SETNOSIGPIPE` idempotently and retains it; restoring a shared
descriptor flag around each write would race another writer. The native caller
must authorize this descriptor configuration. Source adoption must include it
in the hosted output-provider contract, including inherited descriptor effects.
Do not install a process-global ignored handler. Account for the target's
configuration/mask syscall cost. Require pending/mask preservation, broken-pipe
and combined broken-stderr/live-worker tests. Decimal I64 conversion uses unsigned magnitude,
including INT64_MIN, and at most 20 bytes. Println writes bytes then a newline.
Trap output writes the existing diagnostic prefix, message and newline directly
to stderr then calls `_Exit(70)`. It never flushes stdio, walks a pool, joins a
worker or claims normal cleanup. Failure to report on broken stderr still
terminates. OS teardown occurs after process termination; source normal exits
still need checked cleanup and successful joins.

Argument preparation receives an initialized pool, argc and native argv, and
an empty destination. Validate counts, individual lengths and descriptor byte
arithmetic; allocate one plain block for nonempty argc, then build borrowed byte
views without copying payloads. The host caller retains argv immutably until
normal argument release. Zero argc uses no block. Failure leaves the destination
unchanged. Return separate invalid/exhausted/success statuses to the hosted
caller; this is not a source result enum. Release descriptors only after all
source/worker uses finish. Empty prepared arguments still retain their domain.

Reuse the existing native monotonic clock, including its RFC-0075 saturation
contract, rather than add another clock service. Repair a violation of that
accepted contract separately as a current-runtime bug fix, with a retained
UBSan counterexample and boundary regression. The component itself introduces
no current source/ABI behavior change.

## Compiler and runtime design

No production SLIM compiler file depends on this component yet. Native tests
explicitly compile the same C, current clock/task runtime and pool sources.
All consumed source/target/flag identities belong in receipts. Future integration
must add the host files and target option to runtime artifact identities before
linking, and advance incompatible ownership ABI meanings under RFC-0148.
There is no Rust/Python semantic fallback or separately parsed program form.

## Compatibility and migration

Current generated programs retain their accepted adapters. The component's
completion is prerequisite evidence, not acceptance of proposed source borrowing,
effect removal, argv startup or reserved-buffer tasks. Only the complete accepted
memory/host package may perform that cutover, preserving all old failure intent.

## Diagnostics and failure cases

Distinguish internal invalid arguments, startup exhaustion, Bool I/O failure
and process trap. Cover negative/oversized bounds, null empty views, insufficient
spare capacity, embedded NUL, exact EOF/overflow byte, interrupted/short transfers,
descriptor failure and unsupported target. No ordinary I/O failure publishes
uninitialized bytes. Native input validation is not a source ownership proof.

## Performance and complexity

Freeze before/after native file/output workloads at 0, 1, 64 and 4,096 bytes,
1,000 repeated operations, five measured runs after a warmup. Measure buffer
preparation separately from adapter calls. Retain the current-region/stdio
control, identities, actual bytes, setup, runtime and object size. TCP timing
and both maintained loopback application ratios remain required at source/task
adoption; component timing cannot replace them. Preserve every existing budget.

## Alternatives and drawbacks

A fake provider alone cannot establish descriptor or worker behavior. Linking
proposed source semantics immediately would cross unaccepted lifetime boundaries.
The component adds temporary integration work and fixed path/transfer choices;
unbuffered small writes can cost more. Report that cost without an assumed win.

## Test and acceptance plan

Execute RFC-0155's frozen matrix through native temporary files and loopback
servers, plus injected syscall failures and exact output/descriptor observations.
Use ASan/UBSan, symbol/call inspection and pool allocation accounting separately.
Test argv payload identity and failures, output integer extrema, and a live
worker trap with a forbidden-free observer. Preserve the first deterministic
failure and untested target assumptions. Require all AGENTS.md commands and
website validation before component completion; full M2 release and source
acceptance remain separate.

## Ratings and evidence

All ratings are neutral. This authorizes bounded native implementation and
measurement, not a favorable source-safety/performance rating. Unknown target,
source and application properties remain unknown.

## Decision

Accepted under the maintainer's RFC-0112 authorization for detailed staged
runtime decisions. Scope is the independently verified native component;
no source operation, general host handle or performance exception is accepted.

## Implementation

Pending native implementation, measurements and verification.

## Removal and supersession

Supersedes no current source operation. Reject or revise the component if the
specified failure, storage, target or unchanged budget obligations fail. Preserve
counterexamples and regression intent for its replacement.
