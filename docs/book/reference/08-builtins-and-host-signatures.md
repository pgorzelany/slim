# Built-ins and host signatures

This chapter groups the compiler-recognized operations by role; the generated
surface ledger and `slimc builtins` remain the exact inventories.

## Scalar operations

`+`, `-`, `*`, `/`, and `%` are checked integer operations. `==`, `<`, `<=`,
`>`, and `>=` return `Bool`. `!`, `&&`, and `||` operate on `Bool`.

`u8.to_i64` widens explicitly. `i64.to_u8` checks the target domain and traps
when the input is outside it.

## Byte operations

`bytes.len` returns a length, `bytes.get` performs a checked read, and
`bytes.freeze` establishes the maintained frozen-view lifetime relation.
Byte-storage construction and host input follow their declared allocation and
I/O effects.

## Vector operations

`vec.new` constructs typed storage. `vec.len`, `vec.get`, `vec.push`, and
`vec.set` provide the complete accepted vector surface. Access is checked and
growth is allocation-capable.

## Arena operations

`arena.new` constructs typed arena storage. `arena.add` returns `Id[T]`, and
`arena.get` resolves a matching typed identity. IDs are not pointer values.

## Terminal and file I/O

`io.print_i64`, `io.print_bytes`, and `io.println` provide ordered output.
`io.read_file` provides bounded typed file input. These operations require the
effects in their exact signatures.

## Clock and network

`io.monotonic_ms` reads a monotonic duration clock. `io.tcp_exchange` performs
one bounded typed exchange. Neither exposes a general operating-system handle,
socket, thread, or event loop.

## Exact inventory

Run:

```sh
./slimc builtins
```

The website's `reference/surface.json` is generated from
`design/surface.tsv`; tests compare its built-in projection with this command.
Signatures and failure rules are normative in [Core](../../CORE.md) and
[Host](../../HOST.md).
