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

## Weighted decision score

Every durable decision rates each dimension from -2 to +2.

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

## Decision record format

Decision records are Markdown files named `DNNNN-name.md`. They must contain
these single-line fields so the governance tool can validate them:

```text
Status: accepted
Kind: language | architecture | runtime | dependency | compatibility
Primitive: one-kebab-case-name | none
Safety: -2..2
Compile: -2..2
Runtime: -2..2
Minimal: -2..2
Analysis: -2..2
Dogfood: -2..2
Score: -100..100
```

They must also contain the headings `## Need`, `## Alternatives`, `## Costs`,
`## Evidence`, and `## Removal`.

## Surface ledger

`design/surface.tsv` is the canonical inventory of accepted language surface.
Each non-comment row contains a category, unique name, semantic role, and
decision ID. A semantic role may occur only once within a category.

## Performance policy

- Candidate and baseline are measured repeatedly after warmup.
- A reproducible degradation outside the recorded noise band blocks a change.
- Frontend time, external backend time, runtime, peak memory, allocation count,
  and binary size are recorded separately where relevant.
- Scaling tests use geometrically increasing inputs. Ordinary frontend work
  must remain approximately O(n).
- A necessary performance exception requires score +60, quantified impact, and
  a containment or compensation plan. Safety and duplicate semantics never
  receive exceptions.

