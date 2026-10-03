# RFC-0166: Bounded ordinary development operation cost

Status: accepted
Implementation: pending
Process: 1
Audience: both
Author: Codex, under delegated overnight ordinary-library direction
Created: 2026-10-03
DecisionDate: 2026-10-03
Approver: project-maintainer
Kind: compatibility
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Add an experimental ordinary SLIM `development-operation-cost INPUT.ns`
application and reusable source component. A fixed trusted Python adapter
converts checked protocol-2 request and command observations into one framed
input. The application reports independent aggregate dimensions with missing
evidence and overflow explicit. It adds no detail command, quantiles, language
operation, compiler path, runtime ABI, dependency or acceptance fallback.

This reporting work is outside the frozen experiment. Preserve RFC-0164, the
evaluator, corpus, feedback, trial budgets and participant operations unchanged.

## Motivation

RFC-0164 reports outcomes, operation counts and trial elapsed observations but
does not expose the operation timing breakdown already retained by protocol 2.
Read-only inspection of the first frozen-5 `text-budgeted-quote` pair found
baseline/context trial elapsed operands of 263.449/202.647 seconds, wrapper
duration sums of 13.436/20.083 seconds and queue observation sums of
0.330/7.080 seconds. One context interfaces request recorded 7.076 seconds of
queue wait. Native-compile command elapsed sums were 7.734/8.730 seconds.
These inspected records motivate a feedback-cost report; they are not a new
verifier acceptance, causal estimate or effectiveness claim.

The consumer should distinguish capture, queue, wrapper and command-phase
observations so a developer can locate a costly feedback request. Generated-C
emissions and feedback envelopes have different extents and purposes. Preserve
those facts independently instead of deriving model cost from either one.

## Guide-level explanation

Run the fixed adapter against a stopped-writer frozen cohort and its runs.
Retain its separate receipt, then run the ordinary application against the
framed output. The application produces one completely assembled aggregate
report: every planned trial, both conditions, each operation and each command
phase. Maxima retain the trial/request/command provenance needed to find the
original receipt. No source or executable is selected by the input.

Counts and sums describe recorded observations. Simultaneous wrapper calls can
overlap during capture and queueing. Wrapper, capture, queue, command and
executable durations must never be added across dimensions to explain wall
time, subtracted from trial elapsed, or treated as fractions of model activity.
Independent native application performance and model tokens/calls/active time
remain unknown. This application does not establish candidate acceptance.

## Reference-level specification

Admit at most 32 tasks and 64 planned trials, exactly one baseline/context pair
per task. Task IDs are nonempty byte strings of at most 64 bytes and trial IDs
are nonempty byte strings of at most 128 bytes. Admit at most 4,096 request rows
per trial and 8,192 across the input; at most 128 started requests per trial and
8,192 across the input; and at most three commands per started request and
24,576 across the input. Complete input admission is 8 MiB, trial and request
metadata payloads are each at most 1,024 bytes, command-list payloads at most
2,048 bytes, and scalar payloads at most 128 bytes. Report admission is 512 KiB.

These independent reporting limits do not admit every possible denial-heavy
protocol ledger. Sixty-four trials each using all 128 admitted operations fit
the row ceilings. A complete conversion exceeding any row, field, byte or
report ceiling refuses rather than truncates. Refusal changes no trial outcome
and supplies no negative quality evidence. Ordinary file read precedes source
admission; no preread allocation, aggregate filesystem, RSS or CPU bound is
claimed. Allocation failure retains exit 71.

### Canonical framed input

Reuse `std_netstring.parse`, `framed_records.parse`/`integer` and ordinary byte
comparison/index components. The first triple is `(format, freeze SHA,
identities)`. Format is exactly `slim-development-operation-cost-1`; freeze SHA
is exactly 64 lowercase hexadecimal bytes; identities contains exactly three
frames of compiler, evaluator and manifest SHA in that order.

Each following trial triple is `(task ID, condition, trial metadata)`, followed
immediately by exactly its known request-row count of request triples. Condition
is baseline or context. Trials are in strict task-byte order with baseline
immediately before context for the same task. Reject missing pairs, duplicate
tasks and duplicate trial IDs. Use the existing at-most-64-entry byte index for
trial-ID duplication, with no compiler sorting feature.

