# SLIM comparative benchmarks

Each challenge has independent SLIM, C11, and Rust implementations using the
same algorithm and data scale. The runner builds every implementation, rejects
output mismatches, warms each binary, and reports the median of 15 process runs.

Run:

```text
cargo build --release --bins
target/release/slim-bench compare
target/release/slim-bench scaling
```

SLIM compilation time includes the frontend, deterministic C emission, and an
optimized Clang invocation. Frontend-only scaling is reported separately. C is
the unchecked native performance ceiling; Rust retains memory and bounds
safety. SLIM retains bounds and checked-arithmetic traps. Results therefore show
both current code quality and the cost still awaiting proof-based check removal.

The `bytefreq` runner creates an 8 MiB input containing every byte value in a
uniform repeating sequence. Other workloads use deterministic constants in all
three implementations.

These programs are regression evidence, not proof of general language parity.
A benchmark gap is fixed in analysis or code generation; it does not justify
unsafe semantics or benchmark-specific syntax.

