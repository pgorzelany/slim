# RFC-0121: Observed compiler work

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
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Add opt-in observations of actual production compiler execution. Count native
entry and loop-header executions for parsing, checking, ownership, C generation,
cache handling, and snapshot comparison. Keep invalidation estimates, observed
operations, profiling overhead, and ordinary compiler latency distinct.

## Motivation

The current `session` command prints invalidation estimates under historical
operation labels. Unchanged inputs can print zeros while both projects are
loaded and parsed. The whole-project artifact cache has a real hit path, but
avoided checker/generator work has not been instrumented. M0 requires evidence
at actual execution points before M1 introduces retained incremental queries.

## Guide-level explanation

`slim-bench work` builds an instrumented copy of the verified portable compiler
for a bounded measurement campaign. Its records name the source operation and
whether they count native function entries, loop-header visits, or input bytes.
An iteration count includes the final termination test. A call count does not
count a tail `recur` as a new native call; the separate header counter does.

The command compares every instrumented operation with the ordinary production
compiler's output and exit status. A cache hit can avoid all source parsing,
checking, and generation while still reading source bytes and lexing its key
manifest. An unchanged snapshot comparison is not incremental compilation.

## Reference-level specification

Use a closed, versioned list of source-owned function identities and hook kinds.
The instrumentation builder first requires the ordinary production compiler to
reproduce the checked-in C seed byte for byte. It then inserts observation-only
C statements at the unique generated definition and, where requested, unique
`slim_recur` header. Missing or duplicated anchors fail the measurement; they
never silently produce zero counts. Record compiler, seed, instrumentation,
configuration, workload, and input identities with the results.

Every counter has a fixed cap of 1,000,000,000. Increment with a checked
remaining-capacity comparison, never overflowing analyzer arithmetic. An
exhausted counter reports `bounded` and the cap; other counters report `exact`
for the observed process. A missing exit report is unknown, not zero work.
Small-cap tests cross saturation deliberately. Do not infer wall-clock costs,
whole-program safety, query reuse, or agent success from operation counts.

## Compiler and runtime design

The production SLIM sources, checked semantics, ordinary C seed, runtime ABI,
and default binary remain unchanged. Rust is used only to build and run this
measurement apparatus; it supplies no typing, ownership, lowering, or fallback
semantics. The instrumented binary executes the same generated production
compiler. Its sole additions are capped counters and an exit report written
to a harness-owned temporary file. It is never installed as the compiler or
used to produce a release seed. Normal builds pay no instrumentation cost.

The measurement binary is serial, built without worker-enabling macros, and
uses no hidden worker synchronization. Register reporting before compiler
startup so normal returns and explicit allocation-failure exits can report
partial work. A signal or report I/O failure cannot establish a complete count.
Reports do not contaminate compiler stdout, diagnostics, or cache frames.
The ordinary and instrumented commands must agree on stdout, stderr, and exit
status. Instrumentation setup and external C compilation are separate harness
operations and must not be confused with native frontend work.

## Compatibility and migration

No source syntax, builtin, compiler result schema, or release ABI changes.
Historical session numbers remain invalidation estimates. Add explicit observed
work records beside them and update current documentation to prevent reuse
claims based on estimates. Retained incremental checking remains M1 work.

## Diagnostics and failure cases

Missing/duplicate native anchors, changed seed reproduction, malformed telemetry,
missing telemetry, counter saturation, and output disagreement are visible
measurement failures or bounded/unknown results. They do not authorize accepting
a source program. Corrupt or stale cache entries continue through the normal
production miss path; invalid source cannot publish a replacement cache entry.

## Performance and complexity

Instrumentation inserts constant work per named native entry/header visit and
uses a fixed-size counter array. Building the instrumented source is linear in
the verified seed size times the fixed hook-table size. Default compiler work,
application code, and all existing performance budgets remain unchanged.
Gate deterministic work across geometric inputs and compare clean, hit, miss,
edited, failed, and recovered paths. Record normal latency separately; do not
present instrumented latency as production latency.

## Alternatives and drawbacks

Relabelling invalidation estimates does not observe execution. Adding mutable
counter parameters throughout the production compiler would change its hot
paths and default costs merely to measure them. Native observation anchors
instead require a maintained mapping to generated names and headers. Exact
reproduction and output differential checks bound that maintenance risk without
introducing a second semantic authority.

## Test and acceptance plan

Exercise clean checks/emission, unchanged snapshot comparison, real cache hits
and misses, body/signature/layout/effect/borrow-mode edits, insertion/deletion,
rename/reordering/relocation, corruption, and failed-then-recovered input.
Compare emitted C and diagnostics with clean production commands. Verify
ordinary and instrumented outputs, counter saturation and missing reports,
and geometric work. Preserve existing conformance, sanitizer, bootstrap,
governance, native baselines, and performance gates. Add the work campaign to
the full repository verification command.

## Ratings and evidence

Analysis +2 distinguishes observed execution from invalidation estimates.
Other ratings are zero; no speed, safety, runtime, or agent-effectiveness gain
is assumed. Weighted score: 15.

## Decision

Accepted under RFC-0112's delegated implementation authority and the active
M0 goal. This remains Rust verification/measurement, not a production Rust
compiler exception. No hard gate or performance budget is relaxed.

## Implementation

Implemented by `src/bin/slim-bench/work.rs` and the measurement-only native
`benchmarks/instrumentation/work_probe.c`. The Rust module is classified as
infrastructure. Thirty-two counters cover the closed source/runtime hook list;
the default compiler and 3,020,129-byte seed remain unchanged.

Quick and full campaigns pass 214 observed compiler operations apiece, with two
identical observed repetitions and an ordinary production comparison per
operation. The full campaign spans 1,000 through 8,000 declarations. Ten external
C builds and successful native executions are counted separately. All 20 native
challenge C outputs are byte-identical to ordinary production output. The quick
ASan/UBSan campaign and 128 allocation-fault ordinals pass; 107 faults are reached.
Three unit tests cover anchor failure, absent/incomplete evidence, and small-cap
saturation including `UINT64_MAX` under UBSan.

Bootstrap, governance, formatting, Clippy, all 10 unit and 58 integration tests,
325 conformance fixtures and 2,000 malformed mutations, quick performance and
reduction, parallelism, resources, quick native comparison, quick parallel runtime,
and agent gates pass. Actual records and limitations are retained in
`benchmarks/results/archive/2026-09-06-observed-compiler-work.tsv.gz` and the SLIM Next progress
report. An unchanged snapshot's four parses are distinguished from its four zero
invalidation estimates. A cold two-module emission has three parses, including
flattened-source reparsing. These are observed current behavior, not M1 reuse.

## Removal and supersession

M1 may replace these observation hooks with retained-query instrumentation,
but must preserve the actual-work regressions, clean-output comparisons,
historical evidence, and the distinction between exact, bounded, and unknown.
