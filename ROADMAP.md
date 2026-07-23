# SLIM Roadmap

Status: Core 1D shared typed compiler view in progress
Current milestone: unify checking and generation on bounded typed facts
Last updated: 2026-07-22

## Direction

SLIM grows by strengthening guarantees and implementation capability before
expanding language surface. Compiler infrastructure, ordinary libraries, and
analysis are preferred over new primitives. Every durable architecture or
language decision remains subject to design/FEATURE_POLICY.md.

The long-term sequence is:

1. Core 0 conformance and executable semantics.
2. Declaration-local incremental compilation.
3. Deterministic multi-module projects.
4. Full self-hosted compiler parity.
5. Compile-time lifetime and region planning, informed by self-host evidence.
6. Typed optimization and predictable native code generation.
7. Deterministic structured concurrency.
8. Bounded computation, quality grading, and semantic reduction.
9. AI-native tooling and ecosystem hardening.

## Core 0.1 Foundations

Core 0.1 adds no language syntax. It establishes the evidence and compiler
architecture needed to change the language safely.

### Deliverable 1: executable conformance corpus

- Store standalone source fixtures outside Rust unit tests.
- Cover every accepted item, expression, type, effect, ownership rule, and
  built-in.
- Check stable diagnostic codes and primary byte spans.
- Check runtime output, exit status, and defined traps.
- Check canonical formatting and deterministic C generation.
- Exercise malformed-input recovery and multiple diagnostics.
- Fail when the accepted surface ledger has no corresponding coverage tag.

Acceptance:

- The corpus is runnable through one dependency-free command.
- Adding an accepted built-in without a conformance tag fails governance.
- All expected diagnostics compare code and exact primary span.

### Deliverable 2: stage differential gate

- Run applicable fixtures through stage 0 and the self-hosted compiler.
- Compare acceptance, native output, exit status, and determinism.
- Classify unsupported self-host capabilities explicitly in the manifest.
- Reject an unexplained acceptance or runtime divergence.
- Preserve the existing byte-identical compiler bootstrap proof.

Acceptance:

- Every fixture is marked parity or stage0-only with a reason.
- A parity fixture cannot silently fall back to stage 0.
- The differential report lists tested and intentionally deferred behavior.

### Deliverable 3: declaration identities and fingerprints

- Identify declarations by module, declaration kind, and declared name.
- Compute a stable syntax fingerprint from normalized semantic structure.
- Compute a separate public-interface fingerprint.
- Build explicit call and named-type dependency edges.
- Reject duplicate identities with existing compiler diagnostics.
- Keep fingerprints independent of whitespace and source offsets.

Acceptance:

- Whitespace-only edits preserve both fingerprints.
- Body-only edits change syntax but preserve interface fingerprints.
- Signature and data-layout edits change interface fingerprints.
- Graph output is deterministic across processes.

### Deliverable 4: real incremental compilation session

- Retain parsed, lowered, checked, and generated results in memory.
- Reuse unchanged declarations rather than merely calculating an invalidation
  set before performing a full compilation.
- Recheck a syntax-changed declaration.
- Recheck reverse transitive dependents only when its interface changes.
- Invalidate callers for call-signature changes and type users for layout
  changes.
- Reassemble deterministic program output from cached and fresh fragments.
- Rebase or regenerate diagnostic spans correctly after source movement.
- Recover safely from invalid edits and cache corruption.

Acceptance:

- A private body edit in a large valid program rechecks and regenerates exactly
  one declaration.
- A signature change rechecks only the declaration and transitive dependents.
- An unrelated declaration retains its semantic and generated cache entries.
- Incremental output equals a clean compilation byte for byte.
- Existing diagnostics and ownership/effect guarantees remain identical.

### Deliverable 5: empirical incremental benchmarks

- Generate geometrically increasing declaration graphs.
- Measure cold compilation, no-change update, private body edit, leaf
  interface edit, and central interface edit.
- Record declarations parsed, lowered, checked, and generated.
- Report wall time separately from work counts.
- Preserve the existing approximately linear clean-build gate.

Acceptance:

- No-change updates perform zero declaration checks and code generations.
- Private body edit work remains constant as unrelated declarations grow.
- Interface-edit work is proportional to the affected dependency closure.
- Recorded results and reproduction commands are committed.

## Non-goals for Core 0.1

