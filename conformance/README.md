# Core 0 conformance corpus

manifest.tsv is the executable inventory of Core behavior. Each row has six
tab-separated fields:

    id  mode  source  selfhost  expectation  coverage-tags

Modes are check-pass, check-fail, run, trap, allocation-fail, format, and emit. Diagnostic
expectations list exact CODE@START:END entries. Runtime expectations encode
exit status, stdout, and stderr with explicit escapes.

`allocation-fail` runs the native artifact with deterministic failure at its
second allocation and verifies the typed allocation-effect boundary exactly.

The selfhost column is either parity or stage0-only(reason). A parity row must
compile and behave equivalently through the self-hosted compiler when the
differential gate is enabled. Capability differences are data in this manifest;
the runner does not silently fall back to stage 0.

Run the stage-0 corpus with:

    cargo run --bin slim-conform -- check

Run the stage differential gate with:

    cargo run --bin slim-conform -- differential

`projects/manifest.tsv` is the independent Core 0.2 inventory. Its fixtures
exercise project checking, native execution, exact project diagnostics,
relocation, cache corruption, incremental reuse, and worker-count variance.
Project rows follow the same explicit `parity` or `stage0-only(reason)` rule;
the differential runner never silently substitutes stage 0.
