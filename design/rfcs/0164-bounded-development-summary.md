# RFC-0164: Bounded ordinary development summary

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, under delegated overnight ordinary-library direction
Created: 2026-10-02
Approver: project-maintainer
DecisionDate: 2026-10-02
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

Add an experimental ordinary SLIM `development-summary INPUT.ns` application
and reusable source component. A trusted Python measurement adapter converts
the existing validated protocol-2 ledger summary into its one canonical framed
input. This is outside the frozen experiment: no task, feedback, oracle, trial
budget or participant operation changes. It introduces no language operation,
compiler path, dependency or universal quality score.

## Motivation

The current evaluator retains independent outcomes and elapsed observations.
Consume those actual records with the ordinary framing/index components, rather
than derive model success from edit size or native benchmarks. Report all planned
tasks, including incomplete and unprepared trials, and preserve missing evidence
as unknown. This consumer does not evaluate candidates or establish acceptance.

## Guide-level explanation

The default command reports independent counts and accepted-pair elapsed
operands. An explicit `stats` command adds bounded terminal-time order statistics.

## Reference-level specification

Admit at most 32 task pairs, 64 trials and 131,072 input bytes. Task IDs are
nonempty byte strings of at most 64 bytes; trial IDs have at most 128 bytes.
Each nested trial payload has at most 1,024 bytes. Report admission is fixed at
65,536 bytes. Source admission follows ordinary file read; no pre-read physical
storage or allocation bound is claimed. Allocation failure retains exit 71.

## Compiler and runtime design

Reuse `std_netstring.parse`, `framed_records.parse`/`integer` and
`std_byte_index.build`. The first triple is `(format, freeze SHA, identities)`:
format is exactly `slim-development-summary-1`, freeze SHA is 64 lowercase hex
bytes, and identities is a nested triple of compiler/evaluator/manifest SHA
strings in that order. Every following triple is `(task ID, baseline trial,
context trial)`. Conditions are structural, not freely interpreted labels.

Each trial contains exactly these frames, in order:

1. Trial ID, nonempty.
2. Outcome: accepted, correctness-failure, infrastructure-failure, interrupted,
   elapsed-timeout, constraint-failure, unresolved or not-run.
3. Oracle evidence: `true`, `false` or `unknown`.
4. Elapsed nanoseconds: empty for unknown, otherwise canonical nonnegative I64.
5. Basis: `same-host-monotonic-conditional` or `unknown`.
6. Observed started operations: empty for unknown, otherwise canonical 0..4,096.
7. Observed requests: empty for unknown, otherwise canonical 0..4,096.
8. Lifecycle observation: empty for a terminal result, otherwise one existing
   evaluator observation (unprepared, prepared-undispatched,
   dispatched-unsubmitted, unfinished-submission, submitted-awaiting-evaluation,
   unfinished-evaluation).
9. Submitted-source SHA: empty for unknown, otherwise 64 lowercase hex bytes.

Unknown elapsed requires unknown basis; a present elapsed value retains its
conditional monotonic basis and never asserts kernel boot continuity. Accepted
requires true oracle evidence. Known started operations cannot exceed known
requests. Unresolved and not-run outcomes require unknown oracle evidence;
terminal infrastructure outcomes may retain unknown evidence. Duplicate
tasks/trial IDs, trailing frames, invalid enum/identity/
integer fields and inconsistent records reject the entire input. Validation,
admission, assembly and allocation failures before output print no successful
report prefix. Ordinary `io.print_bytes` may publish a prefix before a host
short-write trap; no transactional stdout or runtime ABI change is claimed.
Unknown fields are never replaced with zero or false.

## Diagnostics and failure cases

Validate frame/field shape in original input order, then build the task index and
reject duplicate keys in its stable lexical order. Diagnostics print
`error CODE at POSITION\n`, exit 65; framing/integer errors retain original
source offsets, duplicate errors identify the second original key span. Excess
task count identifies the first excess record; whole-source/output/parameter
admission errors use zero. Invocation uses exit 64, read failure 66. Successful
reports are assembled completely before stdout, in task-byte lexical order.
Public records are not opaque: component guarantees apply only to unmodified
values returned by its validator.

## Compatibility and migration

Header identities are supplied data whose spelling is checked; the SLIM
consumer does not establish source authority from them. The trusted adapter
retains a separate receipt binding its output digest to validated frozen,
evaluator, ledger and result identities. Input fields never establish model
calls, executed traces or kernel boot continuity.

## Performance and complexity

Report counts by condition and each outcome, known true/false/unknown oracle
counts, and observable operation/request counts with unknown counts separate.
For each condition, report the number of known elapsed observations and their
checked sum; sum overflow is unknown while other facts remain available.
Elapsed selection includes all terminal submitted trials with known elapsed,
including failures and timeouts. Unresolved and not-run observations are
excluded even when a cutoff duration is present. Missing terminal elapsed is
reported separately; missing values do not become zero.