- New syntax, aliases, sugar, implicit conversions, or inference rules.
- Imports, packages, generics, traits, macros, async syntax, or FFI.
- Arbitrary threads, locks, channels, or automatic parallel execution.
- A new native backend.
- General equivalence checking or an optimizing reducer.
- Replacing the root-region runtime before lifetime planning is specified.

## Core 0.1 completion evidence

- `slim-conform differential` runs 22 reference fixtures, sends every declared
  parity fixture through the genuine self-hosted compiler, and reports every
  deferred capability with its manifest reason.
- The bootstrap remains byte-identical at stage 2 and stage 3 (112,008 emitted
  C bytes at completion).
- Stable declaration IDs, syntax/interface fingerprints, and deterministic
  call/type dependency edges are implemented by `IncrementalSession`.
- Tests prove private-body and interface-closure invalidation, data-layout
  invalidation, clean/incremental byte equality, source-span rebasing, and
  last-good-cache recovery after invalid edits.
- The committed incremental benchmark records exact declaration work for all
  five scenarios through 8,001 declarations. Whole-file indexing and output
  assembly remain visible wall-time costs, documented in
  `docs/INCREMENTAL.md`, rather than being presented as solved.
- `scripts/verify.sh` enforces all of the above together with governance,
  clean-build scaling, Clippy, tests, and sanitized native execution.

## Core 0.2: deterministic projects

Core 0.2 adds one qualified-reference rule and otherwise puts project structure
in a canonical manifest. The accepted model is specified in
`docs/PROJECTS.md`; D0017 through D0021 record its weighted choices and costs.

### Deliverable 1: canonical project resolution

- Parse one strict versioned project-manifest grammar.
- Resolve only explicit, confined, manifest-relative module paths.
- Validate module identity, direct imports, exports, entry point, and an
  acyclic graph.
- Resolve imported calls and named types only through `module/name`.
- Preserve file identity and exact byte spans in deterministic diagnostics.

Acceptance:

- Relocating a project does not change semantic identities or compiler output.
- Filesystem enumeration, environment, and manifest clause order cannot affect
  discovery.
- Missing files, duplicate IDs/paths, path escapes, cycles, visibility errors,
  and malformed manifests have stable codes and exact spans.
- Standalone and project inputs share the existing check/build/run/emit-c/fmt
  workflow; no ambient manifest discovery or alternate import syntax exists.

### Deliverable 2: canonical public interfaces

- Emit schema-1 text artifacts containing only exported type layouts and
  function signatures, modes, and effects.
- Fully qualify every named type and sort unordered semantic sets.
- Reject private or inaccessible types leaked through an export.
- Fingerprint exact canonical bytes without paths, timestamps, host data, or
  scheduling data.

Acceptance:

- Identical public interfaces produce byte-identical artifacts across clean
  processes and different absolute project locations.
- A private body edit preserves interface bytes and fingerprint.
- Every public signature or layout edit changes the interface fingerprint.
- Unknown schema versions and malformed artifacts are rejected, never guessed.

### Deliverable 3: genuine project incremental reuse

- Extend declaration IDs and call/type edges across modules.
- Retain per-declaration parsed, lowered, checked, interface, and generated
  results in a reusable project session.
- Persist validated module evidence keyed by source and dependency interfaces.
- Validate bounded cache fields and checksums and atomically replace entries.
- Rebuild safely after truncation, mutation, version mismatch, or identity
  mismatch.

Acceptance:

- A private implementation edit rechecks and regenerates exactly its changed
  declaration/module while consumers remain reused.
- An exported-interface edit invalidates exactly the reverse transitive module
  and declaration closure.
- Warm in-memory and persistent-cache output and diagnostics equal a clean
  compilation byte for byte.
- Cache corruption can never make an invalid project pass.

### Deliverable 4: bounded deterministic parallel checking

- Compute stable topological layers and check independent modules with owned
  worker state and immutable completed interfaces.
- Bound workers by requested jobs, hardware, and layer width.
- Prohibit worker-to-worker waits and shared mutable checker state.
- Merge all results in module-identity order and retain `--jobs 1` as oracle.

Acceptance:

- Worker-count and scheduling variance leave diagnostics, interfaces, caches,
  work counts, C, and native behavior byte-identical.
- Stress tests terminate under repeated wide and deep graphs.
- Default parallelism is enabled only when repeated measurements beat serial
  execution outside the recorded noise band.