Trial metadata has exactly seven frames:

1. Trial ID.
2. Supplied outcome, using RFC-0164's eight outcome spellings.
3. Lifecycle, using RFC-0164's six nonterminal spellings, or empty for terminal.
4. Ledger SHA, empty only for an unprepared trial.
5. Terminal result SHA, empty without a checked terminal evaluation.
6. Prepared metadata SHA, empty only for an unprepared trial.
7. Request-row count: empty for unprepared/unknown, otherwise canonical 0..4,096.

An unknown count carries no request rows; it is not a known zero. An unprepared
trial has `not-run`, `unprepared` and empty identity/count fields. Every other
trial has ledger/prepared identities and a known count. Terminal outcome and
lifecycle shape follow RFC-0164 and require a result identity; unresolved and
not-run rows have no result identity. Orphan result files supply no evidence.
Outcome labels and identity spelling are supplied control data in SLIM, not
acceptance authority.

A request triple is `(request UUID, request metadata, command list)`. UUIDs
have canonical lowercase 36-byte spelling and are in strict byte order within
each trial. Adjacent comparison rejects duplicates. UUID reuse across trials
does not conflate the separate `(trial ID, request UUID)` identities. Request
metadata contains exactly eleven frames:

1. Original request ledger sequence, canonical 0..4,095.
2. Operation: check, interfaces, context, build or run.
3. Started: true or false.
4. Finished status: ok, compiler-error, timeout, output-limit, resource-limit,
   infrastructure-error, constraint-error, denied, interrupted or unfinished.
5. Capture nanoseconds.
6. Queue nanoseconds.
7. Wrapper elapsed nanoseconds.
8. Feedback envelope bytes.
9. Source-identity SHA.
10. Expected-identity SHA.
11. Receipt SHA.

Every numeric observation is empty for unknown or canonical nonnegative I64.
Zero is a present observation. A present Boolean, negative, overflowing,
noninteger or noncanonical numeric field is invalid, never silently unknown.
Control fields must be present and valid. Source identity is present exactly
for started requests; expected identity is present exactly for started context
requests. Unstarted requests have no capture/queue observations or receipt.
Unfinished requests have no wrapper/feedback observation or linked receipt.
A terminal started request can lack its receipt or feedback observation;
preserve each supported observation independently. Missing receipt means
unknown command evidence, not a successful empty command list.

A present linked receipt requires one to three command records. Each record is
exactly nine consecutive frames, with its ordinal determined by receipt order:

1. Phase: check, interfaces, context, emit-c, native-compile or execute.
2. Recorded command status.
3. Launcher-to-direct-reap elapsed nanoseconds.
4. Executable elapsed nanoseconds.
5. Stdout bytes.
6. Stderr bytes.
7. Recorded output role: generated-c, or empty when unavailable.
8. Executable SHA, empty when unavailable.
9. Timing basis: monotonic-launcher-to-direct-reap, or unknown when unavailable.

Command status is ok, compiler-error, timeout, output-limit, resource-limit or
infrastructure-error. A present generated-c role is valid only for ok emit-c.
Missing role stays unknown; phase alone does not turn diagnostic or incomplete
output into generated C. A known
command duration requires its recorded timing basis. Executable duration needs
positive fixed-launcher evidence checked by the adapter; an executable SHA
alone is not launch evidence. Present hashes retain the exact hexadecimal
contract. No receipt requires an empty command-list payload. Extra fields,
partial commands, invalid phase sequences and trailing bytes reject. The
transport validator requires one matching check/interfaces/context phase; build
has a prefix of emit-c/native-compile; run has a prefix of
emit-c/native-compile/execute. Every nonfinal command must have status ok.
A shortened prefix must end with a non-ok command.

Header identities, paths omitted from this transport, sequences, operations,
statuses, phase/role/basis tags and framing/count controls are data. Timings and
sizes are observed operands with independent availability. Source authority,
launch association and outcome authority require the fixed checked adapter;
valid transport spelling cannot establish them.

