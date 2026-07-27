# SLIM 0.9 familiar-syntax frontend evidence

Date: 2026-07-27
Decision: D0107
Compiler: self-hosted SLIM 0.9.0, portable C11 seed
Fixed point: 2,743,494 generated C bytes

## Separator-dense frontend

Command:

```text
cargo run --release --bin slim-bench -- performance --quick
```

The generated series stresses required commas in declarations, calls, record
fields, and enum payloads. Commas are counted as lexer tokens and contribute
zero canonical AST nodes. Times are two same-host warmed medians for parsing
and checking together. The recorded noise band is ±5%; the largest repeat
increase is 2.8%.

| declarations | source bytes | lexemes | commas | canonical expression nodes | first median us | repeat median us | repeat/first |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 250 | 23,712 | 6,771 | 1,000 | 1,005 | 3,407 | 3,436 | 1.009 |
| 500 | 47,462 | 13,521 | 2,000 | 2,005 | 5,380 | 5,528 | 1.028 |
| 1,000 | 94,962 | 27,021 | 4,000 | 4,005 | 12,091 | 12,305 | 1.018 |
| 2,000 | 190,962 | 54,021 | 8,000 | 8,005 | 26,700 | 26,296 | 0.985 |

The permanent portable gate remains the geometric exponent budget of 1.25;
absolute medians remain dated same-host evidence. Both runs stay within that
budget, every repeated median stays within the recorded noise band, and the
established runtime, reduction, native-comparison, and parallel/serial ratio
budgets are unchanged.

## Parallelism baseline migration

Every application row in `benchmarks/parallelism-baseline.tsv` records a new
source-byte count because D0107 replaces the complete source spelling. The
checked-site, refinement, blocker, and scheduling columns remain unchanged for
eighteen applications.

`state_machine` and `signal_network` each now expose the existing `bit` helper
as a fourth analyzed function, and direct operator nodes expose four and eight
independent call-pair candidates respectively. The selected counts become
three and five, while both programs retain exactly one executable and one
executed total-recurrence site. This is recorded syntax-work evidence: runtime
behavior, worker shape, and the generated parallel/forced-serial ratio budgets
are unchanged.