### Deliverable 5: project conformance and performance evidence

- Add standalone positive, negative, runtime, relocation, determinism,
  malformed-input, and corruption fixtures.
- Classify self-host project capabilities explicitly with no silent stage-0
  fallback.
- Benchmark geometric wide/deep project graphs, clean and warm builds, private
  and interface edits, invalidation closure, worker counts, and remaining
  whole-project overhead.
- Integrate every project gate into `scripts/verify.sh`.

Acceptance:

- Governance maps each implemented project surface entry to conformance.
- Recorded work counts prove reuse instead of invalidation followed by clean
  recompilation.
- Clean project work remains approximately linear.
- Reproduction commands, machine context, timings, and honest limitations are
  committed.

### Core 0.2 completion evidence

- One explicit schema-1 manifest owns confined paths, direct imports, exports,
  and the sole entry module. `module/name` is the only cross-module reference
  form; no aliases, globs, ambient discovery, re-exports, or source-level
  import/export syntax were added.
- Canonical schema-1 interfaces are path-free and body-free. The strict reader
  rejects unknown versions, malformed structure, noncanonical ordering, and
  invalid modes/effects/types.
- `ProjectSession` performs real declaration reuse. Tests and conformance prove
  zero parse/lower/check/generation work on no-change updates, exactly one of
  each for private-body edits, reverse-closure invalidation for public layouts,
  clean-output equality, and last-good recovery after invalid edits.
- Schema-1 `SLIMCACHE` entries are bounded, checksummed, versioned, identity
  checked, atomically replaced, and exhaustively tested at every truncation and
  every single-byte mutation. Default CLI cache hits reproduce clean interfaces
  and modular C; any miss or rejected entry safely takes the clean path.
- The worker path uses owned state, topological layers, bounded hardware/job
  counts, no worker waits, canonical result merging, and a serial oracle.
  Worker-count tests compare valid output and invalid diagnostics. Measurements
  did not justify enabling parallelism by default.
- The standalone corpus contains 12 explicitly classified project fixtures,
  including native execution, exact diagnostics, formatting, relocation across
  clean compiler processes, cache corruption, incremental work, and job-count
  variance. Self-host support is explicitly `stage0-only(projects)`.
- The committed geometric benchmark covers wide/deep graphs through 65 modules
  and 129 declarations, every requested work count, persistent hits, closure
  size, and one/two/four-worker medians. Clean serial work is linear in recorded
  declaration counts. Remaining whole-project indexing/assembly and cache-file
  overhead are documented rather than claimed solved.
- `scripts/verify.sh` enforces formatting, dependency-free/unsafe-free
  governance, all tests, Core and project conformance, stage differential and
  the 112,008-byte self-host fixed point, standalone/project scaling, and
  sanitized native execution.

## Non-goals for Core 0.2

- Package registries, downloads, semantic version solving, or lockfiles.
- Import aliases, globs, selective symbol imports, re-exports, or implicit
  preludes.
- Cyclic modules, recursive interfaces, build scripts, or include files.
- Stable native ABIs, dynamic linking, foreign functions, or separate native
  object linking.
- Language-level concurrency or automatic parallel execution of SLIM code.

## Core 0.3: SLIM-first self-hosting

Core 0.3 changed the implementation default, not the language surface. New
compiler capability belongs in SLIM. During that milestone Rust stage 0
remained a frozen bootstrap seed and differential oracle under D0022; D0027
subsequently removed it from the active tree.

### Deliverable 1: structured data parity

- Emit records, variants, construction, projection, mutation, and exhaustive
  variant matching from the SLIM compiler.
- Replace parallel compiler arrays with named SLIM records as each capability
  becomes available.
- Promote conformance rows only after native behavior passes through the
  self-hosted compiler without fallback.

Acceptance:

- The compiler lexer stores one `Vec Token`, not synchronized kind/start/end
  vectors.
- The data fixture is classified `parity` and produces the exact expected
  native result.
- Stage 2 and stage 3 generated C remain byte-identical.

### Deliverable 2: complete storage and frontend parity

- Implement the remaining arena and vector-update calls in SLIM. (complete)
- Replace token-stream assumptions with structured syntax, type, effect, and
  ownership data implemented in SLIM.
- Produce stable structured diagnostics and recover from malformed input.

Acceptance:

- Storage and every current diagnostic fixture move from explicit deferral to
  differential parity.
