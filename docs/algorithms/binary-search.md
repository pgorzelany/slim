# Binary search in SLIM

Binary search demonstrates how a familiar range-narrowing algorithm maps to
typed vectors, checked indexing, and tail recurrence without adding a loop
construct.

## Problem and result

The benchmark builds 200,000 sorted even integers and performs 20,000
deterministic queries. Every query is present. The program sums the returned
indexes and prints `1999610000`.

## Small trace

For `[0, 2, 4, 6, 8]` and target `6`, the half-open range starts at `[0, 5)`.
The middle value is `4`, so the next range is `[3, 5)`. Its middle value is
`6`, and the algorithm returns index `3`.

## Function walkthrough

`fill` appends sorted values to the vector. `search` calculates the midpoint
without overflowing the range width, reads the candidate with `vec.get`, and
uses `recur` with either the left or right half. `queries` produces repeatable
targets and accumulates the returned indexes.

## Ownership and effects

The vector is an affine owner created under `alloc`. Search receives it through
`inout`, which provides temporary exclusive access without consuming the
owner. Every vector read is bounds checked. Recurrence requires `partial`
because the general checker does not use the informal range argument as a
termination proof.

## Complexity analysis

Each search takes `O(log n)` time and `O(1)` additional space.

## What differs from loop-oriented languages

`recur` replaces the conventional `while left < right` loop and lowers without
stack growth. Explicit range values make the state transition visible to the
checker.

## Why the result is deterministic

The fill order, query sequence, midpoint calculation, checked reads, and
left-to-right evaluation order are fixed. The program has no concurrent or
ambient source of values, so the same input produces the same checksum.

The workload is benchmark-shaped: its large fill, repeated queries, and
checksum create stable native measurements. See the
[canonical tested source](/handbook/examples/binary-search#canonical-source) and the Handbook
chapters on [control flow](../book/handbook/05-functions-effects-and-control-flow.md)
and [storage](../book/handbook/08-storage-and-allocation.md).
