# SLIM Language Design

Status: Core 1G guarded automatic parallel execution
Name: **SLIM — Small Language for Intelligent Machines**
Last updated: 2026-07-23

## Vision

SLIM, the Small Language for Intelligent Machines, is a programming language
designed for AI to write, inspect, transform, and reason about. It should
produce programs that remain readable to humans while using a compact,
token-efficient syntax.

SLIM aims to combine systems-language performance and static safety with strong
support for program analysis, deterministic concurrency, and mechanical
simplification.

This document records the durable design choices and research aspirations.
Core now has a concrete canonical syntax, type/effect/ownership checker,
deterministic C backend, runtime, benchmark suite, and a restricted
self-hosting compiler. Later sections distinguish implemented mechanisms from
longer-term goals.

## Design Choices and Requirements

### 1. Extremely fast compiler feedback

- Compilation and checking should feel effectively instantaneous.
- Large programs should receive feedback in milliseconds where practical,
  especially after incremental edits.
- Compiler diagnostics must be robust, precise, and useful for both humans and
  AI agents.
- Diagnostic output should make errors mechanically actionable rather than
  merely describing them.

The milliseconds goal primarily applies to parsing, checking, and incremental
development feedback. Clean, fully optimized native builds will necessarily
take longer for sufficiently large programs.

### 2. Compact but readable expression

- The language should be expressive with a small number of orthogonal concepts.
- Syntax should be simple, regular, and readable.
- Source code should preserve tokens without becoming cryptic or ambiguous.
- There should be few redundant ways to express the same operation.
- The language should be easy for an AI to generate and modify reliably.

Token count is an important design constraint, but semantic clarity and reliable
parsing take priority over punctuation-level terseness.

### 3. Static memory management

- SLIM should not require a tracing garbage collector.
- Memory lifetimes should normally be known at compile time.
- Runtime memory-management overhead should be absent or minimal.
- Resource release should be deterministic.
- Unusual cases that require runtime ownership mechanisms should be explicit.

### 4. Memory safety by default

- Safe SLIM programs must not contain use-after-free, double-free, dangling
  references, invalid aliasing, or out-of-bounds memory access.
- Safety should be enforced by the compiler rather than left to convention.
- Any unverifiable or unsafe operation must be isolated and visibly marked.

### 5. Safe and inexpensive parallelism

- Programs should be easy to parallelize.
- Safe code should be free of data races by construction.
- The compiler should prevent deadlocks wherever the language's concurrency
  model permits a static guarantee.
- The compiler should discover and introduce parallel execution when it can
  prove that doing so preserves semantics and is likely to be beneficial.
- Task creation, synchronization, scheduling, and data movement overhead should
  be kept low.
- Parallel execution should preferably be deterministic.

Guaranteeing deadlock freedom requires restricting concurrency primitives.
Arbitrary locks, unrestricted communication, and foreign code cannot receive a
general static deadlock-freedom guarantee.

Core 1G executes a deliberately narrow subset of Core 1F's bounded,
deterministic, pairwise non-overlapping fork/join plan. The compiler requires
complete totality and reorder-safety, atomic typed captures, a codegen-supported
leading `let` chain, and exact work above the measured `posix-v1` threshold.
One child and one parent-owned join guarantee bounded, deadlock-free structure;
spawn failure runs the identical task inline. Unknown, unsupported, and
unprofitable sites remain serial, and unselected programs pay no worker cost.

### 6. Systems-level performance

- Runtime performance should be competitive with Rust and C++.
- The language should permit predictable data layout and memory use.
- Abstractions should have little or no unavoidable runtime cost.
- Hidden allocation and hidden synchronization should be avoided.

Compile speed and maximum runtime optimization are competing objectives. SLIM
may therefore need distinct fast-development and optimized-build modes.

### 7. Strong static typing

- SLIM should be strongly and statically typed.
- The type system should prevent invalid programs while remaining understandable
  through good diagnostics.
- Types should help represent domain rules so invalid states are not
  representable.

Core 1D replaced shallow and repeated token interpretation with one checked
compiler view. Core 1F derives a bounded integer interval and totality view from
those same token identities and types. Missing bounds remain unknown. Core 1G
allows only exact facts from that view to enable guarded code generation;
ordinary checking remains unchanged.

### 8. Programs should be measurable

SLIM should make it possible to evaluate program quality using explicit,
machine-checkable metrics where possible. Complexity is one such metric, but not
the only one.

Candidate metrics include:

- Number of representable states
- Number of valid and reachable states
- Ratio of representable states to valid states
- Number of independent mutable locations
- Aliasing and ownership complexity
- Effect surface: I/O, mutation, allocation, failure, blocking, and
  nondeterminism
- Number of failure and partial-operation paths
- Concurrency and synchronization complexity
- Allocation count and peak live memory
- Worst-case time and space bounds when derivable
- API surface and number of legal operation sequences
- Capability or authority surface
- Number and difficulty of proof obligations
- Structural, cyclomatic, and cognitive complexity

