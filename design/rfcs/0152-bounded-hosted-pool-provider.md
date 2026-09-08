# RFC-0152: Bounded hosted pool provider

Status: proposed
Implementation: pending
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-08
DecisionDate: pending
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

Specify the concrete provider missing from RFC-0151: a thread-confined buddy
pool, reserved at the hosted authority boundary, with checked allocation and
infallible nonblocking release inside the pool. Freeze capacities and algorithmic
limits before native measurements. A bounded independent model tests allocation,
coalescing, failure and reuse; it does not establish a production allocator or
source lifetime checker. This proposal remains pending the coupled API and
worker integration decisions and native evidence.

## Motivation

The current runtime calls malloc during allocation and free during region
teardown. Calling free at every source cleanup boundary cannot establish the
required nonblocking release contract. A domain-local pool keeps release to
ordinary writes in memory exclusively owned by the execution scope. Whole-domain
physical teardown occurs only at the quiescent hosted boundary.

A segregated cache without coalescing can strand reusable capacity. Returning
blocks to libc while allocating complicates latency and failure observations.
A fixed buddy pool offers a finite split/coalesce bound and makes fragmentation
visible as allocation failure, rather than an unbounded search or fallback.

## Guide-level explanation

Only programs explicitly requesting allocator authority construct a pool.
The host reserves its backing storage before invoking the source entry with
that capability. Reservation failure is a hosted startup result; source allocation
failure after entry is ordinary recoverable data. Programs without authority
pay no pool reservation or metadata cost.

Released blocks become reusable by this domain immediately. They are not returned
to the OS until domain teardown. Report live payload, rounded live blocks, free
pool capacity, pool metadata, reserved address space and process RSS separately.
A lower live-owner count is not a claim of lower RSS.

## Reference-level specification

### Frozen candidate limits

The initial hosted default is 268,435,456 pool bytes (256 MiB), selected before
candidate measurements. A host may explicitly choose a power of two from 65,536
through 1,073,741,824 bytes. Reject malformed, non-power-of-two and out-of-range
configuration at the host boundary. This is a new experimental resource contract,
not a relaxation of any existing benchmark budget. Failed corpus admission means
the candidate needs another recorded decision; do not silently grow the default.

The allocation quantum is 64 bytes. There are at most 25 orders, from 64 bytes
to 1 GiB. A plain block reserves a 16-byte header; an owning-recursive block
reserves a 64-byte header including its cleanup continuation. The latter is used
only when the checked element layout needs recursive owning cleanup. No extra
continuation storage is charged to ordinary scalar containers. Measure both
header and rounding overhead. Header size does not specify the source layout of
an element.

The pool base and payload offsets are 16-byte aligned. Payload alignments are
powers of two no greater than 16; unsupported element layouts are rejected at
compilation for the declared target. The provider may reserve up to 15 additional
bytes for base alignment, retaining the original host pointer for teardown.
Charge that allowance and the actual domain-control structure separately from
the configured pool capacity. No source can access alignment slack or headers.

### Allocation and release

For a nonempty request, first validate length/layout arithmetic and require
`payload <= capacity - header` after checking `header <= capacity`. Round the
sum upward from 64 by at most 24 checked doublings. Request overflow, excess
capacity, injected failure and lack of a suitable free block return exhaustion
without modifying live allocations. Zero-length owners allocate no block and
retain the domain relationship.

Choose the smallest available order large enough for the request. Within that
order choose the lowest offset, split leftward, and insert right buddies. Each order has a hierarchical bitmap: a leaf bit denotes a free block; a bit
in each upper level denotes a nonzero word below it. Use 64-bit words, ceiling
division by 64 per level, and stop at one root word. An order-availability word
selects the smallest eligible order. There are at most four bitmap levels.
Minimum lookup descends through at most four words; insertion/removal updates
at most four words. A portable lowest-bit scan examines at most 64 bits per
word. Buddy membership is a direct checked leaf-bit lookup. A linear scan over
all free blocks is not permitted.

Zero all bitmap words at hosted initialization, then mark only the whole pool
free. Sum every order's word counts, including its summary levels, and add one
8-byte availability word. At the maximum pool this is 4,261,024 index bytes,
charged outside usable pool capacity. Reserve this index at startup; never
allocate index nodes during source allocation or release. Bounds and padding
bits must be checked before word addressing. Split and coalesce perform at most
25 bitmap updates (at most 100 hierarchy-word updates plus 25
availability-word updates) and 24 buddy tests. These bounds
exclude owned-element destruction and the portable within-word bit scan.
The model checks the index against ordinary sets at all level boundaries;
C pointer/arithmetic safety and target layout still need native verification.

Release requires a live uniquely owned block. Coalesce with a free buddy of the
same order, continuing for at most 24 merges, then publish the maximal free block.
No allocation, host call, lock, atomic synchronization, source callback or failure
occurs during a valid release. Invalid internal metadata is a runtime invariant
failure, not a source recovery mechanism or permission to access arbitrary memory.
The safe checker, rather than a raw address lookup, prevents stale-owner reuse.

