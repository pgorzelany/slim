# D0079: Close Core 1J structured concurrency

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 0
Dogfood: 0
Score: 70

## Need

D0078 admits one explicit lexical fork only after the production checker,
lowering, runtime, applications, and permanent budgets demonstrate the complete
contract. Core 1J also needs an explicit stopping boundary so latency evidence
does not grow into futures, cancellation state, locks, channels, or a scheduler.

## Alternatives

Leaving the milestone open would make every concurrency mechanism appear
provisional. Adding task handles or cancellation would create source states
that neither maintained application needs. Inferring independence between
external effects would be unsound. The accepted boundary closes on exactly one
two-call wrapper and retains Core 1G automatic parallelism independently.

## Costs

Explicit tasks are direct leaf calls with scalar or byte-view inputs, no
exclusive borrow, no declared `partial`, and at least one bounded clock or TCP
operation. The form is restricted to a function's leading immutable `let`
chain. Every started child is joined; failure to spawn executes inline.
Effectful task allocations use separate regions and are adopted only after the
join. There is no user-visible cancellation state: bounded operations complete
or return typed failure, after which both results are installed in lexical
order.

The allocation status shared by task regions uses C11 atomics. Programs without
an executable site emit no task marker, wrappers, regions, or link flags.

## Evidence

The `dual_fetch` and `dual_health` loopback applications retain identical
forced-serial and POSIX-worker builds. Their accepted same-host
parallel/serial ratios are 0.529 and 0.534, with permanent 0.75 limits.
Conformance fixes the form and `E0356`; integration tests cover owned result
adoption, serial fallback, allocation failure, and post-join use. Existing
runtime tests retain spawn and join fault injection and unsupported-target
fallback.

The complete conformance corpus, 2,000 deterministic malformed-input
mutations, fixed-point bootstrap, sanitizers, scaling, and prior performance
gates remain release requirements.

## Removal

Remove explicit `fork` if a supported target cannot preserve bounded lexical
join, serial equivalence, deterministic result installation, or isolated
allocation ownership. Any broader task shape requires separate application
evidence and a new scored decision. Compatibility aliases and unstructured
escape hatches are forbidden.
