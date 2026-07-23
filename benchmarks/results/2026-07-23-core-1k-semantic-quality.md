# Core 1K semantic quality and reduction

Date: 2026-07-23
Decisions: D0080, D0081
Compiler: self-hosted SLIM, portable C11 seed

## Exact specification evidence

Equivalence schema 2 retains the four-case Boolean product and adds exactly one
256-value domain: a single `U8` parameter. The maintained status predicate has
two accepted states. Its disjunction and inclusive-range implementations are
equivalent over all 256 inputs. Extending the range by one returns the first
canonical counterexample:

```text
(equivalence 2 (status equivalent) (domain exact) (domain-kind u8) (cases 256) (accepted-states 2) (cost-model expression-tokens-v1) (left-cost 26) (right-cost 26))
(equivalence 2 (status different) (domain exact) (domain-kind u8) (left-accepted-states 2) (right-accepted-states 3) (counterexample (inputs 3) (left false) (right true)) (cost-model expression-tokens-v1) (left-cost 26) (right-cost 26))
```

An otherwise checked expression outside the closed evaluator returns
`unknown/unsupported-expression`. No result is generalized beyond its named
domain.

## Named cost evidence

Analysis schema 7 emits `cost-vector 1` per function. Source tokens, declared
effect kinds, static allocation/trap sites, and static proof obligations are
exact under separately named models. Dynamic work and peak bytes remain
unknown with stable missing-bound reasons. The vector has no implicit sum or
universal grade.

Reduction proof schema 2 names `canonical-tokens-v1`. The executable reduction
fixture falls from 410 to 310 canonical tokens and 1,689 to 1,291 canonical
bytes. Proof replay verifies the result, and a second reduction is
byte-identical. Three new atom-only rules account for Boolean idempotence,
identity matching, and common match results; computed calls and scrutinees
remain unchanged.

## Regression evidence

The fixed point is 2,154,131 generated C bytes with SHA-256
`9ac9b3aefaae03c3e7fd518aad038c8e9da6fd255f31e67684d72711fed16bff`.
The quick geometric reduction gate reaches 2,000 declarations without
exceeding its scaling budget. Frontend, parallelism, native comparison, and
agent gates pass without relaxing a budget. The complete release gate remains
the authority for conformance, malformed input, fault injection, sanitizers,
host behavior, and application baselines.
