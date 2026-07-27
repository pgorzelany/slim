# RFC-0083: Close Core 1L and release SLIM 1.0

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 100

## Need

RFC-0082 defines the compatibility boundary, but SLIM 1.0 also needs evidence
that a user can obtain, bootstrap, and use the same source that passed the
repository gates. Core 1L must close on a reproducible clean installation and
canonical documentation, not on an in-repository build directory.

The release has one source archive, one canonical version, one production
compiler path, one runtime ABI, and one compact documentation site rendered
from repository sources. Core 1L adds no language surface.

## Alternatives

Publishing an archive assembled by hand would not prove its contents or
reproducibility. Shipping a prebuilt compiler alone would weaken the
self-hosting trust chain. Calling the existing repository tree a release would
leave clean-install assumptions untested. A larger package manager, binary
distribution system, native backend, or rewritten documentation portal is not
required to validate the 1.0 language.

The accepted source archive is deliberately modest. It contains the compiler,
portable seed, runtime, complete tests and evidence, examples, and canonical
documents; it excludes repository history, caches, build output, and the
isolated website toolchain.

## Costs

The package gate copies only committed tracked paths, verifies an internal
SHA-256 manifest, normalizes archive metadata, and bootstraps a clean
extraction. It adds release time but no compiler pass, generated branch,
runtime allocation, dependency, syntax, alias, unsafe escape, or executable
representation.

Only Darwin/arm64 with Apple Clang is claimed as a validated release platform.
The C11 backend remains portable by design, but another target becomes
supported only after running the same gate there. This is more honest than an
untested platform matrix.

## Evidence

Two independently staged archives are byte-identical. A clean extraction
verifies every shipped digest, reaches the 2,154,365-byte bootstrap fixed
point, runs all shipped examples, runs the deterministic multi-module example
and sieve application, exposes the frozen schemas, and rejects a mismatched
runtime ABI. The complete repository gate passes 116 fixtures, 2,000
malformed-input mutations, 34 end-to-end tests, sanitizers, fault injection,
application suites, and permanent performance budgets.

The website build compiles and runs every promised tutorial example, projects
the accepted surface ledger exactly, checks all four routes and links, and
publishes the same version and status from canonical sources. The dated Core
1L evidence record preserves commands, target details, and remaining limits.

## Removal

A 1.0 release must not be reissued from different source under the same
version. Correct a packaging or documentation defect in a new patch release.
Remove a platform from the supported set if its complete clean-package gate no
longer passes. Any future distribution mechanism must preserve the canonical
version, source manifest, bootstrap proof, ABI check, and compatibility
policy; it must not create a second compiler or language surface.
