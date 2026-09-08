# RFC-0153: Explicit allocation authority and reservation

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-08
DecisionDate: pending
Approver: project-maintainer
Kind: language
Primitive: allocation-authority
Evaluation: m2-experiment
EvaluationRFC: RFC-0147
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Specify explicit allocator authority and separate capacity reservation from
nonallocating insertion. Ordinary source wrappers then return unconsumed values
on allocation failure, including before generic Result exists. This removes a
circular prerequisite: the runtime need not recognize a magic result container
to support a source-library result. This is a proposed RFC-0147 experiment within
M2, not an accepted language feature. Hosted adapters and worker domains still
need their coupled contracts before implementation.

## Motivation

RFC-0151 requires recoverable insertion and a live domain relationship, but leaves
the source API unspecified. An exclusive allocator reference retained by every
owner would prevent two live containers. Mutating allocator state through an
ordinary shared data reference would contradict RFC-0149. Define an opaque
allocation capability instead, whose effectful operation is explicit.

A compiler primitive returning whichever enum a caller expects would privilege
an error-container shape. Reservation returning Bool, followed by checked
nonallocating insertion, lets a normal source function construct any declared
result enum. It also exposes growth separately from moving a value.

## Guide-level explanation

The hosted entry may request one `Allocator['host]` parameter; a zero-parameter
entry requests no authority and constructs no pool. These are distinct resource
requirements, not two spellings of allocation. `'host` is a reserved lifetime
introduced only by the hosted boundary. Helpers name a bound domain lifetime
and receive its capability explicitly.

`Allocator['d]` is an opaque copyable capability, not a reference to ordinary
mutable data. It has no fields, equality, integer conversion, user constructor
or destructor. Copies grant allocation authority in the same execution scope;
they do not extend its lifetime or permit cross-worker use. The `alloc` ceiling
exposes provider mutation. Safe source cannot inspect or mutate provider memory.

`Vec['d, T]`, `Arena['d, T]` and `Bytes['d]` retain that domain relationship.
Empty owners retain it even without a block. Stored element-reference lifetimes
are separate: they must outlive each usable owner containing them, not every
possible use of the allocator. A capability cannot make an escaping reference
safe or permit an owner to survive domain teardown.

## Reference-level specification

### Minimum storage operations

These signatures describe intrinsic type families; `T` is a specification
metavariable until the separate generic-language cutover. They do not accept
unchecked generic source bodies.

| Operation | Contract |
| --- | --- |
| `vec.new(allocator)` | Construct an empty owner in the capability's domain. No block or allocation event; element type comes from an explicit expected vector type. |
| `vec.len(&values)` / `vec.capacity(&values)` | Return initialized length / usable element capacity without mutation or allocation. |
| `vec.reserve(&mut values, minimum)` | Bool result, `alloc` ceiling. Negative minimum traps. Minimum at or below capacity succeeds without changing metadata or allocating. Growth failure preserves initialized elements, length, capacity and allocation identity. |
| `vec.push(&mut values, value)` | Consume the value and append within existing capacity, returning Unit. No growth or allocation; a full vector traps before modifying it. A named affine value uses `move`. |
| `arena.new`, `arena.len`, `arena.capacity`, `arena.reserve`, `arena.add` | Corresponding typed-ID storage operations; add returns the existing typed ID, and does not allocate. IDs retain existing checked access rules. |
| `bytes.freeze(move values)` | Transfer a Vec[U8] block into its immutable owner without allocation, shrinking or copying, under RFC-0150. |

Reservation is the only compiler storage-growth operation for each current
intrinsic collection family. Its library-first justification is the same as
existing intrinsic vector/arena storage: safe source cannot reallocate an opaque
backing block or relocate initialized affine elements with raw pointers. New
capacity reads expose a distinct resource fact, not another length spelling.
The arena family must be reassessed for source-library replacement when private
representation and generic nominal IDs are available; retain no permanent alias.

Growth requests exactly the checked bytes for the requested element count,
plus the provider's specified header. On success usable capacity is the whole
number of elements fitting in the returned block payload. Do not add a second
undocumented geometric growth factor. Element sizes must be positive in the
physical ABI; Unit has zero information but its storage contract needs its own
child decision. Length/size/header arithmetic is validated before allocation.
Relocate initialized representations exactly once, publish the replacement and
release only the old raw block; do not clone or destroy the relocated elements.

### Ordinary recoverable wrappers

A concrete source `push_item` function owns its incoming Item and exclusively
borrows the destination. It checks `len + 1`, calls reserve, and either performs
push then constructs `PushItem::ok(())`, or constructs
`PushItem::exhausted(move item)` without insertion. The enum and wrapper are
ordinary source declarations checked with all other code. Named caller values
transfer to the wrapper at the ordinary owning call boundary. Container reference
reservation and incoming expression evaluation retain left-to-right order.

The checked increment may trap at the source integer bound; unrepresentable
index arithmetic is not converted silently into an allocation result. A wrapper
requiring a recoverable length limit explicitly checks it first and returns a
source-defined failure case. An actual allocator request can fail even if total
free bytes appear sufficient; callers must handle the result.

