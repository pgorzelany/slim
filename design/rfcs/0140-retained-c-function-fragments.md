# RFC-0140: Retained C function fragments

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

Retain actual function prototypes, definitions and executable parallel wrappers
from the sole C emitter across successful internal project-session updates.
Reuse requires complete current checked lowering inputs, stable declaration
correspondence and valid compiler-owned artifact framing. A changed callee body
must invalidate a consuming fragment when it changes a consumed range fact,
counted recurrence or selected/executable parallel site. Source equality alone
is insufficient. Preserve byte-identical clean and incremental C, including
translation-unit order, and observe actual producer and import work.

## Motivation

RFC-0139 makes private identifiers stable under unrelated declaration relocation.
At checkpoint 7dc0737, an unchanged complete snapshot reuses its C artifact, but
every changed-source update still invokes the complete emitter. Retained typing,
plans and range queries therefore do not yet produce local C-emission work.
The internal session already owns both the previous checked source state and its
C bytes, so function spans can share that artifact instead of duplicating a
second complete C payload or parsing emitted C.

## Guide-level explanation

After current source has passed the production checker, compute its ordinary
complete lowering view. Match functions through the existing revision-safe
declaration correspondence. A function may import its previous prototype, body
and helper spans only when every consumed input remains equal under that mapping.
Otherwise invoke the existing emitter for that function. Assemble current output
in the ordinary emitter's exact order and publish its spans only with a complete
successful snapshot. Unchanged-input artifact reuse keeps its existing fast path.

Global header, forward types and checked layout declarations may continue through
the ordinary emitter. Their work must remain visible and separate from retained
function producers. This RFC implements function-fragment retention; public host
transport, native backend artifact retention and retained global analyses remain
requirements of the full M1 goal.

## Reference-level specification

### Current semantic authority and correspondence

Current acceptance always comes from the production checker. A fragment never
repairs a failed, missing or bounded-away checked result. The ordinary emitter
remains the sole producer on every miss, including a cache capacity miss.

Preserve and forward the declaration correspondence and dependency invalidation
already computed by retained checking, rather than constructing a second semantic
dependency graph in emission. Bind it to the exact previous and current revisions
and declaration tables. It is ephemeral compiler-owned data for this transition,
not a serialized certificate or a replacement for current checking. Invalid,
incomplete or unavailable correspondence cannot authorize reuse.

A candidate requires unchanged function identity, interface, body, canonical
shape and complete checked declaration dependencies. In particular, the existing
interface-dependency invalidation must reach dependent consumers. Match checked
node facts and binding/source links through their current declared owners and
local ordinals; do not compare relocated absolute indices as stable identities.
Referenced source type forms and interfaces must remain valid under that same
correspondence. Missing owners or unsupported links cause a miss.

### Complete consumed lowering inputs

At the starting checkpoint, function emission consumes the following inputs.
An implementation must audit every producer call and preserve this inventory when
future emission changes add an input:

| Input | Required equality or validation |
|---|---|
| Function source | Exact current canonical declaration content, interface, checked shape and qualified spelling |
| Checked facts | Current node type/binding tags, source type forms and resolved binding/source links under valid relocation and declaration dependencies |
| Memory lowering | Aligned function owner, `local_region`, `recursive`, allocation-list length and every allocation site's local ordinal and region |
| Range lowering | Every local node's analyzed/lower-known/lower/upper-known/upper/total fields, including unknown and non-applicable rows |
| Counted lowering | Presence and all fields of the function's counted record: local controller/base/body, start, bound, step and iterations; checked owner |
| Parallel lowering | The function's complete ordered site records, with local site/first/second/join identities, both work values, allocation/explicit/executable flags and presence/absence |
| Compiler context | The session's same compiler/runtime/target/options identities and the exact owning checked snapshot |

