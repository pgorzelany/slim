# RFC-0171: Ledger transaction overflow preflight

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, under delegated ordinary-library development direction
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

Make credit, debit and transfer reject an unrepresentable prospective account
update before mutation. Add ordinary ledger rejection code44 after all existing
checks. Modify only `library/applications/ledger/state.slim`; retain public APIs,
the production manifest, parser, effect ceilings, compiler and runtime contracts.

## Motivation

The maintained ledger currently checks missing, closed and insufficient accounts
but then adds balances/counters directly. The existing `overflow.log` exercises
an unrepresentable input literal, not a representable amount whose account update
overflows. A replay opening alice at I64 maximum then crediting1 should retain
alice and count a rejected command instead of trapping. This is a useful real
ordinary-SLIM repair on which to dogfood the current tools. Static source review
establishes the missing guard; actual baseline/candidate behavior remains unknown
until the separately approved native campaign.

## Guide-level explanation

Existing accepted operations and rejection codes remain. After prior checks,
an overflowing balance or operation counter returns `Applied::Rejected(44)`.
A rejected command preserves the entire account vector. A successful transfer
prepares both complete replacement records before setting from, then to, once.
`process` counts rejection44 normally. This does not make arbitrary ledger logs,
public Account values, aggregate totals or process counts total.

## Reference-level specification

Let M=9223372036854775807. Source spans must be valid for the supplied Bytes,
account names unique, and vector accesses valid. Every scalar account field and
amount may be any representable I64, including negatives; no application invariant
is inferred merely from Account's representation. The existing gates decide
eligibility. Open and close remain entirely unchanged.

Preserve every existing rejection and its precedence: open31 negative then32
duplicate; credit33 missing/nonpositive then34 closed; debit35 missing/nonpositive,
36 closed, then37 insufficient; transfer38 missing/nonpositive,39 same account,
then40 closed/insufficient; close41 missing,42 closed, then43 positive balance.
Only after those checks can44 occur. Do not move lookups, checked span work or
record reads across their original position.

Credit checks `balance > M-amount` and `credits == M`. Debit checks `debits == M`.
Transfer checks `to.balance > M-amount`, `to.credits == M`, and `from.debits == M`.
All those predicates precede any prospective overflowing addition and the first
vector mutation. Do not reject an unaffected field merely because it is M or
negative. Preserve both record-construction order and successful vec.set order.
There is no new aggregate sum, scan, allocation or compensating mutation.

The local arithmetic argument uses the already admitted positive amount:
1<=amount<=M makes M-amount representable. balance<=M-amount makes balance+amount
at most M; any representable negative balance plus a positive amount is above
I64 minimum. A representable counter other than M can add1 safely, including
I64 minimum and negative values. Existing funds checks make the from/debit
balance subtraction nonnegative. All branch computations terminate. On ordinary
representable updates the same values and mutation order result; only newly
guarded arithmetic-trap paths become rejection44. Existing allocation failures,
lookups, earlier failures and later process/report traps retain their contracts.

## Compiler and runtime design

Use ordinary SLIM scalar comparisons/arithmetic and existing Rejected(I64).
Keep alloc/partial declarations, ownership parameters and exclusive calls. No new
enum, compiler analysis, parser, runtime primitive, dependency or production API.
Only the credit_account,debit_account,transfer function bodies may change; retain
every other production function and all three signatures/effect declarations exactly.
The production compiler checks every complete source and test caller. Test-only
probe manifests may export apply, copying the exact pinned production body;
the production ledger.project remains unchanged. Generated C review must confirm
guards before updates and preservation of checked arithmetic and region cleanup.

## Compatibility and migration

Existing success bytes, codes31..43 and parser diagnostics remain exact. New44
is an ordinary application behavior change requiring this accepted contract.
No source migration. Frozen protocol2 materials, models, trials and outcomes stay
immutable. This maintainer repair is exposed dogfooding, never a fresh participant
or replacement for the undispatched ledger trial.

## Diagnostics and failure cases