### Aggregate dimensions

Emit separate request and command groups for every planned trial and each
condition, request groups for each of five operations per condition, and command
groups for each of six phases per condition, including empty groups. No selected
accepted subset replaces the complete planned denominator. Request metrics use
observed request rows; ordinary command metrics use observed linked-receipt
command rows. Missing fields within those known rows count as missing operands.

Started requests without a linked receipt have an unknown command count and
unknown phases. Preserve them as unassigned-receipt request coverage at
trial/condition/request-operation scope. Do not distribute them as invented
missing commands or operands among phases. Unknown planned-trial request counts
remain separate too. Complete command count is unknown when either exists;
observed command count still describes its exact serialized domain. An
unprepared trial is not evidence that zero operations occurred.

Generated-C metric eligibility is observed emit-c commands only. A known
generated-C observation additionally requires positive generated-c role and
known stdout bytes. Missing emit-c role or stdout is unknown for that eligible
observation. Non-emit commands are ineligible, counted separately, and never
manufacture missing C observations. A command phase/status list does not prove
an event for an unassigned receipt.

Before adding nonnegative I64 operands, use the existing addition-fit predicate.
Overflow makes only that sum unknown-overflow permanently, without evaluating
the addition; preserve counts, maxima and other metrics. Sums cover the named
known domain rather than complete cost. Repeated generated-C observations are
emission extent, not deduplicated program size. Feedback is the recorded wrapper
envelope extent, separate from command output and generated artifacts.

Maximum ties choose the smallest original request sequence, then lexical UUID,
then command ordinal. Provenance includes the trial ID. For maxima across
trials, if that entire tuple ties, choose lexical trial ID as the final stable
tie-breaker. No sum, product, midpoint, ratio ordering or duration subtraction
is needed. The consumer checks sequence range but does not claim supplied
sequence uniqueness or ledger association; the adapter checks that association.

### Exact aggregate report grammar

Output is canonical netstring data, with no newline-delimited identifier or
alternative human/detail form. The header triple is `(format, freeze SHA,
identities)`, where format is exactly `slim-development-operation-cost-report-1`
and identities is the same three-frame compiler/evaluator/manifest payload as
input. Supplied identity
spelling is not verifier authority.

Each group is exactly `(kind, identity, data)`. Identity always contains three
frames: `(trial ID, selector, condition)`. For trial groups selector is task ID;
for condition groups trial ID and selector are empty; for operation/phase groups
trial ID is empty and selector is the exact operation/phase. Data always
contains exactly five frames `(outcome, lifecycle, coverage, statuses, metrics)`.
Outcome/lifecycle are copied from the trial for trial groups, using input's empty
terminal lifecycle; both are empty as inapplicable controls for other groups.
Coverage, statuses and metrics are nested payloads specified below.

Emit groups in exactly this order:

1. For each trial in canonical task/condition order: trial-request then
   trial-command.
2. For baseline then context: condition-request then condition-command.
3. For baseline then context, each operation in check, interfaces, context,
   build, run order: operation-request.
4. For baseline then context, each phase in check, interfaces, context, emit-c,
   native-compile, execute order: phase-command.

Request-group coverage has exactly nine canonical nonnegative integer frames:
planned trials, trials with unknown request count, observed requests, started
requests, finished requests, unstarted finished requests, unfinished requests,
linked-receipt requests, unassigned-receipt started requests. Operation-group
unknown-trial coverage names potential unassigned scope rather than fabricated
request rows. Statuses has exactly ten observed-row counters in this order: ok,
compiler-error, timeout, output-limit, resource-limit, infrastructure-error,
constraint-error, denied, interrupted, unfinished. It is not a complete count
claim when request scope is unknown.

