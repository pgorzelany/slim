# RFC-0124: Retained compiler substrate

Status: accepted
Implementation: pending
Process: 1
Audience: both
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

Implement RFC-0112 M1 through one retained production compiler substrate: typed
revision-owned source identities, a derived per-function control-flow view,
dependency-tracked declaration queries, and transactional incremental checking
and deterministic emission. Preserve SLIM 0.9 semantics and the M0 regressions.

This contract defines the shared invariants before implementation begins. Its
pending status is an implementation obligation, not an incremental-reuse claim.

## Motivation

At M0 checkpoint 0265c6d the production compiler checks correctly over the named
repair domains, but `session.run` still compares source snapshots. It retains
neither checked declarations nor C fragments. `query.find_snapshot` can scan the
old declaration list, dependency discovery recognizes qualified spellings, and
invalidation scans the edge list for each queued provider. These estimate-only
operations are not suitable as the retained compiler's semantic dependency engine.

`typing.analyze` links and checks a complete program. `PreparedProject` keeps one
flattened source, token vector, fact vector, issue vector, and memory plan for
that operation. Global canonical node positions also appear in generated C local,
temporary, and parallel-wrapper names. Merely caching a successful function or
its C text would reuse stale identities after insertion or reordering.

Dependencies extend beyond interface signatures. Parameter-range facts can depend
on caller arguments, callers can consume body-derived totality/work facts, and
bounded refinement/report selection depends on declared analysis limits and
lexical order. The retained engine must account for these reads explicitly.

## Guide-level explanation

The compiler owns an incremental session. Each update names the current input
files and configuration. An unchanged input can reuse parsed declarations,
checked results, analysis results, and emission fragments established earlier
in that same trusted session. Changed declarations invalidate the queries that
actually depend on the changed information.

An update builds a candidate revision. Only a completely checked candidate can
become the published good revision. Diagnostics from a rejected candidate do not
replace the last good executable or allow stale results to accept changed source.
The caller can distinguish current errors from an explicitly selected last-good
artifact. A missing or corrupt optional artifact is a cache miss.

Observed work accompanies the comparison with a clean compile. Reading files,
matching source slices, remapping metadata, and assembling output remain visible
costs even when no parser, checker, or generator query executes. No-change source
inspection is not described as zero total work.

## Reference-level specification

### Typed identities and lossless source ownership

A session allocates non-reused revision identities using checked arithmetic.
Handles are meaningful only within that session; external records also require
its explicitly checked epoch. Exhaustion rejects the update, never wraps an ID.

Use distinct nominal compiler types for revisions, files, declarations, nodes,
types, bindings, and places at retained-query boundaries. A raw array position
is an implementation detail, not a cross-revision identity. Source revisions
retain the original bytes, including comments and whitespace, and their canonical
parsed SLIM. There is no second accepted program representation.

A declaration key is the complete tuple `(module name, declaration kind,
declared name)`. Paths are provenance, not identity; relocation preserves the
logical key when module identity and content are unchanged. Compare full keys;
a hash or trie lookup must not substitute for collision-safe equality. Reject
duplicate declarations through the existing checker.

A canonical node belongs to a declaration version and has a declaration-local
ordinal. A change to the declaration's parsed content creates a new version;
insertion or reordering of other declarations does not change that ordinal.
An old revision handle must not directly address a new revision. Reuse requires
an explicit map established from the complete key and exact retained content.
Deleting and later reinserting a declaration does not resurrect stale handles.

Source spans name their source file/revision and checked half-open byte interval.
Unchanged declarations can translate local spans through the new declaration
origin. Never infer a link from an old absolute byte offset. Reject stale epochs,
wrong files, negative/oversized offsets, and incomplete maps before indexing.
Tests must cover comments, whitespace, CRLF handling, insertion, reordering,
renaming, deletion/reinsertion, relocation, and counter exhaustion.

A declaration locator may index source boundaries, but it cannot validate new
syntax or create accepted nodes. Changed source is parsed by the existing SLIM
parser. Reused parse results must have an exact source/configuration match.

### Derived control flow and ownership foundation

Derive a per-function view from canonical checked structure. Its blocks, edges,
and operations reference typed canonical nodes and retained type/binding/place
facts. It is ephemeral, cannot be parsed from an input file, and does not retype
expressions or independently approve an ownership operation.

Represent evaluation in source order, branch alternatives and joins, enum payload
scope entry/exit, assignments, transfers, lexical loans, calls, and terminal
recurrence edges. Model `recur` as simultaneous argument evaluation followed by
transfer to the function entry; it does not create a new source loop construct.
Preserve eager Boolean evaluation, traps, allocation failure, and existing
structured-worker joins. Ordinary user recursion remains a call-graph edge.

