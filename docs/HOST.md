# Typed host boundary

Status: Core 1I complete

SLIM host operations use ordinary typed calls and explicit effects.
They cannot appear in a pure function. There is no unsafe source
escape hatch, raw pointer, foreign declaration, ambient lock, or unchecked
handle.

## Existing services

- `io.print-i64(I64) -> Unit`, `io.print-bytes(Bytes) -> Unit`, and
  `io.println(Bytes) -> Unit` require `io`.
- `io.read-file(Bytes, inout Vec[U8]) -> Bool` requires `alloc io`. It appends
  the complete file on success; transport failure returns `false` and leaves
  the output unchanged.
- `io.monotonic-ms() -> I64` requires `io`, performs no SLIM allocation, and
  returns a nonnegative millisecond reading. Successive readings on one
  execution thread do not decrease. The epoch is unspecified; only differences
  are meaningful.
- `io.tcp-exchange(Bytes, I64, Bytes, I64, I64, inout Vec[U8]) -> Bool`
  requires `alloc io`. It sends one finite request to a numeric IP address and
  port, receives at most the declared response bytes before the positive
  timeout, closes the connection, and appends only a complete response.
  Invalid input, timeout, transport error, or limit exhaustion returns `false`
  and leaves the output unchanged.

The monotonic clock is intentionally the only clock. SLIM has no wall-clock,
timezone, calendar, sleep, deadline, or timer alias.

## Capability model

Today the `io` effect is the compile-time host capability: callers and every
transitive function must declare that they may cross the host boundary. This is
explicit and statically enforced, but it grants the whole current `io` set.

The Core 1I application matrix in `benchmarks/host/needs.tsv` did not justify
resource-bearing source handles. The complete TCP exchange keeps descriptor
ownership lexical inside the runtime, so invalid handle and close states are
unrepresentable. A general `Host` value is not pre-approved. Adding one without
narrower authority would cost tokens while preserving the same ambient access
inside an `io` function.

## Target behavior

The runtime uses a native monotonic clock when the target exposes one and a
C11 time source otherwise. It clamps overflow and failed or regressing samples
to the prior thread-local reading. Clock sampling has no allocation, trap, or
`partial` effect.

Unsupported future host capabilities must fail through a typed result or a
documented capability absence. They may not silently become unsafe, trapping,
or globally mutable source operations.

On POSIX targets, TCP connect, send, and receive share the explicit elapsed
deadline. Unsupported targets return `false`; they do not expose a partial
descriptor or trap.

## Not implemented

There are no source socket handles, DNS, TLS, processes, environment mutation,
timers, filesystem writes, directory traversal, foreign calls, or source-level
capability handles. TCP exchange is a bounded whole operation, not access to a
descriptor. The remaining omissions are deliberate until representative
applications define the smallest common operation and failure model.
