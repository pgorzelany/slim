# RFC-0159: Bounded semantic context

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the requested measured agent-development loop
Created: 2026-10-02
DecisionDate: 2026-10-02
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 1
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 25

## Summary

Add `slimc context SOURCE EXPECTED_SOURCE MODULE.DECLARATION`: a bounded
production command reporting one selected declaration's checked context from a
current SLIM module or project. Independently retained expected input must equal
the complete captured current input exactly, including every project module.
Use the existing normal production checker, canonical nodes and origin map.
No language syntax, production Rust semantics, second linker, new source
representation, dependency, public session operation or runtime ABI is added.
This command works before the successor M2 syntax experiment.

## Motivation

An agent repairing code needs checked local types, declaration/provider
signatures, ownership modes, effect ceilings and call targets at the original
source locations. Existing quality reports answer different questions. A
source-only prompt leaves the model to reconstruct facts the checker already
has. Whether checked context improves actual model repair results is unknown;
the requested measured agent-development loop must test that claim separately.

Standalone-only context would exclude the compiler and substantial library apps.
As inspected on 2026-10-02, the compiler occupies approximately 1.12 MiB in 33
modules. Existing captured project preparation already has the needed authority
and source map; its reuse is a narrower change than inventing a new transport.

## Guide-level explanation

A client retains an independent snapshot of the exact source revision, including
the same manifest-relative files for a project, then runs:

```sh
./slimc context program.slim snapshot/program.slim program.main
./slimc context library/http.project snapshot/http.project parser.parse
```

Each input is captured once. Expected project manifests are parsed only to
locate ordered module files; expected SLIM modules are never parsed or checked.
Equal complete captured bytes authorize normal checking of the current input.
Success prints one deterministic JSON object. Zero-based half-open byte spans
refer to original captured module bytes, never underscore-renamed compiler text.

Clients retain expected bytes and the compiler build identity alongside the
report, and compare exact bytes before applying a node-based repair. Using the
same mutable path for both inputs does not preserve an older revision. This
command neither applies patches nor certifies that a repair is desirable.

Invalid source retains the existing compiler diagnostics and publishes no
context. A participant repairing a broken candidate can use checked context from
a separately retained last-good project and selected provider declaration. The
report belongs explicitly to those last-good bytes; it is never evidence that
the rejected candidate is valid. The repaired candidate must pass normal
checking and task acceptance tests independently.

## Reference-level specification

### Complete source authority

Require exactly two source paths and one original qualified declaration selector.
Check path/selector lengths, capture inputs, check admitted input limits, and
compare exact bytes before module parsing or semantic checking. A standalone
expected file is data, not a separately checked program.

For projects, reuse `project.capture_project_input`. Equality requires successful
manifest capture on both sides, identical manifest bytes, equal ordered module
counts, successful reads of every module on both sides, and identical module
bytes at every slot. Identical manifests bind module names, relative paths,
entry, imports and exports as well as order. Independent snapshot directories
are permitted when the complete relative contents agree. A manifest-only,
flattened-source or checksum match never establishes equality.

The accepted program is the captured current input; no source path is reopened
for module parsing, checking or emission. Capture is sequential, not an atomic
filesystem snapshot. The report identifies exactly the immutable captured bytes;
it does not promise that live files stayed unchanged after capture. Clients must
compare their complete captured revision again before applying a repair.

Source identity consists of the exact expected bytes retained by the client:
standalone bytes or manifest plus every ordered module byte string. The response
states `identity_evidence: exact-expected-input-bytes`, with lengths and existing
bounded weighted checksums as non-authoritative fingerprints. Different bytes
reject even if lengths and weighted checksums agree. No cryptographic assurance,
fabricated session epoch/serial or cross-revision stable node identity is claimed.
Canonical ordinals are specific to these bytes and this production compiler build.

### Selection and declaration provenance

The selector uses original `module.declaration` spelling, including entry
`module.main`. Find it by scanning canonical declarations and comparing original
module/name bytes through the existing origin map. Do not interpret flattened
underscore names as original qualified identifiers. A missing selector rejects.
Selection limits reporting, never which declarations are checked.

