# RFC-0162: Injective project namespace encoding

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, under delegated overnight SLIM Next implementation direction
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

Restore the accepted explicit module namespace through one injective encoding
inside the existing canonical project flattener. Derive identifier roles once
from canonical SLIM nodes; encode declarations, user callees and named types,
including unknown names. Preserve lexical value/binder roles and the normal
checker's sole authority. No syntax, alias, local resolver, parsed representation,
semantic fallback, dependency, runtime ABI or performance budget is added.

## Motivation

Captured production checks on 2026-10-02 demonstrate three distinct function,
record and enum names collapsing after dots become underscores: `a.b_c` and
`a_b.c` both become `a_b_c`. The compiler reports a false duplicate. Conversely,
an unqualified unknown `a_value()` or type `a_Value` in an unrelated module with
no imports is accepted because it equals another module's generated name.
A documented `math.bits.value()` call splits at the first dot and is rejected;
qualified exported entry `app.main` does not target the retained `main` name.
A project function named `I64` wrongly renames scalar type uses, although the
same standalone source passes the production checker.

These are finite reproductions of namespace authority defects, not proof that
all accepted programs are safe. Local imported-function spelling, same-module
function/value shadowing and user `Bool`/`alloc` controls currently pass and must
continue to pass. Raw receipts remain under ignored `build/overnight/probes/`.

## Guide-level explanation

A project user declaration belongs to its original module. Qualifiers select an
explicit directly imported module; bare callees and named types select their
current module. The compiler preserves that pair when preparing the single
canonical checked module. Source-level spelling, formatting, interface artifacts,
context selectors and diagnostics continue to use original module/file spans.
Generated private names change; they are not a source compatibility interface.

## Reference-level specification

### Exact identity

For legal ASCII identifier segments, define `escape` byte by byte: `_` becomes
`_0`, `.` becomes `_1`, and other identifier letters/digits remain themselves.
Encode a pair as `p + escape(module) + '__' + escape(identifier)`. An escaped
segment contains no `__`; the unique separator identifies the two segments.
Each underscore escape is invertible and no unescaped underscore exists.
Consequently equal encoded pairs imply equal complete original module and
identifier bytes. Hashes, assumed reserved source prefixes and name restrictions
do not supply identity. An original declaration name equal to a generated spelling
is encoded again from its own original pair; it cannot capture another declaration.

For an unqualified user callee or nominal type, look up the exact original name
in the complete current-module declaration trie. Every present declaration,
including a wrong-kind declaration, uses the pair encoding above. Exact absence
uses the compact private spelling `u + escape(identifier)`. No actual user
declaration renders with initial `u`: all declarations start `p`, except the exact
entry `main`. Thus an absent name cannot link any user declaration or entry main.
Lexical bindings do not supply call/type declarations, even when their spelling
equals this compact name. The normal checker alone rejects the unresolved name;
no accepted cache/history is published for that rejected program. Qualified
references pass original visibility first and retain the declared pair encoding.
This exact absence fact supplies namespace isolation, not a guessed type or scope
fact, and consumes O(original identifier bytes). It avoids repeating a long owner
at every unknown reference in a rejected program. No source spelling is reserved.

The module portion of a qualified function/type reference ends at its **last**
dot: declared identifiers contain no dot, while module identities may. Existing
import/export and self-qualification rules still run against original source.
Use one target-aware entry exception: pair `(entry module, main)` renders `main`
for its declaration and every permitted qualified/bare callee reference. Every
other module's unknown bare `main` renders the compact absent-name sentinel.
Do not infer entry identity merely from the module containing a reference.

### Canonical roles

Derive an ephemeral per-node role view through shared canonical AST accessors.
The view has no parser or type/ownership/effect implementation. A flat canonical
preorder pass guards real tagged forms before role helpers, so a plain parameter
named `fn` or `let` cannot trigger their legacy textual fallbacks. It marks exactly:

- function, struct and enum declaration names;
- ordinary user callees, excluding positively recognized built-in operations;
- non-scalar named types in parameter, return, local annotation, field, enum
  payload and composite type argument positions;
