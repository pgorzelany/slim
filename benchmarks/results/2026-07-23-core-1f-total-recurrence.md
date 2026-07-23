# Core 1F observed-effect and total-recurrence evidence

Date: 2026-07-23
Status: accepted D0066 evidence slice
Host: macOS arm64, same checkout and warmed local toolchain

Commands:

```text
cargo run --release --quiet --bin slim-bench -- parallelism
cargo run --release --quiet --bin slim-bench -- reduction
cargo run --release --quiet --bin slim-bench -- compare --quick
```

## Application result

Schema 3 adds the matched `state_machine` SLIM/C11/Rust application. It contains
three functions, one checked arithmetic site, two comparison refinements, two
safe functions, and one eligible fork site:

```text
run           safe
same          safe
main          unavailable: allocation-or-io
eligible-sites 1
```

`run` applies a finite-state transition two million times. Two calls consume
distinct owned records and join before comparison and output. The report proves
the tasks race-free and deadlock-free but still labels profitability unknown;
D0066 does not create or execute a task.

The original `variants` application now has three total checked remainder
sites and one safe function, `command`. The other eleven original applications
remain without a safe function. Declared capabilities no longer appear as
observed blockers; every real effect, trap, recurrence, exclusive borrow, and
unsafe callee remains.

The preliminary five-sample native run measured:

| language | runtime us | binary bytes |
|---|---:|---:|
| SLIM | 2,808 | 51,744 |
| C11 | 2,833 | 33,440 |
| Rust | 3,271 | 466,336 |

The durable SLIM/C limit is 2.50. The release gate reruns the authoritative
fifteen-sample comparison; these timings are tracking evidence rather than a
portable absolute promise.

## Analysis scaling

| declarations | source bytes | analyze us | analysis bytes |
|---:|---:|---:|---:|
| 1,000 | 69,971 | 6,449 | 579,001 |
| 2,000 | 140,971 | 11,554 | 1,153,001 |
| 4,000 | 282,971 | 20,723 | 2,301,001 |
| 8,000 | 566,971 | 41,064 | 4,613,430 |

The endpoint exponent is 0.890, below the unchanged 1.25 budget. Generated
declaration bodies do not match the accepted recurrence shape, so report size
is unchanged and the additional inspection remains linear.

The portable compiler seed reaches a byte-identical fixed point at 1,880,430 C
bytes with SHA-256
`88d66f396a2fa5b78e23fb3ee484dbf8b778c0f64bb600452d0471f0df855ba2`.
That is 25,662 bytes (1.38%) above D0065.
