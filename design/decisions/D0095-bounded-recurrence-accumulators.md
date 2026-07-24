# D0095: Bounded recurrence accumulators

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

D0094 recovers an exact small counted recurrence and proves its controller
updates total, but counted control flow alone is nearly neutral on the native
corpus. A carried accumulator can still use checked arithmetic on every
iteration, preventing the native backend from unrolling the otherwise exact
loop. Game of Life carries a neighbor total initialized to zero and adds only a
Boolean-derived value in `0..1` across at most nine iterations.

For a D0094 recurrence only, the range view may promote an I64 parameter to a
bounded accumulator when all complete closed-program entry calls agree on one
exact nonnegative start and every recurrence path either passes that same
linked parameter unchanged or adds a checked, total, nonnegative bounded delta
to it. At least one path must change it. The compiler computes
`start + iterations * maximum_delta` inside the existing proof domain and
rejects saturation, negative deltas, unsupported argument shapes, conflicting
entry values, more than 16 parameters, or a result outside the declared
integer-analysis domain.

The resulting range is attached to the stable parameter declaration. The
ordinary range analyzer is replayed over the iterative body, so a checked add
loses its trap only when the exact call node becomes total from that positive
fact. No proof is inferred from a parameter name or from iteration direction.

## Alternatives

Removing overflow checks in optimized builds is rejected. Treating an
accumulator as bounded merely because the loop count is bounded ignores its
update values. A general recurrence solver or unbounded fixed point would
violate predictable compiler work. Source contracts, accumulator annotations,
and wrapping arithmetic aliases are unnecessary for the demonstrated shape.

## Costs

The analysis performs at most 16 linear scans of the already bounded iterative
body for a positively identified D0094 loop. This is a fixed-factor linear
cost, not a source-size-dependent fixed point. It adds no syntax, dependency,
runtime branch, allocation policy, ABI, or executable representation.

Tests must cross unchanged and changed paths, negative and unknown deltas,
conflicting starts, overflow-domain saturation, parameter and iteration
budgets, exact-node arithmetic lowering, and unsupported recurrence arguments.
Compiler geometric scaling and the complete trap corpus remain mandatory.

## Evidence

The D0094 counted-control candidate preserves output and vector bounds checks
but leaves Game of Life near 19.7 ms. A prior diagnostic C experiment combining
the counted shape with positive controller and accumulator facts measured about
6.7 ms, versus about 5.9 ms for matched C. The retained production chain,
including bounded expansion and versioned collection access, measures 4.715 ms
for SLIM versus 7.059 ms for C and 5.043 ms for Rust in the full comparison.
All unsupported arithmetic remains checked.

## Removal

Remove the fact if any recurrence path is skipped, a delta can be negative or
unknown, the analyzer's own arithmetic can leave its proof domain, a promoted
node can still trap on a supported execution, compiler scaling regresses, or
the permanent native corpus shows no material benefit. Retain the negative and
boundary fixtures even if the lowering consumer changes.
