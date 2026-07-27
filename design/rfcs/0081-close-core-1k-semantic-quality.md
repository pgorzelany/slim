# RFC-0081: Close Core 1K semantic quality

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
Dogfood: 0
Score: 75

## Need

RFC-0080 supplies an exact byte specification boundary and named cost vector, but
Core 1K also requires reductions that demonstrably improve the selected cost
without widening the trusted rewrite surface. The milestone needs a stopping
boundary so “optimal” does not become an unbounded search or a claim about all
equivalent programs.

The reducer now recognizes three additional atom-only Boolean laws:
idempotent `and`/`or`, the Boolean identity match, and a match whose two arms
return the same atom. Reduction proofs name `canonical-tokens-v1`, record each
rule, and retain exact before/after token counts.

## Alternatives

Inlining, common-subexpression elimination, algebra over calls, arbitrary
branch elimination, or global equality saturation could improve more programs,
but they would require deeper effect, trap, ownership, and cost reasoning.
Keeping several normal forms or optimizer levels would weaken canonicality.
Adding source annotations would duplicate semantics already visible in checked
expressions.

The accepted rules eliminate only repeated total atoms. They cannot discard a
call, checked operation, allocation, effect, move, mutation, or borrow.

## Costs

The reducer still performs at most eight closure passes and reports at most 64
sites. Every accepted rewrite strictly decreases `canonical-tokens-v1`; the
normal form remains deterministic and idempotent. Proof schema 2 identifies
the cost model and replay continues to recompute the normal form from checked
source rather than trusting proof text.

The extra local tests add constant work per visited call or match. Normal
checking and code generation do not run equivalence or reduction and therefore
pay no runtime or compilation cost for these tools. No Core form, built-in,
effect, ABI, dependency, or alternative IR is added.

## Evidence

The maintained reduction fixture exercises all three new laws in executable
code. Its canonical source falls from 410 to 310 tokens, proof output names
each rule, the reduced program produces the same output, a second reduction is
byte-identical, and independent replay verifies it. Non-applicability fixtures
retain repeated calls, a computed match scrutinee, and distinct result arms.

The byte-state fixture demonstrates a practical specification result: only
two protocol states are accepted out of 256 representable byte values, and an
equivalent range predicate proves the same accepted set. The complete
conformance, fault, malformed-input, bootstrap, sanitizer, performance,
parallelism, and application gates remain required.

## Removal

Remove any rule that cannot retain atom totality, strict token decrease,
determinism, idempotence, or replay. A rule over calls or owned computations
requires a new scored decision and complete effect, trap, ordering, and move
evidence. Core 1K does not promise a globally optimal program, arbitrary
equivalence, or a universal quality score.
