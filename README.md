# SLIM — Small Language for Intelligent Machines

SLIM stands for **Small Language for Intelligent Machines**. It is a
compact systems language designed for AI-generated programs: small
canonical syntax, strong static typing, explicit effects, affine ownership,
default shared borrows, explicit `@` mutation and `^` call-boundary transfer, deterministic
output, and no tracing garbage collector.

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

In a repository checkout, the [experimental source library](library/README.md)
contains reusable modules,
substantial application workloads, canonical API/reference tools, and a seeded
typed source generator. The [component guide](library/COMPONENTS.md) covers
catalog reconciliation, dependency plans, development summaries, observed operation
costs and checked project inputs. Their focused native checks pass; current-source
integration remains pending.
The library and checkout tools stay outside the frozen 0.9 source-release
manifest while RFC-0111 remains proposed.
Its deterministic corpus gate is:

    ./scripts/check-library-corpus.sh

The generated language website is published from this repository at
<https://pgorzelany.github.io/slim/>. The progressive
[SLIM Handbook](docs/HANDBOOK.md), canonical appendices, development contracts,
RFC archive, benchmark evidence, and benchmark-backed algorithm gallery are
all published there. Website generation fails if tracked Markdown is neither
published nor explicitly excluded.

## Verify everything

    ./scripts/verify.sh

This runs formatting, Clippy, unit and integration tests, feature-governance
checks, the self-host fixed-point proof, frontend and direct-reduction scaling
checks, and sanitized compiler and native execution tests. It also runs every
conformance fixture through the production SLIM compiler, checks reduction
idempotence and behavior preservation, and checks both snapshot invalidation
estimates and observed compiler work. Public retained checking and native builds
and M1 release verification are complete. See
[the measured status](docs/INCREMENTAL.md).

The full release and website gate is `./scripts/verify-0.9.sh`.

For the bootstrap proof alone:

    ./bootstrap.sh

For the executable Core oracle and compiler measurements:

    cargo run --bin slim-conform -- differential
    cargo run --release --bin slim-bench -- performance
    cargo run --release --bin slim-bench -- incremental
    cargo run --release --bin slim-bench -- work
    cargo run --release --bin slim-bench -- project
    cargo run --release --bin slim-bench -- applications
    cargo run --release --bin slim-bench -- compare
    cargo run --release --bin slim-bench -- parallelism
    cargo run --release --bin slim-bench -- resources
    cargo run --release --bin slim-bench -- host
    cargo run --release --bin slim-bench -- agent

## Current boundary

SLIM is currently **SLIM 0.9 — experimental, pre-1.0**. The
self-hosted compiler, portable C11 seed, language surface, bounded analysis,
reduction tools, memory model, and exact current limitations are summarized in
[docs/STATUS.md](docs/STATUS.md). That document is the canonical short status;
the compatibility and clean-release contracts are in
[docs/COMPATIBILITY.md](docs/COMPATIBILITY.md) and
[docs/RELEASE.md](docs/RELEASE.md), while decisions and dated benchmark records
retain detailed evidence.