Schema 1 contains source identity, fixed limits, one selected declaration,
direct provider declarations, checked facts/bindings, reference occurrences and
section completeness. Required selected declaration context includes kind,
original qualified name, canonical ordinal, name/declaration spans and exact
complete selected source bytes. Function signature context includes parameters
and their type spans, declared modes and mutability, return type, and effect
ceiling. Structs expose fields/types; enums expose cases/payload types.
Signature source is exact original text, with an exact original source span.
Provider declaration signatures carry the same provenance and independent facts;
their bodies are not recursively traversed.

Declare provenance explicitly: `checked-declaration`, `checked-binding-fact`,
`checked-expression-type`, `checked-call-link` or `checked-type-link`. Each fact
is `exact`, `bounded` with its fixed budget, or `unknown` with a stable reason.
Exact individual facts do not imply an exact complete section when rows were
omitted. No universal quality score is reported.

### Ownership, effects and types

Declared modes preserve current meanings: ordinary, exclusive borrow `@`, or
ownership transfer `^`. Ordinary affine parameters are shared reads under the
existing semantics; ordinary scalars copy. Do not label every ordinary parameter
owned. Decode checked binding facts through the existing `typing.fact_type` and
`typing.fact_borrow_mode` APIs. Report declared mode separately from checked
binding borrow mode, whose values are owner/value, shared and exclusive.
Missing evidence is `unknown: missing-checked-fact`.

These are declaration and lexical binding facts, never proofs of current loan
sets, arbitrary program-point liveness, future move permission or alias
disjointness. A declared effect list is the exact checked capability ceiling,
with `effect_evidence: declared-ceiling`; it proves no observed event. No unused
capability is discharged and no extra call-graph analysis is run.

Type facts use existing checked scalar kinds or original source-backed forms.
Named/container type text comes from its complete original origin-backed span.
Do not expose underscore-renamed type text as source or independently infer
types from names. Type text beyond its fixed budget is explicitly unknown while
its independently valid source span can remain exact.

### Reference and provider provenance

Reference occurrences arise only from semantically identified call, construction
and type AST roles and their checked links. Never report arbitrary positively
linked names as dependencies: lexical name linking also annotates non-reference
roles and binding names reuse a different positive-index meaning. Call edges
record the actual call node/callee span and checked source declaration target.
Type references remain separately classified. Local binding references remain
distinct from top-level provider references.

Built-ins are `exact: builtin` lexical identities, distinct from unsupported or
missing source links; no invented source provider is attached. Unsupported
targets are `unknown: unsupported-reference`, not evidence that no dependency
exists. Self-recursion does not imply termination. The output is a lexical
occurrence list, not a transitive dependency or exhaustive runtime call graph.

Occurrences retain duplicates and lexical order. Emit each direct source
provider once in first-reference lexical order, with an explicit provider key
based on its original module and canonical declaration ordinal. Deduplication
uses only already found checked target identities, not text guesses. Follow no
provider-body edges. Selection of the declaration itself as a provider may be
represented as a self edge without duplicating its signature.

### Bounds and completeness

Version 1 fixes these operational limits:

| Resource | Limit | Behavior |
|---|---:|---|
| Complete captured input on each side | 16,777,216 bytes | Reject before module checking |
| Project modules | 128 | Reject before module checking |
| Canonical nodes | 1,000,000 | Reject before function checking |
| Selected declaration source | 65,536 bytes | Reject rather than mislabel a prefix exact |
| Direct providers | 64 | First-reference prefix; section bounded |
| Selected checked facts/bindings | 512 | Lexical prefix; section bounded |
| Selected reference occurrences | 512 | Lexical prefix; section bounded |
| Type text per row | 4,096 bytes / 128 type nodes | Text unknown if exceeded |
| Provider signature source | 8,192 bytes | Signature text unknown if exceeded |
| Complete response | 1,048,576 bytes | Reject; no partial JSON |
| Each path and selector operand | 4,096 bytes | Reject before capture |

Limits are tooling bounds, not language limits. Existing `io.read_file` allocates
each complete file before SLIM can inspect its size, and project capture locates
modules before total-input admission. This is an admitted input/work/report
contract, not a pre-read allocation or peak-RSS bound. A bounded host read
interface requires a separate accepted contract.

