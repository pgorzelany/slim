# Types and value categories

This chapter defines the accepted scalar, aggregate, storage, identity, and
result categories and the static rules that connect them.

## Scalar types

`Bool`, `I64`, `U8`, and `Void` are the scalar types. Boolean operations accept
and return `Bool`. Integer arithmetic and comparison operate on `I64`. `U8`
conversion is explicit. `Void` represents an effect-oriented result and is not
storable.

There is no implicit conversion, numeric promotion, truthiness, or overload
selection.

## Product and sum types

A named struct is a product whose field names, types, and order come from its
declaration. A named enum is a closed sum whose cases, payload shapes, and
order come from its declaration.

Construction, projection, mutation, case construction, and matching check the
declared name and exact component types.

## Storage types

`Bytes` is a copyable immutable view with a compiler-checked backing lifetime.
`Vec[T]` is an affine typed vector. `Arena[T]` is affine and owns typed
elements, while `Id[T]` is a copyable identity valid for the matching arena
element type. Storage operations are ordinary built-ins with declared effects
and checked indexes.

## Value categories and ownership

`Bool`, `U8`, `I64`, `Id[T]`, and `Bytes` are copyable. Copying `Bytes` copies
only its view and retains the checked relationship to its backing region.
`Vec[T]`, `Arena[T]`, and aggregates containing either are affine. Other
aggregates are copyable.

An affine value may move, after which its old binding is invalid. A move
transfers ownership but performs no implicit allocation or element copy.
`inout` is an exclusive nonescaping borrow of a named caller binding, not
another owning or storable reference type. Safe source cannot form raw pointers
or untracked aliases.

`let` and `var` are binding categories rather than value categories. `let`
rejects direct rebinding in its lexical scope; `var` permits it. Neither changes
the ownership category of the declared type.

## Function signatures

Function types are expressed by declarations rather than first-class function
values. Parameters may be owned inputs or `inout` borrows. Results transfer
their value according to its ownership category. Effects are a checked
capability ceiling on the function body and callees.

## Type diagnostics

Unknown types are `E0354`; expression and argument mismatches use the stable
diagnostic identities specified by [Diagnostics](../../DIAGNOSTICS.md).
Composite type syntax, arity, field identity, and case identity are checked
before lowering.

## Normative boundary

The accepted type grammar is in [Core](../../CORE.md), and storage lifetime
semantics are in [Memory](../../MEMORY.md).
