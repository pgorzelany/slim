# Self-host bootstrap

slimc.slim is the compiler implementation written in canonical SLIM Core.
Stage 0 compiles it to the stage-1 executable. Stage 1 emits the C used to build
stage 2, and stage 2 emits the C for stage 3. The bootstrap succeeds only when
the stage-2 and stage-3 C files are byte-for-byte identical and a program
compiled by the self-hosted compiler runs successfully.

The implementation is deliberately developed in valid Core rather than through
a privileged bootstrap dialect.

The self-hosted path intentionally accepts the subset needed to compile itself:
functions, scalar and byte/vector types, explicit let, calls, Boolean match,
and tail recur. Stage 0 remains the complete Core 0 implementation, including
records and variants, and is the diagnostic reference compiler.

Run the complete proof with:

    cargo run --bin slim-bootstrap

Generated stages and C files are placed in build/selfhost/, which is ignored by
Git.
