# Ownership, borrowing, storage, and lifetimes

This chapter specifies affine moves, exclusive borrows, frozen views,
compiler-selected regions, deterministic destruction, and the absence of raw
memory access in safe source.

## Affine ownership

`Vec[T]`, `Arena[T]`, and aggregates containing either have one owning binding.
Passing them to a plain parameter, returning them, or rebinding another name
may move ownership. Any later use of the old binding is rejected.

The compiler does not repair a move by inserting an implicit copy or reference
count operation.

`Bool`, `U8`, `I64`, `Id[T]`, `Bytes`, and aggregates containing only copyable
members are copyable. `Bytes` copies are immutable views of one
compiler-retained backing region rather than independent owning buffers.

## Exclusive `inout`

An `inout` parameter borrows one named caller binding exclusively for the
duration of a call. The value cannot be moved from, returned, or retained by
the callee. A temporary is not a valid `inout` operand, and two simultaneous
borrow operands may not alias.

Tail recurrence preserves the linked `inout` controller instead of rebinding it
to a different value.

The borrow exists so a callee can inspect or mutate an affine owner while the
caller retains ownership. It ends at return and cannot be stored as a value.
It adds no allocation, owner, reference count, or runtime lifetime object.

## Frozen byte views

Freezing establishes a view tied to the source storage. Operations that would
invalidate or incorrectly reuse the source are rejected. The relationship is
tracked without exposing a raw pointer.

## Allocation

Scalar evaluation, aggregate construction, moves, borrows, projections,
matching, and view copies do not allocate dynamic storage by themselves.
Vectors and arenas allocate or reallocate backing storage when growth requires
capacity. `bytes.freeze` transfers an existing vector buffer into a view
relationship without allocating a second buffer.

The `alloc` effect is a capability ceiling for possible direct or transitive
allocation. It does not assert that a block is obtained on every execution.

## Regions and physical release

The compiler assigns every dynamic block to one lexical region. An allocating
function with no storage result and no `inout` output uses a child region,
which is destroyed on normal or allocation-failure exit. A storage/view result
or `inout` output uses the caller-provided destination region. Allocation-free
functions elide an otherwise empty child region.

Regions destroy their blocks in reverse allocation order. The root region is
destroyed at process shutdown, and trap cleanup destroys the active region
chain. Generated code retains allocation-site identity so injected failure and
cleanup remain reproducible.

Source liveness is narrower than or equal to physical allocation lifetime. The
checker can reject use immediately after a move or overwrite, while the current
function-granularity implementation may retain the backing block until its
region ends. Per-binding early release is not currently promised.

## Checked access

Byte, vector, and arena operations use typed built-ins. Indexes remain checked
unless exact positive evidence applies to the same operation. Safe code never
exposes unchecked indexing.

## Parallel ownership

Structured workers require isolated allocation regions and checked capture
shapes. Ownership transfers only after join. Unknown captures or work remain
serial.

## Deliberate boundary

There are no raw pointers, pointer arithmetic, untracked aliases, tracing
garbage collection, implicit copies, hidden synchronization, user-written
lifetime parameters, or general shared mutable references.

## Normative boundary

[Memory](../../MEMORY.md) is the maintained ownership and allocation contract;
[Parallelism](../../PARALLELISM.md) adds worker-specific rules.
