# RFC-0122: Shadowed binding lowering

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 2
Compile: -1
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 10

## Summary

Preserve checked lexical binding identity in generated C names for every local, parameter, and
enum payload. Restore accepted initializer behavior.

## Motivation

The M0 shadowed-initializer witness returns 2 instead of 42. The checker links
its inner initializer to the outer value 40, but generation declares the inner
zero-initialized C variable with the same spelling before evaluating 40 + 2.
Payload extraction and parallel captures also need the checked identity rather
than whichever C declaration happens to be nearest.

## Guide-level explanation

A binding's initializer sees the enclosing scope. Its body sees the newly
introduced binding. An enum arm introduces its payload bindings in that arm.
Generated code must preserve these existing rules, including mutable places,
shared and exclusive borrows, owning transfers, and recurrence arguments.

## Reference-level specification

Use the checked source use-to-declaration links already retained by the lexical
linker. When emitting a source variable, resolve its linked declaration, or use
its own node for a declaration. Append `_n` and that declaration node to the
existing mangled name at every emission site. Source underscores are encoded as
`_95`, so the suffix cannot collide with a source spelling. Different declarations
have different canonical node identities. Do not infer lexical identity from C
scope or source spelling alone.

## Compiler and runtime design

This is a production SLIM backend repair. It adds no parser, checker, retained
binding table, production Rust semantics, or runtime operation. Every source
variable emission, including parameters, mutation, match extraction, serial and
parallel captures, and recurrence, uses the same naming function.

The local preservation argument is name substitution: every declaration and its
uses receive one consistent C identifier, distinct from all enclosing bindings.
No initializer, expression order, trap, effect, allocation, transfer, or cleanup
changes. The operation is deterministic and total on checked canonical nodes.

## Compatibility and migration

No source syntax, semantics, ABI, diagnostics, or analysis schema changes.
All generated source-variable C identifiers change. Regenerate the
portable seed and compare complete native analysis reports and generated outputs.
Historical artifacts remain associated with their original seed identities.

## Diagnostics and failure cases

Existing checker diagnostics remain authoritative. Undefined initializers,
use-after-move, conflicting borrows, and wrong types must still be rejected.
No lowering fallback accepts an unlinked source program. The original witness
becomes a production execution regression with result 42, never a wrong-result
expected failure.

## Performance and complexity

Emission performs one constant-time link lookup and writes the declaration node
suffix. No binding-table snapshots, new per-node storage, or initializer searches
are introduced. Measure same-host frontend and existing native/performance gates;
retain every budget and analysis baseline. Generated size growth is recorded.

## Alternatives and drawbacks

Renaming only shadowing variables would reduce generated-code churn, but needs
a durable marker beyond the declaration link used during checking. A first
self-link marker was overwritten by binding indexing and the witness still
returned 2; that attempt was removed. Evaluating initializers into extra temporary
storage adds generated state and needs separate treatment of enum extraction and
parallel captures. Scanning initializers can repeat work quadratically. Uniform
use of existing checked declaration identities is the smaller reliable repair.

## Test and acceptance plan

Execute scalar, owned aggregate, enum-payload, mutable/exclusive, recurrence,
and parallel shadowing cases through the production compiler. Preserve negative
and exact diagnostic cases for unsupported source. Cross nested scopes and names
that resemble generated suffixes. Compare bounded shadowed programs with manually
alpha-renamed equivalents; run ASan/UBSan and allocation-fault campaigns. Require
bootstrap fixed point, all repository per-commit gates, actual-work observations,
and unchanged complete native analysis reports.

## Ratings and evidence

Safety +2 restores a demonstrated checked-source lowering violation. Compile -1
records the larger C representation and measured emission cost: the seed grows
16.4%, and all seven same-host compiler-source emission pairs are slower (median
ratio 1.029). Other ratings are zero. This is an architecture repair, with no
language primitive or relaxed performance budget; no speedup or agent benefit
is assumed. Score 10.

## Decision

Accepted under RFC-0112's delegated implementation authority and the active M0
repair goal. No language primitive, production Rust exception, dependency,
performance-budget relaxation, or hard-gate change is authorized.

## Implementation

Implemented by the common source-variable emitter using existing checked links.
The portable fixed point is 3,516,365 C bytes, SHA-256
`a7e4b2f8de6696c27880ebaf170392c7f491b73bdd9157846bf5a9ddc05ae9e4`.
Three native positive and three exact diagnostic conformance rows pass, including
the original result-42 witness. A 32-program fixed domain compares shadowed and
alpha-renamed branches at four depths with local/parameter roots. Existing move,
borrow, counted-lowering, and reinitialization tests remain intact.

ASan/UBSan checks the compiler itself and all three positive fixtures, including
inline and POSIX worker tiers. Each application crosses 64 allocation-fault
ordinals; the compiler crosses 128 ordinals. The actual-work campaign passes in
quick ordinary and sanitizer modes. Its input-byte hooks now resolve the exact
source formal in the generated function signature, rather than guess a node ID.
Optimization/ABI test assertions erase only local suffixes; behavioral identity
is independently checked through native execution.

All 20 complete native analysis reports are byte-identical. Their generated C
changes only local identifiers; per-application size changes are recorded in
`benchmarks/results/2026-09-06-binding-identity-native-changes.tsv`. Paired frontend
and compiler-source emission samples are retained separately. Bootstrap,
governance, formatting, Clippy, all 10 unit and 59 integration tests, 331 conformance
fixtures and 2,000 malformed mutations, quick performance/reduction, parallelism,
resources, quick comparison, quick parallel runtime, and agent gates pass.

## Removal and supersession

A future typed-identity backend may replace this name encoding. It must preserve the
lexical binding regressions and deterministic production source authority.
