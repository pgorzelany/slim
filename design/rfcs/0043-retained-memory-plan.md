# RFC-0043: Retained memory plan

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

Checking already constructed the canonical memory plan for every valid program,
but discarded it before returning `typing/Checked`. C emission then inferred
function-region placement again from source tokens. The duplicate query was
small, but it created two authorities for a safety-relevant backend decision
and prevented later liveness and destruction facts from crossing the checked
boundary at all.

`typing/Checked` now owns the memory plan produced after type linking.
`PreparedProject` retains the same plan, and `codegen/emit_program` requires it.
Functions and plans are consumed in their shared declaration order; emission
checks the stored function token before using its region decision.

## Alternatives

Recomputing the complete plan inside code generation would repeat the most
expensive lifetime walks and still leave checking and generation with separate
objects. Storing only `local_region` on a syntax token would overload the link
encoding, discard the rest of the plan, and make future destruction placement
depend on another side channel. A second backend-specific summary would
duplicate `FunctionPlan`.

One retained plan adds no executable representation or source feature and
makes the existing analysis the only authority.

## Costs

The checked and prepared artifacts each gain one aggregate field. `typing`,
`project`, and the compiler driver now depend on the memory-plan type, and the
internal emission API no longer accepts merely linked tokens. Invalid checked
artifacts carry an empty plan and can never reach emission.

The ordered consumer relies on the planner and declaration traversal selecting
functions identically. Each plan stores its originating function token, and
emission traps on an internal mismatch instead of silently choosing a wrong
region. Allocation-effect and recurrence queries remain token-derived and are
explicit later typed-view work.

## Evidence

Standalone and project compilation produce the same deterministic C and native
behavior through the retained plan. The self-host project continues to check
in about 0.11 seconds. The permanent quick series measured 250, 500, 1,000, and
2,000 declaration emissions in 8,735, 11,008, 21,777, and 41,141 microseconds;
the emit/check ratio at 2,000 declarations was 1.410, below the 4.0 limit.

All 93 conformance fixtures and 2,000 deterministic malformed-input mutations
pass, including project emission, allocation failure, lifetime, record, and
variant programs. The byte-identical fixed-point seed is 1,635,534 C bytes.
Governance requires the retained plan, ordered identity check, and plan-driven
region read, and rejects restoration of the backend region query.

## Removal

Replace the retained plan only with a smaller checked artifact that remains the
single authority for every safety-relevant storage and destruction decision.
Restoring backend lifetime inference or allowing unchecked tokens into ordinary
emission requires a new accepted decision and equivalent fixed-point, scaling,
allocation-failure, and sanitizer evidence.
