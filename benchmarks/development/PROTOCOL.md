# SLIM development evaluation, protocol 2

This bounded transport measures paired development tasks through the ordinary
production compiler. It supplies no SLIM semantics and no general productivity
claim. Protocol 1 remains immutable. RFC-0161 accepts this process boundary.

## Corpus and dispatch

`manifest.json` schema 2 fixes model, reasoning effort, ordered trials, task
definitions and budgets. Each task names relative candidate files, editable
files, a fixed entry project, task text, initial/reference directories and an
independently authored `acceptance.json`. The project manifest is never editable.
An optional task `base` maps candidate-relative filenames to trusted repository
relative `.slim`/`.project` files. Freeze captures those bytes under `base-data/`;
initial/reference directories contain only overrides. All complete files still
appear in the whitelist. Preparation and evaluation compose frozen bytes only,
with no symlinks or live repository fallback. This avoids durable compiler copies.
The coordinator authors and validates tasks, then freezes the corpus before the
first participant prompt. No later candidate teaches the paired participant.
Task/model/budget changes require a new freeze and a separately named denominator.
Root dispatches trials serially and prevents unrelated native performance runs
from overlapping them. No purchases, dependencies or network lookup are needed.

`freeze` retains corpus, evaluator, public docs, compiler binary and runtime
bytes in an isolated directory. It records SHA-256 identities and the native C
compiler path/hash, platform and build environment. SHA-256 identifies artifacts;
the production context command still compares complete expected source bytes.
`prepare` uses frozen material only. Participants see candidate, task and shared
public docs; the oracle and reference remain separate. Same-filesystem isolation
and read-only permissions are advisory, not a secure held-out boundary. Observed
leakage/bypass and human repair intervention require explicit coordinator records.
Unobserved leakage or bypass remains unknown.

## Operation and time contract

Allowed participant operations are `check`, `interfaces`, `context`, `build`,
and `run`. `context` requires an original qualified declaration selector.
`--source REQUEST_ID` selects a retained operation revision; `--expected
REQUEST_ID` supplies a separately retained expected revision. Omitting these
uses a newly captured candidate and a byte-identical independent expected copy.
The output explicitly identifies the source revision used. Last-good facts are
never acceptance of current broken bytes. Temporary participant test edits use
the same fixed candidate files and ordinary wrapper operations.

Every request records observed UTC and same-host monotonic receipt time before
capture. Each admitted file is read once with a checked byte cap and copied into
a new private read-only snapshot before queueing. Sequential capture does not
promise an atomic filesystem revision. Its exact captured bytes, manifest and
file identities authorize this operation. Live candidate mutation after capture
cannot alter checking, C generation, native compilation or execution. Fixed files
must equal their frozen initial bytes; paths must be relative ordinary files.

A POSIX `flock` serializes operations. Concurrent requests wait; overlap is not
a trial violation. Lock acquisition is not promised FIFO. Queue time is separate
from operation execution and counts toward the elapsed trial budget. Requests
received before the published submission cutoff may finish; later requests are
denied until evaluation seals the ledger. After that seal all new requests are
rejected with an observable command error and no ledger mutation. Every admitted
operation consumes one quota unit even if compilation fails or times out. Quota
denial is retained separately and is not by itself an incorrect candidate.

`start` records coordinator-observed dispatch time. `finish` observes its entry
receipt, then immediately publishes `submission-intent` under the ledger lock,
without acquiring the operation lock or validating/capturing source first. The
published UTC and monotonic cutoff are authoritative for request admission and
elapsed dispatch-to-cutoff time. Entry receipt UTC/monotonic stamps are separately
observable; they do not claim capture or publication at entry. Final immutable
source capture and its completion stamps follow independently. A failed capture
still retains the intent and explicit capture error. Elapsed agent time includes
queue wait before the cutoff, while capture completion is outside that duration. Each subprocess
records its own monotonic launch-to-reap duration, argv, return code, stdout/stderr
bytes/hashes and timeout/resource outcome. Queue, wrapper, native and whole-agent
times remain distinct. The bounded stdlib resource launcher records its own
launch-to-reap time and an independent pre-exec monotonic stamp; executable time
starts at that stamp. A close-on-exec control pipe distinguishes launcher errors
from application exit codes, including 125, without trusting application output.
Queue, wrapper, executable and whole-agent
times are never added together to imply wall time or model active time. Host
monotonic stamps are conditional on the same boot. Kernel boot identity is
recorded when available; sandbox denial is explicitly unknown. A backward epoch
or changed observed boot identity rejects, but the backward check cannot prove
the absence of a reboot when kernel evidence is unavailable.

## Fixed bounds and ledger

Manifest budgets are checked against these protocol ceilings before freeze:
32 tasks, 64 trials, 4,096 visited corpus entries (including empty directories),
64 MiB combined corpus bytes, 128 files per task, 16 MiB source per operation, 4 MiB per
source file, 128 acceptance components, 1 MiB JSON documents, 128 operations per
trial, 7,200 seconds per trial, and 60 seconds per subprocess. Each subprocess
has at most 2 MiB combined stdout/stderr, except internal C emission has a fixed
16 MiB cap because the current selfhost compiler emits about 5.1 MiB. Generated
C remains an internal hashed artifact, with no report serialization. Each
process has a 128 MiB per-file output ceiling and a per-process CPU bound
derived from its wall timeout. Aggregate filesystem writes, group CPU and peak
RSS remain unknown. Resource exhaustion is positively classified only from
observed SIGXCPU/SIGXFSZ. A handled resource error followed by ordinary nonzero
exit retains an unknown cause; stderr text is never a resource classifier. These are evaluator bounds, not language limits.
Captured response data is bounded; stdout is drained incrementally, never
collected unbounded at process exit. Timeout/output exhaustion kills the whole
owned process group and reaps its direct leader. Grandchild reaping, escaped
process groups, hostile detached processes and peak RSS remain unknown. This
transport is scoped to trusted compilers and native SLIM programs. No shell is used.