Trial/condition command coverage has exactly nine frames: planned trials, trials
with unknown request count, linked-receipt requests, unassigned-receipt started
requests, observed commands, complete command count, observed emit-c commands,
observed non-emit commands, emit-c commands with unavailable role. Complete
command count is empty/unknown if unknown request scope or an unassigned started
request exists, otherwise the canonical observed count, including known zero.
Phase-command coverage instead has exactly four known observed-domain counters:
commands, emit-c commands, non-emit commands, emit-c commands with unavailable
role. It carries no inferred unassigned-receipt count. Every command-group
statuses payload has exactly six observed counters in order: ok, compiler-error,
timeout, output-limit, resource-limit, infrastructure-error. Command status
reports the checked evaluator classification; request terminal status remains
independent. The adapter checks status/returncode association without exposing
returncode in this transport.

Request metrics appear in capture-ns, queue-ns, wrapper-ns, feedback-bytes order.
Command metrics appear in command-ns, executable-ns, stdout-bytes, stderr-bytes,
generated-c-bytes order. Each metric is one nested frame whose payload has
exactly nine frames: name, eligible count, known count, missing count, sum, sum
reason, maximum, maximum reason, provenance. Counts refer only to the eligible
observed domain, with known plus missing equal to eligible. For generated-c-
bytes this is emit-c only; for ordinary metrics it is every row in that group's
request/command observed domain. Ineligible generated-C count is the separate
non-emit counter. Empty eligible domains have known/missing zero.

Sum is canonical nonnegative I64 with reason exact, empty with reason empty
when no observation is known, or empty with reason overflow after guarded sum
exhaustion. Maximum
is canonical nonnegative I64 with reason exact when any observation is known;
otherwise empty with reason empty. Provenance always contains exactly four
frames `(trial ID, request sequence, request UUID, command ordinal)`. Request
metrics use ordinal zero; command metrics use 1..3. All four are empty when
maximum is unknown. An exact maximum of zero retains nonempty provenance.
No unknown value is encoded as a numeric zero. Empty outcome controls and empty
unknown observation fields have their distinct meanings only in these fixed
positions.

The footer is exactly `(work, counters, unknowns)`. Counters has five canonical
frames in parsed-frame/request-visit/command-visit/numeric-metric-update/total-
work order. Unknowns has exactly six `unknown` frames in model-tokens,
model-calls, active-model-time, native-application-performance,
general-development-effectiveness and kernel-boot-continuity order. End at exact
EOF. Timing aggregates remain conditional same-host monotonic observations;
the fixed report is not model/native-performance or boot attestation.

## Compiler and runtime design

Implement ordinary SLIM model, validation, aggregation and report components
under `library/components/`, with one application/project and synthetic golden.
Reuse framing, canonical integers, overflow guards, byte comparison, the fixed
trial-key index and buffered emission. Keep alloc, io and partial effects
visible. Public component records are not opaque; validated-result guarantees
apply only to unmodified records returned by the validator. No production
selfhost, C seed, runtime, ABI, dependency or surface-ledger change is needed.
Prefer the existing `development_data.matches`, `hash_valid` and `number`
validators and public `development_model.Number`, `Status` and `Aggregate`
records where their contracts fit. The new application manifest may expose
exactly the needed existing validators, including outcome/lifecycle and the
existing byte comparator, without changing their semantics or existing
manifests/consumers; do not add equivalent aliases or copies. Preserve
`std_byte_index.build`'s 4,096-entry hard cap. The global request ceiling uses
per-trial adjacent UUID validation rather than a larger index.

## Trusted measurement adapter

The fixed script imports only fixed repository summary-adapter/evaluator code,
never a module named by input. Reuse the summary adapter's pinned fixed
evaluator loader, bounded reads and identity/lifecycle barriers, and the
evaluator's existing frozen/manifest/ledger/prepared/receipt/output/terminal
result validation. Require the fixed evaluator source SHA to equal the frozen
evaluator identity before import. Validate receipt/output identities for
unresolved trials too, not only terminal evaluations. Read all planned rows
from the same validated frozen manifest; never repair or re-score outcomes.

