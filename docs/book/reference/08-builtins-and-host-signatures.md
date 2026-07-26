# Built-ins and host signatures

This chapter groups the compiler-recognized operations by role; the generated
surface ledger and `slimc builtins` remain the exact inventories.

## Scalar built-ins

`i64.add`, `i64.sub`, `i64.mul`, `i64.div`, and `i64.rem` are checked integer
operations. `i64.eq`, `i64.lt`, `i64.le`, `i64.gt`, and `i64.ge` return
`Bool`. `bool.not`, `bool.and`, and `bool.or` operate on `Bool`.

`u8.to-i64` widens explicitly. `i64.to-u8` checks the target domain and traps
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

`io.print-i64`, `io.print-bytes`, and `io.println` provide ordered output.
`io.read-file` provides bounded typed file input. These operations require the
effects in their exact signatures.

## Clock and network

`io.monotonic-ms` reads a monotonic duration clock. `io.tcp-exchange` performs
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
