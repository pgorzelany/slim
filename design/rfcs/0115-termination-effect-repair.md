# RFC-0115: Termination effect repair

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 35

## Summary

Restore the documented SLIM 0.9 `partial` ceiling for unproven recurrence
and recursive user calls. This is M0 current-contract repair under RFC-0112.
It does not implement the successor's planned separation of effects and
progress contracts.

## Motivation

The existing effect scan checks the effect lists of callees but neither
`recur` nor a cycle of functions with empty effect lists introduces `partial`.
Unconditional recurrence, direct recursion, and mutual recursion therefore
pass without the capability required by the current language contract.

## Guide-level explanation

A function containing an unproven `recur` must declare `partial`. Existing
exact totality facts may discharge that requirement for the exact recurrence
node. A recursive cycle of ordinary user calls requires `partial`; there is
currently no totality proof for that call shape. Callers continue to obey the
declared capability ceiling, even if a particular invocation appears finite.

An acyclic chain of functions without `partial` remains valid. Checked traps
are not conflated with termination: arithmetic and bounds checks keep their
existing semantics and do not acquire a new operation effect.

## Reference-level specification

Run termination checking only after the canonical source has passed typing
and the existing effect checks. For every function lacking `partial`, inspect
every canonical recurrence node. Accept its omission only from a positive
`ranges.fact_total` fact for that exact node. Compute the existing range view
once, lazily when such a recurrence exists. Its fixed four parameter-transfer
passes, 64-refinement limit, integer domain, and structural descent conditions
are unchanged. Missing or exhausted evidence requires the capability.

Repair the two existing totality conjunction walkers for prefixes and argument
lists: reject a non-total current element before tail-recurrence. Their old
`let rest = recur(...); current && rest` form jumped before the conjunction
and therefore ignored earlier elements. A positive descent result requires
every prefix initializer and every recurrent argument, not merely the list's
terminal case. This restores the accepted structural proof, without adding a
new rewrite or abstract domain.

The graph induced by functions without `partial` must be acyclic. Inspect
linked user-call targets in all checked bodies, including unused functions
and syntactically unreachable branches. The existing capability check already
rejects any edge from such a function to a declared-partial function. An
iterative depth-first traversal detects a gray-target edge as a cycle. Black
targets are reused; an unvisited target is entered once.

Traversal starts in declaration order and scans canonical nodes in order.
Diagnostics are deterministic: existing effect errors retain precedence,
then the first unproven recurrence, then the first discovered cycle edge.
This bounded diagnostic choice does not imply acceptance of unreported bodies.

## Compiler and runtime design

Implement the gate in the production SLIM checker. Reuse checked node links
and the existing range analyzer; add no parser, second semantic authority,
production Rust compiler logic, runtime ABI, or dependency.

The call traversal uses a dense node-indexed color table and an explicit
reusable frame vector containing only item, cursor, and end identities. Stack
depth is bounded by the number of declarations; every node and edge is visited
a constant number of times. No recursive call-stack walk or per-root repeated
reachability search is allowed. The view is ephemeral checker-local data.

## Compatibility and migration

Previously accepted undeclared nontermination is rejected. Migrate intended
partial functions and their callers by adding the existing capability, without
changing source evaluation order or native algorithms. Preserve positive
proven recurrence, acyclic call graphs, and declared-partial recursion.

## Diagnostics and failure cases

Use existing E0343 (missing effect) at the recurrence keyword or cycle-closing
call's callee, preserving the normal source-mapped diagnostic path.
Malformed and mistyped source must never enter the graph traversal. Unknown
termination cannot be promoted to exact based on a literal call argument,
direction alone, an absent report, or declared purity.

## Performance and complexity

Graph checking is O(canonical nodes + linked call edges), with O(nodes)
temporary memory. Range evidence is collected only when a function without
`partial` contains recurrence, using the existing fixed analysis limits.
Add permanent geometric acyclic call-chain and repeated shared-dependency
fixtures, independently retaining the existing 1.25 check-exponent ceiling.
Keep all earlier compiler and runtime budgets unchanged. Measure baseline and
candidate on the same host; do not claim LLM productivity from those timings.

## Alternatives and drawbacks

Requiring `partial` on every function rejects ordinary total code. Requiring
it on every `recur` discards accepted exact descent evidence. Searching from
each call independently can be quadratic. Reusing the 64-function parallel
report as an acceptance oracle would incorrectly accept bounded-away cycles.

The exact graph pass adds compiler work. The optional range pass can also
increase checking latency on programs using proven pure recurrence; retain
its cost as a separate workload. The successor will replace this capability
rule only through its explicit control/effect migration.

## Test and acceptance plan

Retain unconditional recurrence, direct recursion, mutual recursion, cycles
beyond 64 functions, invalid controller/prefix/argument recurrence, and calls
to partial functions as production negative fixtures with exact diagnostics.
Positive native fixtures cover the existing structural descent proof and
acyclic shared dependencies; declared-partial recursion is checked without
executing an infinite program. Test deep acyclic graphs without native stack
growth. Bootstrap, governance, conformance/malformed-input, Cargo, quick
performance/reduction/comparison, parallelism, and agent gates must pass.
Use ASan/UBSan and bounded allocation-failure injection on the checker path.

## Ratings and evidence

Safety +2 restores a violated declared capability contract. Compile 0 records
additional linear work without asserting a speedup. Runtime 0 introduces no
runtime mechanism. Minimal 0 retains the existing surface. Analysis +2 makes
missing termination evidence conservative. Dogfood 0 makes no unmeasured
development benefit claim. The weighted score is 35.

## Decision

Accepted within the maintainer's RFC-0112 authorization to implement M0-M7
and commit validated checkpoints. This records delegated implementation
authority, not a separate review of these details. No hard gate is relaxed.

## Implementation

Complete for the current `partial` repair. The production compiler passes
232 conformance fixtures, 2,000 malformed-input mutations, all 512 directed
three-function graphs, and deep chains through 2,048 functions. Fixed-budget
tests accept 32 proven countdowns and reject the 33rd and later functions
when the existing 64-refinement budget is exhausted. No limit was raised.

The two library vector `filled` functions, LZ4 extension writer, and recursive
parallel-analysis fixture now declare `partial`. The implementation report
records sanitizer/fault-injection scope, all required gates, unchanged native
analysis baselines, and paired frontend measurements.

## Removal and supersession

The successor effect/progress RFC may replace this source capability, but
must retain the reproducible nontermination cases and exact/unknown evidence
distinction. Preserve graph scaling workloads under any replacement checker.
