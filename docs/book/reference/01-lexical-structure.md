# Lexical structure

This chapter specifies the accepted characters, tokens, indentation, comments,
literals, names, and punctuation of canonical SLIM source.

## Source text

Source is interpreted as bytes with diagnostics reported in byte offsets.
Canonical line endings and formatter output are deterministic. A file begins
with a module declaration and continues with module-scope declarations.

## Names and reserved forms

Module, declaration, parameter, field, and local names use the identifier form
accepted by the grammar in [Core](../../CORE.md). Keywords and built-in names
occupy their documented roles; unknown callee names are diagnosed rather than
dynamically resolved.

Qualified project references use the single syntax specified by
[Projects](../../PROJECTS.md). Standalone source cannot invent a project
qualification context.

## Literals

The scalar literal forms are Boolean values, signed integer values within the
`I64` domain, byte-compatible values where a `U8` is required, and byte-string
syntax accepted by the core grammar. Literal checking never performs an
implicit numeric conversion.

An unterminated string is `E0107` with an exact source span.

## Whitespace and indentation

ASCII space separates forms and operands. Each block level adds exactly two
spaces. Tabs are `E0103`, odd indentation is `E0104`, and skipped indentation
levels are `E0105`.

Indentation defines structure; braces do not. Commas are required between
parenthesized or bracketed list elements. Missing commas are `E0108`; leading,
doubled, and trailing commas are `E0109`. Braces and semicolons are rejected
under `E0106`.

## Comments and blank lines

Comments and blank lines do not create declarations or block levels. The
formatter preserves the one canonical program structure while producing stable
layout.

## Lexical diagnostics

Malformed tokens, strings, punctuation, and indentation are rejected before
semantic checking. Recovery may continue to report independent errors, but
missing source never becomes an inferred construct. See
[Diagnostics](../../DIAGNOSTICS.md) for ordering and JSON representation.

## Normative boundary

[Core](../../CORE.md) is normative. This chapter reorganizes its lexical rules
for lookup and does not introduce alternate source forms.
