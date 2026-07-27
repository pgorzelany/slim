# RFC-0066: Observed effects and total recurrence

Status: accepted
Implementation: complete
Process: legacy
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

RFC-0065 proves that every original application function has at least two
parallel blockers. Two conservative rules also hide useful positive evidence.
First, a declared effect is a checked capability ceiling, not evidence that an
effect occurred in this compiled body. Second, every `recur` is rejected even
when the checked body contains a simple total descent to an exact bound.

The analysis now classifies observed allocation, I/O, mutation, checked traps,
exclusive borrows, and unresolved callees directly. A declaration that grants
unused `alloc`, `io`, or `partial` capability does not by itself make current
execution observable. Any body edit is rechecked and reanalyzed before code is
generated.

The shared integer view also proves one canonical tail-recurrence shape. The
function must match an I64 parameter against an exact lower bound with
`i64.le`; the true arm is total; the false arm contains only total immutable
bindings followed by one tail `recur`; the corresponding argument subtracts a
positive exact step from that same parameter; and every recurrent argument is
total under the false-arm refinement. The checked subtraction proves no
underflow. Strict descent over finite I64 values then proves termination.

## Alternatives

Adding loop, parallel, range-contract, or effect-inference syntax would create
permanent surface before the existing forms were understood. Treating all
recurrence as total would admit divergence. Trusting a declared empty effect
list would still miss checked traps and mutation. Treating every granted
capability as an event prevents optimization based on the implementation the
compiler is actually emitting.

General ranking functions, relational abstract interpretation, and
interprocedural specialization remain larger future options. The accepted
shape is deliberately one-way, syntactic, and replayed from checked node links
and range facts.

## Costs

The explicit `analyze` command performs one bounded recurrence inspection per
function after its ordinary range walk. It stores no new per-node vector and
adds no ordinary compilation or program runtime work. Division and remainder
totality now depend on a bounded nonzero denominator and the sole I64 overflow
case, `MIN / -1`; remainder bounds derive from the denominator magnitude.

The eleven-category blocker vocabulary remains stable. The
`declared-effects` category is retained for schema compatibility but is empty
when the complete checked implementation is available. Actual allocation and
I/O remain `allocation-or-io`; missing bodies or exceeded graph bounds remain
unknown.

## Evidence

Focused fixtures prove arbitrary-I64 remainder by 3 total with result
`-2..2`, division by 2 total with unknown result bounds, and division by -1
unknown because `I64::MIN` remains possible. They prove a guarded decreasing
countdown total while an equality-guarded recurrence remains unavailable.
Overdeclared pure code is safe; real allocation remains unavailable.

The permanent `state_machine` application performs two independent
two-million-step finite-state computations. `run` is proven total despite its
`partial` capability, both `run` and `same` are safe, and one exact fork site is
reported. SLIM, C11, and Rust produce identical output and have a permanent
native runtime ratio gate.

Schema 3 covers thirteen applications. `variants/command` becomes the first
safe function in the original corpus; `state_machine` contributes two safe
functions and one eligible site. Analysis scales at exponent 0.890 through
8,000 generated declarations, below the permanent 1.25 limit. Acceptance also
requires deterministic reports, all negative cases, bootstrap fixed point,
conformance, malformed-input mutation, sanitizers, allocation failure, and
native performance gates.

## Removal

Never discharge a declared capability when the checked body is unavailable or
the observed call graph is incomplete. Never prove recurrence from direction
alone: exact guard, same linked parameter, positive step, total update, total
arguments, unique tail recurrence, and total base/prefix are all required.
Unsupported guards, multiple recurrent branches, user calls, collection
accesses without bounds, and saturated evidence remain unknown.
