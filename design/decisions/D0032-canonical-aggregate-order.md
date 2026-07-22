# D0032: Canonical aggregate order

Status: accepted
Kind: compatibility
Primitive: none
Safety: 1
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

Named record fields and variant cases make data intent explicit, but allowing
their construction or match clauses in arbitrary order creates many source
spellings for the same operation. It also turns a simple paired validation walk
into repeated member-name searches unless the compiler builds another index.

SLIM has one canonical rule: record construction fields appear exactly once in
record declaration order, and variant match arms appear exactly once in variant
declaration order. Boolean matches contain `true` and `false` exactly once;
their order remains semantically irrelevant because `Bool` is a primitive
rather than a user declaration with a source order.

## Alternatives

Arbitrary order plus duplicate detection is familiar in human-oriented
languages but adds equivalent spellings and name-index work. Positional fields
would save field-name tokens while making generated code and diagnostics less
self-describing. Automatically reordering accepted input during formatting
would make formatting depend on successful cross-declaration resolution.

The declared-order rule retains names at every use while giving agents and the
compiler one representation to produce, inspect, compare, and reduce.

## Costs

Reordering a record or variant declaration requires corresponding uses to be
updated. Generated patches must know declaration order. Boolean matches keep a
small special rule instead of acquiring a synthetic declaration order.

## Evidence

The self-host compiler validates record fields and variant arms with paired
linear walks. Dedicated fixtures reject missing, duplicate, unknown, and
out-of-order fields or cases, wrong payload arity and type, and incomplete
matches. The complete compiler and challenge corpus already follows the
canonical order and passes the byte-identical bootstrap proof.

## Removal

Relax ordering only if measured agent evidence shows a material reliability
gain and a replacement still provides one canonical emitted form with bounded
checking. Remove field names only if a smaller representation preserves
readability, diagnostics, safe refactoring, and self-host utility.
