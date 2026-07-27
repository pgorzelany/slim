# Host I/O

Host interaction is a narrow, effect-gated library boundary covering terminal
output, file input, a monotonic clock, and bounded TCP exchange.

## Terminal output

`io.print_i64`, `io.print_bytes`, and `io.println` require `io`. Output order
follows ordinary left-to-right evaluation.

## File input

`io.read_file(Bytes, inout Vec[U8]) -> Bool` performs a bounded host read. It
appends the complete file on success; failure returns `false` and leaves the
output unchanged. The operation requires `alloc io`.

<!-- slim-fixture: file-input -->

The fixture uses only repository-controlled input in conformance. The language
does not infer filesystem authority from a string.

## Monotonic time

`io.monotonic_ms() -> I64` exposes a monotonic millisecond clock for duration
measurement, not a wall-clock calendar.

<!-- slim-fixture: monotonic-clock -->

Omitting the required capability is diagnosed:

<!-- slim-fixture: missing-clock-effect -->

## Bounded TCP exchange

The host contract exposes one bounded
`io.tcp_exchange(Bytes, I64, Bytes, I64, I64, inout Vec[U8]) -> Bool` rather
than sockets or a general asynchronous networking API.

<!-- slim-fixture: tcp-exchange -->

The checker enforces both the host signature and its effects. The complete
target and capability boundary is in [Host](../../HOST.md).

## Compiler guarantees

Host calls are visible calls with checked argument and result types. They are
never introduced by a pure library operation, formatter, analyzer, or
optimizer.

## Deliberate omissions

There is no environment-variable API, process spawning, unrestricted socket
API, wall clock, thread API, event loop, hidden buffering contract, or ambient
host capability in the accepted surface.
