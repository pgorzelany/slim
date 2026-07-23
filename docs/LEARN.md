# Learn SLIM

This is the shortest path through the implemented Core 1J language. Every SLIM
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

Bounded request/response networking is also one ordinary call:
`io.tcp-exchange` takes a numeric address, port, request bytes, response limit,
positive timeout, and output vector. It requires `alloc io`, returns `false`
instead of exposing a failed handle, and leaves output unchanged on failure.

<!-- slim-example: conformance/pass/tcp_exchange.slim -->

## Structured parallelism

Core 1G may automatically fork two adjacent computations when it proves them
independent, total, reorder-safe, supported by lowering, and profitable.
Unknown or too-small work remains serial.

Core 1J adds one explicit wrapper for independent bounded host calls. It reuses
ordinary `let` and `call`; there are no futures, task handles, locks, or
detached work. One call may run on a child, the other on the parent, and both
join before the continuation. Spawn failure executes the same calls serially.

<!-- slim-example: conformance/pass/structured_fork.slim | output: OK -->

Effectful tasks are checked leaf functions with scalar or byte-view inputs, no
`inout`, no `partial`, and a bounded clock or TCP operation. Task allocation
regions are isolated and ownership transfers only after join.

Continue with `docs/CORE.md` for the grammar and `docs/STATUS.md` for the exact
current boundary.
