# Getting started

This chapter takes a SLIM program from canonical source to checking, execution,
and deterministic C11 output using the production compiler.

## What SLIM optimizes for

SLIM is a small systems language for generated programs. It favors one
canonical spelling, strong static types, explicit effects, affine ownership,
checked operations, deterministic output, and bounded analysis over a large or
convenience-oriented surface.

The compiler accepts `.slim` source directly. There is no separately parsed
intermediate language and no semantic fallback. The normative definition is
[Core](../../CORE.md); the exact implemented boundary is
[Status](../../STATUS.md).

## Your first program

Every standalone source starts with one `module` declaration. An executable has
exactly one `main` with the canonical signature shown here. The `io` effect is
required because the body prints.

<!-- slim-fixture: scalars -->

The complete fixture prints `42` followed by a newline. Calls enclose
comma-separated arguments in parentheses. Indentation is exactly two spaces
per level; missing or trailing commas, semicolons, braces, tabs, and alternate
call spellings are rejected.

## Check, run, and emit

From the repository root:

```sh
./slimc check conformance/pass/scalars.slim
./slimc run conformance/pass/scalars.slim
./slimc emit-c conformance/pass/scalars.slim -o /tmp/scalars.c
```

`check` parses and validates without running the program. `run` builds and runs
a native artifact. `emit-c` writes deterministic portable C11. All three use
the production self-hosted compiler.

## Reading a diagnostic

Diagnostics have stable identities and exact byte spans. Human output is the
default; put `--message-format=json` before the command for newline-delimited
machine-readable records.

<!-- slim-fixture: type-mismatch -->

This program is rejected with `E0344` at the exact span recorded by the
conformance manifest. Invalid user input is a checked result, not a compiler
panic. Continue with [modules and source layout](02-modules-and-source-layout.md)
or consult the [diagnostics contract](../../DIAGNOSTICS.md).

## Deliberate omissions

Getting started does not imply a package registry, dependency resolver, REPL,
dynamic loader, or implicit host access. Projects use explicit manifests, and
host services remain effect-gated.
