# SLIM Roadmap

Status: SLIM 0.9 — experimental, pre-1.0
Current milestone: Pre-1.0 evidence-driven development
Current goal: Measured compiler and library development in SLIM
Last updated: 2026-10-03

SLIM helps humans and agents build safe native software through precise compiler
feedback, explicit resource behavior and reproducible failures. The
[first measured development loop](benchmarks/results/2026-10-02-agent-development-loop.md)
is complete for its bounded scope: production semantic context, independent
conformance/cost evidence and six frozen repair trials. Strict acceptance is 2/3
per condition; general effectiveness remains unknown. Retain opt-in schema 1 and
freeze its breadth. The next outcome is reliable project contracts, useful
ordinary-source components and a stronger compiler/library development experiment.

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

## Current substantial outcome

The maintainer authorized continuous development until 2026-10-03 10:00
Europe/Warsaw (08:00 UTC), starting 2026-10-02 18:06 UTC. Check the clock and
choose further useful work until that boundary; this does not promise that all
successor milestones fit in one night. Parallel implementation feeds reviewed
integration; measured trials and native benchmarks run without competing work.

| Workstream | Concrete result | Acceptance |
| --- | --- | --- |
| Project contracts | Canonical import/export lists and distinct module declaration identities, with checked visibility and original source diagnostics. | Collision, lexical/type-role, nested-expression and visibility fixtures; clean/retained parity; unchanged safety gates and measured checking work. |
| Ordinary SLIM components | Bounded framing and byte indexing, used by catalog queries/snapshot reconciliation, dependency planning and development-result and operation-cost summaries. | Independent behavior/failure matrices, explicit admission bounds, native/resource tests, deterministic results and preserved work budgets. |
| Compiler dogfooding | Capture checked compiler/project bytes with ordinary project-input; reconcile catalog snapshots and plan direct imports. | Matching trusted producer/adapter identities, complete production checking, reproducible outputs and named correctness/resource contracts. |
| Development evaluator | Immutable operation/submission snapshots, queued concurrent requests and separately observed task/tool timing. | Permanent transport, identity, timeout and acceptance tests; no second semantic checker. |
| Held-out evaluation | Larger paired compiler/library tasks with ordinary tools versus optional context, using fresh Sol 6.1 extra-high agents. | Freeze tasks, independent tests, order and budgets before dispatch; retain every outcome and report unknown observations explicitly. |
| Closure | Reviewed, verified commits pushed to `codex/slim-next`; concise current status and results. | Required compiler/runtime/benchmark checks and source-bound repository, release and website acceptance. |

The morning outcome is working source and evidence: reconcile actual compiler
inputs, plan their dependencies, summarize every configured development trial,
and compare ordinary feedback with optional semantic context. Name measured
benefits and costs per domain; retain unknown results and infrastructure failures.
Reliable native session execution is part of acceptance, not an optional cleanup.

Checkpoint `e506464`, pushed to `codex/slim-next`, passes repository,
reproducible release/clean install and website gates. It adds checked project
capture, operation-cost reporting and private duplicate-path validation.
The full invocation's website classification failure and successful isolated
repair remain distinct. Actual million-node and aggregate-source producer
crossings still require native evidence. The
[protocol-2 cohort](benchmarks/development/current.json) has 24 configured fresh
serial Sol 6.1 extra-high trials; eight participants passed their independent
oracles. Dispatch nine was interrupted before a participant started because
fresh-agent capacity was exhausted; fifteen trials remain undispatched. General effectiveness,
model calls and tokens remain unknown; successor milestones retain their gates.

The accepted [duplicate-path optimization](design/rfcs/0167-linear-manifest-validation.md)
reuses the existing byte trie only for repeated raw path identity. The broader
membership experiment failed its unchanged scaling gate and was narrowed.
Original name searches, diagnostics and cycle order remain. Exact work, cleanup,
parity, resource and same-host comparisons pass for the named focused domains;
The verified checkpoint includes these changes. Whole-manifest checking remains
outside the local linear-work claim.

Only reviewed bounded ordinary-source, evaluator and behavior-preserving compiler
contracts are accepted within this window. New language syntax, runtime integration, dependencies and
performance-budget relaxation remain outside its scope. The first pilot stays
frozen. A result may support retaining, narrowing or rejecting context use; a
larger task corpus still does not establish universal agent effectiveness.

Operator timing and transient logs stay in ignored `build/`. Current docs explain
contracts, decisions and next work; durable tests and relevant measurements
remain product infrastructure. Do not create a historical prose journal for
each implementation or verification attempt.

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

The work window does not promise milestone closure or relax performance budgets.
The current language and `docs/COMPATIBILITY.md` remain authoritative until an
accepted and implemented successor decision replaces their named contract.

## Foundation records

The production compiler is self-hosted SLIM with the portable C seed under
[RFC-0027](design/rfcs/0027-portable-c-bootstrap-seed.md). Current language and
implementation boundaries are in [CORE.md](docs/CORE.md) and
[STATUS.md](docs/STATUS.md). Earlier milestone evidence remains in the
[results index](benchmarks/results/README.md); this roadmap tracks current work.

The following closure markers retain the governance-checked historical
boundaries. Detailed evidence stays in the linked decisions and results index.

### Core 1D: complete typed compiler view

Status: complete

Core 1D is accepted by RFC-0058.

### Core 1E: safety-preserving native efficiency

Status: complete

RFC-0061 accepts Core 1E.

## Core 1H: bounded resources and application evidence

Status: complete

Historical acceptance: RFC-0073/0074.

## Core 1I: safe typed host boundary

Status: complete

Historical acceptance: RFC-0075/0076/0077.

## Core 1J: deterministic structured concurrency

Status: complete

Historical acceptance: RFC-0078/0079.

## Core 1K: semantic quality and reduction

Status: complete

Historical acceptance: RFC-0080/0081.

## Core 1L: internal stabilization milestone

Status: complete

Historical acceptance: RFC-0082/0083; this is not an active public 1.0 freeze.
