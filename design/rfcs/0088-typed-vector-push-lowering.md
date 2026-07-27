# RFC-0088: Typed vector-push lowering

Status: rejected
Implementation: not-planned
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 80

## Need

The checked type view also gives every `vec.push` value an exact element type.
The generic runtime helper combines capacity growth with a byte-oriented
`memcpy`; hot initialization loops therefore retain element-size work even
after the helper is inlined.

Capacity, overflow, region allocation, and typed allocation failure remain in
one header-visible `slim_vec_prepare_push` helper. Generated code calls that
helper, assigns the checked element through its typed C pointer, and increments
length only after a successful assignment. The generic `slim_vec_push` calls
the same preparation helper for runtime and compatibility uses.

## Alternatives

Duplicating growth logic in code generation would create two allocation
authorities and is rejected. Reserving capacity implicitly, changing vector
growth, adding a source `push-unchecked`, or allowing failed pushes to mutate
length would change semantics. Mandatory LTO remains outside the build
contract.

## Costs

The self-hosted backend threads the existing dense type facts into the push
emitter. The runtime header gains one internal static-inline helper, while the
ABI and generic push signature remain unchanged. Generated C is larger at each
push site.

Positive scalar and aggregate pushes, reallocation, injected allocation
failure, evaluation order, deterministic C, bootstrap fixed point, sanitizers,
and all application outputs require permanent verification. There is no
syntax, type, ownership, effect, dependency, source compatibility, or safety
change.

## Evidence

The RFC-0087 full result is the baseline: 1.273 geometric mean against C and 1.337
on serial work. The prototype passes aggregate push and allocation-failure
tests, but its quick geometric mean is 1.263, identical to the retained RFC-0087
quick result, and push-heavy applications show no stable improvement. Clang
already specializes the header-visible generic helper sufficiently. The
backend and runtime prototype are therefore removed.

## Removal

Restore generic push emission if preparation and assignment can diverge, a
failed allocation changes vector state, an aggregate representation changes,
or measured performance does not justify the backend path. Never remove the
capacity or allocation checks.