- No invalid input panics either compiler.
- Diagnostic code, primary span, labels, notes, and fixes agree exactly.

### Deliverable 3: formatter and project parity

- Implement canonical formatting, manifest resolution, qualified references,
  public interfaces, and deterministic module assembly in SLIM.
- Keep the compiler in one module until it can compile project manifests; do
  not introduce a second temporary module system or Rust source bundler.
- Split the compiler into explicit SLIM modules only after that gate passes.

Acceptance:

- The self-hosted compiler compiles the project corpus with byte-identical
  observable artifacts and deterministic worker-count-independent results.
- The compiler builds itself as a multi-module SLIM project.
- Project capability never silently invokes stage 0.

### Deliverable 4: incremental self-hosting

- Implement declaration identities, fingerprints, dependencies, sessions, and
  validated caches in SLIM.
- Preserve clean/incremental equality and exact invalidation bounds.
- Freeze Rust production implementation once all conformance rows reach parity.

Acceptance:

- The SLIM compiler meets the existing standalone and project incremental work
  bounds.
- Bootstrap, differential conformance, governance, scaling, and sanitizer gates
  pass together.
- Production Rust remains within `design/rust-budget.tsv`; increases require a
  separate accepted architecture decision.

The completion architecture is fixed by D0023. The self-hosted compiler is
migrated from repeated token-position interpretation to one modular, typed,
declaration-oriented query pipeline. The external corpus is expanded before
parity is claimed so that every accepted project rule, not only the original
project fixtures, is tested. No Core syntax or primitive is added by this
milestone.

### Core 0.3 completion evidence

- The self-hosted compiler emits records, variants, `make`, `get`, `case`,
  variant `match`, and `set`.
- Its lexer uses a copy-safe `Token` record and one `Vec Token` in place of
  three parallel vectors.
- Storage operations and canonical formatting are also implemented in SLIM.
- The self-hosted checker now owns structured diagnostic values, malformed-form
  recovery, scalar/effect checking, Boolean exhaustiveness, and move/`inout`
  state. All 14 standalone diagnostic fixtures, including deterministic
  multi-diagnostic recovery, have exact code-and-primary-span parity.
- The first project corpus reached 33 exact parity fixtures, including project
  resolution, formatting, relocation, emission, visibility, cycle/schema
  diagnostics, and canonical path-free public interfaces.
- The D0023 completion audit expanded the external corpus from 35 to 52
  fixtures. All 52 pass stage 0 and have exact self-host parity. Manifest ordering,
  uniqueness, confinement, entry, imports, module identity, resolution, and
  public-interface closure are now implemented in SLIM and covered by
  differential parity. The larger denominator replaces the earlier claim that
  only session/cache work remained.
- The self-hosted compiler is now an explicit twelve-module acyclic project:
  checking, C generation, project handling, syntax/token utilities, byte-text
  emission, typed IR, query snapshots, incremental sessions, persistent cache,
  deterministic scheduling, coordination, and the executable driver have
  separate SLIM ownership boundaries. The former
  roughly 149 KB compiler module is a roughly 3.6 KB coordinator; every
  extraction preserved differential behavior and a stage-2/stage-3 fixed point.
- The first typed-query slice adds an explicit `ir` module with structured
  declaration kinds and declaration records. The checker now reads and lexes a
  standalone source exactly once, constructs one `Vec ir/Declaration`, and
  reuses that index across its top-level semantic passes. Governance rejects a
  return to per-phase file reads or lexing.
- Project checking now builds one typed `LoadedModule` index and one shared
  token arena. Identity, exports, entry-point rules, qualified-call resolution,
  and interface closure consume that index; the seven newly covered resolution
  and loading rules no longer reread module files in separate semantic passes.
- D0024 replaces trapping file input with one total `io.read-file` operation
  over an explicit unique byte vector. Missing modules now produce stable
  `E0409` diagnostics in SLIM, empty files remain distinguishable from failure,
  and failed reads preserve the caller's vector atomically.
- The first self-hosted session builds stable module/kind/name declaration
  snapshots with separate body and interface spans, derives exact qualified
  declaration edges from the typed project, compares two project states in one
  process, and reports exact parse/lower/check/generate work. Conformance proves
  zero work for no change, one declaration for a private body edit, and the
  exact two-declaration reverse closure for an exported interface edit; the
  result matches a repeated clean self-host oracle. Invalid updates are checked
  without constructing a replacement state, and recovery conformance proves
  that work is measured from the retained last-good snapshot.
