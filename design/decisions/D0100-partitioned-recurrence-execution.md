# D0100: Partitioned recurrence execution

Status: rejected
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 1
Minimal: 0
Analysis: 1
Dogfood: 1
Score: 40

## Need

An affine recurrence that reads shared immutable vectors and writes
`destination[index]` could be split into disjoint index intervals. This would
extend automatic parallelism from independent calls to data-parallel
recurrences without adding source syntax.

A valid proof would require an exact finite interval, pairwise disjoint writes,
read-only shared captures, no allocation or unchecked trap in worker scope,
associative result combination when a reduction is carried, identical serial
fallback, one join per successful spawn, and a profitability decision for the
current worker tier.

## Alternatives

Explicit partition syntax would add another way to express recurrence and make
the programmer responsible for a proof the compiler still must validate.
Lowering the existing spawn threshold favors only coarse one-shot work and
regresses repeated medium work. A persistent pool could reduce creation cost
but adds runtime synchronization and lifecycle state.

## Costs

A production implementation needs a partition and reduction proof, generated
worker ABI, exact serial fallback, nested-worker containment, target-tier
profitability model, and permanent ratios for every accepted shape. A
persistent pool additionally needs initialization, shutdown, failure, and
deadlock tests.

## Evidence

Two existing applications were modified diagnostically, without changing SLIM
source:

- Image convolution split each 65,536-cell round into two disjoint halves.
  Forty worker creations increased fifty-run wall time from about 0.42 seconds
  to 0.57 seconds and increased user CPU from about 0.32 to 0.62 seconds.
- Matrix multiplication split its single 16,384-cell reduction into two
  disjoint ranges and combined the two integer totals after joining. One
  hundred-run wall time improved from about 0.43 seconds to 0.36 seconds, while
  user CPU increased from about 0.27 to 0.31 seconds.

Both experiments produced identical outputs. The present `posix-v1` tier
creates a platform worker for each spawn. That cost is acceptable for one
coarse partition but loses on repeated medium-sized partitions.

## Decision

Do not add this execution shape. Repository policy requires positive
multi-application evidence and a permanent same-host regression budget for a
new parallel execution shape; the existing corpus supplies one positive and
one material negative result. Lowering the threshold to favor the matrix case
would regress convolution and would not be a durable profitability model.

A persistent worker pool might change the result, but it would add runtime
state, synchronization, shutdown, nested-worker, failure, and deadlock
obligations that are not justified by the current evidence. Reconsider only
after at least two existing applications benefit under one deterministic
profitability model and the serial fallback, race proof, trap behavior, and
parallel/forced-serial ratios receive permanent gates.

## Removal

This decision adds no production execution shape to remove. Remove the negative
record only if a later accepted decision supplies the required
multi-application evidence and retains these counterexamples in its permanent
regression corpus.
