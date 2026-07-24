# D0099: Bulk copy recognition

Status: rejected
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 0
Minimal: 1
Analysis: 1
Dogfood: 1
Score: 50

## Need

Several native applications contain the canonical recurrence shape
`destination[index] = source[index]`. A compiler-owned scalar-storage proof
could replace a complete non-overlapping interval with a guarded `memcpy`,
falling back to the original checked recurrence when the start, length, vector
sizes, element storage, or alias facts are insufficient.

## Alternatives

An unchecked copy primitive would weaken safety and expand the language
surface. An unconditional `memcpy` would mishandle short vectors, overlapping
storage, and nontrivial owned elements. Retaining the recurrence leaves these
semantics explicit and lets the backend recognize profitable scalar cases.

## Costs

A production recognizer would require exact interval, storage, alias, overlap,
destruction, and fallback facts plus permanent semantic and performance tests.
Although each check could be bounded and linear, that maintenance cost is not
justified without a measured runtime gain.

## Evidence

The existing Game of Life `copy-grid` recurrence was replaced diagnostically
with a length-guarded `memcpy`; the original checked loop remained as the
fallback. The source and destination are exclusive vector borrows, the element
is `Bool`, the exact interval is `0..16384`, and output remained identical.

One hundred same-host process runs used about 0.36 seconds of user CPU both
before and after the replacement. Inspection of optimized native output showed
that the C compiler already vectorizes the element loop. The intrinsic added no
material runtime benefit.

## Decision

Do not add a bulk-copy analyzer, lowering path, runtime primitive, syntax, or
tests solely for this shape. The proof and maintenance cost does not currently
carry its weight. Keep the existing recurrence and allow native optimization
to recover the copy.

Reconsider only with at least two existing applications where a
safety-preserving guarded bulk operation materially improves same-host runtime,
or when a native backend no longer performs equivalent loop recognition. A
future proposal must also cover overlap semantics, owned element destruction,
allocation failure, zero length, partial ranges, and fallback traps.

## Removal

This decision adds no production capability to remove. Remove the negative
record only if a later accepted decision supersedes it with positive
multi-application evidence and preserves the experiment history.