State helpers return Accepted or Rejected(code); they do not print diagnostics.
The CLI continues counting ordinary rejected commands and using its existing
report. Allocation exhaustion remains71. Aggregate `total`, process accepted/
rejected count additions, malformed caller spans/vectors and host output failure
remain checked partiality. A fixed aggregate-overflow replay and negative-index
total caller must still trap70 with their original runtime stderr; no recovery,
saturation, rollback handler or new successful output is added for those domains.
Positive/negative caller controls preserve ordinary effect checking and exact
source-mapped diagnostics. Sanitizer leak detection is unavailable on this host.

## Performance and complexity

Added work is constant per eligible credit/debit/transfer: at most one safe
subtraction, one balance comparison and two counter comparisons. Existing
account lookup remains linear in account count. No whole-vector preflight scan,
new allocation or public cost promise. No existing budget is changed.

The fixed focus budget is900s overall,60s per native child; stdout16MiB only for C
emitters,8MiB otherwise, stderr256KiB, native file128MiB; fresh ignored output,
live bounded draining, unconditional process-group kill/direct wait on every
exit, before/after source/tool/fixture pins and UTC+monotonic wrapper timing.
No retries, automatic adjustment, CLI widening or arbitrary recorded executable.
Data hold caps:192 cases,128 files,4MiB model,16MiB all files including receipt.
Physical memory, libc allocation, host/transitive toolchain and ABA remain unknown.

## Alternatives and drawbacks

Keep the checked trap: smaller code, but a valid positive command can halt replay.
Saturation changes account values; catching a trap cannot establish preservation.
Scanning aggregate totals per command adds unrelated work. None is accepted.
Additional guards and permanent tests carry maintenance cost, not a free benefit.

## Test and acceptance plan

Before acceptance/production edits, independently review and hold an import-safe
pure-data oracle. It uses mathematical integer poststates, not the future guard
implementation, parses no SLIM/logs and invokes no compiler/reference. Pin each
fixed input, full poststate and exact byte result. All171 state cases include
full vector order and name spans plus an untouched third account:50 credit,
50 debit,40 transfer,4 negative-counter transfer controls and27 precedence/open/
close controls. Include I64 minimum, negatives, zero, M-1, M, amounts1/M, simultaneous
overflow candidates and unchanged max/negative fields. Eight literal CLI replays,
two partial controls, two diagnostic callers and one baseline reproduction yield
184 cases, below192.

The fixed prospective plan has46 native leaf observations:1 cleanup control,
3 baseline emit/build/run observations,2 complete checks,2 emissions,
4 ordinary/sanitized builds,2 complete state batches,
16 replay runs,4 partial runs,2 diagnostic checks,3 semantic-context queries and
7 RFC170 roles.
Invoke RFC170's canonical main in process from captured hash-checked collector
bytes, with fixed sys.argv and redirected stdout/stderr, restoring sys.argv on
every exit as in its verified invoke_main harness. The public collector CLI is
unchanged; this wrapper creates no Python child. The active parent900s timer is
shared: collector run takes the smaller remaining parent time and its own900s
ceiling, restores only the remaining parent deadline and cleans each native
role's owned process group. Observe wrapper elapsed separately from its seven
actual native children without adding their overlapping durations.
All ordinary/sanitized outputs/status/stderr must match independent frozen data.
Complete batch output has171 case sections per variant; failures are never
replaced by the expected prefix or retried. Later unfinished work remains unknown.
Run the fixed closed-pipe descendant cleanup control and pin its helper source;
successful leader exit alone does not establish complete group cleanup.
Before loading any fixed oracle/helper, validate captured source bytes, complete
held model/fixture inventory and direct tool pins. Use ModuleType plus compile
of the captured hash-checked bytes, never cached bytecode or a data-selected path.
The companion campaign-spec.md fixes argv templates, source copies and label order
before data materialization. Future implementation/verifier pins are held separately
before their one native attempt; they never replace independent expected data.