Additionally validate present measurement metadata, because
`validate_receipts` alone verifies receipt/output identity rather than all of
its fields. Bind schema 2, request UUID, operation, compiler SHA, complete source
identity and expected identity to that request's checked request/started records.
Bind original sequence and lifecycle to the checked ledger, and validate
recorded phase prefixes: one check/interfaces/context command, or the supported
emit-c/native-compile/execute prefix for build/run. Validate exact known status,
role and timing-control spellings, numeric type/range, supported null/missing
observations, launch-evidence shape and executable-duration association. Reject
contradictory present metadata. Do not infer failure causes from output text,
replace missing evidence or reinterpret SLIM semantics.
Check status/returncode association against the fixed evaluator contract: ok
requires zero returncode; additional phases require the preceding command's
positive ok result; generated-c role is installed only after positive emit-c
success. Preserve the evaluator's backend/signal/infrastructure classification,
including an unknown native nonzero cause where it remains unknown. Returncode
is checked metadata and need not become another reporting dimension.
The fixed launcher source may be the repository evaluator or this validated
freeze's `corpus/evaluate.py` copy. Admit only those two derived absolute paths,
after observing identical evaluator bytes against the pinned frozen identity.
The recorded path is an identifier: it never chooses code to read, import or
execute. Executable duration measures the recorded pre-exec marker to direct
reap, not execution inside the program's main.

Perform no additional live candidate or submitted-source capture/path reread
to derive operation measurements. Hash source-identity objects using the
evaluator's canonical JSON encoding. Inherited frozen validator file/tree hash
checks retain their accepted scope and bounds. Execute no shell, compiler,
oracle or data-provided command. The normal checked evaluator result remains
the acceptance authority; no second parser, checker or semantic fallback exists.

Pin observed new-adapter and reused summary-adapter source bytes before import
and conversion, together with evaluator/freeze/manifest identities. Observe
prepared, ledger, terminal-result and linked receipt/output identities before
and after conversion; reject changed bytes or lifecycle. Bind the framed output
digest and byte length to these identities, observed row counts and explicit
unknowns in a separate canonical JSON receipt, capped at 4 MiB. Preflight the
complete receipt and framed input before creating fresh distinct output files.
Receipt hashes are explanatory data, not authority without this fixed verifier.

Writers must remain stopped. The inherited adapter limits remain 16 MiB/4,096
records per ledger, 64 KiB per record, 1 MiB per JSON document and at most 64
ledgers, plus the existing frozen/file/tree/output validation ceilings. Do not
infer an aggregate read-byte, filesystem, CPU, RSS, model-cost or validation-time
bound. Before/after observations do not attest loaded interpreter bytecode,
exclude ABA replacement, prove an atomic snapshot or establish kernel boot
continuity. Preserve same-host/boot-conditional timing and advisory filesystem
integrity scope. Host write failure may leave partial fresh adapter outputs;
there is no atomic multi-file transaction.

## Diagnostics and failure cases

Application invocation error exits 64; ordinary read failure exits 66; data,
admission and report errors exit 65; allocation failure retains 71. Print
`error CODE at POSITION\n`. Use 10 admission, 11 format, 12 identity, 13
identifier, 14 enum/control spelling, 15 noncanonical integer, 16 integer
overflow, 17 range, 18 inconsistent metadata, 19 field/count mismatch, 20
task/pair order, 21 duplicate trial, 22 UUID order/duplicate request, 23 report
admission and 24 work admission. Reuse 100 plus netstring error code for framing.
Read and invocation diagnostics use 1 and 2 respectively.
Validate fields in original source order before trial-ID indexing; duplicate
trial errors identify the second original key span in the index's stable order.
Other offsets identify the offending original field or first excess record.
An excessive declared request count uses range code 17 at its numeric payload.
The 129th started request uses admission code 10 at its UUID payload after
validating that request's metadata. Excess trials, global request records and
command records use code 10 at the excess record's first frame header, before
parsing that record.
Whole-input, parameter and report admissions use zero.

Adapter refusal names stable categories: unsupported-evaluator,
invalid-frozen-lifecycle, invalid-receipt-metadata, changed-identity,
changed-lifecycle and admission. Preserve the underlying checked validation
reason. Refusal or missing measurement does not change an existing result.

Assemble the successful report completely before stdout. Validation, admission,
assembly or allocation failure before publication emits no successful report
prefix. Ordinary `io.print_bytes` can publish a prefix before a host short-write
trap; neither transactional stdout nor a runtime change is claimed.

