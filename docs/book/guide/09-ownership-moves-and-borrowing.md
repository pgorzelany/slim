# Ownership, moves, borrowing, and lifetimes

Affine ownership and exclusive `inout` borrowing prevent use-after-move,
untracked aliases, escaping mutable borrows, and nondeterministic destruction
in safe SLIM.

## Affine values

Storage-bearing values are affine: passing, returning, or rebinding them can
move ownership. After a move, the old binding cannot be used.

<!-- slim-fixture: use-after-move -->

Structs and enums containing affine fields follow the same rule as their
contents.

## Exclusive `inout` borrows

An `inout` parameter borrows a named caller binding exclusively for the call.
The borrow is nonescaping and cannot be moved from.

<!-- slim-fixture: move-from-inout -->

Two `inout` operands cannot alias:

<!-- slim-fixture: inout-alias -->

Temporaries cannot be passed as `inout`, and an `inout` value cannot be
returned. These constraints keep the lifetime local and statically visible.

## Frozen views

Freezing byte storage creates a view whose validity is tied to its source.
Using a source in a way that violates the frozen lifetime is rejected.

<!-- slim-fixture: frozen-source-use -->

## Region selection and destruction

The compiler selects lexical allocation regions. Ownership moves may extend a
value to an enclosing valid region, but destruction remains deterministic.
Allocation sites are retained through lowering so failure behavior remains
observable.

## Compiler guarantees

Safe code exposes no raw pointer, unchecked alias, hidden copy, or hidden
synchronization. Diagnostics point to the exact invalid move, borrow, or use.
The complete model is specified in [Memory](../../MEMORY.md).

## Deliberate omissions

There is no borrow syntax for general shared references, no user-written
lifetime parameter, no reference counting, and no tracing collector.
