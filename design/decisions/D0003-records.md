# D0003: Product records

Status: accepted
Kind: language
Primitive: product-record
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

Compiler tokens, spans, syntax nodes, and diagnostics require named product data.

## Alternatives

Tuples and classes duplicate product representation or add unrelated behavior.

## Costs

Records add layout, construction, and projection rules.

## Evidence

One named-field representation covers compiler data and challenge structures.

## Removal

Remove only if another single product representation proves strictly smaller.
