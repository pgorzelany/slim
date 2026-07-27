# RFC-0047: Retained allocation boundaries

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

RFC-0043 made the memory plan authoritative for function-region placement, but
function emission still rescanned the declared effect list to decide whether
to emit its allocation-failure boundary. The retained `AllocationPlan` also
recognized only direct `vec.new` and `arena.new` calls even though vector
growth, arena insertion, file reads, and allocation-capable user calls can all
transfer allocation failure to the caller.

The plan now records every call site that requires the `alloc` effect. Function
emission asks the retained site vector whether a failure boundary is needed;
it no longer reparses the function declaration. Built-in and user-call effect
classification is owned by one small `effects` module shared by diagnostics,
planning, and the remaining per-call backend check.

## Alternatives

Adding allocation and recurrence fields to `FunctionPlan` was measured first,
but the candidate made self-validation exceed eight seconds. Replacing an
existing Boolean with a packed integer summary still exceeded six seconds.
Neither source shape is acceptable at the 0.11-second boundary.

Keeping the backend declaration scan would preserve two authorities. Encoding
the fact as a token sentinel would overload structural links. Reimplementing
built-in effect tables in the planner would create a third semantic definition
that could drift from diagnostics.

The existing allocation-site vector already has the correct ownership and
lifetime: it is produced once during checked planning, retained through project
preparation, and consumed only by emission.

## Costs

The self-hosted compiler gains one internal SLIM module with four effect
queries. Effect codes remain an internal closed convention; no source syntax,
runtime ABI, dependency, or executable representation is added.

An allocation-capable user call is represented as a planned boundary even if a
particular execution does not allocate. This is conservative and does not add
runtime work beyond the failure check already emitted. Per-call emission still
uses the canonical effect query to decide where that check belongs; threading
site cursors through the complete expression emitter remains later typed-view
work and is not claimed here.

## Evidence

The `allocation-user-failure` fixture injects failure inside a user function
whose caller has no direct allocation and requires the normal status-71 report.
This pins both transitive site planning and the generated caller boundary.
Existing storage, lifetime, project, and deterministic emission fixtures remain
green. Self-validation remains about 0.11 seconds.

The complete gate passes 95 fixtures, 2,000 deterministic malformed-input
mutations, performance budgets, sanitizers, allocation-failure injection, and
a byte-identical fixed point of 1,631,733 generated C bytes. Governance
requires the canonical effect consumers and retained-plan query and rejects the
superseded backend and planner scans.

## Removal

Replace the allocation-site vector only with another checked plan query that
retains every failure-propagating call and preserves injected user-call
failure. Do not restore declaration reparsing, duplicated built-in effect
tables, or overloaded token links.
