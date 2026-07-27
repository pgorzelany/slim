# Migrating to SLIM 0.9

Status: SLIM 0.9 — experimental, pre-1.0

SLIM 0.9 atomically replaces the earlier internal syntax. The final compiler
does not include aliases or a legacy parser.

| Before | SLIM 0.9 |
| --- | --- |
| `record Pair:` | `struct Pair:` |
| `variant Maybe:` | `enum Maybe:` |
| `Unit` / `unit` | `Void` / `void` |
| `fork:` | `parallel:` |
| `make Pair(left = 1 right = 2)` | `Pair(left: 1, right: 2)` |
| `get(pair left)` | `pair.left` |
| `case Maybe::Some(value)` | `Maybe::Some(value)` |
| Boolean `match` | `if` / `else` |
| `let x ...` then `set x = ...` | `var x ...` then `x = ...` |
| whitespace-separated lists | comma-separated lists |
| `i64.add(a b)` | `a + b` |
| `i64.eq(a b)` | `a == b` |
| `bool.not(x)` | `!x` |
| `module/name` | `module.name` |
| `io.print-i64` | `io.print_i64` |

Convert every identifier from kebab-case to snake_case. This includes module,
function, parameter, local, field, and built-in names. `.` is used for module
qualification and field projection; `::` is reserved for enum cases.

Remove dummy bindings that stored `Unit` results:

```slim
# before
let shown: Unit = io.print-i64(value)

# SLIM 0.9
io.print_i64(value)
```

For each old `set` target, change its declaration or parameter/pattern binding
to `var` and use ordinary assignment. Keep `inout` where the binding is an
exclusive caller borrow.

Add commas to function parameters and arguments, `recur`, enum payloads,
pattern bindings, effect lists, and constructor fields. SLIM 0.9 rejects
missing and trailing commas; canonical multiline lists also have no trailing
comma.

`Void` is not storable. It is allowed as a function or block result and for
expression statements only. A non-final expression statement must check as
exactly `Void`; discarding another result is an error.

RFC-0107 records the version reset and syntax cutover. RFC-0082 and RFC-0083 remain
unchanged historical evidence.
