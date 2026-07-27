# Direct reduction and semantic analysis

Status: current bounded reduction contract

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

The current reducer admits only these rule families:

- literal Boolean `not`, `and`, and `or` evaluation;
- literal Boolean `match` selection;
- `==` evaluation when both integer spellings are identical;
- removal of `VALUE + 0`, `VALUE - 0`, and `VALUE * 1`
  when `VALUE` is an atom;
- removal of `VALUE && true` and `VALUE || false` when `VALUE`
  is an atom;
- replacement of `VALUE && VALUE` or `VALUE || VALUE` with the
  repeated atom;
- replacement of `(match VALUE (true true) (false false))` with the Boolean
  atom `VALUE`;
- replacement of a Boolean `if` whose two branches return the same atom with that
  atom, only when the scrutinee is also an atom; and
- removal of an unused immutable `Bool` or `I64` binding whose value
  is a matching literal.

Every replacement is recursively reduced. The reducer performs at most eight
closure passes. If the result has not stabilized, it emits the canonical input
unchanged; a subsequent invocation therefore produces the same bytes. An
unused-binding search examines at most 64 tokens and retains the binding when
that bound is exceeded. These bounds prevent deeply nested source from turning
reduction into a default superlinear algorithm.

No rule discards or reorders a call, allocation, I/O operation, checked
operation that may trap, aggregate move, mutation, or exclusive borrow.

`slimc prove-reduction` emits proof schema 2 and names
`canonical-tokens-v1`. Every reported rewrite strictly decreases that cost.
The report records exact canonical source/result token counts and at most 64
sites. Replay recomputes the normal form from checked source; proof text is
never trusted as executable input.

## Analysis contract

`slimc analyze SOURCE` writes a deterministic `(analysis 7 ...)` report. Token
indices in the canonical input are stable node and binding identities. Each
function reports its declared effects and every parameter, lexical binding,
and pattern binding that fits the bounded fact table. Binding facts include the
declared type when locally available, storage ownership class, lexical use
count, last-use token, and dependency-edge count. Unknown pattern types are
reported explicitly rather than inferred unsafely.

The report is data for agents and tests. It is not SLIM source, is never parsed
by the production compiler, and makes no equivalence or optimality claim.
Core 1B additions—state models, effect/failure surfaces, ownership pressure,
totality classifications, reduction proof replay, finite equivalence, and
structural edits—are specified in `docs/QUALITY.md`.
Core 1F reorder-safety and independent parallel-site evidence is specified in
`docs/PARALLELISM.md`. Core 1H recurrence and call-work evidence is specified
in `docs/RESOURCE_BOUNDS.md`. Analysis data is not an executable IR.

## Representation boundary

An analysis may derive an ephemeral per-function control-flow graph from these
facts. Such a graph has no source extension, parser, formatter, persistent file
format, or independent type system. If a future algorithm requires any of
those, it requires a separately weighted architecture decision.
