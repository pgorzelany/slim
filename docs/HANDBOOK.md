# The SLIM Handbook

The Handbook is the single progressive path through the complete implemented
SLIM 0.9 programming model. Read Foundations in order, continue into Systems
programming when ownership and host behavior matter, and use Advanced use and
the appendices as lookup material.

`docs/CORE.md` owns the normative language definition. Focused contracts own
compiler and runtime guarantees. The Handbook explains how those rules compose
without copying their complete tables or historical evidence.

## Foundations

1. [Getting started](book/handbook/01-getting-started.md)
2. [Modules, source layout, and declarations](book/handbook/02-modules-source-and-declarations.md)
3. [Values, types, and operators](book/handbook/03-values-types-and-operators.md)
4. [Bindings, evaluation, and mutation](book/handbook/04-bindings-evaluation-and-mutation.md)
5. [Functions, effects, and control flow](book/handbook/05-functions-effects-and-control-flow.md)
6. [Structs, enums, and patterns](book/handbook/06-structs-enums-and-patterns.md)

## Systems programming

7. [Ownership, borrowing, and memory](book/handbook/07-ownership-borrowing-and-memory.md)
8. [Storage and allocation](book/handbook/08-storage-and-allocation.md)
9. [Failure and host services](book/handbook/09-failure-and-host-services.md)
10. [Projects, imports, and interfaces](book/handbook/10-projects-imports-and-interfaces.md)

## Advanced use

11. [Deterministic parallelism](book/handbook/11-deterministic-parallelism.md)
12. [Compiler tools and evidence](book/handbook/12-compiler-tools-and-evidence.md)

## Examples

The [algorithm gallery](../benchmarks/README.md) publishes all twenty
comparative programs. Six walkthroughs explain representative algorithms in
detail:

- [Binary search](algorithms/binary-search.md)
- [Merge sort](algorithms/merge-sort.md)
- [Breadth-first search](algorithms/breadth-first-search.md)
- [Edit distance](algorithms/edit-distance.md)
- [N-Queens](algorithms/n-queens.md)
- [Game of Life](algorithms/game-of-life.md)

Every complete source listing comes from an executable fixture or comparative
benchmark. The website owns no second `.slim` implementation.

## Appendices

- [Canonical grammar and formatting](CORE.md)
- [Built-ins and host signatures](HOST.md)
- [Diagnostics](DIAGNOSTICS.md)
- [Compatibility](COMPATIBILITY.md)
- [SLIM 0.9 migration](MIGRATION-0.9.md)
- [Current implementation status](STATUS.md)

Website examples use `slim-fixture` or `slim-project-fixture` markers that name
rows in the conformance manifests. Generation embeds the repository source and
expectation; tests execute positive fixtures and compare exact negative
diagnostics.
