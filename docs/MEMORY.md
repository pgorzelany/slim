# SLIM memory contract

Status: SLIM 0.9 current compiler and runtime contract

SLIM combines statically checked ownership with compiler-selected allocation
regions. Safe source has no tracing garbage collector, reference counting,
user-visible allocator, `free`, destructor, or raw pointer. The Handbook
explains the [programmer model](book/handbook/07-ownership-borrowing-and-memory.md);
this document owns the compiler and runtime boundary.

## Source facts consumed by memory planning

Values are classified as `none`, `view`, or `owned`. Scalars and typed IDs have
no dynamic storage identity. `Bytes` is a copyable immutable view. Vectors and
arenas are affine owners. Structs and enums derive the join of their members.

A plain parameter copies a copyable value or shares an affine owner read-only
for the call. An `@` parameter provides one nonescaping exclusive borrow. A
`^` parameter receives a whole affine owner across the call boundary. `^`
accepts a named owner or a freshly produced owner; it is not a partial-move
operator. `let` and `var` control rebinding and do not alter those ownership
categories.

The checker records:

- local, result, or numbered exclusive-borrow escape destinations;
- final required source uses of named values and disjoint owned fields;
- allocation-capable calls and their lexical order; and
- the narrowest conservative region containing every use and escape.

Unknown calls or bounded-away detail may lengthen a lifetime but never shorten
it. Detailed named-value planning is capped at 64 values per function; the
fallback uses the function boundary and records reduced precision.

## Allocation

Scalar operations, aggregate construction, moves, borrows, projection,
matching, and copying a `Bytes` view do not dynamically allocate by themselves.

| Operation | Current allocation behavior |
| --- | --- |
| `vec.new()` / `arena.new()` | Creates an empty region-associated owner; no element buffer is required yet. |
| `vec.push`, `arena.add`, output growth | Allocates or reallocates only when current capacity is insufficient. |
| `bytes.freeze(^vector)` | Consumes the vector and exposes the same buffer as `Bytes`; it does not copy or allocate another buffer. |
| String literals and process arguments | Produce views over process-region storage. |
| File and bounded network operations | May grow an explicit `@Vec[U8]` and may use operation-local scratch storage. |

The `alloc` effect is a checked capability ceiling, not proof that every
execution obtains a block. Allocation and growth commit atomically or report
the single typed exhaustion outcome.

## Region selection and release

Each dynamic block belongs to exactly one generated `SlimRegion`.

- An allocating function with no storage/view result and no exclusive output uses
  a child region destroyed on normal return and allocation-failure exit.
- A function returning storage or a view, or mutating through `@`, uses the
  caller-provided destination region.
- An allocation-free function creates no empty child region.
- Successful vector reallocation releases the replaced buffer after copying.
- Host scratch regions end with the operation.
- The root region ends at program shutdown; trap cleanup walks active regions.

Region destruction releases remaining blocks in reverse allocation order. A
move changes the source owner but does not allocate, copy the buffer, search a
heap, or update a reference count.

Source lifetime and physical release are deliberately distinct. Finishing the
last valid source use makes the value unusable immediately. The current runtime
usually retains an ordinary dynamic buffer until its function-selected region
ends. This conservative retention can release later than an ideal per-binding
plan, but never too early.

## Retained compiler representation

`selfhost/memory.slim` owns the typed `ValuePlan`, `AllocationPlan`,
`DestructionPlan`, `FunctionPlan`, and `Plan`. The checker builds this plan
after type linking and retains it in the checked artifact. Standalone and
project code generation consume the same retained plans in declaration order.
There is no source lifetime syntax, Rust-owned lifetime IR, or backend
reconstruction of effect declarations.

Allocation plans include every allocating built-in and user call declaring
`alloc`. Generated failure boundaries are present exactly when the retained
plan contains an allocation-capable site. Plan construction remains
approximately linear in function size plus bounded call-summary edges.

## Allocation failure

Every region points to a `SlimAllocStatus`. Exhaustion propagates through
functions declaring `alloc`, destroys active local regions, and is handled once
at the executable boundary with exit status `71`.

`SLIM_ALLOC_FAIL_AT=N` selects an allocation ordinal for deterministic tests.
It is runtime test infrastructure, not language surface. Conformance and
sanitizer tests cover successful allocation, every bounded injected failure,
cleanup, retained returned storage, `Bytes` views, aggregate moves, and
exclusive output mutation.

## Historical evidence

RFC-0025 defines compile-time storage planning, RFC-0026 defines typed
allocation failure, RFC-0043 records the retained plan, and RFC-0060 records
allocation-free region elision. Dated measurements and the earlier self-host
census remain available under Development evidence rather than in this current
contract.
