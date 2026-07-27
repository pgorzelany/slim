# RFC-0005: Pattern matching

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: exhaustive-match
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

Programs must inspect variants and Boolean values without separate branching
constructs.

## Alternatives

`if`, `switch`, tag tests, and visitor dispatch duplicate branching semantics.

## Costs

The compiler performs exhaustiveness and payload binding checks.

## Evidence

Match covers every conditional and variant-dispatch need in the bootstrap.

## Removal

Replace only with a single branching construct offering stronger analysis.
