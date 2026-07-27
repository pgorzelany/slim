# RFC-0089: Bounded recurrence induction

Status: rejected
Implementation: not-planned
Process: legacy
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

Most SLIM loops are tail recurrences with an I64 controller, an exact equality
guard, and a positive literal increment. Even when every closed-program call
starts at the same exact value and uses the same exact limit, ordinary
parameter propagation rejects the controller because recurrence changes it.
The range view therefore leaves the increment and dependent arithmetic checked
inside every iteration.

The analyzer may derive a bounded controller fact only for one complete
structural shape: one tail `recur`, an exact `i64.eq` guard between two linked
parameters, an `i64.add` update of that same controller by a positive literal,
an unchanged linked limit argument, exact agreeing closed-program start and
limit values, `start <= limit`, and a distance exactly divisible by the step.
The resulting controller range is `start..limit`.

## Alternatives

Treating a changed parameter as its initial constant is unsound. Inferring
direction without divisibility can skip over the equality guard and eventually
overflow. Assuming that a named `index` or `count` has loop meaning would make
optimization depend on spelling. A general solver or unbounded fixed point
would violate the compiler budget.

Source loop annotations and contracts are not justified while this canonical
shape can be proved from checked code.

## Costs

Each of the existing four parameter passes performs one bounded token scan and
one linear declaration scan. The proof uses stable linked token identities and
the existing `ranges` fact vector; it adds no executable representation.
Unsupported guards, updates, call disagreement, recurrence count, or arithmetic
remain unknown.

Positive and non-applicability tests cross equality direction, divisibility,
recurrence count, closed-call agreement, and the fixed propagation boundary.
The analysis and emission scaling gates, exact trap fixtures, bootstrap fixed
point, parallel blocker ledger, sanitizers, and full native corpus remain
mandatory.

## Evidence

The RFC-0087 full result is the baseline: 1.273 geometric mean against C and 1.337
on serial work. Acceptance requires a material same-host improvement without
removing any collection bound or unsupported arithmetic trap.

The candidate reached the self-hosted fixed point and did prove the intended
loop increments, but the 2026-07-24 full corpus measured 1.286 against C and
1.349 on serial work. The matching quick run was also neutral within noise:
1.260 overall and 1.324 serial, against 1.263 and 1.320 for RFC-0087. No durable
same-host improvement justified the added analyzer surface, so the production
implementation and its provisional tests were removed.

## Removal

Remove the induction fact if any accepted controller can leave the stated
range, skip its equality limit, arise from a second recurrence, or depend on an
unknown call. Retain the safety fixtures and tightened performance budgets.