Report completeness independently for providers, facts and occurrences, each
`exact` when traversal completed within its cap, otherwise `bounded` with the
fixed cap and stable reason. Never infer exact emptiness after truncation or
invent totals for unvisited rows. Count deterministic inspected canonical nodes,
emitted rows and copied source/report bytes separately. A complete-response
budget failure rejects the whole operation. All arithmetic uses bounded counters
and subtraction checks before addition; analyzer arithmetic cannot overflow
inside its declared domain.

### Source spans and byte-exact JSON

Validate each span against its original captured source. Standalone declaration
spans use `syntax.ast_declaration_end`; project spans use the selected declaration
origin through the next declaration's original start in the same file, or that
file's end, with the same trailing separator rule. Name spans use individual
origins; declared type spans cover the complete source form. Inferred scalar
types have a null type span; their expression source remains on the fact row.
Synthetic/virtual nodes preserve their existing checked origin: some have
zero-width spans, while an operator callee can cover the source operation.
Neither mapping makes a synthetic callee independently replaceable. Builtin
target names use existing checked canonical identities rather than interpreting
that span as a name. No pretend line/column coordinates are emitted. This
source-mapping clarification was accepted during independent implementation
review on 2026-10-02, before participant trials; it changes no checked origin or
source semantics.

All JSON strings use `json-byte-escapes-v1`: escape ASCII controls, quotes and
backslashes normally; escape every non-ASCII byte as `\u00XX`. Each decoded JSON
code point in these fields denotes one original byte, so encoding the decoded
string as Latin-1 reconstructs exact UTF-8 or arbitrary byte contents without
Unicode normalization or lossy replacement. Include this encoding label in the
response. Human Unicode interpretation remains a client presentation concern.
Names, paths and exact source text follow the same framing rule.

Construct a complete bounded response before printing. Failure emits a compiler
diagnostic identity and nonzero status. The public wrapper renders failure
diagnostics through the existing human/JSON machinery and prints context only
after a complete successful result.

## Compiler and runtime design

Implement `selfhost/context.slim`, with narrow command wiring in
`selfhost/slimc.slim`, `selfhost/slim.project` and the `slimc` wrapper. Standalone
handling parses once with `syntax.parse_program_result` and calls
`check.check_source` once on those same nodes. Project handling consumes the
current captured `ProjectInput` once through `project.prepare_project_input`,
with empty history and its existing strict canonical-node budget, then uses the
returned checked `PreparedProject`, retained type facts and origin map. Existing
project preparation performs its existing flattening/parsing operations; this
command neither adds a duplicate semantic pass nor conceals those operations.
Expected module sources never receive program parsing or semantic checking.

Derive only an ephemeral bounded view. Do not reparse declarations, parse emitted
text, clone checking, resolve semantic names again or invoke a Rust fallback.
Ordinary commands receive no context traversal/report cost; accepted programs,
generated C, runtime and existing diagnostics remain unchanged. This is a cold
selected context command, not a retained-query or incremental-latency claim.

Initial estimated extra cost of project support over a standalone emitter is
approximately 190 SLIM lines: exact input comparison (~35), original file
metadata (~40), selector/origin mapping (~80), orchestration (~35). This is an
implementation estimate, not measured edit size or native performance evidence.

## Compatibility and migration

Existing source/CLI contracts remain unchanged. Schema 1 is new non-executable
tooling data. Public session transport, cross-revision query retention,
additional fact kinds or changed budgets require an explicit accepted extension
and appropriate schema version. A declined context request does not imply
language rejection of that source.

## Diagnostics and failure cases

The schema-1 field types, null conventions, provenance and exact byte framing
are frozen in [the context contract](../../docs/CONTEXT.md). A repository-wide
diagnostic search found no uses of `E0450` through `E0453`; the existing project
reservation ends at `E0449`. Allocate `E0450` for stale expected complete input,
`E0451` for context limits, `E0452` for an absent selector, and `E0453` for
invalid arguments/input-kind mismatch. Existing file-read, manifest and source-check diagnostics
retain their identities and original mapped spans. Cross every rejection in
permanent production-path tests and record the final codes at implementation.

## Performance and complexity

