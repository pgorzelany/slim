# Core 0.4 memory model and audit

Status: implementation specification

Core 0.4 strengthens the implementation of the existing ownership model. It
adds no source syntax, type, effect, built-in, or alternative allocation path.
The durable architecture decision is D0025.

## Core 0.3 baseline

The source checker already establishes these rules:

- scalar values, typed IDs, immutable `Bytes`, and aggregates containing only
  copyable fields may be copied;
- vectors, arenas, and aggregates containing either are affine and move;
- an `inout` parameter is an exclusive, non-escaping lexical borrow;
- moving an owned binding makes subsequent use or mutation an error;
- `vec.get` and `arena.get` only return copyable elements; and
- `bytes.freeze` consumes one unique `(Vec U8)` and returns a copyable immutable
  view without copying its buffer.

The runtime does not yet exploit those facts. It places all allocations in one
global linked list, searches the list during reallocation, and releases the
list only at process shutdown. This is memory-safe for accepted programs but is
not compile-time lifetime planning or narrow deterministic release.

There is also an ownership projection obligation that global retention masks:
project sessions extract multiple vector fields from an affine `State` record.
Core 0.4 must represent disjoint field moves or conservatively keep the parent
storage live; it must not generate two independently destroyed owners for one
buffer.

## Self-host evidence census

The Core 0.3 twelve-module compiler contains:

| Operation or shape | Count |
| --- | ---: |
| Actual `vec.new` calls | 61 |
| Actual zero-copy `bytes.freeze` calls | 32 |
| Actual total `io.read-file` calls | 15 |
| Vector-valued record fields | 8 |
| Functions returning vectors | 3 |
| Functions returning `Bytes` | 11 |
| Exclusive vector parameters | 408 |
| Actual arena operations | 0 |

The compiler therefore exercises local builders, returned storage, immutable
views, aggregates owning multiple buffers, and mutation through caller-owned
outputs. It does not justify reference counting, explicit lifetime parameters,
user-written regions, or user-defined destructors.

## Planned representation

The typed plan uses four concepts:

1. A storage value is `none`, `view`, or `owned`. Scalars and typed IDs are
   `none`; `Bytes` is a `view`; vectors and arenas are `owned`; records and
   variants derive the join of their members.
2. An escape destination is local, result, or one numbered `inout` output.
   Function summaries contain only these bounded destinations.
3. A liveness endpoint is a source span identifying the final required use of
   a binding or disjoint owned field.
4. A destruction plan maps each owned allocation to the narrowest lexical
   region that contains every liveness endpoint and escape destination.

Analysis is conservative. An unknown call or recursive component may lengthen
a lifetime to the caller or process region, but may never shorten it. Planning
is deterministic and approximately linear in the declaration body plus its
bounded call-summary edges.

`Bytes` never owns through copying. A string literal and process argument view
the process region. `bytes.freeze` transfers the vector buffer to the smallest
region containing all uses of the resulting views. Copying a view copies only
its data and length; all copies share the proved backing-region lifetime.

Vectors and arenas retain unique mutable access. Moving them or an aggregate
containing them transfers the plan identity. Projecting an owned field performs
a disjoint field move recorded by the checker. Overwriting a live owned binding
ends the old value immediately before installing the new value. Branch joins
destroy a value only after every reachable branch has ended its use.

## Runtime boundary

Generated C uses nested `SlimRegion` values. Each dynamic block belongs to
exactly one region, and a region is destroyed in reverse allocation order at
its planned boundary. Reallocation addresses the block through its owning
region metadata rather than searching a process-global list. Storage escaping
through a result or `inout` output is allocated in or transferred to the
destination region selected by the compiler.

No allocation means no allocation record. Regions do not expose pointers or
handles to SLIM source and cannot be selected dynamically by a program.

## Allocation failure boundary

Core 0.3 traps on allocation exhaustion, including inside operations whose
source signature otherwise has no result channel. Core 0.4 must replace this
with one explicit typed failure model, testable through deterministic fault
injection. It may not add `try-` aliases while retaining trapping operations,
use a sentinel typed ID, or silently omit a vector/arena insertion. The exact
replacement is gated on propagation evidence from migrating the self-hosted
compiler and receives its own decision record before runtime behavior changes.

## Acceptance

Core 0.4 is complete only when:

- stage 0 and the self-hosted compiler emit the same canonical storage plans;
- positive and adversarial ownership/lifetime fixtures have exact diagnostic
  and native parity;
- generated C performs deterministic region destruction with no global
  allocation registry;
- fault-injected allocation exhaustion follows the accepted typed model;
- ASan, UBSan, and leak checks pass the bounded runtime corpus;
- clean, incremental, project, cache, and fixed-point gates pass together;
- planning remains approximately linear on geometric inputs; and
- the complete compiler builds itself without lifetime annotations, reference
  counting, a garbage collector, or production-Rust budget growth.
