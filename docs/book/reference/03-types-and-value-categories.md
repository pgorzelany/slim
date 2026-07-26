# Types and value categories

This chapter defines the accepted scalar, aggregate, storage, identity, and
result categories and the static rules that connect them.

## Scalar types

`Bool`, `I64`, `U8`, and `Unit` are the scalar types. Boolean operations accept
and return `Bool`. Integer arithmetic and comparison operate on `I64`. `U8`
conversion is explicit. `Unit` represents the result of effect-oriented
operations.

There is no implicit conversion, numeric promotion, truthiness, or overload
selection.

## Product and sum types

A named record is a product whose field names, types, and order come from its
declaration. A named variant is a closed sum whose cases, payload shapes, and
order come from its declaration.

Construction, projection, mutation, case construction, and matching check the
declared name and exact component types.

## Storage types

`Bytes` is affine byte storage. `Vec[T]` is an affine typed vector.
`Arena[T]` owns typed elements, and `Id[T]` is an identity valid for the
matching arena element type. Storage operations are ordinary built-ins with
declared effects and checked indexes.

## Value categories and ownership

Scalar values are copyable. Storage-bearing values and aggregates containing
them are affine. An affine value may move, after which its old binding is
invalid. `inout` is an exclusive nonescaping borrow of a named mutable binding,
not another owning type.

Frozen byte views retain a checked lifetime relation to their source. Safe
source cannot form raw pointers or untracked aliases.

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