Use the normal approximately linear frontend, linear exact input comparison,
and bounded report traversal/output. Expected snapshots duplicate retained source
bytes and manifest validation, not module parsing or checking. Measure ordinary
checker and cold context separately on geometric fixture sizes, reporting native
time, deterministic inspected-node/row/byte counts and response size. Type text,
provider deduplication and traversal have fixed checked caps. No existing budget
is relaxed. Native timing, peak-memory cost and model repair benefit remain
unknown until independently measured.

## Alternatives and drawbacks

- Source-only prompts remain the control arm; token size is no success rate.
- Existing `analyze` supplies independent quality evidence, not this context.
- Rust extraction would create a forbidden second production semantic path.
- Text/name matching or declaration re-parsing duplicates accepted semantics.
- Hash-only revision guards cannot establish exact equality.
- Standalone-only context excludes important multi-module participant tasks.
- Public sessions add snapshot-selection/framing scope before this treatment is
  independently verified; they remain future work.
- User libraries cannot inspect compiler-owned checked nodes, so this is a
  compiler tooling boundary rather than a new language primitive.

## Test and acceptance plan

Permanent tests execute the production compiler. Cover scalar/named/container
types, struct fields, enum payloads, pure/effectful signatures, all parameter
modes, affine/scalar ordinary parameters, checked local types and semantic call/
type providers. Verify exact bytes and every name/type/signature/declaration span.
Use substantial library and selfhost project selections and a last-good provider
workflow alongside a separately broken candidate.

Require deterministic repeated output; unchanged normal checking and rejection;
missing expected input; changed same-length source; a deliberate weighted-
checksum collision; stale unselected module; unchanged manifest with changed
provider bytes; mirrored snapshot directories; original qualified selectors and
entry main; builtin classification; duplicate/self providers; local names that
collide with declaration spellings; UTF-8 and control-byte escaping; and every
row/type/source/response/input cap. Distinguish exact row facts from bounded
section completeness. The participant experiment tests actual model outcomes
independently of source/model-token proxies and native performance.

Run all repository-mandated bootstrap, governance, cargo and performance,
reduction, parallelism, comparison and agent gates before the coordinated change
is committed. Coordinate shared expensive gates to avoid contaminated results.

## Ratings and evidence

Safety +1 reflects exact stale-input rejection and normal source acceptance.
Analysis +2 reflects checked facts attached to their original nodes/spans with
honest incomplete sections. Compile, runtime and minimality 0 are neutral
proposal ratings, not proof of zero cost. Dogfood 0 records unknown actual repair
benefit. `(1 * 20 + 2 * 15) / 2 = 25`. No language primitive or performance-budget
exception is proposed, so no such threshold or M2 exception is invoked.

## Decision

Accepted on 2026-10-02 after the root coordinator reviewed this concrete
specification under the project maintainer's direct delegation to revise the
roadmap and coordinate feature implementation in service of the stated vision.
The reviewed scope is cold selected context for the current 0.9 compiler,
complete byte-equality preconditions, original source provenance, bounded
reporting and independent evaluation. This accepts no successor source syntax,
runtime change, session-protocol extension, performance-budget relaxation or
claim of improved agent effectiveness. The implementation and coordinated
verification record below supply the separate closure evidence.

## Implementation

Complete on 2026-10-02. `selfhost/context.slim`, production CLI/wrapper dispatch,
E0450-E0453, the schema-1 release contract and permanent conformance implement
the accepted boundary. The
[closure report](../../benchmarks/results/2026-10-02-agent-development-loop.md)
records exact limit crossings, native costs, all participant outcomes and the
passing release components under audited source scopes. Decision: narrow to this opt-in contract; do not
expand breadth or claim general effectiveness. Compiler binary growth of 28.3%,
unknown total context work/memory, and pilot limitations remain contrary
evidence. No performance budget, source rule or runtime ABI was relaxed.

## Removal and supersession

Remove the command if it cannot preserve exact complete input authority, reuse
normal checking, retain honest fact/completeness labels or remain within measured
frontend/report limits. Preserve source-only and independent native controls.
Supersede the schema explicitly if session context is later accepted; do not
retain conflicting source-authority models.
