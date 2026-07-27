# RFC-0004: Tagged variants

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: tagged-variant
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

Tokens, syntax, results, and finite state require closed alternatives with
payloads.

## Alternatives

Nulls, exceptions, unions, and inheritance make valid alternatives less
explicit or less safe.

## Costs

Variants require tag layout and exhaustiveness metadata. RFC-0032 requires match
arms exactly once in declaration order, making exhaustiveness and payload
validation a paired linear walk.

## Evidence

The same construct represents compiler trees, errors, optional values, and
state machines. External fixtures pin unknown, duplicate, out-of-order,
non-exhaustive, wrong-arity, and mistyped cases.

## Removal

Remove only in favor of a smaller closed-sum representation.
