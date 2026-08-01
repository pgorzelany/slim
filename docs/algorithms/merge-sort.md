# Merge sort in SLIM

Merge sort shows recursive decomposition, two exclusive vector borrows, and
explicit scratch storage while preserving stable left-to-right behavior.

## Problem and result

The benchmark fills 20,000 values in descending order, sorts them ascending,
and prints the deterministic weighted checksum `2666866670000`.

## Small trace

For `[4, 3, 2, 1]`, `sort` divides the range into `[4, 3]` and `[2, 1]`.
Each half becomes ordered, then `merge_fill` chooses the smaller leading value
until scratch contains `[1, 2, 3, 4]`. `copy_back` installs that range.

## Function walkthrough

`sort` recursively divides half-open ranges. `merge` coordinates
`merge_fill` and `copy_back`. The two small copy helpers finish whichever side
remains after the other is exhausted. `checksum` traverses the final vector and
makes incorrect ordering observable.

## Ownership and effects

Both `values` and `scratch` remain owned by `main` and are passed as distinct
`@` borrows. The no-alias rule prevents them from referring to the same
owner. Filling may allocate; sorting mutates existing elements and needs no
hidden allocation. Reads, writes, arithmetic, and recurrent argument types are
checked.

## Complexity analysis

Sorting takes `O(n log n)` time and `O(n)` scratch space.

## What differs from loop-oriented languages

Unlike an iterator or slice API, the current language passes explicit indexes
and owner borrows. Recursive calls express the divide phase; tail recurrence
expresses the merge and copy scans.

## Why the result is deterministic

Range splitting, stable left-first selection, copy order, and mutation sites
are fixed. The two exclusive borrows cannot alias, so reads and writes cannot
race or observe an alternative ordering.

This is a benchmark workload rather than a minimal library API. See the
[canonical tested source](/handbook/examples/merge-sort#canonical-source), [ownership](../book/handbook/07-ownership-borrowing-and-memory.md),
and [storage](../book/handbook/08-storage-and-allocation.md).
