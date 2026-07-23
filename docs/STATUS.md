# SLIM Status

Status: Core 1G guarded automatic parallel execution
Compiler version: 0.0.1
Next milestone: Core 1H bounded resources and application evidence
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

Core 1H through Core 1L are the planned path to 1.0. Core 1H begins with
application evidence and compiler-derived resource bounds; it does not
pre-approve resource syntax. Every durable mechanism still requires a weighted
decision, measured cost, diagnostics, specifications, and permanent tests.
Core 1G widens only when a new execution shape has positive
multi-application evidence.

The detailed contracts remain in `docs/CORE.md`, `docs/MEMORY.md`,
`docs/PROJECTS.md`, `docs/REDUCTION.md`, `docs/QUALITY.md`,
`docs/PERFORMANCE.md`, and `docs/PARALLELISM.md`. The concise milestone
sequence and acceptance boundaries are in `ROADMAP.md`.
