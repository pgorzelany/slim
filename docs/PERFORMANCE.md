# SLIM performance contract

Status: SLIM 0.9 permanent regression contract

Performance fixtures and budgets are durable product infrastructure. A dated
result never replaces an executable gate, and safety rules are never weakened
to improve a measurement. RFC-0030 owns the architecture.

## Evidence layers

The repository maintains three independent layers:

1. **Deterministic work gates** check output, repeated byte identity,
   observed compiler work, invalidation estimates, cache behavior, and bootstrap
   fixed points. Invalidation estimates are not executed incremental work.
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

The source-library corpus adds ten real multi-module projects. Its application
gate alternates checking and C emission, requires byte-identical repeated C,
reports source and generated sizes independently, and limits each same-host
emit/check ratio. This supplements geometric generated-source series with
records, variants, ownership, parsing, recursion, binary output, and bounded
host-I/O shapes exercised by maintained programs.

Checked operations remain checked unless a positive fact for the exact node
justifies direct lowering. The canonical native build uses portable `-O3`
without LTO, profiles, or target-specific flags.

## Running the contract

The complete gate is `./scripts/verify.sh`. Focused commands are:

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

`slim-bench work` instruments an opt-in copy of the reproduced production C
compiler. The normal compiler, SLIM semantics, seed, runtime ABI, and generated
application code remain unchanged. Rust only constructs and checks observations;
it supplies no compiler semantics. Every measured operation must match ordinary
compiler stdout, stderr, and exit status, and two observed executions must have
identical counts and output. The full verification command includes the quick
campaign. `work --quick --sanitize` additionally checks the observed compiler
under ASan/UBSan.

The versioned hook table is in `src/bin/slim-bench/work.rs`. Its records identify
the actual source function and whether they count native entries, loop-header
visits, or input bytes. Input-byte counters measure bytes offered to that parser
entry, not bytes successfully parsed before an error. Header visits include the
terminal test; a tail `recur`
is a new header visit, not a new native function entry. Calls count attempts at
that entry point, not successfully checked declarations or all semantic work.

| Counter group | Observed operations |
| --- | --- |
| Parsing | Program-parser entries/input bytes, source lexer headers, data-lexer entries/input bytes, declaration-index entries/headers, name-trie insertion/lookup/edge headers |
| Checking | Checker and typer entries, typed-declaration headers, expression-check entries, ownership find entries/frame closures, memory-plan and range-analysis entries |
| Generation | C-program, C-function, and full-expression emitter entries |
| Cache | Requests, key builds, probes, hit/miss handlers, checksum headers |
| Snapshot model | Snapshot-builder and invalidation-estimate entries |
| File input | Actual runtime read calls and bytes delivered by `fread`, including metadata/artifacts and repeated reads |
| External backend | Harness-observed C compilation calls, separate elapsed time, and native fixture exit status |

The 32 native counters use a fixed 1,000,000,000 cap and arithmetic that checks
remaining capacity before addition. A counter that exceeds the cap reports
`bounded`; unsaturated counts report `exact` for that observed process. A count
exactly equal to the cap remains exact until another increment exceeds it.
Missing, malformed, or incomplete exit reports are unknown and fail the
campaign. Signals and report I/O failures must never appear as zero work.
Small-cap native tests exercise saturation with `UINT64_MAX` under UBSan.

The observer reports to its own temporary file, outside compiler output and
diagnostics. Its reporting I/O is excluded from compiler read counters. The
observed binary is serial, built without worker macros. Setup compilation,
seed reproduction, application C compilation, and native frontend work have
separate records. Instrumented timings are not production compiler latency;
the ordinary performance and paired-latency commands remain independent.

The campaign records seed/compiler/instrumented-source/runtime/probe/harness
identities, host, C toolchain, flags, input bundles, and output identities.
FNV fingerprints are identity aids, not authentication or proof of semantic
equivalence. Seed reproduction and byte-by-byte output comparisons supply
the relevant equality checks. Missing or duplicated observation anchors fail
instead of silently recording zero.

Geometric declaration fixtures gate actual parser/checker/generator visits and
name-trie work. A separate many-owner reinitialization series gates frame
closures and union-find calls rather than inferring them from source size.
For the fixed N-declaration fixture, declaration-index and type-declaration
headers each visit N+2 times, expression checking enters 2N+3 times, and C
function emission enters N+1 times. Lexer headers are capped at twice input
bytes plus one; name-edge headers at 64 times (input bytes plus one). For N owners
reset in both arms, the tracker closes 3N frames and performs at most 16N+32
union-find calls. These are permanent work gates for these named fixtures,
not a claim that their formulas apply to every program.
RFC-0127 additionally observes each function-body check and each binding-fact
materialization loop header. The generated N-function family plus main checks
exactly N+1 bodies and visits 2(N+1) materialization headers. Allocation attempts
are counted at the runtime's existing attempt increment, including an injected
failure but excluding calls declined after failure. Cumulative requested payload
bytes count the size passed at that same point, before zero-size normalization;
they exclude runtime headers/system allocator overhead and are not peak memory.
Both counters use the existing fixed cap. The generated check/emission family
allows at most 16N+128 attempts. The 128-ordinal ownership fixture must reach a
function check before at least one failure and must also reach successful
ordinals beyond its allocations. Failed attempts equal the injected ordinal and
produce no partial standard output.
RFC-0128's geometric shared-layout family contains four aggregate declarations
per group. It requires exactly 5N inline-type visits, 4N emitted definitions for
emission (zero for checking), and at most 16N+128 allocation attempts. Shared leaf
dependencies are completed once by the existing checker. A 256-ordinal campaign
must cross failures after layout traversal starts, failures after aggregate
emission starts, and successful ordinals beyond all fixture allocations; failures
must preserve exact attempt counts and produce no partial C output.
RFC-0129 bounds optional flow construction independently by task steps, blocks,
edges and pending tasks, with a caller budget in 1..1,000,000. The retained
geometric binding-chain fixture has exactly 3N+4 blocks, 3N+2 edges and 2N+1
processed tasks. Native probe observation separately counts derive/walk entries,
walk headers and returned task steps; successful runs require headers = steps +
walk entries, repeatable counters, and ordinary/sanitized output equality.
The measurement-only observer saturates at 1,000,000,000 and never enters the
installed compiler/runtime. Its 512 allocation-fault ordinals require failures
after task walking begins, no successful partial graph output, and later success.
This does not make the preceding recursive checker iterative. The dated flow
report records its pre-existing stack limit under sanitizer instrumentation.
The two-module edit/cache campaign distinguishes the three parses of a clean
build from zero program parses on a hit and four parses for an unchanged
snapshot comparison. All 20 native challenges are emitted by both compilers
and compared byte for byte. Bounded compiler allocation-fault ordinals verify
partial-work reports without changing failure diagnostics. No result establishes
M1 retained incremental queries or an LLM effectiveness rate.

A performance-directed compiler or runtime change records baseline and
candidate measurements on the same host after warmup. Full release evidence
runs the non-quick commands and preserves its dated report.

## Historical evidence

Optimization rationale, acceptance ratios, and milestone-specific measurements
remain in their numbered RFCs and `benchmarks/results/`. The current contract
contains only the durable measurement rules; Development evidence provides the
historical numbers.
