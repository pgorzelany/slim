# RFC-0147: Scoped M2 feature experiments

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, pursuing the SLIM Next M3 goal through its M2 prerequisite
Created: 2026-09-08
DecisionDate: 2026-09-08
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

Adopt the narrow policy change required to evaluate RFC-0112 M2. Permit
explicitly specified elaboration and evidence-backed feature tradeoffs for named
M2 experiments, while preserving semantic safety, canonical operations, measured
performance budgets and every verification command. This is an explicit change
to feature admission, not a claim that the current hard gates already allow it.

The maintainer approved this concrete policy exception on 2026-09-08.
Dependent language decisions still require their own acceptance and evidence.

## Motivation

RFC-0112 requires a process decision before successor elaboration and changed
rating rules. RFC-0123 deliberately supplied neither. Before this decision, AGENTS.md forbade
sugar, and FEATURE_POLICY rejected negative compile/runtime/minimality ratings
even where an experiment would remain within all measured budgets. Governance
applied its +40/no-negative/+2 rule to every accepted language RFC, including
ones marked primitive-free. Calling a language feature an architecture change
or inventing favorable ratings would bypass that rule.

RFC-0109 supplies a concrete caution: 13.6% compiler growth removed only 85
compiler-source bytes. Its rejection remains authoritative historical evidence.
The new process must allow observing such costs without requiring favorable
scores in advance, and must make rejection possible after the experiment.

## Guide-level explanation

A child RFC can authorize a bounded M2 experiment, specify its supported domain,
diagnostics, implementation limits and adoption evidence, and remain pending in
implementation until its tests pass. Acceptance of an experiment is not evidence
of a benefit. A feature that fails its stated adoption criteria must be narrowed
through another accepted decision or removed; it cannot be counted toward M2
closure just because its implementation compiles.

Ordinary library composition is still preferred. There are no permanent aliases,
implicit conversions, hidden allocations/copies/synchronization, or production
Rust semantics. All affected programs pass the sole production SLIM checker.

## Reference-level specification

### Exact approved policy changes

In AGENTS.md, replace only this language-constraint bullet:

> Do not add aliases, syntactic sugar, implicit conversions, hidden control
> flow, hidden allocation, hidden copying, or hidden synchronization.

with:

> Do not add aliases, implicit conversions, hidden allocation, hidden copying,
> or hidden synchronization. Outside an explicitly accepted RFC-0147 M2
> experiment, do not add syntactic sugar or hidden control flow. Within that
> scope, elaboration must have one specified canonical semantic operation,
> explicit evaluation and exit rules, source-mapped diagnostics, bounded
> checking, and measured costs; it may not conceal runtime resource work.

Keep the surrounding canonical-operation, replacement, library-first, safety,
effect, representation, self-hosting and performance constraints unchanged.

In FEATURE_POLICY's hard gates, replace only the sugar/alias item with:

> introduces a permanent alias, or introduces syntactic elaboration outside
> the explicitly accepted RFC-0147 M2 experiment boundary;

After the existing weighted-score threshold paragraph, add:

> A current-process language RFC explicitly marked `Evaluation: m2-experiment`
> and `EvaluationRFC: RFC-0147` may be accepted for a named RFC-0112 M2 scope
> without the +40, primary +2, or nonnegative compile/runtime/minimality rating
> requirements. A negative safety rating remains disqualifying. Arithmetic,
> rating ranges, explicit acceptance, unique operations, library-first design,
> semantic safety, measured performance budgets and all verification gates
> remain mandatory. Unknown benefits and costs must be named as unknown;
> metadata zero is neutral and is not evidence of no cost. Each experiment
> specifies fixed implementation bounds, predeclared adoption evidence and
> removal conditions. Unmarked RFCs retain the existing rules. This exception
> cannot authorize any performance-budget relaxation.

These are the only approved edits to the rules. The +60 performance exception
rule, including measured cause, quantified impact and compensation, is unchanged.
An unfavorable measured result stays recorded even if it is within a budget.

### Eligibility and adoption

The scope is limited to Unit; uniform moves and references; lexical lifetimes
and slices; owned bytes; opaque representation/newtypes; operation effects and
separate progress facts; structured control and operators; local inference;
generic data/functions; finite interfaces; and static callables with explicit
context. Runtime/compatibility children retain their own acceptance requirements.
This does not admit general macros, dynamic dispatch, new backends, hardware
features, general concurrency or a new executable representation.

