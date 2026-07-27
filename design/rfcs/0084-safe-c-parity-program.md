# RFC-0084: Safe C-parity performance program

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

SLIM 1.0 is close to the matched Rust corpus but remains slower than matched C
on most serial applications. The fourteen-application quick baseline on the
release host has a 1.27 geometric-mean SLIM/C runtime ratio; excluding the two
automatically parallel applications gives 1.36. The largest gaps remain in
checked collection traversal, mutation-heavy algorithms, byte processing, and
nested integer work.

Post-1.0 performance work targets a geometric-mean ratio at or below 1.00
without changing the safe language semantics. The benchmark corpus must grow
before the optimizer does: at least six additional matched SLIM, C11, and Rust
applications must cover larger dense-graph, dynamic-programming, search,
disjoint-set, cellular, and stencil work. Every implementation uses the same
algorithm and data scale, produces identical output, and receives a permanent
same-host ratio budget.

Compiler optimizations consume positive facts from the checked SLIM artifact.
A checked operation may lose its dynamic trap only when the existing bounded
analysis proves that exact source node total. Missing, saturated, out-of-domain,
or unsupported evidence keeps the check. New benchmark-specific built-ins,
unchecked profiles, source optimizer hints, or weakened traps are forbidden.

## Alternatives

Optimizing only the existing fourteen programs would make it too easy to fit
the compiler to known shapes. Comparing absolute timings across hosts would
confuse toolchain and machine changes with language progress. Declaring C
parity from two automatically parallel programs would hide the remaining
serial cost. Removing checks in optimized builds would create a second unsafe
language and violate the goal.

The retained `ranges` view already classifies integer expressions by stable
token identity as exact, bounded, or unknown. Code generation can consume that
view directly instead of adding syntax, a second IR, or a duplicate optimizer
analysis. Collection-bound elimination requires a separate positive proof for
the exact access and is not inferred from integer totality alone.

## Costs

The quick and full release gates become longer as the matched corpus grows.
The benchmark runner gains aggregate reporting but per-application rows and
budgets remain authoritative; an aggregate never hides a regression. Each new
workload also needs durable resource and parallelism evidence because the same
applications are compiler-analysis dogfood.

Threading exact facts through code generation increases compiler source and
does bounded constant work at each checked node. Default checking is unchanged;
emission must retain its geometric scaling budget. Generated C changes only
where a positive fact applies. Proof and non-applicability fixtures, trap
fixtures, bootstrap fixed points, sanitizers, and before/after native
measurements are required for every eliminated check class.

A future language-level feature is considered only if multiple expanded
applications expose the same material gap and bounded inference cannot recover
the required fact. Such a proposal is recorded and scored separately with a
quantified expected benefit; this decision does not authorize adding it.

## Evidence

The initial release-host quick run measures 1.01--1.74 SLIM/C on the twelve
serial applications and 0.72--0.94 on the two automatically parallel
applications. Generated matrix C retains checked multiply, add, division,
remainder, and vector-index operations inside its hot loops; Clang reports that
trap control flow prevents loop vectorization. The range analyzer already
records node-indexed totality and bounded results but ordinary code generation
currently uses the view only to derive parallel plans.

Acceptance requires at least twenty matched applications, identical outputs,
dated baseline and candidate measurements, per-application budgets, separate
all-application and serial geometric means, and the complete repository gate.
The final C-parity claim additionally requires a full-run all-application
geometric mean no greater than 1.00, no unexplained workload regression, and
no removed safety fixture or relaxed performance budget.

## Removal

Retain the expanded corpus and every tightened budget even if a particular
optimization is reverted. Remove a proof-driven fast path if its fact cannot
be tied to the exact checked node, if it changes a trap or observable effect,
or if its compiler cost violates the standing scaling limits. Replace this
program only with stronger same-algorithm evidence; never satisfy it by
weakening C, shrinking workloads, discarding slow rows, or changing SLIM's
safe semantics.
