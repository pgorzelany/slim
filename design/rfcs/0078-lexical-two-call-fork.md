# RFC-0078: Lexical two-call fork

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: structured-fork
Safety: 2
Compile: 1
Runtime: 2
Minimal: 1
Analysis: 0
Dogfood: 0
Score: 60

## Need

Core 1G can introduce parallel execution only for proven pure, total CPU work.
Core 1I's dual-endpoint application contains two independent bounded network
exchanges whose latency cannot be overlapped automatically because external
effect independence is not inferable. The program therefore needs one explicit
intent boundary without futures, detached tasks, locks, channels, or a general
scheduler.

`(fork (let first T (call f ...) (let second U (call g ...) body)))` marks one
existing adjacent two-call shape. Both calls start within the lexical form,
both finish before `body`, and results retain first-then-second lexical order.
The form is transparent under serial fallback.

## Alternatives

Futures, async functions, task handles, spawn/join pairs, channels, and locks
create unjoined or cyclic states that the demonstrated application does not
need. Annotating arbitrary expressions complicates capture and result lowering.
Automatically reordering network effects would claim independence the compiler
cannot prove. A second binding syntax would duplicate `let`.

The accepted wrapper reuses ordinary calls, bindings, types, effects, ownership
checking, the Core 1G one-child runtime, and the existing direct-call lowering.
Pure calls are rejected in this form because Core 1G already covers them.

## Costs

The checker validates one extra form and a bounded leaf-task subset. Task calls
must be direct user calls with scalar or byte-view parameters, no `inout`, no
`partial`, no nested user calls or forks, and at least one accepted bounded host
operation. Owned results are allowed; task allocations use isolated regions
that are adopted after the parent-owned join.

One child and the parent execute at most two calls. Spawn failure runs the same
calls serially. There is no pool, queue, detached lifetime,
worker-to-worker wait, or source lock. Programs without `fork` emit no worker
marker or task state.

## Evidence

Conformance fixes shape, effects, capture rejection, serial fallback, and
result order. Two local-loopback applications compare identical serial and
forked bounded exchanges, verify exact bytes or health results, and retain
same-host latency budgets. Allocation failure, spawn failure, join failure,
timeout, partial host failure, and unsupported worker targets have permanent
tests.

Analysis reports explicit intent, race freedom, deadlock freedom, task and join
tokens, the target tier, and execution status. Fixed-point bootstrap,
malformed-input, compiler scaling, native comparisons, sanitizers, and all
prior gates remain mandatory.

## Removal

Remove the form if supported targets cannot preserve lexical join, isolated
allocation, deterministic result installation, serial equivalence, or bounded
failure behavior. Never add detached execution, implicit mutable capture,
`inout` capture, a nested worker tree, user-visible locks, or a compatibility
alias. Generalize the task subset only with new applications and proofs.
