# D0014: Deterministic self-host bootstrap

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 1
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 50

## Need

SLIM must demonstrate that its small Core is sufficient to implement a real
compiler and must detect accidental dependence on behavior available only in
the Rust stage-0 implementation.

## Alternatives

Keeping only stage 0 provides no dogfooding proof. A wrapper that invokes stage
0 is not self-hosting. Comparing executables is unstable across toolchains, so
the proof compares deterministic generated C instead.

## Costs

The repository carries a second, deliberately restricted compiler
implementation and a native bootstrap harness. The full verification gate
performs two compiler generations and three small native builds.

## Evidence

slim-bootstrap compiles `selfhost/slim.project` with stage 0, uses the resulting
native compiler to compile that same explicit SLIM project for two successive
generations, requires byte-identical C at the fixed point, and runs a program
compiled by the self-hosted stage. The project contains acyclic compiler-phase
modules and a minimal entry driver; no Rust source bundler participates in
later stages.

## Removal

Remove the restricted compiler only after a replacement SLIM implementation
compiles the complete accepted surface and proves an equivalent deterministic
fixed point.
