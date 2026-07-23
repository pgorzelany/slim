# Bounded resource evidence

Status: Core 1H accepted bounded evidence

`slimc analyze SOURCE_OR_PROJECT` publishes resource facts inside analysis
schema 6. These facts reuse the checked integer-range view; they add no source
syntax, runtime check, optimizer promise, or executable IR.

## Current proof

The compiler recognizes one canonical tail recurrence: an `I64` controller
stops at an exact lower bound and every recursive step subtracts the same
positive exact value. A retained profile reports the function identity,
controller position, stop bound, and step.

A direct call to that profile has exact iteration work only when the controller
argument is an integer literal accepted by the existing checked calculation.
A nonliteral controller is `unknown`, never zero. The report also gives exact
and unknown call-site counts and the largest exact workload.

## Bounds

- At most 16 recurrence profiles are retained and printed.
- At most 64 profiled call sites are printed.
- All matching call sites are still counted when the call-site print limit is
  exceeded.
- The report states whether recurrence profiles, call sites, or the underlying
  64-refinement range proof were truncated.
- Any truncation changes the enclosing guarantee from `exact` to `bounded`.

The implementation scans checked tokens once and searches only the fixed
16-profile prefix. Ordinary checking, emission, and generated programs do not
perform this analysis.

## Evidence and limits

The permanent 14-application baseline is
`benchmarks/resource-baseline.tsv`, enforced by:

    cargo run --release --quiet --bin slim-bench -- resources

The current corpus proves two 2,000,000-iteration calls in each of
`state_machine` and `signal_network`. The other 12 applications expose no
supported recurrence profile. That is missing evidence, not evidence of zero
work.

The same baseline records structural expression, allocation, and trap sites;
declared effects; owned bindings; maximum live-owned pressure; and exact
total-function counts. These are separate measurements. They do not infer
physical time, allocation volume, peak bytes, or a complete execution-step
bound.

## Boundary

D0074 closes Core 1H without source-level resource contracts. Reconsideration
requires application evidence showing which repeated assumption inference
cannot recover, followed by a separate weighted feature decision, diagnostics,
semantics, and permanent tests.
