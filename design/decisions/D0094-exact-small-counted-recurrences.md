# D0094: Exact small counted recurrences

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

SLIM has one iteration form, tail `recur`. The checked source therefore retains
the semantic controller and argument updates, but generic C lowering turns every
recurrence into assignments and a backward `goto`. Native optimization recovers
ordinary large loops reasonably well, yet it does not reliably recover a small
exact counted loop with multiple tail paths. The eight-neighbor Game of Life
stencil consequently remains about four times the matched Rust implementation.

The bounded range view may record an exact small counted recurrence only when
the complete checked function has a top-level equality guard, both Boolean arms
are present, every recurrence in the iterative arm advances the same linked I64
parameter by the same positive exact step, all closed-program calls agree on an
exact start, the exact bound is known, the distance is nonnegative and exactly
divisible by the step, and the iteration count is at most 16. Missing calls,
computed starts, conflicting calls, unsupported guards or updates, additional
recurrence shapes, saturated arithmetic, and larger counts remain unknown.

The fact is attached to stable source nodes and names the iterative and base
expressions. It is explanatory lowering data, not executable input or a second
IR. A backend may consume the fact as a counted loop or through the bounded
stage expansion accepted by D0096. Every consumer must retain left-to-right
argument materialization, assignment order, early-result paths, allocation
propagation, and the function epilogue.

## Alternatives

Adding `for`, `while`, an unroll annotation, or unchecked recurrence would
duplicate the one canonical source operation. Treating direction as a proof
would be unsound for an equality guard that can be skipped. Reconstructing
loops from emitted C would add a second parser and lose stable checked node
identity. Unconditionally unrolling source text would increase generated code
and compilation cost without a checked bound.

The rejected D0089 induction candidate proved only one recurrence path and did
not materially improve the corpus. D0094 instead records a complete multi-path
counted-loop fact and keeps specialization behind a fixed 16-iteration budget.

## Costs

One token scan per candidate function validates recurrence updates. Direct call
starts are collected by one additional global scan in each existing fixed
analysis pass, rather than rescanning the program for every function. The work
is therefore linear in checked source size with a fixed pass factor; no
source-size-dependent fixed point, solver, dependency, syntax, runtime ABI,
allocation, or language surface is added. Only functions with a positive exact
fact change generated control flow, and the fact can be consumed unchanged by
a future native backend.

Permanent tests must cover one- and multi-path positive loops; computed,
conflicting, overshooting, zero-step, wrong-controller, early-return, and
over-budget non-applicability; deterministic reports and C; preserved checked
collection access; overflow traps outside a proof; bootstrap fixed point; and
geometric compiler scaling.

## Evidence

A diagnostic generated-C experiment on the existing eight-neighbor Game of
Life recurrence showed that recovering the exact counted shape together with
total controller and accumulator arithmetic lowers the program from about
21 ms to 6.7 ms on the recorded host while retaining vector bounds checks; the
matched C program measured about 5.9 ms. Counted control flow alone was nearly
neutral, so retention also requires reusable relational facts rather than a
C-only syntax change.

The retained production lowering preserves all twenty matched outputs. In the
authoritative full comparison, Game of Life measures 4.715 ms for SLIM versus
7.059 ms for matched C and 5.043 ms for Rust. Counted control alone remained
neutral; D0095, D0096, D0097, and D0098 together make the checked facts useful
without weakening unsupported loops.

## Removal

Remove specialization if any accepted call lies outside the recorded start and
bound, if any recurrence path can skip or reverse the controller update, if
evaluation, trap, allocation, cleanup, or result order changes, if the bounded
fact is trusted without replay from checked source, or if compiler scaling or
the native corpus regresses. Retain the decision evidence, tests, and benchmark
corpus even if the C consumer is replaced or rejected.