Retain the M0 availability and loan rules while moving their orchestration onto
the shared view. A control-flow cycle must not silently discard ownership state.
A mandatory fixed-point computation has a checked work limit and rejects an
unproved candidate on exhaustion; optional precision becomes bounded or unknown.
The concrete algorithm and limit require measured child implementation evidence.
No new mandatory limit may reject previously supported source without a separate
compatibility decision. Existing exact supported recurrence shapes must retain
an exact solution, and existing conservative rejections stay conservative.

### Query results and dependencies

Queries return immutable results with typed owners and record every external
semantic input they read. Reuse is a checked dependency decision, not a second
semantic implementation. The normal checker is the only producer of accepted
new type, ownership, effect, and layout results.

The query boundaries must separate at least these kinds of work:

| Result | Required dependency classes |
| --- | --- |
| Source/declaration index | Original bytes, module identity, parser configuration, declaration namespace |
| Parsed declaration | Exact declaration bytes and parse context |
| Declaration interface and layout | Types, field/case order, referenced layouts, exports/import visibility |
| Checked function | Body, resolved names, callee interfaces, ownership modes, effect ceilings, required totality facts |
| Range/totality/resource/parallel facts | All consumed bodies/call sites, caller argument facts, recurrence facts, analysis budgets and ordering dependencies |
| Generated fragment | Complete checked and analysis inputs used by generation, memory plan, target/options/runtime ABI, referenced declarations/types |
| Native object/link artifact | Exact generated C/header content, backend identity, compiler flags, target/runtime identity, link inputs |

Name lookup records negative results and namespace membership as dependencies:
adding a declaration can change a previously unresolved or ambiguous name.
Deletion, renaming, export changes, layout changes, and ownership-mode changes
must invalidate consumers even when a source body's bytes are unchanged.

Track body-derived facts in both directions where required. A caller change can
invalidate a callee's inferred parameter facts; a callee change can invalidate
optimized callers. Recursion components and analysis-bound dependencies must be
complete. A shared refinement cap or lexical report limit cannot be treated as
a per-function constant if the existing algorithm spends it across functions.

Use indexed key lookup and reverse dependency adjacency. Rebuild source indexing
or output assembly linearly when necessary; do not conceal those costs as reused
checking. Avoid the old per-provider whole-edge scans and quadratic declaration
matching. Global invalidation is allowed for a genuinely global dependency, but
must be named and observed; a fabricated universal dependency is not M1 locality.

The dependency checker must reject any retained result with an incomplete owner,
missing dependency, stale configuration, or bounded-away mandatory evidence.
Recompute using the production operation, with the miss counted. Missing evidence
never becomes a successful empty result.

### Atomic candidate publication and bounded storage

Keep retained immutable query records owned by the session. Candidate revisions
refer to those records through validated typed handles. They do not acquire
unchecked aliases to mutable token/fact buffers. Existing mutable flat compiler
views may remain transitional working views only if their import/export mapping
is explicit and complete; they are not serialized semantic authority.

A candidate publishes only after all required checks and dependency validation
succeed. Rejected source, stale input, I/O failure, report failure, and allocation
exhaustion cannot publish a partial checked revision or a replacement executable.
Changes to multiple files form one candidate, not a sequence of partially accepted
programs. Recovered source is compared with a fresh clean compile.

Session storage and work limits are explicit configuration recorded in evidence.
Limit arithmetic is checked before allocation or addition. A bounded service can
reject an update while preserving its last good revision; it must report the
limit rather than silently reset, truncate, or claim a hit. Reset is an explicit
operation that invalidates the previous epoch. Concrete capacities and lifecycle
are specified with the first session implementation and crossed by permanent
tests; this RFC does not invent a language source-size ceiling.

### Emission identity and persistent-cache trust

Generated C remains byte-deterministic for a given checked source/configuration.
Clean and updated compilation must use the same naming and assembly rules.
Global node numbers in M0 C output are not durable fragment keys. Use checked
owner-local identities or explicit typed relocation records produced by the
emitter; do not heuristically parse C text to discover references after emission.
Any intentional clean-C naming change is documented and tested against native
behavior and complete analysis baselines before cutover.

A cache key includes compiler version and implementation identity, runtime ABI
and content, target, options, source/module/interface identities, and all consumed
semantic fingerprints. Validate complete framing and lengths before reading.
Corruption is a miss; checksums are integrity checks, not authentication.

Initially, accepted semantic result reuse is confined to compiler-owned in-memory
history. Arbitrary persisted facts or proof text cannot approve changed source.
Persisted native artifacts remain local derived caches under an explicit trust
boundary. Shared/imported executable caches require a separate trust decision.
The host may orchestrate C compilation and linking, but no Rust component may
supply substitute language semantics.

## Compiler and runtime design

