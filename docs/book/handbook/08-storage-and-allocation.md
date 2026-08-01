# Storage and allocation

SLIM's collection surface separates immutable byte views, typed growable
vectors, and arena-owned stable IDs so allocation and lifetime behavior remain
explicit.

## Byte storage

`Bytes` is a copyable immutable view, not an affine owner. Copying it copies
only its data-and-length view; the compiler keeps the backing region alive for
all copies. Its length and indexed reads use `bytes.len` and `bytes.get`;
indexing traps when out of bounds.

<!-- slim-fixture: bytes-bounds -->

File input appends into an `@Vec[U8]` and returns `Bool`; freezing the completed
vector with `bytes.freeze(^vector)` consumes that unique owner and produces a `Bytes` view of the
same buffer without copying its contents.

<!-- slim-fixture: example-bytes -->

## Typed vectors

`Vec[T]` is typed and affine. Construction and growth require `alloc`.
`vec.len`, `vec.get`, `vec.push`, and `vec.set` are the only accepted vector
operations.

<!-- slim-fixture: example-vector-sum -->

<!-- slim-fixture: storage -->

Both reads and writes check bounds. Growth can fail through the deterministic
allocation-failure path. `vec.new()` creates an empty owner handle; backing
storage is obtained when growth first requires capacity.

## Arenas and stable IDs

`Arena[T]` is affine and owns values of one type. `arena.add` returns a
copyable `Id[T]`, and
`arena.get` resolves an ID within its owning arena. Typed IDs prevent mixing
identities for unrelated element types.

Arena lifetime is lexical and deterministic. IDs do not become raw pointers,
and safe source cannot inspect an address.

## Allocation effects

Operations that may allocate require the `alloc` effect in the enclosing
function and its callers.

<!-- slim-fixture: missing-alloc -->

## Exact rules and common errors

Checked access stays checked unless a positive fact proves the exact access
safe. Values that are out of domain, unsupported, or beyond a refinement budget
remain unknown. Generated storage is destroyed at its compiler-selected region
boundary. Bounds, ownership, element types, IDs, and required `alloc` effects
are checked.

There are no unchecked indexes, raw pointers, general shared references,
implicit collection copies, tracing garbage collection, or allocator selection
in safe source. See the [memory contract](../../MEMORY.md).

## Next

Continue with [failure and host services](09-failure-and-host-services.md).
