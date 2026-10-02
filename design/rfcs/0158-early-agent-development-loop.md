# RFC-0158: Early agent development loop

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, at the project maintainer's delegated roadmap direction
Created: 2026-10-02
DecisionDate: 2026-10-02
Approver: project-maintainer
Kind: process
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Amend RFC-0112's dependency order to evaluate one bounded part of the agent
development loop on the current SLIM 0.9 contract after completed M0 and M1,
before requiring every M2 source migration. The outcome is checked-source
semantic context, independent correctness and limit evidence, a paired repair
feasibility pilot, and an explicit decision to adopt, narrow or reject the slice.

The [separate context architecture RFC](0159-bounded-semantic-context.md) must be accepted before its production
implementation. This process decision introduces no context protocol, language
surface, runtime ABI, dependency, or compiler-semantic change. It neither closes
M2 nor supplies the debugger, replay, incomplete-source service and full change
evidence required for M3 and later milestones.

## Motivation

SLIM's vision is to help humans and agents build safe native software through
precise compiler feedback, explicit resource behavior and reproducible failures.
M0 repaired the current contract; M1 supplies retained production checking and
native sessions. Their closure evidence is identified in the
[SLIM Next progress ledger](../../benchmarks/results/2026-09-05-slim-next-progress.md)
and [M1 closure report](../../benchmarks/results/2026-09-08-m1-closure.md).

The strict M0 -> M1 -> M2 -> M3 order delays a direct test of compiler context
until the whole expressive-core migration. Bounded context for an already
checked 0.9 program does not require successor reference, allocation or generic
semantics. Testing it earlier can establish whether the interface is useful
enough to retain, and which missing capabilities deserve later investment.
Its benefit is currently unknown. Source savings and faster compilation do not
establish repair effectiveness.

## Guide-level explanation

The next substantial outcome is a reviewable development-loop experiment:

1. Implement a small read-only semantic context interface through the production
   self-hosted compiler, using the current checker and checked source identities.
2. Test its facts, source association, deterministic output, fixed limits and
   stale-source rejection independently of agent repair outcomes.
3. Freeze paired repair tasks and independent acceptance tests, then run a small
   same-model pilot with and without the context interface.
4. Report actual successes, failures, costs and unsupported cases; decide which
   part to adopt, narrow or reject.

The current source language remains 0.9. Existing M2 memory, host and source
contracts continue as a separate staged track under their accepted decisions.
An early context tool does not authorize their source cutover or establish their
closure. The full M3 agent/debugger milestone still follows its actual M2
dependencies.

## Reference-level specification

### Dependency amendment

Replace the blanket M2 prerequisite only for this bounded experimental branch:

```text
M0 -> M1 -> early checked-source context and repair pilot
          -> M2 -> remaining M3 -> M4 -> M5 -> M6 -> M7
```

The branch may consume only accepted and implemented 0.9/M1 contracts. A
capability that depends on successor source semantics, a debugger, native value
inspection, failure recording, provider simulation or replay retains its original
dependency. RFC-0112's milestone exit criteria, proving application and broader
held-out evaluation remain authoritative. This amendment changes implementation
priority, not those exit requirements.

### Production context boundary

The separately accepted architecture decision specifies the exact protocol,
source identity, checked-node ownership, work/output limits, conservative
exhaustion behavior, diagnostics and measurements. Context is derived from the
sole production checked artifact, never independently parsed or retyped by a
Rust semantic implementation. No separately executable IR or semantic fallback
is permitted. Any reported fact names its supported domain and is exact, bounded
with its fixed budget, or unknown with a stable reason.

The first slice concerns complete, accepted source. Stale identities must fail
explicitly. Missing evidence and bounded-away facts cannot imply safety or
permission. No context response or agent explanation approves an executable;
the normal checker retains that authority. Incomplete-source tooling and
transactional source editing require their own later specified contracts.

### Paired feasibility pilot

Freeze a versioned pilot protocol before measured trials. Name each initial
source, repair requirement, independent acceptance-test revision, supported
environment, prompt, model/reasoning configuration, tool access and task order.
Pairs use the same model and configuration, initial task and acceptance tests;
the declared treatment is access to the context interface. Both arms retain the
same ordinary compiler, runtime, libraries and other tools. Counterbalance task
order and isolate workspaces so a prior solution cannot leak into its paired run.

Plan three paired tasks: six fresh Sol 6.1 sessions at extra-high reasoning, with
900 seconds and at most 24 compiler-wrapper operations per session. The frozen
protocol defines timing and operation-ledger enforcement. Compiler-wrapper
operations are not model tool invocations. Model tool-call, token and
repair-iteration counts and their enforcement remain unknown where independent
session traces or enforcement are unavailable; record that stable reason rather
than invent counts from source tokens or participant prose. Preserve observed
overruns as unsuccessful trials. This clarification precedes participant trials
and does not relax a compiler performance gate. Do not change a task, acceptance
test or budget after inspecting candidate outcomes without retaining and
separately labelling the earlier run.

Acceptance tests are supplied independently of the repairing agent's patch and
include held-back boundary cases. Compilation and self-authored passing tests
alone cannot count as task success. Record unsuccessful, timed-out and interrupted
runs, human intervention, exact source/compiler/configuration identities, executed
checks and unverified scope. Report completion, correctness failures, time to an
accepted result, model tokens where observable, tool calls, repair iterations and
native/resource quality separately. Do not aggregate them into a goodness score.

