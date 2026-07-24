# Core 1C performance contract

Status: permanent SLIM 1.0 contract

Performance tests are permanent compatibility evidence. They are not deleted
after an optimization lands, and a result file never replaces an executable
gate. D0030 defines the governing architecture.

## Evidence layers

The repository uses three complementary layers:

1. **Deterministic work gates** assert accepted output, byte-identical repeated
   generation, exact incremental work counts, cache behavior, and fixed-point
   bootstrap output.
2. **Portable regression gates** use geometric scaling exponents and same-host
   ratios. These tolerate different computers while rejecting a newly
   superlinear pass or a large relative slowdown.
3. **Frozen measurements** record medians, source and output sizes, toolchain,
   host, and reproduction commands. They make smaller changes visible even
   when they remain inside a safety margin.

The versioned machine-readable budgets live in
`benchmarks/performance-budgets.tsv`. A budget may become stricter without a
compatibility exception. Making it weaker requires an accepted decision scoring
at least +60 with quantified evidence and containment or compensation.

## Required dimensions

The maintained suite separates:

- process startup and source I/O where the public command includes them;
- checking and deterministic C emission;
- clean, unchanged, private-body, and interface-changing incremental work;
- external C compilation;
- generated-program runtime and binary size;
- peak compiler memory where the host exposes a stable measurement;
- source bytes, neutral lexical tokens, diagnostic bytes, and edit span; and
- correctness and safety outcome.

No aggregate score hides one dimension. Native runtime measurements never
stand in for compiler speed, and model-token proxies never stand in for an LLM
success rate.

## Core 1C gates

The quick release suite uses 250, 500, 1,000, and 2,000 declaration programs.
The full suite extends the geometric series through 8,000 declarations. Both
ordinary checking and C emission must have a process-level scaling exponent no
greater than 1.25. At 2,000 declarations, emission may take at most four times
the check duration on the same warmed host.

A second geometric check series places a named aggregate declaration after
every function that references it. This permanently guards the memory planner's
bounded checked-type lookup against restoration of per-reference declaration
scans. Its process-level scaling exponent is also limited to 1.25.

Core 1C must improve the recorded 2,000-declaration C emission result by at
least five times on the baseline host without materially regressing checking,
generated runtime, generated binary size, or compiler peak memory. Challenge
programs must match the C oracle's output; relative runtime budgets are tracked
per workload because the cost of retained checked arithmetic and bounds checks
differs by algorithm.

Incremental declaration work remains exact. Wall-time improvements must attack
measured whole-input indexing, graph construction, or output assembly rather
than weakening invalidation.

## Running the contract

The standard gate is:

```text
./scripts/verify.sh
```

Focused quick measurements are:

```text
cargo run --release --bin slim-bench -- performance --quick
cargo run --release --bin slim-bench -- compare --quick
cargo run --release --bin slim-bench -- host
cargo run --release --bin slim-bench -- agent
```

A full release freeze runs the same commands without `--quick` and commits the
dated results. A performance-directed compiler or runtime change records a
baseline and candidate on the same host after warmup.

## Core 1E native-efficiency boundary

Core 1E keeps checked and unchecked execution from becoming separate language
profiles. D0059 exposes small checked scalar, byte, and vector operations as
one canonical header-visible runtime implementation so the existing C compiler
can optimize the successful path without LTO. Exact arithmetic, conversion,
byte, and vector trap fixtures are permanent correctness gates.

Later check elimination requires a positive proof derived from Core 1D's
checked artifact. Lack of a fact preserves the dynamic check. Source
annotations, optimizer hints, a second executable IR, and LLVM are not part of
this milestone. Native ratios tightened after D0059 are compatibility limits;
they may not be widened without the scored exception and evidence required by
D0030.

D0061 marks Core 1E complete after D0060 removes only child-region work
disproved by the retained allocation plan. The accepted release ratios span
1.017-1.871 for twelve real programs, while the self-host check consumes 0.03
seconds of user CPU. Remaining collection and arithmetic checks require future
positive proofs; the milestone does not infer safety from optimization level.

## Core 1G parallel-runtime contract

D0071 adds same-host generated parallel/forced-serial ratios for
`state_machine` and `signal_network` to the permanent `parallel-runtime` gate.
The command verifies output first, warms both modes, and compares medians from
the identical generated executable. Its existing geometric manual reference is
retained independently. A generated ratio budget may be tightened normally;
relaxing it follows the scored performance-exception policy above.

## Core 1I host-call contract

D0075 compares 100,000 `io.monotonic-ms` samples in generated SLIM with a C
reference using the same runtime operation. The same-host SLIM/C ratio must not
exceed 2.00. This isolates language call and recurrence overhead from the host
clock's own latency. The test also requires successful output-free execution;
it is not a substitute for the type, effect, allocation, trap, or monotonicity
checks.

D0076 separately compiles unrelated Hello executables with the supported
network implementation enabled and disabled. Their same-host binary-size ratio
must not exceed 1.03. The gate bounds inactive host support without pretending
that loopback timing predicts real network latency.

## Post-1.0 safe C-parity program

D0084 expands the matched native corpus from fourteen to twenty applications
before optimizing it. The suite now covers dense graphs, backtracking,
disjoint-set mutation, cellular and image stencils, and byte dynamic
programming in addition to the retained workloads. The runner reports separate
all-corpus and serial-only geometric means; per-application rows remain
authoritative.

D0085 lets C emission consume exact-node integer totality. Direct `+`, `-`,
`*`, `/`, and `%` appear only for an operation already proved total; unknown
facts retain the checked helper. Closed-program exact scalar parameters
propagate for four fixed passes when every call agrees and recurrence preserves
the value.

The first full candidate improves the expanded SLIM/C geometric mean from
1.371 to 1.359 and the serial mean from 1.450 to 1.439.
`image_convolution` improves from 1.596 to 1.171. This is progress, not a C
parity claim: acceptance still requires an all-application full-run mean at or
below 1.00 with every safety and regression gate intact.

## Core 1J structured-host contract

D0078 adds two maintained loopback applications to `slim-bench host`.
`dual_fetch` returns owned byte vectors across the join; `dual_health` returns
scalar facts. Each generated program is built once with forced serial fallback
and once with the POSIX worker tier. Output is checked before five warmed
same-host medians are compared.

Each parallel/serial ratio must not exceed 0.75. The accepted measurements were
0.529 and 0.534. This is a latency regression gate for demonstrably independent
bounded operations, not a claim about public-network performance.
