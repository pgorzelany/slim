# RFC-0127: Isolated function checking

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: -1
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 5

## Summary

Make the existing production function checker a reusable operation with independent
binding, loan, and ownership scratch state. Materialize binding facts before that
scratch state leaves the operation. This supplies a semantic query boundary for RFC-0124.

## Motivation

The current `typing.check_functions` owns one binding table and ownership forest
for the whole program. Although each function starts without a lexical parent,
its scratch identities depend on preceding functions and its storage includes
previous functions' state. A declaration query needs a fresh independent execution
of the same rules, with no old function's loans or moves in scope.

## Guide-level explanation

Each function uses the normal expression/type/ownership checker with its own
scratch tables. Successful and diagnostic facts are retained in the ordinary
current-source fact vector. Scratch binding numbers start at two for every
function and never serve as cross-function or cross-revision identities.
The source-node identity remains the existing canonical declaration node.

## Reference-level specification

`typing.check_function(source, @tokens, item, @facts, @issues)` is the sole body
checking operation. Its input is a function selected from the normal validated
canonical declaration index, with current declaration and lexical links established,
well-formed interfaces/layouts, and a fact vector initialized for the same tokens.
The ordinary whole-program driver preserves its original ordering, early-stop
policy, and global declaration/layout checks, then invokes this operation once
per reached function.

The operation creates two sentinel binding entries, two initial ownership scopes,
and an empty frame vector. It binds the function's parameters with parent -1,
infers the body using the existing functions, checks the existing borrowed-return
rule, and writes binding type/mode facts for that function before discarding access to the
scratch tables. No binding, loan, frame, or union-find parent from a preceding
function is consulted. Existing branch joins, moves, reinitialization, enum payload
loans, and recurrence argument rules remain unchanged.

The current raw name-token link used during checking holds a function-local
scratch binding index. Retained clients use canonical declaration nodes and
materialized facts; they cannot interpret this scratch link in another function.
Typed persistent binding/type/place handles and fact import maps remain separate
RFC-0124 obligations. This operation does not accept serialized facts as evidence.

Fact slots outside the selected function are preserved. Calling the operation on
freshly linked source and initialized facts in different function orders must
produce identical facts and per-function links for accepted programs. The public
whole-program diagnostic order remains lexical and unchanged. A query caller
must provide current linked input and manage invalidation/diagnostic publication;
the body operation alone does not establish those higher-level guarantees.

Scratch work and logically accessible scratch are bounded by the selected function's
traversed source and existing ownership algorithm. The current conservative memory
planner allocates this function's vectors in the caller region because it has
exclusive output parameters. Backing allocations therefore remain until that
region closes; this change does not claim per-function physical reclamation.
Cumulative allocation requests remain approximately linear and are measured. No new fixed inference limit, language size ceiling,
runtime primitive, or semantic fallback is introduced. Allocation failure retains
the runtime's explicit failure behavior and cannot produce accepted partial output.

## Compiler and runtime design

Use the existing SLIM checker and ordinary local vectors. Move the existing
parameter/body/return orchestration into one function; do not copy its inference
rules. The whole-program path remains the only production semantic authority.
Regenerate the portable C seed and preserve deterministic generated C/diagnostics.

## Compatibility and migration

There is no source-language or runtime ABI change. The internal scratch-link
numbering becomes local to its function. Every downstream consumer must continue
using canonical declaration links and retained type/mode facts. This is a reusable
execution boundary, not a retained semantic cache or completed M1 control-flow view.

## Diagnostics and failure cases

Preserve all existing diagnostic codes, spans, ordering and recovery. Earlier
inference-blocking issues still stop the ordinary driver. Per-function callers
must not call the body checker on malformed declarations or stale linked views.

## Performance and complexity

Ordinary checking remains approximately linear. Measure before/after compilation,
allocation work and geometric function families. Independent scratch vectors can
increase allocation operations and retained backing storage. Record that
cost rather than assuming an improvement; existing gates cannot be relaxed.

## Alternatives and drawbacks

Passing global scratch offsets into every query would retain unnecessary coupling.
A new Rust checker or a copied SLIM inference implementation would violate the
single-authority requirement. Fresh scratch tables have a setup cost and do not
by themselves implement cross-revision fact reuse.

## Test and acceptance plan

Compare baseline and candidate generated C, acceptance, diagnostics and complete
analysis for the native corpus and ownership regressions. Exercise production body
checking in forward, reverse and repeated order on independently initialized fact
vectors, including owned parameters, loans, branch reinitialization and recurrence.
Preserve the M0 bounded path oracles and source-map tests. Count actual function
check executions, binding-fact materialization, allocation attempts, and cumulative
requested payload bytes (excluding runtime headers and system allocator overhead), retain geometric evidence, and
run bootstrap, governance, Cargo, conformance, required benchmarks, sanitizer and
allocation-fault checks before checkpointing.

## Ratings and evidence

Analysis +2 establishes the independent execution boundary and function-local
scratch lifetime. Compile -1 records the measured setup/allocation cost: at 1,000 generated
functions, checking is 1.040 times baseline latency, with 9,099 versus 7,106
allocation attempts and 6,058,867 versus 5,172,019 peak live payload bytes.
Other ratings remain zero; weighted score 5. These are same-host fixture
measurements, not universal latency or RSS claims.

## Decision

Accepted under the maintainer's RFC-0112 delegation and explicit M1 completion
request, following RFC-0123 and RFC-0124. This is not independent external review.

## Implementation

Implemented in `selfhost/typing.slim`, with the whole-program driver using the
single `check_function` entry. The portable seed reaches a 3,610,169-byte fixed
point. A production SLIM probe compares all facts and token fields after reverse
and repeated checks of 92 accepted files; Cargo and ASan/UBSan run that probe.
Baseline and candidate agree on complete diagnostics for 193 rejected fixtures
and emitted C/complete analysis for all 20 native applications. The existing
bounded ownership oracles and all required per-commit gates pass.

The observed-work campaign adds actual body/materialization and allocation
counters, permanent linear attempt budgets, and failure-inside-function coverage.
All 128 allocation ordinals agree between ordinary and sanitized compilers;
111 fail without partial output, including 26 after a body check begins, and
17 succeed beyond the fixture's allocations. The source-index/mapping fault
gates remain intact. No physical per-function reclamation or retained reuse is
claimed. [The dated report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
records costs, named validation domains, seed identity, and remaining M1 work.

## Removal and supersession

Future retained queries must preserve one checker, local scratch ownership, complete
fact import dependencies, deterministic diagnostics, and all regression domains.
This contract does not replace the remaining M1 control-flow, retention, publication,
emission, or full release requirements.
