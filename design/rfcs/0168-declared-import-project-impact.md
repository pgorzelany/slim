# RFC-0168: Declared-import project impact data

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, under delegated ordinary-library direction
Created: 2026-10-03
DecisionDate: 2026-10-03
Approver: project-maintainer
Kind: compatibility
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Add an experimental ordinary SLIM `project-impact OLD-CATALOG OLD-GRAPH
NEW-CATALOG NEW-GRAPH` application. Compose existing catalog reconciliation and
the workplan graph loader to report changed supplied catalog records, changed
dependency-list owners, both reverse declared-import closures, and current file
candidates. This turns the checked-project snapshot tools into a useful edit
workflow without introducing compiler semantics or a language operation.

## Motivation

The report is exact for admitted supplied metadata and declared edges. Digest
equality does not prove equal source bytes across hash collisions, and shaped
metadata does not prove source acceptance. A matching trusted successful
RFC-0165 producer invocation and adapter receipt remain separate authority.
The tool never rereads source paths, parses SLIM, checks application invariants,
starts a build, or authorizes incremental compiler acceptance.

## Guide-level explanation

Retain before and after catalog/graph pairs from stopped-writer project snapshots.
Run the application with the four files in the order above. A source-only edit
normally changes a module's path/digest/weight record, and the report selects
that module plus its declared dependents in both snapshots. A changed manifest
record selects every current module conservatively: entry/export rules are not
represented by the graph. Removed roots remain visible in the old closure;
current candidates follow the relations below. A path change is a record
change; a module rename is a removal and an addition.

## Reference-level specification

Read and validate old catalog, old graph plus its association, new catalog, then
new graph plus its association. Preserve existing loader diagnostics and their
source positions. Before reading the next file, finish the current stage.
Prepare every result and its complete byte report before printing once.

Use `catalog_data.load(..., 4096)`, `catalog_reconcile.prepare(..., 8192,
8388608)`, and `workplan_load.load(..., 4095, 65536)` unchanged. Do not call
`workplan_schedule.plan`: the loader accepts nonreciprocal cycles, and marking
on enqueue makes their closure finite. No selfhost API or existing component
signature, behavior, effect or admission is changed.

Each catalog has exactly one `@project` row, guaranteed unique by the catalog
loader; the graph has no such key. Every other catalog key and graph task name
matches exactly once, and their weights agree. Keys use the existing workplan
ASCII name alphabet and 1..64 byte bound. A catalog value is 1..256 non-NUL path
bytes, one NUL, and exactly 64 lowercase hexadecimal digest bytes. The reserved
row's path is exactly `slim.project`. Paths are labels: no new manifest/path
parser, normalization, realpath containment or filesystem access is introduced.
Weights are 0..1048576, and their sum, including `@project`, is at most 4194304
per snapshot. After catalog loading, validate each existing row's shape then
the running sum in lexical catalog order; only after all existing rows pass
check reserved-row presence. An existing malformed row or excess sum therefore
wins over a missing `@project` row. The association merge reports its first
mismatching lexical key. Graph costs equal
these byte weights; they are not estimates of compilation work.

The four input files are each at most 1048576 bytes after ordinary whole-file
read; this does not give preread physical bounds. Each graph has at most 4095
tasks and 65536 edges. Existing framing, duplicate, unknown/self/repeated-edge
checks remain authoritative. No new acyclicity condition is added.

Let D be the module catalog additions, removals and modifications, comparing the
supplied weight and complete value bytes exactly. Let G be matched module names
whose complete dependency-list byte fields differ between graphs. Even an
order-only list change is in G; this preserves a conservative result for
independently supplied graphs whose catalogs and `@project` rows happen to agree.
Graph-only added/removed task names cannot pass each snapshot's association
unless their catalog names also change. Old roots are D's present old names
union G; new roots are D's present new names union G. The old/new closures are
the least sets containing those roots and every dependent of a member in the
respective graph. Current candidates are the new closure union the old closure's
names that still exist in the new snapshot. If `@project` weight/value changed,
current candidates are all new names. Still report the ordinary roots and both
closures; the manifest flag explains the wider selection.

The surviving old closure is a subset of the new closure in this admitted
domain. A surviving original root is already a new root. Along an old closure
edge, an unchanged surviving owner's dependency list preserves that edge; a
changed list makes the surviving owner a G root. A removed prerequisite also
forces its surviving owner into G because the new graph resolves every edge.
Induction along an old root-to-dependent path proves the subset. The union
remains the selection definition; implementation may select the new closure
alone when the manifest is unchanged. Report the old closure independently.

### Successful report

