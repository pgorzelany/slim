# SLIM Roadmap

Status: Core 1G guarded automatic parallel execution
Current milestone: Core 1H bounded resources and application evidence
Last updated: 2026-07-23

## Direction

SLIM grows by strengthening guarantees and implementation capability before
expanding language surface. Compiler analysis, ordinary libraries, and existing
forms are preferred over new primitives. Every durable change follows
`design/FEATURE_POLICY.md`; planning a milestone does not pre-approve its
features.

The path to 1.0 is:

1. Core 1H: bounded resources and application evidence.
2. Core 1I: a safe typed host boundary.
3. Core 1J: deterministic structured concurrency for effectful work.
4. Core 1K: stronger semantic quality and reduction.
5. Core 1L: compatibility and release stabilization.

LLVM, a second executable IR, unrestricted source FFI, detached tasks, locks,
and duplicate syntax are outside this roadmap unless new evidence reverses an
accepted decision.

## Completed foundations

Detailed implementation history belongs in `design/decisions/`; measured
acceptance evidence belongs in `benchmarks/results/`. This table is the durable
summary rather than a second copy of those records.

| Milestone | Result |
| --- | --- |
| Core 0.1 | Executable conformance, differential checking, stable declaration identities, and genuine incremental reuse. |
| Core 0.2 | Deterministic projects, canonical interfaces, validated persistent caches, and bounded module scheduling. |
| Core 0.3 | Full self-hosted compiler parity and a modular SLIM compiler project. |
| Toolchain cutover | The generated portable C11 seed replaced the Rust semantic compiler as the bootstrap trust artifact. |
| Core 0.4 | Affine ownership, compiler-selected regions, deterministic destruction, and typed allocation failure. |
| Core 1A | Direct checked-source analysis and a small terminating, idempotent reduction system. |
| Core 1B | Bounded quality evidence, replayed reduction proofs, finite Boolean equivalence, and structural edits. |
| Core 1C | Permanent compiler, runtime, incremental, comparison, and agent-oriented regression gates. |
| Core 1D | One retained checked compiler view for typing, ownership, effects, memory planning, diagnostics, and emission. |
| Core 1E | Safety-preserving native efficiency with the portable C11 backend. |
| Core 1F | Bounded totality, reorder-safety, and deterministic parallel-plan evidence. |
| Core 1G | Guarded automatic fork/join for one proven, profitable, race-free, and deadlock-free subset. |

Core 0.3 evidence is summarized in
`benchmarks/results/2026-07-21-core-03.md`; Core 0.4 through Core 1C have
milestone reports with matching names under `benchmarks/results/`. Core 1D
through Core 1G close with D0058, D0061, D0069, and D0071 plus their dated
reports.

### Core 1D: complete typed compiler view

Status: complete

Core 1D is accepted by D0058. The compiler retains one bounded typed artifact
instead of reconstructing semantics in later passes. Its permanent gates cover
typed links and plans, project source projection, diagnostics, adversarial
source shapes, fixed-point bootstrap, and scaling.

### Core 1E: safety-preserving native efficiency

Status: complete

D0061 accepts Core 1E. Header-visible checked fast paths and allocation-free
region elision reduce native overhead without unchecked profiles, new syntax,
LLVM, or a second executable IR. The twelve maintained programs remain within
their per-workload runtime budgets.

### Core 1F: deterministic parallelism evidence

Status: complete

Core 1F derives bounded integer proofs, complete blocker sets, total recurrence
facts, reorder-safety, and one deterministic non-overlapping schedule. D0069
kept execution disabled until the worker ABI, lowering, failure behavior,
profitability evidence, and a second application existed.

### Core 1G: guarded automatic execution

Status: complete

D0070 defines a tiered structured worker ABI. D0071 executes only two adjacent
leading direct calls with atomic captures, complete totality and reorder-safety
proofs, exact recurrence work above the accepted threshold, serial fallback,
one parent-owned join, and no nested worker expansion. Everything else remains
serial. The exact boundary is in `docs/PARALLELISM.md`.

## Core 1H: bounded resources and application evidence

Status: planned

Core 1H should make physical limits useful without pretending arbitrary
programs are decidable.

### Work

- Establish a maintained application corpus that is not shaped around one
  optimization.
- Derive execution-step, allocation, peak-live-memory, recurrence, task,
  effect, and failure-path evidence from the existing checked artifact.
- Classify every fact as `exact`, `bounded`, or `unknown`; unknown never means
  safe, cheap, or bad.
- Extend the existing analysis report before proposing source syntax.
- If applications require assumptions that cannot be inferred, evaluate
  exactly one resource-contract mechanism through the weighted feature policy.
- Give every enforced limit defined typed behavior.
- Let proven bounds inform quality comparisons, reduction, equivalence, and the
  automatic-parallel cost model without changing ordinary semantics.

### Acceptance

- Multiple maintained applications exercise each accepted fact and failure
  path.
- Analysis remains bounded, deterministic, approximately linear, and reuses
  the production checked artifact.
