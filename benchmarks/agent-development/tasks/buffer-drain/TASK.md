# Reusable buffer drain and ownership repair

Repair the four-module buffer pipeline. Edit only its five supplied source/project
files and preserve their module boundaries and public signatures below.

`storage.Buffer` has exactly one field `values: Vec[I64]`.
`storage.make(first: I64, second: I64) -> Buffer effects[alloc]` owns a new vector
with these two values. `storage.append(buffer: @Buffer, value: I64) -> I64
effects[alloc]` appends and returns the new length.
`storage.drain(buffer: @Buffer) -> Vec[I64] effects[alloc]` returns the original
owned vector and installs a fresh empty vector in the actual buffer. Use the
canonical checked affine-field replacement operation; no shared-field move,
copy, retained alias or alternative buffer representation is permitted.

`pipeline.round(buffer: @storage.Buffer, value: I64) -> Vec[I64]
effects[alloc, partial]` first reads the current length, appends `length + value`,
then drains. Compute the scalar before reserving the buffer for mutation; keep
left-to-right evaluation and the shared/exclusive/owned public modes exact.
`archive.checksum(values: ^Vec[I64]) -> I64 effects[partial]` consumes its owned
vector and returns its sum. Inputs have at most 32 elements, values in [-100,100].

The application starts with [5,7], runs a round with 3 and prints checksum 17.
It reuses the now-empty buffer for a round with -1 and prints checksum -1.
Each number gets its own newline; return zero. Update every caller and verify
the complete project. Subsequent rounds and shared readers must observe an empty
reusable buffer after each drain. Do not weaken effects, suppress checking,
change ownership modes or manufacture the result instead of draining storage.
