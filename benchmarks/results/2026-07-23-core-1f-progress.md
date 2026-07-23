# Core 1F deterministic-parallelism progress

Date: 2026-07-23
Status: first accepted evidence slice
Host: macOS arm64, same checkout and warmed local toolchain

## Safety evidence before execution

D0062 adds no syntax, effect, built-in, runtime ABI, dependency, task, or
scheduler. Analysis version 3 consumes the ordinary checked token links and
typed facts. The exact fixture proves one adjacent independent fork/join site
and separately rejects checked traps, a transitive unsafe callee, allocation,
I/O, an exclusive borrow, lexical mutation, and recurrence. A mutual call cycle
remains unknown, and a data-dependent adjacent `let` pair is not selected.

Generated tests cross both hard limits. A 66-function program makes a call to a
function beyond the 64-fact table and reports `function-limit`; a checked record
construction with 4,097 user calls crosses the 4,096-edge table and reports
`edge-limit`. Both module summaries are bounded rather than exact. Repeated
fixture reports are byte-identical.

`slimc analyze selfhost/slim.project` reuses the normal prepared project
artifact. It reports 661 checked functions and a 1,281,548-byte deterministic,
balanced analysis artifact. Parallelism is bounded because the project exceeds
the explicit 64-function limit. This is an honest dogfood limitation, not a
whole-compiler safety claim.

## Full analysis scaling

Command: `cargo run --release --quiet --bin slim-bench -- reduction`

| declarations | source bytes | reduce us | analyze us | proof us | analysis bytes |
|---:|---:|---:|---:|---:|---:|
| 1,000 | 69,971 | 6,564 | 6,604 | 7,893 | 568,996 |
| 2,000 | 140,971 | 10,583 | 10,677 | 13,129 | 1,134,996 |
| 4,000 | 282,971 | 19,005 | 19,395 | 23,742 | 2,266,996 |
| 8,000 | 566,971 | 36,570 | 37,357 | 44,974 | 4,547,425 |

The end-to-end analysis exponent is 0.833 from 1,000 to 8,000 declarations,
below the permanent 1.25 gate now stored as
`analysis-exponent/generated-declarations` in the shared budget ledger. Output
bytes remain linear. Five warm full self-host checks remain at 0.03 seconds of
user CPU.

## Bootstrap and cost

The compiler reaches a byte-identical fixed point at 1,711,913 generated C
bytes with SHA-256
`a2f022c8276829b4ff6a553d1202e537c9a881e6faf6795cf80c0dbe24daf06d`.
The evidence slice adds 89,333 seed bytes (5.51%) over D0060.
Ordinary checking and C generation do not invoke the parallel analysis, and
generated user programs do not link it; only the explicit analysis command pays
its analysis work.

No runtime speedup is claimed. Every candidate retains
`profitability unknown`, candidate sites can overlap, and no program executes
in parallel in this slice.
