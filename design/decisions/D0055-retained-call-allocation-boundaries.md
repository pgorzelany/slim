# D0055: Retained call allocation boundaries

Status: accepted
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

D0047 made the checked memory plan retain every call site that can propagate an
allocation failure, and function emission already used the retained vector to
decide whether it needs a failure label. User-call emission nevertheless
reread the callee parameter/effect declaration at every call to decide whether
to branch to that label. The planner and backend shared one canonical effect
query, but code generation still repeated semantic work after checking.

The memory module now exposes a bounded query from call-form token identity to
its retained allocation region. Ordinary expression emission receives the
current function's allocation-site vector, and user calls derive their failure
boundary only from that query. The backend effect-list read is removed.

## Alternatives

A sequential allocation cursor would be smaller but incorrect: token indices
are stored in lexical preorder while computed arguments are emitted before
their enclosing call. A dense token-sized allocation table would make lookup
constant-time but spend one word per token for a sparse Boolean/region fact.
Overloading syntax links would collide with declaration, binding, member, and
case identity. Passing a full `FunctionPlan` by value through recursive emission
would copy unrelated planning handles and summaries.

`AllocationPlan` entries are already appended in strictly increasing token
order, so binary search preserves sparse storage and supports arbitrary
evaluation order without a new structure.

## Costs

Seventeen internal expression-emission boundaries carry one additional
`inout` vector handle. Each user call performs a logarithmic search over the
current function's allocation-capable call sites; functions without such sites
return immediately. The query is reentrant and allocation-free. Built-in
lowering keeps its direct failure checks, while the retained plan remains the
single source for user-call propagation.

Generated C, failure ordering, region choice, runtime state, and source syntax
are unchanged. The plan continues to retain the region even though this cut
only needs presence; later region-sensitive lowering can consume the same
query without changing its identity contract.

## Evidence

The candidate validates the self-host project in about 0.11 seconds and reaches
a byte-identical candidate fixed point. Representative nested-call,
recurrence, storage, allocation, aggregate, lifetime, project, and typed-fact
programs produce C byte-identical to D0054. A dedicated executable fixture
nests allocation-capable user calls, prints `42`, and pins the mismatch between
lexical site order and evaluation order.

`generated-planned-allocation-calls` permanently emits geometrically nested
allocation-capable calls and enforces a 1.25 endpoint exponent budget. D0054
measured 3.200, 5.265, 12.718, and 38.013 milliseconds from 125 through 1,000
calls, exponent 1.190. The candidate measured 3.065, 5.334, 12.934, and 39.125
milliseconds, exponent 1.225. A preceding pass stopped on the existing
computed-argument gate at 1.251; its budget was preserved and the rerun passed.

The authoritative release run measures 3.229, 5.366, 12.737, and 38.688
milliseconds, exponent 1.194. All 100 fixtures, 2,000 deterministic mutations,
performance budgets, sanitizers, allocation-failure injection, native
challenges, and deterministic bootstrap verification pass. The portable fixed
point is 1,628,984 generated C bytes with SHA-256
`eb1c5062d43c4a5fdf5e41572235b7d439a91deac8f4698b0684888e4bfcac97`.

## Removal

Replace the sparse query only with another bounded, reentrant lookup keyed by
exact call-form token identity that preserves arbitrary evaluation order and
all allocation-failure behavior. Do not restore effect-list parsing in codegen,
use a sequential lexical cursor, overload token links, or add a second
allocation authority.
