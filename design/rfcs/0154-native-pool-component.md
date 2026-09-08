# RFC-0154: Native pool component

Status: accepted
Implementation: complete
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

Implement and test RFC-0152's single-domain block provider as an independently
compiled native runtime component. This supplies native evidence needed to decide
the coupled memory package. It does not activate allocation authority, replace
the current runtime, change source syntax or claim M2 adoption. Source integration
still requires accepted ownership, lifetime, cleanup, provider, capability, worker
and host contracts and their migration evidence.

## Motivation

The small Python oracle cannot establish C bounds, alignment, overflow behavior,
actual storage accounting or the absence of host allocation during release.
Those require executable native code. Waiting to implement every source consumer
before testing its allocator would leave the memory contract untested at the
RFC-0112 pre-syntax boundary. A separately verified provider component is a
prerequisite, not a second compiler or a semantic fallback.

## Guide-level explanation

The component owns one fixed pool and its preallocated bitmap indexes. An
internal C caller requests raw blocks, initializes their contents and returns
them exactly once. Source users cannot call this interface. The normal SLIM
checker and future cleanup lowering will remain responsible for source ownership,
reference validity, initialized elements and deterministic destruction order.

Current generated programs continue using runtime ABI 1. They neither link this
component nor reserve a pool. Component tests explicitly compile its C source;
its files will become production runtime inputs only at an accepted integration
cutover, with complete artifact-identity and ABI changes.

## Reference-level specification

### Fixed layout and limits

Retain the predeclared RFC-0152 capacities: 64 KiB through 1 GiB, powers of two;
default candidate 256 MiB; minimum block 64 bytes; at most 25 orders and four
hierarchy levels. Do not change these after seeing native results. Pool and data
addresses are 16-byte aligned. Valid requested alignments are 1/2/4/8/16.

A plain header is exactly 16 bytes: uint64 generation, uint32 requested bytes,
uint8 order, uint8 kind, and uint16 magic. The recursive kind reserves 64 total
header bytes; its additional 48 bytes are initialized but not interpreted by this
component. They belong to the pending RFC-0151 cleanup driver. Releasing a raw
block never destroys its elements or executes a callback.

Bitmap storage includes an 8-byte order-availability word, followed by every
order's leaf and summary words. Use RFC-0152's exact ceiling-division formula;
maximum storage is 4,261,024 bytes. One hosted allocation holds these words,
up to 15 alignment bytes, and the pool. The caller owns a separately measured
control structure. Account for index, alignment, control, rounded live blocks,
requested live payload and reusable free bytes without double counting.

### Internal C protocol

The private runtime header exposes a zero-initialized pool control type, a block
handle and operations to initialize, allocate, release, inspect and destroy.
Initialization validates configuration before calling the host allocator. It
rejects a live destination and leaves an empty destination empty on failure.
There is exactly one hosted reservation attempt for a valid initialization.
The caller must not copy a live control structure or use it concurrently.

Allocation receives uint64 payload bytes, alignment, plain/recursive kind and an
output handle. Invalid alignment/control yields an internal invalid result.
Excess size, full capacity, exhausted attempt identity and injected failure yield
exhaustion. Failed allocation leaves the output handle and all live blocks
unchanged. A zero-size successful result has no block, consumes no attempt and
has no release obligation; its source-domain relationship belongs to the caller.

Every valid positive allocation request consumes one monotonic attempt identity,
including capacity failure. Identity zero is reserved. At UINT64_MAX no further
positive request succeeds; never wrap. A configured positive failure ordinal
rejects exactly that attempt and does not poison later requests. A successful
block header records this identity. The handle records its offset, generation,
requested bytes, order, header kind, payload address and usable capacity.

Choose the smallest adequate available order, then its lowest free offset.
Split to the requested order. Update bitmap ancestors only while word emptiness
changes. Each index update/lookup visits at most four hierarchy words. An order
selection uses the availability word, never a scan over free blocks. A portable
lowest-bit operation examines at most 64 bits. Split/coalesce performs at most
25 index updates and 24 buddy tests. All shifts and addresses have checked
order, word, bit, alignment and extent bounds.

Release verifies the handle's in-pool aligned extent, header size/kind, live magic,
generation, requested size and accounting before modifying state. A valid release
cannot fail. The internal C API reports invalid handles to its caller; this is
an invariant error, not a source-level fallible destructor. Clear the live header,
coalesce same-order free buddies and publish the final free block. No host call,
allocation, lock, atomic operation, source callback or unbounded recursion occurs
on this valid path. OS scheduling/paging is not a real-time guarantee.

Handles and the control structure are trusted C objects created by this component,
not accepted serialized input or hostile native code. Generation checking detects
ordinary stale/duplicate handles; it does not replace source ownership proofs or
promise safety after arbitrary native memory corruption. No checker may infer
source validity from a plausible native header.

Destroy is a hosted operation. It refuses live blocks and frees the reservation
only after a valid empty state. Destroying an empty uninitialized control is
idempotent. Root/child lifetime checks, worker quiescence, pool adoption and
source trap teardown are obligations of the later integration contract; this
component alone cannot certify them.

