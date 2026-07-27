# Core 1F complete blocker-set evidence

Date: 2026-07-23
Status: accepted RFC-0065 evidence slice
Host: macOS arm64, same checkout and warmed local toolchain

Command: `cargo run --release --quiet --bin slim-bench -- parallelism`

Schema 2 retains the prior primary-reason columns and adds one incidence column
for every exact per-function blocker. The twelve application totals are:

| blocker | functions |
|---|---:|
| declared effects | 49 |
| checked trap | 40 |
| recurrence | 31 |
| exclusive borrow | 30 |
| allocation or I/O | 22 |
| unsafe direct callee | 22 |
| lexical mutation | 0 |
| call cycle | 0 |
| function limit | 0 |
| edge limit | 0 |
| missing typed fact | 0 |

Blockers overlap heavily:

| blockers on one function | functions |
|---:|---:|
| 2 | 11 |
| 3 | 11 |
| 4 | 18 |
| 5 | 9 |

No function has zero or one blocker. Removing any single category would unlock
zero functions in the current application corpus. In particular, implementing
read-only borrow precision alone would leave declared effects, traps,
recurrence, or an unsafe callee on every affected function. Conversely,
ignoring declared `partial` solely because another local operation looks safe
would violate the explicit effect contract.

## Analysis scaling

Command: `cargo run --release --quiet --bin slim-bench -- reduction`

| declarations | source bytes | analyze us | analysis bytes |
|---:|---:|---:|---:|
| 1,000 | 69,971 | 6,561 | 579,001 |
| 2,000 | 140,971 | 11,778 | 1,153,001 |
| 4,000 | 282,971 | 21,440 | 2,301,001 |
| 8,000 | 566,971 | 40,057 | 4,613,430 |

The end-to-end exponent is 0.870, below the unchanged 1.25 budget. The blocker
sets add a constant 1,725 report bytes at every sampled size because function
evidence remains capped at 64; token traversal and output still scale linearly.

Self-analysis reports 718 functions, 1,020 checked integer sites, 14
refinements, and zero eligible sites. Its 1,395,195-byte report is balanced and
deterministic. The parallel section remains honestly bounded to 64 function
sets.

The compiler seed reaches a byte-identical fixed point at 1,854,768 generated
C bytes with SHA-256
`1616a2e9792e731b20965c221ffdacc042311d57da8e5556c633e22fb651c919`.
This is 22,466 bytes (1.23%) above RFC-0063. The application report remains a
non-executable analysis artifact; no runtime or language surface changes.