The split/merge bounds follow strictly changing order. Headers and offset checks
must establish alignment, extent and free-list/index membership before access.
Neither the small model nor wall-clock samples establish this native invariant.
A type-specific RFC-0151 cleanup driver destroys initialized members before
releasing their backing block; allocator release is not a substitute for that
ownership trace.

### Scope, workers and abnormal exit

Allocator capability copying does not permit concurrent pool mutation. Each
structured task needs a disjoint child domain and budget, including inline
fallback. The child reservation schedule, quota accounting and post-join owner
adoption must be identical for spawned and declined tasks. Do not share this
pool through a lock or silently turn all verified parallel sites into serial
ones. Fix the child partition/adoption protocol before acceptance; it is not
proved by the single-domain model. Preserve both existing parallel-runtime gates.

Normal physical teardown requires no live owners/borrows and quiescent workers.
At a program trap, report the trap and terminate the process without walking and
freeing another worker's live pool. Successful normal joins still occur exactly
once. A fail-fast path is not a successful source cleanup or resource close.

## Compiler and runtime design

This proposal implements no production path. The normal selfhost checker must
supply domain, layout, initialization and cleanup facts before codegen can use
this provider. The C runtime is the audited provider boundary; Rust or Python
models never accept SLIM source, supply semantic fallbacks or authorize emission.

`scripts/models/m2_pool.py` is verification-only. Its candidate split/merge
model is compared with an independent occupied-cell oracle which enumerates
maximal empty aligned intervals. Every compared state partitions the entire
pool into non-overlapping live and free extents. Reverse release must restore
the initial pool even after failed allocations and reuse.

## Compatibility and migration

Use the RFC-0148 runtime ABI cutover with the complete RFC-0149–0153 contract
package. Keep region-backed old Bytes valid until the owned-Bytes migration.
No current source allocation adopts this provider merely because its model passes.
The model is not shipped as a runtime dependency.

## Diagnostics and failure cases

Name invalid configuration, unsupported alignment, admission exhaustion, invalid
internal metadata, illegal domain escape and unsafe worker-domain sharing
separately. Default evidence remains conservative. Exhausted model work limits
exit unsuccessfully with an explicit unknown result; they do not truncate a
passing campaign. A repeated raw offset is not proof of a valid source owner.

## Performance and complexity

Freeze native series before implementation: pool capacities 64 KiB, 1 MiB and
256 MiB; payload sizes around every power of two through each capacity minus
both header sizes; alternating small/large allocations; reverse and interleaved
release; and 10,000 steady-state allocate/release cycles. Inject each allocation
ordinal in each finite trace. Preserve returned-value and container-state oracles
separately from provider occupancy.

Record reservation cost, split/merge/index work, payload/header/rounding bytes,
peak live storage, retained free capacity, RSS and native code size. Mandatory
compiler work and all existing native ratios remain gates. No measured cost,
no-block native result, allocation throughput or fragmentation benefit exists yet.

## Alternatives and drawbacks

Libc per-owner release lacks the required contract. A growable pool hides a
capacity change and can block on a source allocation path. A fixed pool can
exhaust despite adequate aggregate free bytes because of fragmentation; its
reservation also adds a hosted startup cost. Deterministic minimum selection
adds index metadata and startup zeroing work even when few blocks are live.
These costs and unresolved points are reasons to test, not positive ratings.

## Test and acceptance plan

The frozen small model has eight abstract quanta, request sizes
0/1/2/3/4/5/8/9, success/failure injection for each allocation, release of every
live allocation and an invalid-release witness. Explore all reachable state
transitions through depth four, deduplicating equivalent states. Hard limits are
20,000 states and 250,000 transitions; the explicit command may select depths
1–6 and lower budgets. Exceeding any limit fails with unknown. Separate arithmetic
cases cover capacity/header boundaries and mathematical I64/U64 extremes.

Before acceptance, complete the worker partition/adoption contract and source
API package. Before adoption, compare the C provider with the oracle, exercise
alignment and both actual header layouts under ASan/UBSan, inject startup and
allocation failures, prove release has no host/allocator calls, measure all named
series, and pass the complete compiler/library/application and release gates.
No eight-quantum result proves arbitrary native heaps or lifetime safety.

## Ratings and evidence

All ratings and score are zero. The current region implementation is inspected
behavior. Model results are bounded evidence within their explicit domain;
production safety, resource cost and performance remain unknown. Keep any model
counterexample and native failure in the compact evidence archive.

## Decision

Proposed under RFC-0112. Worker integration and the source API package remain substantive
acceptance blockers. No source syntax, runtime ABI or production behavior changes.

## Implementation

Verification model only; native provider pending. The model is a development
oracle, not a conformance implementation or completed M2 deliverable.

## Removal and supersession

Reject or revise this candidate if fixed capacity, bounded nonblocking release,
worker isolation or unchanged budgets cannot be met. Preserve its measurements
and counterexamples. Never replace a failed native check with model agreement.
