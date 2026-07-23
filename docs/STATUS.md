# SLIM Status

Status: Core 1L release stabilization in progress
Compiler version: 1.0.0
Next milestone: SLIM 1.0 release acceptance
Updated: 2026-07-23

SLIM is the Small Language for Intelligent Machines: a compact systems
language for AI-generated programs. Its production compiler is written in
SLIM, bootstraps from a checked-in portable C11 seed, and reproduces that seed
byte for byte. Core 1H through 1K are accepted; Core 1L is freezing and
packaging the result.

## Implemented

- One canonical S-expression source form; strong static types; explicit
  `alloc`, `io`, and `partial` effects; checked arithmetic and indexing.
- Records, variants, exhaustive matches, affine ownership, immutable bindings,
  exclusive non-escaping `inout` borrows, compiler-selected regions, and
  deterministic destruction without tracing garbage collection.
- Deterministic C11 generation, native build/run, stable diagnostics, JSON
  schema 1, typed allocation failure, and reproducible self-hosting.
- Explicit deterministic projects, canonical interfaces, validated caches,
  declaration reuse, and bounded parallel project checking.
- Bounded analysis schema 7, named cost vectors, exact Boolean-product or
  single-byte equivalence, replayed reduction proofs, and structural edits.
- Guarded automatic fork/join for proven pure work and one lexical two-call
  `fork` for bounded host work, both with serial fallback and no locks or
  detached tasks.
- One monotonic millisecond clock and one bounded whole TCP exchange with
  numeric addresses, explicit limits, typed failure, and no source handle.

## Deliberate bounds

- Integer proofs use a fixed `-1,000,000,000..1,000,000,000` domain; retained
  proof, lifetime, graph, report, and rewrite tables have documented limits.
- Exact equivalence covers at most eight Boolean parameters or one `U8`.
  Reduction is a small terminating, idempotent rule set, not global
  optimization.
- Automatic parallel execution requires two leading independent, total,
  reorder-safe calls with exact profitable work. Explicit `fork` admits two
  checked bounded leaf calls and joins both before continuing.

## Not implemented

Futures, async/await, task handles, cancellation, locks, channels, detached
tasks, DNS, TLS, raw host handles, wall-clock time, general FFI, unsafe source,
general termination/equivalence proofs, global optimal reduction, source
resource contracts, LLVM, and a second executable IR are absent.

## Release boundary

Core 1L adds no language feature. D0082 freezes the source/project surface,
diagnostics, tooling schemas, compatibility policy, and runtime ABI. Remaining
acceptance work is a reproducible source archive, clean-install validation,
full release gates, and publication of the matching canonical website.

The normative language, compatibility, diagnostics, and release contracts are
`docs/CORE.md`, `docs/COMPATIBILITY.md`, `docs/DIAGNOSTICS.md`, and
`docs/RELEASE.md`. Detailed subsystem evidence remains in the other documents,
decisions, and dated benchmark results.
