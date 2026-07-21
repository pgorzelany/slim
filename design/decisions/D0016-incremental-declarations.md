# D0016: Declaration-local incremental compilation

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 2
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 60

## Need

Clean compilation is approximately linear, but millisecond feedback on very
large programs requires reuse. The smallest stable reuse boundary in Core 0 is
a named module declaration.

## Alternatives

Whole-file caching cannot reuse work after ordinary edits. Token-level caching
adds complexity below the semantic invalidation boundary. A plan-only
dependency graph followed by full recompilation does not improve feedback and
must not be presented as incremental compilation.

## Costs

The compiler retains normalized declaration fingerprints, dependency edges,
checked results, generated fragments, and source-relative diagnostic data.
Interface changes require reverse dependency traversal and deterministic output
reassembly.

## Evidence

Tests and benchmarks must count actual parsed, lowered, checked, generated, and
reused declarations. Incremental output must equal clean output byte for byte.

## Removal

Replace only with a finer or coarser query model that demonstrates lower total
complexity and equal or better invalidation precision, determinism, diagnostics,
and measured edit latency.
