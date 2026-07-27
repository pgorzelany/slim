# RFC-0001: Canonical source

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: module
Safety: 1
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

AI tools and the bootstrap compiler need one unambiguous, recoverable source
representation with no normalization ambiguity.

## Alternatives

Brace syntax, indentation syntax, and multiple surface/Core representations add
grammar and migration cost.

## Costs

Typed S-expressions are less familiar to humans and use explicit delimiters.

## Evidence

Every parsed form maps directly to one AST node and canonical formatting can be
idempotent.

## Removal

Replace the format through a versioned migration only if measured token and
compiler improvements justify it; never accept both formats concurrently.