- Limits cannot introduce undefined behavior, hidden allocation, or a second
  execution profile.
- Permanent conformance, malformed-input, scaling, runtime, and fixed-point
  gates pass together.
- Any new surface has one semantic role, a scored decision, diagnostics,
  examples, and removal criteria.

## Core 1I: safe typed host boundary

Status: planned

Core 1I should make SLIM useful for real operating-system work while preserving
the default safety model.

### Work

- Measure application needs for files, clocks, timers, networking, process
  interaction, and platform services.
- Define a small typed capability boundary with explicit effects, ownership,
  failure, and resource limits.
- Keep raw pointers, unchecked aliasing, ambient authority, and arbitrary C
  declarations out of ordinary SLIM source.
- Use one host-operation model across the portable runtime and supported
  targets; do not add a second way to express existing I/O.

### Acceptance

- Representative applications use the boundary without compiler-specific
  escape hatches.
- Invalid handles, lifetime violations, authority leaks, and unreported host
  failures are rejected or represented explicitly.
- Unsupported targets fail predictably or expose a documented capability
  absence.
- The ordinary compiler and programs that do not use host services pay no
  material cost.

## Core 1J: deterministic structured concurrency

Status: planned

Automatic parallelism is suitable for proven pure CPU work, but effectful host
operations have observable intent that cannot generally be inferred. Core 1J
will evaluate one explicit structured form only if Core 1I applications prove
the need.

### Work

- Cover parallel host operations such as starting two requests and joining both
  results.
- Require lexical task ownership, bounded lifetime, one structured join, and
  explicit effect and failure composition.
- Preserve race freedom and deadlock freedom by construction; do not add
  detached tasks, locks, or worker-to-worker waits.
- Define cancellation, timeout, resource exhaustion, and deterministic result
  ordering before execution.
- Reuse Core 1G analysis and the host boundary instead of creating an unrelated
  scheduler type system.

### Acceptance

- At least two representative applications show material latency or throughput
  benefit.
- Every spawn is bounded and every started task is joined or cancelled within
  its lexical scope.
- Scheduling variance cannot change accepted deterministic behavior.
- Serial fallback, partial failure, cancellation, timeout, and limit exhaustion
  have permanent tests.

## Core 1K: semantic quality and reduction

Status: planned

Core 1K should compare programs against explicit specifications and cost models,
not one universal goodness score.

### Work

- Represent selected invariants so valid and reachable states can be
  distinguished from merely representable states.
- Extend bounded equivalence beyond the current finite Boolean subset where
  Core 1H bounds make the domain finite and tractable.
- Add named cost models for source size, runtime work, peak memory, effect
  surface, failure surface, and proof burden.
- Expand reduction only with terminating, deterministic, replayable
  transformations whose chosen cost strictly improves.
- Report counterexamples and proof boundaries rather than optimistic claims.

### Acceptance

- Equivalent and different results are independently checkable; unsupported
  cases remain `unknown`.
- Quality output remains a vector of evidence unless a caller explicitly
  selects a cost model.
- Reduction terminates, is idempotent for its declared domain, preserves
  behavior, and cannot silently trade safety for a lower score.
- Application evidence demonstrates that the added specification burden
  removes real invalid states or proof obligations.

## Core 1L: compatibility and release stabilization

Status: planned

Core 1L turns the accepted language into SLIM 1.0; it is not a feature sweep.

### Work

- Freeze the canonical surface, semantics, diagnostics, analysis schemas,
  project format, generated-runtime contract, and compatibility policy.
- Publish a compact language specification, tested tutorial, reference,
  migration policy, release artifacts, and reproducible bootstrap instructions.
- Validate supported platforms and the portable C11 backend; a new native
  backend is not required for 1.0.
- Define package and library boundaries only to the extent required by the
  maintained 1.0 applications.
- Remove provisional paths, obsolete compatibility code, and undocumented
  behavior before freezing.

### Acceptance

- The compiler builds from the checked-in seed and reproduces its fixed point on
  every supported release platform.
- The complete corpus, sanitizers, fault injection, malformed-input mutations,
  performance budgets, application suites, and website drift checks pass.
- Every accepted surface item maps to a decision, specification, diagnostic
  contract, conformance coverage, and compatibility rule.
- A clean installation can build and run the maintained applications without
  repository-internal knowledge.

## Milestone discipline

For every milestone:

1. Record the need and a measurable baseline.
2. Prefer proof precision, libraries, or compiler implementation over syntax.
3. Score each durable decision and reject duplicate semantics.
4. Implement the smallest complete slice in SLIM.
5. Add permanent correctness and performance gates before claiming a gain.
6. Freeze dated evidence and update `docs/STATUS.md`.
7. Do not call the milestone complete while any required behavior depends on an
   undocumented fallback.

The compiler remains experimental until Core 1L is accepted. A planned item may
be changed or removed when evidence fails; the design goals and feature policy
take precedence over milestone labels.
