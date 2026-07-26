# Records, variants, and patterns

Records model named products, variants model closed alternatives, and
exhaustive patterns expose their structure without unchecked casts.

## Record declarations and construction

Record fields have a canonical declared order. `make` supplies every field in
that order and `get` projects a known field. `set` visibly rebinds a uniquely
accessible local value; SLIM has no separate field-assignment form.

<!-- slim-fixture: record-wide -->

Missing, duplicate, unknown, or out-of-order fields are rejected.

<!-- slim-fixture: field-order -->

## Variant declarations and cases

A variant declares a closed, canonically ordered set of cases. `case`
constructs one alternative with its exact payload shape.

<!-- slim-fixture: variant-wide -->

There is no open extension mechanism or runtime string tag.

## Pattern matching

`match` examines a Boolean or variant. Variant arms occur in declaration order,
bind payload values, agree on one result type, and cover every case.

<!-- slim-fixture: computed-variant-match -->

An unknown case, duplicate arm, wrong payload arity, wrong payload type, or
nonexhaustive match is diagnosed statically.

<!-- slim-fixture: nonexhaustive-variant -->

## Ownership of aggregates

Records and variants containing affine storage are themselves affine. Moving
the aggregate invalidates its previous binding. Pattern payloads follow the
same ownership and lifetime rules as ordinary bindings.

## Compiler guarantees

Aggregate layout and generated C are deterministic. Field and case ordering are
semantic canonicalization rules, not presentation preferences. Pattern
checking never inserts a hidden default.

## Deliberate omissions

SLIM has no structural subtyping, inheritance, reflection, dynamic field
lookup, open variants, implicit destructuring, or unchecked downcasts.
