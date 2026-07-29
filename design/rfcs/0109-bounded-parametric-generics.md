# RFC-0109: Bounded parametric generics

Status: rejected
Implementation: not-planned
Process: 1
Audience: both
Author: SLIM project
Created: 2026-07-28
DecisionDate: 2026-07-29
Approver: project-maintainer
Kind: language
Primitive: parametric-generics
Safety: 0
Compile: 0
Runtime: 2
Minimal: -1
Analysis: 0
Dogfood: -2
Score: 5

## Summary

This RFC proposed generic functions, structs, and enums:

```slim
struct Box[T]:
  value: T

enum Maybe[T]:
  None
  Some(T)

fn identity[T](value: T) -> T:
  value
```

The proposal is rejected in its current form. The experiment worked
technically, but real compiler use removed too little code to justify the
permanent language and compiler cost.

No generic syntax, semantics, diagnostics, specialization machinery, examples,
or compatibility changes from the experiment are merged.

## Motivation

The proposal aimed to replace repeated type-specific declarations without
runtime type objects, boxing, indirect calls, or extra allocation.

Potential library types included `Maybe[T]`, `Result[Ok, Err]`, and generic
containers. The open question was whether those possibilities translated into
enough value in actual SLIM programs and the self-hosted compiler.

The experiment branch was created to answer that question before accepting a
permanent feature.

## Guide-level explanation

The tested design allowed type parameters on declarations and inferred generic
function arguments from ordinary values:

```slim
struct Pair[Left, Right]:
  left: Left
  right: Right

fn first[T](values: Vec[T]) -> T effects[partial]:
  vec.get(values, 0)
```

Named generic types wrote all their arguments. Function calls did not have an
explicit type-argument spelling.

A generic body treated an unknown `T` conservatively because it might own
storage. Concrete specializations then used the normal concrete layout and
ownership classification.

This surface is recorded only to explain the evaluated design. It is not
supported language syntax.

## Reference-level specification

The experiment implemented:

```text
fn name[T, U](...)
struct Name[T, U]:
enum Name[T, U]:
Name[I64, Bool]
```

It included exact inference, nested generic data, cross-module interfaces,
concrete specialization, finite specialization work limits, and stable
candidate diagnostics.

It deliberately excluded defaults, traits, constraints, higher-kinded types,
callable parameters, explicit call-site type arguments, generic effects, and
numeric parameters.

Because the RFC is rejected, this section creates no language contract.

## Compiler and runtime design

The experiment checked a generic declaration once and emitted one concrete C
version for each used type combination. Repeated combinations shared a
specialization. Calls remained direct, and unused generic declarations emitted
no runtime code.

Specialization work was deterministic and bounded. A command could authorize a
larger finite limit, but there was no unlimited mode.

`Vec[T]`, `Arena[T]`, and `Id[T]` participated in the tested type-application
and inference machinery while retaining their existing intrinsic
representations.

The design achieved its runtime goal, but required substantial parser, type
checker, ownership, interface, cache, code generation, diagnostic, and
benchmark machinery.

## Compatibility and migration

There is no migration. Existing source, interface schema 2, cache schema 2,
manifest schema 1, diagnostic schema 1, and runtime ABI 1 remain unchanged.

The rejected syntax receives no compatibility promise. A future proposal may
choose a different surface and must be evaluated independently.

## Diagnostics and failure cases

The experiment covered malformed parameters, arity errors, incomplete or
conflicting inference, concrete ownership errors, and exhausted specialization
work.

Candidate diagnostic codes `E0360`, `E0361`, and `E0362` are not added to the
accepted diagnostic ledger.

The experiment also exposed a pre-existing ownership gap: a concrete affine
element can be shallow-read through an `inout` collection. The generic
experiment rejected the abstract equivalent conservatively, but did not supply
the borrowing or move-out model needed to solve the underlying problem.

That ownership issue remains separate work. It is evidence against accepting
this proposal before collection ownership is better defined.

## Performance and complexity

The implementation used the real self-hosted compiler and reached a
deterministic fixed point.

| Measure | `main` | Generic experiment | Change |
| --- | ---: | ---: | ---: |
| Fixed-point compiler C | 2,743,494 bytes | 3,116,286 bytes | +13.6% |
| Self-hosted source before/after dogfood | 859,230 bytes | 859,145 bytes | -85 bytes |
| Self-hosted lines before/after dogfood | 19,603 | 19,600 | -3 |

