# D0015: Executable conformance corpus

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 50

## Need

Compiler behavior is currently distributed across implementation tests and
documentation. Stage replacement and incremental reuse require an independent,
executable statement of accepted and rejected Core behavior.

## Alternatives

Rust-only tests make the reference implementation the specification. Prose-only
rules cannot block regressions. Snapshotting all diagnostics is brittle and
does not prove that every accepted surface entry is covered.

## Costs

The repository carries fixture sources, a manifest, and a dependency-free
runner. Stable diagnostic expectations require deliberate updates when a
specified diagnostic changes.

## Evidence

The dependency-free `slim-conform` runner validates syntax, types, ownership,
effects, built-ins, runtime behavior, exact diagnostic code and primary span,
multiple-error recovery, formatting, deterministic generation, and declared
self-host capability. Governance rejects accepted surface entries without a
manifest coverage tag.

## Removal

Remove only if a stronger implementation-independent executable specification
covers the same behavior and differential gates.
