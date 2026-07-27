# Safe C-parity proof-directed arithmetic candidate

Date: 2026-07-24
Decisions: RFC-0084, RFC-0085
Baseline compiler: `87314f3c3ea46f0804bb396aa8ae09300cde2d71`
Host: Darwin 25.3.0 arm64
C compiler: Apple Clang 21.0.0
Rust compiler: rustc 1.96.0

## Change

Generated C now replaces a checked integer helper with its direct C operator
only when the existing bounded range view proves that exact source call total.
The same analysis propagates exact total scalar parameters through a closed
call graph for four fixed passes when all calls agree and recurrence preserves
the parameter. Unknown or unsupported facts retain the helper.

A follow-up experiment emitted proven scalar operands as constants. It was
removed because it produced no runtime improvement beyond what Clang already
derived.

## Full candidate

Command:

```text
cargo run --release --bin slim-bench -- compare
```

Times are warmed process medians in microseconds. Ratios below one favor SLIM.

| challenge | SLIM us | C us | SLIM/C | Rust us | SLIM/Rust |
|---|---:|---:|---:|---:|---:|
| gcd_fib | 1,722 | 1,552 | 1.110 | 1,807 | 0.953 |
| sieve | 4,633 | 2,634 | 1.759 | 2,838 | 1.632 |
| bfs | 2,553 | 1,834 | 1.392 | 2,292 | 1.114 |
| matrix | 3,924 | 2,486 | 1.578 | 2,729 | 1.438 |
| merge_sort | 4,024 | 2,261 | 1.780 | 2,450 | 1.642 |
| bytefreq | 9,319 | 5,654 | 1.648 | 6,305 | 1.478 |
| binary_search | 4,011 | 3,253 | 1.233 | 3,481 | 1.152 |
| prefix_sum | 2,540 | 2,227 | 1.141 | 2,449 | 1.037 |
| records | 2,323 | 1,585 | 1.466 | 1,916 | 1.212 |
| variants | 3,929 | 3,781 | 1.039 | 3,981 | 0.987 |
| state_machine | 2,671 | 3,000 | 0.890 | 3,499 | 0.763 |
| signal_network | 3,467 | 4,660 | 0.744 | 4,723 | 0.734 |
| arena_sum | 2,437 | 1,921 | 1.269 | 2,407 | 1.012 |
| knapsack | 2,267 | 2,024 | 1.120 | 2,543 | 0.891 |
| floyd_warshall | 8,900 | 5,461 | 1.630 | 5,756 | 1.546 |
| n_queens | 558,598 | 415,292 | 1.345 | 412,484 | 1.354 |
| union_find | 57,942 | 54,288 | 1.067 | 56,042 | 1.034 |
| game_of_life | 21,200 | 5,808 | 3.650 | 4,887 | 4.338 |
| image_convolution | 8,349 | 7,128 | 1.171 | 7,070 | 1.181 |
| edit_distance | 8,698 | 4,782 | 1.819 | 8,596 | 1.012 |

The all-application geometric means are 1.359 SLIM/C and 1.215 SLIM/Rust.
Excluding the two automatic-parallel applications gives 1.439 SLIM/C and 1.282
SLIM/Rust across eighteen serial applications.

## Interpretation

The candidate improves the RFC-0084 full baseline from 1.371 to 1.359 against C
and from 1.450 to 1.439 on serial work. The stencil arithmetic result validates
exact-node lowering, but the overall gap is still 35.9%; C parity has not been
reached.

Game of Life remains the largest outlier. Its hot path retains vector bounds
checks and arithmetic whose relational collection/index facts are unknown.
Those checks cannot be removed from integer totality alone. The next material
step is a bounded proof of collection access or safe partitioning; neither is
authorized by this result without a separate scored design.
