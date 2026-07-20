# D0006: Tail recurrence

Status: accepted
Kind: language
Primitive: tail-recur
Safety: 1
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

Programs need repetition without `for`, `while`, iterator, and loop families.

## Alternatives

General calls grow the stack; multiple loop syntaxes duplicate recurrence.

## Costs

The checker must enforce tail position and argument arity.

## Evidence

Tail recurrence becomes a backend jump and expresses input traversal and
algorithm challenges.

## Removal

Remove only if ordinary tail calls receive the same mandatory guarantee with no
analysis or diagnostics loss.
