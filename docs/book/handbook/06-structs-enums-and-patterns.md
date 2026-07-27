# Structs, enums, and patterns

Structs model named products, enums model closed alternatives, and
exhaustive patterns expose their structure without unchecked casts.

## Canonical data example

<!-- slim-fixture: example-data -->

The program constructs a `Pair`, projects both fields, wraps the result in an
enum, and extracts the payload through an exhaustive match.

## Struct declarations and construction

Struct fields have a canonical declared order. `Pair(left: 1, right: 2)`
supplies every field in that order and `pair.left` projects a known field.
Ordinary assignment visibly rebinds a `var`; SLIM has no field assignment.

<!-- slim-fixture: record-wide -->

Missing, duplicate, unknown, or out-of-order fields are rejected.

<!-- slim-fixture: field-order -->

## Enum declarations and cases

An enum declares a closed, canonically ordered set of cases.
`Maybe::Some(value)` constructs one alternative with its exact payload shape.

<!-- slim-fixture: variant-wide -->

There is no open extension mechanism or runtime string tag.

## Pattern matching

`match` examines an enum. Enum arms occur in declaration order,
bind payload values, agree on one result type, and cover every case.

<!-- slim-fixture: computed-variant-match -->

An unknown case, duplicate arm, wrong payload arity, wrong payload type, or
nonexhaustive match is diagnosed statically.

<!-- slim-fixture: nonexhaustive-variant -->

## Ownership of aggregates

Structs and enums containing affine storage are themselves affine. Moving
the aggregate invalidates its previous binding. Pattern payloads follow the
same ownership and lifetime rules as ordinary bindings.

## Exact rules and common errors

Aggregate layout and generated C are deterministic. Field and case ordering are
semantic canonicalization rules, not presentation preferences. Pattern
checking never inserts a hidden default. Missing, duplicate, unknown, or
out-of-order fields and cases are rejected.

SLIM has no structural subtyping, inheritance, reflection, dynamic field
lookup, open enums, implicit destructuring, or unchecked downcasts.

## Next

Continue with [ownership, borrowing, and memory](07-ownership-borrowing-and-memory.md).
