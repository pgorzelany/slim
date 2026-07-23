# D0041: Typed recursive-inout diagnostics

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

Recursive transfer of an `inout` parameter must preserve the exact binding.
The legacy checker found violations in a second whole-body traversal and printed
`E0350` immediately. As a result, standalone checking could report the error,
but prepared projects could not project it through the finalized typed issue
stream to the originating module and byte interval.

The existing typed call-argument traversal already visits every recursive
argument beside its parameter. A non-recursive leaf now checks binding identity
only when the callee token is `recur` and the parameter is `inout`. It appends
each offending argument token to the structured issue vector. The legacy recur
body, declaration, and reporting passes are removed.

## Alternatives

Threading the issue vector through a second recursive identity walker was tested
twice and made self-validation exceed the five-second early-abort guard. Token
sentinels followed by another whole-token scan would preserve scaling but add a
side channel and duplicate traversal. Reporting only the first mismatch would
lose the established two-diagnostic behavior.

Extending the traversal that already owns argument, parameter, and issue state
adds one leaf query and no pass. `E0350` uses the unconditional interval append
operation because every mismatched recursive argument is independently useful.

## Costs

Once one recursive identity issue is found, ordinary expression type recovery
for later arguments is intentionally curtailed by the existing typed checker.
Identity comparison still runs first for every remaining argument, preserving
all `E0350` intervals. This affects diagnostics for an already-invalid recur
form, not accepted-program semantics or generated code.

The legacy move diagnostics remain outside the finalized stream and are not
claimed by this decision.

## Evidence

The standalone `recur-rebind` fixture retains both exact diagnostics:
`E0350@114:119` and `E0350@120:124`. The new project fixture projects the same
two violations to `E0350@app@99:104` and `E0350@app@105:109`.

Self-validation remains about 0.22 seconds of user CPU time. Structured issue
rendering reports every interval but normalizes the process failure status to
one. The byte-identical fixed-point seed shrank from 1,643,001 to 1,634,840
generated C bytes. The full
93-fixture conformance corpus and 2,000 malformed-input mutations pass, and
governance rejects
direct `E0350` printing or restoration of the legacy recur reporters.

## Removal

Replace this leaf only with another check in the same authoritative typed
argument traversal that preserves every recursive-inout interval. A separate
body scan or direct diagnostic printer requires a new accepted decision and
same-host scaling evidence.
