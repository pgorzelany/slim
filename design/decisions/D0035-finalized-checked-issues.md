# D0035: Finalized checked issue stream

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 70

## Need

Type analysis produces an initial issue vector inside its view, while later
effect and ownership passes can discover additional errors. Extracting that
vector and appending to it does not mutate the already-built view's vector
length. Rebuilding the view after later passes was also a poor fit: it made the
current compiler take more than 30 seconds to validate its own source instead
of about 0.22 seconds.

`Checked` now retains the finalized issue stream alongside the typed view. The
stream begins as the analyzer's issues and later passes append to that owned
buffer. Standalone reporting, prepared-project source projection, and retained
project artifacts consume `Checked.issues`. The typed view remains semantic
evidence; the checked artifact is the authority for the completed diagnostic
phase.

## Alternatives

Mutating a vector extracted from the view silently failed to update the
retained field. Reconstructing the view worked conceptually but crossed a
measured source-shape performance cliff. Letting each pass print directly keeps
the ownership simple but prevents deferred project projection and machine
consumption.

A finalized stream on `Checked` expresses the phase boundary without adding a
source-language feature, runtime support, or another compiler pass.

## Costs

During checking, the typed view and checked artifact each carry a vector header
for the initial issues. The issue elements are small and errors normally stop
later semantic passes. Consumers must read diagnostics from `Checked.issues`,
not from the analyzer snapshot in `View.issues`.

The first migrated families are missing-effect `E0343`, temporary-inout
`E0348`, and aliased-inout `E0349`. Small leaf helpers append a whole-form or
one-token interval, keeping construction out of boundary-sized coordinator
functions. Recursive ownership collectors remain direct until they can migrate
without threading an additional mutable vector through their hot recursive
shapes.

## Evidence

`project-effect-error` pins `E0343@app@72:94`, `project-inout-temporary` pins
`E0348@app@164:178`, and `project-inout-alias` pins `E0349@app@211:217`.
Their standalone fixtures retain exact prior output. The self-host check remains
about 0.22 seconds on the recorded host. The compiler reaches a byte-identical
1,653,846-byte C fixed point, and all 89 conformance fixtures plus 2,000
deterministic malformed-input mutations pass.

Governance requires `Checked.issues`, project consumption of that field, and
structured `E0343`, `E0348`, and `E0349` production. It rejects restoration of
direct printing for those codes.

## Removal

Replace the finalized stream only with a single owned diagnostic representation
that supports all compiler phases, deferred exact project projection,
deterministic ordering, bounded storage, the same compile-time envelope, and
the full bootstrap and conformance evidence.
