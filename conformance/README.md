# Core 0 conformance corpus

manifest.tsv is the executable inventory of Core behavior. Each row has six
tab-separated fields:

    id  mode  source  selfhost  expectation  coverage-tags

Modes are check-pass, check-fail, run, trap, allocation-fail, format, and emit.
Diagnostic expectations list exact CODE@START:END entries. Runtime expectations
encode exit status, stdout, and stderr with explicit escapes.

`allocation-fail` runs the native artifact with deterministic failure at its
second allocation and verifies the typed allocation-effect boundary exactly.

The legacy selfhost column must be `parity`. It is retained as a machine-checked
assertion that every row belongs to the production SLIM path; the runner rejects
any fallback classification.

Run the production SLIM corpus with:

    cargo run --bin slim-conform -- check

The runner is orchestration only: every fixture is checked by the compiler
built from `bootstrap/slimc-seed.c`. It rejects any non-`parity`
classification. Reproduce the compiler independently with:

    ./bootstrap.sh

Core 1A adds end-to-end reduction assertions outside the manifest inventory:
an exact canonical normal form, repeated-byte idempotence, original/reduced
runtime equivalence, deterministic allocation-failure equivalence, and a
preservation case containing visible I/O followed by a checked overflow trap.
The malformed-input campaign also sends 100 deterministic mutations through
both `reduce` and `analyze`.

`projects/manifest.tsv` is the independent Core 0.2 inventory. Its fixtures
exercise project checking, native execution, exact project diagnostics,
relocation, cache corruption, incremental reuse, and worker-count variance.
Project rows currently require `parity`; the runner never silently substitutes
the former Rust compiler.
