# SLIM comparative benchmarks

Each challenge has independent SLIM, C11, and Rust implementations using the
same algorithm and data scale. The runner builds every implementation, rejects
output mismatches, warms each binary, and reports the median of 15 process runs.

Run:

```text
cargo build --release --bins
target/release/slim-bench compare
target/release/slim-bench scaling
target/release/slim-bench incremental
target/release/slim-bench project
target/release/slim-bench reduction
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

The incremental command generates declaration chains at geometric sizes and
measures cold compilation, no-change updates, private body edits, leaf
interface edits, and central interface edits. It asserts exact declaration
parse/lower/check/generation counts before printing timing results, so a full
recompilation cannot masquerade as incremental work. Committed measurements
live in `benchmarks/results/`.

The `project` command generates geometric wide and deep import graphs. It
measures clean, no-change, private-body, public-interface, and persistent-warm
updates plus clean checks at one, two, and four requested workers. Every row
records module and declaration parse/lower/check/generation/reuse counts,
invalidation closure, persistent hits, requested jobs, and maximum layer
width. Assertions reject unexpected work or worker-dependent artifacts before
printing results.

The current project measurements are intentionally unflattering where the
architecture is unfinished. At 129 declarations, clean serial checks took
2.9--4.1 ms, while two/four-worker checks took 7.3--9.4 ms because workers
clone and rebuild declaration lookup state. Parallelism therefore remains
opt-in and serial remains the default. No-change and private-edit wall time
still grows with project size because source reads, lexical indexing,
environment reconstruction, graph rebuilding, and output assembly remain
whole-project operations. A fully valid persistent cache performs zero
declaration frontend/code-generation work, but many small cache-file reads can
cost more wall time than a clean check at these sizes. See
`results/2026-07-21-project.tsv` for the reproducible evidence.

The Core 0.3 freeze report in `results/2026-07-21-core-03.md` records the full
release gate, bootstrap size, conformance denominator, self-host clean/cache/
session process timings, and explicit claims that remain out of scope.

The `agent` command uses one matched unknown-operation repair in SLIM, C11, and
Rust. It reports source bytes, a language-neutral ASCII lexical-token count, a
clearly labelled `ceil(bytes / 4)` model-token proxy, the common-prefix/suffix
edit span, diagnostic bytes, before/after acceptance, and median process-level
compiler feedback. Every broken fixture must be rejected and every fixed
fixture accepted before a row is printed. These static measurements can test
token economy and edit locality; they are not an LLM generation or repair
success rate. Native runtime and binary-size comparisons remain in `compare`.
