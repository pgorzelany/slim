# D0102: Indexed form boundaries

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

`syntax/skip_form` previously found the token after a compound form by scanning
its complete nested token subtree. A single skip is bounded, but repeated
structural passes turn deeply nested source into superlinear compiler work.
The permanent computed-argument and inout-read series expose this cost after
range facts became a production code-generation input.

Each token now retains its structural successor. Scalar and closing tokens use
the following token index. A single linear pass over each newly lexed token
segment pairs open and closing delimiters and records the index after the
closing delimiter on the open token. `skip_form` reads that index in constant
time. Unmatched opens keep an unknown successor and use the old bounded scan so
malformed-source diagnostics remain available.

The structural successor is separate from the semantic link used for
declarations, bindings, fields, and cases. It is derived from canonical parsed
SLIM and is not a separately parsed representation.

## Alternatives

Teaching every analysis and emitter traversal to avoid its final subtree skip
would leave the same hazard in other consumers and would not help left-nested
forms. Reusing the semantic link would conflate independent identities and
break consumers that attach meaning to form nodes. A dense side table would
duplicate one token-sized index while requiring it to be threaded through
every compiler boundary.

Adding a separately parsed IR is forbidden by the representation policy and is
unnecessary. Relaxing the permanent scaling budgets would hide the measured
cause.

## Costs

Tokens gain one machine-word structural index. Lexing performs one additional
linear token pass and uses a temporary delimiter stack bounded by nesting
depth. Ordinary source pays constant work per token and later structural
consumers avoid repeated subtree scans. Generated program C, runtime ABI,
source syntax, typing, ownership, effects, and evaluation order are unchanged.

Tests must retain malformed delimiter diagnostics, deterministic token
consumers and generated C, geometric compiler-scaling gates, bootstrap fixed
point, and the complete production verification suite.

## Evidence

Before indexed boundaries, local last-argument traversal fixes brought the full
computed-argument series below its 1.250 budget, but the next structurally
nested inout-read series still measured a 1.365 endpoint exponent. This
isolated repeated structural skipping as a shared compiler cost rather than a
range-transfer property.

With indexed boundaries, computed arguments measured 2.238, 2.916, 4.138, and
6.240 milliseconds from 250 through 2,000 calls, exponent 0.493. Planned
allocation calls measured an exponent of 0.487. Inout reads measured 3.494,
5.253, 8.336, and 14.446 milliseconds from 250 through 2,000 parameters,
exponent 0.683. Every unchanged full performance budget passed. The compiler
reached a deterministic fixed point of 2,326,806 generated C bytes with
SHA-256
`45f7c25b9eedc84a60e4a3b2f78e4a8c9a096638181858dc64e57fdc87c16670`.

The quick project-scaling gate also passes for wide and deep graphs through 17
modules and four jobs, confirming that appended module segments are indexed
without rescanning earlier modules. Malformed-input, conformance, and the
remaining production verification gates must remain unchanged for retention.

## Removal

Remove or replace the stored successor only if every structural consumer
retains bounded traversal, malformed forms preserve stable diagnostics, and
the complete geometric scaling suite passes. Never overload the semantic link
or add a second parsed program representation.