## Compatibility and migration

Use one new input format and one aggregate-only command. RFC-0164 retains its
existing purpose, input, adapter and optional statistic unchanged. This adapter
receipt does not make arbitrary supplied input or outcome labels authoritative.
No detail command, compatibility spelling, frozen participant feedback change,
new source representation, dependency or relaxed performance gate is introduced.

## Performance and complexity

Consumer collection visits each request/command once and parses bounded scalar
fields. UUID adjacency visits at most 36 bytes per request; the only index is
the existing fixed 64-trial index. Report grouping has fixed condition,
operation and phase cardinalities. Default collection is roughly linear in
admitted input plus that fixed bounded index, without quantile sorting or an
unbounded per-record scan. The adapter's per-trial UUID ordering is separately
bounded at 4,096 requests, with at most 8,192 globally; measure it separately.

The full frame envelope is 336,518: six header/identity frames, at most ten per
trial, fourteen per request and nine per command. Track parsed frames, request
visits, command visits and numeric-metric updates. A request updates four metrics
in trial/condition/operation groups; a command updates at most five in
trial/condition/phase groups. The corresponding maximum is
`336518 + 8192 + 24576 + 8192*12 + 24576*15 = 836230` work units. Use a default
checked cap of 1,000,000. Any reusable component work parameter must be in
0..1,000,000; reject exhaustion before incrementing or doing that work. These
units exclude instructions, allocation, byte copying and inherited verifier
work, whose independent byte/count limits remain explicit. No CLI work override
or second reporting mode is added.

For exact work counters, every request performs twelve metric visits and every
command fifteen. A metric visit includes its eligibility check and consumes
one work unit even when ineligible; that visit then changes no metric evidence.
Thus non-emit commands incur no generated-C known/missing observation. Index
work and byte copying are separately bounded rather than included in this unit.

Check lengths, counts, indices, counter increments and budget arithmetic before
evaluating them. All request/command/count operands fit their stated domains;
metric sums use the guarded I64 rule. Canonical sequence is at most 4,095,
ordinal at most three, and global request/command counts at most 8,192/24,576.
No admitted counter or envelope multiplication can overflow I64.

The exact grammar has a conservative report envelope below 512 KiB. There are
at most 128 trial groups plus four condition groups, ten operation groups and
twelve phase groups: 154 groups total. Bound each group at 2,304 bytes and
header/footer together at 2,048, yielding
`154*2304 + 2048 = 356864 < 524288` bytes. Counts have at most five digits;
use a conservative 20-byte I64 observation spelling. A provenance payload is
at most 184 bytes: framed 128-byte trial ID, four-digit sequence, 36-byte UUID
and one-digit ordinal. Its enclosing frame is at most 189 bytes. Each metric
payload is at most 319 bytes and its enclosing frame at most 324: framed name
(at most 32 bytes), three counts, sum, sum reason, maximum, maximum reason and
provenance. Five metrics including their enclosing list occupy at most 1,626
bytes; nine coverage fields at most 85, and ten status fields at most 95.
Outcome/lifecycle frames reserve 36/68 bytes. Thus data payload is at most
1,910 bytes and its frame 1,916. Identity frame is at most 218 bytes and kind
frame 36; the group is at most 2,170, within 2,304. Header/footer fit the
separate 2,048 reservation. Check these admitted bounds before arithmetic and
serialization. Tests exercise maximal spellings/values and complete output.
Refuse the whole report if a grammar envelope or complete output ceiling is
breached; discard no group or provenance field.

Measure ordinary/sanitized consumer runs, additional adapter conversion and
inherited validation separately. Use geometric rows/frames, deterministic work
and source/artifact identities. Operator elapsed, observations from prior
requests, native application performance and model-active time stay distinct.
No existing regression budget is removed or relaxed.

## Alternatives and drawbacks

