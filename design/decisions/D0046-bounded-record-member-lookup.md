# D0046: Bounded record member lookup

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

D0038 made exact record-field definitions part of the checked token artifact,
but its first consumer attempts crossed the compiler's source-shape performance
guard. The compatibility backend therefore still scanned every declared field
by name for every supplied field, doing work proportional to supplied fields
times declared fields.

Record binding emission now reads the definition link from the supplied field
form. A small non-recursive verifier checks that the link is within the record's
field region and that the linked declaration has the expected source span. The
recursive emitter keeps its established scalar proof shape and replaces only
the lookup expression. The old textual scanner is removed.

## Alternatives

Lockstep traversal would depend on canonical source order and add another
synchronized recursive cursor. A backend member table would duplicate the
checker-owned links. Retaining the compatibility scan would leave the final
aggregate member query unbounded.

Larger extractions were measured rather than accepted on architectural taste.
Calling an otherwise-valid record-field leaf through a new recursive wrapper
made self-validation pathological, and removing three scalar proof bindings
from the established recursive emitter also crossed the guard. Replacing the
lookup in place retains bounded behavior without growing the language or
creating a second semantic authority.

## Costs

Code generation remains intentionally defined only over checked tokens. A bad
or mismatched structural link reaches the existing internal partial-operation
failure boundary; there is no textual recovery path in the backend.

The span equality check is constant in the number of record fields but linear
in the field identifier's byte length. This is the same token-identity boundary
used elsewhere and avoids trusting an unchecked index blindly.

## Evidence

`record-wide` constructs sixteen ordered fields, projects the last, and prints
`42`. Self-validation remains about 0.11 seconds of user CPU time after the old
scanner is removed. The complete conformance and malformed-input corpus, quick
performance budgets, sanitizers, allocation-failure tests, and byte-identical
bootstrap pass at a fixed-point seed of 1,635,270 generated C bytes.

The rejected recursive-leaf shape accumulated more than 94 seconds of CPU time
before interruption; removing the scalar proof bindings exceeded eight seconds.
The accepted in-place consumer remained at approximately 0.11 seconds.
Governance requires the structural consumer and rejects restoration of the
textual scanner.

## Removal

Replace structural field links only with another bounded checked-member query
that preserves the wide record fixture and the checked-emission precondition.
Restoring a textual field scan requires a new accepted decision and geometric
evidence.
