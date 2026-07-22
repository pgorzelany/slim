# SLIM

SLIM is an experimental systems language designed for AI-generated programs:
small canonical syntax, strong static typing, explicit effects, affine
ownership, exclusive borrows, deterministic output, and no tracing garbage
collector.

The repository contains a working Rust stage-0 compiler, a small C runtime, a
compiler written in SLIM itself, governance gates for language growth, scaling
tests, and comparable SLIM/C/Rust benchmark programs.

## Try it

    cargo run --bin slimc -- check examples/hello.slim
    cargo run --bin slimc -- run examples/hello.slim
    cargo run --bin slimc -- emit-c examples/hello.slim -o /tmp/hello.c

For an explicit multi-module project:

    cargo run --bin slimc -- run conformance/projects/basic/slim.project
    cargo run --bin slimc -- interfaces conformance/projects/basic/slim.project -o /tmp/slim-interfaces

The accepted canonical Core and compiler commands are documented in
[docs/CORE.md](docs/CORE.md). Design goals and known theoretical limits are
recorded in [DESIGN.md](DESIGN.md). Sequenced implementation milestones and
their acceptance gates are in [ROADMAP.md](ROADMAP.md).

## Verify everything

    ./scripts/verify.sh

This runs formatting, Clippy, unit and integration tests, feature-governance
checks, the self-host fixed-point proof, frontend scaling checks, and a
sanitized native execution test. It also runs the standalone conformance
oracle through both compiler stages and checks incremental work bounds.

For the bootstrap proof alone:

    cargo run --bin slim-bootstrap

For the executable Core oracle and incremental measurements:

    cargo run --bin slim-conform -- differential
    cargo run --release --bin slim-bench -- incremental
    cargo run --release --bin slim-bench -- project

## Current boundary

Stage 0 implements Core 0 and provides the reference diagnostics. The
self-hosted compiler is a genuine deterministic compiler, not a wrapper. Core
0.4 is frozen with all 58 standalone and project fixtures at self-host parity.
Its thirteen SLIM modules implement structured syntax and typed declarations,
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
New compiler capabilities default to SLIM under an enforced production-Rust
budget.
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
Language-level concurrency, automatic program parallelization, whole-program
quality grading, and general optimal reduction remain designed goals rather
than implemented claims. The 0.3 freeze evidence and remaining performance gap
are recorded in [benchmarks/results/2026-07-21-core-03.md](benchmarks/results/2026-07-21-core-03.md).
