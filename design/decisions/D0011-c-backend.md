# D0011: Permanent C11 reference backend

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 0
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

SLIM needs an auditable portable bootstrap path and native execution before a
direct backend exists.

## Alternatives

LLVM adds a large dependency; a direct ARM backend is not initially portable.

## Costs

End-to-end builds pay for an external C compilation and cannot meet the final
fast-build target alone.

## Evidence

Installed C toolchains provide optimized native code while deterministic C can
be compared across bootstrap stages.

## Removal

Retain as the reference backend even after a faster direct backend becomes the
default.
