# RFC-0073: Bounded resource evidence before contracts

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

Core 1H must make physical resource limits useful, but current application
evidence does not justify a source-level resource contract. The checked range
view already proves one canonical tail recurrence by retaining its controller
position, stopping bound, and positive decrement. Core 1G consumes that proof
only at selected parallel call sites with literal arguments. Other tools and
calls cannot inspect the reusable recurrence profile or distinguish exact work
from a missing argument bound.

Analysis schema 6 therefore publishes the retained recurrence profiles and
their call-site workloads. A call to a retained profile reports exact iteration
work only when the existing checked calculation succeeds; otherwise it reports
`unknown` with a stable reason. This is resource evidence, not a runtime limit,
termination annotation, optimizer hint, or claim about arbitrary functions.

## Alternatives

A source contract, bounded integer type, loop form, or target profile could
provide missing bounds, but each would add permanent semantics before real
applications show which assumptions inference cannot recover. A separate
resource analyzer would duplicate checked links and recurrence reasoning. An
SMT solver or symbolic algebra dependency would increase latency and trusted
surface without evidence that the current finite proof is insufficient.

The accepted slice reuses the production checked token identities and the one
range view already built by `slimc analyze`. It adds no Core form, built-in,
effect, runtime ABI, dependency, executable IR, or ordinary compilation work.

## Costs

The report retains and prints at most 16 recurrence profiles and 64 profiled
call sites. It scans checked tokens once and searches only the retained
16-profile prefix, so work is linear with a fixed bound. Counts, range-proof
saturation, and truncation are explicit. Calls to profiles beyond the retained
prefix cannot receive an exact result.

The new report increases opt-in analysis output and self-hosted compiler size.
It does not change `check`, code generation, generated programs, or the Core 1G
execution decision. Analysis schema advances from 5 to 6, requiring consumers
to update deliberately rather than misreading new fields.

## Evidence

Acceptance requires exact profile fields for a zero-bound unit-step recurrence,
exact zero and positive call workloads, an explicit unknown result for a
nonliteral controller, deterministic profile and call-site report limits, and
balanced byte-identical reports. The maintained application corpus must record
profile counts, exact and unknown call workloads, maximum exact iterations,
allocation sites, trap sites, and owned-value pressure without interpreting
unknown as zero.

The application baseline, geometric analysis scaling, full conformance and
malformed-input campaigns, fixed-point bootstrap, sanitizers, allocation
failure, native comparisons, and parallel runtime gates remain permanent.

## Removal

Remove a profile or call-work result if it cannot be traced to the exact
checked recurrence proof and call node. Never infer a nonliteral argument,
silently omit truncation, turn unknown into a bound, or use this tooling report
as an execution contract. Any source-level resource declaration still requires
a separate weighted language decision backed by application evidence.
