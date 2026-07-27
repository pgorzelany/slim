# RFC-0018: Qualified project references

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: qualified-reference
Safety: 1
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 60

## Need

Imported declarations need an unambiguous identity that preserves direct
dependency and visibility checks without inserting names into local scope.

## Alternatives

Aliases, selective imports, glob imports, re-exports, and implicit opens each
create additional name-resolution paths. A new expression form would duplicate
the existing positions for function and named-type identifiers. The existing
identifier lexer already accepts `/`, so `module/name` adds one semantic rule
without a desugaring layer.

## Costs

Qualified imported references use more tokens than open namespaces. `/` can no
longer occur freely inside declared names. Renaming a module changes its direct
references, which is appropriate because module identity is part of the public
declaration identity.

## Evidence

Positive and negative fixtures must cover qualified calls and types, missing
direct imports, private exports, self-qualification, transitive access, and
private-type leakage. Resolution and generated output must be independent of
manifest order and scheduling.

## Removal

Replace only with one equally explicit, collision-free reference model that
has lower source and compiler complexity. Never retain aliases or implicit
lookup as compatibility paths.
