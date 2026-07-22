# D0025: Compile-time storage plans

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 80

## Need

Core 0 enforces affine aggregate moves and non-escaping exclusive `inout`
borrows, but its C runtime retains every dynamic allocation in one process-wide
list until shutdown. Reallocation searches that list, storage is not released
at its last statically known lifetime boundary, and the compiler records no
machine-checkable explanation of why a frozen byte view remains valid.

The Core 0.3 compiler is the evidence corpus. It contains 61 actual `vec.new`
operations, 32 zero-copy `bytes.freeze` transfers, 15 total file reads, eight
vector-valued record fields, three vector-returning functions, eleven
byte-returning functions, and 408 exclusive vector parameters. It uses no
actual arena allocation. This requires aggregate ownership, returned storage,
storage placed into output collections, and freely copied immutable byte views;
it does not demonstrate a need for source-level lifetime or region syntax.

Core 0.4 therefore adds one compiler analysis and runtime strategy, not a
language operation. Every allocation receives a deterministic storage plan.
Lexical liveness and escape summaries select the narrowest region proven to
outlive every view or owned destination. The generated program destroys regions
at their statically selected boundary.

## Alternatives

Per-value reference counting would make copying `Bytes` perform hidden atomic
or non-atomic ownership work and would impose runtime management on programs
whose lifetimes are statically evident. Tracing garbage collection conflicts
with the runtime goals. Making `Bytes` affine would force pervasive ownership
plumbing through the compiler even though immutable views are safe to copy when
their backing region is known. Exposing lifetime parameters, region handles,
raw pointers, explicit `drop`, or two allocation APIs would expand the language
before inference has failed on the complete compiler.

Retaining one process region is safe but does not implement narrow deterministic
release. Inserting individual `free` calls from lexical scope alone is unsafe:
storage can escape through returns, frozen byte views, records, variants,
vectors, arenas, and `inout` outputs. An unconstrained interprocedural fixed
point would also violate the default approximately-linear compilation rule.

## Costs

The typed compiler IR gains storage classes, escape edges, liveness endpoints,
and destruction plans. Functions publish bounded summaries for returned values
and `inout` outputs. Recursive call components use a conservative escaping
summary rather than an unbounded analysis. Conservative plans retain storage in
an outer region; they never shorten a lifetime without proof.

The runtime gains explicit nested region records. Allocation metadata is local
to its owning region and destruction walks that region in reverse allocation
order. Generated internal function ABIs may carry hidden region destinations;
this is backend detail and adds no source token or semantic alternative. A
program that performs no dynamic allocation creates no allocation records.

Core 0.4 plans memory lifetime and release but does not introduce user-defined
destructors or optimize unrelated arithmetic, bounds checks, or C emission.
Typed allocation exhaustion is specified separately because failure propagation
is a semantic question, not a reason to duplicate allocation operations.

## Evidence

The compiler emits a deterministic, inspectable storage plan for every
function. Positive fixtures cover local release, returned vectors, frozen byte
views, nested aggregate ownership, branch joins, recurrence, and mutation.
Negative fixtures cover escaping exclusive borrows, invalid aliases, use after
move, and ownership projections whose lifetime cannot be established. Runtime
tests inject allocation failure and sanitizers reject use-after-free,
double-free, leaks in bounded fixtures, and invalid reallocation.

The stage-0 and self-hosted planners must agree on diagnostics and canonical
plans. Clean and incremental compilation use the same plan query; a body edit
invalidates only the declaration and consumers of an interface-level ownership
summary change. The twelve-module compiler must rebuild itself, pass the full
differential corpus, and retain a stage-2/stage-3 byte fixed point. Scaling
measurements must remain approximately linear.

## Removal

Replace region planning only with a smaller ownership strategy that proves the
same view lifetimes, deterministic release, bounded compiler work, zero hidden
copying, and self-host behavior. Remove a stored plan field when it is derivable
locally without repeated analysis. Do not retain a compatibility allocator or
expose explicit lifetime syntax alongside successful inference.
