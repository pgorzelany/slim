# RFC-0163: Catalog snapshot reconciliation

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, under delegated overnight ordinary-library direction
Created: 2026-10-02
DecisionDate: 2026-10-02
Approver: project-maintainer
Kind: compatibility
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Add an experimental ordinary SLIM component comparing two validated catalogs,
and one canonical catalog command: `catalog BEFORE diff AFTER`. Preserve
`catalog PATH exact KEY` and `catalog PATH prefix KEY`. This extends RFC-0160
under RFC-0112; it does not accept a stable library or new language operation.

## Motivation

The existing catalog queries one snapshot. Comparing expected/current component
or input manifests currently requires callers to repeat framing and validation.
Reuse its sorted records to expose actual additions, removals and changes.

## Guide-level explanation

Each input retains canonical key/weight/value frame triples. Reports contain
`changes N\n`, then frame triples `(key, before, after)` in byte lexical key order.
Each nonempty before/after payload is the complete canonical three-frame record;
an empty payload denotes absence. A record has a nonempty key, so this absence
marker cannot equal a present record. Equal snapshots print `changes 0\n`.

## Reference-level specification

`catalog_reconcile.prepare(before, after, change_limit, output_limit)` returns
`Invalid(code, position)` or `Ready(Diff(changes, report_bytes))`. `Change` stores
before/after record ordinals; -1 denotes absence, and both are never absent.
Correctness applies to unmodified `Catalog` values produced by `catalog_data.load`
and unmodified `Diff` values produced by `prepare`; public records are not opaque.
Each input admits at most 4,096 records, 1,048,576 source bytes and 256-byte
nonempty keys, with canonical signed I64 weights and arbitrary byte values.
Limits are 0..8,192 changes and 0..8,388,608 report bytes. Invalid parameters or
excess changes return code 23 at zero; excess predicted report size returns 24
at zero. Caller limits cross permanent tests, including exact output-size limits.

Merge-walk the existing sorted indices. Equal keys change only when I64 weights
differ or value bytes differ. Do not subtract weights or aggregate them. Emit
only changed keys; independent input reordering preserves successful report
bytes. Removed/modified keys use before ordinals; additions use after ordinals.
Expose the existing `std_byte_index.compare(left, left_start, left_end, right,
right_start, right_end)` implementation by explicit experimental export. For each
source its precondition is `0 <= start <= end <= bytes.len(source)`; it returns
-1/0/1 for valid spans. This partial API supplies no forged-span totality promise
or second comparison implementation. Existing build/find/prefix are unchanged.

Validate before completely, then read/validate after, then prepare and emit.
Diff input failures use `error CODE in before|after at POSITION\n`: existing
loader codes/byte offsets and exit 65, or read code 1 at zero and exit 66.
Comparison admission failures use `error CODE in diff at 0\n`, exit 65.
An emitted-byte-count disagreement uses code 25 in diff at zero, exit 65,
before successful stdout; the guard is unreachable only under the stated
producer/unmodified preconditions, and is not forged-record totality evidence.
Invocation failures retain `error 2 at 0\n`, exit 64. Validation, read, admission,
append, count and allocation failures before the single successful-report print
publish no successful prefix. Output failure retains the runtime's partial-write
and trap behavior; stdout is not transactional. Existing query commands retain
their exact reports/errors.

## Compiler and runtime design

Use existing byte/vector/ownership/effect APIs and explicit project imports.
Preparation declares alloc/partial; reporting declares alloc/io/partial. Complete
report-size preflight precedes allocation/mutation of report buffers and stdout.
Change storage may allocate during preparation; exhaustion preserves exit 71.
There is no parser duplication, syntax, alias, source/ABI change, new dependency,
semantic fallback, compiler integration or parent-path import.

## Compatibility and migration

Only mode `diff` is newly accepted. The original catalog corpus member and its
RFC-0030 2.00 emit/check gate remain; no new row or budget relaxation is proposed.
The independently frozen development cohort continues using its original base.

## Diagnostics and failure cases

Test side/offset priority for malformed before and after, duplicate/empty keys,
integer boundaries, source/record admission and missing files. Budget failures
never truncate. Public comparison and reconciliation retain checked partiality;
producer preconditions are not evidence that arbitrary constructed records work.

## Performance and complexity

For B combined input bytes, N combined records, K longest key and
L=ceil(log2(max(1,N))), complete load/merge/preflight/report work is
O(B + N log(max(2,N))) for admitted key lengths. Storage is explicit and bounded
by input, change and output admissions; host reads precede logical admission.
Each source record enters at most one change. Present nested payloads total <=B,
outer keys total <=B, and frame/header overhead is <=23N+13 bytes. Exact output
size is therefore <=2B+23N+13, below the fixed report cap; smaller caller output
limits remain useful and checked. Size arithmetic fits I64 throughout admission.

Before implementation/data collection, freeze an additive whole-pipeline direct
byte/vector-operation gate 128B+64N(K+1)(L+1)+8192, observer schema 2/cap
10,000,000,000. Index merge rounds and duplicate checks contribute <=(2K+6)NL+(2K+2)N; merge key checks
add <=(2K+2)N and value equality scans total <=2B. Input/index initialization and reconciliation/preflight/report
record accesses and pushes have a 64N reserve; framing, serialization and fixed
reports fit 128B+8192. Saturated/unsupported observations stay unknown and fail.
Keep every original catalog/workplan gate. Costs and benefit remain unknown
until artifact-bound measurements; counters exclude runtime/host internals.

## Alternatives and drawbacks

External comparison repeats parsing and binary-output rules. A new general
database or compiler command adds unnecessary scope. Nested reports repeat keys
to keep each present before/after record independently decodable.

## Test and acceptance plan

Use an independent dictionary oracle and canonical Python encoder: reordered,
equal, disjoint, overlapping, binary/prefix keys and values, weight/value changes,
I64 extremes, malformed inputs and limit boundaries. Mutation/negative controls
must detect changed keys, weights, bytes, ordering and omitted changes. Retain
source/artifact identities, allocation-fault sweeps and ASan/UBSan limits. Whole
work families use 64/128/256/512/1,024/2,048/4,096 records per side for equal,
disjoint and changed snapshots, 120-byte common-prefix keys with 32-byte binary
values, and asymmetric N-versus-one inputs (35 rows). Reverse source order on
one side and keep every input beneath existing admission. Dogfood old/current
compiler source-manifest catalogs;
source byte weights remain proxies, never measured compile costs. Coordinator
release gates apply to the exact final source; first validate the heap slice.

## Ratings and evidence

All ratings are neutral zero. Ordinary-source utility, costs and acceptance are
unknown; metadata neutrality proves neither benefit nor absence of cost.

## Decision

Accepted by the coordinator on 2026-10-02 under the maintainer's delegated
overnight implementation and roadmap authority. This accepts only the ordinary
experimental contract above, with neutral ratings and no budget exception.
Implementation followed the coordinator's focused heap correctness acceptance. The
coordinator also accepted the narrow report-size invariant guard on 2026-10-02.

## Implementation

Ordinary-source merge/preflight/report implementation is present. Independent
finite native/sanitizer, malformed/limit/fault and work matrices pass; original
query/planner and coordinator release checks remain source-bound acceptance.
Comparative costs remain unknown pending their separate native measurement slot.

## Removal and supersession

Revise experimental APIs without retaining aliases. Preserve all existing gates
and durable correctness fixtures; retain raw measurements under ignored build.
