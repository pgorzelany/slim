# RFC-0071: Guarded automatic fork/join

Status: accepted
Implementation: complete
Process: legacy
Kind: runtime
Primitive: none
Safety: 2
Compile: 0
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 0
Score: 75

## Need

RFC-0069 deferred automatic execution until a tiered worker ABI, checked
capture/result lowering, serial and failure behavior, bounded nesting, a
target-relevant cost model, and positive evidence on more than one substantial
application existed. RFC-0070 supplies the worker boundary. This decision admits
one deliberately narrow executable subset and satisfies the remaining
conditions without adding source syntax.

The compiler reuses the checked typing, ownership, effect, integer, recurrence,
and deterministic schedule views. A selected site is executable only when both
initializers are direct user calls with atomic checked arguments, both callees
are proven total and reorder-safe, the site is on the function's leading `let`
chain supported by lowering, and each task has an exact recurrence count of at
least 1,000,000 iterations. Missing evidence disables execution.

Generated C captures each call in a typed stack context, attempts the first
wrapper on one worker, runs the second wrapper in worker scope on the parent,
joins the worker, and installs both results into their original immutable
bindings before the continuation. A declined spawn invokes the identical first
wrapper inline. Worker scope on both branches prevents a callee from expanding
the plan while its sibling is active.

## Alternatives

Language-level tasks, locks, futures, and annotations duplicate a plan the
compiler can derive and enlarge the safety surface. A permanent scheduler or
worker pool adds allocation, queues, lifetime management, and startup costs
before evidence supports them. Parallelizing arbitrary expressions complicates
evaluation order and capture ownership. Using source-token size as work repeats
the cost-model error rejected by RFC-0069.

Running any reported site would confuse proof reporting with supported
lowering. The leading-chain check instead makes executable analysis and code
generation agree exactly; nested or otherwise unsupported sites remain visible
but serial.

## Costs

Production C emission now constructs the bounded range and parallel views even
when no site executes. This is linear, shares the normal checked artifact, and
retains the existing compiler-scaling budgets. Unselected generated programs
contain no parallel macro, context, wrapper, task state, environment read,
thread include, or link flag.

An executed POSIX site pays one `pthread_create`, one `pthread_join`, a stack
context, and typed argument copies. The current `posix-v1` model therefore
requires exact work far above the measured crossover. It creates at most one
child; worker scope declines nested creation. Unsupported targets select the
portable serial tier automatically and preserve behavior.

## Evidence

`state_machine` and the independent eight-Boolean `signal_network` application
each contain two exact two-million-iteration total recurrences. Analysis schema
5 records one candidate, selected, reported, executable, and executed site for
each. Generated execution and `SLIM_TASK_DISABLE=1` produce identical output.
The state-machine fixture also covers forced spawn failure and the defined
join-failure trap.

On the acceptance host, quick warmed median generated parallel/forced-serial
ratios were 0.724 for `state_machine` and 0.695 for `signal_network`. Both
applications have permanent same-host ratio budgets. The earlier geometric
manual reference remains in the same release command to retain worker-overhead
and crossover evidence.

A nested profitable fixture is reported but records zero executable and
executed sites because it lies outside the supported leading chain. An ordinary
hello program emits byte-identical worker-free structure. Fixed-point bootstrap,
the native comparison suite, compiler scaling, fallback, no-nesting, and all
prior safety gates remain mandatory.

## Removal

Do not expose tasks as source surface, execute an unknown or merely selected
site, lower outside the analyzer's exact executable subset, omit serial
fallback or a successful join, share mutable or allocating work, or create a
recursive worker tree. Do not lower the work threshold or weaken a generated
parallel ratio budget without a separately scored decision and same-host
evidence. More workers, computed captures, nested sites, a pool, cancellation,
task failure, or another target model each require new proof and permanent
tests.
