# SLIM performance contract

Status: SLIM 0.9 permanent regression contract

Fixtures/budgets are permanent (RFC-0030). Dated results never replace gates;
never weaken safety for measurements.

## Evidence layers

Independent layers:

1. **Deterministic work:** output/repeated byte identity, observed work,
   invalidation estimates (not executed incremental work), cache behavior and
   bootstrap fixed points.
2. **Portable regression:** geometric exponents and same-host ratios, never
   unlike-machine timings.
3. **Dated measurements:** medians, source/output sizes, toolchains, hosts,
   reproduction commands.

Authoritative ledger: `benchmarks/performance-budgets.tsv`. Tightening is direct;
relaxation requires an accepted RFC scoring ≥ +60, measured impact and
containment/compensation.

## Independent dimensions

Separate compiler startup/input, checking, deterministic C emission, incremental
invalidation, external C compilation, generated runtime, binary size, compiler
memory where stable, source/lexical size, diagnostic size, edit span and correctness.
Never aggregate away regressions or equate runtime with compiler speed or token
proxies with LLM success.

Separator-dense metrics: source bytes, neutral/comma lexemes, canonical AST nodes,
parse/check medians. Temporary commas add zero AST nodes.

## Scaling and runtime gates

Geometric declarations: quick 250, 500, 1,000, 2,000; full through 8,000.
Checking/emission/reduction/analysis/proof obey recorded approximately-linear
exponents. Shapes: nested bindings, named type references, owned transfers,
aggregate temporaries, planned allocation calls, shared reads.

Twenty comparative challenges must match C/Rust oracles before runtime ratios.
Per-program budgets govern algorithm-specific checked arithmetic/bounds/storage/
recurrence costs. Host/parallel comparisons use identical operations/generated
programs on one warmed host.

Ten maintained multi-module source-library projects add records/variants/ownership/
parsing/recursion/binary output/bounded host I/O to generated series. Their gate:
alternating check/emission, repeated byte-identical C, separate source/generated
sizes, each same-host emit/check ratio limited.

Direct lowering requires a positive fact for that exact checked node. Native builds:
portable `-O3`, no LTO/profiles/target-specific flags.

## Running the contract

Complete gate: `./scripts/verify.sh`. Focused commands:

```text
cargo run --release --bin slim-bench -- performance --quick
cargo run --release --bin slim-bench -- work --quick
cargo run --release --bin slim-bench -- reduction --quick
cargo run --release --bin slim-bench -- parallelism
cargo run --release --bin slim-bench -- applications --quick
cargo run --release --bin slim-bench -- compare --quick
cargo run --release --bin slim-bench -- host
cargo run --release --bin slim-bench -- agent
```

## Observed compiler work

Opt-in `slim-bench work` instruments a reproduced production C compiler copy;
normal compiler/semantics/seed/runtime ABI/generated applications unchanged. Rust constructs/
checks observations, never semantics. Require ordinary stdout/stderr/exit-status
equality, two observed runs' identical counts/output. Verification includes quick;
`work --quick --sanitize` adds ASan/UBSan.

Versioned `src/bin/slim-bench/work.rs` hooks identify source functions/count units:
native entries, loop headers or input bytes. Bytes are offered parser input, not
parsed bytes; headers include terminal tests. Tail `recur` adds a header, not
native entry. Calls count attempts, not successful declarations/all semantic work.

| Counter group | Observed operations |
| --- | --- |
| Parsing | Program-parser entries/input bytes, source lexer headers, data-lexer entries/input bytes, declaration-index entries/headers, name-trie insertion/lookup/edge headers |
| Checking | Checker/typer entries, typed-declaration headers, expression-check entries, ownership-find entries/frame closures, memory-plan/range-analysis entries |
| Generation | C-program/C-function/full-expression emitter entries |
| Cache | Requests, key builds, probes, hit/miss handlers, checksum headers |
| Snapshot model | Snapshot-builder and invalidation-estimate entries |
| File input | Runtime read calls/`fread`-delivered bytes, including metadata/artifacts/repeated reads |
| External backend | Harness-observed C compilation calls, separate elapsed time, native fixture exit status |

32 native counters check remaining capacity before adding; cap 1,000,000,000.
Unsaturated counts/cap equality: `exact` for the observed process; excess: `bounded`.
Missing/malformed/incomplete exits: unknown/fail. Signals/report I/O failures never
mean zero. Small-cap UBSan saturation: `UINT64_MAX`.

Reports: separate temporary file outside compiler output/diagnostics; reporting I/O
excluded from read counters. Serial, no worker macros. Separate setup compilation/
seed reproduction/application C compilation/native frontend records. Instrumented
timings are not production latency; ordinary performance/paired-latency remain
independent.

Record seed/compiler/instrumented-source/runtime/probe/harness identities,
host/toolchain/flags/input bundles/output identities. FNV aids identity, never
authentication/semantic equivalence; seed reproduction/byte comparisons establish
equality. Missing/duplicate anchors fail, never zero.

