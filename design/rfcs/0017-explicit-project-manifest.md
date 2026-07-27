# RFC-0017: Explicit project manifest

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 1
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 60

## Need

Multi-file compilation needs one reproducible source of module identities,
paths, direct imports, exports, and the entry module. Ambient filesystem search
would make builds depend on state that source does not declare.

## Alternatives

Source-level import and export forms permanently enlarge every module and make
path policy a second concern. Directory discovery, environment search paths,
and extension-based conventions are implicit and non-reproducible. A central
canonical manifest uses the existing S-expression machinery and adds no
runtime or expression semantics.

## Costs

Adding or exposing a module requires editing the manifest. Large projects may
experience merge contention in one file. Strict sorting rejects otherwise
equivalent orderings, but gives AI tools and caches one representation.

## Evidence

Project conformance must cover canonical parsing, path confinement, exact
diagnostics, visibility, cycles, relocation, and byte-identical clean output.
Geometric tests must show approximately linear discovery and resolution.

## Removal

Replace only with a single model that has fewer concepts while retaining
explicit dependency edges, path confinement, deterministic discovery,
relocation, and stable diagnostics. Do not retain this manifest as an alias.
