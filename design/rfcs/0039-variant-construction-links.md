# RFC-0039: Variant construction links

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

Variant construction and matching still scanned all declarations to recover a
variant whose type token was already linked. They also searched cases by text
after the checker had validated exact canonical case order and payload types.

Both declaration scans are removed. Constructor variant tokens and decoded
match type tokens now resolve through retained declaration links. The checker
also stores the validated case definition on each constructor or variant-arm
opening token, whose role does not overlap atom declaration or binding links.

## Alternatives

Keeping textual lookup would duplicate type checking and make compilation
depend on declaration and case counts. A new member table would allocate and
populate another index even though ordered checking already has each definition
in hand. Encoding the link on case-name atoms risks colliding with global name
links; structural opening tokens are unambiguous and otherwise unused.

Publishing case links before consuming them is a deliberate bootstrap schema
migration. The producer checkpoint retains the old case-name reader for one
self-hosted generation, after which a separately verified decision may remove
it.

## Costs

Checked token links are now a role-tagged internal artifact rather than only a
name index. Backends must receive the exact successfully checked token stream,
as already required by RFC-0036. Boolean match arms retain their unlinked opening
tokens because they have no variant case definition.

This checkpoint does not yet claim bounded case lookup in code generation; it
establishes the producer contract and removes only whole-declaration scans.

## Evidence

The `variant-wide` executable fixture constructs the last case of a sixteen-case
variant, matches all cases in canonical order, binds its payload, and prints
`42`. The existing `data` and `variant-local-match` fixtures remain green.

Self-validation remains about 0.21 seconds of user CPU time. The byte-identical
bootstrap seed shrank from 1,647,978 to 1,645,415 generated C bytes. The next
consumer stage can be built by this seed without an out-of-band bootstrap
artifact. All 92 fixtures and 2,000 deterministic malformed-input mutations
pass. Governance requires both structural case-link producers, both
variant declaration-link consumers, the wide fixture, and absence of the old
whole-declaration helper.

## Removal

Replace these links only with a bounded typed aggregate query that preserves
constructor, local-match, and wide-variant evidence. Restoring declaration or
case name scans requires a new accepted decision with scaling evidence.
