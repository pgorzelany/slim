# D0054: Typed aggregate temporaries

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

After D0053, computed call and recursive-transfer temporaries consumed the
checked dense expression-fact table, but three ordinary lowering paths still
reconstructed types from context. Record construction reread a linked field
declaration, variant construction walked payload declarations, and a computed
Boolean match scrutinee used a backend scalar constant. These paths were valid,
but duplicated the type checker's conclusion and left future type changes able
to diverge between acceptance and C lowering.

Record field values, variant payload values, and computed match scrutinees now
query their expression token through `fact_type_index`. Record and variant
declaration traversal remains authoritative for field/case identity, order,
arity, and emitted layout. Facts determine only the type of the temporary that
holds an already checked expression.

## Alternatives

Removing declaration traversal entirely would discard useful structural
validation and confuse expression typing with aggregate layout. Copying types
onto field and payload syntax links would overload links that retain member
identity. Adding a specialized aggregate type table would duplicate the dense
fact view. Keeping the contextual paths would preserve three representations
of an expression type in the backend.

## Costs

Each computed aggregate value and computed Boolean scrutinee performs one
bounds-checked dense fact lookup and the existing closed type-kind mapping.
Payload declaration cursors still advance linearly so generated layout stays
canonical. No source syntax, runtime state, generated data representation, or
ownership rule changes.

The match-arm payload bindings themselves keep their declaration types: pattern
names are bindings rather than evaluated expression roots and therefore do not
have expression facts. Let annotations and result destinations likewise remain
explicit source or caller contracts rather than rediscovered expression types.

## Evidence

The candidate validates the self-host project in about 0.11 seconds and reaches
a byte-identical fixed point. All 99 fixtures and 2,000 deterministic
malformed-input mutations pass. Representative nested-call,
recurrence, storage, record, variant, lifetime, local-variant, project, and new
typed-temporary programs produce C byte-identical to D0053. The dedicated
fixture combines named and scalar computed record fields, named and scalar
computed variant payloads, and a computed Boolean match scrutinee.

`generated-aggregate-temporaries` permanently emits geometrically wide record
and variant constructions and enforces a 1.25 endpoint exponent budget. The
candidate measured 3.288, 5.058, 8.128, and 14.524 milliseconds from 125 through
1,000 field/payload pairs, an endpoint exponent of 0.714. D0053 measured 3.795,
4.995, 8.264, and 14.689 milliseconds on the same workload. A release-confirming
rerun measured 3.783, 5.079, 8.222, and 14.279 milliseconds, exponent 0.639.

The first complete release attempt retained the 1.25 budget and stopped on an
unrelated noisy nested-binding endpoint of 58.144 milliseconds, exponent 1.252.
The immediate rerun and complete rerun measured 22.501 and 23.006 milliseconds
at that endpoint and passed without changing the budget. The complete rerun had
an isolated 17.240-millisecond first aggregate sample; it is recorded rather
than used as scaling evidence. All performance budgets, sanitizers,
allocation-failure injection, native challenges, and deterministic bootstrap
verification pass. The portable fixed point is 1,623,453 generated C bytes with
SHA-256
`fed3fc3edb035a6960a33d8986842b2366eeda8122079d0ecc487fb2176165e8`.

## Removal

Replace these fact queries only with another bounded, reentrant path to the same
checked expression types while retaining independent structural layout
validation and the aggregate-temporary scaling gate. Do not restore backend
temporary typing from field, payload, or hard-coded match context, overload
syntax links, or add a second backend type table.
