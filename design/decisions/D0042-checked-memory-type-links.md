# D0042: Checked memory type links

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

The memory planner classifies named record and variant storage recursively. For
every named type it scanned every declaration by text, even though type analysis
had already validated and linked the type token. Standalone checking also built
the memory plan before those links existed, preventing reuse by construction.

Standalone checking now builds the typed view before the memory plan. Named
storage classification reads the declaration link retained on the checked type
token and the recursive textual declaration scanner is removed. Code generation
already receives checked tokens under D0036, so its storage queries use the same
bounded path.

## Alternatives

A planner-local type table would duplicate declaration indexing and allocate on
every plan. Retaining the scan would multiply declaration work by named fields,
payloads, parameters, and locals. Moving storage classification into the type
checker would couple layout policy to expression validation and require a larger
typed-view schema change.

Reordering two existing pure analyses and consuming the retained link removes
the duplicate lookup without new state or language surface.

## Costs

`memory/analyze` now requires declaration links and is not a supported operation
on merely lexed tokens. Its only standalone caller establishes those links first,
and backend callers already satisfy D0036's checked-token precondition. An
unresolved internal link conservatively retains owned storage classification.

This does not yet make the complete memory plan part of `typing/Checked`; it
only removes named-type rediscovery and aligns phase order.

## Evidence

The self-host project, `lifetimes`, `record-wide`, and `variant-wide` all check,
compile, and execute through the linked classifier. Nine warm self-checks had a
0.11-second median user time and 0.12-second median wall time, versus the prior
approximately 0.22-second self-check boundary: about a twofold improvement.
The permanent named-type parameter series checks 125, 250, 500, and 1,000
late-declared type references in 4,701, 6,099, 7,551, and 8,239 microseconds,
for a 0.270 endpoint scaling exponent under the 1.25 limit.

The byte-identical fixed-point seed shrank from 1,634,840 to 1,631,678 generated
C bytes. The full release gate includes allocation-failure, sanitizer, aggregate,
and bootstrap evidence. Governance requires typed-before-memory order, requires
the linked query, and rejects restoration of the declaration scanner.

## Removal

Replace token links only with another bounded checked type query that preserves
recursive storage classification and the lifetime and aggregate fixtures.
Restoring textual declaration lookup requires a new accepted decision and
geometric named-type scaling evidence.
