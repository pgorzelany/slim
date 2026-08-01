# Edit distance in SLIM

Levenshtein edit distance demonstrates dynamic programming over immutable byte
views with two reusable mutable rows.

## Problem and result

The benchmark compares two fixed byte strings. It repeats the calculation 200
times per process argument and prints the accumulated distance, `4400` for the
normal invocation.

## Small trace

For `cat` and `cut`, the first row represents transforming an empty prefix into
`c`, `cu`, and `cut`. Subsequent rows choose the minimum of deletion,
insertion, and substitution. Only the middle character differs, so the final
distance is one.

## Function walkthrough

`fill` allocates the two row vectors. `columns` computes one row from byte
comparisons and the three neighboring dynamic-programming cells. `copy_row`
makes the completed row the next previous row. `rows` advances through the
left string, and `repeat` resets the buffers for stable benchmark work.

## Ownership and effects

The input `Bytes` values are copyable immutable views. The two row vectors are
distinct affine owners borrowed exclusively through `@`. Allocation occurs only while
building the buffers; later rounds reuse their capacity. Byte and vector reads,
writes, conversions, and arithmetic remain checked.

## Complexity analysis

One distance calculation takes `O(m * n)` time and `O(n)` space.

## What differs from loop-oriented languages

Explicit row buffers make the space optimization visible. Tail recurrence
replaces both row and column loops, while helper functions preserve one clear
mutation boundary.

## Why the result is deterministic

Inputs, row order, cell dependencies, tie handling, and left-to-right
evaluation are fixed. Each new row reads only the completed previous row and
the completed prefix of the current row, so mutation order cannot vary.

See the [canonical tested source](/handbook/examples/edit-distance#canonical-source),
[values and operators](../book/handbook/03-values-types-and-operators.md), and
[storage](../book/handbook/08-storage-and-allocation.md).
