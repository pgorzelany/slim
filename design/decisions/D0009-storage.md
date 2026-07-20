# D0009: Owned bytes, vectors, and typed arenas

Status: accepted
Kind: language
Primitive: typed-storage
Safety: 2
Compile: 1
Runtime: 2
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 80

## Need

The self-hosted compiler needs owned input, ordered collections, stable syntax
identities, and phase-scoped allocation.

## Alternatives

Raw pointers are unsafe; tracing GC violates runtime goals; per-node ownership
creates complex recursive lifetimes.

## Costs

Parameterized storage types require checked operations and drop insertion.

## Evidence

The compiler can store all graph-like structures as arena values plus typed IDs.

## Removal

Remove a storage primitive if ordinary composition provides identical safety,
layout, lifetime, and performance behavior.

