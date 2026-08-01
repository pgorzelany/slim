# Game of Life in SLIM

Conway's Game of Life demonstrates checked two-dimensional indexing,
double-buffered mutation, and repeated deterministic grid transformations.

## Problem and result

The benchmark creates a deterministic 128-by-128 toroidal grid, advances it
for 100 generations, and prints the final live-cell count. For the fixed seed,
the result is `0`.

## Small trace

For each cell, `neighbor_count` visits the surrounding three-by-three window
and skips the center. A dead cell is born with three live neighbors; a live
cell survives with two or three. All new values go to the next buffer before
the buffers are copied.

## Function walkthrough

`fill` creates the initial and next grids. `neighbor_count` converts row and
column offsets into wrapped vector indexes. `step` calculates every next cell.
`copy_grid` installs the completed generation, `generations` repeats the
process, and `checksum` counts the remaining live cells.

## Ownership and effects

`current` and `next` are distinct affine vectors passed through exclusive
`@` borrows. Double buffering prevents a new cell from observing a
partially updated generation. Initial growth requires `alloc`; all accesses,
offset arithmetic, and writes are checked.

## Complexity analysis

For `g` generations of an `n` by `n` grid, runtime is `O(g * n^2)` and storage
is `O(n^2)`.

## What differs from loop-oriented languages

Nested recurrence replaces nested loops. Two explicit vector owners replace
an implicitly managed grid object and make the generation boundary visible.

## Why the result is deterministic

Every cell in a generation reads only `current` and writes only `next`.
Neighbor order, Boolean rules, copying, and generation order are fixed, so no
cell can observe a partially updated generation.

See the [canonical tested source](/handbook/examples/game-of-life#canonical-source),
[operators](../book/handbook/03-values-types-and-operators.md), and
[storage](../book/handbook/08-storage-and-allocation.md).