Production implementation belongs in `selfhost/`, using ordinary SLIM nominal
records/enums and the current runtime. Keep the portable seed reproducible at
each compiler checkpoint. Rust remains limited to independent validation,
measurement, and host orchestration already within its infrastructure boundary.
No dependency, foreign semantic backend, new language primitive, or runtime ABI
change is authorized.

Implement this contract in reviewable stages while keeping the complete M1 goal:

1. Revision-owned identities and lossless declaration/source mapping, exercised
   through production source indexing and strict stale-handle tests.
2. Derived function control flow and reusable checker entry points; preserve the
   complete ownership/effect/layout results and diagnostics before reuse.
3. Retained query storage, complete dependency observation, invalidation, and
   transactional candidate publication; demonstrate real unchanged-check reuse.
4. Cached analysis/emission, stable fragment identity, external backend/object
   reuse, and failed/corrupt-cache recovery; compare every update with clean output.
5. Geometric locality/latency evidence, full native differential corpus, sanitizer
   and fault campaigns, and the full release gate before M1 completion.

These stages are implementation order, not narrower substitute exit criteria.
No stage is called M1 complete on its own. Child implementation contracts must
fix concrete APIs, schemas, algorithms, capacities, and failure diagnostics
before the corresponding production change; they cannot weaken this contract.

## Compatibility and migration

Preserve 0.9 syntax, ownership modes, eager Boolean behavior, effects, allocation
and trap contracts, runtime ABI, and accepted programs. Old estimate-only session
output remains explicitly labelled until its command/schema migration is defined.
A new retained result must not be disguised under historical estimate labels.

Clean compiler and incremental compiler share the same semantic operations.
Keep M0 as a historical executable comparison baseline. M2 references, generic
types, owned Bytes, allocation domains, cleanup changes, and source loops remain
outside this RFC and the active goal.

## Diagnostics and failure cases

Required failure families include stale revision/epoch, missing or invalid node
mapping, duplicate declaration key, incomplete dependency state, corrupt optional
artifact, configuration mismatch, candidate rejection, and explicit service
resource exhaustion. Child command schemas assign stable tooling reason codes;
existing source diagnostics retain their codes and current-revision spans.

Malformed or partially checked source must never produce an accepted executable.
A last-good artifact is available only under its own revision identity; it cannot
be reported as the executable for a rejected current edit.

## Performance and complexity

Keep approximately linear ordinary source/dependency processing and local query
work for changed declarations and their actual dependents. Read/scan/remap/assemble
work is reported separately from semantic query execution. Track emitted bytes,
backend preprocessing/compilation/linking, allocation/storage, and cold startup
independently. No performance budget is relaxed by this RFC.

Preserve RFC-0121 observations while adding actual query hit/miss/execution and
invalidation counters at their production entry points. Instrumentation reports
are exact within their fixed cap, bounded on saturation, and unknown if absent.
Never report instrumented latency as normal latency or invalidation estimates as
executed operations. Record same-host paired cold/warm samples and geometric
work; set new durable reuse gates from validated fixtures, not optimistic targets.

## Alternatives and drawbacks

Whole-project artifact hits already exist and do not meet declaration-local reuse.
Reusing merely source-equal C ignores caller facts, global numbering, configuration,
and analysis limits. Rechecking everything with a fast invalidation report also
fails M1. A separately parsed IR or production Rust rewrite violates the selected
architecture and is not an implementation shortcut.

Typed identities, retained records, reverse dependencies, and explicit mappings
increase compiler code and memory. Tracking a genuine global analysis dependency
can invalidate many results. These costs must be measured, not assigned an assumed
speedup. Breaking work into stages increases temporary adapters, which must be
removed or justified at closure rather than becoming two permanent checker paths.

## Test and acceptance plan

For every update, compare acceptance, complete diagnostics and current spans,
checked analysis/resource evidence, generated C, and native behavior with clean
compilation. Retain exact complete native baseline records; explain every changed
row, including non-primary blockers. Cross all source/configuration changes named
in this RFC, multi-file updates, unchanged input, and failed-then-recovered edits.

Use independent stale-handle/mapping tests and bounded path oracles for the
control-flow/ownership migration. Exercise cache framing lengths, corrupt payloads,
missing entries, wrong compiler/runtime/target/options, negative dependencies,
caller-to-callee facts, callee-to-caller facts, recurrence components, and existing
analysis/report limits. A reordered declaration must neither keep a stale span
nor execute an old C binding identity.

Require observed no-change reuse and local changed/dependent work over geometric
projects, with representative multi-application evidence. Separate unavoidable
linear input/output work from avoided parser/checker/generator work. Preserve all
M0 regressions, full conformance, deterministic malformed-input campaigns,
ASan/UBSan, checked allocation-fault campaigns, bootstrap, governance, Cargo,
performance/reduction/parallelism/resources/comparison/agent gates, and the complete
`scripts/verify-0.9.sh` gate on an identified clean checkpoint.

