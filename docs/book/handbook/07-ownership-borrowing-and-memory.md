# Ownership, borrowing, and memory

Affine ownership, default shared borrowing, explicit `@` exclusive borrowing,
and explicit `^` transfer prevent use-after-move, untracked aliases, escaping
mutable borrows, and nondeterministic destruction in safe SLIM.

## Three value categories

SLIM values are copyable values, copyable views, or affine owners:

| Category | Examples | What passing by value does |
| --- | --- | --- |
| Copyable value | `Bool`, `U8`, `I64`, `Id[T]` | Copies the value. |
| Copyable view | `Bytes` | Copies the immutable data-and-length view while retaining its checked backing lifetime. |
| Affine owner | `Vec[T]`, `Arena[T]` | Borrows read-only, borrows exclusively with `@`, or transfers ownership with `^`. |

Structs and enums derive their category from their members. An aggregate is
affine when any member is an affine owner.

## What a move means

An affine owner may be transferred at most once. Passing it as `^value`,
returning it, storing it in an aggregate, or rebinding another name can move
ownership. The move copies no buffer and performs no allocation. It only
changes which binding may use and eventually release the storage.

`^` specifically marks ownership crossing a call, `recur`, or consuming
built-in boundary. It accepts a whole named owner or a freshly produced owner.
Ordinary affine moves into a local binding, aggregate, variant, or owning
collection slot do not use `^`; their destination already states the ownership
change. SLIM does not currently support `^owner.field` or a branch-selected
existing owner because that would require partial- or conditional-move state.

After a move, the old binding cannot be used:

<!-- slim-fixture: use-after-move -->

Receiving an affine `^` argument does not mean the callee immediately destroys
it. The callee owns the value and may inspect, store, return, forward, or
discard it. Storage is physically released at its compiler-selected region
boundary.

## Why borrowing exists

A plain affine parameter borrows its caller read-only for the call. The caller
uses an ordinary argument and can call any number of shared readers afterward.
Shared arguments may alias because none can mutate or escape.

<!-- slim-fixture: shared-borrows -->

An `@` parameter borrows a named caller binding exclusively. The caller keeps
ownership, the callee may inspect or mutate the value, and the borrow ends at
return. Both boundaries are visible:

```slim
fn append(values: @Vec[I64]) -> Void effects[alloc]:
  vec.push(@values, 42)

append(@values)
```

<!-- slim-fixture: exclusive-borrow -->

The borrow is nonescaping and cannot be moved from:

<!-- slim-fixture: move-from-inout -->

An exclusive operand cannot alias another shared, exclusive, or owned operand
in the same call:

<!-- slim-fixture: inout-alias -->

Temporaries cannot be passed as `@`, and borrowed values cannot be returned as
owned storage. These constraints keep the lifetime local and statically
visible. Shared and `@` modes are parameter capabilities, not storable
reference types, and introduce no allocation or reference-count update.

## Frozen views

`bytes.freeze(^vector)` consumes one unique `Vec[U8]` and produces a copyable immutable
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

- temporary allocating helpers with no storage result and no exclusive output
  use a child region destroyed when the helper returns;
- returned storage, returned views, and `@` output use the caller's
  destination region so they remain valid after the call; and
- the root region is destroyed when the program exits.

Regions release their remaining blocks in reverse allocation order. The
current implementation releases ordinary buffers primarily at function-region
boundaries, not immediately after every last binding use. Source ownership can
end earlier than physical release, but never later than the backing storage's
valid region.

<!-- slim-fixture: lifetimes -->

## Exact rules and common errors

Safe code exposes no raw pointer, unchecked alias, hidden copy, or hidden
synchronization. There is no garbage collector or reference count.
Diagnostics point to the exact invalid move, borrow, or use. The complete
allocation and physical release model is specified in
[Memory](../../MEMORY.md).

There are no storable reference types, user-written lifetime parameters,
reference counting, or tracing collector.

## Next

Continue with [storage and allocation](08-storage-and-allocation.md).
