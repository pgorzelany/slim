# RFC-0113: Ownership repair and adversarial scaling

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-05
DecisionDate: 2026-09-05
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 2
Score: 60

## Summary

Record the first M0 implementation decision under accepted RFC-0112: repair
named owning transfers, distinguish exclusive branch move states, validate
cache frames before key reads, and remove duplicate-declaration rescans.
Add permanent adversarial scaling fixtures with a 1.25 endpoint exponent
ceiling. This tightens coverage; no existing budget is relaxed or removed.

## Motivation

A local vector assignment left the original owner usable. Growing the new
owner could free the original buffer and permit a checked read of freed
storage. Existing explicit call transfers also shared a single mutable move
flag across mutually exclusive branches, incorrectly rejecting valid programs.
The ownership repair exposes that branch defect in the compiler itself.

A declaration-name check rescanned all earlier declarations. The old small
performance series did not reliably detect its quadratic growth. A truncated
cache frame could reach key comparison before its complete length was checked.

## Guide-level explanation

The accepted 0.9 ownership contract continues to apply: a named affine value
transferred into an owning local, aggregate, assignment, or collection slot
cannot subsequently be used. Both exclusive arms may transfer the same owner.
A transfer in any reachable arm prevents a subsequent use after the join.
Borrowed storage still cannot escape into an owning destination.

This repair does not claim complete projection, conditional-result, or
reinitialization tracking. Those are outstanding M0 work. The successor
language remains governed by RFC-0112 and is not shipped by this decision.

## Reference-level specification

A binding's move event identifies a branch component, or no event. Every arm
gets a fresh active component. Finishing that arm makes its component inactive
while sibling arms are checked. At the common join, every direct arm component
merges into the enclosing component and inherits its active state.

A named read is invalid exactly when its recorded move component is active.
For an otherwise accepted program, the latest move is sufficient: a later
move on a compatible path would already fail at its source read; later moves
that pass are in mutually exclusive arms. At the join those alternatives have
the same unavailable outcome. This argument assumes the existing rule that
assignment does not revive an invalidated binding. Adding revival requires
separate definite-initialization analysis, not clearing this component.

Direct-arm lists survive component merges. Nested components may merge into
an arm but never into an enclosing match until that match reaches its join.
There is no inference that a statically written arm must execute.

Cache key reads require a valid header, valid lengths, exact complete frame,
and matching key length. Malformed and truncated frames rebuild normally.
The canonical declaration-name index retains the first declaration identity;
a later declaration with that identity reports the existing duplicate error.

## Compiler and runtime design

Implement branch components in the SLIM checker as a union-find table, using
union by rank and path compression. Bindings retain one move-component index.
One cursor slot tracks the current lexical arm. Direct arm lists permit one
join per arm without copying or scanning the binding table. `recur` remains a
control transfer: all joins run explicitly at the terminal arm-list case.

All state is derived from canonical parsed SLIM, lives only during checking,
and creates no second parser or accepted representation. No runtime ABI,
application allocation behavior, dependency, or production Rust semantics is
added. The portable seed is regenerated and verified at a fixed point.

## Compatibility and migration

Programs relying on duplicated named owners become errors. Previously rejected
exclusive-arm transfers become valid. Existing diagnostic codes and spans are
preserved. Unique declaration links remain identical. Valid cache contents
continue to behave identically. No syntax or language alias is introduced.

## Diagnostics and failure cases

Use E0315 at a use of an invalidated named owner and preserve E0347 for illegal
borrow escape. Keep malformed-cache recovery separate from source diagnostics.
Test transfers before a branch, twice within one arm, inside a nested arm,
and after a join, as well as all four outcomes of a two-level valid branch.

## Performance and complexity

For N bindings, arm components, and named uses, move-state work is amortized
O(N alpha(N)) with O(N) space. Individual root recursion is bounded by
union-by-rank tree height. No branch snapshots, spelling rescans, or whole
binding-table joins are permitted. Nodes without owning transfers receive no
application runtime cost; checking still carries its ordinary semantic tables.

`generated-common-prefix-declarations` checks 2,000, 4,000, 8,000, and 16,000
fixed-width names. `generated-branch-moves` checks 125, 250, 500, and 1,000
simultaneously live owners, each consumed in both exclusive arms. Each series
has a permanent check-exponent ceiling of 1.25, warmup, and repeated samples.
The ordinary quick performance command includes both series. The later M0
conditional-result repair adds a separately measured `generated-conditional-result-moves` variant under the same retained branch-move ceiling: it transfers
through a branch result with a statement prefix and then consumes that result.
The original explicit-call transfer fixture remains unchanged. These augment
all existing gates, including owned-transfer normalized cost.

Preliminary same-host measurements of the declaration/cache-only candidate
versus 97412bf reduced a separate 16,000-declaration check from a 647.0 ms median
to 22.8 ms. Seven interleaved pairs and one warmup were used; concurrent
conformance work introduces contention uncertainty. This is compiler latency,
not evidence of LLM task success. Final combined measurements belong in the
implementation progress report alongside the complete gate results.

## Alternatives and drawbacks

Copying all bindings at each branch makes wide programs quadratic. Replaying
nested sparse logs can also repeatedly visit the same moves at every depth.
A fixed analysis-depth cutoff would need a new conservative rejection rule.
Union-find adds checker state and parameter plumbing, but preserves the
accepted language and avoids those costs. Its applicability to whole named
owners must not be mistaken for a complete borrow checker.

## Test and acceptance plan

Add positive execution and exact negative diagnostic fixtures for each repaired
transfer path and branch condition. Exercise every truncated key prefix in
the existing corruption suite. Preserve the complete conformance and malformed
input suite, bootstrap fixed point, governance, Rust verification, performance,
reduction, parallelism, native comparison, and agent-tool gates.

Record each failure and repair; do not delete a gate after its first success.
M0 and RFC-0112 remain incomplete after this batch.

## Ratings and evidence

Safety +2: closes reproduced named-owner invalidation paths and invalid cache
reads. Compile +2: removes a demonstrated quadratic declaration scan and avoids
quadratic branch snapshots. Runtime 0: no application speedup is attributed.
Minimal 0: no language surface addition; internal state has a real cost.
Analysis +2: distinguishes mutually exclusive moves and conservative joins.
Dogfood +2: the stricter checker checks its own compiler after repairing branch
handling. Score 60 is a design assessment, not a semantic proof or benchmark.

## Decision

Accepted as a within-scope M0 implementation decision under RFC-0112. The
maintainer's authorization was: “approved, set a goal to implement the full
slim next. try to measure if it really increases you speed and effectivenes.”
This records that delegated implementation authority; it does not claim a
separate maintainer review of these details. No hard gate exception is used.

## Implementation

Complete for this bounded repair batch. See [the dated implementation report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
for all verification results and the remaining M0 work. The compiler passes
190 conformance fixtures, 2,000 malformed-input mutations, the 486-program
branch oracle, all required gates, and a 2,894,710-byte C fixed point. The new
quick scaling exponents are 0.755 for common-prefix declarations and 0.707 for
branch transfers, below the permanent 1.25 ceilings. The reproducible paired
frontend command retains raw same-host samples; no agent success claim follows.

## Removal and supersession

A later CFG/place-based checker may replace branch components only while
preserving all repaired cases, exact diagnostics, and scaling gates. Keep the
adversarial workloads and budgets permanently. Do not restore the declaration
scan or compare a cache key before validating its complete containing frame.
