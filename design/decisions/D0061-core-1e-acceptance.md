# D0061: Core 1E acceptance boundary

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 2
Minimal: 2
Analysis: 0
Dogfood: 2
Score: 85

## Need

Core 1E set out to improve native efficiency without LLVM, unsafe execution
profiles, source optimizer hints, or another executable representation. After
two measured changes, further work would require new range, alias, or storage
proofs rather than another mechanical lowering fix. The achieved boundary and
its remaining costs need to be frozen before later milestones add analyses or
parallel execution.

The accepted boundary keeps the portable C11 backend, makes canonical checked
runtime fast paths optimizer-visible, and consumes the retained memory plan to
elide empty child regions. It never removes a trap or bound merely because an
optimized build is requested; absence of proof retains the dynamic check.

## Alternatives

Continuing until every benchmark equals handwritten C would reward benchmark
specialization and conflate different algorithms and storage representations.
An unchecked release profile would create two languages. LLVM or mandatory LTO
would increase dependency and build cost without addressing missing semantic
proof. Fixed arrays, iterators, optimizer hints, and source lifetime annotations
would add surface primarily to repair implementation performance.

The current boundary fixes costs already disproved by checked facts and leaves
collection bounds, arithmetic overflow, and dynamic layout intact where their
proof is unknown.

## Costs

SLIM vectors retain dynamic length, element size, and region metadata while
several C challenge oracles use fixed stack arrays. Matrix source uses flattened
cell traversal with division/remainder where its C oracle uses nested loops.
Merge sort retains checked recursive indices, and byte frequency reads the
whole input through the language's total file API rather than the oracle's
streaming buffer. These are honest remaining semantic or algorithmic costs,
not grounds for unchecked lowering.

External C compilation remains roughly 120-150 milliseconds for the challenge
programs and is still the largest obstacle to the final millisecond build goal.
The C backend remains the permanent portable reference, not necessarily the
future only backend.

## Evidence

The authoritative release run measures SLIM/C runtime ratios of 1.066 for
gcd/fibonacci, 1.720 sieve, 1.386 BFS, 1.620 matrix, 1.871 merge sort, 1.657 byte
frequency, 1.341 binary search, 1.048 prefix sum, 1.308 records, 1.017 variants,
1.235 arena sum, and 1.208 knapsack. All non-startup-dominated durable limits
are tightened again to 1.75-2.5.

Five warm self-host checks consume 0.03 seconds of user CPU. The complete gate
passes 109 fixtures including eleven exact runtime traps, 2,000 deterministic
mutations, sanitizer and allocation-failure checks, all native challenges,
agent feedback evidence, and a byte-identical 1,622,580-byte seed with SHA-256
`a22b64129cd5e4808c4aa4bc256218e894589933452474a5aa3b2fde82365796`.

## Removal

Reopen the boundary only for a profiled cost with a checked proof or a separately
scored backend design. Preserve the exact traps, tightened ratios, portable C
reference, and compiler-latency gates. Do not add unsafe profiles, benchmark
intrinsics, source optimizer hints, mandatory LTO, or LLVM as an implicit
dependency.
