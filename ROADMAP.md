# SLIM Roadmap

Status: Core 0.3 active
Current milestone: Core 0.3 SLIM-first self-hosting
Last updated: 2026-07-21

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
- The standalone corpus contains 11 explicitly classified project fixtures,
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

Core 0.3 changes the implementation default, not the language surface. New
compiler capability belongs in SLIM. Rust stage 0 remains a frozen bootstrap
seed and differential oracle, with production byte ceilings enforced by
governance under D0022.

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

### Core 0.3 first-slice evidence

- The self-hosted compiler emits records, variants, `make`, `get`, `case`,
  variant `match`, and `set`.
- Its lexer uses a copy-safe `Token` record and one `Vec Token` in place of
  three parallel vectors.
- Storage operations and canonical formatting are also implemented in SLIM;
  the differential corpus now has nine exact parity fixtures.
- Governance classifies every Rust source file and rejects production growth
  beyond its recorded byte ceiling.

## Later milestones

### Core 0.4: lifetime and region planning

Use evidence from implementing the complete compiler in SLIM to add escape and
liveness analysis, stack promotion, compiler-inserted narrow regions,
deterministic resource destruction, and typed allocation failure. Add explicit
region surface only if inference evidence demonstrates a need.

### Core 1 research

Develop typed optimization, bounded and total profiles, structured
deterministic concurrency, quality metrics, bounded equivalence, and
cost-directed reduction. Each research result must pass the same feature gates
before entering the language.
