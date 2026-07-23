# SLIM Agent Rules

These rules apply to every change in this repository.

## Before changing code

1. Read `DESIGN.md`, `docs/CORE.md`, and `design/FEATURE_POLICY.md`.
2. Treat every language feature as a permanent cost until proven otherwise.
3. Changes to syntax, semantics, types, ownership, effects, runtime ABI,
   compiler architecture, dependencies, or compatibility require an accepted
   decision record in `design/decisions/`.

## Language constraints

- There is one canonical source form for each semantic operation.
- Do not add aliases, syntactic sugar, implicit conversions, hidden control
  flow, hidden allocation, hidden copying, or hidden synchronization.
- Prefer ordinary library composition. Add a compiler primitive only when a
  required safety, analysis, or performance property cannot be preserved by a
  library implementation.
- Replace an inferior form; never keep two permanent ways to express the same
  operation.
- Pure code is the default. Effects must be visible in function declarations.
- Safe code never exposes raw pointers, unchecked indexing, or untracked
  aliases.

## Performance constraints

- Default clean compilation must remain approximately linear in source size.
- Keep incremental work local to the changed declaration and its dependents.
- Superlinear whole-program work belongs behind an explicit, budgeted command.
- Code that does not use a feature pays no runtime cost for that feature.
- Never weaken a safety rule to improve a benchmark.
- Benchmark before and after any compiler, runtime, or generated-code change
  intended to affect performance.
- Performance fixtures, metric definitions, and regression gates are durable
  product infrastructure. Do not remove a passing gate after an optimization.
- A performance budget may be tightened directly. Relaxing one requires an
  accepted decision scoring at least +60, a measured cause, quantified impact,
  and a containment or compensation plan.
- Keep absolute dated measurements for tracking, but gate portable builds with
  deterministic work, geometric exponents, and same-host ratios rather than
  pretending timings from unlike machines are interchangeable.

## Reduction and representation constraints

- Canonical parsed SLIM is the compiler's sole accepted program
  representation. Do not add a `.sil`, `.slir`, or other separately parsed IR
  without a new accepted decision proving that derived views are insufficient.
- Attach semantic facts to stable SLIM nodes or derive an ephemeral bounded
  view. Do not duplicate parsing, typing, ownership, or effects.
- Every direct rewrite needs a local totality and behavior-preservation
  argument. It must preserve evaluation order and may not hide or discard an
  observable effect, trap, allocation failure, move, mutation, or borrow.
- Reduction must terminate, be deterministic and idempotent, and retain a
  conservative result when a stated analysis bound is exceeded.
- A new rewrite is a feature: weigh it through `design/FEATURE_POLICY.md`, add
  positive and non-applicability tests, and measure direct-reduction scaling.

## Evidence and agent-tool constraints

- Classify every semantic claim as `exact`, `bounded` with its fixed budget, or
  `unknown` with a stable reason. Never turn missing evidence into a negative
  quality score.
- Report independent quality facts; do not invent one universal goodness score
  or infer application invariants from representable state alone.
- Keep default evidence collection approximately linear. Exhaustive search is
  allowed only behind an explicit command with a hard checked bound.
- A checked operation may lose its trap hazard only from a positive fact for
  that exact checked node. Out-of-domain values, saturated refinement budgets,
  missing facts, and unsupported transfers remain unknown.
- Keep abstract interpretation domains and refinement/report limits explicit,
  deterministic, and crossed by permanent tests. The analyzer's own arithmetic
  must be unable to overflow inside its declared proof domain.
- Keep the native application analysis baseline exact and durable. Explain and
  record every changed row; primary blocker counts are not complete hazard sets
  and cannot alone justify a new precision feature.
- Preserve every discovered parallel blocker even when an earlier blocker is
  already sufficient. Safe functions require an exact empty blocker set; graph
  uncertainty must remain explicit at the function boundary.
- Treat a declared effect list as a checked capability ceiling, not proof that
  an event occurred. Discharge an unused capability only from the complete
  checked body and call graph; missing or bounded-away implementation evidence
  remains unknown.
- Prove recurrence total only from a complete structural descent argument:
  exact guard, the same linked controller, positive exact step, total update,
  total recurrent arguments, total base and prefix, and one tail recurrence.
  Direction alone is never a termination proof.
- Keep parallel candidates distinct from the selected plan. Selection uses
  stable lexical node order, emits only pairwise non-overlapping intervals, and
  becomes bounded rather than silently truncating after 64 reported sites.
- Do not reinterpret a selected site as executed. Core 1F execution remains
  disabled until D0069's portable-worker, capture/result, fallback, no-nesting,
  and target-cost reopening conditions are all satisfied by a replacement
  decision and permanent multi-application evidence.
- Equivalence results apply only to the complete domain named in the report. A
  supported mismatch must include the first deterministic counterexample.
- Proof records are explanatory data. Replay must recompute the accepted result
  from checked source rather than trust proof text.
- Agent patches are versioned, bounded, non-executable tooling data. They may
  address one stable canonical node, but the normal checker remains the sole
  authority that accepts the resulting program.
- Source/model-token proxies and edit-size measurements are evidence, not an
  LLM success rate. Keep native performance measurements independent.

## Change requirements

- Implement new production compiler capabilities in `selfhost/` SLIM by
  default. The portable C seed and SLIM sources are the production compiler;
  Rust is limited to bootstrap-independent verification and measurement.
- Do not add production Rust semantics or a second compiler path. Any proposed
  exception requires a separate accepted architecture decision and must not
  bypass the D0027 cutover gate.
- Never claim support through a semantic fallback. Every conformance row must
  execute through the production SLIM compiler.
- Preserve deterministic formatting, diagnostics, and generated C.
- Add positive, negative, and diagnostic tests for behavior changes.
- Run `./bootstrap.sh`, `cargo run --bin slim-govern -- check`, `cargo test`,
  `cargo run --release --bin slim-bench -- performance --quick`,
  `cargo run --release --bin slim-bench -- reduction --quick`,
  `cargo run --release --bin slim-bench -- parallelism`,
  `cargo run --release --bin slim-bench -- compare --quick`, and
  `cargo run --release --bin slim-bench -- agent` before committing a compiler,
  runtime, benchmark, or agent-tool change.
- Do not add a dependency without an accepted architecture decision.
- If a requested change conflicts with a hard gate, record the conflict rather
  than bypassing the gate.