For actual dogfooding retain independent complete before and expected ledger
snapshots, each exact ledger.project plus its nine fixed source bodies preserving
relative layout. Request existing `context BEFORE EXPECTED ledger_state.SELECTOR`
for credit_account, debit_account and transfer individually; apply is just dispatch.
SOURCE and EXPECTED are deliberately byte-identical separate baseline trees:
RFC159's second input is expected source identity, not a changed-source comparison.
These are baseline checked-context queries; RFC170's before/after supplies the diff.
Retain raw output/code/timing without inferring an application invariant. Run
RFC170 against stopped-writer before/after snapshots. Independently expect unchanged
manifest, one changed module and current candidates ledger,ledger_emit,ledger_state
from the fixed declared graph. Source/weight/digest identities are measured separately,
not a semantic oracle. Preserve exact captured/report authority. Record complete
wrapper/role elapsed, diagnostics, repair/check iterations where observed, source
and report bytes and manual steps. This single exposed repair cannot establish
causal effectiveness, saved native time, minimal context or model active time.
Reproduce the held before snapshot's maximum-balance credit trap through one
ordinary actual production emission,CC build and CLI run. Expect70, empty stdout
and the fixed I64 addition-overflow runtime stderr before observation; never use
actual baseline output to select a replacement expectation. The after R01 replay
must instead complete0 with one accepted/one rejected command and unchanged balance.

Existing library corpus, canonical formatting, required repository checks and
source-bound repository/release/website integration precede adoption. Keep total
overflow visible and no compiler-analysis baseline row is silently reclassified.

## Ratings and evidence

Neutral ratings0/Score0 pending actual finite behavior and costs, not zero cost.
Independent native conformance is bounded to184 cases; the local arithmetic
argument names a larger exact scalar domain conditional on valid original callers.
Workflow usefulness, physical resources and general effectiveness remain unknown.

## Decision

Accepted on 2026-10-03 under the maintainer's explicit delegated authority to
choose and implement ordinary SLIM improvements during the overnight work window.
Independent text and data reviews precede this decision. The fixed 184-case
model is `f04786d828c5e86cce4eb11f340fad6c9be47f33ba4a285d97a6c9ddc3033285`;
its 125-file hold is `53377162af22d0f826cc07ea0c67dd3e1d9b5acdc0a477490bd04e60f2fbc4df`.
These identities establish prospective data custody, not source or native acceptance.
The reviewed focused native002 passed all46 observations and184 finite cases,
including retained partial controls and baseline reproduction. The original
native001 artifact-count failure remains preserved. Full current-source
repository/release/website integration remains pending; no existing budget is relaxed.

## Implementation

The three ledger state guards and fixed test observer/oracle/verifier are
implemented. The permanent verifier freshly holds current bounded opaque sources,
its actual accepted RFC bytes and one fixed original-state regression fixture;
both baseline and current execute through the normal production compiler.
Historical complete project trees, model receipts and timings remain ignored
evidence rather than future gate prerequisites. The184 expectations,46 roles,
flags, logical budgets and original source-bound focused receipts remain.
The canonical fresh current campaign passed184 fixed cases and46 child roles
with a128-file hold, model
`a8f5ceea2c700e728b95d89306b17c183f91639ec37ebd3e086455a45dcc5bea`,
freeze `f9bec8d7e1218aae27bb0bf10b62363137df0201209e3507cc9ec773e48361cd`
and receipt `a2439f29f5c330076e7cfe5cf0bef38d663d5c2b035b1ee5edecb7cb754b5c61`.
That receipt pins the pre-note RFC bytes52a5c454 and canonical oracle65cb1e08/
verifier6a255fb5; later current campaigns bind actual RFC metadata afresh.
Source9 code 710bc29 passed repository, release and website gates.
Later metadata is separate from that code tree.

## Removal and supersession

Reject/revise if existing precedence, full poststate, allocation/effect/ownership,
diagnostic, partial-domain or permanent gates cannot be retained. Preserve failed
attempts without tuning expected data after observations. Wider ledger invariants,
aggregate recovery or changed public interfaces need a separately accepted scope.
