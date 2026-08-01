# Deterministic parallelism contract

Status: SLIM 0.9 guarded automatic and explicit structured execution

SLIM has no general concurrency scheduler. It admits only checked structured
work that preserves the serial program's effects, traps, ownership, result
installation, and evaluation order. The Handbook introduces the
[source form](book/handbook/11-deterministic-parallelism.md); this document owns
selection and execution guarantees.

## Reorder safety

A user computation is reorder-safe only when the complete checked body and call
graph prove:

- no observed allocation or I/O;
- no `@` exclusive parameter or lexical mutation;
- no unchecked trap hazard;
- no unknown or unsafe callee; and
- complete recurrence totality where recurrence is present.

An effect declaration is a capability ceiling, not evidence that an event
occurred. Conversely, a checked arithmetic or indexing operation remains a
hazard unless a positive fact proves that exact node total. Cyclic graphs,
missing bodies, unsupported transfers, and exceeded bounds remain unknown.

Two adjacent immutable call initializers form an automatic candidate only when
both are reorder-safe and the second does not depend on the first. Candidate,
selected, reported, executable, and executed counts remain distinct. Selection
uses stable lexical node order and pairwise non-overlapping intervals.

## Bounded analysis

`slimc analyze SOURCE_OR_PROJECT` uses the ordinary validated checked artifact.
Analysis schema 7 retains stable node identities, complete blocker sets,
recurrence work where exact, schedule intent, and the five execution stages.

The graph retains at most 64 functions and 4,096 direct edges and performs at
most 64 resolution passes. At most 64 selected sites are printed, while larger
complete counts remain visible with a bounded guarantee. Safe functions have an
exact empty blocker set; uncertainty is never silently omitted.

## Automatic execution

RFC-0071 permits one narrow automatic shape: two leading direct user calls with
atomic captures, total reorder-safe callees, codegen-supported placement, and
at least 1,000,000 exact recurrence iterations each. Computed captures,
unsupported placement, unknown work, or any safety uncertainty remain serial.

The generated program attempts one child, runs the other task in worker scope,
joins once, then installs both results in lexical order. A declined
worker-creation failure executes the identical task body inline. Every
successful child has one parent-owned join, and worker scope declines nested
spawning.

An admitted task cannot fail and a child has no source operation that waits, so
cancellation and wait cycles do not exist in the automatic subset.

`SLIM_PARALLEL` selects the structured ABI. `SLIM_POSIX_WORKERS` selects the
POSIX worker tier where supported. Programs with no executable site contain no
worker context, environment parsing, thread include, or worker link flag.

## Explicit bounded host work

`parallel:` admits exactly two leading immutable direct leaf calls followed by
their continuation. Parameters are scalars or `Bytes`, never exclusive; neither
callee declares `partial`; each task has a checked bounded host-operation
shape. Pure work remains governed by automatic selection.

Allocating tasks use separate child regions and share only atomic allocation
status. After join, the parent adopts both allocation lists before exposing
owned results. Unsupported targets and declined spawns execute the identical
calls serially. There are no task handles, detached lifetimes, locks, channels,
or observable scheduling order.

## Permanent evidence

The complete twenty-program challenge corpus, compiler dogfood input, overlap
fixtures, and structured host applications are permanent gates.
`benchmarks/parallelism-baseline.tsv` owns analysis counts. The performance
budget ledger owns manual, generated parallel/serial, and structured-host
ratios. Historical derivation and acceptance measurements remain in
RFC-0062 through RFC-0071, RFC-0078 through RFC-0079, and dated Development
evidence.
