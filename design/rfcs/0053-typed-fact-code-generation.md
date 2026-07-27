# RFC-0053: Typed-fact code generation

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

The backend typed computed call arguments by reconstructing either a user
parameter type or a partial built-in signature table. Recursive-transfer
temporaries separately reread parameter declarations. Both paths ran after the
authoritative typed view had already proved and retained the exact expression
type, so an omitted built-in case or a future semantic change could make valid
checking disagree with C lowering.

Ordinary emission now receives the checked dense fact vector and threads it
through the recursive expression emitter. Computed call and `recur` arguments
query their expression token directly. One closed adapter maps the five scalar
type kinds to existing backend scalar codes and passes source type forms through
unchanged. The backend built-in-argument and parameter-type reconstruction
functions are removed.

## Alternatives

Copying the type into syntax-token links would overload links already used for
declarations, bindings, aggregate members, and variant cases. A second backend
type vector would duplicate the typed view. Storing the global fact vector in
every function plan would multiply handles and give the memory plan ownership
of unrelated type evidence. A global mutable emitter context would weaken
reentrancy and future parallel generation.

An earlier Core 1D experiment found that threading another vector through a
recursive aggregate emitter crossed the self-host source-shape guard. The
current dense representation and smaller consumer were therefore measured
before acceptance. Explicit immutable-by-convention `inout` threading keeps one
owner, one query API, and no hidden process state.

## Costs

Twenty internal emitter boundaries carry one additional vector handle. They do
not allocate or copy fact elements. Each computed argument performs one bounds
check, one vector read, and a closed scalar-kind dispatch; source-type forms
retain their checked token identity. Generated C and runtime data structures are
unchanged.

This cut covers computed user/built-in call arguments and recursive transfers.
Locals, aggregate fields, variant payloads, and match destinations still have
explicit declared or contextual types and remain later consumers where using a
fact removes real semantic rediscovery. This decision does not authorize a
second lowering IR or implicit type conversion.

## Evidence

Self-validation remains about 0.10 seconds and the candidate reaches a
byte-identical fixed point. All 98 fixtures and 2,000 deterministic
malformed-input mutations pass. C output is byte-identical to the preceding
compiler for representative nested-call, recurrence, storage, record/variant,
lifetime, local-variant, and project programs.

`generated-computed-arguments` permanently emits geometrically nested calls at
125, 250, 500, and 1,000 levels in quick verification, with an accepted
1.25 exponent budget. The first candidate run measured 2.992, 5.106, 12.157,
and 37.695 milliseconds, an endpoint exponent of 1.218. The preceding compiler
measured 3.138, 5.446, 12.301, and 38.089 milliseconds on the same new workload,
so the fact consumer does not hide a latency regression. The portable fixed
point is 1,623,034 generated C bytes with SHA-256
`f6c123aa537f28e78e62d82f12f391d2ec684c7b318929d51d4036d893ebdba2`.
The authoritative release run measured 2.958, 5.281, 12.113, and 38.210
milliseconds, an endpoint exponent of 1.230. The complete gate passes all
performance budgets, sanitizers, allocation-failure injection, native
challenges, and deterministic bootstrap verification.

## Removal

Replace fact threading only with another reentrant path to the same checked
expression types that remains bounded and passes the computed-argument scaling
gate. Do not restore built-in or parameter type reconstruction in codegen,
overload structural token links, or introduce a backend-owned type table.
