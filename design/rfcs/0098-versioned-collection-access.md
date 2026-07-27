# RFC-0098: Versioned collection access

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

Even with an exact nonnegative index interval, removing a vector bounds check
from that fact alone is unsound because the vector's current length may be
shorter than the interval maximum. Rechecking the actual index on every stencil
load leaves a large runtime gap. Proving every vector's construction history
would require a substantially larger resource domain.

For an atom index with an exact checked lower bound of at least zero and a
checked upper bound, C lowering now emits a versioned index:

`vector.len > upper ? index : checked_index(vector, index)`

The fast path is safe for every value in the interval. The fallback performs
the original check against the actual index, so a shorter vector retains
exactly the prior success or trap behavior rather than acquiring a stronger
precondition. Non-atoms, missing bounds, and possibly negative indices use only
the original checked operation. Both `vec.get` and `vec.set` use the same
lowering.

## Alternatives

Deleting the check or emitting one unconditional `len > upper` guard would
weaken safety or introduce traps for valid smaller indices. Requiring a new
source precondition or unchecked collection operation would expand the language
surface. A complete vector-length effect domain remains valuable future work,
but is not required for a behavior-preserving fast path.

## Costs

Eligible accesses add one length comparison and a cold checked fallback. Native
optimizers may hoist or merge the invariant comparison; code that lacks a
positive interval fact retains its old C exactly. Analysis work is unchanged
and emission adds constant work per collection access. The runtime ABI and
source language are unchanged.

Tests must cover fast and fallback success, fallback traps, negative and
unknown non-applicability, get and set, exact C determinism, retained evaluation
order for atom arguments, vector bounds checks in generated code, and the full
trap corpus.

## Evidence

On the recorded host, one hundred Game of Life processes used about 0.55
seconds of user CPU with ordinary checks. A diagnostic versioned lowering for
the nine neighbor loads used about 0.36 seconds while producing identical
output and retaining every checked fallback. The improvement is independent of
whether a later backend represents the version as a branch, loop version, or
guarded machine block.

Production retention requires a material same-host application gain, no
aggregate regression, and positive fallback and trap tests.

The retained production lowering passes fast-path, short-vector fallback,
fallback-trap, get, set, negative, and unknown non-applicability tests. The
full twenty-application comparison measures Game of Life at 4.715 ms versus
7.059 ms for C and 5.043 ms for Rust. Across the complete corpus SLIM/C is
1.165 and SLIM/Rust is 1.026 by geometric mean, so this named gain does not hide
the remaining aggregate gap.

## Removal

Remove the fast path if its interval is not attached to the exact index node,
if an effectful argument can be evaluated more than once, if a short vector can
bypass its actual-index check, if trap behavior changes, or if the native
corpus does not reproduce the gain.
