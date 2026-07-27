# RFC-0103: Canonical AST module boundary

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 60

## Need

The production compiler calls the canonical parsed token tree an AST, but most
consumers still know its S-expression representation. They identify expression
forms by comparing textual head tokens, locate semantic children with fixed
integer offsets, and advance siblings through delimiter-specific
`skip_form`. A repository audit found about 1,900 such operations outside the
syntax module, including 428 form skips, 447 token comparisons, and roughly 475
probable fixed child offsets.

This coupling makes a source-syntax replacement a simultaneous rewrite of
typing, ownership, effects, memory planning, range proofs, parallelism,
reduction, quality analysis, editing, and code generation. It also lets a
mistaken offset in a semantic pass bypass the grammar authority.

The canonical parsed SLIM tree remains the compiler's sole program
representation, and its stable node identity remains a bounded integer handle.
The syntax module becomes the only authority for raw token storage and
concrete-source traversal. It exposes canonical AST node kinds, source-span and
lexeme queries, stable sibling traversal, and role-specific accessors for
syntax-sensitive structures. The compact preorder node arena is the compiler's
documented AST representation; a source parser must construct that
representation directly and may not leak its own tokens or punctuation.
Compiler consumers may inspect canonical node kinds, identities, identifier
and literal contents, and AST child order, but may not call raw token
operations, infer structure from source punctuation, or use delimiter-specific
`skip_form`.

The initial implementation keeps the current compact token-backed tree and
adds no second AST allocation. Its representation may later change behind the
same API when a replacement parser is accepted.

## Alternatives

Changing directly to a new parser would combine representation migration,
language compatibility, diagnostics, source conversion, and every semantic
consumer in one unverifiable cutover. Translating new source into synthetic
S-expression text would add a hidden frontend, duplicate parsing work, damage
source spans, and preserve dependence on syntax absent from the input.

A second dense object AST would provide conventional node objects but would
increase clean-compile allocation and memory before evidence shows that the
token-backed tree is insufficient. A broad generic `child(node, ordinal)` API
would merely move fixed offsets behind one function and would not establish
semantic roles.

Keeping direct token access is smaller today but makes every future spelling,
diagnostic, incremental, and structural-edit improvement compiler-wide.

## Costs

The syntax module gains a closed canonical node-kind vocabulary and semantic
accessors for modules, declarations, parameters, types, expressions, fields,
cases, match arms, and argument lists. Existing consumers must be migrated off
raw token operations. Governance must reject raw token access and `skip_form`
outside the syntax implementation. Role accessors remain preferred where they
prevent duplicated structural assumptions.

Accessors perform constant work over the existing indexed form boundaries.
There is no runtime-program cost, language syntax, dependency, allocation,
garbage collection, separately parsed IR, or duplicated type/effect/ownership
fact. Compile-time and generated-size changes are measured permanently.

The current parser and canonical formatter remain unchanged at this
checkpoint. Every migrated program must retain diagnostics, stable source
spans, checked facts, generated C, native behavior, and deterministic
bootstrap.

## Evidence

The baseline self-host compiler reaches a fixed point of 2,326,806 generated C
bytes. The full 8,000-declaration emission case is about 69 milliseconds on the
recorded host, and all current compiler, project, reduction, parallelism,
native-comparison, and agent gates pass.

Acceptance requires byte-identical generated C for the complete conformance and
application corpus before any syntax replacement, unchanged exact analysis
baselines, no relaxed performance budget, and a governance inventory showing
that compiler modules no longer access raw parser tokens or delimiter-specific
traversal. The AST checkpoint must be independently committable and reversible.

## Removal

Remove or narrow an accessor if it duplicates another semantic role, exposes a
concrete child ordinal, loses stable source identity, or adds measurable
compiler cost without eliminating representation coupling. Remove the boundary
only if a smaller opaque canonical representation preserves parser
replaceability, all semantic facts, structural edits, diagnostics, and the full
scaling suite. Never replace it with synthetic source lowering or a second
parsed program language.
