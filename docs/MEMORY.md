# Core 0.4 memory model and audit

Status: implemented Core 0.4 boundary

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

## Typed representation

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

`selfhost/memory.slim` owns `ValuePlan`, `AllocationPlan`,
`DestructionPlan`, `FunctionPlan`, and `Plan`. The checker executes the plan
query after creating its one structured declaration index, and the code
generator consumes the same storage classifier and function summary. There is
no Rust-owned lifetime IR and no source annotation.

Analysis is conservative. An unknown call or recursive component may lengthen
a lifetime to the caller or process region, but may never shorten it. Planning
is deterministic and approximately linear in the declaration body plus its
bounded call-summary edges.

Detailed named-value liveness and escape scanning is capped at 64 values per
function. Up to that bound, the planner records the last named use and result
or `inout` escape. Above it, remaining endpoints become the function boundary
and storage escapes use the function summary. The fallback loses release
precision but cannot shorten a lifetime; the plan records whether detailed
liveness was precise. This makes the repeated scan a fixed-factor pass instead
of an unbounded quadratic default.

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

Core 0.4 implements the first conservative placement boundary at function
granularity. A function whose result contains no storage and which has no
`inout` output receives a child region; all transitive allocation in that
function uses the child and generated code destroys it on normal and
allocation-failure exits. Other functions allocate in their caller-provided
destination region. This safely covers returned vectors and byte views,
aggregate ownership, recursion, and output mutation without promotion or
reference counting. Per-binding early destruction and stack promotion of
dynamic buffers remain later precision work; ordinary scalar and aggregate C
values are already stack-resident.

No allocation means no allocation record. Regions do not expose pointers or
handles to SLIM source and cannot be selected dynamically by a program.

## Allocation failure boundary

Core 0.4 replaces allocation traps with D0026's single typed allocation-effect
channel. Every region points to a `SlimAllocStatus`; allocation and growth
either commit atomically or set it to `exhausted`. Generated branches propagate
that outcome through functions declaring `alloc`, destroy active local regions,
and handle it once at the entry boundary with exit code 71. There is no
fallible alias, sentinel ID, exception, or second allocator.

`SLIM_ALLOC_FAIL_AT=N` selects a positive allocation ordinal for deterministic
runtime testing. It is an internal executable test boundary, not language
surface. The differential corpus injects failure through both compiler stages,
and the sanitizer gate exercises every allocation ordinal of its bounded
vector program.

## Acceptance

Core 0.4 is complete only when:

- the self-hosted compiler owns canonical typed storage plans and stage 0
  preserves the same runtime ABI and diagnostics;
- positive and adversarial ownership/lifetime fixtures have exact diagnostic
  and native parity;
- generated C performs deterministic function-region destruction with no global
  allocation registry;
- fault-injected allocation exhaustion follows the accepted typed model;
- ASan, UBSan, and leak checks pass the bounded runtime corpus;
- clean, incremental, project, cache, and fixed-point gates pass together;
- detailed planning is bounded to 64 values and scaling remains approximately
  linear on geometric inputs; and
- the complete compiler builds itself without lifetime annotations, reference
  counting, a garbage collector, or production-Rust budget growth.
