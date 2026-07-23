# Typed host boundary

Status: Core 1I first host-service slice

SLIM host operations use ordinary typed `call` forms and explicit effects.
They cannot appear in a pure `(effects)` function. There is no unsafe source
escape hatch, raw pointer, foreign declaration, ambient lock, or unchecked
handle.

## Existing services

- `io.print-i64(I64) -> Unit`, `io.print-bytes(Bytes) -> Unit`, and
  `io.println(Bytes) -> Unit` require `io`.
- `io.read-file(Bytes, inout (Vec U8)) -> Bool` requires `alloc io`. It appends
  the complete file on success; transport failure returns `false` and leaves
  the output unchanged.
- `io.monotonic-ms() -> I64` requires `io`, performs no SLIM allocation, and
  returns a nonnegative millisecond reading. Successive readings on one
  execution thread do not decrease. The epoch is unspecified; only differences
  are meaningful.

The monotonic clock is intentionally the only clock. SLIM has no wall-clock,
timezone, calendar, sleep, deadline, or timer alias.

## Capability model

Today the `io` effect is the compile-time host capability: callers and every
transitive function must declare that they may cross the host boundary. This is
explicit and statically enforced, but it grants the whole current `io` set.

Core 1I will use application evidence to decide whether resource-bearing or
authority-narrowing services need nominal capability values. A general `Host`
value is not pre-approved. Adding one without narrower authority would cost
tokens while preserving the same ambient access inside an `io` function.

## Target behavior

The runtime uses a native monotonic clock when the target exposes one and a
C11 time source otherwise. It clamps overflow and failed or regressing samples
to the prior thread-local reading. Clock sampling has no allocation, trap, or
`partial` effect.

Unsupported future host capabilities must fail through a typed result or a
documented capability absence. They may not silently become unsafe, trapping,
or globally mutable source operations.

## Not implemented

There are no sockets, DNS, TLS, processes, environment mutation, timers,
filesystem writes, directory traversal, foreign calls, or source-level
capability handles. Their absence is deliberate until representative
applications define the smallest common operation and failure model.
