# RFC-0142: Retained parameter-input queries

Status: accepted
Implementation: complete
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

Retain the range analyzer's recurrence-invariant masks, linked call-argument
topology and complete parameter-input transfer results. Preserve the existing
five function passes, four transfer rounds, two input modes and sole scalar
merge operation. Source-equal bodies alone cannot authorize a transfer hit.
Public sessions, parallel analysis retention, native caching and M1 closure
remain separate obligations.

## Motivation

RFC-0138 retains function range results, but every changed candidate still
collects recurrence invariants and scans the entire canonical token vector twice
after each of the first four passes. Each scan finds linked user calls and merges
their argument facts into callee parameter slots. Many of these input queries
remain equal across passes and across edits to unrelated functions.

The actual producer is `ranges.merge_parameter_fact`. It reads the current
aggregate, all fields of an argument fact and one recurrence-invariant Boolean.
The second, entry-fact mode supplies true for that Boolean. A function's own body
is insufficient as a key: another caller can change its input range or introduce
an unknown contribution without changing the callee's source.

## Guide-level explanation

Discover the complete current set of calls through the existing checked links.
For each callee, compare every contributing call argument's current fact, in
source and argument order, together with the current parameter identities and
recurrence invariants. An identical query can reuse both complete input arrays
for that callee. Changed or missing inputs run the ordinary merge operation.

An empty caller set is a complete discovered set, not an assumption based on an
empty or damaged cache. A changed caller can invalidate a callee transfer even
when that caller's interface and the callee's body are unchanged. A recurrence
change can invalidate the invariant-mode result. Reordering preserves the
ordinary ordered computation; no commutativity assumption grants reuse.

## Reference-level specification

### Sole producers and schedule

Keep `ranges.collect_function_invariants` as the producer of a function's
parameter mask and `ranges.merge_parameter_fact` as the sole scalar transfer.
Call discovery uses exactly the current scan's canonical call classification,
linked callee, ordered parameters and ordered arguments. Factor reusable
structural traversal if needed; do not introduce another parser or resolver.

The retained path may discover ordered call topology once per candidate and
import eligible unchanged caller topology from the preceding good snapshot.
Build indexed reverse adjacency from that complete topology. Do not rescan the
whole call list for each callee. Every discovered call argument belongs to one
callee query, including arguments whose facts are unknown or nonnumeric.

Retain all five lexical function passes and all four inter-pass transfers.
Each transfer reads the complete just-finished output fact vector. Initialize
both input arrays to their existing default facts and write only parameter
bindings. Clear the output scratch only after both input modes are complete.
Do not converge early or change the shared 64-refinement limit, recurrence
proof domain, report limits, or conservative results.

A single ordered contribution walk may compute both input modes. For each
contribution, call the existing merge operation once with the current invariant
and once with true, writing to distinct parameter and entry arrays. Within each
array this preserves the original call/argument order. The merge is pure and
total inside its existing checked fact domain; the two arrays are distinct and
neither is read by the other mode. This is the local order-preservation argument
for combining the two scans. There is no shared precision budget in this merge.
Do not extend that argument to other analysis operations without evidence.

### Structure eligibility and identities

Structural history belongs to the existing checked declaration owners. An
unchanged function may import its invariant mask and call topology only through
the complete source/shape/link and transitive-interface transition already
validated for retained typing. Changed callee parameter order, types, modes,
names or resolution must invalidate affected structure. A prior raw global
token position is never a current identity.

Record parameter, call and argument positions as declaration-local canonical
ordinals. Cross-function references also name the complete typed declaration
owner. Validate and translate each owner through the exact current declaration
map before using an ordinal. Removed owners, wrong epochs, stale revisions,
missing mappings, wrong node families, incomplete slices and changed source
extents cause misses. The original checked source remains available as the
authority for those links. No imported tokens, facts or proof text from an
external file can approve a source program.

Topology is complete only after processing every relevant checked function.
The ordinary scan traverses canonical call nodes; declaration data contains no
executable default expressions. Tests must cross that boundary rather than
infer completeness from a function count. Capacity exhaustion or incomplete
topology uses the ordinary current scan; it never supplies an empty caller set.

### Complete transfer query

The query unit is one callee's complete pair of input arrays. Its key contains:

1. The callee's validated current owner and complete ordered parameter bindings.
2. Every parameter's current recurrence-invariant Boolean.
3. The complete ordered incoming call/argument list, with mapped caller owner,
   call ordinal, argument ordinal and destination parameter ordinal.
4. All six fields of each current contributing `ranges.Fact`: analyzed,
   lower-known, lower, upper-known, upper and total.

Compare complete keys after identity translation. Do not substitute a source
hash, primary blocker, call count, inferred callee interface, or aggregate range
for the actual contribution list. Unknown contributions remain present. No
contributors leaves the exact existing default in every parameter slot.

