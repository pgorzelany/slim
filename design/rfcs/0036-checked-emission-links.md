# RFC-0036: Checked emission links

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

Every ordinary C-emission path now receives the same token stream that passed
checking. Type analysis establishes declaration links on that stream, and the
prepared-project artifact retains them. Code generation nevertheless walked
the complete token stream again to recreate the same declaration links.

`codegen/emit_program` now requires checked tokens and consumes their retained
links. Standalone compilation calls it only after `check/check_source`
succeeds; project compilation and cache misses call it only through a valid
`PreparedProject`. This removes one redundant linear analysis from ordinary
emission without adding a source-language feature or backend dependency.

## Alternatives

Keeping defensive relinking makes the backend usable on unchecked tokens, but
that is not a supported compiler boundary and hides accidental bypasses of the
checked artifact. Passing a second declaration-link table would duplicate
evidence already stored on tokens. Moving all code generation into the type
analyzer would couple otherwise separate responsibilities.

The checked-token precondition is the smallest boundary and reinforces the
single prepared-artifact architecture.

## Costs

`emit_program` is no longer a valid entry point for arbitrary lexed input.
Future callers must check first and retain the same mutable token stream.
Governance and end-to-end tests pin this ordering.

The quick before/after timing series is noisy and does not justify a claimed
latency improvement. At 2,000 generated declarations the medians were 32.279
ms before and 31.987 ms after; smaller samples moved in both directions. The
structural win is exact: one complete relinking walk and its generated C are
gone.

## Evidence

The byte-identical bootstrap fixed point shrank from 1,653,846 to 1,653,599 C
bytes. The hello compiler/runtime smoke test passes, generated output remains
deterministic, and the permanent performance budgets pass. Governance rejects
declaration relinking inside code generation.

## Removal

Restore backend relinking only if an explicitly accepted unchecked-emission API
demonstrates a need worth another whole-token pass and defines how it preserves
the checked-artifact safety boundary. Ordinary compiler paths must continue to
emit from the exact token stream they checked.
