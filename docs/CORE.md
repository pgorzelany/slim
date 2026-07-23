# SLIM 1.0 Core language

Status: stable for major version 1

SLIM source is the only compilable program representation. Whitespace and comments
are insignificant, and `slimc fmt` emits the unique canonical layout.

## Lexical grammar

- Forms are delimited by `(` and `)`.
- Identifiers contain ASCII letters, digits, `_`, `-`, `.`, `/`, `?`, or `!`,
  and may not begin with a digit.
- Integers are base-10 `I64` literals with an optional leading `-`.
- Byte strings use double quotes and the escapes `\\`, `\"`, `\n`, `\r`,
  `\t`, and `\xNN` for an exact hexadecimal byte.
- `;` starts a comment extending through the end of the line.

## Program grammar

```text
program  = (module NAME item*)
item     = function | record | variant
function = (fn NAME (parameter*) TYPE (effects EFFECT*) EXPR)
parameter = (NAME TYPE) | (inout NAME TYPE)
record   = (record NAME ((NAME TYPE)*))
variant  = (variant NAME ((NAME TYPE*)*))
type     = Unit | Bool | U8 | I64 | Bytes | NAME | (Vec TYPE)
         | (Arena TYPE) | (Id TYPE)
```

The expression forms are literals, names, `let`, `call`, `make`,
`get`, `case`, `match`, `recur`, `set`, and `fork`. Each maps directly to one
AST form; there is no desugaring layer.

Every executable defines exactly `(fn main ((args (Vec Bytes))) I64 ... )`.
Element zero is the executable path and remaining values are process arguments.
There is no implicit global argument accessor and no alternative entry-point
signature.

Every type position must contain one scalar type, one declared record or
variant name, or exactly one of `(Vec TYPE)`, `(Arena TYPE)`, and `(Id TYPE)`.
Unknown names, function names used as types, malformed constructors, and extra
type arguments are rejected before code generation.

## Semantics

- Evaluation order is left-to-right.
- Bindings are immutable unless declared as the target of `set` through unique
  lexical access.
- Scalars and typed IDs are copyable. Owned aggregates move.
- An `inout` parameter is a non-escaping exclusive lexical borrow. Calls pass a
  named unique binding and cannot move it while the borrow is active.
- Indexing is checked. Arithmetic overflow is a defined trap in checked builds
  and two's-complement wrapping only through explicitly named wrapping
  operations.
- Pure functions declare `(effects)`. Allocation, I/O, and unproven termination
  require `alloc`, `io`, and `partial` respectively.
- Record construction names every declared field exactly once and in declaration
  order. Field projection must name a field of the value's record type.
- Variant construction names one declared case and supplies exactly its payload
  types. A variant `match` contains every case exactly once and in declaration
  order; each arm binds exactly the declared payload.
- A Boolean `match` contains `true` and `false` exactly once. Boolean arm order
  is irrelevant because `Bool` has no user-declared source order.
- Every nested expression is checked against its expected type. Calls and
  `recur` require exact arity and argument types; `set` preserves the binding's
  declared type; all match arms agree on the enclosing expected type.
- A tail-position `recur` transfers control to the current function entry and
  does not grow the stack.
- Resource failure is returned as an explicit typed value; it is never
  undefined behavior. `io.read-file` has the single total signature
  `(Bytes, inout (Vec U8)) -> Bool`: success appends the complete file, while
  failure returns `false` and leaves the output unchanged.
- `io.monotonic-ms() -> I64` requires `io`, allocates nothing, and returns a
  nonnegative reading that does not decrease within one execution thread. Its
  epoch is unspecified.
- `io.tcp-exchange(Bytes, I64, Bytes, I64, I64, inout (Vec U8)) -> Bool`
  requires `alloc io`; response size and elapsed wait are explicit, every
  descriptor is runtime-lexical, and failure leaves output unchanged.
- `(fork (let first T (call f ...) (let second U (call g ...) body)))` is the
  one explicit concurrency form. It admits two independent leading direct
  leaf calls with bounded host effects, scalar or byte-view inputs, no
  `inout`, and no `partial`. Both calls finish before `body`; results install
  in lexical order. Spawn failure uses the same serial execution.
- Allocation exhaustion is the typed failure outcome of the existing `alloc`
  effect. Generated code propagates it immediately, destroys active regions,
  and handles it once at the executable boundary with exit code 71. SLIM 1.0
  exposes no trapping or `try-` allocation alias and no invalid value sentinel;
  local recovery is not yet source surface.

## Built-in calls

Built-in calls use the same `call` form as ordinary functions and have exactly
one canonical name. SLIM 1.0 includes checked integer arithmetic and comparison,
Boolean operations, byte length/access, vector length/access/update, and basic
I/O required by the compiler. Their concrete signatures are emitted by
`slimc builtins` and tested as part of the language surface.

The current host-service contract and its deliberately absent capabilities are
specified in `docs/HOST.md`.

## Diagnostics

Every diagnostic has a stable code, severity, primary byte span, optional
labels, notes, and fixes. Human and newline-delimited JSON renderings contain
the same information. User input must not panic the compiler.

## Tooling boundary

`slimc analyze`, `reduce`, `prove-reduction`, `verify-reduction`, `equivalent`,
and `edit` are compiler tools, not Core language features. Analysis and proof
commands emit versioned non-executable reports. Reduction and a successfully
checked edit emit ordinary canonical `.slim` source and cannot change which
forms the compiler accepts. Canonical SLIM remains the only compilable
representation; derived facts, proofs, patches, and control-flow views have no
independent executable syntax or type system. The bounded contracts are
specified in `docs/REDUCTION.md` and `docs/QUALITY.md`; compatibility and
diagnostic promises are in `docs/COMPATIBILITY.md` and `docs/DIAGNOSTICS.md`.
