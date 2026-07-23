# D0037: Linked binding types in code generation

Status: accepted
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

Variant-match emission derived a scrutinee's type by scanning only the current
function parameters. The typed checker correctly accepted a variant copied into
a lexical `let`, but code generation then produced invalid C type and tag names.
This was a semantic/backend mismatch on an ordinary bound value.

Type analysis already annotates every parameter and lexical binding use with an
encoded source-type token link. Under D0036, code generation receives that exact
checked token stream. Variant-match emission now decodes the binding link in
constant time and the parameter-only scan is removed.

## Alternatives

Extending the scan to every enclosing `let` would duplicate name resolution and
remain proportional to lexical depth. Passing the full typed view through the
recursive backend is the eventual route for arbitrary expression queries, but
it is a much larger migration and currently crosses a measured compiler-source
performance cliff. Rejecting matches over locals would make a valid, useful
composition needlessly inexpressive.

Consuming the binding link fixes the proven gap with less code and no new
language feature.

## Costs

Variant match code generation requires its scrutinee to be a bound value. This
now includes parameters, lexical locals, and match payload bindings. Direct
variant-producing expressions still require typed-fact consumption and are not
claimed by this decision.

A trial that annotated all composite expression tokens and lowered arbitrary
variant scrutinees was reverted. The annotation could be structured within the
normal self-check envelope, but the generalized nested-expression lowering made
self-validation exceed 12 seconds. No unused expression annotation remains.

## Evidence

The executable `variant-local-match` fixture copies a variant parameter into a
local, matches the local, binds its payload, and prints `42`. It previously
passed checking but emitted invalid C; it now compiles and runs through the
self-hosted toolchain. The original parameter-based data fixture remains green.

The self-check remains about 0.22 seconds. Removing the parameter scan shrank
the byte-identical fixed-point seed from 1,653,599 to 1,650,892 C bytes. All 90
fixtures and 2,000 deterministic malformed-input mutations pass. Governance
requires linked type consumption and rejects restoration of the parameter scan.

## Removal

Replace this lookup only with a bounded typed-fact query that handles bound and
arbitrary expression scrutinees, preserves the executable fixture, does not
restore lexical or parameter scans, and passes the same compile-time,
bootstrap, and conformance gates.