Comparing a full memory record instead of the consumed subset is conservative and
permitted, provided it stays bounded and approximately linear. A consumed input
may not be omitted because it usually agrees or because its absence already
prevents one optimization. Distinguish missing, bounded and positive facts.

Use the current range result and the ordinary current parallel producer. Function
source equality does not imply equal derived facts: callee-body changes, literal
call changes, refinement saturation, graph limits and lexical selection can
change lowering in another unchanged function. Compare those results before
import, even when retained typing imported the function. Cache immutable source
and checked data only through the owning snapshot; add no second type, ownership,
range or parallel semantic implementation.

### Fragment storage and framing

Record intervals as the sole emitter appends to its ordinary output vector.
Never discover boundaries by parsing, scanning or rewriting generated C. A
fragment identifies its checked declaration, kind, exact byte start and length,
and integrity metadata. Prototype and definition records can use declaration
slots. Global parallel wrappers use the checked owning declaration and the
parameter-list-relative site/task identity established by RFC-0139.

Reuse spans from the previous snapshot's existing complete immutable C artifact.
Do not retain an extra whole-program C copy just to serve function fragments.
New output receives fresh intervals in its own artifact. Wrapper records must
preserve the existing global site order, which need not be function-lexical order;
never group wrappers by owner if that changes clean output order.

Validate revision ownership, declaration alignment, fragment kind, section order,
nonnegative overflow-safe bounds, expected counts, non-overlapping framing and
integrity before import. Duplicate, stale, inverted, out-of-range, truncated or
inconsistent optional metadata causes a miss. A miss cannot make an otherwise
valid current program fail acceptance or expose a partial artifact.

The RFC-0133 trust boundary remains: internal state is compiler-owned. Its bounded
checksum detects tested accidental corruption; it is not authentication against
an attacker who can replace both bytes and metadata. Persisted arbitrary C or
semantic data is not admitted by this RFC. Public host identity binding and any
external executable-cache trust boundary require their own completed contract.

### Bounds, work and publication

Use the existing one-million-node/record and 67,108,864-byte session ceilings.
Fragment metadata has an explicit bound no larger than those limits; parallel
wrapper metadata remains bounded by the existing 64 reported sites and two tasks
per site. Check capacity before optional admission. Exceeding an optional history
bound discards that optional history and uses normal current emission.

Share the checked transition map. One bounded pass over current declarations and
consumed local facts may decide reuse, with the existing 64-site bound. Index
counted records by checked function ownership in one pass or use their validated
lexical order; their 64-row report limit is not a storage cap. The ordinary and
retained emitter may share an indexed counted-record input instead of repeating
`find_counted` over the complete vector for every function. Preserve exactly the
same selected record and lowering. No per-token all-declaration search,
per-function full counted-vector scan or superlinear default index is permitted. Copying imported C and assembling complete output remains
linear in output bytes and must not be represented as zero work.

Preserve existing attempted-revision, input/node/C-byte accounting and last-good
snapshot semantics. A changed output is charged by its full produced artifact
size under the existing cumulative C payload budget. Do not double-charge an
extra retained copy, undercharge imported bytes in a new artifact, or publish
fragment metadata from a failed candidate. Failed checking, generation, allocation
or admission leaves the previous successful snapshot usable.

Keep the existing whole-artifact generation report distinct from function work.
Native observation must count actual prototype, definition and wrapper producer
entries, fragment imports and imported bytes separately. No-change complete
artifact hits invoke none of those producers or fragment-copy operations; they
still perform the ordinary input/integrity checks. Cold paths and corrupt history
must visibly execute the ordinary producers. Retain all prior instrumentation
and its permanent range scratch-work gates.

## Compiler and runtime design

Implement in production SLIM, using existing canonical nodes, retained checked
state and ordinary immutable byte spans. The existing emitter may expose small
shared section/producer helpers so ordinary and retained entry points construct
identical output without duplicating language lowering. Carry transition data
through checking and project preparation only as needed for current emission.
Do not enlarge canonical Token, add a parsed IR, C parser, production Rust path,
runtime primitive or external dependency.