Python-only receipt summaries avoid an ordinary consumer but retain reporting
outside maintained SLIM applications. Adding fields to RFC-0164 would widen a
completed outcome/elapsed contract. Quantiles, detail output, deduplication and
model attribution add permanent costs without current necessity. Admitting all
possible denial-heavy ledgers would greatly enlarge the independent report
domain. The chosen complete-input refusal is explicit. Buffered reports and
fixed metadata/verification work have measurable costs, currently unknown.

## Test and acceptance plan

Use production ordinary and address/undefined-sanitized binaries and an
independent finite Python framing/report oracle. No oracle implements SLIM
syntax, typing, ownership, effects or candidate acceptance. Cover:

- Every operation, phase, terminal status and lifecycle, including unstarted
  denied/no receipt, terminal started/no receipt, unfinished, unresolved,
  prepared not-run and unprepared unknown counts. Preserve all planned rows.
- Crossed known/missing fields, zero, I64 maximum, exact sum endpoint, overflow
  by one and later observations after overflow. Other dimensions and maxima
  remain available. Test every maximum tie level and supplied sequence reuse.
- Write the independent exact output oracle from the report grammar before
  implementing its serializer. Cross emit-c role/stdout absence, non-emit
  ineligibility and unassigned receipts: phase domains gain no invented missing
  command, and every status/coverage field retains its fixed scope and order.
- 0/1/32/33 tasks, 64/65 trials, 128/129 starts, 4,096/4,097 requests per trial,
  8,192/8,193 globally and three/four commands; exact byte, scalar, metadata,
  command-list, output-envelope and work boundaries. A valid workload at its
  exact work cap passes and the same workload one unit below refuses.
- Truncated/extra/noncanonical frames, count disagreement, invalid ordering,
  duplicate trial/request IDs, missing pair, trailing bytes and original error
  offsets. Malformed present integers/booleans/negative values reject.
- Independently altered schema/request/operation/compiler/source/expected,
  phase/role/status/numeric/timing metadata with recomputed valid receipt hashes;
  hash validity alone cannot bypass checked association. Cross launch failure,
  unavailable executable observations, status/returncode association, positive
  predecessor requirements and positively recorded generated-C role.
- Changed evaluator/adapter/summary-adapter/freeze/prepared/ledger/result,
  receipt/output identities, lifecycle changes during read and orphan artifacts.
  Ensure no shell, compiler, oracle or input-selected module is executed.
- Allocation failures before report publication, maximal 64-trial output with
  maximum provenance/value spellings, and geometric collection/serialization
  work. Keep successful and failed measurement receipts in ignored build paths.
- Actual stopped-writer frozen-5 first-pair conversion and exact independent
  oracle parity; inspected operands alone do not satisfy this acceptance gate.

Register the focused permanent verifier, synthetic corpus row and exact golden.
Run inexpensive scoped checks first, then the required repository gates in a
coordinated quiet slot before commit. Reuse a completed gate only for unchanged
source and artifact identities. Keep one concise current result and roadmap
status; no dated operator journal or modified frozen cohort is required.

## Ratings and evidence

All ratings and score are neutral. This is an ordinary compatibility/tooling
feature with no primitive; score zero is not evidence of zero runtime, storage,
validation or maintenance cost. The recorded queue/phase distinction establishes
a useful reporting gap. Implementation cost, measured feedback benefit and
general development effectiveness remain unknown until their own validation.

## Decision

Explicit coordinator acceptance on 2026-10-03 under the maintainer's delegated
overnight ordinary-library direction, after review of independent dimensions,
complete framed report grammar, arithmetic and output/work envelopes. Acceptance
precedes implementation. No participant/evaluator change, new language primitive,
dependency or performance-budget relaxation is authorized.

## Implementation

Pending. Create ordinary components/application/project, fixed adapter and
independent focused verifier after acceptance. This RFC draft itself executes
no compiler, native validation, benchmark or frozen evaluation.

## Removal and supersession

Remove or replace this consumer if it cannot preserve complete admitted rows,
checked metadata association, independent missing evidence, safe arithmetic,
deterministic provenance or durable gates. Preserve RFC-0164 and original frozen
measurements. A replacement must retain one canonical transport and the fixed
evaluator's acceptance authority, with no semantic fallback.