The result contains both final facts for every ordered parameter binding.
Pass number is not an input to the merge. Search current-candidate versions
first, then eligible previous-snapshot versions, with at most four distinct
versions per callee across one transfer lookup. Each snapshot may retain up to
four versions. After a current miss, compare the previous chain only if its
complete length fits the remaining lookup allowance; otherwise recompute. Never
accept a matching prefix of an over-budget chain. Preserve the ordinary
function-range query's separate incoming-refinement-count key: this RFC does not weaken RFC-0138's dependencies.
Budget effects that change an argument fact therefore change this transfer key.

### Internal API and records

Implement the orchestration in `retained`, avoiding a cycle from `ranges` back
to the retained checker. The internal operations are:

- `input_start(source, tokens, old, current, mapping, limit) -> InputState`:
  validate optional prior structure, construct the complete current topology,
  parameter map and inverse adjacency, and initialize candidate-owned pools.
- `input_transfer(source, tokens, facts, current, @state, @parameters, @entries)`:
  fill both complete input arrays by validated import or the sole merge producer.
  Invoke it once after each of the first four function passes.
- `input_finish(^state) -> InputHistory`: expose only complete, admitted,
  compiler-owned history for a successful enclosing candidate.

The source parameters use the existing `Bytes`, `Vec[syntax.Token]`,
`Vec[ranges.Fact]`, `Cache`, `Index` and `Update` types. `limit` is I64. The
operations use existing allocation and checked-partial effects. No new source
effect or runtime operation is introduced.

`InputHistory` contains revision-owned function entries and contiguous pools:
invariant rows, call-argument rows, transfer entries, complete transfer-key rows
and paired parameter-result rows. A function entry records owner, source extent
and its invariant/argument slices. An argument row records callee owner and
caller-local call/argument ordinals plus the callee-local parameter ordinal.
A transfer entry records callee owner, key/result slices and a bounded previous
version link. Its key rows include caller identity and the complete compact
argument fact. Result rows include the parameter ordinal, invariant bit and both
complete output facts. Use the existing lossless four-bit/two-I64 fact encoding.

Direct declaration slot/head maps are bounded by the checked declaration count.
Keep the large pool headers behind a zero-or-one owned history vector in the
retained cache, as with existing range history. Zero means missing; a count other
than one is invalid. Current mutable scratch and published immutable history
have separate ownership. No mutable scratch alias escapes a transfer.

### Admission, integrity and failure

The new optional history has a separate checked record limit in 1..1,000,000,
initially supplied from the caller's existing retained-node limit. Charge the
sum of function entries, invariant rows, call-argument rows, transfer entries,
key rows and paired-result rows before publishing their slices. This does not
increase or repurpose RFC-0138's existing range-history record allowance; that
history retains its own unchanged limit. Report the additional storage cost.
Head maps and temporary adjacency are independently bounded by current checked
declarations, parameters and admitted call-argument rows.

Check subtraction-based framing, nonnegative counts, strictly bounded version
links, every owner/local ordinal and every compact fact domain before any import.
An integrity seal must cover all recorded fields, ordering and pool lengths,
with checked bounded arithmetic. Its role is detecting tested accidental damage
to compiler-owned optional data; it is not authentication or authority for
arbitrary imported semantic records. Full identity and key comparisons still
apply after a successful seal check.

An invalid optional limit, saturated record budget, malformed structure or bad
seal causes current ordinary recomputation and declines incomplete new history.
It cannot reject otherwise supported source, weaken a fact, silently truncate
contributors, or replace an unknown contribution with a default. Do not publish
unframed trailing rows. A bounded list cannot silently accept its first matching
prefix while ignoring extra linked records. Allocation failure keeps the existing
status-71 boundary. Only the enclosing completely successful candidate may make
the new history eligible for a later update.

The record limit is not a peak-RSS promise. Epoch attempt/input/node/C limits,
physical owning-epoch cleanup and all existing resource/performance gates remain
unchanged. Measure the new record widths, live bytes, allocations and copies.

## Compiler and runtime design

Carry the new current input state through retained range orchestration, and carry
finished optional history through its result into the successful typing cache.
Use existing SLIM records, vectors, checked arithmetic and owner maps. Preserve
the ordinary compiler's lazy range behavior. Its current scan remains the
conservative path when no complete retained topology is available and shares the
same scalar producer. There is no production Rust analysis or semantic fallback.

## Compatibility and migration

Preserve acceptance, ordered current-span diagnostics, all range and recurrence
fields, refinement summaries, termination results, independent hazard/resource
facts, parallel plans, generated C and native behavior. This RFC supersedes only
RFC-0138's requirement to execute both whole-token input scans on every retained
transfer. It preserves their complete results and the fixed analysis schedule.
It introduces no public session schema or new language surface.

