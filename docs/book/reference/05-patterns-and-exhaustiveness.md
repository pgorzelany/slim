# Patterns and exhaustiveness

This chapter specifies Boolean and variant matching, arm order, payload
bindings, result agreement, and the requirement for complete coverage.

## Match domains

A match scrutinee is either `Bool` or a declared variant. Other types do not
gain a general pattern protocol.

Boolean matches cover `false` and `true`; their arm order is irrelevant because
`Bool` has no user-declared source order. Variant matches cover every declared
case in declaration order.

## Variant patterns

A variant arm names one case and binds its payload values with the types fixed
by the declaration. Payload arity and types are exact. An arm cannot name an
unknown case or repeat a case.

## Exhaustiveness

Every match covers its complete closed domain. Missing Boolean coverage is
`E0336`; missing variant coverage is rejected with the stable identity recorded
by the conformance corpus. The compiler does not add a hidden default arm.

## Arm order and uniqueness

Canonical order is semantic surface: Boolean arms and variant arms appear in
their required order, and duplicates are rejected. This keeps formatting,
diagnostics, analysis, and generated output deterministic.

## Result type

Every arm produces the same result type. Ownership transfers in an arm follow
the ordinary affine rules, and payload lifetimes cannot escape their valid
region.

## Evaluation

The scrutinee is evaluated exactly once. Only the selected arm executes.
Effects, traps, moves, mutation, and allocation behavior before selection and
inside the selected arm remain observable.

## Deliberate boundary

There are no wildcard, guard, range, structural, open-variant, reflection, or
user-defined matcher forms. Closed exhaustive matching is the sole accepted
selection mechanism for variant values.

## Normative boundary

Exact forms are in [Core](../../CORE.md). Diagnostic codes, spans, and recovery
ordering are in [Diagnostics](../../DIAGNOSTICS.md).
