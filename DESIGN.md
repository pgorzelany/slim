# SLIM Language Design

Status: Core 1F proof infrastructure and research direction
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
compiler view. Core 1F now derives a bounded integer interval and totality view
from those same token identities and types. Missing bounds remain unknown, and
no proof analysis changes ordinary checking or generated code.

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

## Provisional Design Directions

The following ideas are promising consequences of the requirements, but are not
yet final decisions:

- Ownership, affine types, regions, and deterministic destruction for memory
  management
- Immutable values by default with uniquely owned mutation
- Algebraic data types and exhaustive pattern matching
- An effect and capability system that exposes mutation, allocation, I/O,
  failure, blocking, spawning, and foreign calls
- Structured fork/join concurrency, deterministic data-parallel operations, or
  typed communication protocols
- Pure maps, pipelines, and associative reductions that the compiler can safely
  parallelize
- A small canonical typed core representation beneath a readable surface syntax
- Incremental, query-based compilation with independently checkable modules
- Machine-readable diagnostics with source spans, fix suggestions, and proof
  obligations
- Specialized analyzable domains for pure computations, parallel dataflow, and
  finite-state machines
- First-class bounded functions and machines with declared or inferred limits
  on memory, steps, inputs, tasks, and communication queues
- A distinction between total functions, productive long-running processes,
  and explicitly partial computations
- Defined resource-exhaustion results for bounded computations
- Explicit unsafe and foreign-function boundaries

## Open Questions

1. What is the smallest useful semantic core of the language?
2. Should SLIM require ownership annotations, infer them, or combine both?
3. Which memory patterns need explicit regions, arenas, handles, or reference
   counting?
4. Which concurrency primitives can provide both usability and static deadlock
   freedom?
5. How should the compiler decide whether proven parallelism is profitable?
6. Which effects should be represented in function types?
7. How much type inference is compatible with predictable, fast compilation?
8. Which subset should guarantee termination or support full normalization?
9. What constitutes observable behavior when comparing two programs?
10. Which program-quality metrics should be enforced, reported, or used only as
    optimization guidance?
11. What cost models should reduction and optimization support?
12. Should AI tools manipulate source text, a canonical syntax tree, or both?
13. How should interoperability with C, operating systems, and existing
    libraries affect the safety model?
14. Which resource bounds belong in types, function contracts, build profiles,
    or target descriptions?
15. Should portable and target-bounded semantics be separate language profiles
    or different guarantee levels within one language?
16. How should bounded internal state and unbounded external event streams be
    represented and verified?

## Current Non-Goals

Until explicitly reconsidered, SLIM does not promise:

- Millisecond clean optimized builds for programs of arbitrary size
- Static deadlock freedom in the presence of unrestricted locks or foreign code
- Automatic profitable parallelization of arbitrary algorithms
- Decidable equivalence for arbitrary programs
- A universal globally optimal normal form for every program
