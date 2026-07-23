# SLIM Status

Status: Core 1I safe typed host boundary in progress
Compiler version: 0.0.1
Next milestone: Core 1I safe typed host boundary
Updated: 2026-07-23

SLIM is experimental. The production compiler is written in SLIM, bootstraps
from a checked-in portable C11 seed, and reproduces that seed byte for byte.
The language and tooling are usable for the maintained examples, conformance
corpus, compiler project, and benchmark applications, but the surface is not
yet a stable public release.

## Implemented

- One canonical S-expression source form with a deterministic formatter.
- Strong static types, explicit `alloc`, `io`, and `partial` effects, checked
  arithmetic, checked indexing, records, variants, and exhaustive matching.
- Affine ownership for aggregates, immutable bindings by default, exclusive
  non-escaping `inout` borrows, and deterministic destruction.
- Compiler-selected regions, typed allocation failure, and no tracing garbage
  collector or reference counting.
- Deterministic C11 generation, native build and run commands, stable
  diagnostics, JSON diagnostics, and allocation-failure testing.
- Deterministic multi-module projects, canonical interfaces, incremental
  declaration reuse, validated persistent caches, and bounded parallel project
  checking.
- Direct source reduction, replayed reduction proofs, bounded quality evidence,
  finite Boolean equivalence with counterexamples, and versioned structural
  edits.
- Guarded automatic fork/join execution for the exact Core 1G subset, with
  serial fallback, one parent-owned join, and no nested worker expansion.
- One effect-gated, allocation-free monotonic millisecond clock with no
  wall-clock or timer alias.

## Guarded or bounded

- Integer proofs use a fixed `-1,000,000,000..1,000,000,000` domain and retain
  at most 64 refinements.
- Parallel analysis retains at most 64 functions and 4,096 call edges, resolves
  the graph in at most 64 passes, and prints at most 64 selected sites.
- Exact finite equivalence currently supports bounded Boolean subjects rather
  than arbitrary programs.
- Reduction is a small deterministic, terminating, idempotent rewrite set, not
  a globally optimal normalizer.
- Per-binding lifetime planning retains at most 64 values before conservatively
  falling back to a function boundary.
- Automatic execution currently requires two adjacent leading direct calls
  with atomic captures, complete reorder-safety and totality proofs, and at
  least 1,000,000 exact recurrence iterations per task. Other sites stay
  serial.
- Analysis schema 6 retains at most 16 canonical recurrence profiles and
  prints at most 64 profiled calls. Literal controllers may have exact
  iteration work; nonliteral controllers and unsupported recurrences remain
  unknown.

## Not implemented

- Source-level tasks, futures, async/await, locks, channels, explicit
  parallelism, or a general concurrency scheduler.
- Network, socket, timer, foreign-function, or unsafe source interfaces.
- General profitable automatic parallelization, a worker pool, cancellation,
  or arbitrary capture and result lowering.
- General termination proofs, general program equivalence, inferred application
  invariants, or a globally optimal reduction algorithm.
- Source syntax for physical resource bounds or local recovery from allocation
  failure.
- A native backend beyond the portable C11 backend, release packages, or a
  compatibility-stable 1.0 surface.

## Next boundary

Core 1I through Core 1L are the remaining path to 1.0. D0074 closes Core 1H
without resource syntax: the 14-application baseline did not justify treating
structural counts as physical time or bytes. D0075 begins Core 1I with one
typed, `io`-effect-gated monotonic clock. Next, representative applications
must determine whether resource-bearing host services need nominal capability
values and which networking or process boundary is small enough to keep.
Every durable mechanism still requires a weighted decision, measured cost,
diagnostics, specifications, and permanent tests.

The detailed contracts remain in `docs/CORE.md`, `docs/MEMORY.md`,
`docs/PROJECTS.md`, `docs/REDUCTION.md`, `docs/QUALITY.md`,
`docs/PERFORMANCE.md`, `docs/PARALLELISM.md`, and
`docs/RESOURCE_BOUNDS.md`, and `docs/HOST.md`. The concise milestone sequence
and acceptance boundaries are in `ROADMAP.md`.
