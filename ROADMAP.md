# SLIM Roadmap

Status: SLIM 0.9 — experimental, pre-1.0
Current milestone: Pre-1.0 evidence-driven development
Current goal: Reliable project checks and compiler dogfooding
Last updated: 2026-10-02

SLIM helps humans and agents build safe native software through precise compiler
feedback, explicit resource behavior and reproducible failures. The
[first measured development loop](benchmarks/results/2026-10-02-agent-development-loop.md)
is complete for its bounded scope: production semantic context, independent
conformance/cost evidence and six frozen repair trials. Strict acceptance is 2/3
per condition; general effectiveness remains unknown. Retain opt-in schema 1 and
freeze its breadth. The next outcome is reliable project contracts and a stronger
compiler/library dogfooding experiment.

[RFC-0112](design/rfcs/0112-agent-development-and-os-foundation.md) remains the
accepted successor direction. [RFC-0158](design/rfcs/0158-early-agent-development-loop.md)
amends its dependency order for this bounded slice after completed M0/M1, before
requiring all M2 migrations. The full M3 agent/debugger milestone still retains
its actual M2 dependencies. Milestone planning does not pre-approve features or
waive `design/FEATURE_POLICY.md`.

## Completed early agent loop

The completed bounded outcome implements and evaluates an interface that helps a
checked SLIM program before changing it. Reuse the production compiler's checked
source facts; keep ordinary checking as the sole acceptance authority. The
[context architecture decision](design/rfcs/0159-bounded-semantic-context.md)
specifies the supported source/project shape, identities, protocol and fixed
work/output budgets before production implementation.

| Deliverable | Dependencies | Exit evidence |
| --- | --- | --- |
| Bounded semantic context | Completed M0/M1; accepted context architecture | Production self-hosted SLIM implementation; documented facts, source association, limits and unsupported cases; deterministic output. |
| Independent validation | Frozen context contract and workloads | Positive, negative and diagnostic expectations; wrong/stale source and project identities rejected; exact limit boundaries crossed; geometric work and same-host query costs measured. |
| Repair feasibility pilot | Working context; frozen tasks and independent acceptance tests | Three paired tasks in six fresh Sol 6.1 extra-high sessions; 15 minutes and 24 compiler-wrapper operations per session; model tool-call/token observation and enforcement remain unknown where traces are unavailable; all successes, failures and interruptions retained. |
| Adoption decision | Validation, pilot and unchanged repository/release gates | Explicit adopt/narrow/reject disposition, observed costs and remaining unknowns; a bounded result stays bounded. |

The paired runs use the same model, reasoning configuration, ordinary tools,
libraries, initial tasks and acceptance tests. Context access is the declared
treatment. Freeze ordering, isolation, timing and budgets before measured runs;
keep human intervention, model tokens where observable, tool calls, repair
iterations, native/resource quality and time to an accepted result separate.
No outside model API spending is authorized. This feasibility pilot measures its
named tasks; broader effectiveness needs the later controlled evaluation.

This outcome excludes source migration, session-protocol expansion, incomplete
source acceptance, debugger value inspection, failure recording, provider
simulation and replay. Each needs its own accepted contract and evidence.

## Next substantial outcome

1. Reproduce and repair the existing manifest import/export canonical-order and
   uniqueness contract in production SLIM. Cross mixed-case and prefix ordering,
   duplicate and unsorted lists with positive/negative/diagnostic conformance;
   preserve the frozen pilot on its original compiler and measure costs.
2. Prepare a new preregistered evaluation protocol with explicit or queued
   concurrency, immutable per-operation source inputs, independent dispatch and
   submission observations, and component acceptance aligned with public docs.
   Keep the first protocol and every outcome unchanged.
3. Evaluate larger held-out compiler/library repairs with ordinary tools versus
   context before expanding context, sessions or source surface. Choose task
   domains and budgets before trials; keep native/resource quality independent.

Exit evidence is the reconciled project contract, permanent regression gates,
all frozen held-out outcomes and an explicit evidence-based next decision. No
new language feature, provider, dependency or budget relaxation is pre-approved.

## Dependencies and remaining milestones

M0 current-contract repairs and M1 retained checking/native sessions are complete
at their identified release checkpoints. Their costs and limits remain in the
[SLIM Next progress ledger](benchmarks/results/2026-09-05-slim-next-progress.md)
and [M1 closure report](benchmarks/results/2026-09-08-m1-closure.md).

| Track | Current state | Remaining deliverable and exit |
| --- | --- | --- |
| Early agent loop | Complete for bounded RFC-0158/0159 scope | Retain opt-in schema 1; costs and failed protocol/contract observations stay in the closure report. |
| M2: expressive safe core | Separately staged contracts and native experiments; source cutovers pending | Follow the [dependency slices](benchmarks/results/2026-09-08-m2-plan.md); accepted coupled ownership/lifetime/allocation contracts, two substantial applications, compiler/library migration, resource budgets and full release evidence precede closure. |
| M3: agent and debugger interface | Full milestone pending | Add remaining service/edit/evidence and debugger contracts, source/value tests and measured query behavior; early context alone cannot close it. |
| M4: reproducible component laboratory | Pending | The same native component under deterministic providers, generated properties, failure replay and bounded minimization; validate the provider model and incomplete recordings. |
| M5: systems resource contracts | Pending | Explicit authority/lifetime/cleanup and no-alloc/no-block contracts with exhaustion, call-path and native resource evidence. |
| M6: freestanding kernel substrate | Pending | One RISC-V/QEMU target, audited machine boundary, isolation and repeatable fault-containment scenarios. |
| M7: OS research slice | Pending | The maintained storage service across native runner, simulator and OS, with cross-environment faults, resource evidence and independent agent evaluation. |