- nominal record/enum construction targets, including legal scalar-looking
  declaration spellings; construction resolution does not use annotation
  scalar precedence.

Visit every supported expression shape, including let/statement/assignment,
call, construction, projection, if/match arms, recurrence and parallel body.
Unknown unqualified user callees and named types receive the compact disjoint
encoding only from exact trie absence; they never preserve raw generated spelling.
Type-role scalar precedence stays with the existing canonical scalar helper.
User function `I64` is encoded as a callee/declaration, while scalar `I64` stays
raw in an actual type role. Vec/Arena/Id constructor markers remain canonical.
Current Core has no user type-parameter declaration/bound-reference nodes. Its
composite argument comes from `ast_type_argument` and follows the existing
scalar/named-type rules; spelling `T` alone is never guessed as a binder. A
future accepted type-parameter feature must preserve declaration/reference roles
together through its shared canonical helpers and extend this view's coverage;
it must not recast a bound reference as an unknown concrete module type. This
RFC introduces no unsupported generic semantics.

Preserve lexical binders/value occurrences, strings, integers, boolean/void
literals, operator virtual nodes, control/declaration/type markers, effects,
mutable/exclusive/owned modes, field names and case/pattern labels. A keyword
spelling is excluded by its actual role, never by a blanket text comparison.
SLIM is first order: expression atoms resolve only to checked lexical bindings
or literals; ordinary callees/types resolve declarations. Lexical names equal
to encoded globals therefore remain values and cannot open a module namespace.
Existing lexical scope and shadowing are checked by the sole normal checker.

### Original namespace visibility

Restore the already documented direct-import, export, and no-self-qualification
rules for every qualified named-type and nominal-construction reference as well
as user calls. Preserve existing call diagnostic precedence and the exact
original canonical reference-form diagnostic interval used by that authority;
new annotation failures use the exact original type atom span. Call and nominal
construction intervals come from the canonical node and its closing token,
which currently delimit the callee/type head rather than the full surface
invocation. No lexer, parser or source-span representation change is introduced.
Use E0417 for self-qualified,
E0416 for unavailable/non-imported modules, and E0415 for private, missing, or
wrong-kind target declarations. Each checked module retains its first invalid
reference in original canonical node order, following the existing authority.

Build one ephemeral namespace metadata view from the canonical originals, with
the existing syntax.NameNode/NameEdge trie helpers: a module index and per-module
direct-import, export, declaration-node indices, plus each module's role vector.
Trie lookup consumes exact original reference bytes; ASCII edge fanout is fixed.
Declarations remain canonical nodes and the normal checker decides their types,
arity, effects, ownership and behavior. No local resolver or semantic parser is
introduced. Build indices once and reuse the role view for visibility and
flattening; do not add per-reference linear module/declaration/list scans.

The existing empty-record call syntax may select a qualified exported struct
only when that original struct has no fields and the call has no arguments.
The normal flattened checker then classifies and accepts the canonical empty
construction. Wrong-kind and nonempty record calls retain their existing
visibility diagnostics. Record and enum construction tags require their actual
nominal declaration kinds. Scalar precedence applies to annotations, while
nominal construction targets resolve declarations even for legal names such as
I64 or Bool.

Do not use the module's broad text-to-declaration links to decide which local
atoms to rename. Existing links may still classify empty record calls. No local
scope resolver, alpha-renaming pass or global-name lookup fallback is introduced.

### Source authority and totality

The role view derives from the complete structurally validated canonical slice.
Traversal advances through existing bounded node/sibling helpers and visits each
role once. Unsupported or malformed structure never guesses a role or publishes
checked history. The resulting names retain the same node ordering/count, source
origin module and file-relative half-open spans. Flattening adds no expression,
effect, allocation, move, mutation or control edge to the compiled application.
Ordinary checking and retained checking consume the same transformed canonical
source, with the existing parser/checker and original provenance table.

## Compiler and runtime design

