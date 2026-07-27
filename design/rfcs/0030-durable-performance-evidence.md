# RFC-0030: Durable performance evidence and indexed compilation

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 0
Compile: 2
Runtime: 1
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 60

## Need

SLIM's fast-feedback goal is a permanent property, not a milestone-local
demonstration. Core 1B measured ordinary checking at 16.2 ms for 2,000
declarations, but deterministic C emission took 251.9 ms and grew
superlinearly. A private edit to an 8,001-declaration program also retained
whole-program wall-time work despite exact declaration-local semantic work.

Core 1C makes performance evidence part of the maintained product contract.
The repository stores versioned metric definitions, budgets, workload
manifests, and frozen results. The quick release gate checks correctness,
determinism, work counts, scaling exponents, and same-host ratios. Full runs
record wall time, peak memory where portable, generated size, native runtime,
and binary size. Relaxing a budget is an architecture change: it requires an
accepted decision with score at least +60, a measured cause, quantified impact,
and a containment or compensation plan.

Production optimization remains in SLIM. Repeated declaration-name scans are
replaced by one compiler-owned derived index shared by project flattening and
code generation. The index is not source syntax, a second IR, or a persisted
trusted artifact; it is rebuilt from checked canonical SLIM and cannot change
name resolution.

## Alternatives

A one-time before/after report would show a speedup but would not prevent its
later loss. Absolute microsecond limits alone are unsuitable because hosts,
operating systems, and toolchains vary. Core 1C therefore combines portable
algorithmic gates and same-host ratios with dated absolute measurements.

Adding a compiler-specific bulk-output language primitive could reduce byte
emission overhead, but it would grow source and runtime surface before the
measured repeated lookup and whole-program scan costs are removed. A direct
native backend would add a much larger permanent implementation and trust cost
while leaving frontend and incremental scans untouched.

Linear name lookup preserves a smaller implementation but makes every call and
project atom rescan declarations. Memoization keyed by source offsets would
make offsets an unstable semantic interface. The derived name index is local
to one checked compilation and stores only lookup acceleration data.

## Costs

The benchmark runner gains versioned manifests, budget parsing, regression
checks, phase-oriented commands, and a larger matched workload corpus. This is
bootstrap-independent Rust measurement infrastructure; it does not implement
SLIM semantics. The normal release gate becomes longer because a bounded quick
performance and challenge pass always runs.

The self-hosted compiler gains an internal name index and threads it through
the project flattener and C emitter. Building the index costs linear source-name
bytes plus bounded lookup overhead and additional memory proportional to the
number of declared-name bytes. The index is discarded with the compilation
region. Checked programs and generated binaries pay no runtime cost.

Budgets deliberately include a noise margin. They detect substantial
regressions rather than pretending that noisy microbenchmarks are exact.
Frozen result files remain observations and cannot silently redefine a gate.

## Evidence

Acceptance requires a pre-change baseline, profiler evidence for each targeted
bottleneck, and repeated candidate measurements after warmup. Quick and full
geometric series must enforce both check and C-emission exponents at or below
1.25. At 2,000 generated declarations, process-level emission must be no more
than four times checking and at least five times faster than the frozen Core 1B
251.9 ms baseline on the recorded host.

The permanent challenge manifest must cover scalar computation, vectors,
records or variants, mutation, recursion, checked failure, byte processing,
and graph-shaped work. Every matched SLIM/C/Rust program must produce identical
output. Native runtime and binary-size ratios are checked independently from
compiler feedback and agent-edit evidence.

Incremental tests continue to assert exact declaration work and add durable
wall-time/scaling evidence for no-change and private-body edits. The complete
conformance corpus, malformed-input campaign, sanitizer and allocation-failure
tests, deterministic generated C, and byte-identical self-host fixed point
remain mandatory.

## Removal

Remove a metric that cannot be reproduced or does not protect a design goal;
replace it with one versioned metric rather than retaining aliases. Remove an
optimization if its derived state can affect semantics, determinism, or
diagnostic identity. If the internal index does not beat bounded linear lookup
on the real compiler and geometric corpus, revert it while retaining the
performance gate that exposed the problem.

