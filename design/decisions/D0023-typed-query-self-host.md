# D0023: Typed query self-host

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 70

## Need

The self-hosted compiler proves that Core can compile standalone programs and
explicit projects, but its semantic implementation still repeatedly interprets
token positions inside one large compiler module. The current differential
corpus also covers fewer project failures than the accepted project model.
Adding incremental state or persistent evidence directly to that representation
would make token layout an accidental cache interface and preserve duplicate
clean and incremental paths.

Core 0.3 completes through one typed, declaration-oriented query pipeline
implemented in SLIM. Clean compilation, incremental updates, project checking,
interface construction, and code generation use the same typed declarations,
stable identities, dependency edges, and deterministic fragments.

## Alternatives

Porting only the two deferred incremental and cache fixtures would improve the
reported parity count without demonstrating the complete project semantics.
Keeping the compiler monolithic would make ownership and dependency boundaries
implicit. Rewriting the compiler in one step would make differential failures
difficult to localize. Adding macros, generics, reflection, filesystem handles,
or language-level concurrency would expand Core before ordinary records,
variants, arenas, vectors, functions, and explicit projects have been shown
insufficient.

The compiler is therefore migrated in verified slices. A temporary old/new
implementation may exist only within one slice and is removed before that slice
is accepted; it never becomes two permanent ways to compile a construct.

## Costs

The compiler gains explicit syntax, typed IR, diagnostic, semantic, project,
query, cache, code-generation, and driver modules. Typed arena nodes and cached
declaration fragments retain more memory than the current streaming token
walk. Incremental sessions require stable fingerprints, reverse dependency
edges, source-relative spans, transactional last-good state, and exact work
accounting. Persistent reuse requires strict bounded decoding and corruption
tests.

Full C emission remains proportional to emitted bytes. Millisecond feedback is
claimed for checking and local incremental updates only when measurements
support it, not for writing an arbitrarily large native translation unit.

Rust may continue to perform operating-system transport and test orchestration,
but it must not decide name resolution, types, effects, ownership, invalidation,
cache validity, or result ordering. Production Rust remains within the D0022
ceilings.

## Evidence

Every accepted standalone and project rule is mapped to an external fixture.
The SLIM compiler must match stage 0 diagnostics and artifacts for those
fixtures without fallback. No-change updates perform zero declaration parse,
lower, check, or generation work; a private body edit performs exactly one of
each; interface changes perform work equal to the reverse dependency closure.
Failed edits preserve the last good state. Warm validated cache entries skip
the covered module work, and every tested truncation or mutation rebuilds
safely.

The compiler builds itself as an explicit acyclic SLIM project through the same
query and cache paths. Worker batches contain immutable independent inputs,
have no worker-to-worker waits or shared mutable checker state, and merge in
module identity order. One, two, and four requested workers match the serial
oracle. Clean scaling, incremental work, persistent reuse, governance,
differential conformance, native sanitizers, and the stage-2/stage-3 byte fixed
point pass together before Core 0.3 is complete.

## Removal

Replace this query model only with a smaller model that demonstrates equal or
better diagnostic fidelity, invalidation precision, deterministic artifacts,
memory safety, clean scaling, edit latency, and self-hosting evidence. Remove a
cache layer or compiler module when its responsibility becomes ordinary
composition elsewhere; do not retain compatibility aliases or duplicate
pipelines.
