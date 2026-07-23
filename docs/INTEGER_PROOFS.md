# Integer proofs

Status: Core 1F bounded totality evidence

SLIM analysis derives integer facts from the normal checked token and type
view. These facts are evidence for quality and parallelism; they do not alter
source semantics or generated C.

## Abstract domain

Each analyzed expression records whether evaluation is proven total and may
record a lower bound, an upper bound, or both. The proof domain is deliberately
limited to `-1,000,000,000..1,000,000,000`. A bound outside that domain becomes
unknown. This makes all endpoint calculations safe inside the compiler and
keeps the implementation independent of a solver.

The first transfer set understands:

- exact integer literals and immutable `let` flow;
- `i64.add` and `i64.sub` when an exact constant and the required one-sided
  bound prove safety;
- `i64.mul` when both operands and the result are bounded;
- `i64.div` and `i64.rem` when bounded divisors exclude zero;
- `i64.to-u8` when the input is proven inside `0..255`; and
- one-sided refinements from direct `i64.lt`, `i64.le`, `i64.gt`, and `i64.ge`
  comparisons against an exact constant in Boolean match arms.

Unknown values, mutation, collection indices, recurrence, user-call return
ranges, and values outside the proof domain remain unknown. Unknown is never
permission to remove a check or reorder a trap.

## Bounded reports

`slimc analyze SOURCE_OR_PROJECT` emits an `integer-proofs` section. It records
the fixed domain, the 64-refinement limit, whether that limit saturated, and up
to 64 checked-operation sites with their node identity, totality status, and
known bounds. The checked-site count covers the whole input even when printed
facts are bounded.

Quality reports a function as total only when its body fact is positive and it
has no recurrence. Parallelism ignores a checked-trap hazard only when the
fact for that exact call node is positive; call-graph, effect, ownership,
mutation, recurrence, race, and deadlock rules still apply independently.

No ordinary check, C emission, or generated program pays for this analysis.
Only the explicit analysis command constructs the interval view.
