# SLIM Feature Policy

SLIM is a small compositional language for AI-generated programs. A feature is
not valuable merely because it is convenient or familiar.

## Hard gates

A feature is rejected if it:

- duplicates an existing semantic operation;
- introduces syntactic sugar or a permanent alias;
- weakens memory, type, effect, or concurrency safety;
- adds hidden allocation, copying, synchronization, or nondeterminism;
- can be an ordinary library function without losing a required guarantee;
- adds a default superlinear compiler pass; or
- lacks specifications, diagnostics, tests, and measured costs.

## Weighted RFC score

Every durable proposal rates each dimension from -2 to +2.

| Key | Dimension | Weight |
|---|---|---:|
| safety | Safety and correctness | 20 |
| compile | Compilation speed and scaling | 20 |
| runtime | Runtime performance and predictability | 20 |
| minimal | Semantic minimality, composition, token economy | 20 |
| analysis | Analysis, reduction, determinism | 15 |
| dogfood | Demonstrated self-hosting utility | 5 |

The normalized score is `sum(rating * weight) / 2`, in the range -100 to 100.
An accepted language primitive must score at least +40, have a +2 in one
dimension, and have no negative safety, compile, runtime, or minimality rating.

## When an RFC is required

An RFC is required for every user-visible feature, removal, syntax, semantic,
type, effect, built-in, or compatibility change. It is also required for a
substantial change to compiler architecture, canonical representation, runtime
ABI, ownership or safety, dependencies, or a durable performance budget.

An RFC is not required for a behavior-preserving refactor, a bug fix that
restores accepted behavior, documentation corrections, additional tests, or a
small optimization that remains within the accepted architecture and budgets.
When the classification is uncertain, use an RFC.

## RFC format

RFCs are Markdown files under `design/rfcs/` named `NNNN-name.md`. Their title
uses `# RFC-NNNN: Name`. Current-process RFCs contain these single-line fields:

```text
Status: proposed | accepted | rejected | withdrawn | superseded
Implementation: pending | complete | not-planned
Process: 1
Audience: user | developer | both
Author: name
Created: YYYY-MM-DD
DecisionDate: YYYY-MM-DD
Approver: project-maintainer
Kind: language | architecture | runtime | dependency | compatibility | process
Primitive: one-kebab-case-name | none
Safety: -2..2
Compile: -2..2
Runtime: -2..2
Minimal: -2..2
Analysis: -2..2
Dogfood: -2..2
Score: -100..100
```

They also contain Summary, Motivation, guide-level and reference-level
explanations, compiler/runtime design, compatibility, diagnostics, performance,
alternatives, tests, ratings, the maintainer decision, implementation, and
removal/supersession sections. An accepted RFC and its complete implementation
may merge together. Only explicit project-maintainer approval changes a
proposal to accepted.

The assistant or author proposes ratings and evidence. Governance validates
the arithmetic and hard gates; ratings do not substitute for maintainer
approval. Legacy RFCs retain the earlier headings and declare `Process:
legacy`.

## Surface ledger

`design/surface.tsv` is the canonical inventory of accepted language surface.
Each non-comment row contains a category, unique name, semantic role, and RFC
ID. A semantic role may occur only once within a category. Active surface rows
must cite an accepted RFC whose implementation is complete.

## Performance policy

- Candidate and baseline are measured repeatedly after warmup.
- A reproducible degradation outside the recorded noise band blocks a change.
- Frontend time, external backend time, runtime, peak memory, allocation count,
  and binary size are recorded separately where relevant.
- Scaling tests use geometrically increasing inputs. Ordinary frontend work
  must remain approximately O(n).
- A necessary performance exception requires an accepted RFC scoring at least
  +60, quantified impact, and a containment or compensation plan. Safety and
  duplicate semantics never receive exceptions.
