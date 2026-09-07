# RFC-0139: Stable declaration-local C identifiers

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

Give generated C locals, temporaries and private parallel helpers deterministic
identities within their owning SLIM function. Unrelated declaration insertion,
deletion or reordering must not rename an otherwise unchanged function's C.
Keep the sole emitter, checked binding links, complete lowering inputs and every
current runtime operation. This is a prerequisite for retained C fragments, not
an implementation of their dependency tracking or storage.

## Motivation

At checkpoint d157792, `emit_source_name` appends the binding's whole-program
canonical node index, `emit_temp_name` uses an expression's whole-program index,
and parallel context/runner names use a site's whole-program index. A preceding
declaration edit can change all these names despite unchanged checked behavior.
Consequently old C fragments cannot be reused byte-for-byte after relocation.
Rewriting emitted C would add an unnecessary second interpretation step.

## Guide-level explanation

A local or temporary name uses its canonical node ordinal relative to its
function's parameter-list node. Prototype parameters and function definitions use
the same origin. Shadowed bindings retain distinct ordinals; references still
follow the normal checker's binding link. An edit inside a function may change
its local ordinals and therefore its fragment. This does not promise stable
identities for changed declarations or unchanged lowering after changed facts.

Private parallel helper names additionally include the escaped qualified owner
function name because those identifiers live at translation-unit scope. Existing
function, nominal-type, field and case names keep their current spelling.

## Reference-level specification

### Local identities

Carry the current function's parameter-list node through every emitter that
materializes or references a source local, temporary, destination, match payload,
recurrence argument or parallel capture. The canonical parameter-list origin is
already supplied to most expression emitters. Do not pass a dummy zero origin to
an emitter that uses local identities.

For a source name, resolve the binding declaration through the existing checked
link, falling back to the named binder itself only in the same places as the
current emitter. Keep the escaped source spelling and `_n` marker, but append
`binding_node - parameter_list_node`. For a temporary, keep the `slim_t_` prefix
and append `expression_node - parameter_list_node`. All named binders and
expression temporaries in a checked function follow its parameter-list node.
These differences are nonnegative and injective inside the function. The current
signed destination encoding is decoded before forming a local ordinal; it is
not itself a serialized identity.

Current prototype and definition parameter traversal must share that function's
origin, including exclusive parameters. Match-payload bindings and all recurrence
materialization/update paths use the same origin. Local scopes, declaration order,
evaluation order and existing alpha-renaming behavior remain unchanged.

### Translation-unit helper identities

Name each parallel context and runner from the escaped qualified owner function
name, a reserved separator, the site's parameter-list-relative ordinal and the
existing task ordinal. The existing escape form encodes source underscores as
underscore followed by decimal digits; choose a separator beginning with underscore
and a letter, so source spellings cannot collide with its boundary. Task and site
ordinals have explicit separators. Prefixes distinguish contexts, runners, locals
and temporaries.

Function-body emission already knows its origin. The global wrapper pass may
locate an executable site's owner through checked canonical declaration extents.
There are at most 64 reported sites, so at most 64 lexical owner scans are allowed;
no unbounded per-node backward search or all-function scan for every local name is
permitted. Preserve the current wrapper order. Every executable site must belong
to a current checked function; an invalid owner is an internal failure, never a
fallback to unstable global names or guessed ownership.

### Stability and limits

When a function's complete checked lowering inputs stay equal after relocation,
its prototype, body and private helper identifiers stay equal. Changes to range
facts, selected/executable parallel sites, nominal spelling, effects, ownership
plans or other consumed lowering inputs can legitimately change emitted code.
Future fragment retention must track those dependencies before reuse; this RFC
does not infer a reusable fragment from source equality alone.

The sole C emitter constructs these names directly. Do not parse or rewrite
emitted C, add a separately parsed IR, mutate canonical checked nodes to carry
emission-only numbering, or persist a second node-identity authority.

## Compiler and runtime design