Implement roles and namespace rendering in production `selfhost/` against the
canonical syntax API. Use the existing project preparation/formatter/origin path.
The adapter supplies identity; normal typing, ownership and effects decide all
acceptance. No production Rust semantic path or runtime change is permitted.

Namespace metadata construction is O(canonical nodes + indexed original name
bytes) with fixed ASCII fanout; visibility queries are O(original reference
bytes), with constant node-role/kind access. This replaces existing repeated
call scans and does not introduce superlinear work for named-type references.

## Compatibility and migration

Refresh the portable C seed to a deterministic fixed point after implementation.
Generated private global and some previously over-prefixed local C names change;
source interface identities, local binding ordinals and declaration-relative C
stability remain defined by their existing contracts. Complete lowering keys and
compiler configuration identity must invalidate incompatible old retained facts,
parse/emission fragments and artifacts; no old spelling compatibility fallback.
Aggregate project `analyze` reports name declarations from the derived canonical
project source; those labels migrate by the exact pair encoding. Binding labels
retain their original lexical roles. Label migration must preserve the complete
report's guarantees, blocker sets, statuses, domains, bounds and numeric facts.
Compare clean and retained results, rejected updates, relocation and unrelated
module/declaration edits against freshly checked current source.

Native measurement instrumentation currently assumes underscore-flattened owner
names. Migrate work-probe, input/checking probes and project-list anchors to the
new exact encoding, retaining all existing metrics and exact one-anchor checks.
Add role traversal observations where needed; do not remove a passing gate or
claim old counters measured new code. Preserve source context/interface text via
original provenance rather than decoding generated spelling heuristically.

Freeze the held-out protocol-2 corpus/compiler bytes before any production edit.
Its captured experiment remains independent of later namespace repairs.

## Diagnostics and failure cases

Existing checker codes remain authoritative. Invalid unknown callees/types must
reject at their original file-relative spans; true same-module duplicates still
reject. Distinct original pairs must not create a duplicate. Direct-import/private
visibility and self-qualification diagnostics remain source-mapped. Extra source
identifiers matching internal spelling do not gain access or become reserved.

## Performance and complexity

The **new role traversal** is O(canonical nodes plus inspected original bytes),
with one bounded role entry per node under the existing 1,000,000-node module
boundary. Escaping has at most two output bytes per admitted segment byte.
Namespace-emitted bytes are bounded by the sum of owner and identifier bytes per
encoded occurrence, plus constant separators. Lexical local names do not acquire
owner-prefix expansion. No per-atom whole-module declaration/scope scan is added.

The existing textual prefix adapter already repeats long owner names at known
bare callee/type occurrences. This proposal does not claim raw-source-linear
complete flattening for an arbitrary long owner with many short known calls;
measure that latent cost separately. Unknown-name lookup/encoding remains linear
in original identifier bytes and adds no owner-times-unknown-reference expansion.
Measure ordinary project geometric families and adversarial long-owner families
with many known calls, unknown calls and unknown annotation types before/after,
with source bytes, emitted bytes, observed role/byte work and frontend time
separate. Measure native C backend, generated size and representative runtime
separately. Any permanent gate failure blocks adoption; no budget relaxation is
included. Compile/runtime benefit and absolute cost remain unknown before these
measurements.

## Alternatives and drawbacks

Only changing the separator misses unknown bare names that equal internal global
spelling and leaves the scalar-role rewrite. Reserving a prefix rejects legal
source. Uniformly prefixing every local identifier repeats long module identities
unnecessarily. A new local resolver or independently parsed representation would
duplicate existing authority. Ordinal/hash module identities weaken stable exact
source identity or invalidate unrelated modules. The chosen role view adds
compiler code and private-name bytes; those costs require measured acceptance.

## Test and acceptance plan

Permanent production conformance covers distinct function/record/enum pairs,
old and new generated-spelling unknown callees/types, true duplicates, dotted
module calls/types, qualified exported entry main, self/private/unimported
references, scalar/builtin/control/effect-looking legal names, lexical shadowing,
fields/cases, mutable parameters/payloads, ownership modes, recurrence and parallel
bodies. Pin exact source-mapped diagnostics and positive controls. Test canonical
role coverage and transformation determinism through the normal production path.

