# RFC-0156: Reserved-buffer structured tasks

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-08
DecisionDate: pending
Approver: project-maintainer
Kind: language
Primitive: none
Evaluation: m2-experiment
EvaluationRFC: RFC-0147
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Migrate existing structured host tasks to explicitly reserved buffers and uniform
moves. Keep the single `parallel:` wrapper, two direct leaf calls, one possible
worker, identical inline fallback and post-join result installation. Task bodies
may mutate their uniquely moved byte buffer through RFC-0155 host operations,
but may neither allocate nor release backing storage. Every moved buffer must
return exactly once inside its task result. The parent resumes release authority
only after join. No child allocator, quota syntax, shared pool, task handle or
new scheduler is needed for this shape.

This is a proposed M2 ownership/owned-byte/effect migration of RFC-0078/0079;
it does not broaden general concurrency. RFC-0071 automatic pure tasks retain
their existing requirements and performance gates. No current source or runtime
changes under this draft.

## Motivation

The current tasks create child regions, allocate response vectors internally,
return owners, and adopt those regions after join. A fixed infallible-release
pool cannot safely replace each linked region without defining reservation,
partitioning, child-state lifetimes, empty-owner adoption and release rules.
Rebinding a parent Allocator implicitly would also conceal allocation work.

RFC-0155 removes the reason maintained host tasks allocate internally: response
storage can be reserved explicitly before launching their bounded exchange.
Uniform moves let each task exclusively hold its preallocated buffer. Returning
it preserves the existing owned-result obligation without reallocating,
retagging providers or allowing worker cleanup to mutate the parent pool.
This revises the unimplemented child-pool assumption in RFC-0151/0152/0153;
those drafts are not evidence that new domain machinery is required.

## Guide-level explanation

Prepare both output buffers in source order, handling each reserve result before
entering `parallel`. Each direct task call receives one moved buffer plus its
ordinary copied/shared inputs. For example, the following is successor syntax
only, with lifetime binders abbreviated in the explanatory types:

```text
parallel:
  let first: Reply['host] = fetch(8080, "LEFT", move left_buffer)
  let second: Reply['host] = fetch(8081, "RIGHT", move right_buffer)
  # Both results are now available to the parent.
```

The task returns `Reply(ok: ok, body: move buffer)` after a bounded exchange.
If the exchange fails, it still returns the unchanged initialized buffer and a
false status. `dual_health` likewise returns its response buffer with the health
result; the parent releases it after join. Parent result processing, freezing,
growth and cleanup are ordinary checked source operations.

The new entry and explicit reservation rules are RFC-0153/0155 obligations.
A reserved buffer carries its origin lifetime, but does not grant a task
permission to allocate or release through that domain. This is a checked
execution restriction, not an unchecked Send assertion or a source cast.

## Reference-level specification

### Exact initial task shape

Keep the existing supported leading two-let form and direct leaf user calls.
At least one accepted bounded clock/TCP operation is still required. Preserve
the existing prohibition on nested user calls, nested forks and unknown work
where the current selection rule requires evidence. The later effect child
must independently map current partial rejection to exact trap/progress facts;
declared effect ceilings alone never prove occurrence or absence of a hazard.

Input values may be the current copyable scalar family, shared byte slices,
and at most one whole moved `Vec['d, U8]` per task. No exclusive-reference
capture, projected owner move, owning element container, owned Bytes input,
allocator capability (including nested capability data), or borrowed-result
escape is admitted by this initial shape. Shared input loans must remain live
and immutable through the complete join boundary; they may borrow parent-owned
data, not just literals. A captured view into either moved buffer is rejected.
Distinct source variable names alone do not establish distinct origins.

A task with a moved buffer must return precisely that same owner, either directly
or as one declared aggregate/enum payload alongside copyable scalar fields.
Every returning branch carries it exactly once. The origin may move through
local whole bindings and aggregate construction; it may not be replaced,
frozen, partially moved, copied, dropped or sent to another call as an owner.
A task with no moved owner returns only the previously supported copyable
result family. The initial result shape has no nested owning collection or
borrowed reference. Generalize only through a separate measured decision.

These restrictions preserve owned response results for both maintained host
applications. They do not claim every formerly allocating task body can run
unchanged. Migrate its allocation explicitly to the parent and retain every
old semantic witness under the specified successor contract. An explicit task
outside the supported shape receives the existing unsupported-task diagnostic
family with a stable reason, rather than being silently serialized.

