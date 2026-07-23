# D0063: Bounded integer totality facts

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

D0062 correctly treats every checked arithmetic operation as a possible trap.
That is safe but too coarse for useful deterministic-parallelism evidence:
guarded increments, literal arithmetic, a nonzero division, and a checked byte
conversion remain indistinguishable from overflow, division by zero, and an
invalid conversion. Empty effect lists cannot supply the missing proof.

Analysis version 4 therefore derives sound integer interval and operation
totality facts from the already checked tokens and types. Quality and
parallelism consume the same facts. A checked operation is removable from the
trap hazard set only when its exact source node has a positive totality proof.

## Alternatives

Unbounded symbolic arithmetic, an SMT solver, or whole-program theorem prover
would increase dependencies, latency, and semantic complexity before SLIM has
evidence that it needs them. Runtime speculation cannot preserve trap order.
Treating small literals as globally unchecked would be unsound after their
values flow through parameters or branches. User-written range annotations
would add permanent syntax before inference has been tested on applications.

The accepted slice is compiler evidence only. It adds no language form,
built-in, runtime ABI, dependency, optimizer, or scheduler.

## Costs

The abstract domain records optional lower and upper I64 bounds inside
`-1,000,000,000..1,000,000,000`. Values outside that proof domain remain
unknown even when native I64 arithmetic would be safe. The narrower domain
ensures the analyzer's own endpoint arithmetic cannot overflow.

At most 64 persistent comparison refinements are retained. Saturation is
reported and subsequent branches lose precision rather than safety. At most 64
checked sites are printed, though every checked site is analyzed. Facts are
indexed by existing token identities and refinement lookup is bounded, so work
remains linear in checked token count with a fixed constant.

The first transfer set covers exact literals, immutable `let` flow, one-sided
`lt`/`le`/`gt`/`ge` branch refinement, addition and subtraction by an exact
constant, bounded multiplication, nonzero bounded division and remainder, and
checked I64-to-U8 conversion. Mutation, collection bounds, user-call return
ranges, out-of-domain values, and missing typed facts remain unknown.

## Evidence

Acceptance requires deterministic balanced reports that distinguish guarded
from unguarded arithmetic, prove exact nested arithmetic, nonzero division, and
valid conversion, preserve division-by-zero and domain-limit hazards, cross the
64-refinement and 64-reported-site boundaries, and cause the parallelism and
quality consumers to agree with the shared facts. The complete conformance,
mutation, allocation-failure, sanitizer, performance, project, and byte-exact
self-host gates remain mandatory. Existing analysis scaling at or below the
permanent 1.25 exponent is not relaxed.

## Removal

Remove or narrow any transfer whose interval proof is not conservative. Never
infer a missing bound, execute from an unknown fact, silently widen the proof
domain, or replace checked runtime behavior without a separate scored decision
and source-order trap proof. A future range annotation or stronger solver must
show application evidence that inference cannot provide and must preserve the
same unknown-by-default boundary.
