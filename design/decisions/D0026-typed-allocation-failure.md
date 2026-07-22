# D0026: Typed allocation-effect failure

Status: accepted
Kind: runtime
Primitive: none
Safety: 2
Compile: 1
Runtime: 1
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

Allocation exhaustion currently calls the untyped runtime trap from operations
such as `vec.push`, even though their source result has no failure channel.
Adding fallible aliases would duplicate every allocation operation, and changing
all container results would add large permanent surface before Core has generic
sum types. Core already has the explicit `alloc` effect, so allocation failure
can be a typed outcome of that one effect.

Every region therefore carries one `SlimAllocStatus` with the states `ok` and
`exhausted`. Allocation-capable runtime operations set the status and return a
success bit without committing a partial mutation. Generated code tests the
status immediately and propagates it through `alloc` functions to the entry
wrapper, which reports a deterministic diagnostic and exits with code 71.
Ordinary source results remain valid only on the `ok` path. This is one typed
effect channel, not a second source operation.

## Alternatives

Trapping retains no typed distinction and prevents deterministic testing.
Nullable vectors, invalid typed IDs, or negative lengths would add invalid
states to safe values. Per-operation `try-` aliases duplicate semantics. A new
generic `Result` type and explicit propagation syntax would be useful only if
evidence shows programs need to recover locally; the compiler corpus currently
propagates allocation failure to its process boundary. `setjmp` or exceptions
would hide non-local control flow and skip lexical destruction, so propagation
is ordinary generated branches through the declared `alloc` effect.

## Costs

An allocation attempt performs one predictable status check and increment.
Only `alloc` functions contain failure branches; pure functions keep their
existing control flow. The internal C ABI carries the region, which already
contains the status. The entry wrapper reads the test-only
`SLIM_ALLOC_FAIL_AT` process setting as a positive allocation ordinal; absent
or invalid input disables injection. No environment operation is exposed to
SLIM source.

Core 0.4 does not permit local recovery from exhaustion. The process boundary
is the sole handler, while the typed status and reverse region cleanup preserve
the option to add one general effect handler later if self-host evidence pays
for it.

## Evidence

Fault-injected native tests fail every allocation ordinal exercised by bounded
programs. Each failure must exit 71, emit the same diagnostic with the selected
ordinal, preserve runtime safety under ASan and UBSan, and destroy all active
regions. Success output and artifacts remain unchanged when injection is
absent. Stage 0 and the self-hosted compiler generate the same propagation
boundary, and the compiler fixed point runs with the channel enabled.

## Removal

Replace this channel only with one smaller typed allocation-effect outcome that
preserves immediate propagation, atomic container mutation, deterministic
fault injection, and lexical cleanup. Do not retain both process propagation
and trapping allocation paths, and do not add fallible aliases to the source
surface.
