# D0013: Freeze byte vectors

Status: accepted
Kind: language
Primitive: byte-freeze
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

The self-hosted compiler must construct generated output incrementally and pass
the finished bytes to the existing I/O operations without copying.

## Alternatives

A second mutable byte-string type duplicates vectors; implicit conversion hides
ownership transfer and possible cost; printing one byte at a time is slow.

## Costs

The checker recognizes one representation transition from `(Vec U8)` to
`Bytes`, and the runtime validates the element layout.

## Evidence

`bytes.freeze` consumes unique vector ownership and returns an immutable view in
constant time with no allocation or copy.

## Removal

Remove if owned vectors and immutable bytes adopt one representation and the
operation becomes a type-only move with identical diagnostics.

