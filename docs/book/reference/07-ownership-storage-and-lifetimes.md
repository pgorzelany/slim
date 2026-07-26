# Ownership, borrowing, storage, and lifetimes

This chapter specifies affine moves, exclusive borrows, frozen views,
compiler-selected regions, deterministic destruction, and the absence of raw
memory access in safe source.

## Affine ownership

`Bytes`, `Vec[T]`, `Arena[T]`, and aggregates that contain affine values have
one owning binding. Passing, returning, or rebinding may move ownership. Any
later use of the old binding is rejected.

The compiler does not repair a move by inserting an implicit copy or reference
count operation.

## Exclusive `inout`

An `inout` parameter borrows one named caller binding exclusively for the
duration of a call. The value cannot be moved from, returned, or retained by
the callee. A temporary is not a valid `inout` operand, and two simultaneous
borrow operands may not alias.

Tail recurrence preserves the linked `inout` controller instead of rebinding it
to a different value.

## Frozen byte views

Freezing establishes a view tied to the source storage. Operations that would
invalidate or incorrectly reuse the source are rejected. The relationship is
tracked without exposing a raw pointer.

## Regions

The compiler selects lexical allocation regions and deterministically destroys
owned storage when its valid region ends. Ownership transfer may move storage
to a valid enclosing region. Generated code retains allocation-site identity so
injected failure and cleanup remain reproducible.

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
