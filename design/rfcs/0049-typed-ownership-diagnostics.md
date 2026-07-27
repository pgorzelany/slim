# RFC-0049: Typed ownership diagnostics

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

Aggregate transfer was the final semantic family rendered by a separate
post-type token walk. That walk rediscovered parameter types, tracked moved
names by spelling in a growing vector, could confuse shadowed bindings, and
printed `E0315` and `E0347` before prepared projects could retain them. Its
linear search per transfer also made a flat function with many owned
parameters quadratic.

The typed lexical binding now retains borrowed and moved state. Local token
links carry both their checked type token and declaration token in one bounded
integer encoding; the declaration token links directly to its binding record.
User-call transfers and `bytes.freeze` therefore update the exact binding in
constant time while type inference already visits the expression. Returning an
owned value through an `inout` parameter is checked at the typed function
boundary.

Ownership issues are non-blocking for inference, while type issues remain
blocking. This preserves complete type checking and permits multiple ordered
ownership findings without creating a second event stream or analysis pass.

## Alternatives

A separate ownership event vector, a second ownership module, and recursive
conversion of the legacy scanner were measured and rejected because they
duplicated traversal and created more state to synchronize. Retaining the old
name-token vector would preserve spelling-based shadowing errors and
superlinear lookup. Adding a second field to every syntax token would make the
common token representation larger when the existing signed link already has
a disjoint local-binding domain.

Reporting only the first move error would be smaller but would let one safety
finding suppress later type and ownership diagnostics. Treating an ownership
error as a type failure would have the same defect. The explicit
`blocks_inference` bit records the real distinction without weakening either
error.

## Costs

Each compile-time binding stores two Boolean state fields. Each structured
issue stores one Boolean inference-control field. Local-link construction uses
one multiply and owned-transfer lookup uses one remainder; both operate only in
the compiler and are bounded by the validated token count. Generated programs
and the runtime representation are unchanged.

Move state remains deliberately conservative across match arms, matching the
previous accepted semantics. This decision does not claim path-sensitive
ownership joins, general borrowing, or automatic lifetime inference beyond the
existing affine aggregate and exclusive `inout` rules.

## Evidence

All prior standalone ownership fixtures retain their exact codes and byte
intervals. The prepared-project fixture retains four ordered findings:
`E0315@app@248:254`, `E0347@app@329:335`, `E0315@app@482:488`, and
`E0347@app@576:582`, covering user calls, borrowed transfers,
`bytes.freeze`, and borrowed returns.

The permanent geometric `generated-owned-transfers` gate measures 125, 250,
500, and 1,000 transfers in quick verification. The first accepted run was
4.836, 8.102, 14.275, and 26.535 milliseconds, below the 1.25 exponent budget.
Self-validation remains about 0.10 seconds. The complete release gate passes
97 fixtures, 2,000 deterministic malformed-input mutations, and a
byte-identical fixed point of 1,619,795 generated C bytes with SHA-256
`f4021a1524dcb4dcfaa45aff76d0875b3df0ec9bcc7e55d7f447e10bbd4e5f71`.

## Removal

Replace the packed local link only with a representation that keeps exact
binding identity, constant-time checked type lookup, constant-time transfer
lookup, project-stable diagnostics, and no larger common token cost. Replace
conservative move state only with a path-sensitive ownership analysis that
proves at least the same safety and retains approximately linear scaling. Do
not restore the legacy post-type scanner or direct diagnostic rendering.
