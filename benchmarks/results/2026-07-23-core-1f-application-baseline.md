# Core 1F application parallelism baseline

Date: 2026-07-23
Status: accepted RFC-0064 negative evidence
Host: macOS arm64, same checkout and warmed local toolchain

Command: `cargo run --release --quiet --bin slim-bench -- parallelism`

The permanent native challenge suite now also acts as the parallel-safety
application corpus. Every analysis runs twice, must be byte-identical and
balanced, and must match `benchmarks/parallelism-baseline.tsv` exactly.

| applications | source bytes | functions | checked sites | reported total sites | refinements | safe functions | eligible sites |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 12 | 16,521 | 49 | 140 | 1 | 0 | 0 | 0 |

The compiler's primary reasons across all functions are:

| primary reason | functions |
|---|---:|
| exclusive borrow | 30 |
| allocation or I/O | 10 |
| checked trap | 5 |
| recurrence | 4 |

Declared effects, lexical mutation, unsafe callees, call cycles, table limits,
and missing typed facts are not primary reasons in this corpus.

This result prevents two premature conclusions. RFC-0063 materially improves the
sound proof infrastructure but does not yet make the existing applications
parallelizable. Also, the 30 exclusive-borrow labels do not prove that borrow
precision should be implemented first: the report records only the first
blocker, and most of those functions also recur, perform checked collection
operations, or mutate their borrowed collection. The next evidence step must
expose coexisting blockers or run controlled application transformations.

No runtime speedup, scheduler, or new language feature is claimed.
