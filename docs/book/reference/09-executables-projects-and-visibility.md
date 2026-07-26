# Executables, projects, qualification, and visibility

This chapter specifies standalone entry points and the manifest-controlled
identity, dependency, visibility, and interface rules for multi-module
programs.

## Standalone executable

A standalone source has one module and one canonical
`main(args: Vec[Bytes]) -> I64`. Element zero is the executable path and later
elements are process arguments. Effects are declared by the function in the
ordinary way.

## Project manifest

A `slim.project` file declares its schema version, ordered modules, repository-
confined source paths, direct imports, exports, and one entry module. The
manifest is the sole project source form.

Module declarations in source must agree with manifest identities. Duplicate,
unsorted, missing, escaping, or mismatched paths and identities are rejected.

## Direct imports

A module may refer across a boundary only through a declared direct import and
the canonical qualified reference form. Transitive dependency reach-through,
self-import, self-qualification, and unknown imports are errors.

## Exports and visibility

Only explicitly exported declarations are visible across modules. Private
declarations cannot be referenced externally. Public signatures may not leak a
private component type.

## Project entry

The manifest's entry module must define the canonical `main`. A non-entry
module may not define another entry. Entry selection is never inferred from
filenames or scan order.

## Interfaces

The compiler can emit deterministic checked module interfaces. Interfaces and
incremental caches are derived artifacts and are validated against canonical
source. They are never parsed as an alternate program.

## Determinism and locality

Project results are invariant under repository relocation and worker-count
changes. Incremental checking is local to a changed declaration and its
dependents. Cycles and incomplete graph evidence remain explicit.

## Normative boundary

The exact manifest schema, diagnostics, cache rules, and interface format are
specified by [Projects](../../PROJECTS.md).
