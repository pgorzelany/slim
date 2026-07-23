# Deterministic parallelism

Status: Core 1F evidence boundary

SLIM has no language-level concurrency primitive and no program runtime
scheduler. Core 1F first establishes which checked computations could be placed
in a structured fork/join plan without changing observable behavior. This is a
proof boundary, not an execution claim.

## Reorder-safety

A function is currently `safe` for concurrent reordering only when the checked
compiler view proves all of the following:

- its complete checked implementation has no observed allocation or I/O;
- it has no `inout` parameter or lexical mutation;
- it contains no allocating, I/O, or checked-trapping operation; and
- every user function it calls has the same property; and
- any recurrence has D0066's complete finite-descent proof.

Defined overflow and bounds traps remain observable even in an `(effects)`
function, so declared effects alone never establish reorder-safety. Conversely,
an effect declaration is a capability ceiling, not an event: an unused
capability does not block a function when its complete checked body and call
graph are available. A checked integer operation stops being a hazard only
when D0063/D0066's fact for that exact source node positively proves totality.
Cyclic call graphs and facts beyond a fixed bound are `unknown`. Unknown never
means safe.

The analysis recognizes an independent fork site when two adjacent immutable
`let` initializers are reorder-safe user computations and the second does not
depend on the first binding. Both results join before the original body. With
no mutation, exclusive borrow, effect, trap, or unstructured synchronization in
either task, the reported group is data-race-free and deadlock-free under that
structured model.

## Fixed bounds and output

`slimc analyze SOURCE_OR_PROJECT` remains the only command. A project is
analyzed through its normal validated, flattened, typed artifact; there is no
second project parser or checker. Analysis version 4 contains the version 3
`parallelism` section with stable source-token node identities, per-function
safety facts, candidate fork sites, and explicit reasons for unavailable or
unknown results. It stores at most 64 functions and 4,096 direct call edges and
performs at most 64 graph-resolution passes.

Candidate sites may overlap. D0067 derives one schedule by scanning stable
source nodes in lexical order, selecting the earliest candidate at or after the
prior selected join. The result is unique and pairwise non-overlapping. The
report retains the complete candidate and selected counts and prints at most 64
selected sites; a larger plan is labelled bounded rather than truncated
silently. It still does not create a task or change generated code. Source
task-token counts are exact structural facts; runtime profitability remains
`unknown` until a separately measured cost model justifies execution.

The shared `integer-proofs` view can prove guarded additions/subtractions,
bounded arithmetic, nonzero division/remainder, and checked byte conversion.
Its fixed domain and refinement limits are described in
`docs/INTEGER_PROOFS.md`. Parallel analysis never reconstructs those facts.

## Application baseline

D0064 makes the native algorithm challenges a permanent analysis
corpus. `slim-bench parallelism` runs every report twice and compares source
size, proof counts, primary safety reasons, and eligible sites with
`benchmarks/parallelism-baseline.tsv`. The command runs in every full release
verification. Baseline changes require a dated explanation.

The initial corpus has 49 functions and 140 checked integer sites, but only one
reported checked site is total and no function or fork site is eligible. Thirty
functions first report an exclusive borrow, ten allocation or I/O, five a
checked trap, and four recurrence. These are primary reasons only; secondary
hazards remain masked. This result rejects execution work and rejects choosing
the next precision feature from headline counts alone.

D0065 removes that evidence ambiguity without weakening classification. Every
retained function now prints a fixed-order `blockers` set. It contains every
direct local blocker plus one stable graph-boundary blocker for an unsafe or
unknown callee. Safe functions have an exact empty set. The set does not
recursively copy every callee detail; `callee-not-safe` is the compositional
boundary.

In the application corpus, all 49 functions have declared effects, 40 have an
unproved checked trap, 31 recur, 30 borrow exclusively, 22 allocate or perform
I/O, and 22 call an unsafe function. Every function has between two and five
blockers. Removing any single category would therefore unlock zero functions.

D0066 advances the baseline to thirteen applications. Declared
effects remain reported for compatibility but no longer block a function when
the complete implementation proves that no granted capability is exercised.
The shared integer view also proves one canonical, strictly decreasing tail
recurrence total. The new `state_machine` application performs two independent
two-million-step recurrences: both are total and reorder-safe, and their
adjacent calls produce one exact positive fork candidate. The original
`variants/command` helper also becomes safe. Real allocation, I/O, unchecked
traps, unsupported recurrence, exclusive access, and graph uncertainty remain
blockers.

D0067 advances the application baseline to schema 4. It stores complete
candidate, selected, and reported-site counts in addition to every prior
metric. `state_machine` remains exactly `1/1/1`; the other twelve applications
remain zero. A separate overlap fixture proves that four candidates become
three selected non-overlapping sites, and a 65-selection boundary fixture
prints 64 sites with a bounded schedule guarantee.

The SLIM compiler project is a permanent dogfood input. Its current 661-function
checked artifact exceeds the 64-function evidence bound and therefore reports a
bounded result rather than silently treating omitted functions as safe.

## Failure and execution boundary

D0068 defines the only accepted execution semantics. Eligible tasks are proven
total and contain no allocation, I/O, trap, mutation, exclusive borrow, or
unknown callee. Existing ownership checking ensures the two expressions cannot
move the same owned input. Results install into their original immutable
bindings before the continuation. A future worker-creation failure must execute
inline; a successfully created worker has one parent-owned join. Since a task
cannot fail and a child has no source operation that waits, cancellation and
wait cycles do not exist in this subset.

D0069 deliberately keeps production execution disabled. The acceptance host's
C11 compiler has no `<threads.h>`; POSIX threads are available only as a
non-portable reference. Repeated reference measurements place this
state-machine body's crossover near 100,000 iterations per task, but that
host/body result is not a portable static cost model. The report therefore
states `(execution ... (status disabled) (reason
no-portable-runtime-or-cost-model))`, and each site retains profitability
`unknown` with reason `target-cost-unavailable`.

The `parallel-runtime` release command preserves the reference experiment and
its 2.00 largest-work ratio budget. Reopening execution requires a portable or
explicitly tiered worker ABI, general ownership-safe capture/result lowering,
serial fallback and join tests, bounded no-nesting behavior, and
target-calibrated or profile-backed evidence on more than one application.
