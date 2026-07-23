# D0080: Finite byte equivalence and named cost vectors

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

Boolean products make the existing equivalence command independently
checkable, but they cannot express a common bounded machine-state boundary: one
byte with a small accepted subset. Quality output also exposes useful facts
without naming how a caller should interpret or compare their costs. Core 1K
needs one larger exact domain and transparent cost models without implying
that arbitrary program equivalence, valid application state, dynamic work, or
peak bytes are decidable.

Analysis schema 7 therefore names a versioned cost vector, and equivalence
schema 2 admits exactly one `U8` parameter as an alternative to the existing
zero-to-eight-Boolean product. Both mechanisms remain explicit tooling over
ordinary checked SLIM.

## Alternatives

Contract syntax, refinement types, theorem declarations, or a second
specification language would permanently expand the surface before maintained
programs justify it. General integer domains or symbolic solvers would make
the proof boundary and compile cost harder to predict. A universal score would
hide tradeoffs and incorrectly rank unknown facts.

The accepted design treats an ordinary pure `subject` function as the explicit
predicate, enumerates all 256 byte values, and reports a vector whose unknown
physical costs stay unknown.

## Costs

`slimc equivalent` remains an opt-in command. It accepts a pure Boolean
`subject` with either zero through eight Boolean parameters or exactly one
`U8` parameter, a 256-token expression limit, and a closed evaluator subset.
It reports exact domain kind, case count, accepted-state counts, the first
canonical counterexample, and `expression-tokens-v1` costs. Unsupported
signatures and expressions return stable `unknown` reasons.

Analysis schema 7 adds `cost-vector 1`. Source tokens, declared effect kinds,
static allocation/trap sites, and static proof obligations are exact within
their named models. Dynamic runtime work and peak bytes are explicitly unknown
until execution-frequency, call, allocation-volume, and layout bounds exist.
No value is summed into a universal grade.

## Evidence

Permanent fixtures prove two differently written predicates accept exactly
two of 256 byte states, find byte `3` as the first counterexample to a
three-state predicate, and preserve unsupported arithmetic as `unknown`.
Boolean equivalence retains its exact four-case result. Reports are
deterministic, balanced, and exercised through the production SLIM compiler.

The quality fixture checks every named cost and both unknown physical-cost
boundaries. Fixed-point bootstrap, malformed-input tests, analysis scaling,
the application baselines, and all prior performance gates remain mandatory.

## Removal

Remove the byte domain if it cannot stay exact, deterministic, bounded to 256
cases, or independently checkable. Remove or version a cost model if its
definition changes. Never infer application validity from accepted states,
turn unknown physical cost into zero, add an aggregate score by default, or
admit a separately parsed specification representation.
