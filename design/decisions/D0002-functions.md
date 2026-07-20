# D0002: Functions, application, and lexical binding

Status: accepted
Kind: language
Primitive: function-application
Safety: 1
Compile: 2
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

Functions and lexical binding are the universal composition mechanism required
to implement the compiler and ordinary programs.

## Alternatives

Methods, operators, pipelines, and implicit calls duplicate application.

## Costs

Calls require type and effect checking and may require backend stack frames.

## Evidence

All compiler behavior can be represented through named functions and explicit
calls; tail recurrence removes loop-specific syntax.

## Removal

This is foundational and can only be replaced with a smaller universal
composition mechanism.