The successful report is one canonical netstring sequence, followed by EOF:

1. `slim-project-impact-1`, manifest-changed `0` or `1`, the old and new reserved
   catalog records as nested canonical catalog triples.
2. Count of D, then classification (`added`, `removed`, `modified`), name, old
   nested catalog triple (empty when absent), and new nested triple for each D.
3. Count of G, then name, old exact dependency field and new exact dependency
   field for each G.
4. Old-root count and names, new-root count and names, old-closure count and names,
   new-closure count and names, in that order.
5. Current-candidate count and each current nested catalog triple.

Every set/list uses strict lexical byte order. Nested triples contain key,
canonical decimal weight and original value bytes. Fields retain the existing
1 MiB netstring payload bound. Cap the complete report at 8388608 bytes, checking
its exact size and all additions before report allocation/emission. At most
8190 module changes, 4095 changed graph owners and 4095 names per closure/root
or candidate list are reachable. The existing reconcile change budget also
counts the shared reserved row and admits at most 8191 changes here.

## Compiler and runtime design

New ordinary application components live under `library/applications/project_impact/`
with a `library/project-impact.project` entry manifest. Preparation/closure
helpers explicitly declare `alloc, partial`; only the CLI/file-reading/emitter has `io`.
Reuse existing indexes and `Graph.heads`/`Edge.dependent` reverse adjacency;
do not rebuild compiler dependency semantics or introduce a new index primitive.
Prepare helpers accept only loader-built, successfully shape/association-validated,
unmodified catalogs and graphs. Public records are not opaque: forged spans,
ordinals, heads or edges retain checked partiality and confer no guarantee.

Build one root mark vector per graph, then traverse its existing reverse edges
with a vector queue. Mark before enqueue, enqueue each node once, and pop each
queued node once. Each followed edge reaches a loader-valid ordinal. At most N
pushes and pops and E edge visits occur per graph, including cycles. For each
snapshot queue pushes = queue pops = closure cardinality; followed edges = the
number of declared edges whose prerequisite is in that closure. Terminal link
checks add one per popped node. These invariants prove local termination and
prevent duplicate queue expansion. Iterate sorted indexes to report, independently
of queue or linked-edge order. This is ordinary data composition, not a direct
rewrite of checked SLIM or a new semantic program representation.

## Compatibility and migration

The application has one four-file invocation and no compiler compatibility
change. Existing catalog/workplan formats, public helpers and diagnostics stay
unchanged. Adoption adds an experimental ordinary library application; it does
not replace compiler checking or change an existing application's behavior.

## Diagnostics and failure cases

Errors are `error CODE in STAGE at POSITION\n`, with empty stderr for ordinary
validation. Invocation error 2 in `arguments` at 0 exits 64; read error 1 at 0
in the corresponding input stage exits 66. Loader failures retain their existing
codes and offsets and exit 65. New shape/sum/reserved-row error 40 names the
offending catalog key's payload start (0 when the reserved row is missing).
Association error 41 names the first unmatched key's source stage/offset, or
the graph key for a weight mismatch. Stages are `before-catalog`, `before-graph`,
`after-catalog`, `after-graph`; prepare/output error 42 in `impact` is at 0.
Helper output limits outside 0..8388608 or a report larger than its supplied
limit receive 42. An internal size/emission disagreement is error 43 in `impact`
at 0. No successful report prefix is printed before complete preparation.
Allocation failure remains exit 71. Host stdout failure may expose a prefix;
no transactional I/O or atomic four-file capture is claimed.

## Performance and complexity

Total work is O(B + (N+E) log(N+1)), including existing catalog/index construction
and graph name resolution; names/paths have the fixed bounds above. Do not label
the inherited E log N loader work linear. Additional closure work is O(N+E),
with O(N) marks/queue and bounded output storage. No default compiler pass changes.
Logical caps do not establish RSS, libc allocation, host I/O, or whole compiler
time bounds. No model productivity or minimal semantic recheck claim is made.

Permanent geometric fixtures use five N values 16,32,64,128,256 for chains,
fan-out, nonreciprocal cycles, shared-dependency graphs and 64-byte prefix names.
Observe exact new-helper enqueue/pop/edge/link work, compare it with the finite
data model, and gate endpoint closure exponent <=1.15 against N+E+1. Counter
operands/increments/sums/products are checked in 0..1000000000; retain cap-1,
cap, cap+1 and UINT64_MAX controls. Existing budgets/gates are unchanged.

## Alternatives and drawbacks

