# Safe C-parity typed vector-set result

Date: 2026-07-24
Decisions: D0086, D0087
Parent: `ba5443e`
Host: Darwin 25.3.0 arm64
C compiler: Apple Clang 21.0.0
Rust compiler: rustc 1.96.0

The backend now lowers `vec.set` through the checked element type to a direct C
assignment guarded by the same `slim_vec_check_index` operation. It removes no
bounds trap. Aggregate assignment, out-of-bounds failure, sanitizers, and all
twenty matched outputs remain permanent tests.

Command:

```text
cargo run --release --bin slim-bench -- compare
```

| challenge | SLIM us | C us | SLIM/C | Rust us | SLIM/Rust |
|---|---:|---:|---:|---:|---:|
| gcd_fib | 1,958 | 1,914 | 1.023 | 2,037 | 0.961 |
| sieve | 4,589 | 2,754 | 1.666 | 2,831 | 1.621 |
| bfs | 2,342 | 1,761 | 1.330 | 2,140 | 1.094 |
| matrix | 3,925 | 2,497 | 1.572 | 2,847 | 1.379 |
| merge_sort | 2,480 | 2,152 | 1.152 | 2,403 | 1.032 |
| bytefreq | 9,262 | 5,837 | 1.587 | 6,273 | 1.476 |
| binary_search | 4,122 | 3,356 | 1.228 | 3,550 | 1.161 |
| prefix_sum | 2,635 | 2,099 | 1.255 | 2,497 | 1.055 |
| records | 2,308 | 1,634 | 1.412 | 1,915 | 1.205 |
| variants | 4,028 | 3,697 | 1.090 | 4,044 | 0.996 |
| state_machine | 2,703 | 3,061 | 0.883 | 3,428 | 0.789 |
| signal_network | 3,482 | 4,612 | 0.755 | 4,620 | 0.754 |
| arena_sum | 2,186 | 2,104 | 1.039 | 2,520 | 0.867 |
| knapsack | 2,372 | 2,147 | 1.105 | 2,452 | 0.967 |
| floyd_warshall | 8,921 | 5,675 | 1.572 | 5,815 | 1.534 |
| n_queens | 562,903 | 415,451 | 1.355 | 412,084 | 1.366 |
| union_find | 58,706 | 53,877 | 1.090 | 56,894 | 1.032 |
| game_of_life | 21,013 | 5,866 | 3.582 | 4,864 | 4.320 |
| image_convolution | 8,456 | 6,891 | 1.227 | 7,193 | 1.176 |
| edit_distance | 5,301 | 5,113 | 1.037 | 8,429 | 0.629 |

The all-application SLIM/C geometric mean is 1.273 and the serial mean is
1.337. Relative to the canonical O3 parent, `merge_sort` improves from 1.679
to 1.152 and `edit_distance` from 1.738 to 1.037. Timing noise moves some
unaffected rows in either direction; the aggregate and mutation-heavy gains
are material.

C parity is still not reached. The remaining 27.3% aggregate gap is dominated
by checked collection reads, byte traversal, and the Game of Life stencil.
