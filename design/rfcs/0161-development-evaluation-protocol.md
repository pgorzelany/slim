# RFC-0161: Development evaluation protocol 2

Status: accepted
Implementation: pending
Process: 1
Audience: developer
Author: Codex, under delegated overnight SLIM Next implementation direction
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

Replace the next experiment's transport with a bounded stdlib evaluator under
`benchmarks/development/`. Keep protocol 1, its frozen tasks and results intact.
This is measurement infrastructure under RFC-0112 and RFC-0158, with the sole
production compiler supplying SLIM acceptance. It adds no language, compiler,
runtime, dependency or performance-budget contract.

## Motivation

Protocol 1 rejected concurrent wrapper requests and hashed live source before
reopening it for compilation. A next experiment needs honest source association,
separate operation timing, preserved failures and independent task acceptance.
These repairs establish measurement validity; improved agent effectiveness is
unknown.

## Guide-level explanation

Freeze the complete task corpus, independent acceptance fixtures, evaluator,
public documentation and compiler/runtime identities before dispatch. Prepare
isolated paired candidates. Each request captures immutable source bytes before
waiting for explicit serialized execution. All checks, generation and native
runs consume that snapshot. Final submission first publishes a separate
receipt/cutoff intent, then captures an immutable revision. Independent evaluation
atomically seals completed request/note state before oracle work. Model configuration, trial order and
budgets come from the frozen manifest, not from observed outcomes.

## Reference-level specification

The [protocol](../../benchmarks/development/PROTOCOL.md) defines schema 2,
allowed operations, artifact identity, bounds and failure precedence. Acceptance
data names fixed operation enums, frozen source overlays, expected bytes and
diagnostics; it never supplies commands or shell code. Original complete-project
acceptance and independent positive controls accompany held-back client probes.
Oracle outputs are independently authored, not copied from reference output.

Each attempt receives a unique request record. A locked append-only validated
hash chain retains request, execution and terminal records, including denials,
compiler failures, timeouts and infrastructure failures. Requests serialize
without making overlap a protocol violation. Queue time, wrapper duration and
each process launch-to-reap monotonic duration are separate. Observed dispatch
and submission receipt/publication/capture stamps remain separate from native timings.
Evaluation-intent seals the authoritative state; later attempts fail observably
without ledger mutation. Unfinished lifecycle rows remain unresolved with unknown
acceptance, even if an orphan result file exists. A killed evaluator
can leave a visible unfinished request; recovery explicitly records interruption.

Exact captured files, executable/runtime/configuration and command/input/output
identities accompany records. Capture is sequential, not an atomic filesystem
transaction. The immutable captured bytes, rather than the later live files,
are the admitted input. All subprocesses have checked wall, CPU, file and output
bounds; file limits apply per file and CPU per process, not aggregate work.
Resource signals are observed; handled nonzero causes remain unknown. Trusted
tool abnormal signals and native C backend failures are infrastructure.
Internal generated C admits 16 MiB, native/feedback streams 2 MiB.
Timeout/output exhaustion kills the owned process group and reaps its direct
leader. Grandchild reaping, escaped process groups, peak RSS, unobserved
direct-wrapper bypass and actual model tokens/calls remain unknown.
Cross-process elapsed time assumes the same boot when sandboxed kernel boot
identity is unavailable; single-process monotonic operation time is observed.

## Compiler and runtime design

No production change. The Python evaluator transports bytes and invokes the
ordinary production SLIM compiler and identified native C toolchain. It neither
parses SLIM nor implements typing, ownership, effects or runtime semantics.
Project manifests are fixed frozen files; source overlays cannot change their
input paths. No implicit bootstrap, dependency installation, network access or
outside model spending occurs.
Declared repository base sources are captured data, with sparse task overlays;
later preparation/evaluation never falls back to live repository bytes.

## Compatibility and migration

Schema 2 and its denominator are separate from protocol 1. Historical evidence
is never reclassified or replaced. Tasks and budgets freeze only after corpus
positive controls pass, and before any measured participant is dispatched.
Changing a frozen experiment requires a new freeze and separately named run.

## Diagnostics and failure cases

Distinguish correctness, constraint, elapsed timeout, interrupted and
infrastructure outcomes. Saturated/corrupt ledgers halt without deleting rows;
missing terminal records remain unknown until explicit recovery. Infrastructure
failure cannot establish model incorrectness. A failing negative probe cannot
count as an intended rejection without its accepted positive control.

## Performance and complexity

Source capture, hashing and each replay are linear within fixed admitted
byte/record bounds. Incremental scandir bounds all visited corpus entries at
4,096 and combined retained bytes at 64 MiB before sorting/retention. Each append
replays the fixed bounded ledger. The opt-in evaluator does no default compiler work. It records actual
wrapper operations and process time, not inferred model tokens or activity.
Same-host native performance remains a separate existing gate. No budget is
relaxed and no effectiveness benefit is claimed before measured trials.

## Alternatives and drawbacks

Keeping protocol 1 would retain source races and punitive concurrent-request
classification. A general scheduler or semantic test language would add needless
scope. POSIX locks and process groups make this transport host-specific. Local
read-only snapshots and advisory isolation are reproducibility controls, not
security boundaries against a hostile participant on the same filesystem.

## Test and acceptance plan

Permanent tests cross concurrent requests, immutable snapshot mutation, stale
expected inputs, changed compiler/corpus identities, deadline/quota rejection,
launch errors, timeout and output limits, ledger corruption/interruption, and
independent positive/negative acceptance. Submission/evaluation barriers cross
concurrent attempts, unfinished seals and orphan results; complete diagnostic
streams reject extra/malformed lines. Corpus boundaries count empty directories
and check combined bytes before retention. A production-compiler sanity fixture
executes separately from fake-process transport tests. The coordinator owns
required bootstrap/governance/Cargo/benchmark gates before any commit. Passing
infrastructure tests does not close M2, full M3 or general effectiveness.

## Ratings and evidence

All ratings and score are zero: a process decision admitting no primitive.
Neutral metadata is not evidence of zero implementation cost or benefit.

## Decision

Accepted on 2026-10-02 after coordinator review of this exact contract under the
maintainer's explicit delegated overnight direction and RFC-0112 implementation
authorization. The accepted scope is bounded evaluation infrastructure and a
later frozen experiment, with no production semantics or outside expenditure.

## Implementation

Pending independent fixture verification and coordinated required gates.

## Removal and supersession

Supersedes protocol 1 only for future runs. Remove or narrow this evaluator if
its captured-source authority, independent acceptance or complete failure
accounting cannot be preserved. Keep protocol 1 and all original observations.
