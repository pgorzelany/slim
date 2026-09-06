# RFC-0133: Transactional project snapshots

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-07
DecisionDate: 2026-09-07
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

Implement a compiler-owned project session with captured input bytes, explicit
configuration fingerprints, monotonic attempted revisions, a last good checked
snapshot and optional generated C. Publish only completed candidates. Reuse an
unchanged complete snapshot without source parsing, checking or C generation;
otherwise use the shared retained project checker. Keep every remaining M1
obligation, including declaration-level parsed/analysis/emission retention and
public command migration.

## Motivation

Retained project preparation now provides checked results but has no owner that
selects successful history and preserves it across failed updates. A session also
must not read an input key and later reopen files for compilation: those two reads
can observe different contents. Whole-snapshot reuse needs original inputs and
explicit configuration identities, not just a successful typing cache.

## Guide-level explanation

Capture the current manifest and module bytes once. The same captured input is
validated and compiled, and becomes the input identity of a successful snapshot.
Every admitted update consumes a checked attempted revision. Rejected input, capacity
exhaustion and invalid configuration leave the last good snapshot selected.

An exact input/configuration match may reuse the complete good snapshot. Its
published revision remains the revision that produced it, distinct from the new
attempt number. Changed input goes through current project validation and retained
checking. A configuration change discards all query eligibility. Identical checked
canonical source and configuration can reuse whole C after current validation;
a missing or damaged optional C buffer is regenerated from checked source.

## Reference-level specification

### Captured inputs

Add a compiler-owned ProjectInput with status, original manifest bytes and its
checked metadata, and the ordered raw module byte buffers. Capture does manifest
validation and file reads; it does not parse SLIM modules. Preparation parses the
captured module bytes and shares the existing identity/visibility/entry/layout/
semantic checks and original diagnostic mapping. Ordinary path preparation uses the same manifest, module and final checking
operations while preserving its interleaved read/parse order and early failure.
Session capture records individual read failures and preparation reports them in
module order, so an earlier syntax failure still precedes a later missing file. The legacy source-index loader keeps its
existing API and uses the same module parse/validation helper.

Module buffers correspond exactly to the checked manifest's ordered module list.
The capture result, not a later filesystem read, is used for checking and for the
retained input key. Captured metadata is trusted compiler-owned data, not a
serialized semantic schema. The public transport must supply raw bytes or use
this capture operation; external facts/tokens cannot approve source.

### Session state and identities

Configuration has separate complete byte fingerprints for compiler implementation,
runtime content/ABI, target and options. Each is nonempty and at most 4,096 bytes.
The compiler host attests these identities; they are equality guards inside a
trusted running compiler, not authentication of imported executable caches. Public
transport must bind them to its actual toolchain before that transport is complete.
A difference in any field forces cold checking and generation.

State owns limits, an attempted Revision counter and an optional good snapshot.
An accepted snapshot records the configuration supplied for its candidate. The good snapshot owns captured input, prepared checked project, typing
history, and optional C with its exact length and bounded checksum. Never accept
persisted arbitrary semantic facts. The attempted revision advances even when a
candidate is rejected. Exhaustion reports a stable tooling reason and never wraps.
An unchanged hit retains the old published revision; it does not reinterpret old
node identities as belonging to the attempted revision. Epoch construction reserves
serial 1; its first admitted attempt has serial 2. Invalid limits, exhausted attempt
capacity and revision exhaustion reject admission without issuing another identity.

### Limits and lifetime

Configuration exposes a maximum of 64 update attempts per owning epoch, admitted
input bytes in 1..67,108,864, retained canonical nodes in 1..1,000,000, and C payload
bytes in 1..67,108,864. Invalid limits reject the operation. Account input and
canonical storage cumulatively within the epoch before admitting new retained
payloads; failed candidates also consume attempted work. Strict captured
preparation reports its actual canonical-node count and declines before function
checking when that admission limit is exceeded; it cannot fall through to the
ordinary checker as the earlier optional typing-cache API does. Check remaining capacity
before additions. Capacity rejection cannot reset history or silently claim reuse.

These are admitted-input/query and retained-payload limits, not an RSS claim.
Filesystem read buffers precede admission; current unbounded read_file behavior is
unchanged and must be reported separately. The normal parser/checker/emitter's
transient allocations retain their existing contracts. A generated C buffer is
checked against its retained payload limit before publication.

The owning epoch is a scalar-returning function without exclusive output parameters,
so existing generated region cleanup reclaims its complete session allocation
region on return. There is no in-place reset that can reset accounting while
keeping old backing allocations alive. Reset ends that owner before creating a
new epoch with identity.reset; exhaustion cannot restart at an old identity.
No new allocator, cleanup syntax, runtime ABI or M2 allocation domain is introduced.
An API caller that retains the state outside this owner has not established the
public service lifetime contract.

### Publication, C and work

Build a candidate and its report before installing it. Require accepted current
project preparation, eligible typed history, matching context and successful
payload-limit checks. The single final replacement selects the good snapshot.
Source errors, capture failure and capacity errors do not mutate it. Allocation
failure follows the existing status-71 process boundary and cannot produce a
complete successful response or replace a host-published artifact.

An unchanged input/configuration hit retains all checked fields and C. Captured
raw bytes and manifest metadata still incur visible input work; zero parser/checker
queries does not mean zero total work. Missing structural snapshot metadata forces
a cold miss. Optional C integrity failures cause C regeneration, never acceptance
of externally supplied semantic facts. Whole-C reuse requires complete identical
canonical source/configuration or the same immutable checked snapshot, not a hash
match in place of source equality. Partial C-fragment reuse remains parent work.