## Compiler and runtime design

Use `runtime/slim_pool.h` and `runtime/slim_pool.c`, without changing the current
`slim_rt` build inputs or generated C. No Rust compiler/runtime semantics and no
new dependency are introduced. Existing source and native artifact identities
stay unchanged while the component is unreferenced. Integration must add these
files to every consumed runtime identity before linking them into source programs.

Tests compile the same C implementation normally and with ASan/UBSan. Sanitizer-only annotations
poison released payloads and unused tails, retaining initialized headers for
stale-handle checks. Deliberate use-after-release and one-byte overruns must
produce the expected sanitizer diagnostics at the probe source. This supplements
ordinary accounting checks; it does not prove all possible pooled accesses safe. Optional
work instrumentation is compile-time test instrumentation; report its layout and
time separately from the ordinary component. Hosted malloc/free wrappers observe
real calls and inject initialization failure. They do not supply allocator
semantics or replace release with a fake provider.

## Compatibility and migration

This is a child implementation experiment of RFC-0152, whose source/worker
adoption remains proposed. It resolves no outstanding language decision. Preserve
the current seed, runtime ABI, schemas, installed behavior and release gates.
The future source cutover may use the verified component only after accepting
the full memory package; neither these C entry points nor the test command are
new source-language operations.

## Diagnostics and failure cases

Distinguish invalid configuration/handle from ordinary exhaustion. Check zero,
misalignment, oversized integer requests, corrupt generation/kind/order/size,
duplicate release, exhaustion and retry, live-pool destruction and repeated
initialization. Test both exact and out-of-range attempt/failure identities.
A test failure blocks component completion. Any source/native safety claim
outside the tested domain remains unknown.

## Performance and complexity

Preserve RFC-0152's frozen native series and split/merge/index budgets. Measure
ordinary control size, header sizes, index bytes, allocation attempts, rounded
and requested live bytes, reservation/operation time and RSS. Compare with an
identified current-region control recorded before implementation; microbenchmark
ratios are observations, not application speedups or portable budgets.

Ordinary allocation/release have fixed provider work bounds independent of live
block count. Initialization zeroes the bounded index; physical teardown is a host
operation. Full ownership destruction remains outside this block provider and
must be measured separately. No compiler/runtime performance budget is relaxed.

## Alternatives and drawbacks

Continuing with only a set-based model leaves native hazards untested. Linking an
unaccepted memory package immediately would cross the source/worker boundary.
The component stage adds a private C interface and temporary integration work;
its value is native evidence and reusable runtime code, not a milestone label.
It may still be rejected if integration or unchanged budgets fail.

## Test and acceptance plan

Execute the frozen small-model traces through a production-sized 64 KiB pool:
map each abstract quantum to 8 KiB and subtract the chosen header from positive
payload requests. Compare every result, free extent and live allocation with
the independent occupied-cell oracle. This uses the ordinary native capacity
validator, not a test-only smaller heap. Preserve failure and retry behavior.

Also test actual 64-byte minimum blocks, both headers and every supported
alignment; all hierarchy boundaries; capacity and UINT64_MAX arithmetic; stale
handles and refused teardown; injected host initialization and allocation
failure; reverse/interleaved releases; and the frozen 10,000-cycle workload.
Run ordinary and sanitizer variants and observe zero host allocation/free calls
between successful initialization and empty teardown. Prove the valid release
call graph contains no host/provider callbacks by source inspection as well.

Component completion requires these native comparisons and measurements plus
all AGENTS.md pre-commit commands and website publication. Full memory adoption
still requires the compiler/library/applications and complete milestone release
gate. A component pass does not complete RFC-0152, M2 or M3.

## Ratings and evidence

All ratings and score are zero. Acceptance authorizes a bounded native runtime
experiment within RFC-0112; performance and source-safety benefit remain unknown.
The preceding oracle and baseline measurements are retained separately from
candidate results. Do not convert unknown evidence into favorable ratings.

## Decision

Accepted on 2026-09-08 under the maintainer's explicit RFC-0112 authorization
for detailed staged runtime decisions. Scope is the native block-provider
component and its verification. The unaccepted source/worker adoption boundary
is preserved; no policy or numerical performance exception is granted.

## Implementation

Implemented in `runtime/slim_pool.c`/`.h`, with the native probe, independent
Python replay and a permanent Cargo hook. The [native component report](../../benchmarks/results/2026-09-08-m2-native-pool.md)
records bounded comparison domains, sanitizer witnesses, host-call observations,
work/space measurements, negative timing costs and required verification.
This child completes the block component only; integration and adoption remain
unfinished under RFC-0152 and the complete M2 memory package.

## Removal and supersession

This supersedes no current source operation or runtime ABI. Remove the component
if the candidate fails its fixed bounds or integration requirements, preserving
its failing evidence. Keep the independent model and regression intent for any
replacement provider; never count an unused prototype as completed M2 adoption.
