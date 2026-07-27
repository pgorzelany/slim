# N-Queens in SLIM

N-Queens shows iterative backtracking expressed as an explicit state machine
over one mutable vector rather than recursive stack frames.

## Problem and result

The benchmark counts placements of thirteen queens with no shared row, column,
or diagonal. It prints the known solution count `73712`.

## Small trace

For a four-row board, the vector stores the selected column for each row.
When a candidate conflicts, the search advances the column. When a row runs
out of columns, it clears that row, moves to the preceding row, and resumes
after the previous choice.

## Function walkthrough

`safe` scans earlier rows for a matching column or diagonal.
`fill` initializes every row to `-1`. `search` carries the current row, column,
board size, and solution total. Its branches encode advance, descend,
backtrack, and count-solution transitions.

## Ownership and effects

The columns vector is one affine owner borrowed exclusively by `safe`, `fill`,
and `search`. Mutations use `vec.set`; the borrow cannot escape or alias another
argument. Initialization requires `alloc`, and the unsupported general
recurrences require `partial`.

## Complexity analysis

Worst-case search is `O(n!)` with `O(n)` vector storage.

## What differs from loop-oriented languages

Conventional recursive backtracking relies on call-stack state. This program
instead carries the state explicitly through `recur`, making backtracking
transitions and storage use observable.

## Why the result is deterministic

Rows and candidate columns are considered in a fixed order. Every transition
updates one exclusively borrowed vector, and there is no concurrent search or
unordered collection, so the solution count is repeatable.

See the [canonical tested source](/handbook/examples/n-queens#canonical-source),
[control flow](../book/handbook/05-functions-effects-and-control-flow.md), and
[ownership](../book/handbook/07-ownership-borrowing-and-memory.md).
