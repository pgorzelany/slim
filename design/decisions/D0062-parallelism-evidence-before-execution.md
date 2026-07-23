# D0062: Parallelism evidence before execution

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

SLIM intends to discover parallel work while guaranteeing deterministic
behavior, data-race freedom, and deadlock freedom. Declared purity is not
enough: checked arithmetic, bounds checks, recursion, exclusive borrows, and
lexical mutation can remain observable even when a function declares no I/O or
allocation effect. Executing work in parallel before the compiler can explain
those hazards would turn an aspiration into an unsafe optimization policy.

Core 1F therefore begins with non-executable evidence. The existing `analyze`
command consumes the normal checked token links and typed facts, classifies a
bounded function graph, and reports adjacent independent computations only
when neither can allocate, perform I/O, mutate, borrow exclusively, recurse,
or trap. Missing facts produce `unknown`, never permission to reorder.

## Alternatives

A `par`, task, future, lock, or channel form would add permanent language and
runtime surface before a useful safe subset is known. Treating empty effect
lists as purity would incorrectly ignore defined traps and mutation. Speculative
parallel execution followed by rollback cannot undo I/O, traps, allocation
failure order, or resource consumption. Compiler-only heuristics without a
versioned report would be difficult to test, explain, and keep deterministic.

The accepted slice adds no source form. It extends one existing evidence report
with facts that later lowering may consume only after a separate execution
decision and profitability proof.

## Costs

The analysis stores at most 64 function facts and 4,096 direct call edges and
uses a fixed 64-pass graph closure. Larger or cyclic unresolved graphs remain
explicitly unknown. Candidate sites may overlap and are evidence, not a chosen
schedule. Static task-token counts do not establish runtime profitability,
so the report must retain `profitability unknown`.

The first classifier is intentionally conservative. A checked operation remains
unavailable for reordering until a positive totality, range, or bounds proof is
added to the shared compiler view. This will miss safe work but cannot permit a
known effect, trap, mutation, exclusive borrow, or recurrence.

## Evidence

Acceptance requires exact fixtures for independent work and every exclusion
class, unresolved-cycle, function-limit, and edge-limit fixtures,
byte-identical balanced reports, unchanged ordinary checking and C generation,
geometric analysis scaling at or below the permanent 1.25 exponent, the
complete release gate, and a byte-identical self-host bootstrap. The self-host
compiler's normal prepared project artifact is a permanent bounded dogfood
input.

## Removal

Remove or narrow a classification if its safety proof is incomplete. Do not
turn evidence into execution, add concurrency surface, infer profitability, or
relax a bound without another scored decision and measurements. A later
execution slice must preserve source-order traps, allocation failures, output,
and deterministic joining and must cost nothing in programs with no selected
parallel plan.
