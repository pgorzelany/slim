# D0056: Typed match scrutinees

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

Variant-match lowering still decoded a source-type token from the older packed
local-binding link, and match kind was inferred from whether the first pattern
was spelled `true` or `false`. This worked for bound variants but not arbitrary
checked expressions. A direct `(case ...)` scrutinee passed type checking and
then trapped the compiler while code generation tried to decode a local link
from a form token.

Match lowering now queries the checked fact for the scrutinee token. The type
selects Boolean or variant lowering, provides the variant declaration type, and
types a temporary whenever the scrutinee is a computed form. The legacy packed
type decoder and pattern-spelling kind inference are removed.

## Alternatives

Restricting scrutinees to bound names would expose a special-case language rule
solely for the backend. Extending the packed-link decoder to forms would create
a second expression annotation domain beside dense facts. Inspecting a
constructor name or first pattern would continue to infer semantics from syntax
after the type checker had already established the exact scrutinee type.

The fact table already crosses every recursive match-emission boundary, so this
change adds no new plan, vector, field, or threading parameter.

## Costs

Every match performs one existing constant-time fact lookup and closed type-kind
mapping. A computed variant scrutinee now gets the same scoped temporary already
used for computed Boolean scrutinees; bound values emit unchanged C. No syntax,
runtime representation, ownership rule, or memory-management behavior changes.

Pattern payload bindings remain typed from the linked case declaration because
they are binding declarations, not evaluated expression roots. That structural
contract is distinct from scrutinee expression typing.

## Evidence

The D0055 compiler accepts the new direct computed-variant fixture at check time
but traps during emission with `I64 division by zero`. The candidate compiles
and runs the same source, printing `42`. Existing bound Boolean/variant,
aggregate, allocation, recurrence, storage, lifetime, and project programs
produce C byte-identical to D0055.

The candidate validates the self-host project in about 0.11 seconds and reaches
a byte-identical fixed point. All 101 fixtures, 2,000 deterministic mutations,
performance budgets, sanitizers, allocation-failure injection, native
challenges, and deterministic bootstrap verification pass. The portable fixed
point is 1,627,142 generated C bytes with SHA-256
`c9ce8ca18be88c8fbd03b31e3231737cc641cc1e0476748ac8aa4b288869a031`.

## Removal

Replace scrutinee facts only with another bounded authoritative expression-type
query that supports arbitrary checked Boolean and variant expressions. Do not
restore packed binding-type decoding in codegen, pattern-spelling type
inference, or a bound-name-only restriction.
