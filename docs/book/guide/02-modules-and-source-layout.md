# Modules, source layout, and canonical formatting

SLIM source has one stable textual form so generators, reviewers, formatters,
and compiler caches agree on the same program representation.

## Module identity

A standalone file begins with one module name:

```slim
module hello
```

In a project, the manifest maps that declared identity to a repository-confined
source path. Source identity, import identity, and manifest identity must agree;
the compiler does not infer modules from directory names.

## Significant indentation

Blocks use exactly two ASCII spaces for each indentation level. Tabs, odd
indentation, and skipped levels are separate errors.

<!-- slim-fixture: tab-indentation -->

The malformed fixture is rejected with `E0103`. The conformance corpus also
fixes `E0104` for odd indentation and `E0105` for skipped levels.

Canonical punctuation follows the same rule: parenthesized lists require
commas, while braces and semicolons are rejected.

<!-- slim-fixture: missing-comma -->

## Formatting

`./slimc format PATH` emits canonical source deterministically. Formatting
changes layout, not semantics, and formatted output must check again. There is
no configuration file for alternate indentation or punctuation styles.

The repository fixture below is the permanent formatter contract:

<!-- slim-fixture: format -->

## Source organization

Declarations occur in source order beneath the module line. Put type
declarations and functions at module scope. Imports and exports belong to a
project manifest rather than hidden source discovery. See
[projects and interfaces](12-projects-imports-and-interfaces.md) and the
[project contract](../../PROJECTS.md).

## Compiler guarantees

Canonical parsing is deterministic. Diagnostics use byte spans into the exact
input. Derived C, interfaces, caches, proofs, and reports never become alternate
accepted source forms.

## Deliberate omissions

SLIM has no preprocessor, conditional source syntax, alternate formatter style,
or implicit module search path.
