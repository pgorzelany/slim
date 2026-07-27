# RFC-0024: Total file input

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: total-file-read
Safety: 2
Compile: 0
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 50

## Need

Robust compiler diagnostics require missing, unreadable, malformed-path, and
short-read failures to be ordinary typed control flow. The original
`io.read-file` returned `Bytes` and terminated the process on every transport
failure, so a self-hosted compiler could not produce the accepted `E0409`
missing-module diagnostic.

`io.read-file` is replaced in place with one total signature. It takes a path
and an `inout (Vec U8)`, appends the complete file on success, returns `true`,
and leaves the vector unchanged while returning `false` on transport failure.
Allocation exhaustion remains a runtime trap. The operation has `alloc` and
`io` effects and no `partial` effect.

## Alternatives

A second probing primitive would create two filesystem operations, introduce a
time-of-check/time-of-use race, and still fail to make reading total. Returning
empty bytes would conflate a valid empty file with failure. A negative-length
or null-byte sentinel would create a hidden invalid `Bytes` state and weaken
memory safety. A new result type would add permanent type surface when existing
Boolean control flow and unique byte vectors already express the operation.

## Costs

Every caller must allocate an explicit byte vector, inspect the Boolean result,
and freeze the vector when immutable bytes are required. The runtime performs
one additional success branch; it retains the same file read and allocation
work. This is a source-breaking replacement, and the trapping signature is
removed rather than retained as an alias.

## Evidence

Conformance distinguishes successful empty input from failure, verifies that a
failed read leaves an existing vector unchanged, and runs storage through both
compiler generations. Project differential conformance must report `E0409` for
a missing module without stderr output or a runtime trap. Bootstrap fixed-point,
governance, sanitizers, and runtime tests cover the replacement implementation.

## Removal

Replace this operation only with a smaller single total operation that
preserves explicit allocation and I/O effects, unambiguous failure, atomic
output mutation, and missing-source compiler diagnostics. Do not restore a
trapping alias or a separate existence probe.