Freeze a separate concrete wrapper/result pair for every migrated element type
before the cutover. Later generic source Result/Option and collection wrappers
replace those concrete families atomically under their child RFC. The compiler
never special-cases result variant names or treats a successful Bool alone as
proof for an unrelated checked append node. Optional trap analysis remains
unknown without a positive fact for that exact operation.

Explicit bytes copying is a source-library operation: create a byte vector in
the supplied domain, reserve its checked input length, copy through checked
reads/appends, and freeze. Its ordinary source enum carries allocation failure.
No extra compiler `bytes.copy` primitive is justified by this contract.

### Hosted adapters and workers

Every source-visible allocation in host adapters must use supplied authority or
caller-owned storage. File/path/response scratch, partial-read cleanup, OS failure
and output-capacity failure need explicit bounded contracts. Preserve each existing
host failure-state test; do not turn a partial native write into a successful
source append or lose the unconsumed source value.

A separate hosted-adapter child must enumerate those signatures, buffer bounds,
result enums and native calls before this package is accepted. In particular,
removing allocation from a function declaration is invalid if fopen/stdio or
scratch-buffer code still allocates behind it. Neither this proposal nor the
pool model proves a no-allocation or no-block property for existing host code.

Allocator capabilities and domain-bound owners are not implicitly Send. Existing
structured worker behavior needs the RFC-0152 partition/adoption decision; unknown
capture work or unsupported loans remain conservative. Preserve valid current
execution shapes and their regression ratios through an explicit migration.

## Compiler and runtime design

The sole selfhost checker links domain binders, capability arguments, container
origins, reference lifetimes and owning destinations to canonical nodes. It
rejects missing/forged authority, domain escape and cross-scope use. No Rust
semantics, implicit global allocator lookup, refcounting or shared mutable-data
exception implements this capability.

The runtime returns allocation status and an uncommitted block. Source reserve
commits only after the exact checked request succeeds. Builtin effect metadata
and generated code must agree about the nonallocating operations. Optional
analysis facts do not authorize storage mutation or ownership transfer.

## Compatibility and migration

Requires RFC-0148–0152, Unit, private representation and the hosted-adapter child.
Old automatically growing push/add are removed at the same executable cutover
as source fallible wrappers and capacity reservation. Do not ship both growth
semantics under a mode flag. Migrate all compiler, library, fixture and executable
documentation calls through the production transition compiler and new seed.

Existing `alloc` and `partial` ceilings remain during the first reservation
stage; the later effect/progress child owns their replacement. The fixed pool's
post-entry operations cannot block. Hosted startup and blocking I/O stay named
host boundaries, and the later `block` ceiling must cover them where applicable.

## Diagnostics and failure cases

Pin codes and spans for missing authority, unknown domain binder, invalid main
shape, forbidden capability construction/capture, escaping owner/reference,
negative reservation, append without capacity and incompatible element layout.
Typed allocation exhaustion remains source data. Keep malformed configuration
and unknown optional analysis separate from a source lifetime error.

## Performance and complexity

Freeze before/after compiler and library corpora, the concrete wrapper inventory,
reserve/growth traces and both substantial application oracles before measuring.
Record code size, allocation attempts, relocation bytes, live/retained storage
and frontend work independently. Helper/wrapper overhead and provider reservation
cost remain unknown; source savings do not establish an agent benefit. Retain
all current budgets and complete milestone gates.

## Alternatives and drawbacks

A magic generic failure enum would create another semantic authority. Keeping
implicit growth prevents explicit fallible library composition. Reservation plus
append needs more source code and retains a checked append hazard until its
exact precondition is proved. Copyable authority needs its own checked domain
and worker rules; it must not be described as an ordinary shared reference.

## Test and acceptance plan

Before acceptance, finish the hosted and worker contracts and freeze a concrete
wrapper inventory. Require domain/capability positive and negative matrices,
multiple simultaneous owners, empty-owner escapes, stored short element loans,
returned owners, borrowed allocator origins, and every forbidden worker capture.

For each concrete wrapper, inject every provider allocation ordinal and compare
full container state, returned incoming value and release trace against an
independent operation model. Cover no-growth success, growth, every rounding
boundary, nested affine values, returned failure then retry, and full-capacity
append traps. Run all retained-query, bootstrap, conformance, application and
release gates. A provider model alone proves none of these source properties.

## Ratings and evidence

All ratings and score are zero under the explicit RFC-0147 experiment marker.
Current intrinsic storage and allocating host scratch are inspected behavior.
Successor safety, compile/runtime cost and adoption benefit remain unknown until
production evidence exists. No negative quality score is inferred from unknowns.

## Decision

Proposed. This specifies the authority/reservation composition, while hosted
adapters, workers and concrete wrapper inventory remain acceptance prerequisites.

## Implementation

Pending. No production source capability, reserve operation or changed insertion
semantics is accepted by this draft.

## Removal and supersession

When implemented, supersede the corresponding implicit allocator/growing-insert
parts of RFC-0009 and RFC-0150's provisional compiler-copy implication. Preserve
all old failure and ownership witnesses. Reject or revise if explicit authority,
ordinary result composition or the unchanged performance gates cannot be met.
