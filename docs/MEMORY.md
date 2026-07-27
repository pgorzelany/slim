# SLIM memory model

Status: SLIM 0.9 implementation, stabilized through Core 1E

SLIM uses statically checked ownership and compiler-selected allocation regions.
It has no tracing garbage collector, reference counting, user-visible
allocator, `free`, destructor, or raw pointer in safe source. The durable
architecture decision is D0025.

This document distinguishes the source-level lifetime rules from the current
physical release precision. A value can become unusable at its last valid
source operation before the runtime releases its backing allocation.

## Programmer model

There are three storage categories:

| Category | Examples | Source behavior | Runtime representation |
| --- | --- | --- | --- |
| Copyable value | `Bool`, `U8`, `I64`, `Id[T]` | Assignment and by-value calls copy the value. | An inline scalar or small identity. |
| Copyable view | `Bytes` | Copying duplicates the view, not its bytes. The compiler keeps the backing storage alive for every copy. | A data pointer and length with no ownership exposed to source. |
| Affine owner | `Vec[T]`, `Arena[T]` | Assignment and by-value calls move ownership. The old binding cannot be used afterward. | A small handle referring to uniquely owned region storage. |

Structs and enums derive their category from their members. An aggregate
containing an affine owner is affine. An aggregate containing only copyable
values and views is copyable. `Void` is not a storable value category.

Affine means "usable as an owner at most once," not "destroyed at the next
call." A move transfers the right to use and eventually release storage. It
does not copy the buffer, allocate another buffer, or necessarily free
anything at the move site.

SLIM therefore does not have one uniform "everything is a reference" model,
nor a simple source-level split between reference and value types. Scalars and
copyable aggregates are ordinary values. `Bytes` is a copyable immutable view.
Vectors and arenas are affine owner values. Safe source cannot inspect the
pointers used by the C representation.

## Bindings are separate from ownership

`let` and `var` control whether a name can be directly rebound in its lexical
scope:

- `let value: T = ...` rejects a later `value = ...`;
- `var value: T = ...` permits a later assignment of the same exact type; and
- neither spelling changes whether `T` is copyable, a view, or affine.

`let` is not a compile-time constant and does not imply deep immutability. A
`let` binding can name the unique owner of a vector whose elements are changed
through vector operations or an `inout` call. Passing a named binding to an
`inout` parameter explicitly authorizes that callee to mutate through the
borrow. `var` is required for direct rebinding in the current lexical scope.

## Owned calls and borrowed calls

A plain parameter receives its argument by value. Copyable arguments copy;
affine arguments move. An owned affine parameter may store, return, forward,
or discard its value.

An `inout` parameter instead receives temporary exclusive access to one named
caller binding. The call syntax is ordinary; `inout` appears in the callee's
declaration. During the call:

- no second owner is created and ownership remains with the caller;
- the callee may inspect or mutate the borrowed value;
- the callee may not move, retain, or return the borrow;
- a temporary cannot be borrowed; and
- two `inout` arguments in the same call may not alias.

The borrow ends when the call returns, after which the caller may use its
binding again. `inout` is a checked call relationship, not a storable reference
type, and it introduces no reference-count operation or allocation.

## When allocation happens

Ordinary scalar operations, struct and enum construction, moves, borrows,
field projection, matching, and copying a `Bytes` view do not allocate dynamic
storage by themselves. Ordinary scalar and aggregate C values are normally
stack-resident or held in registers.

Dynamic allocation occurs for storage operations that need backing memory:

| Operation | Allocation behavior |
| --- | --- |
| `vec.new()` / `arena.new()` | Creates an empty region-associated owner handle. No element buffer exists until storage grows. |
| `vec.push`, `arena.add`, and output growth | Allocate or reallocate when existing capacity is insufficient. |
| File and network output | May grow the supplied `inout Vec[U8]`; host implementations may also use short-lived scratch storage. |
| `bytes.freeze(vector)` | Consumes the unique `Vec[U8]` and creates a `Bytes` view of the same buffer without copying or allocating a second buffer. |
| String literals and process arguments | Create `Bytes` views over storage that already exists in the process region. |

The `alloc` effect is permission for an operation or transitive callee to
allocate and report exhaustion. It is a checked capability ceiling, not proof
that every execution obtains a dynamic block. Allocation and vector growth
commit atomically or report the single typed exhaustion outcome.

## When storage is released

Every dynamic allocation belongs to exactly one runtime region selected by the
compiler. The current implementation releases memory at these boundaries:

- A function that may allocate, returns no storage or view, and has no `inout`
  output receives a child region. That region is destroyed on normal return and
  on allocation-failure exit.
