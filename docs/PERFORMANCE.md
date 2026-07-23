# Core 1C performance contract

Status: Core 1C complete

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
