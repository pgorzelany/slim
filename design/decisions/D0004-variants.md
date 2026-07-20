# D0004: Tagged variants

Status: accepted
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

Variants require tag layout and exhaustiveness metadata.

## Evidence

The same construct represents compiler trees, errors, optional values, and
state machines.

## Removal

Remove only in favor of a smaller closed-sum representation.
