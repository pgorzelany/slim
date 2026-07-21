# Self-host bootstrap

`slim.project` is the self-hosting input. It contains explicit modules for
syntax/token utilities, byte-text emission, checking, project handling, C
generation, coordination, and the minimal executable driver. Stage 0 compiles
the project to the stage-1 executable. Stage 1 emits the C used to build stage
2, and stage 2 emits the C for stage 3. The bootstrap succeeds only when the
stage-2 and stage-3 C files are byte-for-byte identical and a program compiled
by the self-hosted compiler runs successfully.

The implementation is deliberately developed in valid Core rather than through
a privileged bootstrap dialect.

The self-hosted path accepts the subset needed to compile itself plus the Core
standalone corpus: records, variants, `make`, `get`, `case`, variant `match`,
`set`, vector update, arenas, canonical formatting, and structured checking.
Its lexer dogfoods this support with a `Token` record and a single `Vec Token`.
The checker diagnoses malformed forms, scalar/effect errors, non-exhaustive or
duplicate Boolean arms, use-after-move, and invalid `inout`/`recur` ownership.
The compiler also resolves canonical project manifests, reads explicit
manifest-relative modules, lowers qualified references into one deterministic
translation unit, and checks the original project visibility/cycle/schema
corpus. It emits canonical path-free public interface artifacts. D0023 expands
the differential corpus to the complete accepted project ledger; stage 0
remains the oracle for the explicitly classified manifest, loading,
resolution, interface, incremental-session, and cache cases while those
capabilities move into SLIM.

New compiler capability is implemented in this SLIM compiler by default. The
production Rust ceilings in `design/rust-budget.tsv` prevent unnoticed stage-0
growth while still allowing Rust bootstrap, conformance, benchmark, and
governance infrastructure.

Run the complete proof with:

    cargo run --bin slim-bootstrap

Generated stages and C files are placed in build/selfhost/, which is ignored by
Git.
