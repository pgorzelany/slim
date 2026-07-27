# RFC-0016: Declaration-local incremental compilation

Status: accepted
Implementation: complete
Process: legacy
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

Tests count actual parsed, lowered, checked, generated, and reused declarations
and compare incremental output with clean output byte for byte. The geometric
benchmark in `benchmarks/results/2026-07-21-incremental.tsv` records cold,
no-change, body, leaf-interface, and central-interface updates through 8,001
declarations, including the remaining whole-file wall-time costs.

## Removal

Replace only with a finer or coarser query model that demonstrates lower total
complexity and equal or better invalidation precision, determinism, diagnostics,
and measured edit latency.
