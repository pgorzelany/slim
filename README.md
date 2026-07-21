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

## Current boundary

Stage 0 implements Core 0 and provides the reference diagnostics. The
self-hosted compiler is a genuine deterministic compiler, not a wrapper, but
currently implements the function/vector subset required to compile itself.
Core 0.1 also has declaration-local in-memory incremental compilation with
stable syntax/interface fingerprints and dependency-aware invalidation; its
architecture and measured remaining whole-file costs are documented in
[docs/INCREMENTAL.md](docs/INCREMENTAL.md).
Concurrency, automatic parallelization, whole-program quality grading, and
general optimal reduction remain designed goals rather than implemented
claims.