For example, ten independent properties with ten possible values describe
`10^10` representable states. If only a small fraction are valid, a better type
model should encode only the legal combinations. Comparisons of this kind
require declared invariants or a behavioral specification; the compiler cannot
infer intent from representation alone.

Core 1B implements a first conservative metric vector over canonical checked
SLIM: primitive representable-state cardinality, structural/effect/failure and
reduction sites, ownership pressure, dependency facts, and a narrow totality
classification. Every fact is labelled exact, bounded, or unknown. It does not
claim to measure valid or reachable application states without a specification.

Core 1F strengthens totality for a bounded integer domain. It proves selected
literal, immutable-flow, guarded arithmetic, denominator-safe
division/remainder, checked conversion sites, and one canonical strictly
decreasing tail recurrence. Mutation, collection bounds, general recurrence,
interprocedural return ranges, and out-of-domain values remain unknown.

### 9. Programs should be reducible

- The compiler or associated tooling should simplify programs through
  semantics-preserving transformations.
- Equivalent constructions should converge toward a smaller, more canonical
  representation wherever possible.
- The system should be able to explain or record the transformations it makes.
- Optimization must use an explicit cost model, because "optimal" can mean
  source size, runtime, memory use, compile time, parallelism, energy use, or
  ease of proof.

A universal algorithm that reduces every possible program to a unique globally
optimal equivalent form is not possible for a general Turing-complete language.
Strong canonicalization and minimization remain possible for restricted
subsets, finite-state systems, pure expressions, and bounded transformation
systems.

### 10. Physical limits should be expressible

A physical digital computer has finite memory and therefore a finite number of
machine states. Turing completeness describes an idealized language with
arbitrarily extendable resources, not any single physical computer.

SLIM should expose this distinction instead of always hiding it behind an
unbounded abstract machine:

- Portable programs may use scalable, conceptually unbounded semantics and may
  therefore have the usual limits of Turing-complete computation.
- Bounded programs should be able to declare or derive limits on inputs,
  memory, execution steps, recursion, tasks, queues, and other resources.
- Exceeding a declared resource limit must have defined behavior rather than
  causing undefined behavior.
- Resource bounds should participate in program analysis and, where useful, in
  types or contracts.
- A compiler should report the strongest guarantee it established and the
  assumptions or bounds on which that guarantee depends.

For a completely fixed machine and bounded environment, termination, state
reachability, and behavioral equivalence are decidable in principle because the
complete system is finite-state. The state space may still be far too large to
enumerate. SLIM should therefore use types, invariants, symbolic methods,
abstraction, and compositional proofs rather than depending on exhaustive search
alone.

Long-running services require a further distinction: their internal state can
remain bounded while their sequence of external inputs is indefinitely long.
SLIM should model such programs as productive processes or finite-state
transducers where appropriate, rather than treating all non-termination as the
same phenomenon.

## Fundamental Boundaries

The design should acknowledge these limits rather than hiding them:

- Reading, parsing, optimizing, and generating code impose lower bounds on clean
  build time.
- Equivalence of arbitrary programs is undecidable.
- Termination of arbitrary programs is undecidable.
- Deadlock freedom is undecidable for unrestricted concurrency.
- Profitable automatic parallelization cannot be determined for all programs
  and inputs.
- No single optimal program exists without a chosen behavioral specification
  and cost model.

These undecidability results apply to the language's general, effectively
unbounded model. On one fixed finite target they become finite-state questions,
but the resulting state space is normally too large for naive exhaustive
analysis to be useful.

SLIM can provide stronger guarantees by defining analyzable subsets and making
those subsets the natural way to write most programs.

## Established implementation choices

Core 1G already has affine ownership, immutable bindings, exclusive mutation,
algebraic data types, explicit effects, deterministic projects, incremental
checking, machine-readable diagnostics, a retained typed compiler view,
bounded analysis, direct-source reduction, and guarded automatic fork/join.
There is no second executable IR, garbage collector, source-level task system,
or unrestricted foreign boundary.

## Remaining research questions

The Core 1H through Core 1L sequence in `ROADMAP.md` focuses the remaining
design work:

1. Which resource bounds can be derived, and which require one explicit
   contract?
2. How should bounded internal state interact with indefinite external event
   streams?
3. What is the smallest safe host-capability boundary for real applications?
4. Can one structured effectful-concurrency form cover demonstrated needs while
   preserving race and deadlock freedom?
5. Which invariants and cost models make state-space comparison and reduction
   useful without claiming one universal quality score?
6. Which behaviors and artifacts must be frozen for a credible 1.0
   compatibility promise?

## Current Non-Goals

Until explicitly reconsidered, SLIM does not promise:

- Millisecond clean optimized builds for programs of arbitrary size
- Static deadlock freedom in the presence of unrestricted locks or foreign code
- Automatic profitable parallelization of arbitrary algorithms
- Decidable equivalence for arbitrary programs
- A universal globally optimal normal form for every program
