# SLIM Core 0

Status: internal bootstrap specification

SLIM Core is the only compilable source representation. Whitespace and comments
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
function = (fn NAME ((NAME TYPE)*) TYPE (effects EFFECT*) EXPR)
record   = (record NAME ((NAME TYPE)*))
variant  = (variant NAME ((NAME TYPE*)*))
type     = Unit | Bool | U8 | I64 | Bytes | NAME | (Vec TYPE)
         | (Arena TYPE) | (Id TYPE)
```

The bootstrap expression forms are literals, names, `let`, `call`, `make`,
`get`, `case`, `match`, `recur`, and `set`. Each maps directly to one AST form;
there is no desugaring layer.

## Semantics

- Evaluation order is left-to-right.
- Bindings are immutable unless declared as the target of `set` through unique
  lexical access.
- Scalars and typed IDs are copyable. Owned aggregates move.
- Indexing is checked. Arithmetic overflow is a defined trap in checked builds
  and two's-complement wrapping only through explicitly named wrapping
  operations.
- Pure functions declare `(effects)`. Allocation, I/O, and unproven termination
  require `alloc`, `io`, and `partial` respectively.
- `match` must cover every variant or both Boolean values.
- A tail-position `recur` transfers control to the current function entry and
  does not grow the stack.
- Resource failure is returned as a variant value; it is never undefined
  behavior.

## Built-in calls

Built-in calls use the same `call` form as ordinary functions and have exactly
one canonical name. Core 0 includes checked integer arithmetic and comparison,
Boolean operations, byte length/access, vector length/access/update, and basic
I/O required by the compiler. Their concrete signatures are emitted by
`slimc builtins` and tested as part of the language surface.

## Diagnostics

Every diagnostic has a stable code, severity, primary byte span, optional
labels, notes, and fixes. Human and newline-delimited JSON renderings contain
the same information. User input must not panic the compiler.
