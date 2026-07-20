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

## Change requirements

- Preserve deterministic formatting, diagnostics, and generated C.
- Add positive, negative, and diagnostic tests for behavior changes.
- Run `cargo run --bin slim-govern -- check` and `cargo test` before committing.
- Do not add a dependency without an accepted architecture decision.
- If a requested change conflicts with a hard gate, record the conflict rather
  than bypassing the gate.

