# RFC-0058: Core 1D acceptance boundary

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

Core 1D replaced independent checker, planner, and backend rediscovery with one
checked compiler view over twenty-six measured slices. Leaving the milestone
open after its acceptance conditions are met would make its boundary movable
and obscure which later optimization work may rely on checked facts, links,
issues, and plans as authoritative inputs.

The accepted boundary is one checked token stream with exact declaration and
local-binding links, one dense token-indexed expression-type table, one
finalized interval-issue stream, and one retained memory plan. Standalone and
prepared-project emission require that artifact. Later passes may derive
optimization facts from it, but may not recreate a second semantic authority.

## Alternatives

A separate executable typed IR would duplicate Core syntax, identity, and type
rules before an optimization has demonstrated that need. Keeping the milestone
open would avoid a formal checkpoint but provide no regression boundary.
Packing every derived property into syntax tokens would make the common token
representation larger and couple unrelated analyses. Retaining mutable checker
bindings would expose phase-local move state after checking.

The accepted artifact keeps source structure for canonical lowering and stores
only facts whose later consumers would otherwise repeat safety- or
scaling-relevant analysis.

## Costs

Dense expression facts require one compact entry per token, including tokens
that are not expression roots. Local links pack type, mode, and declaration
identity into a bounded integer whose schema is now governed. The memory plan
remains a separate analysis because importing typing into memory would create a
module cycle; its syntax inspection constructs the authoritative plan rather
than recreating a backend decision.

Code generation still reads source forms, literals, declared types, aggregate
layout, and parameter declarations. Those are lowering inputs and ABI
contracts, not recovered semantic conclusions. Changes to the checked artifact
or a new backend-specific semantic table require another weighted decision and
must preserve the full acceptance gate.

## Evidence

The final audit finds no declaration relinking, parameter-name type/mode scan,
aggregate-member search, effect-list read, recurrence-body scan, memory-region
inference, or match-type reconstruction in ordinary code generation.
`syntax/name_is_inout` remains only in memory-plan construction, where it
classifies escape targets without a typing dependency. All finalized type,
effect, match, recurrence, and ownership issues use the same interval stream;
prepared projects project both endpoints through the retained token origins.

Permanent geometric workloads cover declarations, nested bindings, named
types, owned transfers, computed arguments, aggregate temporaries, planned
allocation calls, and linked `inout` reads under 1.25 scaling budgets. The
release compiler checks its 468-kilobyte self-host project in about 0.11 seconds.
The complete gate passes 101 conformance fixtures, 2,000 deterministic
malformed-input mutations, same-host performance budgets, allocation-failure
injection, sanitizers, native challenges, agent feedback checks, and a
byte-identical 1,629,310-byte portable bootstrap seed with SHA-256
`90f60cadb6d845ed9cfe6bd58a81578d8cc9fd9f112690c3e42f9d03dc15b2c7`.

## Removal

Reopen this boundary only through a scored replacement that names the semantic
authority, proves checked/emitted behavior equivalent, retains exact project
diagnostic projection, and passes every standing correctness and performance
gate. Do not add unchecked ordinary emission, a parallel backend type system,
or textual fallback lookup for a fact already present in the checked artifact.
