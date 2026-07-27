# Core 1F deterministic schedule selection

Date: 2026-07-23
Decision: RFC-0067

## Result

The analysis now distinguishes safety candidates from one executable-shaped
plan. Stable lexical order selects the earliest non-overlapping interval and
advances its exclusion boundary to the selected join. The policy is linear in
the existing token walk and introduces no source syntax or generated-code
change.

The overlap fixture reports four candidates, three selected sites, and three
reported sites. The two candidates inside its three-call chain become one
selected site. A generated 129-candidate chain selects 65 sites, prints the
first 64, and labels the schedule bounded. Repeated reports are byte-identical.

Application baseline schema 4 retains all blocker columns and adds candidate,
selected, and reported counts. All thirteen applications agree with the new
schema; `state_machine` remains the sole application opportunity at `1/1/1`.

Full 1,000/2,000/4,000/8,000-declaration analysis measured
6,752/11,157/20,885/39,934 microseconds. The endpoint exponent is 0.855,
below the durable 1.25 limit.

The portable bootstrap seed is 1,887,280 generated C bytes at SHA-256
`06443f659d007d1c685ce68fe7166cbdc167f6becf2800750a08c5ac87ec5f4e`.

## Boundary

This is still evidence, not execution. Selection does not create tasks and
does not claim profitability. Failure order, task cost, worker overhead, and
the portable execution decision remain separate Core 1F gates.