The strongest natural compiler use replaced two substitution records with one
generic `Entry[T]`, instantiated for a type reference and an integer type
index. That refactor removed only 85 source bytes and three lines. No other
same-shaped compiler records could be safely unified without erasing useful
domain distinctions.

Specialization itself scaled approximately linearly in the final experiment:

| Concrete versions | Source bytes | Emit time |
| ---: | ---: | ---: |
| 125 | 16,170 | 5.303 ms |
| 250 | 32,670 | 8.107 ms |
| 500 | 65,670 | 15.894 ms |
| 1,000 | 131,670 | 36.893 ms |

The measured exponent was 0.93. Paired compiler measurements did not show a
stable runtime or compile-time regression, and representative generated
programs retained direct concrete C.

The rejection is therefore not based on poor asymptotic behavior or runtime
dispatch. It is based on the amount of permanent implementation and language
surface required for the small demonstrated source benefit.

## Alternatives and drawbacks

A smaller standard-library experiment was considered. `Maybe[T]`,
`Result[T, E]`, and `NonEmpty[T]` are plausible generic data types, but the
larger expected wins were not available:

- generic collection reads need sound element borrowing or a `Copy`-like
  constraint;
- affine collection algorithms need a safe move-out operation;
- queues, maps, sets, arenas, and typed identifiers need opaque fields;
- generic `map`, `filter`, `fold`, and `sort` need callable parameters or
  constraints; and
- a user-defined phantom `Id[T]` conflicts with the experiment's rule that
  every type parameter must appear in stored data.

Adding those capabilities merely to justify generics would compound the
feature cost before demand was established.

Handwritten domain types remain more verbose, but preserve meaningful names
and prevent accidentally mixing same-shaped values. Ordinary non-generic
helpers remain preferable where the value types are already identical.

## Test and acceptance plan

The rejected experiment passed bootstrap, governance, 40 end-to-end tests, 156
conformance fixtures, 2,000 deterministic malformed-input mutations,
performance, reduction, parallelism, native comparison, and agent
measurements.

Passing tests established that the implementation was credible. It did not
establish that the feature was valuable enough to retain.

A future generics RFC should start from new evidence and must demonstrate:

- substantial reduction in real library or application code;
- a sound collection ownership story;
- useful abstractions that preserve domain distinctions;
- bounded approximately linear compilation;
- no runtime mechanism for unused or direct generic code; and
- a materially better benefit-to-complexity ratio.

## Ratings and evidence

Safety is `0`: the generic checks were conservative, but the experiment exposed
an unresolved collection ownership prerequisite.

Compile is `0`: specialization was bounded and near-linear, but the compiler
implementation grew substantially.

Runtime is `+2`: generated calls and layouts were concrete, with no runtime
generic mechanism.

Minimality is `-1`: the permanent surface and compiler machinery outweighed
the demonstrated source reduction. Analysis is `0`. Dogfood is `-2` because
the self-hosted compiler found only one small natural use.

The weighted score is
`(0*20 + 0*20 + 2*20 - 1*20 + 0*15 - 2*5) / 2 = 5`.

## Decision

Rejected by the project maintainer on 2026-07-29.

The implementation proved that bounded monomorphized generics were feasible,
deterministic, and runtime-free. It did not prove that they carry their weight
in SLIM:

- the fixed-point compiler grew by 13.6%;
- compiler dogfooding removed only 85 source bytes and three lines;
- most compelling standard-library uses depended on additional unimplemented
  ownership, visibility, constraint, or callable features; and
- the experiment exposed an ownership problem that must be solved separately.

The RFC document is retained as evidence. The implementation, compiler seed,
tests, examples, diagnostics, schema changes, and benchmark additions are
discarded and do not merge.

## Implementation

Not planned. A complete branch-local experiment was used to evaluate the
proposal, but its production changes were intentionally dropped after the
rejection decision.

Rust was used only for independent verification and measurement; the
experiment itself ran through the self-hosted production compiler path.

## Removal and supersession

There is no production feature to remove.

A future proposal may revisit generics only with new evidence. It should not
treat this rejected syntax, specialization policy, or diagnostic set as an
accepted starting point, and it must not preserve them as aliases.