Only the explicit `development-summary INPUT.ns stats` command reports the
elapsed median as two middle order statistics `(lower, upper)`. With
odd cardinality they are equal; with even cardinality their mathematical
midpoint is intended, without evaluating an overflowing addition. Empty elapsed
domains report unknown. This explicit bounded statistic admits at most 32 values
per condition and checks a maximum of 1,024 insertion-work units per condition;
its bounded status and observed work are reported separately. Each displaced
value or terminating comparison consumes one unit. The default command performs
no quadratic statistics. Parser and evidence collection visit input bytes and
fields approximately linearly; fixed-size indices retain the existing component
limits, with no unbounded per-record scan.

Paired oracle counts are TT/TF/FT/FF only when both observations are known;
every other pair remains unknown. Per task, elapsed ratio evidence is the exact
raw pair `(context nanoseconds, baseline nanoseconds)` only when both trials are
strictly accepted, both elapsed values are known and the baseline is positive.
Report the strictly accepted pair subset count beside complete all-outcome
counts. No float, product, subtraction or ratio ordering is
required. Missing observations or a zero denominator report distinct unknown
reasons. These are descriptive conditional elapsed facts, not causal or native
performance claims. Model tokens, model calls, active model time and general
development effectiveness remain unknown unless independently observed.

### Trusted measurement adapter

Import only the fixed repository evaluator, never code selected through an input
path. Require its digest to match the frozen evaluator identity and use its
existing manifest, ledger seal, receipt and terminal-result hash validation.
All planned trial rows come from that one frozen manifest; absent runs remain
explicit. Preserve oracle evidence independently of strict outcome, including
unknown evidence on incomplete acceptance. An unsupported evaluator identity or
invalid ledger refuses conversion rather than repairs or re-scores a result.
Pin observed adapter source bytes before conversion and reject a changed digest
before returning the framed receipt. This records stable observed source bytes;
it does not attest interpreter-loaded bytecode or exclude ABA replacement.
Writers must remain stopped while this trusted adapter reads its inputs.

The adapter inherits the current fixed evaluator bounds: 16 MiB/4,096 records
per ledger, 64 KiB per record and 1 MiB per JSON document. At most 64 ledgers
are admitted (at most 1 GiB of ledger bytes); frozen/source/receipt checks retain
the evaluator's declared per-file and tree bounds. No aggregate filesystem,
native CPU, validation-time or model-cost bound is inferred from those caps.
Framed output has the consumer's explicit byte ceiling. Do not execute a shell,
compiler, oracle or data-provided command. The adapter is measurement tooling,
not an independent SLIM parser/typechecker or acceptance authority.

## Alternatives and drawbacks

Keeping analysis only in Python repeats reporting outside ordinary SLIM.
The component adds explicit metadata, counters and buffered output costs; fixed
indices and the optional bounded statistic keep their domains visible. It does
not reduce those costs to a universal score or widen the frozen experiment.

## Test and acceptance plan

Use production ordinary/sanitized binaries and an independent finite Python
measurement oracle. Cover 0/1/32/33 tasks, 64/65 trials, malformed/truncated and
extra fields, duplicate identities, original offsets, source/payload/output
ceilings, missing elapsed, zero denominator, exact I64 endpoints, sum overflow,
and midpoint/ratio operands at I64 maximum. Include actual validated-ledger
conversion and changed result/ledger/freeze identity rejection. Preserve every
planned row and cross unknown outcomes without a negative quality inference.
Exercise allocation failures and bounded work; keep raw receipts in ignored
build directories and one concise current result. Required repository gates
remain; no existing budget is relaxed.

## Implementation

Implement the reusable model, validation, statistics and report components in
`library/components/development_summary_*.slim`, the ordinary application in
`library/applications/development_summary/main.slim`, and fixed adapter and
focused verification tools in `scripts/`. Register a synthetic input and exact
golden in the ordinary library corpus. Scoped production validation and the
required repository gates remain acceptance requirements.

## Ratings and evidence

All ratings are neutral, not claims of zero cost. Utility, speed and general
effectiveness are unknown until measured in their stated domains.

## Decision

Acceptance is explicit coordinator authorization under the maintainer's delegated overnight
direction on 2026-10-02. This decision adopts the preregistered definitions in
`benchmarks/development/PROTOCOL.md` (definition SHA
`eb3e33ed0f5ab8d3555abdf461705012d40fdc7036d71a5556f470c163ee3bc0`):
all terminal submitted outcomes in elapsed medians, only two strictly accepted
trials in paired ratios, complete denominators alongside subset facts, and
unknown model traces/costs. The ignored local preregistration receipt is
measurement evidence rather than a dependency of this contract. The scoped
feature is implemented and passed finite ordinary/sanitized and trusted adapter
validation. Full repository closure remains pending until coordinator gates.

## Removal and supersession

Replace with a smaller ordinary component only if exact identities, independent
unknowns, planned denominators, deterministic offsets and durable gates remain.
Do not retain a second input spelling, inferred model score or semantic fallback.
