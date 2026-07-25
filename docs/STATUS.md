# SLIM Status

Status: SLIM 1.0 released
Compiler version: 1.0.0
Next milestone: Post-1.0 evidence-driven development
Updated: 2026-07-24

SLIM is the Small Language for Intelligent Machines: a compact systems
language for AI-generated programs. Its production compiler is written in
SLIM, bootstraps from a checked-in portable C11 seed, and reproduces that seed
byte for byte. Core 1H through 1L are accepted and the 1.0 surface is governed
by an explicit compatibility contract.

## Implemented

- One canonical two-space significant-indentation source form; strong static
  types; explicit `alloc`, `io`, and `partial` effects; checked arithmetic and
  indexing.
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
- A permanent twenty-application SLIM/C/Rust performance corpus with
  proof-directed direct arithmetic lowering for exact total checked nodes.
- One canonical portable `-O3` native build and typed `vec.set` lowering that
  retains the shared bounds check while avoiding generic byte copies.

## Deliberate bounds

- Integer proofs use a fixed `-1,000,000,000..1,000,000,000` domain; retained
  parameter propagation uses four passes; retained proof, lifetime, graph,
  report, and rewrite tables have documented limits.
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

Core 1L added no language feature. D0082 freezes the source/project surface,
diagnostics, tooling schemas, compatibility policy, and runtime ABI. D0083
accepts the reproducible source archive, clean-install gate, complete release
suite, and matching canonical website.

The normative language, compatibility, diagnostics, and release contracts are
`docs/CORE.md`, `docs/COMPATIBILITY.md`, `docs/DIAGNOSTICS.md`, and
`docs/RELEASE.md`. Detailed subsystem evidence remains in the other documents,
decisions, and dated benchmark results.

Post-1.0 safe performance work is active under D0084 through D0087. The current
expanded full candidate measures 1.273× C overall and 1.337× across serial
applications, so the repository does not yet claim C parity. The next material
boundary is proof-backed collection reads or safe partitioning; no new language
feature has been accepted for it.
