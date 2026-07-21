# Self-host bootstrap

slimc.slim is the compiler implementation written in canonical SLIM Core.
Stage 0 compiles it to the stage-1 executable. Stage 1 emits the C used to build
stage 2, and stage 2 emits the C for stage 3. The bootstrap succeeds only when
the stage-2 and stage-3 C files are byte-for-byte identical and a program
compiled by the self-hosted compiler runs successfully.

The implementation is deliberately developed in valid Core rather than through
a privileged bootstrap dialect.

The self-hosted path accepts the subset needed to compile itself plus the first
Core 0.3 parity slice: records, variants, `make`, `get`, `case`, variant
`match`, `set`, vector update, arenas, and canonical formatting. Its lexer
dogfoods this support with a `Token` record and a single `Vec Token`. Stage 0
remains the complete Core 0 implementation and the diagnostic reference
compiler.

New compiler capability is implemented in this SLIM compiler by default. The
production Rust ceilings in `design/rust-budget.tsv` prevent unnoticed stage-0
growth while still allowing Rust bootstrap, conformance, benchmark, and
governance infrastructure.

Run the complete proof with:

    cargo run --bin slim-bootstrap

Generated stages and C files are placed in build/selfhost/, which is ignored by
Git.
