# Direct reduction and semantic analysis

Status: Core 1A internal tool specification

Canonical `.slim` source is the compiler's only accepted program
representation. The reducer traverses checked expression forms directly and
emits another canonical `.slim` module. The analyzer derives versioned facts
from the same token tree. Neither tool changes Core syntax or program
acceptance.

## Reduction contract

`slimc reduce SOURCE` writes a reduced canonical module to standard output.
The command rejects malformed or semantically invalid input through the normal
compiler diagnostics. Applying it twice must produce the same bytes as applying
it once.

Core 1A admits only these rule families:

- literal Boolean `not`, `and`, and `or` evaluation;
- literal Boolean `match` selection;
- `i64.eq` evaluation when both integer spellings are identical;
- removal of `i64.add VALUE 0`, `i64.sub VALUE 0`, and `i64.mul VALUE 1`
  when `VALUE` is an atom;
- removal of `bool.and VALUE true` and `bool.or VALUE false` when `VALUE`
  is an atom; and
- removal of an unused immutable `Unit`, `Bool`, or `I64` binding whose value
  is a matching literal.

Every replacement is recursively reduced. The reducer performs at most eight
closure passes. If the result has not stabilized, it emits the canonical input
unchanged; a subsequent invocation therefore produces the same bytes. An
unused-binding search examines at most 64 tokens and retains the binding when
that bound is exceeded. These bounds prevent deeply nested source from turning
reduction into a default superlinear algorithm.

No rule discards or reorders a call, allocation, I/O operation, checked
operation that may trap, aggregate move, mutation, or exclusive borrow.

## Analysis contract

`slimc analyze SOURCE` writes a deterministic `(analysis 1 ...)` report. Token
indices in the canonical input are stable node and binding identities. Each
function reports its declared effects and every parameter, lexical binding,
and pattern binding that fits the bounded fact table. Binding facts include the
declared type when locally available, storage ownership class, lexical use
count, last-use token, and dependency-edge count. Unknown pattern types are
reported explicitly rather than inferred unsafely.

The report is data for agents and tests. It is not SLIM source, is never parsed
by the production compiler, and makes no equivalence or optimality claim.

## Representation boundary

An analysis may derive an ephemeral per-function control-flow graph from these
facts. Such a graph has no source extension, parser, formatter, persistent file
format, or independent type system. If a future algorithm requires any of
those, it requires a separately weighted architecture decision.
