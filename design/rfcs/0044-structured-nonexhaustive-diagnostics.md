# RFC-0044: Structured nonexhaustive diagnostics

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 1
Compile: 0
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 50

## Need

Boolean exhaustiveness checking already returned the first incomplete match as
a token index, but its coordinator immediately converted that token to bytes
and printed `E0336`. Standalone diagnostics were correct; prepared projects
could not project the error back through their token-origin table.

The existing finder now appends the complete match form to the finalized issue
stream. Standalone checking renders that stream, while deferred project checking
retains it for the normal module-local reporter. The finder, code, diagnostic
order, and accepted language are unchanged.

## Alternatives

Teaching the project layer to parse already-rendered flattened diagnostics
would create a second diagnostic protocol and lose token identity. Re-running
exhaustiveness per source module would duplicate checking and could diverge
from the flattened program. Moving all Boolean recovery at once would repeat
the source-shape failure seen in earlier recursive issue-vector experiments.

A single coordinator conversion reuses the interval representation and adds no
pass, syntax, runtime state, or diagnostic form.

## Costs

The nonexhaustive coordinator receives the issue vector and is now responsible
for an internal allocation on invalid input. Invalid standalone input renders
the structured issue before running the existing secondary Boolean recovery;
deferred checking leaves that legacy recovery silent so it cannot emit
unprojected project spans.

Duplicate-arm and secondary recovery diagnostics in a syntactically invalid
Boolean match remain legacy work. Move diagnostics also remain outside the
finalized stream.

## Evidence

The standalone fixtures retain exact `E0336@66:87` and the four-diagnostic
ordering `E0336`, `E0314`, `E0335`, `E0344`. The new prepared-project fixture
pins `E0336@app@56:77`, proving interval projection to the original module.
Self-validation remains about 0.11 seconds.

All 94 conformance fixtures and 2,000 deterministic malformed-input mutations
pass. The byte-identical fixed-point seed is 1,635,539 C bytes. Governance
requires the structured producer and project fixture and rejects restoration of
a direct `E0336` print.

## Removal

Replace this issue only with a more general structured exhaustiveness result
that preserves the code, complete interval, ordering, standalone rendering,
and project projection. Do not restore rendered-text transport between phases.
