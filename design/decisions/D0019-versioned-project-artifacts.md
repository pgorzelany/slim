# D0019: Versioned project interfaces and caches

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 70

## Need

Precise cross-module invalidation requires a body-independent public interface.
Cross-process reuse requires cache evidence that can be rejected safely rather
than trusting timestamps or host paths.

## Alternatives

Hashing complete source invalidates consumers after private edits. Serializing
compiler memory layout is unstable and unsafe to trust. Timestamp caches are
incorrect under clock, checkout, and relocation changes. Third-party object or
serialization formats would add dependencies and more semantics than Core 0.2
needs.

## Costs

The compiler owns strict text-interface and binary-cache encoders, versioning,
bounds checks, checksums, atomic writes, and corruption tests. Schema changes
require explicit compatibility decisions. Interface artifacts deliberately do
not promise a stable external native ABI.

## Evidence

Tests must compare artifact bytes across processes, project locations, manifest
order, worker counts, and private edits. Every truncated or mutated cache class
must rebuild safely. Incremental results and diagnostics must equal a clean
oracle byte for byte.

## Removal

Replace only with a smaller validated format that preserves relocation,
determinism, exact invalidation, corruption recovery, and dependency-free
operation. Old readers must be removed rather than silently guessing schemas.
