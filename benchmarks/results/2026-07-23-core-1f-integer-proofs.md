# Core 1F bounded integer proof evidence

Date: 2026-07-23
Status: accepted RFC-0063 evidence slice
Host: macOS arm64, same checkout and warmed local toolchain

## Proof behavior

RFC-0063 adds no syntax, built-in, effect, runtime ABI, dependency, optimizer, or
scheduler. Analysis version 4 derives bounded integer facts from the ordinary
checked token and type view. Quality and parallelism consume the same exact-node
totality fact; neither reconstructs or weakens the proof.

The permanent fixture proves guarded upper and lower arithmetic, exact nested
arithmetic, nonzero division, and a valid checked I64-to-U8 conversion. It
retains checked-trap hazards for unguarded parameter arithmetic, division by
zero, and a value outside the fixed proof domain. Its report contains nine
checked sites, four branch refinements, and one otherwise eligible fork site.

Generated tests cross both output bounds. Thirty-three nested comparisons fill
the 64-refinement table and report saturation. Sixty-five checked operations
report the complete site count, print exactly 64 site facts, and label the
section bounded. Repeated reports are byte-identical and balanced.

`slimc analyze selfhost/slim.project` reports 712 checked functions, 1,013
checked integer sites, 14 retained refinements, no refinement saturation, and
zero eligible fork sites. Its 1,379,953-byte report is deterministic and
balanced. This is useful proof dogfooding but not evidence that executing the
compiler in parallel would help.

## Full analysis scaling

Command: `cargo run --release --quiet --bin slim-bench -- reduction`

| declarations | source bytes | reduce us | analyze us | proof us | analysis bytes |
|---:|---:|---:|---:|---:|---:|
| 1,000 | 69,971 | 6,036 | 6,631 | 7,282 | 577,276 |
| 2,000 | 140,971 | 10,609 | 11,653 | 13,084 | 1,151,276 |
| 4,000 | 282,971 | 19,342 | 20,798 | 24,342 | 2,299,276 |
| 8,000 | 566,971 | 35,509 | 40,347 | 45,110 | 4,611,705 |

The end-to-end analysis exponent is 0.868 from 1,000 to 8,000 declarations,
below the unchanged permanent 1.25 gate. Analysis output remains linear. The
8,000-declaration analysis time is 8.0% above the RFC-0062 sample, while output is
1.4% larger. This cost is paid only by the explicit analysis command; ordinary
checking and C generation do not invoke the proof view, and generated user
programs do not link it.

## Bootstrap and cost

The compiler reaches a byte-identical fixed point at 1,832,302 generated C
bytes with SHA-256
`3dfd1d5603223167bdde881217492037c756eb61bae7274f1f80b8fd1419a58b`.
The proof slice adds 120,389 seed bytes (7.03%) over RFC-0062 and 209,722 bytes
(12.93%) over the Core 1E boundary. The cost is accepted because it creates one
shared safety fact rather than parallel checker-specific logic, crosses every
declared bound, preserves the permanent scaling budget, and adds no runtime or
language surface.

No runtime speedup is claimed. Profitability remains unknown, candidate sites
can overlap, and SLIM still executes programs serially.