Report attempted and published revisions separately, candidate status/reason,
actual function checks/reuse/imports, whole-snapshot reuse, and C generation/reuse.
Keep negative/unknown inference counts distinct from exact zero. Native observers
must count actual program parses, checks and generation independently. External
backend execution is not part of this core and must not be counted as avoided work.

## Compiler and runtime design

Implement captured input and shared preparation in project.slim, and the session
owner/update logic in session.slim. Use ordinary records, checked vectors and the
existing production checker/emitter. A one-element Usage vector owns mutable
counters; an optional one-element Artifact vector owns C and its checksum. Updates
use ordinary vec.set and checked affine-field replacement. No scalar-field
mutation, scalar mem.replace extension or implicit copying operation is added. Preserve existing public estimate wrappers
until the separate command/schema migration is implemented; do not put retained
work values under historical estimate labels.

## Compatibility and migration

No source syntax, ownership, effect, runtime or accepted-program changes. Ordinary
commands preserve output and diagnostics. This contract introduces the internal
session core and its ownership/lifetime recipe. Public CLI/transport migration,
actual host identity binding and external artifact transactions remain explicit
M1 requirements, alongside flow ownership and granular retained queries.

## Diagnostics and failure cases

Distinguish input rejection, invalid configuration, invalid limits, attempt/input/
node/C capacity, exhausted revisions, missing snapshot metadata and damaged
optional C. Source diagnostics retain current module spans. Last-good output is
always labelled with its published owner; it is not the executable for rejected
changed input. Host publication requires a complete successful response.

The stable session reasons are:

| Reason | Meaning |
| --- | --- |
| S0001 | Captured input or current project checking rejected the candidate |
| S0002 | Limits or usage counters are invalid |
| S0003 | The epoch's admitted attempt capacity is exhausted |
| S0004 | Admitted input-byte capacity is exhausted |
| S0005 | Retained canonical-node capacity is exhausted |
| S0006 | Generated C payload capacity is exhausted |
| S0007 | A fresh attempted revision cannot be issued |
| S0008 | Checked preparation supplied no eligible typing history |
| S0009 | A configuration fingerprint is empty or exceeds 4,096 bytes |

Missing snapshot metadata and damaged optional C are misses, not source errors.
Source rejection retains its ordinary status; session admission/capacity failures
return status 65. Unknown inference work remains -1, distinct from an observed
zero. Input/node/C usage saturates at its configured limit on capacity failure;
unchanged checked history can still be reused after node/C exhaustion when input
and attempt capacity remain.

## Performance and complexity

Exact whole-input matching is linear in captured bytes. Changed checking uses
indexed retained declaration dependencies and the sole normal checker. No-change
source parser/checker/emitter calls must be zero. Keep captured-byte admission visible and include file reads/data framing,
comparison and allocation in total latency; the zero-query observation excludes
capture and is not a zero-total-work claim. Public host/backend accounting remains
a separate parent requirement. Measure ordinary before/after project
preparation and clean-versus-session geometric edits without claiming a portable
latency improvement. Preserve all existing budgets.

## Alternatives and drawbacks

A key read followed by a separate preparation read admits inconsistent input
versions. An estimate-only state does not retain accepted results. Reusing C from
partial key matches loses semantic/configuration dependencies. Unlimited reset
inside one caller region would conceal accumulating storage. New source-level
process/allocator primitives are not authorized by M1.

## Test and acceptance plan

Compare captured preparation with ordinary project preparation, including a file
change after capture, source diagnostics and all prepared fields/C. Test unchanged
updates, body/interface/layout/effect/ownership edits, module/declaration namespace
changes, relocation, rejection/recovery, every configuration field, exact/beyond
capacity boundaries, revision exhaustion, missing metadata and C corruption.
Observe source parsing/checking/generation counts, selected revision identities
and immutable prior good output. Exercise epoch cleanup under native instrumentation,
ASan/UBSan and bounded allocation-fault campaigns. Preserve required checkpoint
checks and the full parent release audit.

## Ratings and evidence

Analysis +2, score 15, targets actual transactional checked-history ownership and
observed reuse. Compile remains zero until measurements justify more. Record all
limits, source/seed sizes, costs and incomplete parent requirements honestly.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit M1 goal. This is
not external review. No existing semantic, compatibility or performance gate is
relaxed. The full M1 objective remains unchanged.

## Implementation

Implemented by captured ProjectInput/shared preparation in project.slim and the
transactional owner in session.slim. The production probe, native phase/epoch
observer, permanent integration matrix and sanitizer/fault campaign implement this
RFC's internal core and owner recipe. No public service completion is claimed.

The checkpoint report in
`benchmarks/results/2026-09-05-slim-next-progress.md` records exact test domains,
remaining M1 work and the 3,985,480-byte seed with SHA-256
`aaf9a4305bc3516b8a6cbe367a7eb057584fdf2ae8ccb050e4e88304ef64e1d8`.
The dated session matrix, work, faults, native and latency TSVs retain measurements.
All 94 accepted files, 10 unit tests, 68 integration tests, 333 conformance fixtures,
2,000 malformed mutations and required checkpoint gates pass. The 2,048 native/
sanitized fault ordinals contain 426 matching status-71 failures and 1,622
successes. Geometric observations through 4,000 helpers distinguish actual cold,
unchanged and one-body-edit work. At that size two clean generation passes versus
cold-plus-unchanged session take 65.847/43.816 ms; a body edit takes 66.979/90.513 ms.
These process totals do not support a general changed-input latency improvement.

Public transport/host identity binding, full service integration, granular queries
and the full parent release gate remain pending under RFC-0124.

## Removal and supersession

A replacement must preserve exact captured-input ownership, complete context keys,
atomic selection of checked good history, revision exhaustion, physical epoch
cleanup, optional-artifact misses, actual observed work and every durable gate.
