# D0074: Close Core 1H without resource syntax

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 100

## Need

Core 1H must decide whether current applications justify a source-level
resource contract. The permanent 14-application corpus now records structural
expression work, effects, allocation and trap sites, ownership pressure,
totality, reusable recurrence profiles, and exact or unknown call work. The
existing parallel baseline separately records task selection and execution.

Only two applications expose the supported exact recurrence shape. Structural
allocation sites do not determine dynamic allocation volume or peak bytes, and
partial effects do not determine which failure path is reached. Adding a
contract now would choose assumptions and syntax before the missing evidence is
understood.

## Alternatives

A loop form, bounded integer, resource annotation, allocation budget, target
profile, or general cost language could make more bounds expressible. Each
adds permanent semantics and at least one more way to state information already
present in ordinary control flow and types. Expanding inference to equality
loops or parameter-dependent bounds is plausible, but requires a distinct
conditional or upper-bound guarantee and representative positive tests before
it can affect safety, optimization, or parallel execution.

The accepted boundary keeps schema 6 and the existing Core surface. It treats
physical allocation volume, peak bytes, and unsupported execution work as
unknown and moves the next milestone to the safe typed host boundary.

## Costs

SLIM 1H cannot enforce a user-specified time, allocation, or memory budget.
Agents must distinguish exact iteration work from structural expression,
allocation, effect, trap, and ownership counts. The current recurrence proof
is narrow, and 12 maintained applications receive no workload result.

In exchange, ordinary compilation and generated programs pay no new cost.
There is no annotation to maintain, no duplicated loop construct, no target
dependent type rule, and no premature promise that a structural count predicts
physical time or bytes.

## Evidence

The resource baseline analyzes every application twice, requires byte-identical
balanced schema-6 reports, and records 1,488 expression nodes, 24 allocation
sites, 142 trap sites, 27 owned bindings, 53 effectful functions, 19
allocation-effect functions, 52 partial functions, five exactly total
functions, two recurrence profiles, and four exact profiled calls. The two
positive applications each prove two 2,000,000-iteration calls.

Profile and call report limits, zero work, unknown nonliteral controllers,
governance, geometric scaling, conformance, malformed input, fixed-point
bootstrap, sanitizers, fault injection, native comparison, and parallel runtime
checks are permanent gates.

## Removal

Reopen Core 1H only when maintained applications demonstrate a repeated bound
that cannot be recovered safely from checked source and that materially
improves safety, optimization, scheduling, or diagnostics. Any future resource
surface must replace an unknown with a precisely defined guarantee; it must not
duplicate control flow, reinterpret structural counts as physical units, or
weaken typed allocation failure.
