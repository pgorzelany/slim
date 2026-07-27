# RFC-0010: Explicit effects

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: explicit-effects
Safety: 2
Compile: 1
Runtime: 2
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 80

## Need

The compiler must distinguish pure computation from allocation, I/O, and
unproven termination for analysis and automatic parallelization.

## Alternatives

Implicit effects hide costs and make call reordering unsafe.

## Costs

Call checking propagates and validates effect sets.

## Evidence

The bootstrap has concrete pure, allocating, I/O, and recursive functions.

## Removal

An effect can be removed if no accepted operation requires it.

