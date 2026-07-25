# Learn SLIM

This is a guided tour of the complete accepted SLIM 1.0 language surface.
Read it in order for the programming model, then use the generated surface
index at the end as a completeness checklist. Every substantial SLIM example
is loaded from a repository fixture and compiled by the website test; the
website does not keep a second source copy.

The tour is split into
[source basics](#run-and-format-a-program),
[values and control](#scalar-values-and-checked-operations),
[data and memory](#records-variants-construction-and-projection),
[host services](#host-i-o),
[projects and parallelism](#projects-and-qualified-references), and
[compiler tools](#diagnostics-and-compiler-tools).

## Run and format a program

From the repository root:

```sh
./slimc check examples/hello.slim
./slimc run examples/hello.slim
./slimc fmt examples/hello.slim --check
./slimc build examples/hello.slim -o /tmp/hello
./slimc emit-c examples/hello.slim -o /tmp/hello.c
```

<!-- slim-example: examples/hello.slim | output: hello from SLIM -->

Each source starts with one `module NAME` header. Blocks use exactly two ASCII
spaces per level. Tabs, odd or skipped indentation, braces, commas, and
semicolons are rejected; `slimc fmt` emits the one canonical layout. `#`
starts a line comment.

Identifiers may contain ASCII letters, digits, `_`, `-`, `.`, `/`, `?`, or
`!`, but cannot start with a digit. Calls use `function(argument...)` with
whitespace-separated arguments. Integers are signed base-10 `I64` literals.
Byte strings use double quotes and support `\\`, `\"`, `\n`, `\r`, `\t`, and
exact `\xNN` byte escapes.

## Program shape, functions, and entry

A function declares every parameter and its result. There are no inferred
public signatures, overloads, default arguments, methods, operators, or
implicit conversions. A type is one of the four scalar types, a declared
record or variant, or `Bytes`, `Vec[T]`, `Arena[T]`, or `Id[T]`.

Every executable has exactly
`fn main(args: Vec[Bytes]) -> I64 ...`. Element zero is the executable path;
the remaining values are explicit process arguments. The result must fit
`0..255` and becomes the process exit code.

Functions without an `effects[...]` clause are pure. Calls have the same syntax
whether they target a user function, a qualified project function, or a
built-in.

## Scalar values and checked operations

The scalar types are:

- `Unit`, with the single value `unit`, represents a completed operation with
  no interesting result.
- `Bool` has the literals `true` and `false`.
- `I64` is a signed 64-bit integer.
- `U8` is an unsigned byte.

SLIM uses named operations instead of symbolic operators. `i64.add`,
`i64.sub`, `i64.mul`, `i64.div`, and `i64.rem` take two `I64` values and return
`I64`. The comparisons `i64.eq`, `i64.lt`, `i64.le`, `i64.gt`, and `i64.ge`
return `Bool`. `bool.not`, `bool.and`, and `bool.or` are the complete Boolean
operation set. `u8.to-i64` widens exactly; `i64.to-u8` checks that the value
fits.

<!-- slim-example: conformance/pass/scalars.slim | output: 42 -->

Arithmetic overflow, division or remainder by zero, failed `I64`-to-`U8`
conversion, and out-of-bounds access are defined runtime traps, never undefined
behavior. A trap is not an effect and cannot be caught in SLIM 1.0. The
compiler may remove a check only when it has a positive fact for that exact
operation.

## Bindings, evaluation, mutation, and repetition

Evaluation is left-to-right. `let name: Type = value` creates a typed binding.
Bindings are immutable except for the single `set name = value` form, which
requires unique lexical access and preserves the declared type. There are no
assigning operators or implicit mutation.

`match` is the only branching form. A Boolean match contains `true` and
`false` exactly once. A variant match contains every declared case exactly
once and in declaration order. Every arm produces the same expected type.

Tail-position `recur(arguments...)` transfers control to the beginning of the
current function with exact parameter types and does not grow the stack. It is
the one repetition primitive; there is no second loop syntax. Recurrence not
proved total belongs under `partial`.

<!-- slim-example: examples/countdown.slim | output: 55 -->

The 1.0 checker enforces `partial` transitively when a function calls another
function that declares it. It does not yet infer a missing `partial`
declaration from every directly written unproved `recur`; authors must retain
the capability unless totality has been established. This implementation gap
is recorded on the Status page.

## Records, variants, construction, and projection

A `record` is product data with an ordered list of fields. `make T(...)` names
every field exactly once and in declaration order. `get(value field)` is the
only projection form.

A `variant` is sum data with zero or more typed payload values per case.
`case T::Case(...)` constructs one case. Pattern bindings in `match` have the
declared payload types. Exhaustiveness and canonical case order make an added
case a compile-time obligation rather than silently skipped behavior.

This fixture also demonstrates the one `set` form:

<!-- slim-example: conformance/pass/data.slim | output: 42 -->

Records and variants containing only copyable values are copyable. Those
containing owned storage are affine and move with that storage.

## Effects and failure

An effect list is an ordered capability ceiling:

- `alloc` permits allocation or a call that may allocate.
- `io` permits a host operation.
- `partial` permits a call or recurrence whose termination is not proved.

When present, effects appear once and in exactly that order. A caller must
declare every capability required transitively by the operations it performs.
Declaring a capability does not prove that the operation occurred, and an
unused capability may be discharged by complete body and call-graph analysis
when the compiler decides whether work is reorder-safe.

Checked traps are deliberately separate from effects. Host operations return
ordinary typed success values where recovery is supported. Allocation
exhaustion uses the `alloc` failure channel, unwinds compiler-selected regions,
and is handled once at the executable boundary with exit code 71; SLIM 1.0 has
no source-level local allocation recovery.

## Bytes, vectors, arenas, and IDs

`Bytes` is an immutable byte view. `bytes.len` returns its length and
`bytes.get` performs checked access. `bytes.freeze` consumes one unique
`Vec[U8]` and creates a view of the same storage without copying.

`Vec[T]` is an owned growable sequence. `vec.new` creates one; `vec.len` and
`vec.get` observe it; `vec.push` and `vec.set` mutate it through unique access.
`vec.get` returns only copyable elements, so reading never creates two owners.
`vec.new` and `vec.push` require `alloc`; `vec.set` retains the existing length
and does not allocate.

<!-- slim-example: examples/bytes.slim | output: Hi -->

`Arena[T]` is an owned typed region. `arena.add` stores a value and returns a
copyable `Id[T]`; `arena.get` performs checked lookup using an ID from the
matching arena element type. SLIM exposes neither raw addresses nor pointer
arithmetic.

The storage conformance fixture exercises every byte, vector, arena, and ID
built-in in one checked program:

<!-- slim-example: conformance/pass/storage.slim -->

## Ownership, borrowing, and lifetimes

Scalars, IDs, immutable `Bytes` views, and aggregates containing only copyable
fields copy. Vectors, arenas, and aggregates containing either are affine:
passing, returning, storing, or projecting them transfers ownership. Using a
moved value is a compile-time error.

An `inout` parameter is a non-escaping exclusive lexical borrow. The caller
passes one named unique binding; temporaries, aliases, moves during the borrow,
and returning the borrow are rejected.

<!-- slim-example: examples/inout.slim | output: 42 -->

The compiler plans backing regions, storage escape destinations, last uses,
and deterministic destruction. Conservative bounds may lengthen a lifetime
but never shorten it. There is no tracing garbage collector, reference
counting, user-written lifetime annotation, user-selected allocator, or
unsafe pointer escape hatch.

## Host I/O

Host calls use ordinary types and explicit effects:

- `io.print-i64(I64)`, `io.print-bytes(Bytes)`, and `io.println(Bytes)` return
  `Unit` and require `io`.
- `io.read-file(Bytes, inout Vec[U8]) -> Bool` requires `alloc io`; success
  appends the complete file, while failure returns `false` and leaves output
  unchanged.
- `io.monotonic-ms() -> I64` requires `io`, allocates nothing, and returns
  nondecreasing milliseconds for elapsed-time comparisons. Its epoch is not
  wall-clock time.
- `io.tcp-exchange(Bytes, I64, Bytes, I64, I64, inout Vec[U8]) -> Bool`
  requires `alloc io`. It performs one bounded numeric-address request and
  response with explicit response and timeout limits. Failure closes the
  runtime-lexical handle, returns `false`, and leaves output unchanged.

<!-- slim-example: conformance/pass/file_input.slim | output: OK -->

<!-- slim-example: conformance/pass/monotonic_clock.slim | output: OK -->

<!-- slim-example: conformance/pass/tcp_exchange.slim -->

SLIM 1.0 deliberately has no source socket or file handles, DNS, TLS,
filesystem writes, wall clock, general FFI, or ambient unsafe host access.

## Projects and qualified references

A `slim.project` manifest names the entry module, source paths, direct imports,
and exports. Imported declarations use exactly one qualified form such as
`math/answer`; there are no aliases, glob imports, implicit imports,
re-exports, or source search paths.

```sh
./slimc check conformance/projects/basic/slim.project
./slimc run conformance/projects/basic/slim.project
./slimc interfaces conformance/projects/basic/slim.project -o /tmp/slim-interfaces
./slimc check conformance/projects/basic/slim.project --jobs 2
```

The manifest owns dependency order and visibility. Interfaces, diagnostics,
caches, and generated C are deterministic. `--jobs N` checks independent
topological layers with isolated workers; `--jobs 1` remains the serial oracle
and default because the current measured projects did not justify a parallel
default.

## Automatic and explicit parallelism

The compiler can automatically fork two adjacent immutable calls when it
proves them independent, total, reorder-safe, supported by lowering, and
profitable. Tasks cannot allocate, perform I/O, trap, mutate, borrow
exclusively, call an unknown function, or wait. Results join before the
continuation. Unknown or too-small work remains serial.

SLIM also has one explicit `fork:` wrapper for two independent bounded host
calls whose effect independence is not inferable. It reuses ordinary `let`
bindings and calls; there are no futures, task handles, locks, channels, or
detached work. One call may run on a child and one on the parent, and both join
before the continuation. A declined spawn executes the identical work inline.

<!-- slim-example: conformance/pass/structured_fork.slim | output: OK -->

Explicit tasks are checked leaf functions with scalar or byte-view inputs, no
`inout`, no `partial`, and one bounded clock or TCP operation. Their allocation
regions are isolated and ownership transfers only after join. These
restrictions make admitted sites data-race-free and deadlock-free by
construction; SLIM does not expose a general concurrency scheduler.

## Diagnostics and compiler tools

Every diagnostic has a stable code, severity, primary byte span, optional
labels, notes, and fixes. Human and newline-delimited JSON output carry the same
facts; use `--message-format=json` before `check`, `build`, `run`, or another
diagnostic-producing command. Invalid user input must not panic the compiler.

Analysis tools consume the same checked canonical SLIM program:

```sh
./slimc analyze examples/countdown.slim
./slimc reduce conformance/pass/reduction.slim > /tmp/reduced.slim
./slimc prove-reduction conformance/pass/reduction.slim
./slimc verify-reduction conformance/pass/reduction.slim /tmp/reduced.slim
./slimc equivalent conformance/evidence/equivalent-left.slim conformance/evidence/equivalent-right.slim
./slimc builtins
./slimc runtime /tmp/slim-runtime
```

`analyze` reports independent quality and cost facts as exact, bounded with a
fixed budget, or unknown with a stable reason. `reduce` performs a small
terminating, deterministic, idempotent set of behavior-preserving rewrites.
Proof verification recomputes the result from checked source. `equivalent` is
exact only over the reported finite Boolean-product or single-byte domain.
`edit` applies one bounded non-executable structural patch and publishes output
only after the normal checker accepts it.

Reports, proofs, patches, interfaces, caches, and generated C are derived
artifacts, not additional executable languages. Canonical `.slim` remains the
compiler's sole accepted program representation.

## Complete accepted surface

The following index is generated directly from `design/surface.tsv`. It is the
complete accepted source, type, effect, entry, built-in, and backend surface for
the displayed compiler version. If that ledger changes, the website rebuild
changes this list and its completeness test automatically.

<!-- slim-surface-index -->

For normative grammar and edge cases use `docs/CORE.md`; for the exact
implemented boundary and deliberate omissions use `docs/STATUS.md`.
