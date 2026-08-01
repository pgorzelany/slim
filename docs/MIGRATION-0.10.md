# Migrating ownership modes

RFC-0110 replaces implicit affine parameter consumption and `inout` with three
canonical parameter modes:

| Declaration | Call | Meaning |
| --- | --- | --- |
| `values: Vec[I64]` | `read(values)` | Shared, read-only, nonescaping borrow |
| `values: @Vec[I64]` | `mutate(@values)` | Exclusive, read-write, nonescaping borrow |
| `values: ^Vec[I64]` | `consume(^values)` | Ownership transfer |

Copyable parameters remain plain and copy their value. `@` and `^` are rejected
for copyable parameters.

To migrate:

1. Replace `inout values: T` with `values: @T` and mark every corresponding
   call or `recur` argument with `@`.
2. Leave affine parameters plain when the body only reads them. Remove the
   now-unnecessary `@` at their call sites.
3. Change an affine parameter to `values: ^T` when the callee stores, returns,
   freezes, forwards, or otherwise takes ownership. Mark each transfer with
   `^`.
4. Mark mutating built-ins such as `vec.push`, `vec.set`, `arena.add`,
   `io.read_file`, and `io.tcp_exchange` with `@`. Mark
   `bytes.freeze(^vector)` with `^`.

The compiler does not retain `inout` as an alias and does not infer public
parameter modes from function bodies. Mode mismatches use `E0360`; attempts to
mutate, move, or escape a borrow retain the existing ownership diagnostics.

`^` is required only when ownership crosses a function, `recur`, or consuming
built-in boundary. It accepts a whole named owner or a freshly produced affine
owner. It is not written on ordinary moves into `let`, `var`, structs, enum
payloads, or owning collection slots. A projection such as `^owner.values` is
rejected because SLIM does not track partially moved aggregate owners; first
move the field through the ordinary owning destination supported by the
program's data flow, then transfer that whole named owner.

Project interface schema 3 records parameter modes as `copy`, `shared`,
`exclusive`, or `owned`. Schema 2 artifacts must be regenerated.
