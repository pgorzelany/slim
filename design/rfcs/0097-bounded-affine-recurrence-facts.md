# RFC-0097: Bounded affine recurrence facts

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 1
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

The range analyzer previously retained only exact call-site constants and only
recognized RFC-0094 recurrences whose equality bound was a literal and whose
distance was at most 16. That was enough to expand the neighborhood loop, but
not to prove ordinary loops such as `index == count` where every closed call
supplies an exact `count`. It also discarded useful bounded arguments at
function boundaries and returned needlessly symmetric ranges for nonnegative
division and remainder.

The analyzer may now prove an upward affine recurrence range when the complete
checked function has the RFC-0094 equality shape, all closed calls agree on exact
nonnegative start and bound values inside the integer proof domain, every
iterative recurrence advances the same linked controller by exactly one, and
the base has no recurrence. The iterative body receives `start..bound-1`; the
whole parameter retains `start..bound`. Only distances at most 16 become
RFC-0094/RFC-0096 expansion facts.

Total bounded parameter facts may cross a call boundary during the existing
four fixed propagation passes. Multiple calls join their intervals; missing,
unbounded, non-total, changed recurrence arguments, and facts outside the proof
domain remain unknown. Positive- or negative-divisor quotient ranges use the
four interval corners. Remainder ranges retain the known sign of the dividend.

## Alternatives

Increasing the propagation-pass limit did not improve the Game of Life facts
and would charge every compilation for call depth. Treating `index != bound` as
`index < bound` without a complete recurrence proof is unsound. Retaining only
constants loses ordinary affine loops. A general symbolic solver or
source-level range annotation is unnecessary for the measured cases.

## Costs

The existing fixed four passes and proof domain remain unchanged. Interval
join, quotient, and remainder transfers perform constant work. The recurrence
scan is linear in the function and runs only for the exact equality candidate.
No syntax, dependency, runtime check, ABI, or separately parsed representation
is added.

Tests must cover literal and parameter bounds, bounded cross-call propagation,
multiple-call joins, signed division and remainder, changed bound and
controller arguments, domain saturation, iterative `bound-1`, over-16
non-expansion, and exact report stability.

## Evidence

The Game of Life analysis now proves its large recurrent controller increments
total and derives `0..127` row and column coordinates plus a `0..16383`
neighbor index. A separate bounded-argument fixture changes a callee's checked
addition from unknown to total. A fifth propagation pass produced no additional
facts and was rejected.

These facts alone were runtime-neutral on Game of Life, but they are reusable
target-independent evidence and enable the safety-preserving versioned access
accepted by RFC-0098.

The exact application-analysis baseline changes in eleven rows because positive
bounded facts prove additional arithmetic sites total and discharge only
checked-trap blockers:

- `matrix` reports 4 to 11 total sites and 3 to 2 checked-trap blockers.
- `merge_sort` reports 0 to 1 total site; blockers are unchanged.
- `bytefreq`, `prefix_sum`, and `arena_sum` report 0 to 3 total sites and 4 or
  2 to one fewer checked-trap blocker.
- `binary_search` reports 0 to 4 total sites and 3 to 2 checked-trap blockers.
- `variants` reports 3 to 4 total sites and 2 to 1 checked-trap blockers.
- `floyd_warshall` reports 4 to 12 total sites and 5 to 3 checked-trap blockers.
- `n_queens` reports 2 to 3 total sites and 4 to 3 checked-trap blockers.
- `union_find` reports 0 to 7 total sites and 6 to 4 checked-trap blockers.
- `game_of_life` reports 6 to 26 and `image_convolution` 10 to 30 total sites;
  each changes from 6 or 5 to two fewer checked-trap blockers.

All reason counts, non-trap blocker classes, safe functions, candidates,
selected sites, executable sites, executed sites, and eligible sites remain
exactly unchanged. The stronger facts therefore do not authorize new parallel
execution.

## Removal

Remove a transfer if any supported value can fall outside its reported
interval, if a recurrence can overshoot or change its bound, if analyzer
arithmetic can overflow, if propagation becomes source-depth dependent, or if
the permanent signed and boundary corpus regresses.
