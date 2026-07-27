# RFC-0028: Direct typed reduction on canonical SLIM

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 1
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

Core SLIM is already a canonical, low-level representation: every expression
has one source form, evaluation order is fixed, types and effects are explicit,
and there is no desugaring layer. Core 1 research needs deterministic program
reduction, stable quality facts, and eventually dependence analysis for bounded
equivalence and parallel scheduling. Creating a separately parsed intermediate
language would duplicate syntax, typing, ownership, and maintenance without
first demonstrating that canonical SLIM is inadequate.

The compiler will therefore optimize checked SLIM forms directly. Stable token
indices identify nodes and bindings. Compact side tables may attach declared
types, effects, storage ownership, lexical uses, last uses, and dependency
edges. A per-function control-flow view may be constructed on demand when a
specific analysis cannot be expressed safely over the tree, but it is derived
data and is never accepted as compiler input.

Core 1A begins with a deliberately small terminating reduction system. It may
evaluate literal Boolean operations, select literal Boolean matches, eliminate
unused immutable scalar-literal bindings, and apply identities whose removed
operations are proven total for every operand. Reductions recurse into the
selected result. The tool attempts at most eight whole-module closure passes;
if cascading rewrites have not stabilized, it returns the canonical unchanged
module. This conservative fallback keeps one invocation idempotent and work
linear in source size with a fixed multiplier.

## Alternatives

A conventional HIR/MIR/LIR stack would make some control-flow algorithms
familiar, but it would establish several overlapping program representations
before SLIM has evidence that they carry their compile-time and governance
cost. Optimizing generated C delegates local machine optimization effectively,
but C has already erased source effects, ownership, regions, declaration
identity, and the structure needed for source reduction and quality reports.

Performing unrestricted algebraic rewriting directly on tokens would be small
but unsafe. Checked arithmetic may trap, calls are evaluated left to right,
allocation failure is observable, aggregate values move, and I/O cannot be
discarded or reordered. Core 1A instead admits only individually specified
rules with explicit totality and ownership arguments.

Leaving source entirely unoptimized remains a valid fallback, but provides no
executable basis for the reduction and program-quality goals. The direct pass
is removable and cannot become a prerequisite for accepting a valid program.

## Costs

The self-hosted compiler gains one direct reducer and one semantic-analysis
module. Reduction requires at most eight linear traversals and an output
buffer; unused-binding scans inspect at most 64 tokens and otherwise retain the
binding. Analysis uses at most 64 per-function binding facts and conservatively
stops adding detail at that limit. Neither pass is part of ordinary
`check` unless a later separately measured decision justifies it. Ordinary C
generation may consume proven reductions only after behavior and performance
gates demonstrate a benefit.

The `reduce` and `analyze` compiler commands are tool surfaces, not Core syntax.
Reduced output is ordinary canonical `.slim` source and can be checked through
the normal compiler. Analysis output is a versioned report, not executable
input. No `.sil`
or `.slir` source format, parser, formatter, serializer, public type system, or
compatibility path is introduced.

## Evidence

Every rule has a positive fixture, a non-applicability fixture covering an
observable effect or trap, and an idempotence assertion. Original and reduced
programs produce identical exit status, stdout, and stderr under ordinary
execution and deterministic allocation-failure injection. The preservation
fixture keeps visible I/O before a checked overflow trap. A deeply nested test
exercises the bounded conservative fallback.

Geometric 250/500/1,000/2,000-declaration measurements keep both `reduce` and
`analyze` under the 1.25 scaling-exponent gate. All 61 conformance fixtures and
2,000 malformed-input mutations pass, including 100 mutations additionally
sent through both tools. The sixteen-module self-hosted compiler reproduces a
1,141,206-byte fixed point with SHA-256
`aeef245413e5766d86ca84efdcea63f3b29694728c4b9a4cf774ce387bfe5572`.
The implementation is written in SLIM and the active Rust programs remain
bootstrap-independent orchestration and measurement only. Full measurements
and reproduction commands are frozen in
`benchmarks/results/2026-07-22-core-1a.md`.

## Removal

Remove a rewrite when its totality or behavior-preservation proof is invalid,
when it makes the reduction system non-terminating or non-idempotent, or when
its measured cost exceeds its value. Remove a stored semantic fact when it can
be derived locally without repeated work. A future control-flow representation
may become persistent or externally inspectable only through a new accepted
decision demonstrating why direct canonical SLIM and ephemeral side tables are
insufficient. Never retain both representations as accepted compiler input.