This is a small feasibility and variance pilot. It can establish that the
measurement workflow executes and characterize observed tasks; it cannot
establish general agent effectiveness or language superiority. The larger
multiple-model, equivalent-tooling and established-language evaluation in
RFC-0112 remains future explicitly budgeted work. This decision authorizes no
outside model API spending.

## Compiler and runtime design

This RFC adds no production semantics or runtime work. Implement the context
capability in `selfhost/` under its accepted child architecture; keep Rust limited
to bootstrap-independent verification and measurement. Reuse canonical nodes,
existing checking and source identities. Default evidence collection remains
approximately linear; any exhaustive activity requires an explicit hard bound.
Programs and commands that do not use the context feature retain their accepted
behavior and performance obligations.

## Compatibility and migration

There is no source, version, ABI or existing schema migration in this process
decision. The context child specifies any new tooling schema and its version
policy. RFC-0148 and the coupled M2 ownership/lifetime/allocation decisions retain
their acceptance and cutover obligations. RFC-0147's narrow M2 policy exception
is neither extended to this branch nor treated as a waiver of other hard gates.

Historical roadmap, completed milestone and measurement evidence remains linked
to its original revisions. Current summaries may be shortened, but cannot
relabel earlier experiments as current passes or erase measured costs, failures
and contrary results.

## Diagnostics and failure cases

Reject stale or invalid context selections and distinguish unsupported context,
work exhaustion and output exhaustion according to the child contract. Unknown
is an evidence classification, not a negative quality score. A failing normal
check remains a failing normal check regardless of an agent's interpretation.
Test wrong-source identities, moved source positions, limit boundaries, malformed
requests and rejected/recovered edits where the child interface supports them.

## Performance and complexity

No runtime or compiler performance improvement is claimed here. Freeze the
context workloads and metric definitions before measurements; measure admission,
checking, context construction, output work/bytes and query latency separately.
Record geometric deterministic-work scaling and same-host costs. Keep M1 setup,
body-update and native build costs visible in the end-to-end pilot.

All existing fixtures, absolute dated measurements and portable regression
budgets remain in force. No safety rule or performance budget is relaxed. A
needed exception blocks its slice until the existing policy's separate accepted
decision and evidence exist.

## Alternatives and drawbacks

Completing all M2 work first preserves the original order but delays direct
interface evidence. Attempting all of M3 now couples context to debugger and
failure infrastructure that this experiment cannot validate. Adding tools around
the existing language is a viable product alternative, as RFC-0112 already
recognizes; the pilot helps investigate it without assuming a syntax change is
necessary. The early branch adds a tooling contract, tests and measurement
maintenance before its usefulness is known.

## Test and acceptance plan

The outcome closes only when all four deliverables have reviewable evidence:

| Deliverable | Exit evidence |
| --- | --- |
| Production semantic context | Accepted architecture, implementation through the sole SLIM compiler, stable documented schema and unsupported cases. |
| Correctness and limits | Independent positive/negative/diagnostic expectations; exact source association and staleness checks; every named capacity boundary; deterministic outputs and geometric work measurements. |
| Repair pilot | Preregistered finite protocol, frozen independent acceptance tests, isolated paired same-model runs, complete failure-inclusive results and separate observed metrics. |
| Decision | Explicit adopt/narrow/reject disposition against the frozen evidence; remaining M2/M3 obligations and general-effectiveness uncertainty stay visible. |

Run every AGENTS.md bootstrap, governance, Cargo, performance, reduction,
parallelism, comparison and agent command before committing a compiler,
runtime, benchmark or agent-tool change. Run the complete release gate before
closing this implementation outcome. A documentation check supplies no compiler
or milestone acceptance evidence.

Adopt a slice only within its tested contract after its correctness and resource
obligations pass. Narrow a feature whose unsupported cases or costs prevent the
original contract from being useful; revise its decision and tests explicitly.
Reject or remove a feature that cannot meet the hard gates. An inconclusive
pilot remains inconclusive; retain its measurements and do not promote it into
a productivity claim.

## Ratings and evidence

All ratings are neutral zero; score zero. This process amendment has no measured
implementation benefit and admits no primitive. Zero is not proof of zero cost.
M0/M1 completion is supported by their identified closure records. Future
context costs, repair outcomes and adoption are unknown until measured in the
specified domain. Source/model-token proxies remain separate evidence.

## Decision

Accepted on 2026-10-02 under the project maintainer's direct instruction:
"You can change the roadmap to whatever you want as long as it helps us achieve
the vision you defined." The maintainer also authorized coordination and feature
implementation by subagents. The coordinator reviewed this bounded contract
before recording acceptance. The delegated authority covers this dependency
amendment and experimental goal; it supplies no waiver of safety, performance,
architecture or accepted-child requirements and authorizes no outside API spend.

## Implementation

Complete on 2026-10-02 for the named bounded branch. Production schema 1,
independent limits/staleness conformance, native costs, all six frozen Sol 6.1
xhigh trials and all release components are recorded in the
[closure report](../../benchmarks/results/2026-10-02-agent-development-loop.md).
Decision: narrow; retain opt-in context, freeze breadth, repair the existing
manifest contract and improve the next evaluation protocol. Strict acceptance
is 2/3 per condition, with independent behavior and protocol/contract failures
reported separately. General effectiveness and M2/full M3 remain pending.

## Removal and supersession

This amends only RFC-0112's blanket M2-before-M3 dependency for the named early
experiment and the next-work priority left open by RFC-0123. It supersedes no
source contract, milestone exit requirement, historical evidence or permanent
regression gate. A later decision may restore the original order, narrow or
remove context, or choose a simpler development toolchain; retain the evidence
that motivated that decision.
