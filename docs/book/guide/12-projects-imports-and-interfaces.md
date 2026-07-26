# Projects, imports, exports, and interfaces

Multi-module programs use one explicit manifest that fixes module identity,
source paths, imports, exports, visibility, and the executable entry.

## The project manifest

A project is checked through its `slim.project` file:

```sh
./slimc check conformance/projects/basic/slim.project
./slimc run conformance/projects/basic/slim.project
```

The manifest version, module ordering, paths, declared module names, and entry
are checked. Paths remain confined to the project.

<!-- slim-project-fixture: project-run -->

## Imports and qualification

Dependencies are explicit and direct. Cross-module references use the
qualified name required by the project contract. A module may not reach a
transitive dependency it did not import, qualify itself as though it were
external, or import an unknown module.

<!-- slim-project-fixture: project-unknown-import -->

## Exports and visibility

Only declared exports form the public module interface. Private declarations
cannot be referenced from another module, and public types must be closed over
publicly visible component types.

<!-- slim-project-fixture: project-private -->

## Derived interfaces and caches

`./slimc interfaces PROJECT -o DIRECTORY` emits deterministic checked
interfaces. Incremental caches and interfaces are derived artifacts; corrupt or
stale data is rejected or recomputed. Neither is accepted source.

## Compiler guarantees

Checking is deterministic across relocation and worker counts. Incremental work
is local to a changed declaration and its dependents. All conformance rows run
through the production SLIM compiler.

## Deliberate omissions

There is no package registry, dependency solver, implicit module search,
wildcard import, re-export alias, dynamic loading, or alternate project
language. See [Projects](../../PROJECTS.md).
