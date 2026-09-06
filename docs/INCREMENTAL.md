# Incremental compilation status

The production SLIM compiler does **not yet implement retained, incremental
parsing, type checking, or C generation**. RFC-0112 M1 makes those an explicit
implementation goal. The former Rust `IncrementalSession` API is not part of
the production compiler.

## What the current session command measures

`selfhost/session.slim` loads the initial and updated projects and builds
source snapshots. `selfhost/query.slim` compares declarations, identifies
changed bodies and interfaces, and propagates dependency invalidation. The
identity is `(module, declaration kind, declared name)`.

The four integers printed by the internal `session` command retain historical
field names `parsed lowered checked generated`. They are **invalidation
estimates**, not counters of operations performed. In `query.measure_update`,
`parsed` counts classified changes, `lowered` copies that count, `checked`
counts invalidation flags, and `generated` copies the invalidated count.
No retained typed bodies or generated fragments are reused by this command.
A zero estimate on an unchanged project does not mean zero frontend work.

The recovery variant performs clean checks of its initial, rejected, and
recovered inputs and compares snapshots after recovery. It does not establish
transactional retention of a previous checked compilation.

## Historical measurements

`cargo run --release --bin slim-bench -- incremental` exercises this dependency
invalidation model. It remains useful as a regression test for the selected
change set and its scaling, but cannot establish incremental compiler latency
or avoided parse/check/code-generation work.

The [2026-07-21 measurements](../benchmarks/results/2026-07-21-incremental.tsv)
are retained as historical evidence. Their operation-labelled columns must
not be cited as demonstrated reuse by today's production SLIM compiler.

## Acceptance boundary for SLIM Next

M1 must retain checked results attached to stable canonical SLIM identities,
invalidate their actual dependencies, and compare every successful update with
a clean compilation. It must instrument work where parsing, checking, and
emission actually occur. Tests must include body, signature, layout, effect,
borrow-mode, deletion, insertion, relocation, and failed-then-recovered edits.

Source indexing, metadata updates, output assembly, external C compilation,
and queueing must remain visible as separate costs. A fast invalidation model
is useful infrastructure; only a measured working compiler can establish a
fast compilation cycle. See [the SLIM Next implementation report](../benchmarks/results/2026-09-05-slim-next-progress.md).
