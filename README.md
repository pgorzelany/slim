# SLIM — Small Language for Intelligent Machines

SLIM stands for **Small Language for Intelligent Machines**. It is an
experimental systems language designed for AI-generated programs: small
canonical syntax, strong static typing, explicit effects, affine ownership,
exclusive borrows, deterministic output, and no tracing garbage collector.

The repository contains a compiler written in SLIM itself, a portable generated
C bootstrap seed, a small C runtime, governance gates for language growth,
scaling tests, and comparable SLIM/C/Rust benchmark programs. The former Rust
semantic compiler has been removed from the active tree and remains available
only through Git history.

## Try it

    ./slimc check examples/hello.slim
    ./slimc run examples/hello.slim
    ./slimc emit-c examples/hello.slim -o /tmp/hello.c
    ./slimc reduce conformance/pass/reduction.slim
    ./slimc analyze examples/vector_sum.slim
    ./slimc prove-reduction conformance/pass/reduction.slim
    ./slimc equivalent conformance/evidence/equivalent-left.slim conformance/evidence/equivalent-right.slim
    ./slimc edit conformance/evidence/equivalent-left.slim conformance/evidence/edit.patch

For an explicit multi-module project:

    ./slimc run conformance/projects/basic/slim.project
    ./slimc interfaces conformance/projects/basic/slim.project -o /tmp/slim-interfaces

The accepted canonical Core and compiler commands are documented in
[docs/CORE.md](docs/CORE.md). Design goals and known theoretical limits are
recorded in [DESIGN.md](DESIGN.md). Sequenced implementation milestones and
their acceptance gates are in [ROADMAP.md](ROADMAP.md).

## Verify everything

    ./scripts/verify.sh

This runs formatting, Clippy, unit and integration tests, feature-governance
checks, the self-host fixed-point proof, frontend and direct-reduction scaling
checks, and sanitized compiler and native execution tests. It also runs every
conformance fixture through the production SLIM compiler, checks reduction
idempotence and behavior preservation, and checks incremental work bounds.

For the bootstrap proof alone:

    ./bootstrap.sh

For the executable Core oracle and incremental measurements:

    cargo run --bin slim-conform -- differential
    cargo run --release --bin slim-bench -- incremental
    cargo run --release --bin slim-bench -- project
    cargo run --release --bin slim-bench -- agent

## Current boundary

The self-hosted compiler is a genuine deterministic compiler, not a wrapper.
The checked-in C11 seed builds it without Rust and reproduces the compiler C
byte for byte. Core 1B is frozen with all 62 standalone and project fixtures
running through the SLIM compiler with no semantic fallback.
Its twenty SLIM modules implement structured syntax, bounded malformed-input
validation, and typed declarations,
standalone and project checking, deterministic C generation,
declaration-level query invalidation, transactional last-good sessions, a
bounded validated persistent cache, deterministic bounded scheduling, and a
SLIM-owned bounded lifetime planner. Core 0.4 adds compiler-selected function
regions, self-describing region allocations with deterministic reverse
destruction, and a typed allocation-effect failure channel. It adds no source
syntax, allocator alias, reference counting, or garbage collector. Detailed
per-binding planning is capped at 64 values and conservatively falls back to a
function boundary; dynamic-buffer stack promotion and earlier lexical release
remain future precision work.
New compiler capabilities must be implemented in SLIM. Governance rejects
production Rust semantics and permits Rust only for bootstrap-independent
verification and measurement infrastructure.
Core 0.1 also has declaration-local in-memory incremental compilation with
stable syntax/interface fingerprints and dependency-aware invalidation; its
architecture and measured remaining whole-file costs are documented in
[docs/INCREMENTAL.md](docs/INCREMENTAL.md).
Core 0.2 adds explicit deterministic projects, qualified cross-module
references, canonical public interfaces, safe persistent cache recovery,
declaration-level project reuse, and an opt-in deterministic worker path. The
measured worker path is currently slower than serial and is not enabled by
default. Core 0.3's self-host scheduler likewise plans bounded dependency
batches but executes them serially because Core has no concurrency runtime.
Language-level concurrency, automatic program parallelization, inferred
application invariants, general equivalence, and globally optimal reduction
remain designed goals rather than implemented claims. Core 1A adds a small
direct reducer and bounded semantic facts without new source syntax or a
separate IR language. Core 1B adds classified quality evidence, independently
replayed reduction records, exact finite Boolean equivalence with concrete
counterexamples, and one versioned node edit protocol. Its limits are
documented in [docs/QUALITY.md](docs/QUALITY.md), alongside the Core 1A contract
in [docs/REDUCTION.md](docs/REDUCTION.md). The 0.3 freeze evidence and remaining
performance gap are recorded in
[benchmarks/results/2026-07-21-core-03.md](benchmarks/results/2026-07-21-core-03.md).
