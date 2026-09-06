# RFC-0123: Milestone evidence and experiment boundaries

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: process
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Close M0 against its named repair and verification evidence, preserve honest
current-product claims, and define the decision boundary for M1 experiments.
No language hard gate or performance budget needs relaxation for this step.

## Motivation

RFC-0112 accepts a staged research direction, not an implemented successor.
M0 repairs current contracts and establishes measurements. Historical Rust
incremental results and snapshot invalidation estimates cannot establish current
production reuse. A completed repair campaign likewise does not prove every
possible program safe or show improved agent productivity.

## Guide-level explanation

A milestone closes when its specified repairs, durable regressions, measurement
requirements, and required release checks are satisfied on an identified source
revision. Record the observed domain, failed attempts, costs, and unsupported
cases. Later discoveries reopen the affected contract as new work; no milestone
label overrides a failing reproducer or hard gate.

M1 is the next implementation milestone and needs its own accepted architecture
specifications before production changes. This decision does not implement it.
The current goal ends with M0; work on M1-M7 needs a subsequent task or goal.

## Reference-level specification

For M0, retain a table mapping every RFC-0112 review observation and every added
repair witness to permanent production regressions. Require current bootstrap,
conformance, bounded sanitizer/fault campaigns, actual execution counters,
unchanged native analysis/resource baselines except explicitly documented rows,
and the existing full `scripts/verify-0.9.sh` gate. Keep the portable seed digest,
checkpoint identities, dated measurements, and gate outcomes in the progress
report. Do not turn unknown evidence into a passing safety or quality score.

Current-product documents must distinguish whole-project artifact hits, snapshot
invalidation estimates, retained compiler views within one compilation, and
actual retained incremental queries across edits. Historical RFCs and reports
remain evidence of their named revisions. Correct current summaries that project
old implementation results onto today's compiler. Source/edit proxies remain
independent of unmeasured LLM success or development speed.

## Compiler and runtime design

M1 may propose typed identities and ephemeral derived control-flow views attached
to canonical parsed SLIM, with one checker as authority. Its child architecture
RFC must define identity lifetime, dependency completeness, edit invalidation,
view bounds, conservative exhaustion behavior, serialization identity, and
clean-versus-updated differential tests. It must preserve production self-hosting,
no second parsed IR, approximately linear ordinary work, and current semantics.
No M1 implementation or parsed representation is introduced by this process RFC.

## Compatibility and migration

SLIM 0.9 source, ABI, release schemas, and feature policy remain unchanged.
M2 ownership/reference/allocator/generics and syntax proposals remain behind the
specific decision boundaries in RFC-0112. In particular, admitting sugar, aliases,
or changed primitive rating rules requires the separate process decision that
RFC-0112 names. This M0 closure supplies no such approval.

## Diagnostics and failure cases

Missing evidence, a known unclosed current-contract reproducer, a failed release
check, or a needed hard-gate exception prevents closure. Preserve failed logs and
repair the cause rather than delete the test. Website archive tests must compare
canonical source inventory and metadata with published output; a growing archive
must neither lose new documents nor freeze at a historical count. Generated build
artifacts are outside the publication source inventory.

## Performance and complexity

No default compiler or runtime cost is added. All permanent performance fixtures
and budgets remain in force, including the +60 decision required to relax a
budget. Measure real parser/checker/generator/cache operations separately from
external C compilation, invalidation estimates, instrumentation overhead, and
agent outcomes. A negative cost measurement remains visible even when all gates
pass, as recorded for RFC-0122.

## Alternatives and drawbacks

Declaring the entire roadmap implemented at M0 would erase unmet obligations.
Changing language policy now would pre-approve choices not needed by the next
compiler-substrate experiment. Waiting for a proof of all possible implementations
would supply no operational milestone criterion; bounded validation must instead
state its exact tested domain and retain unknowns. This process adds maintenance
of the evidence ledger and current/historical distinction.

## Test and acceptance plan

Audit current design, roadmap, status, handbook, project/cache, performance, and
quality summaries against production behavior and observed counters. Check that
all repair witnesses are in production conformance. Run governance and the full
repository, reproducible package, clean-install, ABI-mismatch, and website gates.
Record completion only after those checks pass; retain any failed attempt.

## Ratings and evidence

Analysis +2 makes decision and evidence boundaries explicit. Other ratings are
zero. No safety, speed, runtime, or agent-effectiveness improvement is inferred.
Weighted score: 15.

## Decision

Accepted under the maintainer's RFC-0112 delegation and the active M0 goal.
No change to AGENTS.md, FEATURE_POLICY hard gates, production language semantics,
dependencies, or performance limits is needed or authorized here.

## Implementation

The current documentation separates artifact reuse and invalidation estimates
from unimplemented M1 retained queries. The SLIM Next progress report maps repair
observations to permanent regressions and records gate status. This process
specification is complete; M0 completion remains conditional on its final gate
outcomes, and M1 implementation remains pending.

## Removal and supersession

A subsequent accepted process decision may refine milestone evidence, but must
preserve historical failures, measured costs, permanent regressions, and the
independent authority of the repository's hard gates.
