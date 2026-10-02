# Semantic-context feasibility pilot, protocol 1

Status: preregistration, recorded before participant trials. Trial results belong
in separate dated artifacts and do not mutate this protocol. This is a tooling ablation
on three fixed SLIM tasks, not a language comparison or a general success-rate
experiment. RFC-0112 motivates the pilot; the normal production compiler remains
the sole semantic authority. The evaluator contains no SLIM checker or semantics.

## Preregistration and task isolation

The manifest names exactly three paired tasks and six trials. For each task, a
fresh Sol 6.1 agent at `xhigh` receives the identical initial source and task text
under baseline or context treatment. Each participant works once. Trial order is
fixed in the manifest before freezing; it alternates conditions to reduce a
simple order confound. Trials run serially; no participant sees another trial's
patch, outcome, reference candidate or conversation. The coordinator must not
teach later participants lessons from earlier trials. No retries replace failed
trials. Any follow-up pilot gets a new protocol and denominator.

`prepare` refuses an unfrozen or changed corpus. `freeze` records exact SHA-256
identities of this protocol, manifest, participant material, reference sanity
candidates, oracle material and evaluator. Task definitions and oracles freeze
before the first participant prompt. Freeze is not an acceptance result.

Participant bundles contain only the task, initial project and shared public
language/project documentation. Oracle and reference directories are separate;
prompts prohibit inspecting them, the evaluator implementation, other trials,
compiler implementation, git history or the parent conversation. This is an
advisory same-filesystem restriction, not a security boundary. Leakage discovered
by observation invalidates the trial and remains in the denominator. The pilot
does not claim strong blinding or an inaccessible held-out test set.

## Access and fixed budgets

Both conditions may read/edit their candidate, read the supplied public docs,
use ordinary production check/build/run feedback and run their own candidate
tests. Treatment additionally permits `context` on the candidate and an exact
captured source/project snapshot under RFC-0159. Baseline has no context access.
All participant compiler feedback goes through the evaluator's `tool` command;
its operation ledger counts actual wrapper operations, independently of model
tool invocations. The wrapper never exposes the oracle or reference result.

Model and reasoning effort are Sol 6.1 and `xhigh` for all six trials. Each trial
has 900 seconds elapsed from dispatch and at most 24 compiler-tool operations.
No single compiler or native execution may exceed 30 seconds. Root observes the
elapsed deadline and interrupts a late participant. A wrapper operation after
the deadline or the 24th operation fails. Participants may submit one final
candidate; evaluation begins only after submission or interruption. Participant
self-reported time, tokens and model tool calls are not independent evidence.
The wrapper's enforcement is local; direct tool bypass discovered in available
session evidence is a constraint failure. Unobserved bypass remains unknown.

The initial compiler/runtime snapshot is identical within a pair and its hashes
are recorded. No API purchase, external model service, dependency installation or
network lookup is part of this pilot. The current user-authorized collaboration
sessions supply the participants. No full compiler/performance run overlaps a
trial or a native measurement.

## Independent acceptance

Oracle authors fix the task specification and acceptance before trials. The
participant cannot earn success from compilation or self-authored passing tests.
The evaluator first checks candidate paths and editable-file limits, then checks
and builds the complete original candidate project, including every caller.
It executes the application, compares exported interface artifacts with the
declared public contract, and compiles injected clients against the same source.
Those clients execute finite independent input matrices and compare bytes with
Python-computed expected results from the declared specification. Separate
clients must remain rejected at ownership and effect boundaries. Existing
reference candidates only establish that the fixed task/oracle is feasible;
their outputs do not define the expected results.

Task domains and finite oracle cases are recorded separately. Passing them is a
bounded task-acceptance observation, not equivalence over all possible inputs or
a universal memory-safety proof. The oracle is intentionally small enough to
inspect. Changes to oracle/reference material after freeze invalidate the freeze.
An evaluator crash, unvalidated oracle or infrastructure failure is recorded as
infrastructure failure, never silently counted as model correctness.

## Measurements and report

For all six trials preserve dispatch/deadline/submission timestamps, observed
wall time, final source hashes, baseline diagnostics, exact command argv,
return codes, timeouts, feedback bytes, wrapper operations, acceptance components
and outcome. Record check/build/native durations separately from agent elapsed
time. Source bytes and changed prefix/suffix spans are edit evidence, never model
tokens or LLM success proxies. Actual model tokens and model tool invocation
counts are `unknown` unless independent session capture exposes them; final
participant prose does not fill these fields. Repair iteration count is also
unknown unless a captured patch/check sequence supports it.

Outcomes include accepted, correctness failure, constraint failure, elapsed
timeout, and infrastructure failure. Preserve every trial and failure. Report
paired accepted counts, wall time and the independent oracle components without
aggregating them into a quality score. Human intervention and leakage observations
are separate fields. Native/resource performance is not measured by these small
oracle executions and remains a separate compiler benchmark gate.

The pilot estimates feasibility and variance for a later held-out experiment.
With only three pairs, one model configuration and advisory isolation, it cannot
establish a general success rate, causality across language versions or an
adoption threshold. No numerical language-adoption threshold is chosen here.
All three initial projects are checker-accepted: two API-development migrations
and one behavioral ownership repair. This pilot does not evaluate semantic
context on an invalid editor snapshot; separate tool conformance tests cover
explicit invalid-source rejection.
