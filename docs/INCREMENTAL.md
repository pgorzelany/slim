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

## Observed execution

`cargo run --release --bin slim-bench -- work --quick` now observes actual
native compiler entry points and loop headers. It builds an instrumented copy
of the reproduced portable seed, compares each operation with the ordinary
compiler, and repeats the observation to require identical counters and output.
It never installs that copy or adds counters to normal builds. Add `--sanitize`
to run the observed compiler under ASan/UBSan. Without `--quick`, the declaration
series extends through 8,000 declarations.

The maintained two-module fixture gives these exact per-process counts:

| Operation | Program parses | Checker calls | C-generation calls | Artifact-cache hits |
| --- | ---: | ---: | ---: | ---: |
| Clean C emission | 3 | 1 | 1 | 0 |
| Cold artifact-cache miss | 3 | 1 | 1 | 0 |
| Unchanged artifact-cache hit | 0 | 0 | 0 | 1 |
| Changed body with an old artifact | 3 | 1 | 1 | 0 |
| Unchanged snapshot comparison | 4 | 0 | 0 | 0 |

The third parse on a clean build is the existing flattened-source reparse after
the two module parses. A cache hit still reads the key manifest, both source
files, and the artifact frame, lexes the manifest, and validates the checksum.
The snapshot comparison parses each module in both input projects even when
its four printed invalidation estimates are zero. These counts demonstrate
whole-artifact reuse and snapshot-model work; they do not demonstrate retained
incremental checking or emission.

The session recovery variant performs clean checks of its initial, rejected, and
recovered inputs and compares snapshots after recovery. It does not establish
transactional retention of a previous checked compilation.

The observed campaign compares clean and cached C through body, signature,
layout, effect, borrow-mode, insertion, deletion, rename, reordering, and
relocation cases. It also checks corruption and failed-then-recovered inputs.
Only successful native miss frames are published by the harness; a rejected
input leaves the last good artifact unchanged. The recovered original source
can hit that artifact while session recovery still performs clean checks.
See [the work-measurement contract](PERFORMANCE.md#observed-compiler-work) and
[RFC-0121](../design/rfcs/0121-observed-compiler-work.md) for the observation
boundary, counter caps, and failure rules.

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
