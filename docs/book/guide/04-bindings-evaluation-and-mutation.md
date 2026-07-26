# Bindings, evaluation, and mutation

Bindings make value flow visible: `let` introduces a value, calls evaluate
left to right, and mutation occurs only through the canonical `set` or `inout`
forms.

## Local bindings

A local binding names one typed value:

```slim
let answer: I64 = i64.add(40 2)
```

The annotation is checked exactly. Names resolve lexically, and an unknown name
is an error rather than a dynamic lookup.

<!-- slim-fixture: unknown-name -->

## Evaluation order

Call operands and nested expressions evaluate from left to right. This matters
when expressions can trap, allocate, move a value, or perform a declared
effect.

<!-- slim-fixture: nested-calls -->

Compiler rewrites and generated C preserve this order. A pure-looking
expression is not permission to discard an observable trap or allocation
failure.

## Visible rebinding

`set name = value` changes a uniquely accessible lexical binding while
preserving its exact declared type. It is the single canonical rebinding form;
record fields are not assigned through a hidden property setter.

<!-- slim-fixture: data -->

The checker rejects a value of the wrong type:

<!-- slim-fixture: set-type -->

## Exclusive mutation through `inout`

An `inout` parameter is an exclusive, nonescaping borrow of a named caller
binding. The parameter declaration marks the borrow; the ordinary call must
supply a named unique binding. The same binding cannot be passed to two
simultaneous `inout` parameters, and temporaries cannot be borrowed.

<!-- slim-fixture: inout -->

See [ownership and borrowing](09-ownership-moves-and-borrowing.md) for the move,
alias, and lifetime rules.

## Deliberate omissions

SLIM has no hidden property setters, implicit copying, reference identity, or
general aliasing mutation. Mutation is syntactically visible and statically
exclusive.
