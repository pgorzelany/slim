# RFC-0075: Effect-gated monotonic clock

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: monotonic-clock
Safety: 2
Compile: 1
Runtime: 2
Minimal: 1
Analysis: 1
Dogfood: 1
Score: 70

## Need

Core 1I needs evidence from a real host service before choosing a general
capability-handle model. The compiler, benchmark harnesses, timeouts, retry
logic, and later structured concurrency need elapsed time, not civil time.
SLIM currently has file and console I/O but no clock, so programs must receive
timestamps as artificial arguments or cannot measure a deadline.

`io.monotonic-ms` is one zero-argument operation returning a nonnegative `I64`
millisecond value from a process-local monotonic clock. It requires the existing
`io` effect, performs no allocation, and cannot be used by a pure function.
Successive readings on one execution thread do not decrease. Only differences
between readings are meaningful; the epoch is unspecified.

## Alternatives

Wall time carries calendar, timezone, leap, and clock-adjustment semantics that
deadline code does not need. Nanoseconds imply precision targets cannot
guarantee and overflow sooner. A timer object or capability handle would add
storage and ownership surface before one operation demonstrates that a handle
improves safety. Passing time through every call remains possible for pure,
deterministic logic but cannot itself sample the host.

The operation stays in the existing `io` namespace and effect. It is not an
alias for another clock and does not pre-approve sleep, timers, networking,
processes, or a general host handle.

## Costs

Each call crosses the host clock boundary and is nondeterministic, so it cannot
be reduced, reordered, automatically parallelized, or classified total by the
current pure-operation analysis. On supported POSIX targets the runtime uses
the monotonic clock. A portable fallback uses C11 UTC time only when the target
lacks the monotonic API; the runtime clamps out-of-domain values rather than
overflowing.

Typing and code generation add constant-time built-in dispatch. Programs that
do not call the operation emit the same call-free code and pay no runtime cost.

## Evidence

Conformance requires two readings to be nondecreasing, exact `I64` typing, an
`io` effect diagnostic when capability is absent, deterministic C emission, and
native execution through both compiler generations. Governance requires one
surface row and forbids clock aliases.

The application probe records that the operation allocates no SLIM memory and
adds no `partial` effect. Full fixed-point bootstrap, malformed input,
sanitizers, allocation failure, geometric scaling, native comparisons, and
parallel runtime gates remain mandatory.

## Removal

Remove the operation if supported targets cannot provide a stable
nondecreasing value or if a later single capability model replaces it with
strictly stronger authority control. Do not retain an alias, expose an epoch,
silently trap, allocate a timer object, or let a clock call enter pure or
automatically reordered code.
