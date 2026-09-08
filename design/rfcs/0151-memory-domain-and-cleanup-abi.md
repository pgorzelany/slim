# RFC-0151: Memory-domain and cleanup ABI

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: Codex, pursuing SLIM Next M2 prerequisites
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

Propose the runtime contract for explicit allocation domains, per-owner cleanup
and recoverable exhaustion. The source-language parts require the accepted
ownership/byte children and a separate concrete allocation-API child; this runtime
RFC cannot admit source types or operations by avoiding language-feature review.
The concrete RFC-0152 provider and RFC-0153 source API are now drafted.
Their worker/host integration and native adoption evidence remain prerequisites.
No ABI or production memory behavior changes under this draft.

## Motivation

Current SlimRegion owns a linked allocation list; logical moves do not remove
blocks and release normally waits for the selected region. The sticky shared
allocation status unwinds allocating calls to exit 71. This cannot implement
ordinary recoverable insertion that returns its unconsumed value. A direct
recursive destructor would also make recursive owning containers depend on
unbounded native-stack depth.

RFC-0149/0150 establish ownership and loans before prompt cleanup. The runtime
must preserve those facts without discovering aliases, copying owners, trusting
proof text or allocating hidden cleanup stacks.

## Guide-level explanation

Hosted entry supplies memory authority explicitly. Storage remembers which
domain can release it; that authority remains live while any dependent owner or
borrow can be used. Returning an owner transfers its release obligation, not
the allocator. Empty containers need no backing block but retain their domain
relationship. Clone/copy and capacity growth remain explicit allocating actions.

Ordinary destruction releases storage at a specified boundary and has no user
callback, recoverable error or source blocking operation. Exhaustion is an
ordinary result. Hosted startup or an application can deliberately convert an
unhandled failure to exit 71; the allocator no longer poisons all later operations.

## Reference-level specification

### Domain authority and source prerequisites

