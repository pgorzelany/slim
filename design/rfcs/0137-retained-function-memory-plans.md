# RFC-0137: Retained function memory plans

Status: accepted
Implementation: pending
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-07
DecisionDate: 2026-09-07
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Retain the existing production memory planner's per-function results under typed
declaration owners. Reuse only after complete body/interface dependency validation
and explicit relocation of every node and byte position. Feed these results into
the existing checked project and emitter, preserving every field of the clean plan.

## Motivation

M1 currently reuses declaration parsing and function typing, but calls
`memory.analyze` again after each changed project's typing. It rebuilds every
function plan, including its bounded liveness/escape scans. These plans are actual
inputs to C generation and resource reports, so retaining them is part of the
required analysis/emission substrate. This does not replace range, totality,
parallel, C-fragment or backend-artifact retention.

## Guide-level explanation

A function whose body, resolved references and consumed interfaces are unchanged
can reuse its previous memory plan. Moving the function in source reconstructs
current plan positions. A changed callee effect list or referenced aggregate layout
invalidates the appropriate consumers. Missing optional planning history invokes
the ordinary planner even when typing itself was reused. Failed source preserves
the complete last-good snapshot and its history.

## Reference-level specification

The sole plan producer remains `memory.build_function_plan`. It reads function
source, parameter/local type forms, transitive nominal storage layouts, explicit
parameter modes, source names and call effect ceilings. Its allocation classifier
uses `effects.call_requires`, which reads the checked declared effect list rather
than inferred body events. Its liveness and escape calculations inspect only the
current function body, with the existing 64-value precision boundary. Recurrence
presence is a structural body fact. These are the complete current dependency
classes; a future planner read must extend this contract before reuse is allowed.

Use the existing RFC-0130 exact declaration map, source/shape/prior-link comparison
and reverse interface dependency propagation. The resulting body-invalidation
decision is a conservative sufficient dependency key for these planner reads.
It includes layout and effect dependencies through the same resolved declaration
links; namespace changes still undergo ordinary project/type validation. A callee
body change alone need not invalidate this plan when its declared interface is
unchanged. It can still invalidate later body-derived analyses and C generation.
Do not infer those separate results from this plan's validity.

Add optional planning history to the internal retained cache. Each saved function
plan has an exact `identity.DeclarationId` owner. Stored token positions are
declaration-local ordinals with that owner, not transferable old absolute indices.
The saved form records the existing scalar flags/counts, value plans, allocation
plans and destruction plans. Local encoding may be compact private tooling data;
query boundaries must resolve it through the typed owner and current map.

Preserve the existing distinction between positions:

- Function, value name/type/declaration, allocation site and destruction end are
  canonical token positions. Validate their complete local range before import.
- For value indices below 64, `last_use` is a source byte end. Store a checked
  offset from the declaration's complete source origin and translate to current
  bytes only after exact full-source matching.
- At indices 64 and above, the current conservative `last_use` is the body's
  canonical end token. Preserve this legacy representation explicitly as a token
  marker; do not accidentally translate it as bytes or change baseline rows.
- Region selectors, storage/escape classifications, counts and flags are values,
  not node positions. Preserve them exactly.

Use an explicit discriminant for the two last-use representations. Validate it
against the value ordinal, so missing or mismatched position metadata is a miss.
Validate complete saved vector lengths, owners and every local node/span before
importing any result. Publish no partial function plan. Compiler-owned history is
the only accepted origin; arbitrary serialized records cannot approve memory
placement. No C text parsing or plan reconstruction from report text is allowed.

Lookup must use the existing indexed declaration match and a direct plan slot map;
do not scan all prior function plans for each declaration. Insertion, deletion,
reordering and relocation establish current owners. A deleted plan is absent from
the next published history and is not resurrected on later reinsertion.

### Storage and failure bounds

The query uses the retained candidate's existing canonical-node budget in
1..1,000,000. Independently bound saved function entries, value entries, allocation
entries and destruction entries by that budget. Check remaining capacity before
adding any vector size. Decline the complete optional new planning history on a
storage admission failure while preserving ordinary planning and accepted source.
The containing service's existing explicit candidate resource limits still apply.
These bounds describe retained records, not peak RSS or physical epoch reclamation.

