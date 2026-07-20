# Self-host bootstrap

`slimc.slim` is the compiler implementation written in canonical SLIM Core.
Stage 0 compiles it to the stage-1 executable. The bootstrap harness then asks
stage 1 to compile the same source, builds stage 2, and compares stage-1 and
stage-2 compiler output at the fixed point.

The implementation is deliberately developed in valid Core rather than through
a privileged bootstrap dialect.