## Diagnostics and failure cases

No new source diagnostic or precision rule is added. Missing optional history is
a miss, not an error or an exact negative fact. Failed candidates preserve their
existing source diagnostics and last-good snapshot. Counter saturation remains
bounded with its fixed observer cap; absent observation is unknown.

## Performance and complexity

Construction and reverse adjacency are linear in canonical structure and call
arguments. Each transfer compares its complete contribution lists, searches at
most four versions per callee, and imports or merges actual parameter facts.
No-change whole-snapshot reuse continues to bypass this work altogether.

Preserve existing observer meanings, including actual old scan invocations.
Add separate counters for structural production/import, contribution discovery,
adjacency construction, transfer rounds, key comparisons, scalar merges,
result imports, history admission and copies. Do not label comparison or import
as avoided total work. A retained transfer can replace two old scan calls with
one observed transfer round; its complete result must still match both modes.

Measure baseline/candidate cold checking/emission, retained unchanged/body/
interface edits and multi-caller geometries after warmup with quiet interleaved
same-host controls. Record contrary results, source/seed growth, allocations,
live memory and external backend costs separately. No budget is relaxed and no
speedup is assumed by accepting this contract.

## Alternatives and drawbacks

Keeping eight global scans is simpler and remains the fallback. Caching by callee
body misses caller-derived inputs. Caching only an already-joined interval hides
unknown contributions and incomplete dependency sets. Whole-program source
equality would discard useful locality. A second range solver would duplicate
semantic authority. Additional topology, keys, result pools and checks increase
compiler size and memory; measured costs can require a smaller implementation.

## Test and acceptance plan

Compare every input array after each of the four rounds and both modes against
the existing scan over complete checked source. Also compare all final range,
recurrence, typing, termination, analysis, C and native results with clean runs.
Use permanent independent cases for no callers, one/many callers, repeated calls,
nested arguments, parameter ordering, unknown/out-of-domain arguments, recurrence
invariants, counted recurrences and caller-driven changes over multiple passes.

Cross caller/callee body and interface edits, ownership/effect changes,
insertion/deletion/renaming/reordering, module relocation, rejected/recovered
edits, all host configuration fields and revision/epoch changes. Cross the
existing refinement/report limits without changing them. Exercise exact/beyond
record limits, wrong owners/nodes/slices, invalid fact bits/domains, altered valid
values, damaged seals, excessive/cyclic version links and incomplete caller sets.

Require geometric actual-work evidence and unchanged complete application
baselines. Run ASan/UBSan, bounded allocation-fault recovery and all required
checkpoint commands. The public integrated differential and full release gate
remain M1 closure work even after this child is complete.

## Ratings and evidence

Analysis +2 specifies complete caller-input dependencies and observable reuse.
All cost and safety ratings remain zero pending measurements; score 15. Approval
does not establish an implementation result or an agent-productivity benefit.

## Decision

Accepted under RFC-0112's explicit delegation of in-scope implementation details
and the active goal to finish M1. This is a child architecture contract, not an
external review or permission to weaken a source, safety or performance gate.

## Implementation

Implemented in the production SLIM compiler and portable seed
`64107135f2677bcada015bfe5cd292bb1c3a8f41216fe2c92ebf7793c9df5f61`
(4,716,564 C bytes). Complete current/previous query results and eligible unchanged
structure are retained; the ordinary scans remain the conservative fallback.
Complete empty adjacency omits an unnecessary query, and the full entry-invariant
mask is materialized only when ordinary fallback needs it.

The [checkpoint report](../../benchmarks/results/2026-09-07-m1-input-queries.md)
records the complete four-round/two-array oracle, 19 edit pairs, corruption and
budget/version boundaries, and geometric actual-work measurements through 4,000
callers. Ordinary and ASan/UBSan session comparisons preserve prepared fields,
ranges, recurrence records, plans, C, complete derived parallel-view fields and
range/quality/parallel report components. All required checkpoint commands pass,
as do 1,024 retained-typing and 6,144 session allocation-fault ordinals.

The final clean and retained same-host control comparisons pass. Earlier rejected
cost measurements remain archived. The seed, row widths, allocations, live bytes,
copies and external backend/native costs are recorded separately; no performance
budget is relaxed and no significant agent-productivity benefit is established.
Public host-bound sessions, retained global analysis, native artifact caching and
the complete parent release audit remain M1 obligations.

## Removal and supersession

A replacement must preserve complete caller discovery, exact current input facts,
both transfer modes, fixed passes and bounds, typed identity translation,
conservative misses, snapshot publication rules and all durable regressions.
