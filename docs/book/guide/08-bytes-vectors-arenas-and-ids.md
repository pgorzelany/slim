# Bytes, vectors, arenas, and IDs

SLIM's collection surface separates immutable byte views, typed growable
vectors, and arena-owned stable IDs so allocation and lifetime behavior remain
explicit.

## Byte storage

`Bytes` is an affine byte buffer. Its length and indexed reads use `bytes.len`
and `bytes.get`; indexing traps when out of bounds. `bytes.freeze` produces a
frozen view and participates in the lifetime rules.

<!-- slim-fixture: bytes-bounds -->

File input appends into an `inout Vec[U8]` and returns `Bool`; freezing the
completed vector produces byte storage for read-only use.

## Typed vectors

`Vec[T]` is typed and affine. Construction and growth require `alloc`.
`vec.len`, `vec.get`, `vec.push`, and `vec.set` are the only accepted vector
operations.

<!-- slim-fixture: storage -->

Both reads and writes check bounds. Growth can fail through the deterministic
allocation-failure path.

## Arenas and stable IDs

`Arena[T]` owns values of one type. `arena.add` returns an `Id[T]`, and
`arena.get` resolves an ID within its owning arena. Typed IDs prevent mixing
identities for unrelated element types.

Arena lifetime is lexical and deterministic. IDs do not become raw pointers,
and safe source cannot inspect an address.

## Allocation effects

Operations that may allocate require the `alloc` effect in the enclosing
function and its callers.

<!-- slim-fixture: missing-alloc -->

## Compiler guarantees

Checked access stays checked unless a positive fact proves the exact access
safe. Values that are out of domain, unsupported, or beyond a refinement budget
remain unknown. Generated storage is destroyed at its compiler-selected region
boundary.

## Deliberate omissions

There are no unchecked indexes, raw pointers, general shared references,
implicit collection copies, tracing garbage collection, or allocator selection
in safe source. See the [memory contract](../../MEMORY.md).
