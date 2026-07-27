# RFC-0059: Inlinable checked runtime fast paths

Status: accepted
Implementation: complete
Process: legacy
Kind: runtime
Primitive: none
Safety: 2
Compile: 0
Runtime: 2
Minimal: 2
Analysis: 0
Dogfood: 2
Score: 65

## Need

Generated C called an external runtime function for every checked integer
operation, byte access, vector bound, vector update, and vector push. Because
the runtime is a separate translation unit and SLIM does not require link-time
optimization, the C optimizer could not see or combine the hot safety check
with its surrounding loop. Real programs paid call and generic-operation costs
even when the successful path was a few instructions.

The existing operations now remain in the permanent C11 runtime header as
`static inline` checked fast paths. Slow allocation, file, output, region, and
trap behavior stays in `slim_rt.c`. This changes no SLIM syntax, type, effect,
failure, or trap semantics and adds no optimizer dependency.

## Alternatives

Requiring LTO would make build behavior toolchain-specific and increase the
external compilation contract. Emitting unchecked arithmetic or collection
access would violate Core safety. Adding an unsafe release profile would create
two semantic modes. Duplicating each operation directly in code generation
would enlarge the SLIM compiler and create a second runtime implementation.
LLVM is unnecessary for this boundary and remains out of scope.

Header-visible checked helpers preserve one implementation while allowing an
ordinary C optimizer to inline, specialize element sizes, remove redundant
loads, and keep the cold trap or reallocation path out of hot loops.

## Costs

Every generated translation unit parses the inline helper bodies, though native
compile medians remain in the same roughly 130-to-150-millisecond band on the
recorded host. The runtime header now includes `string.h` for typed vector
copies. The existing compiler builtins for checked overflow remain the portable
C-toolchain requirement already used by `slim_rt.c`.

Moving vector growth also closes the former signed `capacity * 2` overflow
before multiplication. Bounds, arithmetic, conversion, allocation, and region
checks remain present on every path where they were previously required.

## Evidence

Same-host quick comparisons reduce SLIM runtime from 13.484 to 4.461
milliseconds for sieve, 15.367 to 3.712 for matrix multiplication, 52.597 to
8.604 for byte frequency, and 3.669 to 2.036 for knapsack. All twelve challenge
outputs remain equal to their C and Rust counterparts, and generated native
binaries shrink from about 52.6 to 51.7 kilobytes.

The self-hosted compiler exercises the same operations intensively. Five warm
checks of its 468-kilobyte project each consume 0.05 seconds of user CPU, down
from the approximately 0.11-second Core 1D boundary. The portable compiler C
remains byte-identical at 1,629,310 bytes because the fast path is the separately
audited runtime contract rather than generated compiler source.

Eight new permanent trap fixtures cover subtraction and multiplication
overflow, division by zero and overflow, remainder by zero, narrowing failure,
byte bounds, and vector-set bounds. Together with the existing addition and
vector-get fixtures, the corpus contains 109 cases and eleven runtime traps.
Native runtime-ratio budgets are tightened for every materially affected
challenge. Full sanitizers and allocation-failure injection remain release
gates.

## Removal

Move a helper out of the header only if a measured replacement retains the
same trap and allocation behavior and passes the tightened native ratios. Do
not replace these paths with unchecked operations, require LTO, duplicate the
runtime in code generation, or add a user-visible optimization profile.
