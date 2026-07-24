# Safe C-parity canonical O3 result

Date: 2026-07-24
Decision: D0086
Parent: `ba5443e`
Host: Darwin 25.3.0 arm64
C compiler: Apple Clang 21.0.0
Rust compiler: rustc 1.96.0

`slimc build` uses the same portable `-O3` optimization level as the matched C
and Rust application builds. Bootstrap remains at `-O2`; no safety check,
runtime ABI, source form, or target-specific flag changes.

Command:

```text
cargo run --release --bin slim-bench -- compare
```

| challenge | SLIM us | C us | SLIM/C | Rust us | SLIM/Rust |
|---|---:|---:|---:|---:|---:|
| gcd_fib | 1,701 | 1,649 | 1.032 | 1,931 | 0.881 |
| sieve | 4,661 | 2,607 | 1.788 | 2,773 | 1.681 |
| bfs | 2,290 | 1,822 | 1.257 | 2,091 | 1.095 |
| matrix | 3,958 | 2,728 | 1.451 | 2,790 | 1.419 |
| merge_sort | 3,778 | 2,250 | 1.679 | 2,431 | 1.554 |
| bytefreq | 9,297 | 5,756 | 1.615 | 6,328 | 1.469 |
| binary_search | 3,988 | 3,154 | 1.264 | 3,430 | 1.163 |
| prefix_sum | 2,498 | 2,093 | 1.194 | 2,494 | 1.002 |
| records | 2,229 | 1,651 | 1.350 | 1,917 | 1.163 |
| variants | 4,012 | 3,973 | 1.010 | 4,141 | 0.969 |
| state_machine | 2,682 | 3,065 | 0.875 | 3,492 | 0.768 |
| signal_network | 3,482 | 4,663 | 0.747 | 4,539 | 0.767 |
| arena_sum | 2,459 | 2,014 | 1.221 | 2,418 | 1.017 |
| knapsack | 2,157 | 1,906 | 1.132 | 2,414 | 0.894 |
| floyd_warshall | 8,949 | 5,478 | 1.634 | 5,814 | 1.539 |
| n_queens | 562,734 | 415,500 | 1.354 | 412,588 | 1.364 |
| union_find | 58,176 | 53,909 | 1.079 | 56,181 | 1.036 |
| game_of_life | 20,931 | 5,867 | 3.568 | 4,771 | 4.387 |
| image_convolution | 8,647 | 6,658 | 1.299 | 7,124 | 1.214 |
| edit_distance | 8,681 | 4,995 | 1.738 | 8,767 | 0.990 |

The all-application SLIM/C geometric mean is 1.335, down from 1.359 at
canonical `-O2`. The serial mean is 1.412, down from 1.439. Median SLIM build
times remain in the same approximate 156–198 ms band. The canonical O3 build
is retained.