Compare clean/retained checked results and generated C after body/interface,
module/declaration insert/delete/reorder/rename, rejected update and recovery.
Run geometric work/performance, project-list gate, normal conformance, allocation
failure/sanitizer checks and all required bootstrap/governance/Cargo/benchmark
gates through the coordinator. Passing finite fixtures is not universal safety.
The permanent namespace observer bounds every unsigned work counter at
1,000,000,000 before increment/addition. Its identical helper is exercised at
cap-minus-one to cap, maximum admitted addition, increment/addition crossings
and a UINT64_MAX overflow attempt; crossings abort the observer rather than
publish a wrapped work value. The geometric fixtures retain their role/trie/
emitted-byte limits independently of this observer representational ceiling.
Addition also rejects an already out-of-domain counter before subtraction;
fixed controls cover UINT64_MAX plus one and cap-plus-one plus zero.

## Ratings and evidence

Safety +1 recognizes finite demonstrated unauthorized cross-module acceptance.
Analysis +2 recognizes exact pair identity and preserved original provenance.
Score 25. Compile/runtime/minimal/dogfood zero are neutral metadata: costs and
selfhosting utility remain unknown until production measurements, not zero cost.
This adds no primitive and authorizes no performance exception.

## Decision

Accepted by the coordinating maintainer agent on 2026-10-02 under the human
maintainer's explicit delegated overnight implementation direction and RFC-0112
authorization, after exact scope, role/identity proof, costs, migration and
acceptance review. Primitive none; no score exception or budget relaxation.
The corpus/compiler freeze precedes implementation edits. The coordinator also
explicitly accepted the documented nominal visibility/empty-record restoration
and once-built shared trie metadata extension before its dependent edits.
The coordinator explicitly accepted exact compact undeclared-name containment
and mode-free record-field accessor restoration before those dependent edits.
The coordinator also accepted preserving the original canonical reference-form
diagnostic interval after the first fixture invocation exposed a mistaken
whole-surface-span prediction. The existing `project-private` E0415@app@48:59
row remains unchanged. Only three new constructor visibility expectations were
corrected: Number 60:80 to 60:70, Empty 60:71 to 60:69, and Choice 60:79 to
60:70. The proof source is the existing canonical `ExprNode.end`,
`emit_expr_node` closing-token provenance and `ast_node_end` accessor;
annotation atom intervals remain unchanged. The failed invocation and original
predictions remain in the local verification receipts.
The new unknown-main nonentry fixture's initial standalone E0102 prediction
was also corrected to E0314@lib@48:52: a nonentry-only file violates the
standalone entry requirement before checking. An independent control replaces
that four-byte absent callee with `nope` and appends a valid standalone main,
establishing the normal checker's E0314 interval without changing the project
fixture or production implementation. Its original mistaken prediction and
failed invocation remain retained locally; no old conformance row changed.

## Implementation

Implemented in production `selfhost/project.slim`; refreshed portable seed
generation two equals generation three exactly. The 87 new namespace rows and
48 preexisting diagnostic rows pass without changing any old expectation.
The scoped campaign passes 445 fixtures and 2,000 deterministic malformed
inputs; the integrated checkpoint adds three nested-expression fixtures. The permanent namespace observer passes 16 geometric inputs and eight
counter boundary controls; input and checking observers pass on actual generated
C. Current identities, finite domains and unfinished acceptance are recorded in
`benchmarks/results/project-namespace-current.json`; its complete original rows
remain in the linked compressed archive. Source tree
`3ec37262c81a26ad9c91833f69fd06946bcd538e` passes complete repository, reproducible
release and website verification and is pushed as `90de8e9`. Separate quiet
frontend/backend/native observations retain mixed ratios and named inputs;
these finite checks establish no universal safety, speed or model-benefit claim.

## Removal and supersession

Replace only with a smaller exact namespace adapter retaining sole checker
acceptance, canonical source, current provenance and durable gates. Do not retain
underscore aliases, inferred global access or a compatibility semantic fallback.
