# D0076: Bounded TCP exchange

Status: accepted
Kind: language
Primitive: bounded-tcp-exchange
Safety: 2
Compile: 1
Runtime: 1
Minimal: 2
Analysis: 0
Dogfood: 0
Score: 60

## Need

The Core 1I application matrix needs one network operation before Core 1J can
test parallel host work. A request/response client needs to connect, send a
finite byte request, receive a finite byte response, close the connection, and
report failure. Exposing a socket handle would also require states for
unconnected, connected, half-closed, failed, and closed resources plus a legal
operation protocol that current applications do not need.

`io.tcp-exchange(address, port, request, response-limit, timeout-ms, output)`
performs that complete protocol as one `alloc io` operation and returns `Bool`.
The address is numeric IPv4 or IPv6, the port is in `1..65535`, the response
limit is nonnegative, and the timeout is positive. Success appends the complete
response to the `Vec U8`; failure returns `false` and leaves it unchanged.

## Alternatives

Raw sockets, file descriptors, connect/send/receive/close calls, or a general
host handle expose illegal state sequences, partial writes, lifetime mistakes,
and authority without improving the demonstrated request/response program.
DNS introduces a separately blocking resolver and name-policy boundary. HTTP
or TLS would require protocol, certificate, dependency, and version policy
that does not belong in the language primitive.

A process-based client or unrestricted C declaration violates the ordinary
safe host boundary. An unbounded convenience request hides memory and blocking
cost. The accepted operation instead composes with ordinary byte processing
and the existing `io` and `alloc` effects.

## Costs

The portable runtime gains one optional POSIX socket implementation. Supported
targets open one descriptor per call and close it before returning. Programs
that do not use the operation emit no call or generated state. The compiler
adds constant-time built-in dispatch and one direct lowering.

On the acceptance host, linking the implementation increased an unrelated
Hello executable from 52,008 to 52,792 bytes, a 1.015 ratio. A permanent 1.03
same-host binary ratio bounds that inactive cost.

The timeout covers connect, send, and receive after numeric address parsing.
It is not a real-time guarantee: host scheduling and kernel behavior can
overshoot. Unsupported targets return `false`. `Bool` deliberately reports one
transport-failure class; applications that need typed failure reasons must
first demonstrate how those reasons change safe control flow.

## Evidence

Conformance fixes the signature, effects, and missing-capability diagnostic.
A loopback application verifies exact request bytes, response bytes, output
preservation for invalid input, response-limit failure, and descriptor closure
without external network access. Analysis must report the operation as
effectful and allocation-capable, never pure or automatically reorderable.
An independently compiled network-disabled runtime verifies predictable typed
failure on an unsupported target.

Governance requires one surface row, forbids socket and network aliases, and
keeps the complete fixed-point, malformed-input, scaling, sanitizer,
allocation-failure, native-performance, and worker gates mandatory.

## Removal

Remove the operation if it cannot preserve the timeout, output, closure, or
typed failure contract on supported targets. Never retain a compatibility
alias, expose its descriptor, silently make DNS or TLS ambient, omit the
response bound, add an unbounded timeout, or split the protocol into resource
operations without a new application-backed ownership design.
