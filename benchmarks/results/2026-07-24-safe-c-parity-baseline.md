# Safe C-parity expanded-corpus baseline

Date: 2026-07-24
Decision: D0084
Compiler base: `87314f3c3ea46f0804bb396aa8ae09300cde2d71`
Host: Darwin 25.3.0 arm64
C compiler: Apple Clang 21.0.0
Rust compiler: rustc 1.96.0

## Corpus

The permanent matched corpus grows from fourteen to twenty SLIM/C11/Rust
applications before any optimizer change. The six additions are:

- `floyd_warshall`: dense all-pairs graph work;
- `n_queens`: backtracking search;
- `union_find`: disjoint-set traversal and mutation;
- `game_of_life`: Boolean cellular stencil;
- `image_convolution`: integer image stencil; and
- `edit_distance`: byte-oriented dynamic programming.

Each trio uses the same algorithm and data scale and produces identical output.
The resource and parallelism baselines retain the checked facts for every new
application.

## Full baseline

Command:

```text
cargo run --release --bin slim-bench -- compare
```

Times are warmed process medians in microseconds. Ratios below one favor SLIM.

| challenge | SLIM us | C us | SLIM/C | Rust us | SLIM/Rust |
|---|---:|---:|---:|---:|---:|
| gcd_fib | 1,717 | 1,804 | 0.952 | 1,888 | 0.909 |
| sieve | 4,594 | 2,636 | 1.743 | 2,754 | 1.668 |
| bfs | 2,625 | 1,810 | 1.450 | 2,178 | 1.205 |
| matrix | 4,016 | 2,499 | 1.607 | 2,698 | 1.489 |
| merge_sort | 3,820 | 2,221 | 1.720 | 2,383 | 1.603 |
| bytefreq | 9,299 | 5,607 | 1.658 | 6,310 | 1.474 |
| binary_search | 4,147 | 3,227 | 1.285 | 3,391 | 1.223 |
| prefix_sum | 2,565 | 2,179 | 1.177 | 2,591 | 0.990 |
| records | 2,303 | 1,671 | 1.378 | 1,972 | 1.168 |
| variants | 3,841 | 3,869 | 0.993 | 4,049 | 0.949 |
| state_machine | 2,632 | 3,040 | 0.866 | 3,406 | 0.773 |
| signal_network | 3,646 | 4,638 | 0.786 | 4,697 | 0.776 |
| arena_sum | 2,459 | 1,983 | 1.240 | 2,541 | 0.968 |
| knapsack | 2,381 | 2,009 | 1.185 | 2,394 | 0.995 |
| floyd_warshall | 8,867 | 5,443 | 1.629 | 5,673 | 1.563 |
| n_queens | 559,525 | 416,003 | 1.345 | 413,516 | 1.353 |
| union_find | 58,417 | 53,973 | 1.082 | 56,563 | 1.033 |
| game_of_life | 21,152 | 5,854 | 3.613 | 4,856 | 4.356 |
| image_convolution | 10,883 | 6,821 | 1.596 | 7,120 | 1.529 |
| edit_distance | 8,745 | 4,986 | 1.754 | 8,700 | 1.005 |

The all-application geometric means are 1.371 SLIM/C and 1.239 SLIM/Rust.
Excluding the two automatic-parallel candidates gives 1.450 SLIM/C and 1.306
SLIM/Rust across eighteen serial applications.

## Initial diagnosis

Generated code keeps checked arithmetic and collection access inside every hot
recurrence. Clang reports that trap control flow prevents vectorization in the
matrix loop. The existing node-indexed `ranges` view can prove selected
arithmetic total, but code generation currently consumes it only indirectly
through parallel planning. Collection length relationships and recurrent
parameter bounds remain unknown, explaining why the stencil applications add
many checks and why Game of Life is the largest new gap.

The first optimizer step must therefore consume positive facts already attached
to the exact checked node. It may not infer safety from optimization level,
application name, or an unchecked C oracle. Unsupported and bounded-away cases
retain the runtime trap.
