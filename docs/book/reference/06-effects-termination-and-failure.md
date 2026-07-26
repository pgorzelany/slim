# Effects, termination, and failure

This chapter specifies effect ceilings, recurrence totality evidence, checked
traps, typed host failure, and allocation exhaustion.

## Effect lists

The accepted effects are `alloc`, `io`, and `partial`, written in canonical
order. Pure functions omit the clause. A declaration is a checked capability
ceiling: every operation and call in the body must fit, but declaration alone
does not prove that an event occurs.

`alloc` covers allocation-capable operations. `io` covers the maintained host
boundary. `partial` is required when recurrence has not been proven total.

## Effect propagation

Calls propagate their required capabilities to callers. The checker reports a
missing capability at the exact operation or call. It may discharge an unused
capability only from complete checked body and call-graph evidence; missing or
bounded-away implementation evidence remains unknown.

## Recurrence and totality

`recur` is tail recurrence to the current function with a complete typed
argument list. Totality requires the complete structural descent proof defined
by [Resource bounds](../../RESOURCE_BOUNDS.md). Direction or an apparent
decrease alone is insufficient.

Analysis classifies work as exact, bounded with an explicit fixed limit, or
unknown with a stable reason. Default checking remains approximately linear.

## Runtime traps

Checked arithmetic, conversion, and indexing trap on invalid domains. Traps
have stable runtime messages and exit status `70`. A trap hazard disappears
only from a positive fact for that exact checked node.

## Allocation exhaustion

Allocation exhaustion uses the deterministic failure path with exit status
`71`. Allocation sites and region cleanup survive lowering. Failure injection
in conformance provides exact evidence for ordering and destruction.

## Typed host failure

Fallible file and TCP host services return `Bool` and leave their `inout`
output unchanged on failure. SLIM does not use exceptions or hidden unwinding
for host errors.

## Normative boundary

See [Core](../../CORE.md), [Memory](../../MEMORY.md), and
[Host](../../HOST.md).