The source API child must define an opaque Allocator capability, explicit hosted
entry parameters, and domain-lifetime positions for Vec['d, T], Arena['d, T]
and Bytes['d]. The domain lifetime is distinct from an owner's actual lexical
lifetime and from references stored inside it. All must remain valid at every
use; a short-lived element reference need not outlive the entire allocator,
but it must outlive every usable owner that contains it.

Only the hosted boundary initially constructs allocator authority. Source cannot
forge it from an integer, inspect provider internals, destroy it early or create
an unchecked provider. Multiple sequential allocations may use the same domain;
the source API must not hold an exclusive allocator borrow for each owner's
entire lifetime, which would prevent creating a second live container. Specify
the authority's permission separately from ordinary mutable-data aliasing in
that language child. Do not smuggle interior mutation through a normal shared
reference rule.

A domain is confined to its execution scope. Workers use separate child domains
under the existing structured-worker rules. A parent may adopt completed child
storage only after the child has joined; no concurrent linked-list mutation or
allocator sharing is introduced. A declined spawn executes the identical task
and ownership transitions inline. New borrowed-capture or execution shapes still
need their own measured parallelism decision.

### ABI contract

The first incompatible runtime cutover advances ABI 1 to 2 under RFC-0148.
These are proposed C boundary roles, not source-callable raw-memory functions:

| Role | Required representation / behavior |
| --- | --- |
| Domain | Opaque pointer to stable provider state, with no source-forgeable representation; lifetime/permission checking is static. |
| Allocate | Direct runtime entry receives domain, checked byte size and alignment; returns success+pointer or exhaustion, with no global sticky failure state. |
| Release | Direct runtime entry receives the original block/domain metadata; consumes that block exactly once and returns no failure. |
| Vector/arena owner | Buffer pointer, I64 length/capacity, checked element layout and domain identity; source element type determines generated operations. |
| Bytes owner | Buffer pointer, I64 length, original capacity/allocation metadata and domain identity; no mutable source access. |
| Slice | Typed pointer and I64 length; no owning release field, refcount or runtime lifetime extension. |
| Cleanup continuation | Provider-owned block metadata sufficient for bounded-stack destruction without allocating a cleanup stack; inaccessible to source. |

Alignment must be supported by the selected provider and declared target.
Check length/capacity, element-size multiplication, header addition and alignment
rounding before allocation or pointer arithmetic. Source checked integer overflow
remains a trap. A representable request the allocator cannot satisfy, including
its checked capacity limit, returns exhaustion without modifying an existing
container. Unsupported type layout/target alignment is a compile-time diagnostic.

An empty owner has length/capacity zero and no block; its domain still must be
valid. No malloc(0), pointer arithmetic on null, or element cleanup occurs.
Freezing transfers the exact block without allocating, shrinking or cloning.
Cross-domain copies are explicit fallible operations; a move alone cannot
retag a block's provider or defeat its lifetime relationship.

### Failure-atomic growth

Evaluate the container reservation and incoming value once in source order.
Check arithmetic and obtain a replacement buffer before changing length,
capacity or ownership of existing elements. On exhaustion, return the incoming
owner in the source result's failure branch; the container's length, capacity,
element order, bytes and ownership remain unchanged. No leaked temporary or
sticky status prevents a subsequent smaller allocation from succeeding.

On success, relocate initialized representations without invoking clone or
destruction, append the incoming owner, publish the new metadata and release
the old raw buffer. This relocation is the specified growth work, measured
separately from semantic copying. Old element storage is no longer initialized
and must not be destroyed a second time. Checked loans prohibit an operation
that could invalidate a live element reference, even when spare capacity exists.

The source API child must enumerate fallible constructors, reserve/push/arena
insertion, bytes.copy and allocating host adapters, including concrete result
enums before generic Result exists. No compiler-recognized magic Result or
hidden failure propagation is allowed. The runtime returns status; the ordinary
checker controls result ownership and exhaustive handling.

### Cleanup order and stack use

At normal lexical exit, destroy initialized owners in reverse lexical binding
declaration order. Reassignment/reinitialization does not change that binding's
position. Path-dependent initialized owners use the checked flags from RFC-0149;
an unavailable source-use state alone does not mean there is nothing to release.
Within structs, reverse field declaration order; within
the active enum case, reverse payload order; within vectors/arenas, reverse
initialized-element order, then the backing block. Shared references do not
release their referents. Moved owners are absent from cleanup.

For assignment, finish the right-hand value, install it and release the old
value under the checked reservation. For return, evaluate/transfer the result
before cleaning remaining locals. For recurrence, evaluate all arguments in
source order, transfer the next iteration's owners, clean the rest and reenter
without native-stack growth. Later break/continue/return syntax must supply
equivalent checked exit edges before being accepted.

Recursive owning containers require a nonrecursive cleanup driver. Use intrusive
continuation storage in already allocated owner blocks: a pending parent remains
allocated while its initialized children are destroyed. A compiler-generated
type-specific step describes the next child or completed block; the driver
advances a LIFO chain without allocating. Generated cleanup steps are audited
runtime metadata, not user destructors, closures or source dynamic dispatch.
Charge all continuation/header bytes to the allocation and report the indirect
dispatch/step cost. Programs without owned recursive storage allocate no recursive
cleanup continuation metadata. A simpler common layout is not permission to
charge unused features a runtime cost.

Every step either advances a finite initialized field/element cursor, descends
to a uniquely owned child, or releases a completed block. Acyclic owning storage
follows from no partial/self-referential ownership or raw aliases; typed arena
IDs do not own their targets. Counter arithmetic is checked within the block's
validated extent. No fixed cleanup depth may silently leak storage or trap on
otherwise valid normal destruction. A candidate implementation must demonstrate
bounded native stack on a deep recursive-owner witness before adoption.

### Provider and abnormal-exit boundary

Release must be infallible and nonblocking under the named provider contract.
This does not imply a real-time bound or immunity to OS scheduling. Neither
wall-clock samples nor merely calling libc free proves the required contract.
Before acceptance, specify and audit the hosted provider, including whether
release returns blocks to a domain-local reusable pool and when physical pages
are returned to the host. Report live owned bytes, reusable retained bytes,
provider overhead and process RSS separately; logical release is not a claim
that the OS RSS immediately falls.

A provider needing locks, I/O, blocking close or allocation during source cleanup
does not meet this proposal without a separate explicit decision. RFC-0152 now specifies a fixed thread-confined buddy-pool candidate with startup
index metadata, explicit capacity limits and allocation-free release. Its bounded
model is not native evidence. Worker-domain integration and measured fragmentation
remain required before this package can be adopted. Do not select a default
capacity from a successful candidate after seeing its results.

On a defined program trap, normal cleanup is not promised. Hosted termination
must not free another running worker's accessible storage before terminating
the process. Specify the fail-fast/teardown ordering in the concrete provider
implementation and preserve trap diagnostics and exit behavior. Normal domain
teardown requires quiescent workers and no live owners/borrows. Effectful OS
handle closing remains M5; releasing memory is not evidence that a resource was
successfully closed.

## Compiler and runtime design

The normal SLIM checker produces initialization, domain and destruction facts;
selfhost/memory.slim retains the accepted plan, and codegen consumes it. The
64-value conservative region fallback may remain for optional reporting but
cannot authorize premature release or omit mandatory cleanup. Reusable plans
depend on all consumed ownership/copy/layout/domain/exit facts and runtime ABI.

The portable seed and generated C share this ABI; compiler host sessions/native
caches must capture its exact header/source identity. Compiler-owned source
bytes, typed records and native query storage need explicit lifetimes and reset
cleanup. Preserve M1's transactional publication, last-good selection and exact
owned-byte release on reset. A corrupt retained plan misses; it cannot invent
an initialized owner or destructor omission.

## Compatibility and migration

Stage provider/domain preparation first while retaining safe old lifetimes,
then the complete owned-Bytes cutover, then prompt cleanup, then recoverable
allocation APIs. Each stage has its own executable prerelease and matching seed.
No mixed ABI runtime objects or unchecked old Bytes escape survives a cutover.
Typed results replace exit-71 propagation only when every allocating caller and
host adapter has migrated; intentional hosted exit-71 policies stay explicit.

## Diagnostics and failure cases

Required static families are domain escape, forged/missing authority, incompatible
domain transfer, unsupported provider/target, missing cleanup fact and unsupported
layout. Exhaustion is source data, not a successful allocation or a compiler
diagnostic. Corrupt internal ABI metadata must fail deterministically before
memory access; no source-visible raw pointer recovery is provided.

## Performance and complexity

Allocation/growth and cleanup costs are independent measurements. Cleanup is
linear in initialized owned members plus released blocks and uses bounded
native stack; provider release work must be specified separately. Measure header
bytes, fragmentation, retained free blocks, native code size and recursive-owner
depth series. Preserve every existing performance gate and both parallel-runtime
ratios. Unknown provider timing/space behavior cannot satisfy an acceptance bound.

## Alternatives and drawbacks

Region-only retention does not deliver prompt reusable storage. Recursive C
destructors risk stack exhaustion; an auxiliary allocating stack would make
cleanup itself fail. Intrusive cleanup metadata adds space and dispatch cost.
A local pool can retain physical memory and fragment; it must earn adoption
against existing same-host resource budgets rather than hiding those costs.

## Test and acceptance plan

Before acceptance, fix provider layout/capacity/work limits and the concrete
source allocation API through their child decisions. Freeze allocation/growth,
live-owner, recursive-depth and fragmentation workloads before implementation
measurements. Require independent ownership/release traces; normal return,
branch exit, assignment, recurrence, nested aggregate and worker cleanup; and
below/at/above every configured limit. Inject every allocation ordinal in each
bounded workload, including startup and failure while constructing a nested value.

Assert unchanged container state and returned incoming owner on failure,
successful reuse after failure, exact release once, stable freeze identity,
no allocation during cleanup and bounded native stack. Use ASan/UBSan plus
provider byte accounting; one does not substitute for the other. Preserve all
M0 regressions, M1 session/reset campaigns, conformance, all AGENTS.md commands
and the full clean release gate at M2 closure.

## Ratings and evidence

All ratings and score are zero. Current region and sticky-failure mechanisms
are exact inspected behavior; proposed provider safety, performance and costs
are unknown. The provider and source-API acceptance blockers are substantive,
not an invitation to implement against an unspecified ABI.

## Decision

Proposed. No standalone source-language acceptance is implied by its runtime
kind; all source changes need their accepted language children and policy gates.

## Implementation

Pending. Resolve provider and source-API child specifications before accepting
the complete memory package or starting its production implementation.

## Removal and supersession

When implemented, this replaces the active memory ownership/release/failure
contract from RFC-0025/0026/0043/0060 for the successor only. Preserve old seed,
runtime and failure evidence. Reject the candidate if nonblocking infallible
cleanup, safe allocation recovery or the unchanged budgets cannot be established.
