# RFC-0096: Bounded counted recurrence expansion

Status: accepted
Implementation: complete
Process: legacy
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

RFC-0094's exact counted-loop fact and RFC-0095's accumulator ranges remove proven
overflow checks, but emitting the recurrence as a C loop remains nearly neutral
on the native corpus. The verbose, safety-preserving C body exceeds the native
compiler's loop-unrolling cost model. Neither direct `continue` lowering nor a
native unroll pragma changed the Game of Life result materially.

For a RFC-0094 fact only, the compiler may emit one guarded `do` stage per exact
iteration instead of a backward C edge. A recurrence still materializes every
changed argument left to right, performs assignments in parameter order, and
uses `continue` to leave exactly one stage. A non-recurrent path retains the
existing early-result exit. The base expression executes after all stages.
Expansion is limited by RFC-0094's fixed maximum of 16 iterations.

The specialized path is unavailable when the iterative body contains an
explicit fork or a selected automatic-parallel site. Those functions retain
the ordinary recurrence and parallel lowering. The source language, runtime
ABI, and canonical `recur` operation are unchanged.

## Alternatives

Adding loop or unroll syntax would expose a second source form for recurrence.
Backend-specific pragmas were rejected because the measured compiler ignored
the request for this body and a future backend could not reuse it. Raising
global native-compiler unroll thresholds increases compile time and code size
for unrelated code. Unbounded expansion would violate SLIM's compilation and
generated-size goals.

The compiler-owned expansion consumes an exact checked fact, has a fixed
budget, and is backend-independent. A future native backend can apply the same
semantic transformation without preserving any C-specific mechanism.

## Costs

Emission visits an accepted iterative body at most 16 times, so generated code
and emission work remain linear with a fixed factor. Only positively proven
small counted recurrences pay that factor. The larger generated function can
increase downstream compilation time and binary size; the permanent comparison
suite must measure both.

Tests must cover zero and maximum iteration counts, multiple recurrence paths,
early-result paths, explicit and automatic parallel non-applicability,
left-to-right changed-argument evaluation, preserved traps and bounds checks,
deterministic C, bootstrap fixed point, and compiler scaling.

## Evidence

On the recorded host, the retained C loop measured about 19.1 ms for Game of
Life. Removing recurrence copies and labels remained about 19.1 ms, and a full
unroll pragma was also neutral. A diagnostic fixed nine-stage expansion of the
same checked body reduced ten process runs from about 0.18 seconds of user CPU
to about 0.05 seconds, matching the previously simplified 6.7 ms prototype's
fully unrolled native shape.

Production retention requires exact output parity across the full corpus, a
material Game of Life gain, bounded compilation and binary-size impact, and no
safety or parallelism regression.

The retained production path preserves all twenty matched outputs and measures
Game of Life at 4.715 ms versus 7.059 ms for C and 5.043 ms for Rust in the
full comparison. The compiler fixed point is 2,325,047 generated C bytes, the
full geometric performance suite passes, and no selected or executable
parallel site changes.

## Removal

Remove expansion if a stage can execute more or fewer times than the exact
fact, a recurrence can target the wrong stage, an early result reaches the
base, argument evaluation or cleanup order changes, parallel work is
serialized, generated size or compile scaling exceeds its permanent budget, or
the named runtime gain does not reproduce. Keep the exact recurrence and
accumulator facts even if this consumer is rejected.
