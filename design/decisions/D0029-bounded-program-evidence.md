# D0029: Bounded program evidence and agent edit protocols

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

Core 1A can attach facts to canonical SLIM and apply a small direct reducer, but
it cannot yet answer the language's central program-quality questions. A useful
answer must distinguish exact proof from bounded evidence and from an unknown
result. It must also let an agent apply a local structural change without
re-emitting a complete deeply nested module.

Core 1B therefore adds opt-in evidence tools over the existing checked token
tree. Analysis version 2 reports structural, effect, ownership, dependency,
failure, reduction, and representable-state facts with an explicit guarantee
class. Direct reduction exposes a bounded proof report and an exact replay
check. Equivalence is exhaustive only for a frozen finite pure Boolean subset
with at most eight inputs and 256 expression tokens. It either proves equality
for the complete declared domain, returns the first canonical counterexample,
or reports `unknown` with a reason.

Agent edits use a versioned non-executable `(slim-edit 1 ...)` patch containing
one stable token-node identity and one replacement form. The edit tool traverses
canonical SLIM directly and emits one canonical candidate. The normal compiler
remains the only authority that accepts the candidate as a program.

## Alternatives

A universal equivalence engine or global quality score would be more impressive
but would be dishonest: arbitrary equivalence is undecidable, most quality
dimensions are incomparable, and valid-state ratios require intent that source
representation alone does not provide. Core 1B reports a metric vector and
unknowns rather than collapsing evidence into an invented scalar grade.

Adding refinement syntax, contracts, a theorem language, or a second graph IR
could express more properties, but none is justified until the bounded tools
show a concrete missing guarantee. Textual search-and-replace is smaller than a
structural edit protocol but cannot reject non-node byte offsets and makes
agents reproduce irrelevant source.

Proof records could be trusted as standalone certificates. Core 1B instead
replays them by recomputing the unique bounded normal form from the original
checked source. This keeps the trusted base to the existing checker and reducer
and avoids a second proof-language parser.

## Costs

The self-hosted compiler gains bounded quality, proof, equivalence, and edit
modules plus command dispatch. Ordinary `check`, `emit-c`, and native programs
do not invoke them and pay no runtime cost. Default analysis remains linear in
tokens plus fixed 64-fact work per function. Reduction proof discovery records
at most 64 source sites. Equivalence performs at most 256 cases by 256
expression tokens and is available only through an explicit command.

The edit protocol is additional tooling surface, not SLIM syntax. It replaces a
form by stable token identity, never accepts a second program representation,
and carries no implicit type or ownership rule. A caller must check the emitted
candidate normally; the repository launcher does so before publishing output.

Static source-size and lexical-token proxies do not establish that an LLM will
generate better code. The agent benchmark labels them as proxies and records
compiler acceptance, repair locality, diagnostics, native time, and binary size
separately from hypotheses.

## Evidence

Acceptance requires exact deterministic reports, supported/unsupported subset
boundaries, equivalent and counterexample fixtures, proof-record and replay
tests, valid and invalid structural patches, malformed-input campaigns through
every new command, geometric analysis scaling, and unchanged ordinary check
scaling. The complete conformance corpus, sanitizers, allocation-failure tests,
portable bootstrap, and byte-identical self-host fixed point remain mandatory.

At least one pair of differently structured Boolean functions must be proved
equivalent over its entire domain, and one pair must return a stable concrete
counterexample. At least one state model must report an exact power-of-two state
count, while storage with no portable bound must report `unknown` rather than a
fabricated number. An edit must change the addressed form, preserve unrelated
canonical bytes semantically, and pass normal checking before the launcher
emits it.

## Removal

Remove a metric if its guarantee class cannot be stated precisely or if it
duplicates another fact without adding a decision-relevant distinction. Remove
or narrow an equivalence form if exhaustive evaluation can observe a trap,
effect, recursion, allocation, or unsupported call. Remove the edit protocol if
stable node identity cannot prevent ambiguous replacement. Any source-level
bound, contract, theorem, or concurrency feature still requires its own
weighted language decision and surface-ledger entry.