- A schema-1 self-host cache stores an exact path-independent project input key
  and generated C behind bounded 64 MiB fields, exact-length decoding, and a
  mutation-sensitive checksum. Valid hits skip project checking and generation;
  missing, stale, truncated, structurally mutated, payload-mutated, and
  checksum-mutated entries rebuild through the clean SLIM path. Rust performs
  only the filesystem write/read orchestration required by D0023.
- The SLIM scheduler computes stable Kahn layers from explicit imports, exposes
  only prior-layer modules while selecting a batch, bounds requested width by
  module count and a four-worker implementation ceiling, and terminates with an
  invalid schedule on no progress. Core still launches no threads: each owned
  batch executes through the serial oracle and merges in manifest identity
  order. Widths one, two, four, and saturated input produce byte-identical C
  and diagnostics; the compiler's own bootstrap uses this path.
- The compiler bootstraps from the explicit
  `selfhost/slim.project`; stage 2 and stage 3 remain byte-identical without a
  Rust source bundler or stage-0 fallback.
- Governance classifies every Rust source file and rejects production growth
  beyond its recorded byte ceiling.
- The final combined gate passes formatting, warning-free Clippy, 61 Rust
  tests, governance, 52-fixture differential conformance with zero deferrals,
  the 592,704-byte stage-2/stage-3 fixed point, frontend and incremental work
  assertions, project scheduling/cache assertions, and ASan/UBSan native
  execution.
- On the recorded Apple M1 Max machine, the stage-2 compiler's median process
  time is 2.147 ms for the basic project, 322.838 ms for its own twelve-module
  project, and 7.796 ms for a validated warm self-cache hit (21 samples after
  warmup). This is evidence of the completed architecture, not attainment of
  the long-term millisecond-at-large-scale compilation goal. Full conditions
  and existing scaling results are recorded under `benchmarks/results/`.

## Toolchain cutover

Status: complete

D0027 makes the checked-in deterministic C11 seed the only bootstrap trust
artifact and the sixteen-module SLIM project the sole active semantic
compiler. A clean bootstrap uses no Cargo or `rustc`, reproduces successive C
generations byte for byte, and exposes the SLIM compiler through the root
`slimc` launcher. Rust remains only in three bootstrap-independent programs for
conformance orchestration, governance, and benchmark measurement.

Acceptance evidence:

- all 61 standalone and project fixtures execute through SLIM with no semantic
  fallback;
- 2,000 deterministic malformed-input mutations reject without a compiler
  trap;
- process-level check scaling stays within the 1.25 exponent gate;
- the generated seed is 1,141,206 bytes with SHA-256
  `aeef245413e5766d86ca84efdcea63f3b29694728c4b9a4cf774ce387bfe5572`;
- governance rejects active Rust semantic modules, production Rust budget
  entries, Cargo semantic targets, and Rust-dependent production launchers;
- sanitizer and deterministic allocation-failure gates cover both the compiler
  seed and generated native programs.

## Later milestones

### Core 0.4: lifetime and region planning

Status: complete

Use evidence from implementing the complete compiler in SLIM to add escape and
liveness analysis, stack promotion, compiler-inserted narrow regions,
deterministic resource destruction, and typed allocation failure. Add explicit
region surface only if inference evidence demonstrates a need.

Core 0.4 adds no source surface by default. D0025 selects bounded function
summaries and compiler-chosen nested regions: immutable `Bytes` remains a
copyable view whose backing region is proved, while vectors, arenas, and
aggregates containing them retain unique ownership. The implementation order is
the external lifetime corpus, typed plans, escape/liveness analysis,
deterministic region destruction, separately justified typed allocation
failure, incremental integration, and a self-hosted fixed-point freeze. The
working specification and baseline census are in `docs/MEMORY.md`.

The frozen implementation adds a thirteenth self-host module with typed value,
allocation, and destruction plans; bounds detailed per-function analysis to 64
values with a conservative fallback; stack-places region descriptors and
ordinary values; selects nested function regions; destroys dynamic blocks in
reverse order without a global allocation registry; and propagates allocation
exhaustion through D0026's typed `alloc`-effect channel. It adds no source
surface. Dynamic buffers remain region allocated, and per-binding earlier
release is future precision work rather than a hidden Core 0.4 claim.

