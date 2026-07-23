# D0065: Complete parallel blocker sets

Status: accepted
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

D0064 proves that no existing application function is parallel-safe, but its
single primary reason per function hides coexisting hazards. Choosing a feature
from those primary counts could spend compiler complexity without making one
function eligible.

The parallel analysis now retains an exact Boolean blocker set for every
retained function. It records the same eleven stable categories as the primary
reason vocabulary. Direct body scans retain every local mutation, checked trap,
allocation/I/O call, and recurrence; function collection adds declared effects,
exclusive parameters, missing typed facts, and edge saturation; a final graph
pass adds unsafe-callee, call-cycle, and missing-function evidence without
changing the existing primary classification.

## Alternatives

Rescanning source in the Rust benchmark would duplicate compiler semantics.
Propagating every transitive callee detail would produce noisy, potentially
large explanations; the direct function records the stable `callee-not-safe`
boundary instead. A dynamic vector of reason values would allocate per
function and require deduplication. One integer bit mask would depend on
bitwise surface not present in Core or checked arithmetic invariants inside the
analyzer.

The accepted fixed Boolean record is explicit, bounded by the existing
64-function table, and uses no new language primitive.

## Costs

`FunctionFact` carries eleven booleans and reports them in fixed reason order.
The existing body scan fills local facts, so local token work remains one pass.
One bounded graph pass visits the already retained direct edges to attach
function-boundary blockers. Primary statuses, fork-site selection, generated C
for user programs, runtime behavior, and language surface do not change.

The application baseline advances to schema 2 with eleven additional blocker
incidence columns. Every report still runs twice and every row remains exact.

## Evidence

Across the twelve applications, all 49 functions have declared effects, 40
contain an unproved checked trap, 31 recur, 30 accept an exclusive parameter,
22 allocate or perform I/O, and 22 directly call a function that is not safe.
No lexical `set`, call cycle, table limit, or missing typed fact appears in the
corpus.

Every application function has at least two blockers: eleven have two, eleven
have three, eighteen have four, and nine have five. Therefore removing any one
blocker category would make zero current functions safe. This rules out both
runtime execution work and a single headline-driven precision change as the
next milestone.

Acceptance requires exact empty sets for safe fixture functions, combined
local sets, graph blockers, function/edge-bound blockers, schema-2 application
agreement, byte-identical reports, analysis scaling under the existing 1.25
budget, and all bootstrap, conformance, mutation, sanitizer,
allocation-failure, and performance gates.

## Removal

Never omit a discovered local blocker because an earlier blocker already makes
the function unavailable. Never attach a blocker to a safe resolved call.
Unknown graph state remains an explicit call-cycle or bound reason. Extend the
fixed vocabulary only through another scored decision with permanent fixture
and application evidence.