### Resource-preservation check

Derive the task's normal-exit cleanup plan and owner origins from the sole
RFC-0149/0151 checker. For every accepted task path require:

1. No provider allocation, reserve/growth, release or domain teardown operation.
2. No cleanup edge releases the captured owner or another heap owner.
3. Every normal result contains the one captured origin exactly once, or there
   was no captured origin and the result carries no owner.
4. Every mutation is confined to the moved U8 buffer or ordinary local scalar
   state. Its host accesses have complete checked type/loan evidence.
5. Every borrowed input retains the caller's join-scoped reservation. No source
   operation can end that reservation while either task might use it.

An unused `alloc` capability is not assumed harmless from its spelling; the
complete body and permitted callees must establish the empty provider-operation
set. Missing or bounded-away facts reject mandatory task eligibility. Keep each
blocker independently even when another is already sufficient. A raw pool
header, empty-buffer pointer or favorable benchmark never substitutes for this
source proof. Empty owners obey the same origin rule as nonempty ones.

Use the existing mandatory ownership work/state budgets from RFC-0149; this
check consumes those counters rather than opening another unlimited pass.
Results retain checked origin identity, complete cleanup-edge dependencies and
the same bounded/unknown reasons in retained queries. Generated code does not
recompute an optimistic effect scan. Never charge exhaustive path enumeration
to the ordinary compiler: merge finite owner-origin facts on the canonical
control view and reject on declared budget exhaustion.

### Runtime ownership interval

The parent evaluates both call captures in source order and moves each buffer
into its typed task context before starting either call. It suspends source use
of the captured owners and shared-loan conflicts until both calls finish. A
successful first spawn executes task one on the worker; task two runs in worker
scope on the parent. On a declined spawn, task two then the identical task-one
body execute inline, retaining the current fallback schedule.

Worker scope still declines nested spawning. The parent performs exactly one
join for a successful spawn and none for a declined spawn. Results install in
first-then-second lexical order only after successful completion/join. Both
contexts and all borrowed source backing remain live until then. Normal scope
exit, recurrence, result inspection or reassignment cannot bypass that edge.

The provider state is thread-confined to the parent and remains quiescent while
these task bodies run. Threads access only their disjoint initialized/spare
payload storage and immutable inputs. They do not access allocator indexes,
attempt counters, block headers or release metadata. Even separate blocks from
the same root pool need no allocator lock or atomic counter under this rule.
The parent regains ordinary reserve/release authority after join. Backing
allocation identity and empty-owner domain identity never change, so no adoption
walk, provider retag or lifetime extension is needed.

If a program traps or a join fails, use RFC-0155 process termination without
freeing another worker's accessible pool. That path is not normal cleanup.
Normal executions still demonstrate eventual release exactly once and safe
post-join buffer resizing. The reserved-buffer protocol neither recovers from
arbitrary native corruption nor cancels another worker.

### Explicit reservation and failures

Each application selects and reserves its response bound before entering the
form. For dual_fetch keep 64 bytes per response and the existing 2,000 ms
timeout; for dual_health keep 8 bytes and the same timeout. Reservation failure
is ordinary source data. If the application deliberately exits 71, it does so
before any task or network request starts. Record this intentional change from
the old sticky cross-worker allocation failure, which could occur after a
request had been sent. Do not relabel the old failure ordinal as a new one.

The reservation schedule and allocation-attempt identities are identical in
POSIX, portable serial, forced-serial and spawn-failure builds. No hidden child
pool or per-worker reservation is added. Runtime scheduling faults do not alter
which source reserve operations occurred. Output/transport failure still
returns both owned buffers after the join boundary.

## Compiler and runtime design

Extend the production typed capture/result descriptor with checked buffer
origin and cleanup eligibility. The direct generated context stores the same
owner representation; moving it is not semantic cloning. RFC-0151's ordinary
initialization flags determine when the parent/context/result owns it. Clear
each moved-from obligation and install each result once. Runtime ABI identities
include the actual consumed pool, host adapter, task and generated layout files.