The freeze evidence is recorded in
`benchmarks/results/2026-07-22-core-04.md`.

### Core 1 research

Develop typed optimization, bounded and total profiles, structured
deterministic concurrency, quality metrics, bounded equivalence, and
cost-directed reduction. Each research result must pass the same feature gates
before entering the language.

### Core 1A: direct typed reduction

Status: complete

D0028 keeps canonical parsed SLIM as the compiler's sole program
representation. The self-hosted `analysis` module attaches stable, bounded
type, ownership, scope, use, last-use, and dependency facts to that tree. The
`reduce` module applies a deliberately small set of individually justified
total rewrites directly to checked expressions. There is no `.sil` or `.slir`
input language, parser, formatter, serializer, or parallel type system.

The public research tools are `slimc analyze SOURCE` and
`slimc reduce SOURCE`. Reduced output is ordinary canonical SLIM, is
idempotent, and is accepted by the normal compiler. Analysis is a
versioned report rather than executable input. Reduction is limited to eight
closure passes and unused-binding searches inspect at most 64 tokens; analysis
stores at most 64 binding facts per function. Exceeding a bound produces a
conservative result rather than unbounded work.

Acceptance evidence:

- original and reduced fixtures have identical exit status, stdout, stderr,
  defined traps, and deterministic allocation-failure behavior;
- observable calls, allocation, I/O, checked traps, aggregate moves, mutation,
  and exclusive borrows are neither discarded nor reordered;
- exact normal forms, repeated-output determinism, idempotence, malformed-input
  diagnostics, and deep fallback are covered end to end;
- geometric 250/500/1,000/2,000-declaration measurements keep both tools below
  the 1.25 scaling-exponent gate without changing the ordinary check path;
- all 61 conformance fixtures and 2,000 malformed-input mutations pass,
  including 100 direct `reduce`/`analyze` probes; and
- the sixteen-module compiler reproduces the 1,141,206-byte portable seed at a
  byte-identical fixed point.

The frozen measurements and reproduction commands are recorded in
`benchmarks/results/2026-07-22-core-1a.md`.

### Core 1B: bounded semantics and quality evidence

Status: complete

D0029 adds no source-language feature. Analysis version 2 classifies exact,
bounded, and unknown facts for structural/effect/failure surface, representable
primitive state cardinality, ownership pressure, direct dependencies,
reduction opportunity, and a deliberately conservative totality result. It
does not infer application invariants, reachable-state ratios, asymptotic cost,
or one universal goodness score.

`prove-reduction` records at most 64 stable source-node rule justifications;
`verify-reduction` independently recomputes the unique eight-pass normal form.
`equivalent` is exact only for checked pure Boolean `subject` functions with at
most eight inputs and 256 expression tokens, returning the first canonical
counterexample on disagreement. `edit` consumes one versioned, non-executable,
64-token replacement addressed by canonical token identity, and the launcher
publishes it only after the normal checker accepts the candidate.

Acceptance evidence:

- the production implementation is four new SLIM modules—`quality`, `proof`,
  `equivalence`, and `edit`—with no Core surface or runtime ABI addition;
- exact/unknown state models, conservative totality, ownership pressure,
  proof determinism/replay, equivalence boundaries/counterexamples, and valid,
  malformed, non-executable, and type-invalid patches are tested end to end;
- all 62 conformance fixtures and 2,000 malformed mutations pass, with 100
  random probes through every bounded tool command;
- geometric 250/500/1,000/2,000-declaration measurements keep check, reduce,
  analysis, and proof discovery below the 1.25 process-scaling exponent gate;
- the matched SLIM/C/Rust agent fixture records lexical/model-token proxies,
  edit spans, diagnostics, acceptance, and feedback without claiming an LLM
  success rate; and
- the twenty-module compiler reproduces the 1,364,504-byte portable C seed at
  a byte-identical fixed point.

Frozen measurements, reproduction commands, and negative results are recorded
in `benchmarks/results/2026-07-22-core-1b.md`.

### Core 1C: real programs and a fast compiler loop

Status: complete

D0030 makes compiler and generated-program performance tests permanent release
infrastructure. Core 1C extends the matched challenge and agent corpora, stores
versioned budgets, and makes any budget relaxation require a scored architecture
decision with quantified evidence. Portable gates use exact work,
determinism, geometric exponents, and same-host ratios; dated results retain
absolute medians for longitudinal tracking.

