# D0091: Structured recurrence C lowering

Status: rejected
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 1
Dogfood: 1
Score: 80

## Need

Checked SLIM permits `recur` only in tail position, and the memory plan marks a
function recursive when its body contains that form. The C backend currently
lowers the function entry to a label and each recurrence to a backward `goto`.
This is correct, but it discards the source's structured loop fact before the
native optimizer sees it. Stencil and nested-loop code is especially sensitive
to native loop recognition and unrolling.

For a function already proven tail-recursive, emit one `while (true)` block.
Each `recur` evaluates all arguments into temporaries, installs them in source
parameter order exactly as before, and emits `continue`. Falling through the
checked function body emits `break`, followed by the same allocation-failure
and return epilogue.

## Alternatives

Adding `while` or `for` syntax would create a second source spelling for
recurrence and is rejected. Reconstructing loops from arbitrary C after
generation would add a second parser and violate the single-representation
rule. Native compiler-specific unroll pragmas are not portable and would
require a separate cost model.

Keeping the label form is the baseline. This decision is retained only if the
native corpus shows that preserving structure carries its weight.

## Costs

The backend changes three fixed emissions and adds one fixed close for functions
the existing memory plan already classifies. There is no new pass, analysis,
syntax, dependency, runtime branch, allocation, or ABI. Generated C remains
deterministic C11. Compile time and output size stay linear in source size.

Tests require a recursive function to contain `while (true)`, `continue`, and
`break` without the old recurrence label, while a non-recursive function
contains none of those generated recurrence markers. Nested `let` and `match`,
aggregate recurrence arguments, allocation failure, and tail-call outputs
remain covered by the existing executable fixtures.

## Evidence

The D0087 full result is the baseline: 1.273 geometric mean against C and 1.337
on serial work. Retention requires a measured same-host improvement without
changing output, trap order, allocation failure, or cleanup.

The candidate reached the self-hosted fixed point and all sampled outputs were
unchanged. The 2026-07-24 quick comparison measured 1.273 against C overall and
1.339 on serial work, versus the retained D0087 quick result of 1.263 and
1.320. Game of Life improved only slightly and remained about four times C.
Native optimization canonicalized the structured and label forms closely
enough that the added backend branch did not carry its weight. The production
implementation and provisional marker assertion were removed.

## Removal

Restore label lowering if any accepted tail recurrence changes evaluation or
cleanup order, generated C ceases to be portable, compile scaling regresses, or
the expanded corpus shows no material benefit. Do not add source loop aliases
or unchecked recurrence as a replacement.
