# Ownership, moves, borrowing, and lifetimes

Affine ownership and exclusive `inout` borrowing prevent use-after-move,
untracked aliases, escaping mutable borrows, and nondeterministic destruction
in safe SLIM.

## Three value categories

SLIM values are copyable values, copyable views, or affine owners:

| Category | Examples | What passing by value does |
| --- | --- | --- |
| Copyable value | `Bool`, `U8`, `I64`, `Id[T]` | Copies the value. |
| Copyable view | `Bytes` | Copies the immutable data-and-length view while retaining its checked backing lifetime. |
| Affine owner | `Vec[T]`, `Arena[T]` | Moves ownership and invalidates the old binding. |

Structs and enums derive their category from their members. An aggregate is
affine when any member is an affine owner.

## What a move means

An affine owner may be transferred at most once. Passing it to a plain
parameter, returning it, storing it in an aggregate, or rebinding another name
can move ownership. The move copies no buffer and performs no allocation. It
only changes which binding may use and eventually release the storage.

After a move, the old binding cannot be used:

<!-- slim-fixture: use-after-move -->

Receiving an affine argument by value does not mean the callee immediately
destroys it. The callee owns the value and may inspect, store, return, forward,
or discard it. Storage is physically released at its compiler-selected region
boundary.

## Why borrowing exists

An `inout` parameter borrows a named caller binding exclusively for the call.
It is used when a function needs temporary access without taking ownership.
This avoids copying a buffer or forcing the function to return the owner merely
so the caller can continue using it.

The caller keeps ownership, the callee may inspect or mutate the value, and the
borrow ends at return. The parameter declaration carries `inout`; the call
site remains an ordinary call with a named argument.

The borrow is nonescaping and cannot be moved from:

<!-- slim-fixture: move-from-inout -->

Two `inout` operands cannot alias:

<!-- slim-fixture: inout-alias -->

Temporaries cannot be passed as `inout`, and an `inout` value cannot be
returned. These constraints keep the lifetime local and statically visible.
`inout` is not another owning or reference type, and it introduces no
allocation or reference-count update.

## Frozen views

`bytes.freeze` consumes one unique `Vec[U8]` and produces a copyable immutable
`Bytes` view of the same buffer. Copying the view does not copy the buffer. The
compiler instead keeps its backing region alive for every valid view.

Using the consumed vector afterward is rejected:

<!-- slim-fixture: frozen-source-use -->

## Allocation and region destruction

Scalars, moves, borrows, aggregate construction, and copying `Bytes` do not
allocate dynamic storage by themselves. Vectors and arenas obtain backing
storage when they grow. The `alloc` effect permits allocation and exhaustion;
it does not promise that every execution allocates.

The compiler assigns each allocation to one lexical runtime region:

- temporary allocating helpers with no storage result and no `inout` output
  use a child region destroyed when the helper returns;
- returned storage, returned views, and `inout` output use the caller's
  destination region so they remain valid after the call; and
- the root region is destroyed when the program exits.

Regions release their remaining blocks in reverse allocation order. The
current implementation releases ordinary buffers primarily at function-region
boundaries, not immediately after every last binding use. Source ownership can
end earlier than physical release, but never later than the backing storage's
valid region.

<!-- slim-fixture: lifetimes -->

## Compiler guarantees

Safe code exposes no raw pointer, unchecked alias, hidden copy, or hidden
synchronization. There is no garbage collector or reference count.
Diagnostics point to the exact invalid move, borrow, or use. The complete
allocation and physical release model is specified in
[Memory](../../MEMORY.md).

## Deliberate omissions

There is no borrow syntax for general shared references, no user-written
lifetime parameter, no reference counting, and no tracing collector.