The first measured targets are repeated declaration lookup in standalone C
generation, declaration-name rescanning during project flattening, whole-input
incremental reconstruction, and output assembly. Production changes remain in
SLIM and add no syntax, type, effect, built-in, runtime ABI, dependency, or
second program representation. The compiler may build a bounded derived name
index from checked canonical source; the index is not executable or trusted
across compilations.

Acceptance requires both check and C-emission scaling exponents at or below
1.25, a fivefold improvement over Core 1B's recorded 2,000-declaration emission
time on the baseline host, an emit/check ratio no greater than four, exact
incremental work with materially lower large-edit wall time, matched challenge
outputs, independently guarded native runtime ratios, the full release gate,
and a byte-identical self-host fixed point.

Acceptance evidence:

- full 1,000/2,000/4,000/8,000 declaration runs measure check and C-emission
  exponents of 0.91 and 0.94, below the permanent 1.25 limits;
- 2,000-declaration emission is 8.9 times faster than the frozen Core 1B
  result, and the full 8,000-declaration process falls from 4.17 s to 0.13 s;
- peak resident memory on that same 8,000-declaration input falls from 33.52
  MB to 32.70 MB;
- 65-module wide and deep sessions take roughly 8--9 ms while retaining exact
  zero-declaration no-change work and one-declaration private/interface work;
- twelve manifest-driven SLIM/C/Rust challenges produce identical output and
  pass independent native-runtime budgets; five matched agent repair cases
  enforce rejection before repair and acceptance afterward;
- governance prevents removal of the corpus minima, coverage dimensions,
  release commands, required budgets, or atomic bootstrap installation; and
- the complete release gate passes and the 1,388,660-byte self-host seed
  reproduces byte for byte.

Frozen measurements, profiler findings, reproduction commands, and negative
results are recorded in `benchmarks/results/2026-07-22-core-1c.md`.

### Core 1D: complete typed compiler view

Status: in progress

D0031 makes one derived typed view authoritative for expression semantics, and
D0032 gives record construction and variant matching one canonical declared
order. The first slice is implemented in SLIM: it validates all declared type
forms and nested expressions, exact call and recursive-transfer signatures,
record and variant members, assignments, matches, and the executable boundary.
Standalone checking and flattened project checking use the same analyzer. The
stronger boundary passes 86 external fixtures, 2,000 deterministic malformed
mutations, all matched challenge programs, and a byte-identical self-host
bootstrap.

The second slice retains a checked typed artifact through standalone emission,
tags lexical uses with scoped type links, and reuses declaration links in the
effect and ownership passes. Removing three repeated whole-program call
resolution scans reduced the 468,568-byte self-host project check from a
3.91-second median to 0.20 seconds on the recorded host. Permanent full-series
measurements produce 0.916 and 0.980 check-scaling exponents for user-call-heavy
declarations and deeply nested lexical bindings respectively. The measurements
and honest boundary are recorded in
`benchmarks/results/2026-07-23-core-1d-progress.md`.

The third slice introduces D0033's prepared project artifact. Project checking,
scheduling, ordinary emission, and persistent-cache misses now share one loaded,
flattened, lexed, and checked representation. One origin record per flattened
token projects structured type issues back to the source module and exact local
byte span; `project-type-error` pins `E0344@app@56:60`.

The fourth slice adopts D0034's token-interval issue model. One-token type
issues and whole-expression diagnostics now have the same compact structured
representation, and both standalone and project reporters render from its two
endpoints. The compiler remains at a 0.22-second self-check on the recorded
host. A first recursive legacy-collector design was rejected after increasing
that check beyond 40 seconds; migration will continue only through smaller
measured slices.

The fifth slice establishes D0035's finalized issue stream on `Checked`.
Analyzer issues and later semantic issues now cross one explicit phase boundary,
and prepared projects consume that final stream rather than the analyzer's
snapshot. Missing-effect `E0343`, temporary-inout `E0348`, and aliased-inout
`E0349` are the first migrated legacy families. Three project fixtures pin
their exact module-local source ranges. The self-check remains about 0.22
seconds, the byte-identical seed is 1,653,846 C bytes, and the corpus contains
89 fixtures plus 2,000 malformed mutations.

The sixth slice adopts D0036's checked-emission precondition. Ordinary code
generation consumes declaration links retained on the checked token stream and
no longer rebuilds them. The fixed-point seed shrank by 247 C bytes. A quick
before/after series was noisy, so no runtime or latency improvement is claimed;
the durable result is removal of one unconditional whole-token analysis pass.

