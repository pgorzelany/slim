# Diagnostics, tooling, and derived artifacts

This chapter specifies stable diagnostic records and the boundary between
canonical source and compiler-generated reports, proofs, edits, interfaces,
caches, and C.

## Diagnostic record

Every diagnostic has a stable code, severity, primary byte span, and optional
labels, notes, and fixes. Human output and newline-delimited JSON expose the
same facts. Ordering is deterministic.

Put `--message-format=json` before a diagnostic-producing command. Invalid user
input must produce checked diagnostics rather than panic the compiler.

## Core compiler commands

`check`, `run`, `build`, and `emit-c` validate canonical source through the
production compiler. `format` emits canonical layout. `builtins` reports the
accepted built-in inventory. `runtime` emits maintained runtime support.

## Analysis reports

`analyze` reports independent facts with exact, explicitly bounded, or stable
unknown classifications. It does not collapse missing evidence into a negative
quality score or infer application invariants from representable state.

## Reduction records

`reduce` performs the accepted bounded set of direct canonical-source rewrites.
It terminates, is deterministic and idempotent, and preserves observable
behavior. `prove-reduction` records explanatory evidence;
`verify-reduction` recomputes the result from checked source.

## Equivalence reports

`equivalent` is exact only over the complete finite domain named in the report.
A supported mismatch contains the first deterministic counterexample.
Unsupported or exhausted domains remain explicit rather than becoming false
equivalence claims.

## Agent edits

An agent patch is versioned, bounded, non-executable tooling data addressing one
stable canonical node. The ordinary checker is the sole authority that accepts
and publishes the resulting program.

## Other derived artifacts

Generated C11, interfaces, caches, performance records, analysis reports,
proofs, and patches are derived data. None is a separately parsed executable
language. Clean compilation accepts only canonical parsed SLIM.

## Normative boundary

See [Diagnostics](../../DIAGNOSTICS.md), [Quality](../../QUALITY.md),
[Reduction](../../REDUCTION.md), [Performance](../../PERFORMANCE.md), and
[Compatibility](../../COMPATIBILITY.md).
