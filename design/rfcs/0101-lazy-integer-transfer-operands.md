# RFC-0101: Lazy call operand traversal

Status: accepted
Implementation: complete
Process: legacy
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

Several call consumers advanced past an argument before determining whether
another argument existed. For a deeply nested final argument, that unnecessary
advance walks the complete nested subtree at every call level. The integer
range dispatcher also located a second argument and loaded two range facts
before determining whether a call was an integer operation. The permanent
computed-argument series exposed the resulting superlinear emission: its full
geometric exponent reached 1.636 against the 1.250 budget.

The dispatcher now classifies the callee first. It locates two operands only
for `i64.add`, `i64.sub`, `i64.mul`, `i64.div`, and `i64.rem`; it loads one
operand for `i64.to-u8`; and it performs no operand traversal for other calls.
Range propagation and code generation inspect the next checked parameter before
advancing past the corresponding argument. The range expression analyzer uses
the linked user-parameter list for the same purpose. The same argument order,
transfer functions, and facts are used after classification.

## Alternatives

Relaxing the permanent scaling budget is forbidden without measured cause and
compensation. Caching every `skip_form` result would add another dense table and
memory cost when the dispatcher does not need the span at all. Special-casing
the benchmark's `identity` name would not solve the structural issue.

## Costs

Five callee comparisons move before operand lookup for every analyzed call.
They are constant work already performed by the old dispatcher. User-call
consumers perform one checked next-parameter query before deciding whether an
argument advance is needed. No source syntax, semantic fact, dependency,
runtime operation, ABI, or representation is added.

Tests must retain all integer transfer results, exact checked-site reports,
malformed-source behavior, and the geometric computed-argument gate. The full
compiler and reduction scaling suites remain permanent.

## Evidence

Before the change, the quick computed-argument exponent repeatedly measured
about 1.29 and the full 250-to-2,000-call exponent measured 1.636. Lazy integer
dispatch reduced the full exponent to about 1.57; avoiding the final
parameter-propagation advance reduced it to 1.438. Linked user-argument
analysis brought the series to 3.396, 5.263, 12.170, and 36.974 milliseconds,
an endpoint exponent of 1.148 without changing the fixture or its 1.250
budget.

RFC-0102 subsequently removes the shared structural-skip cost for all form
consumers. The local rules remain because they avoid semantically unnecessary
work and do not depend on the successor representation.

## Removal

Remove or replace this ordering only if non-arithmetic calls and final
arguments still avoid unnecessary traversal, every integer transfer remains
identical, and the permanent scaling budget passes.
