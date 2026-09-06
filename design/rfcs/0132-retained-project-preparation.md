# RFC-0132: Retained project preparation

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

Connect project preparation to the production retained checker through one shared
project validation and source-mapped diagnostic path. Return the prepared checked
project, eligible immutable typing history and actual function-query work as one
attempt. This is a prerequisite for M1's transactional project session, not its
public protocol or lifecycle completion.

## Motivation

RFC-0130's retained checker accepts canonical flattened source. Project preparation
currently unconditionally calls ordinary checking after loading, validating and
flattening modules. A session must not bypass module identity, import/export,
entry-point, cycle, manifest or source-origin checks to access retained inference.
It also needs failed preparation to return no publishable history.

## Guide-level explanation

Both ordinary and retained preparation use the same project operation. Retained
preparation supplies the prior compiler-owned typing history, a new revision and
the existing token limit. The loader validates the current complete project before
checking its canonical flattened source. On a hit the normal retained operation
imports checked facts through typed maps. On a miss it calls the sole function
checker. Project diagnostics continue to name the current original module/span.

## Reference-level specification

Add ProjectAttempt containing PreparedProject, retained.Cache and retained.Work.
One internal preparation implementation takes the optional-retention policy,
previous history, requested revision and token budget. Existing ordinary entry
points use the no-retention policy and preserve their return type and output.
A retained project entry point returns ProjectAttempt for a future session owner.
Do not duplicate the manifest/module validation sequence or checker semantics.

Successful preparation preserves flattened source, tokens, origins, manifest
source/tokens, all facts, layout order, issues and memory plans. Generated C from
the prepared result must equal clean preparation byte for byte. Every existing
project validation runs against current input before any retained typing can be
used. In particular, an unchanged flattened body cannot bypass changed export or
import visibility. Module/path relocation uses current provenance; old byte
positions cannot supply diagnostics for new files.

Failures before typing return invalid empty history and executed=-1 (unknown
function-query count), reused=0, imported=0. Checker failures follow the existing
retained Attempt rules. Invalid/exhausted token limits and stale revisions retain
the existing explicit miss/publication behavior. The caller retains its previous
good attempt; this function never mutates it. Tests exercise failure followed by
recovery against the same prior good history.

The existing project flattener remains the sole current project representation
adapter. Its qualified names and current validation determine the checked source;
the retained index matches complete flattened declaration keys with byte equality.
Declaration source keys must include the complete source interval up to the next
declaration start, or the end of the owning module source for its last declaration,
with trailing separator whitespace removed. Trim only spaces, tabs, CR and LF;
comments remain content. This preserves reuse under declaration reordering without
ignoring any expression syntax or using another parser.
A synthetic closing canonical node can carry only a callee/operator anchor; its
end is not an enclosing source boundary. Use one derived declaration-boundary
helper for both retained typing and source snapshot maps. Keep all parser token
fields and diagnostics unchanged. This fixes the discovered nested-literal edit
that incorrectly reused an old main body. Cross these boundaries with permanent
literal, string, operator, member and nested-call edits, including rejected updates.
No new parser, separately accepted IR, qualification rule or semantic fallback is
introduced. Per-module parsed retention and full logical module-key session records
remain required by the parent M1 contract.

## Compiler and runtime design

Implement in production project/check SLIM modules. Use the existing retained and ordinary checker wrappers, expose the existing
uncached-attempt constructor, and factor one preparation implementation.
Keep ordinary wrappers and current public commands; do not change the historical
session-estimate output in this step. No runtime ABI or dependency change.

## Compatibility and migration

Preserve current command output, rejection status, source-mapped diagnostics,
native analysis/resource rows, generated C and every existing gate. This internal
integration does not claim retained module parsing, persistent service transactions,
retained global analysis or C fragments. Those remain M1 implementation work.

## Diagnostics and failure cases

Missing files, malformed manifests/modules, import/export changes, invalid entry
points, cycles, duplicate declarations and semantic rejection use the same checked
project diagnostics. No failure creates valid history. Original accepted history
remains usable after rejection, capacity misses and project relocation.

## Performance and complexity

Default preparation should keep its existing approximately linear work and avoid
allocating optional snapshots. Measure default project checking before/after.
Retained preparation still loads/parses modules, flattens/reparses and checks global
facts. Count actual function-check calls separately from these operations. No
incremental latency benefit is presumed merely from inference reuse.

## Alternatives and drawbacks

A separate retained project loader would risk divergent validation and diagnostics.
Calling an ordinary full check before a retained check would duplicate work and
hide an ineffective cache. Reusing flattened history before current project
validation would miss visibility and provenance dependencies.

## Test and acceptance plan

Compare ordinary and retained prepared fields and emitted C on accepted projects,
including native applications wrapped in manifests and multi-module forward
layouts. Compare full rejection status/output for malformed and semantic cases.
Exercise body/interface/layout/effect/ownership-mode/import/export/entry changes,
module/declaration insertion/deletion/renaming/reordering, relocation, and recovery.
Use native-observed checker calls and geometric project families to establish
actual inference reuse, preserving existing imported-node definitions and bounds.
Run sanitizer and bounded allocation-fault probes plus required checkpoint gates.

## Ratings and evidence

Analysis +2, score 15, for production project-level access to checked retained
results and actual work. Compile remains zero until broader performance evidence
justifies a claim. Record seed size, default cost and the remaining M1 obligations.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit M1 goal. This is
not independent external review. No language compatibility or performance gate is
relaxed, and the parent M1 scope remains unchanged.

## Implementation

Implemented in the shared `project.prepare_loaded_project` and preparation path,
with a ProjectAttempt result for retained callers. Complete declaration source
extents are derived once in syntax and used by both retained typing and query
source snapshots. The prior synthetic-closing-node source-key defect is covered
by permanent accepted and rejected nested-content mutations.

The [progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
records 94 project differentials, all prepared/memory fields and C equality,
module/visibility/ownership/layout/recovery cases, geometric actual work through
4,000 helpers, 2,048 project fault ordinals, the existing source campaign and all
required checkpoint gates. Default project checks have paired 4,000-helper medians
20.940/20.705 ms; retained construction plus update remains slower than two clean
preparations. Analysis +2 and score 15 are retained without a latency claim.

Bootstrap is 3,873,212 C bytes, SHA-256
`252560991d434a503c9e1c60a016f5fdefec6cf7633d3672cc9d817159c766de`.
The public persistent session, parsed-source retention, cached analysis/C and
service limits remain parent M1 obligations.

## Removal and supersession

A future session must use this shared authoritative preparation or its single
replacement. It must preserve complete current project validation, provenance,
typed checked reuse, failure recovery, actual work evidence and all durable gates.
