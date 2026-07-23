# D0064: Durable parallelism application baseline

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 60

## Need

D0062 and D0063 establish bounded safety facts on focused fixtures, but focused
fixtures cannot justify runtime scheduling or the next precision feature. The
repository already has twelve matched algorithm applications used for native
performance. Their analysis classifications must become durable evidence too.

The accepted `parallelism` benchmark runs analysis twice for every challenge,
requires byte-identical balanced version-4 reports, extracts one fixed metric
row, and compares it with a checked-in baseline. The full command is part of
release verification. Any compiler or application change that alters a row
must explain the change and update the baseline intentionally.

## Alternatives

A one-time result document would immediately become stale. Counting only fork
sites would hide whether safety precision improves while application shape
still prevents a candidate. Adding new synthetic parallel examples would
repeat the fixture bias that this decision is intended to correct. Running an
external static analyzer would duplicate SLIM semantics in Rust.

The harness therefore measures reports produced by the SLIM compiler itself.
Rust remains measurement and verification infrastructure only.

## Costs

The initial baseline had nineteen explicit columns: source size, function
count, checked-site count, reported total sites, refinements, safe functions,
eleven stable primary-reason counts, and eligible sites. D0065 advances it to
schema 2 with eleven complete-blocker columns. Exact source size makes
application edits visible. Exact classifications make proof changes visible.
Improvements are expected to require baseline updates; silent drift is not.

The reason counts are the compiler's current primary reasons, not a complete
set of coexisting blockers. D0064 therefore cannot by itself justify selecting
the largest primary count as the next feature. Complete multi-blocker evidence
or a controlled application experiment is required first.

## Evidence

The initial twelve applications contain 16,521 source bytes, 49 functions, and
140 checked integer sites. One reported site is proved total. No function and
no fork site is eligible. Primary reasons are 30 exclusive borrows, ten
allocation/I/O functions, five checked traps, and four recurrences. All other
reason counts are zero.

Acceptance requires exact baseline agreement, deterministic balanced reports,
manifest/baseline set equality, a release-verification invocation, governance
coverage, and the complete existing bootstrap, conformance, mutation,
sanitizer, allocation-failure, and performance gates.

## Removal

Do not remove or weaken the baseline because a deliberate analysis improvement
changes it. Update the row and dated evidence with the responsible decision.
Remove a challenge only if the native benchmark itself is retired through an
equally explicit decision. Never interpret a primary reason count as proof
that fixing that reason alone makes a function safe.
