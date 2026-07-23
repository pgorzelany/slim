# Deterministic parallelism

Status: Core 1J guarded automatic and explicit structured execution

SLIM has no general concurrency scheduler. Core 1F establishes which checked
computations can enter a structured fork/join plan without changing observable
behavior. Core 1G executes one proven pure subset automatically. Core 1J adds
one explicit lexical wrapper for independent bounded host operations whose
effect independence cannot be inferred.

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
second project parser or checker. Analysis schema 7 preserves the Core 1G
parallel fields: stable
source-token node identities, per-function safety facts, exact recurrence work
when derivable, candidate fork sites, and distinct selected, executable, and
executed counts. Unavailable or unknown results retain explicit reasons. It
stores at most 64 functions and 4,096 direct call edges and performs at most 64
graph-resolution passes.

Candidate sites may overlap. D0067 derives one schedule by scanning stable
source nodes in lexical order, selecting the earliest candidate at or after the
prior selected join. The result is unique and pairwise non-overlapping. The
report retains the complete candidate and selected counts and prints at most 64
selected sites; a larger plan is labelled bounded rather than truncated
silently. Source task-token counts remain structural evidence only. D0071 uses
exact total-recurrence iterations, never token counts, for its narrow
`posix-v1` execution decision.

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

## Core 1G tiered worker ABI

D0070 satisfies the worker, fallback, join-failure, and bounded no-nesting
parts of that reopening contract without enabling generated execution. The
runtime has two explicit opt-in tiers:

- `SLIM_PARALLEL` exposes the structured ABI but declines worker creation, so
  generated code must execute the task body inline; and
- `SLIM_PARALLEL` plus `SLIM_POSIX_WORKERS` implements the same ABI with one
  POSIX worker and one parent-owned join.

Neither macro is defined for ordinary generated programs. They therefore
include no thread header, task declarations, task state, environment parsing,
or worker calls. The build driver will opt in only after code generation emits
an executable plan; unsupported targets retain the first tier and identical
serial semantics.

The ABI attempts one child, explicitly runs the same body inline after a
declined attempt, and joins every successful attempt. Both a real worker and
its inline fallback run in worker scope, where further attempts are declined.
One selected site therefore creates at most one worker even if an admitted
callee later contains another selected site.

`SLIM_TASK_FAIL_AT`, `SLIM_TASK_DISABLE`, and `SLIM_TASK_JOIN_FAIL_AT` are
test-only deterministic environment controls in opted-in binaries. They prove
the fallback and external join-failure paths without making either outcome
source-observable.

Automatic execution is still disabled at this checkpoint. D0070 does not by
itself establish ownership-safe capture/result lowering, profitability, or the
required second positive application.

## Core 1G guarded execution

D0071 supersedes D0069's production non-execution boundary for one exact
subset. A selected site becomes executable only when both initializers are
direct user calls with atomic checked arguments, both callees are proven total
and reorder-safe, the site is on the function's leading `let` chain supported
by code generation, and both calls have at least 1,000,000 exact recurrence
iterations. Missing work, computed arguments, unsupported placement, or any
safety uncertainty leaves the site serial.

The production generator emits typed stack contexts and wrappers for both
calls. It attempts one child, runs the second wrapper in worker scope on the
parent, joins every successful child, and then installs both results into their
original immutable bindings. A declined spawn invokes the same first wrapper
inline. Both branches therefore decline transitive nested creation, so
execution never expands into an unbounded task tree. Supported POSIX targets
select the worker tier; other targets use the portable serial tier.

Analysis schema 7 and the permanent application baseline distinguish all five
stages: candidate, selected, reported, executable, and executed. Both
`state_machine` and the independent eight-Boolean `signal_network` challenge
retain exactly `1/1/1/1/1`; the other twelve applications retain zero executed
sites. A nested profitable fixture proves that an unsupported location remains
reported but not executable.

`slim-bench parallel-runtime` now retains both D0069's geometric manual
reference and generated parallel-versus-forced-serial measurements for the two
positive applications. On the acceptance host the accepted quick ratios were
0.724 and 0.695. Permanent same-host budgets prevent silent regressions.
Unselected programs, including `hello`, contain no task macro, context,
wrapper, environment parsing, thread include, or worker link flag.

## Core 1J explicit bounded host work

D0078 accepts exactly:

```text
(fork
  (let first T (call f ...)
    (let second U (call g ...)
      body)))
```

The wrapper is allowed only on a function's leading immutable `let` chain.
Both initializers are direct calls to checked leaf functions. Parameters are
scalars or `Bytes`, never `inout`; neither function declares `partial`; the
checked body contains at least one `io.monotonic-ms` or bounded
`io.tcp-exchange` operation and no nested user call, mutation, recurrence, or
fork. Pure work uses Core 1G instead.

The compiler starts at most one child, runs the other call on the parent, joins
the child, and installs `first` then `second` before evaluating `body`.
Unsupported targets and failed spawns execute the identical calls serially.
There is no task handle, detached lifetime, worker-to-worker wait, lock,
channel, or observable scheduling order.

Allocating tasks receive separate child regions. Their allocation status is
shared through C11 atomics, but their allocation lists are not shared. After
join, the parent adopts both lists before exposing owned results. Allocation
failure is then propagated through the existing typed `alloc` path.

Analysis schema 7 labels every site `(intent explicit)`, keeps race-free and
deadlock-free proofs visible, and distinguishes explicit selection from Core
1G's target profitability model. The `dual_fetch` and `dual_health`
applications retain forced-serial and POSIX-worker builds. Their accepted
same-host ratios are 0.529 and 0.534 under permanent 0.75 limits.

D0079 closes Core 1J without cancellation state. Admitted operations already
have explicit response and timeout bounds; every started call completes with a
value or typed failure and is then joined. A future cancellation mechanism
would add states and failure composition not required by the applications.
