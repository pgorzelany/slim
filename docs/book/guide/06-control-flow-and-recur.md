# Control flow and `recur`

SLIM uses a small set of structured forms for branching, exhaustive matching,
and recurrence, keeping control flow explicit and analyzable.

## Boolean selection

A Boolean `match` requires `Bool`; truthiness and implicit integer-to-Boolean
conversion do not exist. Both source structure and emitted control flow remain
deterministic.

## Exhaustive matching

`match` handles Boolean or variant alternatives. Arms may not repeat and must
cover the complete domain. Variant arms follow declaration order; Boolean arm
order is irrelevant because `Bool` has no declared source order.

<!-- slim-fixture: nonexhaustive -->

Exhaustiveness means a checked match does not gain a hidden default branch.
Variant patterns and payload bindings are covered in
[records, variants, and patterns](07-records-variants-and-patterns.md).

## Tail recurrence

`recur` transfers control to the current function with a complete new argument
list. It is the single canonical recurrence form.

<!-- slim-fixture: recur -->

The checker verifies arity and types. An `inout` controller must be preserved
according to the exclusive-borrow rules.

<!-- slim-fixture: recur-rebind -->

## Totality evidence

A recurrence is proven total only from a complete structural descent argument:
an exact guard, the same linked controller, a positive exact step, a total
update, total recurrent arguments, total base and prefix, and one tail
recurrence. Direction alone is not proof.

If totality is not proven, the containing function requires `partial`.
Analysis reports exact, bounded with its fixed limit, or unknown with a stable
reason.

## Compiler guarantees

Lowering does not introduce recursion through a second source form. Reduction
terminates, is deterministic and idempotent, and preserves evaluation order,
effects, traps, allocation failure, moves, mutation, and borrows.

## Deliberate omissions

There are no `goto`, implicit loop desugaring, exceptions, generators, or
unbounded proof searches in the default checker.