Implement the naming context in `selfhost/codegen.slim`, using ordinary scalar
arguments and existing canonical syntax helpers. Add no global owner array, new
runtime primitive, dependency or semantic compiler path. Ordinary and retained
emission use the same names. Function-relative numbering requires constant work
per identifier; the bounded wrapper-owner lookup remains approximately linear.

## Compatibility and migration

Generated private C identifiers change, so generated C and the portable seed
are regenerated. This does not change SLIM acceptance, diagnostics, public function
semantics, runtime ABI, layout, checks, trap behavior, allocation, cleanup, worker
placement or scheduling. Public SLIM function and nominal names retain their
existing C spelling. Old artifact identities must not be reused across the compiler
change; public actual-host identity binding remains separate M1 work.

## Diagnostics and failure cases

No new source diagnostic or accepted language form is introduced. Invalid or
missing semantic facts must not be repaired by naming. The ordinary checker
remains the acceptance authority and existing diagnostics are preserved.

## Performance and complexity

Record geometric frontend emission and output-size costs before and after the
change, with repeated same-host samples. Keep external C compilation and native
runtime separate. Local naming adds scalar context and subtraction rather than
whole-program metadata. Measure effects on compiler and native generated output;
no speedup or performance exception is presumed. Preserve every existing gate.

## Alternatives and drawbacks

Global node numbers prevent relocation-stable fragments. Hashing local source
would require collision handling and can collapse identical shadowed binders.
A new full node-owner vector adds default memory cost despite existing function
context. Rewriting C identifiers after emission would duplicate interpretation and
risk changing string literals. Shorter local identifiers can still change native
compiler optimization/layout, so native/runtime gates remain required.

## Test and acceptance plan

Exercise unchanged function prototypes, bodies and executable parallel helpers
through preceding body growth, insertion, deletion, reordering and module relocation.
Include shadowed binders, borrowed and owned parameters, match payloads, counted
recurrence expansion, explicit and automatic parallel sites, similar escaped
function names and string literals resembling generated identifiers.

Compare full generated token streams with the prior compiler, allowing only
consistent, injective renaming of the stated private identifiers in their C
scopes. String/character literals and comments are opaque to that test normalizer;
production never normalizes emitted C. Compile and execute the native corpus,
preserve complete analysis and diagnostics, run sanitizer/fault tests and all
required checkpoint gates, reproduce the new portable seed, and retain geometric
cost evidence. Add a permanent relocation-identity regression to production tests.

## Ratings and evidence

Analysis +2 removes an avoidable whole-program-position dependency from generated
private identifiers. Other ratings remain zero until measurements. Score 15.
There is no new semantic guarantee, proof rule or performance-budget exception.

## Decision

Accepted under the maintainer's RFC-0112 implementation delegation and the active
full M1 goal. This authorizes the stated internal compiler naming change, not a
new source feature, completed C-fragment retention or completion of M1.

## Implementation

Implemented from validated checkpoint d157792 in the sole SLIM emitter. The
regenerated strict fixed point is 4,090,266 C bytes with SHA-256
`323f36cd855a58204aee41cf1615aaa58ec0cefc76ca5e572ffaf5407872b76b`.
The permanent identity verifier covers 96 accepted programs and checks prototype
and definition origins, opaque literals, relocation, helper collisions, native
workers/fallback and changed-body non-equality. The old/new comparison preserves
complete analysis and all 197 rejected diagnostics. Sanitizers and both bounded
2,048-ordinal allocation-fault campaigns pass. Required checkpoint gates pass.

The [SLIM Next progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
records every changed native C row, geometric output sizes, repeated same-host
frontend/backend/runtime measurements, peak memory, first-run outliers and
interleaved baseline controls. All 20 measured native machine-text sections are
identical on that host; no cross-target equivalence or speedup is claimed.

Parallel/global query retention, flow ownership, public host-bound sessions,
C-fragment/backend-artifact retention and complete release closure remain M1 work.

## Removal and supersession

Preserve injective declaration-local names, checked binding links, collision-free
translation-unit helper identities, one emitter, exact semantic behavior and all
relocation, runtime and performance regression evidence.
