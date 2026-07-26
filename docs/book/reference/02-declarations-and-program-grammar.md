# Declarations and program grammar

This chapter organizes the canonical grammar for modules, types, functions,
blocks, and standalone executable entry points.

## Compilation unit

A source file contains one module declaration followed by zero or more
module-scope declarations. The compiler accepts canonical parsed SLIM as its
sole program representation.

```text
module <module-name>
<declarations>
```

Project membership, imports, exports, and entry selection are provided by a
checked manifest rather than source-file discovery.

## Type declarations

A `record` declares an ordered list of named fields. A `variant` declares an
ordered closed set of cases and their payload types. Order is canonical and is
checked at construction and matching sites.

Malformed or unknown component types are rejected when declarations are
checked. Declarations do not create structural subtyping or implicit
conversions.

## Function declarations

A `fn` declaration contains a name, ordered parameters with exact types, a
result type, an optional canonical effects list, and an indented body. A pure
function omits the effects clause.

The body may contain bindings, structured control, visible rebinding, and the
canonical recurrence form. Its final expression produces the function result;
there is no separate `return` form. Arity and result agreement are checked for
all paths.

## Entry point

A standalone executable has exactly one
`main(args: Vec[Bytes]) -> I64`. Its effects are declared normally. Element
zero is the executable path; remaining values are process arguments. A
project's entry module is named by the manifest, and the project contract
rejects missing or non-entry `main` definitions.

## Blocks

Blocks are introduced only by grammar forms that require an indented body.
Each child line is exactly one indentation level deeper. Empty, skipped, or
malformed block structure is rejected, not repaired through hidden statements.

## Canonicality

Calls use parentheses with whitespace-separated arguments. Fields, cases,
effect lists, manifest modules, and pattern arms use their required canonical
order. The grammar has one spelling for each semantic operation.

## Normative boundary

The exact productions and accepted built-ins are in [Core](../../CORE.md).
Project-level grammar is in [Projects](../../PROJECTS.md).
