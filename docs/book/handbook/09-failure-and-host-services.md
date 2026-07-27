# Failure and host services

SLIM distinguishes checked runtime traps, typed host failure, and deterministic
allocation exhaustion. Host interaction remains a narrow, effect-gated
boundary.

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

## Terminal and file I/O

`io.print_i64`, `io.print_bytes`, and `io.println` require `io`. Output follows
ordinary left-to-right evaluation.

`io.read_file(Bytes, inout Vec[U8]) -> Bool` appends the complete file on
success. Failure returns `false` and leaves the output unchanged. The operation
requires `alloc` and `io`.

<!-- slim-fixture: file-input -->

## Time and bounded networking

`io.monotonic_ms() -> I64` provides a monotonic millisecond clock for duration
measurement, not calendar time.

<!-- slim-fixture: monotonic-clock -->

Omitting its capability is diagnosed:

<!-- slim-fixture: missing-clock-effect -->

Networking is one bounded whole exchange rather than a socket API:

<!-- slim-fixture: tcp-exchange -->

The complete signature, target support, deadline, output, and failure rules are
specified by the [host contract](../../HOST.md).

## Compile-time rejection

Type, ownership, effect, project, and syntax errors prevent execution. Their
diagnostics have stable codes and byte spans and can be consumed as JSON.

## Exact rules and common errors

Reduction and generated-code optimization preserve visible output before a
trap, trap identity, allocation ordering, allocation failure, moves, mutation,
and destruction. Host calls retain checked argument and result types and are
never introduced by a pure operation. Unknown evidence never becomes a safety
claim.

There are no exceptions, implicit panics, unchecked arithmetic modes, nullable
failure sentinels, environment API, process spawning, unrestricted sockets,
wall clock, event loop, or ambient host authority.

## Next

Continue with [projects, imports, and interfaces](10-projects-imports-and-interfaces.md).
For exact diagnostic records, see [Diagnostics](../../DIAGNOSTICS.md).