Preserve `SLIM_PARALLEL` and the separate platform-worker macro. Allocation-free
automatic CPU sites do not acquire buffer descriptors, pool reservations or
extra runtime checks. Programs without an executable site retain no task state,
environment reads or thread flags. This migration is not an excuse to weaken
RFC-0071's threshold or either automatic parallel ratio.

## Compatibility and migration

Accept with the complete ownership/cleanup and RFC-0155 storage contracts.
Replace child-region allocating host tasks at the source/ABI cutover with
parent reservation and moved-buffer returns. Do not ship both ownership protocols
as permanent options. Historical seed/runtime evidence remains reproducible;
the successor's conformance executes only through its production compiler.

Retain the owned-result integration gate: it must still check actual worker and
inline execution, exact bytes after join, original allocation identity, further
parent growth, release exactly once and failure recovery. Change only obsolete
generated-C region/adopt string assertions to assertions for the actual checked
transfer/return/join protocol at implementation. Preserve the native legacy
region-adoption witness as scoped historical evidence and add the current ABI
equivalent; merely deleting that gate would not constitute migration.

## Diagnostics and failure cases

Reject repeated/missing returned origins, captured allocator authority, an
owned input shared by tasks, overlap between input views and moved output,
cleanup/growth in a task, escaping borrowed results, unsupported owner layouts
and exhausted mandatory checking. Pin source spans to the responsible transfer,
cleanup edge, call or capture. A rejected explicit shape is not a successful
serial fallback. Unsupported platform spawning of an accepted shape does use
the identical inline protocol.

## Performance and complexity

Freeze dual_fetch/dual_health input bytes, response bounds and latency fixtures
before native measurements. Preserve their 0.75 parallel/serial limits, and the
state_machine/signal_network budgets separately. Report parent reservations,
header bytes, peak live storage, capture/result size, native code size, checker
work, setup and runtime. Hoisting admission can retain buffers longer; fewer
copies or no child-state metadata do not establish a net performance benefit.

## Alternatives and drawbacks

A child buddy pool per task would require explicit reservation/quota authority,
large retained chunks and post-join empty-owner/provider adoption. Supporting
arbitrary allocating tasks is a separate concurrency feature, unnecessary for
the preserved bounded network operations. A lock around the parent allocator
would violate the release contract. Exclusive reference captures could avoid
the owner return but would remove the existing owned-result witness and add a
second capture shape. The selected proposal uses one move/return protocol.

Its cost is a restrictive checked task body and explicit source preparation.
Every failed attempt to preserve current application behavior must be recorded;
the protocol is not accepted just because it is easier to implement.

## Test and acceptance plan

Before acceptance, execute a native pool/task witness with two distinct buffers
from one domain; both zero and nonzero capacities; both completion orders;
successful spawn, declined spawn, forced serial and unsupported platform. Check
provider indexes/counters/header bytes unchanged while tasks execute, exact
payload results, original identities after join, parent resize/release after
join and empty-pool teardown. Inject each preparation allocation ordinal and
spawn/join failure. Check a live-worker trap never frees the backing pool.

Then require production source positive/negative origin/loan/cleanup matrices,
all branch results, budget boundaries, stale retained plan rejection and exact
blocker sets. A native scheduling witness does not prove the source checker.
Run both loopback applications and both automatic CPU applications through the
production compiler with unchanged same-host gates. Keep every allocation,
spawn/join, timeout and transport failure as a separately mapped witness.
Run every AGENTS.md command and the complete milestone release gate at M2
closure. No favorable native result alone completes worker adoption or M2.

## Ratings and evidence

All ratings are neutral under the scoped M2 move/owned-byte/effect experiment.
The current task/region behavior and maintained application buffer bounds are
inspected facts. Successor source soundness, native worker behavior and measured
cost remain unknown. No general concurrency or performance exception is granted.

## Decision

Proposed. Native and source evidence are outstanding. This supplies an explicit
alternative to the unimplemented child-pool assumption, not permission to remove
owned-result, failure, parallelism or lifecycle gates.

## Implementation

Pending; current generated programs still use the accepted region/task protocol.

## Removal and supersession

If accepted and implemented, supersede only the allocating explicit-host-task
capture/adoption parts of RFC-0078/0079 for the successor. Keep automatic pure
execution separate. Reject or revise if any required current application intent,
ownership obligation or unchanged performance budget cannot be preserved.
