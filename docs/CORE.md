# SLIM 0.9 Core language

Status: SLIM 0.9 — experimental, pre-1.0

SLIM source is the only compilable program representation. Leading whitespace
defines blocks, and `slimc fmt` emits the unique canonical layout. SLIM 0.9 is
an atomic syntax replacement: the compiler does not accept the removed
pre-0.9 spellings.

## Lexical grammar

- Indentation is exactly two ASCII spaces per level. Tabs, odd indentation,
  and skipped levels are errors. Blank lines and `#` comments do not change
  indentation.
- Identifiers match `[A-Za-z_][A-Za-z0-9_]*`. A qualified module reference is
  a dot-separated sequence of identifiers. `::` is reserved for enum cases.
- Integers are base-10 `I64` literals with an optional leading `-`.
- Byte strings use double quotes and the escapes `\\`, `\"`, `\n`, `\r`,
  `\t`, and `\xNN`.
- Parenthesized and bracketed lists require commas between elements. Missing,
  leading, doubled, and trailing commas are rejected.
- Braces and semicolons are not part of the language.

## Program grammar

```text
program    = "module" identifier NEWLINE item*
item       = function | struct | enum
function   = "fn" identifier "(" parameters? ")" "->" type effects? ":" block
parameters = parameter ("," parameter)*
parameter  = "var"? identifier ":" ("@" | "^")? type
effects    = "effects" "[" effect ("," effect)* "]"
struct     = "struct" identifier ":" NEWLINE INDENT field* DEDENT
field      = identifier ":" type NEWLINE
enum       = "enum" identifier ":" NEWLINE INDENT enum_case+ DEDENT
enum_case  = identifier ("(" types? ")")? NEWLINE
types      = type ("," type)*
type       = Void | Bool | U8 | I64 | Bytes | identifier
           | "Vec" "[" type "]" | "Arena" "[" type "]" | "Id" "[" type "]"
block      = statement* expression
statement  = "let" identifier ":" type "=" expression
           | "var" identifier ":" type "=" expression
           | identifier "=" expression
           | void_expression
```

Effects are drawn from `alloc`, `io`, and `partial`, in that order without
duplicates. Every executable defines exactly
`fn main(args: Vec[Bytes]) -> I64 ...`.

Applications are comma-separated and evaluated left to right. `@` marks an
exclusive borrow argument and `^` marks ownership transfer across the call or
recurrence boundary:

```slim
function_name(first, second)
mutate(@storage)
consume(^storage)
recur(next, total)
Pair(left: 20, right: 22)
Maybe::Some(value)
```

Function arguments remain positional. Named, colon-separated elements identify
struct construction and must provide every field once in declaration order.
`Pair()` is empty-struct construction when `Pair` resolves to a struct and a
zero-argument call when it resolves to a function. Declarations share one
namespace, so the two cannot have the same fully qualified name.

Projection uses `value.field`; module qualification uses `module.name`; enum
construction uses `Type::Case`. A match pattern uses the case name within the
scrutinee's known enum. Nested projection is allowed.

## Expressions and control flow

The scalar and Boolean operators are:

```text
highest  !                  right associative
         * / %              left associative
         + -                left associative
         < <= > >=          left associative
         ==                 left associative
         &&                 left associative
lowest   ||                 left associative
```

There is no `!=` and no general unary negation; a negative integer is a literal.
Operators map directly to the existing checked operations. Grouping uses
parentheses and does not change left-to-right operand evaluation.

`if`/`else` is the sole Boolean branch form:

```slim
if ready && count > 0:
  value
else:
  fallback
```

`match` is restricted to enums. It contains every case exactly once in
declaration order, and each pattern binds exactly the declared payload:

```slim
match value:
  None:
    0
  Some(inner):
    inner
```

`parallel:` is the one explicit concurrency form. It has checked execution,
join, effect, and serial-fallback behavior.

## Bindings and Void

`let` creates an immutable binding. `var` creates a mutable binding. Assignment
is valid only for `var`, mutable parameters and pattern bindings, or an `@`
parameter. An affine parameter without a mode is a shared, read-only,
nonescaping lexical borrow. `@` grants an exclusive, read-write, nonescaping
borrow. `^` receives ownership from the caller. Calls and `recur` must repeat
`@` or `^` exactly; copyable parameters accept neither marker.

Every non-final block expression must have exact type `Void`; silently
discarding another result is an error. `void` is the explicit `Void` value.
`Void` may be used only as a function result, a Void block result, or an
expression statement. It cannot be a parameter, stored binding, struct field,
enum payload, or collection element. The compiler retains its internal unit
representation and lowers it to `SlimUnit`; this source rename has no runtime
or ABI effect.

## Semantics

- Evaluation order is left to right.
- `Bool`, `U8`, `I64`, typed IDs, immutable `Bytes` views, and aggregates
  containing only copyable members are copyable. Vectors, arenas, and
  aggregates containing either are affine and move.
- Indexing is checked. Checked arithmetic overflow, division by zero, and
  remainder by zero are defined traps.
- Pure functions omit the effects clause. Allocation, I/O, and unproven
  termination require `alloc`, `io`, and `partial`.
- Calls and `recur` require exact arity and types. Assignment preserves the
  binding type. Branch results must agree.
- Plain affine arguments are shared and remain usable after the call. `@name`
  borrows a named owner exclusively for the call. `^name` transfers one whole
  named affine owner and invalidates that name; `^expression` is otherwise
  accepted only when the expression freshly produces the transferred owner.
- `^` is the call-boundary marker, not a marker on every affine move. Existing
  moves into local bindings, aggregate payloads, and owning collection slots
  remain unmarked because the owning destination is explicit. Field
  projections and branch-selected existing owners are not valid `^` sources
  without partial- or conditional-move tracking.
- A tail-position `recur` transfers control to the current function entry
  without growing the stack.
- Resource failure remains an explicit typed result.
- Structured parallel execution retains identical task bodies, deterministic
  joins, serial fallback, and the existing runtime gates.

## Built-ins and qualification

Built-ins use snake_case names such as `io.print_i64`, `io.read_file`,
`io.tcp_exchange`, `io.monotonic_ms`, `vec.get`, and `bytes.freeze`.
Read-only storage operands are plain; mutation/output operands use `@`; and
`bytes.freeze(^vector)` consumes its vector.
Arithmetic, comparisons, and Boolean operations use the operator syntax above;
their earlier callable spellings are not aliases.

The complete signatures are emitted by `slimc builtins`. Host behavior is
specified in `docs/HOST.md`.

## Diagnostics and tooling

Every diagnostic has a stable code, severity, primary byte span, optional
labels, notes, and fixes. Human and JSON renderings carry the same information.

Analysis, reduction, equivalence, proof, edit, and agent-patch outputs are
versioned tooling data, never a second executable representation. Canonical
SLIM remains the compiler's sole accepted program form. Compatibility and
diagnostic policies are specified in `docs/COMPATIBILITY.md` and
`docs/DIAGNOSTICS.md`.
