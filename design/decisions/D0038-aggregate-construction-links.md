# D0038: Aggregate construction links

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

Record construction passed type checking with a declaration and a canonical
field sequence already known, but C emission rediscovered the record by scanning
every declaration and rediscovered each field by scanning its declaration list.
That duplicated checked work and made the backend depend on textual lookup.

The record type token already carries its declaration link, so emission now
uses that link directly. During the canonical lockstep field check, the checker
also records each validated field-definition token on the supplied field form's
otherwise-unused opening token. This publishes the remaining fact without a
new table, syntax form, or runtime representation.

## Alternatives

A backend hash table would repeat name resolution and add allocation and setup
cost to every emission. Walking record definitions in lockstep would be linear
and uses D0032's canonical ordering, but the tested recursive source shape made
the current compiler take more than five seconds to validate itself instead of
about 0.22 seconds. Passing another typed-view vector through the recursive
backend has already shown the same class of source-shape cliff.

Retaining the fact on the checked token stream is the smallest producer-side
contract. The existing field-name reader remains as a temporary compatibility
path until consuming the retained link can pass the permanent scaling gate.

## Costs

The meaning of a token link now depends on token role: declaration-name atoms
link to declarations, bound-value atoms use the existing encoded type link, and
validated record-field opening tokens link to field definitions. These roles
are disjoint and are available only after successful checking.

This decision does not claim bounded field lookup in code generation yet. It
removes the whole-declaration scan for every record construction and establishes
the checked field-link artifact, while deliberately retaining the old field
scan rather than accepting a large compiler regression.

## Evidence

The `record-wide` executable fixture constructs a canonical sixteen-field
record, projects the final field, and prints `42`. The compiler self-check stays
at about 0.21 seconds of user CPU time. The two attempted direct consumers—an
ordered lockstep walk and a token-link reader—both crossed the five-second
early-abort boundary and were reverted.

The byte-identical bootstrap seed shrank from 1,650,892 to 1,647,978 C bytes.
All 91 fixtures and 2,000 deterministic malformed-input mutations pass.
Governance pins the record declaration-link consumer, the field-link producer,
and the adversarial fixture.

## Removal

Remove the field-form annotation only when a bounded typed aggregate query
replaces it and the backend no longer scans field names. Restoring record
declaration scans requires a new accepted decision and evidence that retained
checked links cannot satisfy the supported emission boundary.
