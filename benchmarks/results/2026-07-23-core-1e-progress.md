# Core 1E native-efficiency progress

Date: 2026-07-23
Status: first accepted slice
Host: macOS arm64, same checkout and warmed local toolchains
Command: `cargo run --release --quiet --bin slim-bench -- compare --quick`

## Inlinable checked runtime

D0059 moves checked scalar, byte, and vector fast paths from the separate
runtime translation unit into one canonical `static inline` header
implementation. Slow traps, region allocation, file I/O, and output remain
out-of-line. No check, effect, syntax, or source profile is removed.

| Challenge | Baseline SLIM us | Candidate SLIM us | Candidate C us | Candidate SLIM/C | SLIM speedup |
|---|---:|---:|---:|---:|---:|
| gcd_fib | 1,625 | 1,702 | 1,441 | 1.181 | 0.95x |
| sieve | 13,484 | 4,461 | 2,470 | 1.806 | 3.02x |
| bfs | 2,667 | 2,478 | 1,763 | 1.406 | 1.08x |
| matrix | 15,367 | 3,712 | 2,303 | 1.612 | 4.14x |
| merge_sort | 4,688 | 3,601 | 1,738 | 2.072 | 1.30x |
| bytefreq | 52,597 | 8,604 | 5,174 | 1.663 | 6.11x |
| binary_search | 5,807 | 3,628 | 2,668 | 1.360 | 1.60x |
| prefix_sum | 4,402 | 2,174 | 1,804 | 1.205 | 2.02x |
| records | 8,537 | 5,080 | 1,401 | 3.626 | 1.68x |
| variants | 23,511 | 17,280 | 3,595 | 4.807 | 1.36x |
| arena_sum | 2,924 | 1,899 | 1,823 | 1.042 | 1.54x |
| knapsack | 3,669 | 2,036 | 1,898 | 1.073 | 1.80x |

`gcd_fib` is below process-startup resolution and is recorded rather than
claimed as a gain. Candidate native build medians otherwise remain near the
baseline band, and binaries shrink by roughly 0.8 kilobytes. The accepted
change is protected by tighter per-challenge runtime ratios and eleven exact
trap fixtures, not by this result file alone.

Five warm `build/toolchain/slimc check selfhost/slim.project` runs each consume
0.05 seconds of user CPU and 0.05-0.06 seconds wall time, compared with the
approximately 0.11-second Core 1D self-check boundary. The complete release
gate passes 109 fixtures, 2,000 mutations, tightened native ratios, sanitizers,
allocation-failure injection, and the unchanged byte-identical 1,629,310-byte
bootstrap seed.