The bounded storage service remains the eventual integration target. A separate
codec/parser and queue/allocation workload prevent one application from defining
every language decision. Library composition, proof precision and compiler
implementation take priority over new primitives. Broader automatic parallelism,
a new backend and additional source surface require demonstrated need.

## Decision and closure discipline

1. Record the need and freeze tasks, independent oracles, workloads and budgets.
2. Accept the smallest contract before dependent production changes; implement
   compiler capabilities in `selfhost/` through the portable seed.
3. Preserve canonical source, exact/bounded/unknown evidence and every permanent
   correctness, performance, parallel and native-analysis gate.
4. Measure actual work and costs separately from source/edit proxies and agent
   outcomes. Keep failures and contrary evidence at their original revisions.
5. Run the required AGENTS.md checks before committing compiler, runtime,
   benchmark or agent-tool changes, and the full release gate at outcome closure.
6. Adopt, narrow or reject from the stated evidence. Never call a milestone
   complete through an undocumented fallback or a roadmap label.

There are no calendar promises or performance-budget relaxations in this plan.
The current language and `docs/COMPATIBILITY.md` remain authoritative until an
accepted and implemented successor decision replaces their named contract.

## Completed foundations

The records below preserve historical milestone facts at their original
revisions. They are foundations, not claims that every historical implementation
is present in the current compiler. Detailed acceptance and measurements remain
in linked RFCs and reports.

| Milestone | Historical result and record |
| --- | --- |
| Core 0.1 | Executable conformance, differential checking and stable declaration identities. Historical Rust incremental reuse is not current production reuse. |
| Core 0.2 | Deterministic projects, canonical interfaces, validated caches and bounded module scheduling. |
| Core 0.3 | Self-hosted compiler parity and a modular SLIM project; [acceptance report](benchmarks/results/2026-07-21-core-03.md). |
| Toolchain cutover | The generated portable C11 seed replaced Rust semantics as the bootstrap trust artifact; [RFC-0027](design/rfcs/0027-portable-c-bootstrap-seed.md). |
| Core 0.4 | Affine ownership, compiler-selected regions, deterministic destruction and typed allocation failure; [report](benchmarks/results/2026-07-22-core-04.md). |
| Core 1A | Checked-source analysis and terminating, idempotent reduction; [report](benchmarks/results/2026-07-22-core-1a.md). |
| Core 1B | Bounded quality evidence, replayed proofs, finite Boolean equivalence and structural edits; [report](benchmarks/results/2026-07-22-core-1b.md). |
| Core 1C | Permanent compiler, runtime, incremental, comparison and agent-oriented regression gates; [report](benchmarks/results/2026-07-22-core-1c.md). |

### Core 1D: complete typed compiler view

Status: complete

Core 1D is accepted by RFC-0058. One bounded checked view serves typing,
ownership, effects, memory planning, diagnostics and emission. Its fixed-point,
identity, project and scaling evidence is in the
[acceptance report](benchmarks/results/2026-07-23-core-1d-progress.md).

### Core 1E: safety-preserving native efficiency

Status: complete

RFC-0061 accepts Core 1E. Checked fast paths and allocation-free region elision
retain the portable C11 backend and workload runtime budgets;
[measurements and acceptance](benchmarks/results/2026-07-23-core-1e-progress.md).

### Core 1F: deterministic parallelism evidence

Status: complete

RFC-0069 closes bounded totality, complete blocker sets, recurrence facts and
pairwise non-overlapping planning before execution;
[acceptance and linked precision records](benchmarks/results/2026-07-23-core-1f-acceptance.md).

### Core 1G: guarded automatic execution

Status: complete

RFC-0070/0071 admit only a proven, profitable leading two-call shape, one
parent-owned join, identical serial fallback and no nested expansion;
[execution evidence](benchmarks/results/2026-07-23-core-1g-automatic-execution.md).
The exact current boundary is in `docs/PARALLELISM.md`.

## Core 1H: bounded resources and application evidence

Status: complete

RFC-0073/0074 retain a fourteen-application baseline and bounded resource facts
without source resource contracts;
[acceptance and unsupported bounds](benchmarks/results/2026-07-23-core-1h-resource-evidence.md).

## Core 1I: safe typed host boundary

Status: complete

RFC-0075/0076/0077 retain an effect-gated monotonic clock and bounded whole TCP
exchange without source handles or FFI;
[clock evidence](benchmarks/results/2026-07-23-core-1i-monotonic-clock.md) and
[TCP evidence](benchmarks/results/2026-07-23-core-1i-bounded-tcp.md).

## Core 1J: deterministic structured concurrency

Status: complete

RFC-0078/0079 retain one lexical two-call region with isolated task storage and
deterministic join. The maintained dual-request applications, serial/parallel
ratios, fault and unsupported-tier evidence remain in the
[acceptance report](benchmarks/results/2026-07-23-core-1j-structured-concurrency.md).

## Core 1K: semantic quality and reduction

Status: complete

RFC-0080/0081 retain exact Boolean/byte specifications, independent named cost
vectors and three strictly reducing atom rules. State counts, first
counterexamples, idempotence and proof replay remain in the
[acceptance report](benchmarks/results/2026-07-23-core-1k-semantic-quality.md).

## Core 1L: internal stabilization milestone

Status: complete

RFC-0082/0083 stabilized compatibility, diagnostics, runtime ABI, reproducible
source packaging and clean installation on the identified Darwin/arm64 target;
[release evidence and honest limits](benchmarks/results/2026-07-23-core-1l-slim-1-0.md).
This historical internal closure is not an active public 1.0 freeze.
