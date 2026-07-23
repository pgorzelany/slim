# D0070: Tiered structured worker ABI

Status: accepted
Kind: runtime
Primitive: none
Safety: 2
Compile: 2
Runtime: 1
Minimal: 2
Analysis: 2
Dogfood: 0
Score: 85

## Need

D0069 requires a portable or explicitly tiered worker ABI before automatic
parallel execution can be reconsidered. ISO C11 workers are not available on
the acceptance host, while permanently requiring POSIX threads would exclude
other C11 targets and charge every generated program for an optimization that
most programs do not use.

The runtime now exposes one opt-in structured task ABI when `SLIM_PARALLEL` is
defined. A POSIX tier is selected separately with `SLIM_POSIX_WORKERS`; the
portable base tier always declines worker creation and therefore executes the
same task inline. Programs that do not opt in see no task declarations, task
state, environment reads, thread header, or linked worker implementation.

The ABI has exactly three operations: attempt one child, run that child body
inline after a declined attempt, and join one successfully created child.
Worker and fallback bodies run in a marked worker scope. Any nested attempt is
declined, bounding one fork site to at most one extra worker and preventing a
recursive task tree.

## Alternatives

Making POSIX threads unconditional would violate the portable C backend.
Adding a permanent scheduler or third-party pool would add dependencies,
startup, queueing, allocation, and shutdown costs before application evidence
justifies them. Exposing task operations in SLIM would duplicate the compiler's
derived fork/join plan and make synchronization source-visible. Silently
running a declined task inside `spawn` would hide control flow and make result
installation harder to audit.

The tiered ABI keeps fallback explicit in generated C, but it adds no SLIM
syntax or callable built-in. A future target can implement the same contract
without changing source semantics.

## Costs

Selected programs on the POSIX tier pay one `pthread_create` and one
`pthread_join` per executed site. The base tier pays a predictable failed
attempt and direct call. No program links either path unless code generation
marks it as selected.

The reference runtime retains small process-global fault-injection counters.
They are initialized only in opted-in binaries. The no-nesting rule ensures a
worker never mutates those counters, so the accepted one-parent execution
model does not introduce a counter race.

This decision supplies only the worker mechanism. Automatic production
execution remains disabled until capture/result lowering, target profitability,
and multi-application evidence satisfy the remaining D0069 conditions.

## Evidence

The permanent runtime fixture compiles both tiers with strict C11 warnings. The
portable tier declines creation and reaches the expected result through inline
execution. On Unix, the POSIX tier creates and joins one worker. Its child
attempts a nested spawn, which is declined on both the worker and fallback
paths. `SLIM_TASK_FAIL_AT=1` and `SLIM_TASK_DISABLE=1` deterministically prove
serial fallback; `SLIM_TASK_JOIN_FAIL_AT=1` proves that an external join failure
becomes the existing defined runtime trap rather than continuing with an
uninstalled result.

The focused end-to-end test passes for the base tier and the acceptance host's
POSIX tier. Existing builds do not define `SLIM_PARALLEL`, so their runtime
object remains worker-free.

## Removal

Do not make the POSIX tier unconditional, detach a worker, omit a successful
join, or allow nested creation. Do not expose this optimization ABI as language
surface. A pool, another platform tier, more than one child per site, task
failure, or cancellation requires a separately scored decision with permanent
ordering, fallback, and failure tests.
