# Learn SLIM

This is the shortest path through the implemented Core 1I language. Every SLIM
example below is loaded from a repository fixture and compiled by the website
test; the website does not keep a second copy.

## Run a program

From the repository root:

```sh
./slimc check examples/hello.slim
./slimc run examples/hello.slim
```

<!-- slim-example: examples/hello.slim | output: hello from SLIM -->

A program is one `(module ...)` form. Its entry point has one exact signature:
`main` receives `(Vec Bytes)` and returns `I64`. The `(effects io)` list is a
checked capability ceiling: this function may perform I/O.

## Functions, types, and effects

Functions declare every parameter, result, and permitted effect. `(effects)`
means pure. Allocation requires `alloc`, I/O requires `io`, and recurrence that
has not been proven total requires `partial`. Calls have one canonical form:
`(call function argument...)`.

`let` introduces an immutable binding. Arithmetic uses named checked built-ins
such as `i64.add`; overflow is a defined trap rather than undefined behavior.
Tail-position `recur` repeats the current function without growing the stack.

<!-- slim-example: examples/countdown.slim | output: 55 -->

## Records, variants, and matches

Records represent product data and variants represent alternatives. Record
construction names every field in declaration order. Variant matches must be
exhaustive and follow declared case order, so adding a case cannot silently
skip existing logic.

<!-- slim-example: examples/data.slim | output: 42 -->

## Ownership and exclusive access

Scalars and typed arena IDs copy. Owned aggregates such as vectors, byte
buffers, records, and variants move. `inout` is a non-escaping exclusive
borrow: the caller passes one named unique binding, and aliases or temporaries
are rejected.

<!-- slim-example: examples/inout.slim | output: 42 -->

SLIM uses compiler-selected regions and deterministic destruction. There is no
tracing garbage collector. Allocation exhaustion follows the typed `alloc`
failure path and is handled at the executable boundary.

## Projects

A `slim.project` manifest names the entry module, module paths, imports, and
exports. Cross-module references use one qualified form such as
`math/answer`.

```sh
./slimc check conformance/projects/basic/slim.project
./slimc run conformance/projects/basic/slim.project
./slimc interfaces conformance/projects/basic/slim.project -o /tmp/slim-interfaces
```

Project resolution, public interfaces, diagnostics, and generated C are
deterministic. The compiler retains declaration-level fingerprints for
incremental reuse.

## Inspect and simplify

Analysis and transformation commands operate on ordinary checked SLIM; they do
not introduce a second executable language.

```sh
./slimc analyze examples/countdown.slim
./slimc reduce conformance/pass/reduction.slim
./slimc prove-reduction conformance/pass/reduction.slim
./slimc equivalent conformance/evidence/equivalent-left.slim conformance/evidence/equivalent-right.slim
```

Every analysis result says whether a fact is exact, bounded, or unknown.
Reduction is deterministic and idempotent. Equivalence is exact only for the
finite Boolean domain named in its report.

## Host services

Host calls use ordinary types and explicit effects. The monotonic clock has one
canonical form, `(call io.monotonic-ms)`, and requires `io`. It allocates
nothing and returns nondecreasing milliseconds for elapsed-time comparisons;
it is not wall-clock time.

<!-- slim-example: conformance/pass/monotonic_clock.slim | output: OK -->

## Automatic parallelism

SLIM has no source-level concurrency form. Core 1G may automatically fork two
adjacent computations only when the checked program proves them independent,
total, reorder-safe, supported by lowering, and profitable under the current
target model. One computation runs on a child and one on the parent; both join
before the original continuation.

Unknown, effectful, trapping, mutating, allocating, unsupported, or too-small
work remains serial. Spawn failure runs the same task inline, and unselected
programs contain no worker machinery.

Continue with `docs/CORE.md` for the grammar and `docs/STATUS.md` for the exact
current boundary.