Missing history, invalid owner/range/position tags, stale epochs or compiler
configuration, unavailable mappings and incomplete dependencies are misses. The
ordinary planner runs for the affected function. Optional plan misses do not
invalidate otherwise complete typing history. No failed candidate replaces the
last-good cache. Allocation failure retains the existing status-71 behavior.

## Compiler and runtime design

Keep the implementation in production SLIM. `retained.Analyzed` carries the
current memory plan as well as the current typing view and candidate history.
The retained operation plans only after the complete typing issue vector is empty,
using the already computed update map. The ordinary path calls the same memory
planner after typing as before. `check` consumes the supplied retained plan and
does not call `memory.analyze` a second time. Its later fork/termination checks
remain required before candidate publication.

Successful candidate publication moves planning history with typing history.
Current `memory.Plan` remains a private flat working view for existing codegen and
reports; imports explicitly reconstruct its current positions. There is no second
planner, new runtime primitive, dependency or production Rust semantic operation.
Saved typing row/link payload budgets from RFC-0131 remain unchanged.

## Compatibility and migration

Preserve source acceptance, diagnostics, every memory-plan field, native analysis,
resource rows and generated C. This is retained execution of the existing planning
algorithm, not a new ownership proof or liveness precision feature. The unusual
legacy conservative last-use marker is preserved, not silently repaired here.
Public actual-host identity binding remains a separate required M1 obligation;
existing internal session configuration eligibility continues to guard history.

## Diagnostics and failure cases

No new source diagnostic. Optional misses invoke ordinary planning. Wrong current
source, rejected checking and incomplete mandatory service budgets retain their
existing failure behavior. A valid plan cannot make rejected source acceptable or
authorize an executable for a failed revision.

## Performance and complexity

Native observation must count actual function-plan construction separately from
typing, parser, range/parallel analysis, generation and import. Instrument the
existing producer and the import operation; do not infer execution from a reported
cache hit. Gate no-change snapshot work and local body-edit planning on geometric
multi-module fixtures, while retaining linear source/map/import costs explicitly.
Measure cold/update latency and stored record sizes. No speedup, scan-free update,
physical reclamation or performance-budget relaxation is presumed.

## Alternatives and drawbacks

Source equality alone misses referenced layouts and declared effect ceilings.
Recomputing plans and reporting estimated reuse does not implement M1 retention.
Keeping old absolute node/byte positions creates stale resource and emitter inputs.
A second memory planner would duplicate semantic authority. Saved plans add memory
and copying costs, which must be measured alongside avoided bounded scans.

## Test and acceptance plan

Compare every scalar and nested vector field with clean planning for the complete
accepted conformance/native corpus and multi-file update matrix. Cover body edits,
effect/interface/ownership changes, transitive layout edits, insertion/deletion/
reinsertion/reordering, relocation, comments and CRLF. Cross 63/64/65 planned values
and compare both last-use representations after source and node shifts. Preserve
complete C and native resource/analysis baselines.

Withdraw saved plan entries, slot maps and position metadata independently; test
wrong epochs/owners, invalid local positions, byte limits and storage admission
boundaries. Verify misses produce identical clean results and failed updates retain
last-good history. Observe actual planning and import work through 4,000 functions,
including interface dependents and independent functions. Run ordinary/ASan/UBSan
differentials, a bounded allocation-fault campaign, all required compiler checkpoint
gates and full parent M1 closure separately.

## Ratings and evidence

Analysis +2 specifies complete planner dependencies and typed relocation of two
different legacy position domains. Other ratings remain zero pending measurements.
Weighted score 15. No improved ownership safety or compile latency is inferred.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit full M1 goal.
This is approval of the stated internal contract, not external review, completed
implementation or permission to relax compatibility/performance gates.

## Implementation

Pending. The current producer and dependency reads have been inspected at 1fca486.
The parent M1 goal remains incomplete for flow ownership/loans, public sessions,
retained global analyses/C fragments/backend artifacts and complete release closure.

## Removal and supersession

Preserve sole planner authority, exact current plans and resource rows, all typed
owner/node/span checks, complete dependencies, optional misses, transactional
publication, observed work and measured cost evidence.