Rechecking every source conservatively remains available through the compiler.
Using only the new graph omits the explanatory old closure; ignoring changed
graph lists trusts an association it cannot prove; scheduling rejects allowed
cycles. A new compiler incremental API or Python SLIM parser adds unnecessary
permanent semantics. The library approach has the costs and limited authority
stated above.

## Test and acceptance plan

The ignored prospective `project-impact-oracle.py` constructs only fixed literal
record/edge/source data, canonical frames and a mathematical least closure. It
does not parse arbitrary catalog/graph input, SLIM or manifests, execute children,
or decide source acceptance. Explicit small-case claims cross-check its model.
Every positive row independently asserts surviving old closure is a subset of
new closure and the union equals new closure. Include both reciprocal two-node
and nonreciprocal cycles in the graph loader's domain.
Seal its source, materialized inputs/expected bytes, bounds and pins before any
implementation/native observation. The campaign is fixed; retain failures and
unknowns without retries or raised budgets.

Run every finite positive/negative diagnostic row through the ordinary and
address/undefined-sanitized production SLIM-built application. Cross N=4095/4096,
E=65536/65537, input extent, name/path/hash/weight/aggregate admission, reserved
and key/weight association in both snapshots, malformed/repeated/self/unknown
graph data, empty graphs, deletion mapping, graph-only changes and cycles.
Fixed argument-count controls require error 2 at 0. Exercise helper output
limits at exact report-size-1/size/size+1, not an invented crossing of the default
8 MiB cap. Keep parsed-node counts and unreachable whole-report caps unknown.
Fixed harness FIFOs with no writer replace the next input after invalid old
catalog, invalid old graph, invalid old association and invalid new catalog.
Catalog guards cover both loader and new shape failures. Literal mixed cases
require existing-row shape/sum diagnostics before missing reserved-row absence.
Require the preceding exact diagnostic under ordinary/sanitized builds; a
blocked read or timeout fails the ordering check. These guards add test coverage
to the stated CLI ordering, not new file-reading semantics.

Two real fixed before/after source bundles use the compiler project and existing
catalog application. Their 50 source paths, literal declarations and exact
manifest-byte pins stay fixed. Read module bodies as bounded opaque bytes and
record their complete current hashes in each fresh freeze; ordinary body edits
do not require permanent historical body hashes or copied historical sources.
The first hold may compare the initial source hashes separately in its acceptance
record. Never change/rebind an existing held campaign. Stopped writers and source
byte comparisons before and after publication retain the campaign's identity;
no atomic filesystem capture is claimed.

Append one newline to one named source while preserving
their manifest and declared imports. Compare trusted successful producer outputs,
adapter receipts and the impact report with independently materialized source
bytes and declared data. This demonstrates that bounded workflow only; checker
acceptance is established by actual production invocations, separately from this
tool's accepted metadata. Record the producer source/control-compiler distinction.
An exact manifest pin mismatch or source bound failure stops data preparation;
a matching manifest does not establish that its current module bodies are valid
SLIM. Matching native producer/checker invocations remain required.

Focused native campaign is explicitly bounded: 900 seconds overall, <=60 seconds
per child, live stdout/stderr caps 8 MiB/256 KiB (16 MiB stdout only for compiler
C emission), sequential groups killed/reaped on all exits, fresh ignored outputs,
source/compiler/CC/oracle/fixture pins before and after, UTC and monotonic whole
timing. No arbitrary recorded executable, new dependency or semantic fallback.
Full repository/release gates and matching identities remain required before
adoption. An inexpensive data freeze is separate from native acceptance.

## Ratings and evidence

All ratings are neutral pending measurements; Score 0 is not a
claim of zero cost or measured utility.

## Decision

Accepted by the coordinating maintainer under the direct user delegation to
choose the overnight roadmap. Independent static review and a source-pinned
data hold precede implementation; no native result or source acceptance is
inferred from this decision.

## Implementation

The seven ordinary application modules and independent fixed verifier are
implemented. Canonical formatting and 433 ordinary/sanitized gates plus ten
geometric rows pass on the frozen 124-case domain. Actual checked compiler and
catalog source snapshots establish the separate producer workflow and candidate
scope facts in the [current record](../../benchmarks/results/project-impact-current.json).
The corpus and permanent verifier pass the full source-bound repository,
reproducible release, clean-install and website gate at checkpoint `915f836`.
The current record retains focused source identities and the separate full
integration identity; routine attempts remain in ignored build storage.

## Removal and supersession

Remove/supersede the experiment if exact metadata/closure parity, bounded work,
diagnostics, source authority separation or permanent repository gates fail;
do not weaken a gate or call candidate files a semantic recheck plan.