## Compatibility and migration

Accepted SLIM, diagnostics, runtime ABI and generated C stay unchanged from
RFC-0139's implemented form. Cache miss behavior is ordinary checked compilation.
Internal instrumentation may advance its schema with permanent compatibility
updates to every reader; keep whole-artifact and per-fragment work distinguishable.
No public command may claim actual retained emission until it uses this path and
binds actual host identities.

## Diagnostics and failure cases

Add no source diagnostic. Preserve existing stable session reasons and exact
checker diagnostics. Invalid optional C history is a miss, not proof of a source
error. Only complete accepted source and successful current generation can
publish a replacement artifact. Unsupported or incomplete lowering evidence must
remain conservative; never fabricate a reusable result.

## Performance and complexity

Measure before and after on the same host, separating checking, key comparison,
actual generation/import, complete C byte assembly, external backend work and
native runtime. Preserve geometric deterministic work and all existing performance
gates. Record cold, unchanged, local body, dependency-fact, insertion/reorder and
large/dense workloads, including many proven counted functions, plus peak memory
and allocation effects. Storage sharing
and producer counts are not themselves proof of a latency improvement. A
reproducible regression outside the recorded noise band blocks the checkpoint;
this RFC relaxes no budget.

## Alternatives and drawbacks

Whole-source equality already supplies unchanged snapshot reuse and does not
provide local changed-source emission. Source-only function caching misses
body-derived range/parallel inputs. Hashing or reparsing emitted C duplicates work
and does not establish semantic eligibility. Persisting a second full payload
wastes the owning artifact's existing storage. Recomputing a separate semantic
graph in emission creates another authority. These alternatives are rejected.

## Test and acceptance plan

Require clean-versus-retained byte equality and observed producer/import counts
for cold, unchanged and successive body edits; signature, layout, effect and
borrow-mode edits; insertion, deletion, rename, reorder and module relocation;
failed update and recovery; compiler/runtime/target/options changes; optional
history exhaustion; and bounded corruption of every new metadata field and span.

Use unchanged callers whose changed callees alter range facts, counted lowering,
parallel totality/work or selected/executable sites. Cross the permanent refinement,
recurrence and parallel report/graph limits. Positive tests must demonstrate both
real fragment reuse and real invalidation; unsupported/bounded facts stay serial
or checked exactly as clean output requires. Include shadowed names, affine
replacement/matches, counted recurrence, mixed implicit/explicit wrappers, several
sites sharing a function and module-qualified similar helper names.

Extend native query observation rather than infer reuse from metadata. Preserve
complete native analysis/resource rows and all prior tests. Run bootstrap,
governance, Cargo tests, all required benchmark gates, appropriate sanitizers,
bounded allocation-fault campaigns and geometric work/latency/size measurements.
Record exact seed and artifact identities and unresolved limits in the progress
report. Full M1 release closure remains a separate required final audit.

## Ratings and evidence

Analysis +2 establishes explicit checked lowering dependencies and actual
fragment reuse. Other ratings are zero pending measurements; score 15. No new
source primitive, performance exception or unbounded semantic claim is introduced.

## Decision

Accepted under the maintainer's RFC-0112 implementation delegation and the active
full M1 goal. Authorization covers this internal architecture and its complete
validation; it does not redefine M1 around function-fragment retention.

## Implementation

Pending. Start from validated checkpoint 7dc0737. Record implementation and exact
evidence in the SLIM Next progress report. Actual flow availability/loans, public
host-bound sessions, retained parallel/global analysis, native backend artifact
retention and complete release closure remain in the full M1 scope.

## Removal and supersession

Preserve complete current lowering dependencies, exact clean/incremental bytes,
sole-producer cache misses, bounded compiler-owned storage, transactional failure
recovery and all actual-work, locality, corruption and performance gates.
