# RFC-0003: Product records

Status: accepted
Implementation: complete
Process: legacy
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

Records add layout, construction, and projection rules. RFC-0032 requires every
construction to name each field exactly once in declaration order, avoiding
equivalent source orderings and repeated member lookup.

## Evidence

One named-field representation covers compiler data and challenge structures.
The self-host compiler validates construction with a paired linear walk and
pins missing, duplicate, unknown, out-of-order, and mistyped fields in the
external conformance corpus.

## Removal

Remove only if another single product representation proves strictly smaller.