Permanent geometric fixtures observe parser/checker/generator/name-trie work;
many-owner reinitialization observes frame closures/union-find. Fixture-only formulas:

- N declarations: declaration-index/type-declaration headers N+2 each,
  expression-check entries 2N+3, C-function entries N+1; lexer headers ≤ twice
  input bytes + one, name-edge headers ≤ 64 × (input bytes + one).
- N owners reset in both arms: 3N frame closures, ≤ 16N+32 union-find calls.
- RFC-0127 N functions plus main: exactly N+1 body checks and 2(N+1)
  binding-fact materialization headers; check/emission ≤ 16N+128 allocation attempts.
  Attempts count at the runtime increment, including injected failure/excluding
  later declined calls. Cumulative requested payload bytes use its size before
  zero-size normalization, excluding runtime headers/system allocator overhead, not
  peak memory. Both counters retain the cap. The 128-ordinal ownership campaign
  requires at least one failure after function checking starts and success beyond
  allocations. Failed
  attempts equal injected ordinals; no partial stdout.
- RFC-0128: four aggregate declarations/group, exactly 5N inline-type visits, 4N emitted
  definitions (zero for checking), ≤ 16N+128 allocation attempts; shared leaves complete
  once by the existing checker. The 256 fault ordinals cross layout-started/emission-started failures and
  success beyond allocations. Failures preserve exact attempts; no partial C.
- RFC-0129 optional flow: independent task-step/block/edge/pending-task bounds,
  caller budget 1..1,000,000. Binding chains: exactly 3N+4 blocks, 3N+2 edges,
  2N+1 processed tasks. Native probes separately count derive/walk entries,
  walk headers/returned steps. Success: headers = steps + walk entries,
  repeatable counts, ordinary/sanitized output equality. Measurement-only observer:
  cap 1,000,000,000, absent from installed compiler/runtime. 512 fault ordinals:
  post-walk-start failures, no successful partial graph, later success. Preceding
  checker stays recursive; dated report records its pre-existing sanitizer stack
  limit.

Two-module edit/cache parses: clean three, hit zero program parses, unchanged
snapshot four. Both compilers emit 20 native challenges for byte comparison.
Bounded allocation faults verify partial work/preserve diagnostics, establishing
neither M1 retained queries nor LLM effectiveness.

Performance-directed compiler/runtime changes record warmed same-host
baseline/candidate measurements. Full releases run non-quick commands and retain
dated reports.

## Historical evidence

RFCs/`benchmarks/results/` preserve rationale, acceptance ratios and milestone
measurements; Development evidence supplies numbers, this contract durable rules.

### Retained function typing

RFC-0130 inference reuse is native-observed; RFC-0143 public framing has pending
closure. Geometric unchanged/helper-body edits execute zero/one function checks; parsing/
linking/globals/generation persist. The [current work table](../benchmarks/results/archive/2026-09-06-m1-compact-retained-work.tsv.gz)
records counts/repeated clean equivalence; the [current latency table](../benchmarks/results/archive/2026-09-06-m1-compact-retained-latency.tsv.gz)
separates ordinary checks, old/new retained storage and two-clean comparisons.
RFC-0131 lowers setup/update overhead; snapshot-plus-update still exceeds two clean
checks here, not isolated warm-query latency. Snapshot creation/copying/public
integration remain M1 work; gates unchanged.

RFC-0131 measured-host node records: 264→64 bytes; temporary pre-inference links: 104→eight bytes.
Native probe compilation gates payload ceilings, not peak RSS; additional indexes/
capacity slack/copied source spans. Configurable token limit: 1..1,000,000, not bytes/RSS.
Invalid/over-limit configurations: ordinary checks/capacity misses. Nonnegative Work
counters are exact within this token domain; `executed = -1`: retained-loop work
uncounted (capacity misses check ordinarily; early rejection skips typing).
Missing counters never mean zero. Separate observer: 64 phases, checked saturation at
1,000,000,000 calls; returns cannot certify publication.
[Current fault observations](../benchmarks/results/archive/2026-09-06-m1-compact-retained-faults.tsv.gz)
cover 512 full differential-probe allocation ordinals.

### Retained project preparation

RFC-0132 connects the same checker to project validation/source-mapped preparation.
[Project work observations](../benchmarks/results/archive/2026-09-07-m1-retained-project-work.tsv.gz):
zero/one function-check calls for unchanged/helper-body edits through 4,000 helpers/two modules.
Complete fields/memory plans/C match clean preparation; permanent family imports:
15N+22/15N+7 canonical nodes respectively. Module parsing/flattening/reparse/globals/
generation remain work.

[Project timings](../benchmarks/results/archive/2026-09-07-m1-retained-project-latency.tsv.gz)
separate ordinary before/after preparation from two-clean-versus-retained runs;
the latter favors clean preparation, not isolated warm-query latency.
[Project fault observations](../benchmarks/results/archive/2026-09-07-m1-retained-project-faults.tsv.gz)
cover 2,048 complete differential-probe ordinals. Phase returns are not publication;
historical estimates use a measurement fixture, public-session measurements remain
independent.
