# D0092: Fixed-size array type

Status: rejected
Kind: language
Primitive: fixed-size-array
Safety: 2
Compile: -1
Runtime: 2
Minimal: -1
Analysis: 2
Dogfood: 0
Score: 35

## Need

The expanded native corpus shows that stencil, byte traversal, sieve, and
matrix code spend meaningful work preserving checked arithmetic and collection
bounds. A hypothetical fixed-size collection could make element count part of
the checked type, permit static or region-inline storage, and give the analyzer
a constant upper bound without recovering vector construction history.

This is the strongest language-level information proposal found during the
safe C-parity work. It is not accepted. A useful design would need one canonical
initialization form, exact move and borrow rules, an ABI for small and large
sizes, checked indexing, a rule for passing sizes across functions, and either
numeric type parameters or a different non-duplicating representation.

## Alternatives

Bounded compiler inference can recover induction ranges, invariant arguments,
vector lengths, and repeated guard relationships from existing source without
adding syntax. Dynamic guard hoisting and small exact-recurrence specialization
can preserve fallback behavior when a proof is unavailable. Both should be
exhausted first.

`Array<T, N>` beside `Vec<T>` risks two permanent ways to represent the same
sequence. Replacing `Vec` outright is incompatible with dynamic input,
incremental construction, and bounded network/file operations. A library array
cannot currently give the compiler a checked numeric type parameter or
stack/inline representation guarantee.

A value-tied bounded-index type could express more dynamic cases, but it also
introduces dependent value identity, lifetime transport, and another indexing
surface. It is not a smaller first feature.

## Costs

The feature adds a type constructor with a value-level argument, type equality
rules, formatter and diagnostic cases, initialization and ownership semantics,
new generated C layout, aggregate-size limits, and new stack/region failure
questions. Large arrays make implicit copies or stack placement unacceptable,
while reference-based storage overlaps `Vec`. These are negative compile and
minimality costs even though bounds safety and optimization information are
strong.

The current fixed-size Game of Life fixture is not enough evidence. A temporary
diagnostic build that removed all six generated vector bounds checks improved
the measured program only from about 21.4 ms to 19.4 ms on the recorded host;
handwritten C remained about 6.4 ms because exact small-loop unrolling and
checked-arithmetic proofs were still missing. An array feature therefore would
not by itself close the dominant gap.

## Evidence

Acceptance would require multiple real programs whose sizes are intrinsic,
measured gains beyond what bounded inference and guard hoisting achieve, a
single non-duplicating relationship with `Vec`, compile-scaling evidence,
self-host use, and a score of at least +40 with no negative safety, compile,
runtime, or minimality rating. This proposal scores 35 and fails the compile
and minimality hard gates.

No syntax, surface-ledger row, compiler path, runtime ABI, or compatibility
promise is added. The proposal is flagged for review only if later inference
work demonstrates that static size is the remaining material blocker.

## Removal

Keep the rejection while existing source can expose equivalent proofs through
bounded analysis or while the type duplicates vector semantics. Replace this
record only with a measured design that defines initialization, ownership,
layout, size transport, diagnostics, and migration as one coherent feature;
never add an array literal or unchecked indexing alias independently.
