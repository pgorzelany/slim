# D0008: Bootstrap scalar types

Status: accepted
Kind: language
Primitive: scalar-types
Safety: 2
Compile: 2
Runtime: 2
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 90

## Need

Booleans, bytes, sizes, signed arithmetic, and unit results are required for
parsing, checking, native process results, and challenges.

## Alternatives

One arbitrary-precision number adds allocation and unpredictable runtime cost.

## Costs

Each scalar requires explicit operation signatures and overflow semantics.

## Evidence

The selected fixed-width set covers the bootstrap without implicit conversions.

## Removal

Individual scalars may be removed if the compiler and corpus demonstrate no
distinct semantic or layout need.
