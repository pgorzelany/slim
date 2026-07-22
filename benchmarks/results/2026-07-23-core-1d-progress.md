# Core 1D typed-view progress

Date: 2026-07-23
Status: progress checkpoint, not milestone completion
Host: Apple Silicon arm64, macOS 26.3 (25D125), Apple Clang 21.0.0

## What changed

- Standalone compilation now reads and lexes a valid input once, returns one
  checked artifact containing the typed view, and retains that artifact through
  C emission.
- The type analyzer builds scoped lexical links before expression validation;
  downstream lookups use tagged token links instead of walking every enclosing
  binding.
- Effect and ownership passes resolve user calls through the same declaration
  links. Three repeated whole-program declaration scans and their unused lookup
  functions were removed.
- Syntax and structural checks run before memory or type analysis. The existing
  deterministic multi-diagnostic recovery remains intact.
- The permanent performance workload now makes every generated declaration
  call a user function and adds a geometric deeply nested lexical-binding
  series. Both are governed by versioned exponent budgets.

No syntax, type, effect, built-in, runtime ABI, dependency, LLVM component, or
alternate executable representation was added.

## Measured result

Before linked call resolution, nine idle-host checks of the 468,568-byte
self-host project centered around 3.91 seconds. After the change, nine checks
had a 0.20-second median (one 0.33-second outlier), approximately 19.6 times
faster. This is still process-level project checking rather than incremental
edit latency, and it is not described as a millisecond clean build.

The full permanent performance run reported:

| workload | first | last | first check | last check | exponent |
|---|---:|---:|---:|---:|---:|
| user-call declarations | 1,000 | 8,000 | 10,176 us | 68,486 us | 0.916 |
| nested lexical bindings | 250 | 2,000 | 6,564 us | 50,412 us | 0.980 |

At 8,000 declarations, deterministic emission took 111,491 us and the
emit/check ratio was 1.628. Both check series and emission remain under the
permanent 1.25 scaling-exponent limit, and the ratio remains under 4.0.

Reproduce with:

```text
cargo run --release --quiet --bin slim-bench -- performance
./bootstrap.sh
cargo run --quiet --bin slim-conform -- check
```

## Remaining Core 1D blockers

- Project compilation still reloads, reflattens, relexes, and rechecks between
  validation and emission instead of retaining one project artifact.
- Flattened project type diagnostics still need exact module and original-byte
  source-map projection.
- Code generation and memory planning still rediscover some facts from token
  structure instead of consuming dense typed-view queries.
- Member lookup, adversarial aggregate tests, allocation-failure checks,
  sanitizers, and the complete release gate remain to be frozen together.
