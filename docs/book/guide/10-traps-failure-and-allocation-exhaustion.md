# Traps, failure, and allocation exhaustion

SLIM distinguishes checked runtime traps, typed host failure, and deterministic
allocation exhaustion so each failure path remains explicit and testable.

## Runtime traps

Checked arithmetic, conversions, and indexing trap when their contract is
violated. A trap exits with status `70` and a stable message.

<!-- slim-fixture: division-zero -->

A trap is observable. The compiler may eliminate a trap hazard only from
positive evidence for that exact checked operation.

## Allocation exhaustion

Allocation-capable operations require `alloc`. Exhaustion follows the
deterministic runtime failure contract, exits with status `71`, and destroys
completed regions correctly.

<!-- slim-fixture: allocation-failure -->

The conformance harness can inject failure at a precise allocation count, which
makes cleanup and retained allocation sites permanent, repeatable evidence.

## Host failure

Fallible host operations return `Bool` rather than throwing. A caller uses
`if`/`else`; on file or TCP failure, the documented output vector is unchanged.
Declaring `io` permits the capability but does not create hidden control flow.

## Compile-time rejection

Type, ownership, effect, project, and syntax errors prevent execution. Their
diagnostics have stable codes and byte spans and can be consumed as JSON.

## Compiler guarantees

Reduction and generated-code optimization preserve visible output before a
trap, trap identity, allocation ordering, allocation failure, moves, mutation,
and destruction. Unknown evidence never becomes a safety claim.

## Deliberate omissions

There are no exceptions, implicit panics, unchecked arithmetic modes, nullable
failure sentinels, or recovery handlers hidden by syntax. See
[host I/O](11-host-io.md) and [Diagnostics](../../DIAGNOSTICS.md).
