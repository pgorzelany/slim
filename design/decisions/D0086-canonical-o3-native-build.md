# D0086: Canonical O3 native build

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 2
Minimal: 2
Analysis: 1
Dogfood: 1
Score: 70

## Need

The permanent native comparison compiles matched C with `-O3` and Rust with
`opt-level=3`, but the one supported `slimc build` path compiles generated C
with `-O2`. This leaves safe host optimization unused and makes the comparison
unnecessarily asymmetric.

`slimc build` and `slimc run` use `-O3` for their one canonical native
executable. Generated C, runtime checks, target macros, effects, allocation
failure, and diagnostics are unchanged. Bootstrap continues to use `-O2`
because bootstrap latency is a compiler-distribution concern rather than the
runtime artifact measured by the application suite.

## Alternatives

Adding debug/release/unsafe source profiles would create multiple ways to build
the same program and is rejected. Removing checks at higher optimization would
change SLIM safety and is forbidden. Mandatory LTO was rejected by D0059
because of compile latency and toolchain weight; this decision does not add it.
Target-specific `-march=native` would make artifacts host-dependent and is not
accepted.

Keeping `-O2` solely because it was the initial default would preserve a known
comparison handicap without a language or safety benefit.

## Costs

Native C compilation may take longer. The comparison runner already records
SLIM build time for every application, and the complete release gate recompiles
the representative corpus. The change is retained only if full same-host
runtime evidence improves materially without an unreasonable compile-time
increase.

There is no syntax, semantic, dependency, runtime ABI, compiler pass, target
feature, or source compatibility cost. `-O3` remains a portable optimization
level understood by the supported C compiler.

## Evidence

The D0085 full candidate is the `-O2` baseline: 1.359 geometric mean against C
and 1.439 across serial applications. The full `-O3` result is 1.335 overall
and 1.412 across serial work. All twenty outputs remain identical, median SLIM
build times remain in the same approximate 156–198 ms band, and the complete
trap, fault-injection, sanitizer, bootstrap, and repository gates pass.

## Removal

Restore `-O2` if the full corpus does not improve materially, any supported
program changes output or trap behavior, native compile cost becomes
unreasonable, or a supported C toolchain cannot honor the flag. Do not replace
this with source build profiles, target-specific flags, LTO, or weakened safety
without a separate accepted decision.
