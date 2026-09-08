# RFC-0150: Checked slices and owned bytes

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: Codex, pursuing SLIM Next M2 prerequisites
Created: 2026-09-08
DecisionDate: pending
Approver: project-maintainer
Kind: language
Primitive: checked-storage-views
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Replace region-dependent copyable byte views with immutable byte ownership and
explicit lifetime-checked slices. Element reads borrow rather than silently
copying affine values. This proposed contract requires RFC-0149's loan checker,
RFC-0151's memory-domain ABI and RFC-0148's atomic migration boundaries.

## Motivation

The current `SlimBytes` is only data/length; freezing consumes the vector but
returned views depend on conservative caller/root retention. Prompt per-owner
cleanup would be unsound if that representation were simply renamed. Generic
container APIs also need a read operation that works for affine elements without
the shallow-copy ownership problem identified in RFC-0109.

## Guide-level explanation

`Bytes['d]` owns an immutable byte buffer allocated in domain `'d`.
`&'a [U8]` borrows a sequence until lifetime `'a` ends. A string literal has
static backing and type `&'static [U8]`; obtaining an owned copy is an explicit,
fallible allocation. Freezing a byte vector transfers its allocation to Bytes
without copying. Borrowing a byte owner creates a checked slice without copying.

The same slice operations work for other element types. Reading an affine
element yields a reference; extracting it requires a replacement that leaves
the container initialized. Safe source never receives a raw pointer.

## Reference-level specification

### Canonical types and operations

`[T]` is an unsized slice referent and may occur only behind `&`/`&mut`.
It is not an owning collection, fixed array or standalone storable value.
Reference and lifetime syntax follows RFC-0149. Dynamic indices and lengths
remain I64 until a separate accepted USize migration; negative values trap.
The initial built-in Vec/Arena lifetime slots follow RFC-0151, independently
of later generic source declarations.

| Operation | Proposed checked contract |
| --- | --- |
| `vec.get(reference, index)` | Shared vector reference returns a shared element reference; an exclusive reference moved into the call returns an exclusive element reference. The result retains the input loan and storage reservation. |
| `vec.slice(reference, start, length)` | Returns a slice with the same shared/exclusive permission; consumes an exclusive reference value, while shared references copy. |
| `slice.len(reference)` | Returns length from a shared slice reference. Use explicit read reborrowing for an exclusive slice. |
| `slice.get(reference, index)` | Returns a reference to the checked element, preserving permission and backing loan. |
| `slice.range(reference, start, length)` | Returns a checked subslice with the same permission and a lifetime no longer than its input. |
| `bytes.freeze(move buffer)` | Consumes Vec['d, U8] and returns Bytes['d], transferring its buffer/domain; cannot allocate, copy or fail. |
| `bytes.view(reference)` | Shared reference to Bytes returns a shared byte slice with the borrow's lifetime. Bytes supplies no mutable view. |
| `bytes.copy(domain, slice)` | Explicitly allocates an owned byte copy; returns the RFC-0151 typed failure on exhaustion. |

Each name denotes one operation with a permission rule, not separately selected
overloads. Exact parameter/result signatures and owned result enum names must
be installed in the built-in ledger at implementation. Remove old `bytes.get`
and `bytes.len` in favor of `slice.get`/`slice.len` on the explicit byte view;
do not leave duplicate read APIs. `vec.len` remains the length of an owner,
while `slice.len` is the length of a possibly proper subrange. A copyable
element is read as `*vec.get(...)` or `*slice.get(...)`; affine dereference
does not become an owning read.

Exclusive-reference arguments may be explicit reborrows such as `&mut *r`.
Calls do not implicitly convert an exclusive reference to shared access.
`arena.get` receives the corresponding borrowing result contract and retains
its arena origin; typed IDs are not ownership or lifetime proof by themselves.

For a sequence of length n, index validity is `0 <= index < n`. A range requires
`0 <= start <= n` and `0 <= length <= n - start`; subtraction is performed only
after checking start. Validate element-size multiplication against SIZE_MAX and
the allocated extent before pointer arithmetic. Empty ranges at n are valid;
an empty view may use a null pointer and must not perform arithmetic on it.
One-past pointers are never dereferenced. Range failure is a defined trap;
recoverable parser errors remain ordinary source results after explicit checks.

### Storage reservation and ownership

Every element/slice loan also reserves its backing container allocation. While
that loan is live, reject moving, dropping, freezing, reallocating or replacing
the backing container, even if a particular push currently fits its capacity.
The checker must not make alias safety depend on hidden spare-capacity state.
An exclusive view permits element replacement/mutation only through its checked
referent. Other paths into overlapping storage remain unavailable.

Initially, dynamic indices and ranges from the same allocation overlap
conservatively, including two apparently different integer variable names.
Do not add split-at/disjoint-range inference or parallel execution shapes here.
Different known whole owners remain disjoint. Static backing is immutable;
forming a mutable reference to a literal is rejected.

Bytes is always affine, including an empty owner. Ownership never depends on
length or whether a storage pointer happens to be static. Freeze preserves
the vector allocation's capacity and allocator identity for later correct
release; it does not shrink the buffer. A byte owner cannot contain a borrowed
pointer masquerading as owned storage. Its only first-stage constructors are
freeze and explicit copy. Moving Bytes does not change its domain relationship.

