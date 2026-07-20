# D0007: Unique mutation

Status: accepted
Kind: language
Primitive: unique-mutation
Safety: 2
Compile: 1
Runtime: 2
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 80

## Need

Compiler arenas, vectors, and performance kernels require in-place updates.

## Alternatives

Persistent data alone adds allocation; unrestricted references add aliasing.

## Costs

The checker tracks lexical uniqueness and moved values.

## Evidence

A single explicit mutation operation supports compiler construction without
general pointer semantics.

## Removal

Remove if ownership-aware library calls can provide identical diagnostics and
performance without a language form.
