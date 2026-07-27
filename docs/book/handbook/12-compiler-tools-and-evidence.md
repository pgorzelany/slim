# Compiler tools and evidence

SLIM's compiler tools operate on the same checked canonical program and report
stable, bounded evidence without creating another executable language.

## Diagnostic output

Every diagnostic has a stable code, severity, primary byte span, and optional
labels, notes, and fixes. Human and newline-delimited JSON output contain the
same facts.

```sh
./slimc --message-format=json check conformance/fail/multiple.slim
```

Recovery preserves independent diagnostics where possible:

<!-- slim-fixture: multiple -->

## Analysis

`analyze` reports independent quality and cost facts:

```sh
./slimc analyze examples/countdown.slim
```

Each semantic claim is exact, bounded with its fixed budget, or unknown with a
stable reason. Missing evidence is not a negative score.

## Reduction and proof

```sh
./slimc reduce conformance/pass/reduction.slim
./slimc prove-reduction conformance/pass/reduction.slim
./slimc verify-reduction conformance/pass/reduction.slim /tmp/reduced.slim
```

Direct reduction is terminating, deterministic, idempotent, and
behavior-preserving. Verification recomputes the accepted result from checked
source instead of trusting proof text.

## Equivalence and structural edits

`equivalent` is exact only over the complete finite domain named in its report.
An accepted mismatch contains the first deterministic counterexample. `edit`
accepts a versioned, bounded, non-executable patch addressing one stable
canonical node, then publishes output only after the normal checker accepts it.

## Derived artifacts

Formatted source, C11 output, interfaces, caches, analysis reports, proofs, and
patches remain derived data. Canonical parsed `.slim` is the sole accepted
program representation.

## Exact rules and common errors

There is no language server protocol promise, debugger, profiler, REPL, package
manager, executable proof language, or agent-only semantic bypass. Tool
contracts are detailed in [Quality](../../QUALITY.md),
[Reduction](../../REDUCTION.md), and [Diagnostics](../../DIAGNOSTICS.md).

Reports must distinguish exact, bounded, and unknown facts. Proof text is never
trusted as executable authority, and derived artifacts never replace checked
source.

## Next

Browse the [algorithm gallery](../../../benchmarks/README.md) or use the
[Core appendix](../../CORE.md) for exact syntax and semantics.
