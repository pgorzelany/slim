# D0034: Token-interval diagnostics

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 60

## Need

A structured compiler issue originally named one token. That was sufficient for
unknown names and type mismatches, but effect, alias, and exhaustiveness errors
can cover a whole expression. Project source projection must not shorten those
diagnostics merely because flattened byte offsets are not source coordinates.

Every structured issue therefore stores an inclusive start-token and end-token
index. A one-token issue sets both fields to the same index. Standalone
rendering obtains the first token's start byte and the last token's end byte;
project rendering applies the same rule through the prepared artifact's origin
table. One representation now preserves both point and range diagnostics.

## Alternatives

Storing flattened byte offsets would make standalone rendering easy but would
reintroduce ambiguous project coordinates. Storing module names and byte spans
inside every issue would couple semantic analysis to the project loader and
duplicate origin data. Keeping only one token would force broad diagnostics to
lose information.

Token intervals remain compact, deterministic, and independent of whether the
checked input is a standalone module or a prepared project.

## Costs

An issue grows by one integer. Consumers must use the interval endpoints rather
than assume one token, and an issue must not span different source modules.
That invariant follows from expressions being contained by one module and will
be enforced when all legacy semantic reports enter this channel.

A trial that threaded an additional mutable issue vector through the recursive
effect, recursion, and ownership walkers was rejected: the current compiler
took more than 40 seconds to validate its own source instead of about 0.22
seconds. The interval model is retained, but collector migration must proceed in
smaller measured slices without accepting that source-shape regression.

## Evidence

The self-hosted compiler bootstraps to a byte-identical 1,652,703-byte C fixed
point with token intervals. All 86 conformance fixtures and 2,000 deterministic
malformed-input mutations pass. The existing `project-type-error` fixture still
pins the exact `E0344@app@56:60` module-local range. Governance requires the
interval fields, the shared append operation, and both standalone and project
interval consumers.

## Removal

Replace token intervals only with a representation that preserves exact
standalone and module-local ranges, remains independent of project loading,
uses linear bounded storage, and passes the same bootstrap, conformance, and
compile-time gates.

D0035 defines the finalized checked issue stream and migrates missing-effect
diagnostics as the first interval-producing legacy family.
