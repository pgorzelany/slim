# RFC-0040: Bounded variant member lookup

Status: accepted
Implementation: complete
Process: legacy
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

RFC-0039 made exact variant case definitions part of the checked token artifact but
kept the textual case reader for one bootstrap-compatible generation. Leaving
that reader in place would preserve work proportional to the number of cases
for every constructor and every emitted match arm.

Constructor emission now reads the case link from the constructor form, and
variant-arm emission reads it from the arm form. Each query is independent of
declaration count, case count, and identifier length. The obsolete recursive
case-name scanner is removed.

## Alternatives

Lockstep emission could reuse canonical arm order, but it would couple backend
control flow to a second synchronized cursor and still would not help standalone
constructors. A backend member table would duplicate links already retained by
the checker. Keeping the compatibility scan indefinitely would undermine the
purpose of the producer-first migration.

Direct structural token links are the smallest common query for constructors
and arms and require no new language surface or runtime data.

## Costs

Code generation is intentionally invalid on unchecked tokens. It must consume
the exact checked stream as required by RFC-0036. A defensive fallback preserves
the existing partial-operation failure boundary if an internal caller violates
that contract; it is not a supported textual lookup path.

Record-field consumption remains separately blocked by the measured source-shape
regression in RFC-0038, so this decision claims bounded variant members only.

## Evidence

The producer seed from RFC-0039 builds this consumer without an out-of-band seed
rewrite. `variant-wide`, `variant-local-match`, and `data` all compile and run,
with the sixteen-case fixture printing `42`. Self-validation remains about 0.22
seconds of user CPU time.

The byte-identical fixed-point seed shrank from 1,645,415 to 1,643,001 generated
C bytes. All 92 fixtures and 2,000 deterministic malformed-input mutations pass.
Governance rejects the old case scanner and requires both structural link
consumers.

## Removal

Replace structural links only with another bounded typed-member query that
preserves the wide constructor and exhaustive-match fixtures. Restoring a
textual case scan requires a new accepted decision and geometric evidence.
