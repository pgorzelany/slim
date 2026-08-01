# SLIM performance contract

Status: SLIM 0.9 permanent regression contract

Performance fixtures and budgets are durable product infrastructure. A dated
result never replaces an executable gate, and safety rules are never weakened
to improve a measurement. RFC-0030 owns the architecture.

## Evidence layers

The repository maintains three independent layers:

1. **Deterministic work gates** check output, repeated byte identity,
   incremental work, cache behavior, and bootstrap fixed points.
2. **Portable regression gates** use geometric scaling exponents and same-host
   ratios, avoiding comparisons between unlike machines.
3. **Dated measurements** record medians, source/output sizes, toolchains,
   hosts, and reproduction commands.

`benchmarks/performance-budgets.tsv` is the canonical machine-readable budget
ledger. A budget may tighten directly. Relaxing one requires an accepted RFC
scoring at least +60, measured impact, and containment or compensation.

## Independent dimensions

The suite reports compiler startup and input, checking, deterministic C
emission, incremental invalidation, external C compilation, generated runtime,
binary size, compiler memory where stable, source and lexical size, diagnostic
size, edit span, and correctness separately. No aggregate score hides a
regression. Native runtime is not compiler speed, and token proxies are not an
LLM success rate.

The separator-dense frontend series records source bytes, neutral lexemes,
comma lexemes, canonical AST nodes, and parse/check medians. Commas remain
temporary tokens and add zero canonical AST nodes.

## Scaling and runtime gates

Quick geometric compiler series use 250, 500, 1,000, and 2,000 declarations;
full series extend through 8,000. Ordinary checking, emission, reduction,
analysis, and proof remain within their recorded approximately-linear exponent
budgets. Separate shapes cover nested bindings, named type references, owned
transfers, aggregate temporaries, planned allocation calls, and shared reads.

The twenty comparative challenges must match their C and Rust oracles before
runtime ratios are measured. Per-program budgets remain authoritative because
checked arithmetic, bounds, storage, and recurrence costs differ by algorithm.
Host and parallel gates compare the same operation or generated program on the
same warmed host.

Checked operations remain checked unless a positive fact for the exact node
justifies direct lowering. The canonical native build uses portable `-O3`
without LTO, profiles, or target-specific flags.

## Running the contract

The complete gate is `./scripts/verify.sh`. Focused commands are:

```text
cargo run --release --bin slim-bench -- performance --quick
cargo run --release --bin slim-bench -- reduction --quick
cargo run --release --bin slim-bench -- parallelism
cargo run --release --bin slim-bench -- compare --quick
cargo run --release --bin slim-bench -- host
cargo run --release --bin slim-bench -- agent
```

A performance-directed compiler or runtime change records baseline and
candidate measurements on the same host after warmup. Full release evidence
runs the non-quick commands and preserves its dated report.

## Historical evidence

Optimization rationale, acceptance ratios, and milestone-specific measurements
remain in their numbered RFCs and `benchmarks/results/`. The current contract
contains only the durable measurement rules; Development evidence provides the
historical numbers.
