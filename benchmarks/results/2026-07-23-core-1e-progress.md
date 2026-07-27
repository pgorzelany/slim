# Core 1E native-efficiency progress

Date: 2026-07-23
Status: milestone complete
Host: macOS arm64, same checkout and warmed local toolchains
Command: `cargo run --release --quiet --bin slim-bench -- compare --quick`

## Inlinable checked runtime

RFC-0059 moves checked scalar, byte, and vector fast paths from the separate
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

## Allocation-free region elision

Profiling the two remaining aggregate gaps showed that every small helper paid
for child-region initialization and destruction even when its retained plan had
zero allocation sites. RFC-0060 combines the existing placement and allocation
facts; it does not infer effects again or change the uniform region ABI.

| Challenge | RFC-0059 SLIM us | RFC-0060 SLIM us | RFC-0060 C us | RFC-0060 SLIM/C | Speedup |
|---|---:|---:|---:|---:|---:|
| records | 5,080 | 1,844 | 1,283 | 1.437 | 2.75x |
| variants | 17,280 | 3,387 | 3,265 | 1.037 | 5.10x |

Their permanent ratio limits tighten from 5.0 to 3.0 and from 6.0 to 2.0. Five
warm self-checks each consume 0.03 seconds of user CPU. The byte-identical seed
is 1,622,580 C bytes with SHA-256
`a22b64129cd5e4808c4aa4bc256218e894589933452474a5aa3b2fde82365796`.
The authoritative complete release run measures records at 2.104/1.369
milliseconds SLIM/C and variants at 3.421/3.542 milliseconds SLIM/C, with every
correctness, safety, performance, and bootstrap gate passing.

## Core 1E acceptance

RFC-0061 freezes the milestone with release SLIM/C ratios from 1.017 to 1.871.
Dynamic vector bounds and layout remain checked where no range/storage proof is
available. Matrix's flattened traversal, merge sort's checked recursive
indices, and byte frequency's total whole-file input are recorded algorithmic
or semantic differences rather than hidden as optimizer failures. All
non-startup-dominated budgets tighten to 1.75-2.5, and the full release evidence
remains the acceptance gate.
