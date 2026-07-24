# D0087: Typed vector-set lowering

Status: accepted
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

The checked type view gives every `vec.set` target and value one exact element
type. Generated `vec.get` already uses that type for a direct C access guarded
by `slim_vec_check_index`, while `vec.set` calls the generic byte-oriented
runtime helper. Hot mutation therefore repeats element-size multiplication,
offset-overflow control flow, and `memcpy` even though a successfully allocated
typed vector already guarantees its storage extent.

Code generation lowers a checked `vec.set` to one typed C assignment using the
shared `slim_vec_check_index` result. The bounds check and trap remain exactly
at the operation. Computed arguments remain materialized before the assignment,
preserving evaluation order.

## Alternatives

Removing the bounds check is forbidden. Adding `vec.set-unchecked`, an unsafe
profile, a specialized source builtin per element type, or a second vector
representation would enlarge the language surface. Mandatory LTO might
specialize the generic helper but remains rejected by D0059.

Keeping the generic `memcpy` path for all sets ignores the checked type fact
already consumed by symmetric `vec.get` lowering.

## Costs

The self-hosted backend threads its existing dense typing facts into the
vector-set emitter and emits a typed assignment. The runtime helper remains the
canonical generic implementation for runtime and compatibility use; code
generation continues to call its one canonical bounds checker.

Generated C changes for every `vec.set`, so exact positive behavior, out-of-
bounds traps, aggregate element assignment, deterministic emission, sanitizer
coverage, bootstrap fixed point, and all twenty application outputs require
permanent tests. There is no syntax, dependency, runtime ABI, allocation,
ownership, effect, or source compatibility change.

## Evidence

The D0086 full result is the baseline. Typed set lowering improves the full
SLIM/C geometric mean from 1.335 to 1.273 and the serial mean from 1.412 to
1.337. `merge_sort` improves from 1.679 to 1.152 and `edit_distance` from 1.738
to 1.037. All twenty outputs remain identical; the aggregate-element fixture,
out-of-bounds trap, bootstrap fixed point, sanitizers, and complete repository
gate pass.

## Removal

Restore the generic helper call if typed assignment changes any supported
value representation, evaluation order, bounds trap, sanitizer result, or
allocation failure. Retain the benchmark and safety fixtures. Do not replace
this lowering with unchecked access or source aliases.
