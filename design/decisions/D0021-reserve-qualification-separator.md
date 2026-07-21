# D0021: Reserve the qualification separator

Status: accepted
Kind: compatibility
Primitive: none
Safety: 0
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 50

## Need

`module/name` must parse to exactly one declaration identity. Allowing `/` in
declared module, item, parameter, field, case, or local names would make the
same token either an atomic name or a qualified reference depending on lookup.

## Alternatives

Escaping, context-sensitive lookup, and multiple separators add syntax and
resolution branches. Keeping slash-names only in standalone compilation would
give source different name semantics inside projects. Reserving `/` globally
creates one rule.

## Costs

Pre-Core-0.2 source that declares a name containing `/` must rename it, normally
to `-`. There is no deprecated alias period. Existing built-ins use `.` and the
repository corpus contains no slash declaration, so the measured migration is
empty.

## Evidence

Governance and conformance must reject slash in every declaration/binding role,
accept exactly one-slash qualified references in project resolution, and prove
that standalone and project parsing assign the same meaning to a token.

## Removal

Replace only with a single qualification encoding that permits a simpler lexer
and resolver. Do not reintroduce contextual slash-names or a second separator.