- A function that returns storage or a view, or mutates through `inout`,
  allocates into a caller-provided destination region. That storage survives
  the call and is released with the destination region.
- Allocation-free functions do not create an empty child region.
- Successful vector reallocation releases the replaced buffer after its
  contents have been copied into the new block.
- Short-lived runtime scratch regions are destroyed at the end of their host
  operation.
- The root region is destroyed at program shutdown. Runtime trap cleanup walks
  the active region chain before exiting.

Destroying a region frees all its remaining blocks in reverse allocation
order. Ownership moves do not need to search a heap or update reference
counts, because each block continues to have exactly one region.

Current physical release precision is conservative and primarily
function-region-based. The checker records last uses and escape destinations,
but per-binding early destruction is not yet implemented for ordinary dynamic
buffers. Consequently, overwriting or finishing the last use of an owner ends
that source-level value immediately, while its backing block may remain until
the containing region ends. This can retain memory longer than the narrowest
theoretical lifetime, but it cannot release memory too early.

One useful mental timeline is:

```text
caller region
  ├─ call local helper
  │    ├─ allocate in child region
  │    └─ return: destroy child region
  ├─ call builder returning Vec or Bytes
  │    ├─ allocate in caller destination region
  │    └─ return: result remains valid
  └─ caller region ends: destroy retained allocations
```

## Historical Core 0.3 baseline

Before Core 0.4, the source checker already established these rules:

- scalar values, typed IDs, immutable `Bytes`, and aggregates containing only
  copyable fields may be copied;
- vectors, arenas, and aggregates containing either are affine and move;
- an `inout` parameter is an exclusive, non-escaping lexical borrow;
- moving an owned binding makes subsequent use or mutation an error;
- `vec.get` and `arena.get` only return copyable elements; and
- `bytes.freeze` consumes one unique `Vec[U8]` and returns a copyable immutable
  view without copying its buffer.

The runtime did not yet exploit those facts. It placed all allocations in one
global linked list, searched the list during reallocation, and released the
list only at process shutdown. This was memory-safe for accepted programs but
was not compile-time lifetime planning or narrow deterministic release.

There was also an ownership projection obligation that global retention
masked: project sessions extracted multiple vector fields from an affine
`State` struct. Core 0.4 represented disjoint field moves while conservatively
retaining their backing regions; it did not generate two independently
destroyed owners for one buffer.

## Self-host evidence census

The Core 0.3 twelve-module compiler contains:

| Operation or shape | Count |
| --- | ---: |
| Actual `vec.new` calls | 61 |
| Actual zero-copy `bytes.freeze` calls | 32 |
| Actual total `io.read_file` calls | 15 |
| Vector-valued struct fields | 8 |
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
   `none`; `Bytes` is a `view`; vectors and arenas are `owned`; structs and
   enums derive the join of their members.
2. An escape destination is local, result, or one numbered `inout` output.
   Function summaries contain only these bounded destinations.
3. A liveness endpoint is a source span identifying the final required use of
   a binding or disjoint owned field.
4. A destruction plan maps each owned allocation to the narrowest lexical
   region that contains every liveness endpoint and escape destination.

`selfhost/memory.slim` owns `ValuePlan`, `AllocationPlan`,
`DestructionPlan`, `FunctionPlan`, and `Plan`. The checker executes the plan
query after type linking and retains that exact plan in its checked artifact.
Standalone and prepared-project code generation consume the retained function
plans in declaration order and verify their function identity before selecting
a region. There is no Rust-owned lifetime IR and no source annotation.

Allocation plans include every call boundary that can propagate allocation
failure: allocating built-ins and user functions declaring `alloc`. The shared
SLIM `effects` module is the single classifier used by diagnostics and memory
planning. A function's generated failure boundary is present exactly when its
retained plan has at least one such site. User-call emission binary-searches
that sparse, lexically ordered site vector by exact call-form token, so nested
evaluation order does not require a second effect read or a sequential cursor.
The backend does not reparse function or callee effect declarations.

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
granularity. A function whose result contains no storage, which has no `inout`
output, and whose retained plan contains direct or transitive allocation uses a
child region. Generated code destroys that child on normal and
allocation-failure exits. If the plan contains no allocation site, D0060 elides
the empty child and forwards the caller region through the uniform ABI. Other
functions allocate in their caller-provided destination region. This safely
covers returned vectors and byte views, aggregate ownership, recursion, and
output mutation without promotion or reference counting. Per-binding early
destruction and stack promotion of dynamic buffers remain later precision
work; ordinary scalar and aggregate C values are already stack-resident.

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
