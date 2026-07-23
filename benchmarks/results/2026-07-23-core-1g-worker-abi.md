# Core 1G tiered-worker checkpoint

Date: 2026-07-23
Decision: D0070
Status: accepted runtime foundation; generated execution still disabled

## Contract

The worker ABI is compiled only when `SLIM_PARALLEL` is defined. Its portable
base tier always declines creation and uses an explicit inline call. Defining
`SLIM_POSIX_WORKERS` adds one POSIX child and one parent-owned join. Both the
worker and fallback enter worker scope, where a nested spawn is declined.

This adds no source syntax, compiler primitive, dependency, scheduler, detached
task, lock, channel, cancellation path, or worker cost to unselected programs.

## Permanent fixture

`tests/fixtures/parallel_runtime.c` exercises an outer task whose body attempts
another task and increments one result. The end-to-end test compiles it with
strict warnings and establishes:

| tier or fault | outer created | nested created | result | outcome |
|---|---:|---:|---:|---|
| portable base | 0 | 0 | 42 | inline success |
| POSIX | 1 | 0 | 42 | joined success |
| POSIX spawn failure | 0 | 0 | 42 | inline success |
| POSIX disabled | 0 | 0 | 42 | inline success |
| POSIX join failure | 1 | 0 | unavailable | defined exit 70 trap |

The focused Rust-owned verification test passes on the acceptance Unix host.
This is a runtime ABI test, not an application speedup claim.

## Remaining execution gate

Core 1G must still derive an executable plan from the checked SLIM view, lower
general ownership-safe argument capture and result installation, admit only
target-profitable work, emit zero worker machinery for unselected programs,
and retain positive behavior and performance evidence for at least two
substantial applications.
