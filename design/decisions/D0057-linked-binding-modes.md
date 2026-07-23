# D0057: Linked binding modes

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

The checked token stream retained exact local declaration identity and type, but
C generation still determined whether every named value was an `inout` pointer
by rescanning the current function's parameter list by spelling. A function
with many borrowed parameters and many reads therefore performed quadratic
ordinary-generation work after lexical binding had already resolved each use.
The typed borrowed-return check repeated the same scan at its final boundary.

Local token links now pack one binding-mode bit beside the existing type and
declaration token. A single typed query decodes whether a linked use is `inout`.
Code generation and the borrowed-return check consume that query, removing both
parameter-list rescans.

## Alternatives

Adding a Boolean to every syntax token would enlarge the common representation.
Retaining the analyzer's mutable `Binding` vector in every checked artifact
would keep moved-state implementation details alive after checking and require
another token-to-binding query. A spelling cache would duplicate lexical
identity and need shadowing invalidation. Reusing an unused sign or sentinel
would make the existing type/declaration encoding ambiguous.

The packed local-link domain is already disjoint from declaration links. Its
declaration remainder remains unchanged; the quotient becomes
`type * 2 + mode`, so move tracking continues to recover exact binding identity
without another field.

## Costs

Link construction adds one multiply, addition, and mode bit per binding. Type
decoding adds one division by two, and mode queries add one remainder. All are
constant-time compiler operations bounded by the validated token count. The
factor-of-two packing remains far below signed 64-bit limits for any token
vector that can fit in memory.

The memory planner still classifies `inout` escape targets while building the
authoritative plan; it cannot depend on `typing` without creating a module
cycle. This decision removes redundant typed/code-generation rescans, not
syntax inspection required to construct a separate checked plan.

Generated C, ABI pointer representation, alias rules, ownership diagnostics,
runtime state, and source syntax are unchanged.

## Evidence

The candidate validates the self-host project in about 0.11 seconds and reaches
a byte-identical candidate fixed point. Existing `inout`, recurrence,
ownership-error, match-binding, aggregate, allocation, and project programs
produce C or diagnostics byte-identical to D0056.

`generated-inout-binding-reads` permanently emits geometrically wide borrowed
parameter lists and one read per parameter. It protects both the packed-mode
producer/consumer schema and removal of the former scan. The former compiler
measured an endpoint exponent of 1.298 and failed the new 1.25 budget; the
release compiler measures 5.383, 8.679, 18.479, and 47.779 milliseconds from
125 through 1,000 parameters, exponent 1.050.

The four typed-view emit series now share balanced ascending/descending rounds
and an even-sample median. This preserves every accepted budget while keeping
machine drift from being misclassified as program-size growth. The release
gate passes all 101 fixtures, 2,000 deterministic malformed-input mutations,
performance budgets, sanitizers, allocation-failure injection, native
challenges, and bootstrap verification. The byte-identical portable seed is
1,629,310 generated C bytes with SHA-256
`90f60cadb6d845ed9cfe6bd58a81578d8cc9fd9f112690c3e42f9d03dc15b2c7`.

## Removal

Replace the mode bit only with another bounded exact-binding query that handles
shadowing, preserves type and declaration identity, and passes the inout-read
scaling gate. Do not restore parameter spelling scans in typing or codegen, add
a field to every token, or retain mutable checking state solely for lowering.
