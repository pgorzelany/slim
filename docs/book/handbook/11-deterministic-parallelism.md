# Deterministic parallelism

SLIM admits only structured parallel work whose checked shape preserves
determinism, race freedom, deadlock freedom, and the serial meaning of the
program.

## Explicit structured `parallel`

`parallel` admits two independent leading direct leaf-call bindings, waits for both,
and installs results in lexical order before the remaining block.

<!-- slim-fixture: structured-parallel -->

Accepted tasks are checked leaf functions with restricted inputs, no `@`
exclusive parameters,
no `partial`, isolated allocation regions, and only the bounded host operation
shape recorded by the parallelism contract.

## Declined execution

Selection and execution are distinct. If a platform worker is unavailable or a
checked site does not meet the execution threshold, the identical task body
runs inline. Every successful spawn joins exactly once, and worker scope
declines nested spawning.

## Automatic candidates

The analyzer may discover reorder-safe recurrence candidates. Candidate,
selected, executable, and executed sites remain distinct. Selection follows
stable lexical node order and emits pairwise non-overlapping intervals.

Unknown work, computed captures, unsupported placement, bounded-away facts, and
unreported sites remain serial.

## Rejected shapes

An effectful or otherwise unsupported task boundary is rejected rather than
silently acquiring weaker semantics.

<!-- slim-fixture: invalid-structured-parallel -->

## Exact rules and common errors

Safe accepted sites are data-race-free and deadlock-free by construction.
Declined spawning preserves identical behavior. Generated parallel/forced
serial ratios are permanent same-host regression contracts for accepted
application shapes. Unsupported tasks are rejected or remain serial rather
than acquiring weaker semantics.

There are no threads, locks, atomics, channels, futures, scheduler handles,
detached tasks, or general shared-memory concurrency APIs. See
[Parallelism](../../PARALLELISM.md).

## Next

Continue with [compiler tools and evidence](12-compiler-tools-and-evidence.md).
