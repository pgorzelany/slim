# Functions, entry points, and effects

Functions have explicit parameter and result types, one canonical executable
entry signature, and a checked effect ceiling that makes observable behavior
visible at every call boundary.

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
Argument count, types, ownership transfer, borrow exclusivity, and
effect compatibility are all checked.

<!-- slim-fixture: call-arity -->

## Compiler guarantees

Pure code is the default. Missing implementation evidence never discharges a
declared capability. A safe function requires an exact empty blocker set; graph
uncertainty remains visible at the function boundary.

## Deliberate omissions

There are no default arguments, overload resolution, implicit effect
polymorphism, exceptions, async functions, closures, or hidden entry-point
discovery in the accepted language.
