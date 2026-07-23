# Core 1J structured concurrency evidence

Date: 2026-07-23  
Host: macOS arm64, same-host medians  
Command: `cargo run --release --bin slim-bench -- host`

| Application | Forced serial | POSIX worker | Parallel / serial | Budget |
| --- | ---: | ---: | ---: | ---: |
| `dual_fetch` | 175,091 µs | 92,596 µs | 0.529 | 0.75 |
| `dual_health` | 173,919 µs | 92,909 µs | 0.534 | 0.75 |

The two programs use different result shapes: `dual_fetch` returns owned
response vectors across the join, while `dual_health` returns scalar health
facts. Both use two independent bounded loopback exchanges and compare the
same generated program under the portable serial tier and POSIX worker tier.

The same run measured 100,000 clock calls at a 1.065 SLIM/C ratio and unrelated
Hello binaries at a 1.015 supported-network/disabled-network size ratio. The
existing 2.00 and 1.03 budgets therefore remain unchanged.

Correctness is gated separately: conformance, deterministic analysis,
post-join owned-vector use, region adoption and resize, spawn fallback,
allocation fault injection, join fault injection, unsupported worker fallback,
and malformed-input recovery must all pass.
