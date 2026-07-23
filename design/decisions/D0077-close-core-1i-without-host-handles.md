# D0077: Close Core 1I without host handles

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 2
Minimal: 2
Analysis: 1
Dogfood: 1
Score: 90

## Need

Core 1I must decide whether real host work requires source-level resource or
capability handles. The maintained matrix covers compiler file input, elapsed
deadlines, a bounded dual-endpoint fetch, process orchestration, immutable
configuration, and generated filesystem output.

File input, the monotonic clock, main arguments, stdout, and one bounded TCP
exchange cover the in-language needs. The compiler remains self-hosted with
those operations. Process orchestration and persistence remain responsibilities
of the external launcher; moving them into ordinary source would grant broader
authority without improving a maintained application.

The TCP exchange opens, uses, half-closes, and closes its descriptor inside one
runtime call. Consequently no invalid, aliased, leaked, or use-after-close
handle state is representable in SLIM.

## Alternatives

A general `Host`, `File`, `Socket`, or process handle would introduce ownership,
state transitions, target capability discovery, error variants, and legal
operation sequences. Raw descriptors, foreign declarations, or a C escape
hatch would weaken memory, authority, and deadlock guarantees. Separate
connect, send, receive, and close operations duplicate a complete protocol the
only network application does not need.

DNS, TLS, writable files, directories, child processes, environment mutation,
sleep, and timers remain possible future boundaries. None has current
application and dependency evidence sufficient to carry permanent surface.

## Costs

SLIM 1.0 cannot resolve host names, negotiate TLS, launch a process, write a
file directly, or hold a long-lived connection. Applications use numeric
addresses and external launchers. TCP transport failures share one explicit
`false` result rather than a reason variant.

In exchange, Core 1I adds only two canonical operations, no new syntax, no
handle type, no unsafe mode, no dependency, and no new memory-lifetime rule.
Programs without calls perform no host work. The measured inactive TCP binary
cost remains below its permanent 1.03 same-host budget.

## Evidence

The host-needs matrix is permanent. The self-hosted compiler uses total file
input, the clock has a same-operation C comparison, and the loopback TCP
application covers success, invalid input, limit failure, output preservation,
descriptor closure, and unsupported targets. Type and effect diagnostics,
analysis blockers, one-spelling governance, 113-fixture conformance, 2,000
malformed mutations, and fixed-point bootstrap remain mandatory.

The dual-endpoint application establishes the Core 1J need: two independent
effectful exchanges should overlap, but automatic pure-work parallelism
correctly refuses them.

## Removal

Reopen Core 1I only when a maintained application cannot be expressed through
the accepted whole operations and an external launcher. Any future handle must
make illegal states unrepresentable, have lexical ownership and typed closure,
and replace rather than alias an existing operation. Never add a general host
or foreign boundary as a compatibility shortcut.
