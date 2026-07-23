# Deterministic parallelism

Status: Core 1F evidence boundary

SLIM has no language-level concurrency primitive and no program runtime
scheduler. Core 1F first establishes which checked computations could be placed
in a structured fork/join plan without changing observable behavior. This is a
proof boundary, not an execution claim.

## Reorder-safety

A function is currently `safe` for concurrent reordering only when the checked
compiler view proves all of the following:

- its declared effect list is empty;
- it has no `inout` parameter, lexical mutation, or recurrence;
- it contains no allocating, I/O, or checked-trapping operation; and
- every user function it calls has the same property.

Defined overflow and bounds traps remain observable even in an `(effects)`
function, so declared effects alone never establish reorder-safety. Cyclic call
graphs and facts beyond a fixed bound are `unknown`. Unknown never means safe.

The analysis recognizes an independent fork site when two adjacent immutable
`let` initializers are reorder-safe user computations and the second does not
depend on the first binding. Both results join before the original body. With
no mutation, exclusive borrow, effect, trap, or unstructured synchronization in
either task, the reported group is data-race-free and deadlock-free under that
structured model.

## Fixed bounds and output

`slimc analyze SOURCE_OR_PROJECT` remains the only command. A project is
analyzed through its normal validated, flattened, typed artifact; there is no
second project parser or checker. Analysis version 3 adds a
`parallelism` section with stable source-token node identities, per-function
safety facts, candidate fork sites, and explicit reasons for unavailable or
unknown results. It stores at most 64 functions and 4,096 direct call edges and
performs at most 64 graph-resolution passes.

Candidate sites may overlap. The report does not select a schedule, create a
task, or change generated code. Source task-token counts are exact
structural facts; runtime profitability remains `unknown` until a separately
measured cost model justifies execution.

The SLIM compiler project is a permanent dogfood input. Its current 661-function
checked artifact exceeds the 64-function evidence bound and therefore reports a
bounded result rather than silently treating omitted functions as safe.

## Execution boundary

Any future execution slice requires another accepted decision. It must define
deterministic trap and allocation-failure order, bounded task creation, joining,
ownership transfer, cancellation, and a profitability gate. It may not expose
locks or hidden synchronization, and code without a selected plan must pay no
runtime cost.
