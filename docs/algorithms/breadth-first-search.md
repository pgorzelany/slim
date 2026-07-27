# Breadth-first search in SLIM

Breadth-first search demonstrates a growing vector used as a queue and a
recurrent cursor that never invalidates previously enqueued values.

## Problem and result

The program traverses an implicit binary tree whose node numbers run from zero
through 100,000. It visits nodes in breadth-first order and prints their sum,
`5000050000`.

## Small trace

The queue begins as `[0]`. Visiting zero appends `1` and `2`. Visiting one
appends `3` and `4`; visiting two appends `5` and `6`. Advancing one cursor
through the growing vector produces level order without removing elements.

## Function walkthrough

`enqueue` checks the node limit before appending. `visit` reads the current
node, calculates its two children, appends valid children, and uses `recur`
with the next queue index and accumulated total. It stops when the cursor
reaches the current vector length.

## Ownership and effects

`main` owns the queue. `visit` and `enqueue` borrow it through `inout`, allowing
growth without moving the vector away from the caller. Growth requires
`alloc`; recurrent traversal requires `partial`; printing requires `io`.
Bounds and arithmetic remain checked.

## Complexity analysis

The traversal takes `O(V + E)` time and `O(V)` queue storage.

## What differs from loop-oriented languages

A conventional implementation might use a mutable queue object and a `while`
loop. SLIM uses one affine vector, visible `vec.push` operations, and a
recurrent index.

## Why the result is deterministic

The node limit, child order, queue growth, checked reads, and evaluation order
are fixed. No work races with the traversal, so every run visits the same
nodes in the same order and produces the same checksum.

See the [canonical tested source](/handbook/examples/bfs#canonical-source), [functions and
control flow](../book/handbook/05-functions-effects-and-control-flow.md), and
[storage](../book/handbook/08-storage-and-allocation.md).
