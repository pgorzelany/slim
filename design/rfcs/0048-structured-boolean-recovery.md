# RFC-0048: Structured Boolean recovery

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 1
Compile: 0
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 50

## Need

RFC-0044 moved the primary nonexhaustive `E0336` interval into the finalized issue
stream, but recovery inside that invalid match still printed unknown-name,
duplicate-arm, and arm-type diagnostics directly. Standalone programs received
useful ordered feedback; prepared projects could retain only the first error.

The existing recovery walk now appends `E0314`, `E0335`, and `E0344` token
intervals to the same vector immediately after `E0336`. Rendering happens once
after collection. Deferred project checking runs the identical collector and
projects every issue through the existing token-origin table.

## Alternatives

Moving one recovery code at a time would split one source-ordered diagnostic
sequence across structured and rendered channels, changing the stable order.
Parsing rendered diagnostics in the project layer or rechecking each module
would create a second protocol and analysis path.

The all-family migration was previously deferred because larger recursive
issue-vector experiments crossed the compiler performance guard. This bounded
collector was measured directly and remains at the 0.11-second self-check
boundary, so no partial compatibility path is needed.

## Costs

Invalid Boolean matches allocate up to one issue for each discovered recovery
error before rendering. Valid programs do not enter this path. The recovery
rules remain intentionally shallow: they recognize scalar arm results,
parameters, and prior lexical `let` names rather than pretending to run the
complete type analyzer on an already invalid match.

No syntax, diagnostic code, accepted program, runtime state, or output order is
changed.

## Evidence

The standalone `multiple` fixture remains exactly `E0336`, `E0314`, `E0335`,
then `E0344` with its prior byte intervals. The new prepared-project fixture
projects the same sequence as `E0336@app@56:96`, `E0314@app@74:81`,
`E0335@app@83:95`, and `E0344@app@89:94`.

Self-validation remains about 0.11 seconds. The complete gate passes 96
fixtures, 2,000 deterministic malformed-input mutations, all performance and
safety checks, and a byte-identical fixed point of 1,631,074 generated C
bytes. Governance requires all three structured producers and rejects the
superseded direct recovery walkers.

## Removal

Replace this collector only with a more complete structured recovery analysis
that preserves code identity, intervals, source order, standalone output, and
project projection. Do not restore rendered diagnostic transport between
compiler phases.
