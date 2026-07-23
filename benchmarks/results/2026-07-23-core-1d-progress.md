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

## Rejected dense-index experiment

A first dense `expression token -> fact` vector was implemented and then
reverted. The existing compiler took more than 13 seconds to check the modified
self-host project, compared with about 0.21 seconds after the candidate was
removed. The candidate had not yet been bootstrapped, so this measures a
source-shape performance cliff in the current compiler rather than the runtime
cost of constructing the proposed index. It is not evidence against dense
lookup in principle; it is evidence that this particular change cannot land
without profiling, a geometric fixture, and a same-host regression result.

## Token-interval diagnostic checkpoint

D0034 replaces the structured issue's single token with inclusive start and
end token indices. Standalone and prepared-project reporters consume the same
interval, while one-token type issues set both endpoints identically. The
self-host check remained about 0.22 seconds and the compiler bootstrapped to a
byte-identical 1,652,703-byte C seed.

An attempted all-at-once migration of the effect, recursive-borrow, and move
walkers threaded another mutable issue vector through their recursive call
graphs. Even after removing the move portion, self-validation exceeded 40
seconds. The experiment was reverted rather than accepting a compiler
performance regression. The representation remains; collectors will migrate in
smaller slices with source-shape measurements before each checkpoint.

## Finalized issue stream and first migrated family

D0035 makes `Checked.issues` the completed diagnostic stream. It starts with
the analyzer snapshot and is extended by later phases without rebuilding the
typed view. This avoids both the ownership bug in mutating an extracted vector
and the measured view-reconstruction performance cliff.

Missing-effect `E0343` is the first migrated family. The non-recursive effect
coordinator appends the complete call interval; standalone output is unchanged
and `project-effect-error` pins `E0343@app@72:94`. Self-validation remained
about 0.22 seconds.

The same bounded pattern then migrated temporary-inout `E0348` and
aliased-inout `E0349`. Their project fixtures pin `E0348@app@164:178` and
`E0349@app@211:217` while standalone output stays unchanged. Inline issue
construction initially crossed the source-shape cliff; extracting small
whole-form and one-token leaf helpers restored the approximately 0.22-second
self-check. Final seed size and corpus totals are recorded after the release
gate: a byte-identical 1,653,846-byte C seed, 89 fixtures, and 2,000 malformed
mutations.

## Checked emission links

D0036 removes declaration relinking from `codegen/emit_program`. Standalone and
project compilation already retain the exact token stream linked during type
analysis, so the backend now consumes that checked evidence directly. The
fixed-point seed shrank from 1,653,846 to 1,653,599 C bytes.

A same-host quick series did not show a clean latency signal. The 2,000
declaration median moved from 32.279 ms to 31.987 ms, the 1,000 declaration
point improved from 22.132 ms to 16.344 ms, and smaller points varied in both
directions. This is recorded as one proven linear pass removed, not as a stable
percentage speedup. Permanent exponent and emit/check-ratio budgets remain the
performance authority.

## Bound variant type links

D0037 closes a checker/backend gap exposed by an adversarial aggregate fixture.
A variant copied from a parameter into a lexical local passed type checking but
the backend's parameter-only type scan emitted invalid C. Variant-match emission
now decodes the scoped type link already attached to the bound scrutinee. The
obsolete parameter scan was removed.

`variant-local-match` compiles, runs, and prints `42`; the previous parameter
fixture remains green. Self-validation stays near 0.22 seconds and the
byte-identical seed shrank from 1,653,599 to 1,650,892 C bytes. The corpus now
passes 90 fixtures and 2,000 malformed mutations.

A broader trial annotated composite expressions and lowered a direct
variant-producing match scrutinee. The annotation itself could be kept small,
but generalized nested-expression lowering pushed self-validation beyond 12
seconds. It was reverted completely. This checkpoint claims bounded scrutinees,
not arbitrary expression lowering.

## Remaining Core 1D blockers

- Project checking, scheduling, ordinary emission, and cache misses now share
  one prepared artifact. Each flattened token retains its module and original
  byte span; the `project-type-error` fixture pins `E0344@app@56:60`.
- Legacy exhaustiveness, recursive-inout, and move diagnostics still need to
  move into the structured issue channel so every project semantic diagnostic
  uses the same projection.
- Code generation and memory planning still rediscover some facts from token
  structure instead of consuming dense typed-view queries.
- Member lookup, adversarial aggregate tests, allocation-failure checks,
  sanitizers, and the complete release gate remain to be frozen together.
