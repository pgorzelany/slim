# Bindings, evaluation, and mutation

Bindings make value flow visible: `let` prevents direct rebinding, `var`
permits it, calls evaluate left to right, and access to owned storage remains
exclusive.

## Local bindings

A local binding names one typed value:

```slim
let answer: I64 = 40 + 2
```

The annotation is checked exactly. Names resolve lexically, and an unknown name
is an error rather than a dynamic lookup.

<!-- slim-fixture: unknown-name -->

`let` applies to the binding, not recursively to all storage reachable through
its value. A `let Vec[T]` remains the unique owner of mutable vector storage and
can be supplied explicitly as `@values` to `vec.push`, `vec.set`, or an `@`
parameter. It cannot be directly replaced with `values = another_vector`.
`var` permits that direct replacement.

Binding mutability is independent of ownership: both `let` and `var` can hold
copyable values, copyable `Bytes` views, or affine owners. A move can invalidate
either kind of binding.

## Evaluation order

Call operands and nested expressions evaluate from left to right. This matters
when expressions can trap, allocate, move a value, or perform a declared
effect.

<!-- slim-fixture: nested-calls -->

Compiler rewrites and generated C preserve this order. A pure-looking
expression is not permission to discard an observable trap or allocation
failure.

## Visible rebinding

`name = value` changes a uniquely accessible `var` while preserving its exact
declared type. Assignment to `let` is rejected; struct fields are not assigned
through a hidden property setter.

<!-- slim-fixture: data -->

The checker rejects a value of the wrong type:

<!-- slim-fixture: set-type -->

## Exclusive mutation through `@`

An `@` parameter is an exclusive, nonescaping borrow of a named caller binding.
The declaration writes `values: @Vec[I64]`; the call writes `@values`. The same
binding cannot be passed to two simultaneous exclusive parameters, and
temporaries cannot be borrowed.

The borrow lets a function inspect or mutate an affine owner without taking
ownership from the caller. It ends when the call returns. `@` is not a
storable reference type and does not allocate or perform reference counting.

<!-- slim-fixture: exclusive-borrow -->

See [ownership and borrowing](07-ownership-borrowing-and-memory.md) for the move,
alias, and lifetime rules.

## Exact rules and common errors

SLIM has no hidden property setters, implicit copying, reference identity, or
general aliasing mutation. Mutation is syntactically visible and statically
exclusive. Assignment to `let`, incorrect assignment types, aliased exclusive
arguments, temporary borrows, and use after a move are rejected.

## Next

Continue with [functions, effects, and control flow](05-functions-effects-and-control-flow.md).
