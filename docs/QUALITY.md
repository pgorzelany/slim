# Bounded program evidence

Status: Core 1B internal tool specification

Core 1B adds evidence about checked canonical SLIM without changing Core
syntax. Every result is classified as one of:

- `exact`: established for the complete stated domain;
- `bounded`: complete only within an explicit fixed limit; or
- `unknown`: the compiler lacks a required bound, invariant, or supported form.

The tools never turn `unknown` into a negative quality judgment. They emit a
vector of facts rather than a universal goodness score.

## Analysis version 2

`slimc analyze SOURCE` emits `(analysis 2 ...)`. It retains Core 1A binding
identity, declared type, storage ownership, use, last-use, scope, and dependency
facts. It adds:

- exact token, declaration, function, record, and variant counts;
- exact declared effect and structural mutation/call/match/recur counts;
- allocation-site, checked-trap-site, and partial-operation counts;
- an exact `total` classification only for call-free, recursion-free finite
  expressions, and `unknown` for calls or recursion;
- bounded owned-binding and maximum-live-owned pressure;
- exact primitive record state models when every field has a known
  power-of-two cardinality; and
- explicit unknown state models for dynamically sized or unresolved storage.

State models describe representable values, not valid application states. A
valid-state ratio requires an invariant or behavioral specification that Core
1B does not infer.

Default analysis stores at most 64 binding facts per function. Pairwise
live-range pressure therefore has a fixed maximum of 4,096 comparisons.
Structural metrics use one token traversal. Exceeding a bound marks the result
`bounded` or `unknown`; it never silently shortens a lifetime or claims an exact
result.

## Reduction evidence and replay

`slimc prove-reduction SOURCE` reports at most 64 directly justified rewrite
sites as `(rewrite NODE RULE JUSTIFICATION)`, where `NODE` is a stable token
identity in the checked source. The report also records canonical source and
normal-form byte/token costs and the fixed eight-pass limit.

`slimc verify-reduction ORIGINAL REDUCED` recomputes the unique Core 1A normal
form and reports `verified` only when `REDUCED` is its exact canonical bytes.
It does not trust proof text as executable input.

## Finite equivalence

`slimc equivalent LEFT RIGHT` compares the function named `subject` in two
checked standalone modules. Core 1B accepts only:

- zero through eight parameters, all `Bool`;
- a `Bool` result and an empty `(effects)` declaration;
- at most 256 tokens in the subject expression; and
- Boolean literals/names, immutable Boolean `let`, `bool.not`, `bool.and`,
  `bool.or`, and exhaustive Boolean `match`.

The tool enumerates all `2^N` inputs in canonical false-before-true order. It
emits `equivalent`, `different` with the first concrete counterexample, or
`unknown` with a stable reason. No result applies to arbitrary SLIM programs.

## Structural edit protocol

`slimc edit SOURCE PATCH` accepts one non-executable patch:

    (slim-edit 1 (node TOKEN_INDEX) (replace FORM))

`TOKEN_INDEX` must be the start of an existing form and `FORM` is limited to 64
tokens. The tool replaces exactly that form and emits canonical SLIM. The root
launcher checks the candidate with the normal compiler before publishing it.
The identity is stable within the exact canonical `SOURCE` supplied to the
command; patches are not portable across independently edited source versions.
The patch format is tooling data: it is never accepted by `slimc check`, never
defines types or effects, and is not an alternative program representation.

## Agent benchmark boundary

`slim-bench agent` records source bytes, language-neutral lexical tokens, a
clearly labelled bytes-per-four model-token proxy, local edit span, compiler
acceptance before and after a repair, and diagnostic bytes for matched
SLIM/C/Rust fixtures. Native performance and binary size remain the independent
`slim-bench compare` measurements.

These measurements can falsify token-economy and repair-locality claims. They
do not substitute for controlled model-generation trials and must not be
reported as an LLM success rate.