### Failure and replacement

`mem.replace(vec.get(&mut values, index), replacement)` and the equivalent
slice place reserve the container/root while evaluating subsequent operands.
Bounds checking occurs before replacement evaluation, preserving left-to-right
trap/effect order. Failed replacement evaluation leaves the old element in place.
After success, the old value transfers exactly once and no uninitialized slot
is exposed. At this access cutover remove `vec.set` and install checked
element-place assignment, for example `*vec.get(&mut values, index) = value`,
together. No intermediate checkpoint accepts both store spellings. Assignment
releases the old element; mem.replace transfers it to the caller.

No generic clone hook, reference counting, implicit promotion of literal bytes,
copy-on-write, interior mutation or runtime lifetime extension is introduced.
The runtime memory package supplies typed allocation failure for bytes.copy.

## Compiler and runtime design

Canonical checked slice facts contain owner origin, element type, permission
and region. Bounds facts may remove a check only for that exact checked node;
unsupported arithmetic or exhausted range analysis retains the check. Lifetime
acceptance is mandatory and cannot rely on optional range precision.

The proposed C view is a typed data pointer plus checked I64 length; its region
is erased. Owned Bytes additionally retains the buffer allocation/domain needed
by RFC-0151. Empty views must not rely on undefined C pointer arithmetic. No new
ABI layout becomes active until the runtime child and full host migration pass.

Include element type/layout, permissions, lifetimes, origin maps, ABI and consumed
range facts in retained result dependencies. Edits to a provider's byte ownership
or returned lifetime invalidate unchanged callers and native artifacts.

## Compatibility and migration

First migrate reference and slice operations while old byte retention remains
valid. Then atomically replace old Bytes views across compiler source storage,
retained parse/query records, all library and host APIs, literals, arguments,
worker captures and result adoption. Reject an ambiguous view-to-owner conversion;
choose an explicitly checked borrow or an explicit owned copy instead.

The allocator and cleanup cutovers follow the resulting ownership graph.
Copyable aggregates containing old Bytes require individual reclassification;
mechanically changing only the Bytes name would preserve the wrong ownership.
Keep every returned-view, nested aggregate, reallocation and root-lifetime
regression with its migrated positive or negative expectation explained.

## Diagnostics and failure cases

Specify stable families for mutable literal borrow, invalid slice type position,
escaping element/range, conflicting storage reservation, affine copy attempt,
invalid freeze input and incorrect replacement place. Pin source and origin/use
spans before implementation. Runtime checks report operation and index/range/length
without exposing raw addresses. No inference from a missing origin permits access.

## Performance and complexity

View construction and freeze are constant work with zero buffer allocations and
copies. Access retains checked arithmetic and bounds unless exactly discharged.
Measure pointer/length/domain storage, empty owners, retained byte capacities,
explicit copy frequency and compiler input retention separately. Cold/retained
query and native benchmarks must include the owned-input migration's actual costs.

## Alternatives and drawbacks

Keeping copyable Bytes views forces conservative retention. Implicitly copying
all escaping views hides allocation and loses the zero-copy API. Runtime reference
counting adds synchronization/copy cost. Explicit views add source operations;
conservative range overlap can reject useful disjoint work. New precision needs
its own evidence and child decision rather than weakening the reservation rule.

## Test and acceptance plan

Freeze a 120-case bounds matrix: lengths 0–3 × five indices (-1, 0, n-1, n,
n+1, preserving duplicate-valued named cases) × three permission/backing modes
(shared owner, exclusive owner, static literal) × two operations (get/range).
For range use the named index as start and length 0; add a separate exhaustive
range matrix n=0–3, start=-1–4 and length=-1–4 (144 cases). Pin the oracle from
the inequalities above. Add I64 extremes and multiplication/extent overflow.

Cross element/slice loans with grow, drop, move, freeze, replacement and scope
exit; include affine elements and returned references. Verify freeze's exact
pointer/capacity/domain transfer, zero-copy behavior and release once under
ASan/UBSan. Compare all migrated compiler/library programs and cold/retained
queries. Run all required gates and unchanged resource/parallel baselines.
Static literal success does not prove dynamic-owner lifetime safety.

## Ratings and evidence

Ratings and score are zero. Current Bytes representation and shallow element
copying are exact inspected mechanisms; successor safety and costs remain
unknown until implementation evidence. The matrices specify bounded test
domains, not universal proof. Experimental acceptance requires the approved
policy and a measured adoption decision.

## Decision

Proposed; no compiler surface or ABI is accepted by this draft.

## Implementation

Pending. The ownership and allocator contracts must be accepted together before
changing Bytes representation; prompt cleanup follows the checked owner cutover.

## Removal and supersession

At cutover this replaces the active Bytes/view and collection-read portions of
RFC-0009/0013/0098 while retaining their safety and behavioral regressions.
Reject adoption if the whole corpus needs unchecked lifetime exceptions or
hidden copies to remain executable within the unchanged budgets.