The seventh slice fixes a typed/backend mismatch under D0037. Variant matches
now decode the scrutinee binding's retained type link instead of scanning only
function parameters. `variant-local-match` proves a variant can flow through a
lexical `let` and still compile and run. The obsolete scan was removed, shrinking
the fixed-point seed to 1,650,892 C bytes; all 90 fixtures and 2,000 malformed
mutations pass at the approximately 0.22-second self-check boundary.

The eighth slice adopts D0038's aggregate-construction links. Record emission
uses the checked record declaration link instead of scanning all declarations,
and canonical field checking publishes each validated field definition on the
field form. The `record-wide` fixture exercises sixteen ordered fields. Direct
field-link and lockstep backend consumers were reverted after crossing the
five-second self-check guard, so the compatibility field scan remains an
explicit Core 1D blocker. The fixed-point seed is 1,647,978 C bytes and all 91
fixtures plus 2,000 malformed mutations pass.

The ninth slice adopts D0039's variant-construction links as a bootstrap schema
migration. Variant construction and matching consume retained declaration
links, while the checker publishes each validated constructor and match-arm
case definition on its structural opening token. `variant-wide` exercises all
sixteen ordered arms and prints `42`. The producer checkpoint deliberately
keeps the old case-name reader for one generation; its byte-identical seed is
1,645,415 C bytes, with 92 fixtures and 2,000 malformed mutations passing.

The tenth slice completes that migration under D0040. Constructors and match
arms consume their structural case links directly, removing the recursive
case-name scanner and making variant member lookup independent of case count.
The producer seed builds the consumer normally, self-validation remains about
0.22 seconds, and the fixed-point seed shrinks to 1,643,001 C bytes.

The eleventh slice adopts D0041's typed recursive-inout diagnostics. The
existing typed argument traversal checks `recur` identity and appends every
`E0350` interval; four legacy reporting functions and their whole-body walk are
removed. A project fixture pins two module-local intervals, self-validation
stays near 0.22 seconds, and the fixed-point seed shrinks to 1,634,840 C bytes;
93 fixtures and 2,000 malformed mutations pass.

The twelfth slice adopts D0042's checked memory type links. Standalone checking
establishes the typed view before planning, and recursive record/variant storage
classification consumes retained declaration links instead of scanning all
items by name. The fixed-point seed shrinks to 1,631,678 C bytes without a new
plan field, pass, or source feature. Nine warm self-checks have a 0.11-second
median user time, about twice as fast as the preceding 0.22-second boundary.

The thirteenth slice adopts D0043's retained memory plan. `typing/Checked` and
`PreparedProject` now carry the one plan already produced during checking, and
ordinary standalone and project emission require that plan. Function plans are
consumed in declaration order with an identity check, and region selection no
longer has a backend-owned inference path. The self-check remains near 0.11
seconds and the byte-identical seed is 1,635,534 C bytes.

The fourteenth slice adopts D0044's structured nonexhaustive diagnostics. The
existing bounded Boolean-match finder appends `E0336` with the complete match
interval instead of printing flattened bytes. A project fixture pins
`E0336@app@56:77`; standalone multiple-diagnostic ordering is unchanged. The
self-check remains near 0.11 seconds, the corpus grows to 94 fixtures, and the
byte-identical seed is 1,635,539 C bytes.

The fifteenth slice adopts D0046's bounded record member lookup. Record
construction consumes the checked field-definition link through a constant-time
structural verifier, and the final recursive textual aggregate-member scanner
is removed. The sixteen-field `record-wide` fixture remains executable and the
self-check remains near 0.11 seconds. The byte-identical fixed-point seed is
1,635,270 C bytes.

This is not yet a completed milestone. Code generation still rediscovers some
semantic facts from tokens. Allocation, partial, and I/O
effect failures and exclusive-borrow call rules now use structured project
source projection, while legacy move and secondary Boolean-recovery reports
have not yet joined that channel. Core 1D
acceptance requires bounded expression-fact lookup, removal of the remaining
redundant ordinary-generation analysis, complete project
diagnostic projection, adversarial deep/wide tests, geometric scaling and
same-host regression budgets, allocation-failure and sanitizer coverage, and
the complete release/bootstrap gate.
