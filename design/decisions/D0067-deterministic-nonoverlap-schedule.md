# D0067: Deterministic non-overlap schedule

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

D0062 through D0066 identify safe adjacent computations, but candidate sites may
overlap. Executing or even costing all overlapping sites would describe an
impossible plan and make results depend on traversal accidents. Core 1F needs
one bounded deterministic schedule before it can reason about failure or
profitability.

The accepted policy scans stable source-token identities in lexical order. It
selects the earliest candidate whose site is at or after the prior selected
join, then advances the exclusion boundary to that candidate's join. This
greedy interval policy yields a unique, pairwise non-overlapping plan without
search, backtracking, weights, or a new program construct.

## Alternatives

Maximum-cardinality and weighted interval scheduling require storing and
sorting candidates, and weights are not sound before the cost model exists.
Selecting the innermost candidate makes an outer plan depend on later syntax.
Reporting all candidates as a plan is internally inconsistent. Exposing a
source-level `parallel` marker would add language surface before automatic
selection has justified execution.

Lexical-earliest selection is intentionally simple and stable. A later measured
cost model may replace it through another scored decision, but it may not keep
two simultaneous scheduling semantics.

## Costs

The explicit analysis command carries four I64 counters: complete candidate
count, selected count, reported count, and the last selected join. It performs
one comparison for each already recognized candidate. At most 64 selected sites
are printed; selection continues after the reporting bound so the complete
candidate and selected counts remain exact. If selected sites exceed 64, the
reported plan is labelled bounded.

Ordinary checking, C generation, runtime behavior, and language surface do not
change. Code with no candidate pays no runtime cost.

## Evidence

The permanent fixture contains three adjacent safe calls. Its two overlapping
candidates deterministically produce one selected site, while independent sites
in other functions remain selected. The complete fixture reports four
candidates, three selected sites, and three reported sites.

A generated chain has 129 candidates and 65 selected sites. It prints exactly
64 sites, reports the complete counts, and labels only the schedule view
bounded. Repeated reports are byte-identical. Application baseline schema 4
stores candidate, selected, and reported counts for all thirteen challenges;
the positive `state_machine` row remains exactly `1/1/1`.

Acceptance also requires unchanged analysis scaling, bootstrap fixed point,
conformance, mutation, sanitizer, allocation-failure, and native performance
gates.

## Removal

Never emit overlapping selected intervals. Never use hash, thread, timing, or
platform order as a tie-break. Never call a truncated report exact. A future
weighted policy must replace this one, publish its bounded inputs and
complexity, and demonstrate a measured execution benefit.