Every marked child names its scope, baseline, finite work/output bounds where
applicable, unsupported cases, independent semantic oracle, before/after cost
measurements, and removal trigger. Freeze workload and metric definitions before
implementation measurements. For abstraction adoption, require the two
substantial applications and compiler/library migration from RFC-0112; source
savings alone cannot substitute for safety and resource evidence. No universal
quality score or inferred agent success rate determines adoption.

Accepted experimental surface is still tracked in the ordinary surface ledger
only after its implementation is complete. M2 closes only after explicit
adoption decisions and the full release evidence for every retained experiment.
The marker grants no source-compatibility promise beyond the staged successor
compatibility contract. It expires for new admissions at M2 closure; previously
accepted records retain their historical policy identity.

## Compiler and runtime design

There is no production compiler/runtime change in this decision. After approval,
the Rust governance verifier must parse the two evaluation fields, reject partial
or unknown combinations, require this accepted process RFC, restrict markers to
current-process language children after RFC-0147, and preserve ordinary checking
for unmarked RFCs. Actual scope and evidence are reviewed in each child decision;
the marker is not a machine proof of benefit or safety.

An accepted marked child skips only the three specified rating tests. A negative
safety rating, bad arithmetic, duplicate primitive, invalid disposition or an
incomplete surface owner still fails. Existing legacy disposition counts and
all historical RFC checks remain unchanged. Do not relabel earlier rejected
records or modify their scores.

## Compatibility and migration

This process alone changes no VERSION, runtime ABI, schema or accepted source.
The companion staged-compatibility proposal governs later cutovers. Current 0.9
contracts and every compiler/release verification command remain in force until
the corresponding accepted and implemented successor decision replaces them.

## Diagnostics and failure cases

Governance must explain the invalid marker, missing accepted policy, disallowed
RFC kind/process/order, negative safety rating, or ordinary threshold violation.
Missing experiment evidence blocks adoption; it is not encoded as a negative
semantic fact. A child that needs an additional policy exception must state that
conflict explicitly rather than broaden this scope by interpretation.

## Performance and complexity

The metadata check adds constant work per RFC. Language and native runtime costs
are unchanged by this proposal. Later experimental costs remain unknown until
measured separately. Approximately linear default compilation, local incremental
dependencies, deterministic bounds and all permanent budgets remain mandatory.

## Alternatives and drawbacks

Keeping current admission rules avoids a process exception but leaves the
specified M2 package blocked wherever it conflicts. Positive placeholder ratings
would hide unknowns. Removing all feature gates would be broader than necessary.
The selected proposal adds policy metadata and human review obligations; it
does not mechanize a determination that a language feature is worthwhile.

## Test and acceptance plan

Before this process implementation merges, add governance cases for unmarked
low-score, negative-cost and missing-primary-benefit rejections; eligible marked
children; marker typo/partial fields; non-language/legacy/earlier records;
unaccepted policy; negative safety; bad score arithmetic; duplicate operations;
and incomplete surface owners. Prove each existing rule still applies outside
the exception. Check that the listed performance and safety rules did not change.

Run governance and website publication for the proposed text. Implementing the
governance change is agent-tool work and requires all AGENTS.md verification
commands before commit. No passing documentation check authorizes implementation
of a language child or establishes M2/M3 completion.

## Ratings and evidence

All ratings are neutral zero; score zero. No productivity, safety, compilation
or runtime benefit is measured. The policy conflict is exact within the inspected
rule text and `check_rfcs` implementation. Experiment benefits are unknown.

## Decision

Accepted by the project maintainer on 2026-09-08 with “I approve”, following
the concrete policy preview and change summary. Approval covers this scoped
process change; it does not accept the pending language/runtime children or
establish M2 or M3 completion.

## Implementation

Implemented in AGENTS.md, FEATURE_POLICY.md and `src/bin/slim-govern.rs`.
Six focused governance tests cover eligibility, malformed metadata, retained
rating/safety/arithmetic/disposition rules, and surface activation. The eight
AGENTS.md pre-commit commands passed; the [prerequisite ledger](../../benchmarks/results/2026-09-08-m2-prerequisites.md)
identifies the source, failed sandbox attempt and complete rerun. No compiler or
runtime semantics change in this process implementation.

## Removal and supersession

This narrowly amends FEATURE_POLICY and RFC-0108's rating enforcement
for tagged M2 experiments only. It supersedes no language feature or historical
rejection. A later process decision may withdraw new experimental admissions;
retain historical evidence and record any resulting RFC-0112 scope conflict.