M1 closes only when every RFC-0112 M1 deliverable and exit obligation is proved
by current artifacts. Pending or indirect evidence remains incomplete. Actual
agent effectiveness and M2-M7 implementation are not inferred from M1 results.

## Ratings and evidence

Analysis +2 specifies the dependency, revision, and publication boundaries that
snapshot estimates lack. Other ratings are zero until measured implementation
exists. Weighted score 15. No compile/runtime improvement or safety proof is
assumed from this design document.

## Decision

Accepted under the maintainer's RFC-0112 delegation and the explicit goal to
finish M1, following RFC-0123. This is approval of the stated implementation
contracts within existing hard gates, not a separate external review, a completed
implementation, or authorization to relax performance or compatibility policy.

## Implementation

In progress. RFC-0125's checked source identities and RFC-0126's exact revision
maps and lazy declaration index are implemented in production. RFC-0127 adds
the independently callable function checker with isolated scratch state and
materialized facts; it does not retain semantic results or reclaim backing
storage per function. RFC-0128 retains the existing layout checker's completion
order for dependency-ordered C definitions, removing the recorded forward-type
emission blocker. RFC-0129 supplies the optional bounded structural function-flow
view and native work/fault observations. Ownership orchestration over that view,
typed place facts, full query/session integration, retained analysis/emission,
service transactions and the full M1 validation remain pending. The SLIM
Next progress report records each checkpoint; M0 remains the historical comparison
baseline, not a claim that the current seed is unchanged.

RFC-0130 implements internal retained function typing with checked relocation and
interface dependency invalidation. Global checks and analysis still execute; the
public session is unchanged. Measured snapshot setup/copying overhead still exceeds
the inference savings in the two-revision workload. Public query integration,
service transactions and retained analysis/emission remain pending.

RFC-0131 compacts retained storage while preserving full typed semantic handles at
query boundaries. It reduces measured setup/update overhead and permanently gates
fixed payload sizes; the two-revision workload remains slower than two clean checks.
It does not change the remaining parent milestones or service obligations.

RFC-0132 connects retained inference to the shared project preparation path and
fixes complete declaration source extents in retained/source-map keys. Current
project visibility and source origins are validated before reuse. Public session
transactions, retained parsing/analysis/emission and service lifecycle remain
pending; this prerequisite does not complete them.

RFC-0133 implements the internal transactional project snapshot, bounded epoch
accounting, failed-update recovery and optional whole-C integrity/reuse. Its
configuration fingerprints remain caller-attested; public host binding and
transport/lifecycle integration remain pending. Changed input still reparses
project sources and regenerates whole C.

RFC-0134 exposes typed, bounded lexical place facts from retained checked bindings
and a matching-owner flow adapter. It preserves the saved-row budget and does not
infer alias uniqueness, availability or loan state. Flow-based ownership
orchestration, retained declaration parsing/global analysis/C fragments/backend
work and complete M1 validation remain required.

RFC-0135 implements retained declaration parsing for original modules and the
flattened project, including lexical/lookahead dependencies, current canonical
tokens, transactional publication and capacity tests. Native session schema 2
separates lexing, declaration grammar execution and node imports. A body edit in
the geometric fixture executes two declaration grammars, but metadata adds measured
cost and global analysis and whole-C emission still run. Flow-based ownership/loan
orchestration, public host identity and lifecycle integration, retained global
analysis/C fragments/backend artifacts, and complete M1 closure remain required.

RFC-0136 repairs the recorded minimum-I64 emission blocker and decimal leading-zero
lowering, and rejects out-of-domain integer literals through E0361 under its
explicit compatibility contract. This supplies correct literal artifacts for
subsequent retained emission; it does not complete retained analysis or generation.

RFC-0137 implements retained function memory plans with typed owners, local
node/byte relocation, pooled saved rows, checked framing and independent record
budgets. Native session schema 3 separates actual plan construction from imports.
Full current plans and C match clean preparation across the corpus, relocation,
metadata/capacity cases and scalar/dense geometry; two bounded fault campaigns
cover unchanged and changed snapshots. The nested prototype's cold penalty and
pooled candidate's cost evidence are preserved. This completes function memory
planning reuse, not range/parallel analysis, flow ownership, public host-bound
sessions, stable C fragments/backend artifacts or the full M1 exit criteria.

## Removal and supersession

Superseding implementations must preserve canonical-source authority, all stale
identity and dependency regressions, transactional publication, measured reuse,
complete native evidence, and historical costs. Retained tooling data must never
become an independent parser or authority for accepted source semantics.