Native compilation selects the existing structured runtime with the fixed
`-DSLIM_PARALLEL=1` argument only when trusted emitted C begins with the exact
canonical line `#define SLIM_PARALLEL 1`. It enables no platform worker macro.
Text elsewhere in generated C cannot select runtime configuration.

Each trial has one append-only `ledger.jsonl`, guarded by a separate lock and
validated on every replay/append. Schema 2 accepts only known record kinds and
state transitions, rejects duplicate JSON keys/nonfinite numbers and retains a
SHA-256 predecessor chain. Each line is capped at 64 KiB and each ledger at
16 MiB/4,096 records. Saturation or corruption halts the trial as infrastructure
failure; records are never truncated or silently repaired. Requests retain all
terminal outcomes within this fixed bound. Before oracle work, an atomic
`evaluation-intent` seals the exact completed request/note set and its predecessor
hash. Only the terminal evaluation record may follow. Requests or notes attempted
during or after evaluation produce explicit command errors with no mutation.
Late coordinator evidence cannot silently change a sealed result; any later
necessary disposition must separately name the original result identity.
An evaluator killed between records leaves a visible unfinished lifecycle.
`recover` can record interrupted requests and failed submission capture after
acquiring the operation lock. The coordinator must first stop or independently
observe completion/interruption of every unfinished wrapper and submission
invocation; that lock alone cannot prove a queued CLI or capture is dead. Recovery
reason text records the coordinator observation and is not proof. An unfinished evaluation seal remains unresolved
for explicit coordinator disposition; an orphan result is never trusted.
Replay tracks active/admitted requests once per record and is linear within the
fixed ledger bound; each append replays that bounded ledger. A hash chain detects accidental corruption but is
not authority against deliberate rewriting on the same filesystem.

## Independent acceptance and outcomes

Acceptance JSON contains only fixed operation enums, frozen source overlay paths,
expected exit/stdout/stderr bytes or a complete canonical diagnostic stream, and a named finite
domain. It cannot supply argv, executables or shell fragments. Every candidate
first receives complete original-project checking. Original application/interface
components and injected clients execute the same immutable submitted revision.
Negative probes require a separate accepted positive control. Diagnostic arrays
serialize exactly as `Ecode@module@start:end` plus newline for each ordered row;
extra, malformed or missing stdout lines reject the component. References establish
feasibility; independent specification/oracle data defines acceptance.
Before dispatch, `verify` requires initial task acceptance to fail and reference
acceptance to pass, with no infrastructure/resource/timeout failures in either.
The initial normal checker expectation remains independently declared. This
non-vacuity gate rejects an already-solved or infeasible corpus.

Compile success and participant tests cannot earn success. Each component reports
its own commands and bounded domain. Frozen fixture changes, launch errors,
timeouts or observed resource signals are infrastructure outcomes, distinct from a
candidate's observed behavioral/contract failure. Trusted compiler/check/context
abnormal signals and any nonzero native C backend exit are infrastructure with
an unknown cause. Native candidate exit/trap is observed behavior within the
frozen oracle domain; no universal model-fault claim follows. Strict trial outcomes are
`accepted`, `correctness-failure`, `constraint-failure`, `elapsed-timeout`,
`interrupted`, or `infrastructure-failure`; oracle acceptance is retained
separately. A component with infrastructure, timeout, output-limit or resource-limit
evidence is null/unknown, including an incomplete positive control. Its raw command
receipts and expected identities remain available; incomplete output cannot supply
a behavioral counterexample. Whole-oracle acceptance is null if any component is
unknown; complete independent component results remain visible. Budget denials and concurrent requests are observations, not hidden
penalties. Coordinator-observed bypass/leakage can invalidate a trial through
an explicit note before the evaluation seal. Summaries preserve every frozen row.
Undispatched rows are `not-run`; dispatched rows without terminal evaluation are
`unresolved` with their exact lifecycle observation, counts and retained stamps.
Their oracle acceptance is null/unknown, not false. Only a hash-validated terminal
ledger evaluation can establish accepted outcome; orphan results are ignored.

Raw snapshots, process output and ledger receipts live under ignored `build/`.
Durable source contains this protocol, fixtures/tests and one concise current
result. Preserve every measured trial/failure; do not write a historical report
per iteration. Actual model tokens, model calls and repair iterations remain
`unknown-not-observed` unless independent session evidence measures them. Report
paired acceptance, time, operation count, independent oracle components, human
intervention and native quality separately, without one quality score.

For the larger paired cohort, predeclare descriptive aggregation before dispatch.
Report configured, dispatched and terminal counts and every outcome per condition;
partial runs do not yield a completed-cohort success rate. Keep strict acceptance
and finite oracle acceptance separate. Median dispatch-to-cutoff time includes
all terminal submitted trials, including failures and timeouts, and is not time
to success. Paired context/baseline time ratios use only pairs with two strict
acceptances; name that selected subset and its size alongside all outcomes.
Report admitted wrapper operations separately from model calls. Any changed-file
or positional byte-difference count is a source proxy, not edit distance, model
token use or evidence of native performance. No aggregate universal quality
score or general effectiveness claim follows from these descriptive statistics.
