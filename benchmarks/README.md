# SLIM comparative benchmarks

Each challenge has independent SLIM, C11, and Rust implementations using the
same algorithm and data scale. The runner builds every implementation, rejects
output mismatches, warms each binary, and reports the median of 15 process runs.

Run:

```text
cargo build --release --bins
target/release/slim-bench performance
target/release/slim-bench compare
target/release/slim-bench incremental
target/release/slim-bench project
target/release/slim-bench reduction
target/release/slim-bench parallelism
target/release/slim-bench parallel-runtime
target/release/slim-bench agent
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

`performance` is the canonical compiler-speed gate. It checks ordinary
checking and deterministic C emission over geometric declaration counts,
including both scaling exponents and the same-host emit/check ratio. Every
generated declaration after the first calls a user function, so repeated
whole-program call resolution cannot hide behind a built-in-only workload. A
second geometric series checks deeply nested lexical bindings that repeatedly
reference an outer parameter, guarding both scope lookup and compiler stack
behavior. Its
versioned limits are in `performance-budgets.tsv`; relaxing a limit requires
the D0030 decision process. The standard verification script runs the quick
performance and matched-challenge gates on every release.

The incremental command generates wide and deep module graphs at geometric
sizes and measures no-change, private-body, and public-interface sessions. It
asserts exact parse/lower/check/generation work and enforces independent
scaling budgets, so a full recompilation cannot masquerade as incremental
work. Committed measurements live in `benchmarks/results/`.

The `project` command generates geometric wide and deep import graphs and
measures deterministic C emission at one, two, and four requested workers.
Repeated runs and every worker count must produce identical bytes. Serial
wide/deep emission has its own scaling budget. Workers remain opt-in because
these small graphs still show no stable parallel speedup; the compiler does
not claim concurrency that its runtime cannot yet provide.

The `parallelism` command runs the SLIM-written version-5 analysis twice over
every native challenge, requires deterministic balanced reports, and compares
schema-versioned safety and opportunity metrics with
`parallelism-baseline.tsv`. Schema 5 retains primary reasons, complete
per-function blocker incidence, and
candidate/selected/reported/executable/executed schedule counts across fourteen
applications. The `state_machine` and `signal_network` rows are permanent
positive cases: each has two independent two-million-step total recurrences and
must retain exactly one site through all five stages.
Changing an application or its proof classification requires an intentional
baseline update and dated explanation. Reason columns are primary blockers;
they must not be treated as the complete set of hazards in a function.

The `parallel-runtime` command is a permanent execution-boundary gate. It keeps
the serial and manual POSIX fork/join `state_machine` reference at geometric
recurrence counts to retain worker-overhead and crossover evidence. It also
builds the production-generated `state_machine` and `signal_network`, verifies
parallel output against `SLIM_TASK_DISABLE=1`, and measures warmed median
parallel/forced-serial ratios. The manual and generated ratios have separate
durable budgets. Generated workers use D0070's explicit POSIX tier; unsupported
targets retain the same production code with the portable inline tier.

The Core 0.3 freeze report in `results/2026-07-21-core-03.md` records the full
release gate, bootstrap size, conformance denominator, self-host clean/cache/
session process timings, and explicit claims that remain out of scope.

The `agent` command reads a permanent manifest of matched repairs in SLIM,
C11, and Rust. Core 1C covers unknown operations, type mismatches, missing
capabilities, non-exhaustive matches, and unterminated input. It reports source
bytes, a language-neutral ASCII lexical-token count, a clearly labelled
`ceil(bytes / 4)` model-token proxy, the common-prefix/suffix edit span,
diagnostic bytes, before/after acceptance, and median process-level compiler
feedback. Every broken fixture must be rejected and every fixed fixture
accepted before a row is printed. These static measurements can test token
economy and edit locality; they are not an LLM generation or repair success
rate. Native runtime and binary-size comparisons remain in `compare`.
