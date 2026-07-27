# RFC-0027: Portable C bootstrap seed and SLIM-primary toolchain

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

Core 0.4 has complete differential conformance and a byte-identical
self-hosted fixed point, but a clean checkout still asks the duplicate Rust
compiler to create the first native SLIM compiler. Ordinary `slimc`,
conformance, and benchmark commands also continue to select Rust semantics by
default. This makes the implementation boundary disagree with the
SLIM-first policy and requires two semantic compilers to remain synchronized.

The production compiler will therefore bootstrap from one checked-in,
deterministic C11 translation of `selfhost/slim.project`. A C11 compiler builds
that seed; the resulting executable compiles the same SLIM project for two
successive generations; and the bootstrap accepts only byte-identical generated
C. The seed is a generated trust artifact, not maintained source and not a
second compiler design.

The self-hosted compiler becomes the sole active implementation of parsing,
checking, project semantics, incremental queries, memory planning, and C
generation. Minimal launchers may perform filesystem transport, invoke the C
toolchain, and run executables, but may not decide language semantics.

## Alternatives

Keeping Rust stage 0 indefinitely preserves an independent differential oracle
but also preserves duplicate semantics and permits the user-facing path to
avoid dogfooding SLIM. Checking in a native executable would be smaller to
build but would tie the repository to one operating system and architecture.
A handwritten C seed would create a third semantic implementation. Adding
process, linker, environment, or filesystem-control primitives to SLIM solely
for the launcher would enlarge the language without improving compiled
programs.

Expected fixture outputs, canonical artifacts, successive-generation equality,
malformed-input hardening, and sanitizer execution replace the active Rust
semantic oracle. The final Rust implementation remains recoverable from Git
history for forensic comparison.

## Costs

The repository carries an approximately 1-megabyte generated C seed plus its
SHA-256 digest and provenance. Review applies to the SLIM source and generator;
the seed is verified mechanically. A clean bootstrap requires a C11 compiler.
Removing Rust semantics requires test and benchmark infrastructure to invoke
the native SLIM compiler instead of linking compiler modules directly.

The loss of a permanently maintained independent implementation increases the
importance of external conformance expectations, arbitrary-input tests, fixed
point comparison, deterministic regeneration, and testing with more than one C
toolchain where available. Rust may remain for orchestration, governance, and
measurement when it contains no language decision.

## Evidence

All 61 accepted standalone and project fixtures have exact self-host parity,
and 2,000 deterministic malformed-input mutations reject without a compiler
trap. The Core 0.4 freeze produced identical 829,446-byte stage-2 and stage-3
C. The current Core 1A compiler, including direct reduction and semantic
analysis, produces identical 1,141,206-byte successive generations with
SHA-256
`aeef245413e5766d86ca84efdcea63f3b29694728c4b9a4cf774ce387bfe5572`.
The self-hosted project already implements standalone and project checking,
formatting, interfaces, cache and incremental sessions, deterministic job
planning, C emission, lifetime planning, and typed allocation failure. RFC-0014
and RFC-0022 permit retiring active stage 0 after this parity and fixed-point
boundary is reached.

The cutover is accepted only when a clean checkout can build the seed and
reproduce the fixed point without `cargo` or `rustc`; ordinary compiler and
conformance paths contain no Rust semantic fallback; arbitrary malformed input
cannot crash the compiler; native sanitizer and injected-allocation-failure
gates pass; and the duplicate Rust semantic modules are absent from the active
tree.

## Removal

Replace the generated C seed only with a smaller portable seed that can compile
the complete accepted SLIM compiler and preserve deterministic successive
generation equality. If SLIM eventually targets a stable machine-independent
object format, that artifact may replace C only after equivalent provenance,
portability, audit, and recovery gates pass. Never retain two default bootstrap
seeds or two active semantic compilers.
