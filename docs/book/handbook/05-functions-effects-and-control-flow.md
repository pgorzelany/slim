# Functions, effects, and control flow

Functions have explicit parameter and result types, one canonical executable
entry signature, a checked effect ceiling, and a small set of structured
control-flow forms.

## Function declarations

A function names each parameter type and its result:

```slim
fn double(value: I64) -> I64:
  value + value
```

Pure functions omit an effects clause. Effectful functions list required
capabilities in canonical order inside `effects[...]`.

## The executable entry

A standalone executable defines exactly:

```slim
fn main(args: Vec[Bytes]) -> I64 effects[io]:
  io.println("hello")
  0
```

`main` takes `args: Vec[Bytes]` and returns an `I64` process status. Element
zero is the executable path and later elements are process arguments. Other
signatures are rejected.

<!-- slim-fixture: main-return -->

Project manifests identify the one entry module. Non-entry modules may not
smuggle in another executable entry.

## Effects are capability ceilings

The accepted effects are `alloc`, `io`, and `partial`. A function body and all
calls it makes must remain within its declared list. Declaration is permission,
not proof that an event occurred.

<!-- slim-fixture: missing-effect -->

`alloc` covers allocation-capable storage operations. `io` covers host-visible
I/O, clocks, and networking. `partial` admits recurrence that the checker has
not proved total.

## Calls

Calls use `callee(arguments...)`, with required commas between arguments.
Argument count, types, ownership transfer, borrow exclusivity, and effect
compatibility are all checked. Plain affine arguments are shared read-only.
Exclusive calls use `@argument`; consuming calls use `^argument`.

<!-- slim-fixture: call-arity -->

## Boolean selection

`if`/`else` is the sole Boolean branch form. Its condition must be `Bool`, and
both branches must produce the same exact type. There is no truthiness or
implicit integer-to-Boolean conversion.

## Exhaustive enum matching

`match` handles enums only. Arms occur in declaration order, may not repeat,
and must cover the complete enum without a hidden default.

<!-- slim-fixture: nonexhaustive -->

Payload bindings and ownership are introduced in
[structs, enums, and patterns](06-structs-enums-and-patterns.md).

## Tail recurrence

`recur` transfers control to the current function with a complete new argument
list. It is the canonical form for repetition and tail recurrence.

<!-- slim-fixture: example-countdown -->

The example carries both its controller and accumulated sum through the
recursive call. Its fixed input produces `55`.

<!-- slim-fixture: recur -->

The checker verifies arity, types, ownership, and exact preservation of shared,
`@`, and `^` controller modes.

<!-- slim-fixture: recur-rebind -->

A recurrence is proven total only from a complete structural descent argument:
an exact guard, the same linked controller, a positive exact step, total
updates and arguments, a total base and prefix, and one tail recurrence. When
that evidence is unavailable, the function requires `partial`.

## Exact rules and common errors

Pure code is the default. Missing implementation evidence never discharges a
declared capability. A safe function requires an exact empty blocker set; graph
uncertainty remains visible at the function boundary. Calls reject missing
commas, incorrect arity or types, ownership misuse, undeclared effects, and
branch-result disagreement.

There are no default arguments, overload resolution, implicit effect
polymorphism, exceptions, async functions, closures, `goto`, generators, or
hidden entry-point discovery.

## Next

Continue with [structs, enums, and patterns](06-structs-enums-and-patterns.md).
The normative grammar and effect rules are in [Core](../../CORE.md).
