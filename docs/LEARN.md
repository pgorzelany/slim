# Learn SLIM

The SLIM language book is a chaptered path through the complete implemented
programming model, generated and tested from the same repository commit as the
compiler.

`docs/CORE.md` remains the normative language definition. The guide explains
how the rules compose, the language reference organizes them for precise
lookup, and the focused contracts retain their authority for memory, projects,
diagnostics, host services, analysis, performance, parallel execution, and
compatibility.

## Guide

1. [Getting started](book/guide/01-getting-started.md)
2. [Modules, source layout, and canonical formatting](book/guide/02-modules-and-source-layout.md)
3. [Scalar values and checked operations](book/guide/03-scalar-values-and-checked-operations.md)
4. [Bindings, evaluation, and mutation](book/guide/04-bindings-evaluation-and-mutation.md)
5. [Functions, entry points, and effects](book/guide/05-functions-entry-and-effects.md)
6. [Control flow and `recur`](book/guide/06-control-flow-and-recur.md)
7. [Structs, enums, and patterns](book/guide/07-structs-enums-and-patterns.md)
8. [Bytes, vectors, arenas, and IDs](book/guide/08-bytes-vectors-arenas-and-ids.md)
9. [Ownership, moves, borrowing, and lifetimes](book/guide/09-ownership-moves-and-borrowing.md)
10. [Traps, failure, and allocation exhaustion](book/guide/10-traps-failure-and-allocation-exhaustion.md)
11. [Host I/O](book/guide/11-host-io.md)
12. [Projects, imports, exports, and interfaces](book/guide/12-projects-imports-and-interfaces.md)
13. [Deterministic parallelism](book/guide/13-deterministic-parallelism.md)
14. [Diagnostics and compiler tools](book/guide/14-diagnostics-and-compiler-tools.md)

## Language reference

1. [Lexical structure](book/reference/01-lexical-structure.md)
2. [Declarations and program grammar](book/reference/02-declarations-and-program-grammar.md)
3. [Types and value categories](book/reference/03-types-and-value-categories.md)
4. [Expressions and evaluation order](book/reference/04-expressions-and-evaluation-order.md)
5. [Patterns and exhaustiveness](book/reference/05-patterns-and-exhaustiveness.md)
6. [Effects, termination, and failure](book/reference/06-effects-termination-and-failure.md)
7. [Ownership, borrowing, storage, and lifetimes](book/reference/07-ownership-storage-and-lifetimes.md)
8. [Built-ins and host signatures](book/reference/08-builtins-and-host-signatures.md)
9. [Executables, projects, qualification, and visibility](book/reference/09-executables-projects-and-visibility.md)
10. [Diagnostics, tooling, and derived artifacts](book/reference/10-diagnostics-tools-and-derived-artifacts.md)

## Canonical contracts

- [Core language](CORE.md)
- [Compatibility](COMPATIBILITY.md)
- [Diagnostics](DIAGNOSTICS.md)
- [Memory](MEMORY.md)
- [Projects](PROJECTS.md)
- [Quality evidence](QUALITY.md)
- [Direct reduction](REDUCTION.md)
- [Performance](PERFORMANCE.md)
- [Parallelism](PARALLELISM.md)
- [Resource bounds](RESOURCE_BOUNDS.md)
- [Host services](HOST.md)
- [Release](RELEASE.md)

Website examples use `slim-fixture` or `slim-project-fixture` markers that name
rows in the conformance manifests. Generation embeds the repository source and
expectation; tests execute positive fixtures and compare exact negative
diagnostics. The website never owns a second `.slim` program.
