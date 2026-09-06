# RFC-0128: Checked layout emission order

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 1
Dogfood: 1
Score: 10

## Summary

Retain the existing inline-layout checker's completion order in its checked result
and use that order to emit complete C aggregate definitions. This repairs the
recorded M1 blocker for nominal compiler records referring to later modules/types.

## Motivation

The checker accepts acyclic inline record/enum dependencies independently of source
order. C requires an inline field's complete type definition before the containing
aggregate. The current backend emits source order, so accepted forward references
can fail in the native backend. Forward typedefs do not make inline fields complete.

## Guide-level explanation

A struct or enum with an inline field is emitted after that field's struct or enum.
Ordinary source ordering and field/case ordering remain unchanged. Collection/Id
constructors break inline layout dependencies as already specified by the checker.
There is no new source syntax, inferred conversion, or runtime representation.

## Reference-level specification

The existing `typing.check_inline_layout_item` performs the only inline-layout
dependency traversal. It uses its existing marks and canonical resolved type links,
visits dependencies in field/payload order, and appends the declaration node once
when marking it complete. The outer root order remains lexical. A shared dependency
is not expanded again. Existing E0354 cycle diagnostics and recovery are unchanged.

`typing.View.layouts` owns this completion-order vector. The vector is current-
source explanatory data from the normal checker, not an independently accepted
program representation. It contains raw canonical positions only within that
checked source; retained cross-revision queries must later supply typed ownership
and a complete import map under RFC-0124. It cannot be deserialized to authorize
emission. Invalid checking results cannot reach the normal emission path.

`codegen.emit_program` receives the checked layout order alongside checked facts
and the memory plan. It emits forward typedefs in the existing lexical order,
then emits each listed complete definition once. Definition bodies, tags, field
order, alignments, names, function emission and runtime ABI are unchanged. Source
which already defines dependencies first retains byte-identical generated C.

There is no second topological sort, copied layout checker, global text search, or
new whole-program dependency scan. A cycle can leave diagnostic-only traversal
results, but it cannot produce accepted C. Allocation failure preserves status 71
and cannot publish a partial C artifact or replace a successful cache entry.

## Compiler and runtime design

Add one ordinary vector to the checked view and thread it through the existing
layout traversal and two production generator callers. Keep empty views empty.
The sole producer appends after the existing completion mark; the emitter consumes
rather than rediscovers dependencies. No new module, dependency, primitive, or
runtime code is required. Regenerate the portable seed normally.

## Compatibility and migration

This restores existing accepted source behavior. Complete definition order changes
only where the old order did not satisfy C's complete-field requirement. Negative
cycle/type fixtures preserve their codes, spans, ordering and acceptance status.
Existing exact native analysis and resource rows must remain unchanged.

## Diagnostics and failure cases

Unknown/malformed types and inline cycles remain the normal checker's responsibility.
The order is not a validation certificate for a modified token vector. Normal
emission requires the same complete checked source that owns all its inputs.
Do not add an internal fallback which guesses missing definitions.

## Performance and complexity

Append one I64 per completed aggregate; consume that vector once. The existing
approximately linear layout traversal and lazy marks remain intact. Programs with
no data declarations do not allocate a nonempty order buffer. Record before/after
latency, allocation work, and geometric aggregate families; preserve all gates.
This is a compiler storage cost and makes no runtime speed claim.

## Alternatives and drawbacks

A second backend dependency walker would duplicate a checked semantic relation.
Ordering modules or renaming compiler modules around the defect would preserve the
bug and prevent ordinary cross-module nominal composition. Retaining the existing
completion order costs one vector descriptor and one position per aggregate.

## Test and acceptance plan

Run native C compilation and execution over every permutation of mixed record/enum
chains and shared dependencies; include aggregate containers, cross-module forward
references, unchanged bytes for previously supported programs, complete analysis,
and deterministic C on repeated emission. Preserve inline-cycle/unknown-type
negative and diagnostic fixtures. Observe geometric layout visits and emitted
aggregate counts, sanitizer execution, and bounded compiler allocation failures.
Run bootstrap and all required checkpoint gates before marking this child complete.

## Ratings and evidence

Analysis +1 retains a checked dependency result instead of recomputing it. Dogfood
+1 enables ordinary nominal compiler records across module order. Other ratings
remain zero; weighted score 10. Measured same-host check/emission ratios remain
near one, without a claimed speed improvement. The retained result costs one
40-byte vector descriptor and 8 bytes per entry before vector capacity slack on
the recorded host. No data declarations means no order-buffer heap allocation.

## Decision

Accepted under the maintainer's RFC-0112 delegation and explicit M1 completion
request, following RFC-0123 and RFC-0124. This is not independent external review.

## Implementation

Implemented by retaining layout completion order in `typing.View` and
`project.PreparedProject`, and passing it through the two production generator
callers. The existing memory-plan governance gate remains intact. The seed reaches
a 3,613,447-byte fixed point. All 24 mixed aggregate permutations and a cross-module
forward-reference case compile and run through the production compiler. A permanent
conformance/sanitizer fixture also covers collection-mediated self-reference.

All 10 unit and 64 integration tests, 332 conformance fixtures plus 2,000 malformed
mutations, required checkpoint benchmarks, complete native analysis/resource
baselines, and sanitizer/fault gates pass. Existing native C and 193 rejected
fixtures' diagnostics remain byte-identical to 8d7b504. The observed layout family
visits 5N inline types and emits 4N aggregate definitions. In 256 allocation-fault
ordinals, 196 fail without partial C and 60 succeed; failures occur after layout
traversal and aggregate emission begin. The dated progress report and linked TSVs
record exact scopes, costs and identities.

The derived flow/ownership view, retained semantic queries, transactional
publication and incremental C emission remain outstanding M1 obligations.

## Removal and supersession

Any replacement must preserve the checker's sole authority, complete deterministic
layout dependency ordering, existing cycle rejection, and all native regressions.
