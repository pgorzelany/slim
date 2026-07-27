# RFC-0052: Dense expression facts

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 1
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

The typed view previously appended valid expression facts in recursive
completion order. A consumer could enumerate them, but finding the type for a
particular token required a linear search and the postorder vector could not be
binary-searched by token identity. Threading that representation into recursive
code generation would turn repeated type queries quadratic.

The fact vector is now dense over the validated token vector. Its index is the
expression token identity, and each compact `Fact` stores only a `TypeRef`.
Initialization installs an invalid type for every token; successful inference
replaces the slot with `vec.set`. `fact_type` checks the index bounds and returns
the slot in constant time. `finish_type` rereads every stored result through
that query, so all existing checking and scaling workloads continuously execute
the indexed path.

## Alternatives

Sorting the former postorder facts would add another pass and either require a
general sort facility or make insertion superlinear. A hash table would add a
larger compiler primitive, nondeterministic iteration concerns, and unnecessary
per-entry key storage for an already dense integer domain. Publishing types in
syntax-token links would overload links that already carry declarations,
bindings, fields, cases, and packed local identity.

Keeping the expression token redundantly inside each dense fact would spend one
machine word per token without adding identity: vector position is the key.
Keeping the sparse bag until a backend migration would leave the central
complexity risk untested.

## Costs

The compiler retains one two-word `TypeRef` per lexical token, including tokens
that are not expression roots, and performs one linear initialization pass.
This is a predictable memory-for-query-time trade: storage remains proportional
to the already bounded module, and generated programs and the runtime are
unchanged. Invalid or out-of-range queries return the existing invalid type
sentinel rather than trapping.

This decision establishes storage and query semantics; it does not claim that
ordinary code generation consumes expression facts yet. Backend migration must
use this single table and preserve the recursive source-shape performance gate.

## Evidence

The candidate validates the self-host project in about 0.10 seconds and reaches
a byte-identical candidate fixed point. All 98 conformance fixtures and 2,000
deterministic malformed-input mutations pass.

The permanent quick performance workload exercises `fact_type` once for every
successfully inferred expression. A repeat run measured named-type series of
2.471, 2.816, 4.221, and 6.184 milliseconds and nested-binding series of 4.404,
6.398, 11.333, and 22.383 milliseconds for 125 through 1,000 units, below their
1.25 exponent budgets. The first run had a transient 34.162-millisecond final
named-type sample and correctly failed at exponent 1.269; it is recorded rather
than used to weaken the gate. The complete release run remains authoritative.
The portable fixed point is 1,625,465 generated C bytes with SHA-256
`d57893e96f6321aa3b5b1df6a66d28ce31ff9ced4393b3956fa642494ee3adf0`.
The authoritative release series measures 2.665, 2.994, 3.975, and 6.309
milliseconds for named-type parameters and 4.222, 6.752, 11.723, and 22.265
milliseconds for nested bindings. The complete gate passes all scaling and
same-host budgets, sanitizers, allocation-failure injection, native challenges,
and deterministic bootstrap verification.

## Removal

Replace the dense table only with a representation that provides deterministic
bounded lookup by expression token, retains the same strong type result, and
passes the existing adversarial scaling gates. Do not restore a sparse linear
search, overload syntax links, or introduce a second backend type table.
