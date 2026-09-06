# RFC-0130: Retained function typing

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
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Retain successful per-function typing results in compiler-owned memory and import
them into a freshly linked canonical revision through explicit typed relocation.
Execute the same isolated production checker for misses. This implements actual
type/ownership inference reuse; other checker phases remain current-revision work.

## Motivation

RFC-0127 isolated inference, but raw fact forms and packed local links cannot cross
revisions. Declaration-local equality alone also misses changed callee signatures
and transitive aggregate copyability. A safe reuse boundary must solve both.

## Guide-level explanation

A compiler-owned prior successful result may supply unchanged function facts.
Changed functions or changed interface dependencies execute the ordinary checker.
Rejected candidates cannot become accepted history. Source loading, parsing,
linking, declaration checks, termination, memory, analysis and generation are not
claimed as hits by this typing layer.

## Reference-level specification

Retained records carry revision-owned declaration/node identities, nominal type
and binding identities, original source bytes and exact declaration keys. For this
initial adapter, one canonical input has one file identity; project flattening
and original-file provenance remain the project layer's responsibility.

Match full module/declaration names and kind using an indexed namespace, not hashes
alone. Exact body bytes and canonical shape must agree for a function hit. Capture
pre-inference name-resolution outcomes, including missing links and local shadowing.
Compare those outcomes in the new revision, so adding/removing a name cannot change
resolution while preserving a hit. Duplicate or incomplete owners cannot be reused.

Store fact forms as nominal TypeIds naming canonical nodes. Store binding slots as
nominal BindingIds owned by the function and distinguish them from source links.
Decode packed local links into a binding declaration node, optional type node and
mode. Re-encode only after checked relocation to the new token count. Missing,
stale, incomplete or out-of-range references produce a miss before any import.

A relocation maps an unchanged complete declaration, or an unchanged function
interface prefix only. Interface-only maps cannot address a changed body. Both
require exact bytes and canonical shape for their mapped extent, full keys, a
current valid revision in the same epoch, and strictly increasing revision serials.
No old handle directly indexes the new view. This extends RFC-0126's whole-body
map with an explicitly narrower checked interface map.

Build reverse dependency adjacency from resolved pre-inference declaration links.
Data declaration bodies are interfaces; function headers are interfaces. Propagate
changed interfaces through this graph once per declaration/edge. A function body
change invalidates its own inference, without being misreported as an interface
change. Aggregate interfaces transitively determine parameter/field copyability;
callee interfaces determine types, modes and capability ceilings. Function typing
does not consume callee body analysis. The existing whole-program termination,
range and memory phases still execute and cannot consume stale body-derived facts.

The dependency completeness argument is specific to current `typing.check_function`:
external reads resolve through linked declared types, aggregate field/case lists,
or callee parameter/result declarations. Follow their declared-type edges
transitively. Builtin rules and inference implementation are fixed by the running
compiler. Negative name results are compared explicitly. New external semantic
reads require updating this boundary and permanent invalidation tests.

Only completely successful full checks publish eligible snapshots. Keep prior
history immutable during candidate checking. A rejected update returns its own
current diagnostics and an ineligible candidate, leaving the caller's prior good
snapshot intact. Allocation failure retains the existing status 71 and cannot
publish. Arbitrary serialized records are never accepted semantic authority.

The optional entry point takes an explicit canonical-token limit in 1..1,000,000
per snapshot, including checked arithmetic for encoded links. Invalid limits or
inputs exceeding the selected limit run the ordinary checker and report an
explicit capacity miss; no language-size ceiling is added. Test actual snapshots
at their exact token count and one below, and cross the configuration endpoints.
This storage eligibility limit does not change semantic analysis budgets.
This is not yet the bounded persistent service lifecycle of RFC-0124. It does not
silently describe unavailable retained storage as reuse.

## Compiler and runtime design

Implement in production SLIM. Share the ordinary checker's setup and isolated
function checker. A retained analysis is orchestration and checked data relocation,
not another type or ownership implementation. Keep the ordinary entry point and
new internal retained entry point on the same complete validation/termination path.
No public command or persisted format is introduced by this child.

## Compatibility and migration

Preserve acceptance, diagnostics and current spans, facts, token links, memory
plans and analysis. Clean and retained generation must be byte-identical. The
implementation also repairs missing unused-variable suppression for enum payload
bindings: emit the same `(void)` C suppression already used for ordinary bindings.
This permits checked source with unused payloads to compile under strict native
flags, without changing evaluation or runtime behavior. Record its intentional
C text differences separately from retained-versus-clean equivalence. Clean checking need not allocate retained payloads.
The estimate-only session command remains honestly labelled until its later command
migration. Ownership orchestration over RFC-0129 flow remains a parent obligation.

## Diagnostics and failure cases

Missing history, wrong revisions, capacity limits, source/shape changes, changed
resolution or dependencies, incomplete maps and unsuccessful prior checks are
explicit misses. Recompute using normal checking; never approve a partial import.
Failed current checks retain ordinary error codes and spans.

## Performance and complexity

Index and relocate linearly in source and dependency size. Reverse adjacency avoids
per-provider whole-edge scans. Record executed and reused functions separately from
node remapping, linking and global checks. Compare native observations with reported
work and preserve all existing gates. Measure snapshot storage and seed growth.

## Alternatives and drawbacks

Raw-vector reuse retains stale indices. Rechecking everything preserves correctness
but is not incremental inference. A universal body dependency destroys locality.
Typed records and explicit maps add memory and source scanning; this cost is visible.
Whole-program analysis retention and reclamation remain separate required work.

## Test and acceptance plan

Compare clean and retained acceptance, diagnostics, every fact/link, complete native
analysis and generated C over unchanged, body/interface/type/layout/mode/effect edits,
insertions, deletions, renames, reorderings, bad candidates and recovery. Cross stale
identity and mapping boundaries. Observe actual isolated checker calls and no-change
hits, geometric locality, sanitizer and allocation-failure behavior. Run all required
checkpoint gates before marking implementation complete.

## Ratings and evidence

Analysis +2, other dimensions zero, score 15. Required default gates pass; the
new internal retained operation has measured setup/copying costs and is not a
latency improvement. Do not infer a speed improvement from a hit count alone.

## Decision

Accepted under the maintainer's RFC-0112 delegation and explicit M1 completion goal.
This is not independent external review. The full parent contract remains required.

## Implementation

Implemented in `retained.slim` and the shared `check_source_retained` path.
Bootstrap reaches 3,845,929 C bytes, SHA-256
`5f232902750c04e1a58d76b399ad3030a551f7f947167de5271cf4c1f4c64c2a`.
The [dated progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
records native-observed zero-change reuse, edit/dependency/identity/capacity/metadata
boundaries, 94 accepted files, 193 rejected-file parity checks, 4,000-function
geometric evidence, 512 fault ordinals, all required checkpoint gates, storage
cost and paired timings. All 10 unit and 66 integration tests pass.

The additional snapshot setup/copying cost makes the measured cold-plus-update
operation slower than two clean checks. No default gate is relaxed, and no latency
improvement is claimed. This child completes the internal typing reuse boundary;
the parent still requires public integration, lower retained overhead, full service
transactions, ownership/flow migration and retained analysis/emission.

## Removal and supersession

Remove transitional adapters only after their complete reuse, relocation, failure
and dependency guarantees are preserved. Never replace the sole semantic checker.
