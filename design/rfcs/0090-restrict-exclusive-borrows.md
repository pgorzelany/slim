# RFC-0090: Restrict-qualified exclusive borrows

Status: rejected
Implementation: not-planned
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 100

## Need

SLIM already proves that every `inout` argument is a non-escaping exclusive
borrow and rejects two live exclusive arguments for the same binding with
`E0349`. Generated C currently erases that fact to an ordinary pointer, so the
native optimizer must conservatively assume that borrowed vectors and records
may alias. This particularly limits loop optimization when a function reads one
borrowed collection while writing another.

Emit C11 `restrict` on every generated `inout` parameter. This is a lowering of
the existing ownership proof, not a new promise made by source authors. Owned
values, scalar parameters, the region parameter, runtime entry points, and the
SLIM runtime ABI remain unchanged.

## Alternatives

Adding source annotations would duplicate the mandatory meaning of `inout` and
create an unsafe promise that users or agents could state incorrectly.
Whole-program alias analysis would repeat an exact checker result at greater
compile cost. Retaining ordinary pointers leaves useful safety information
unavailable to the C optimizer.

## Costs

Parameter emission appends one fixed token for an already-classified borrow.
There is no new pass, dependency, syntax, runtime branch, allocation, or
representation. The generated C contract is valid only while `inout` remains
exclusive and non-escaping; the existing alias and escape diagnostics are
therefore permanent prerequisites.

Positive generation tests require `restrict` on borrowed collection and record
parameters. Scalar and owned parameters must remain unqualified. Existing
`E0348` temporary-borrow and `E0349` alias fixtures remain negative authority.
Bootstrap, sanitizers, C compilation, and the expanded native comparison remain
mandatory.

## Evidence

The RFC-0087 full result is the baseline: 1.273 geometric mean against C and 1.337
on serial work. Retention requires a same-host improvement or a clear
application-level win without a material aggregate regression.

The candidate reached the self-hosted fixed point, emitted valid C11, and
preserved the existing alias diagnostics. Its 2026-07-24 quick comparison
measured 1.276 against C overall and 1.357 on serial work, versus the retained
RFC-0087 quick result of 1.263 and 1.320. Game of Life remained effectively
unchanged. The production qualifier and provisional generation assertion were
removed because no measured benefit justified changing the generated C
contract.

## Removal

Remove the qualifier if any accepted SLIM program can access one object through
two independently based restricted pointers, if a supported C11 compiler
rejects the generated declaration, or if measured native code does not benefit.
Do not replace it with a user-written alias promise.
