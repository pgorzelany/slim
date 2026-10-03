# RFC-0160: Bounded source data components

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, at the project maintainer's delegated overnight direction
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

Add experimental ordinary SLIM netstring framing and byte-span indexing, used
by a framed record catalog and a dependency work planner. This is an RFC-0112
application implementation slice, not RFC-0111 stable-library acceptance.

## Motivation

Existing maintained applications repeat linear key lookup and specialized text
parsing. Two new consumers exercise reusable binary framing, sorted lookup,
validation, checked aggregation and graph scheduling with current source APIs.

## Guide-level explanation

Projects explicitly list source modules beneath `library/`. A catalog loads
key/weight/value frame triples and queries exact keys or prefixes. A work plan
loads name/cost/dependency-list frame triples and reports deterministic lexical
topological order and critical path completion costs. Both reject incomplete or
invalid records before emitting a successful report.

## Reference-level specification

Frames use canonical netstrings: decimal byte length, colon, payload, comma.
Only zero has a leading zero. Parsing returns end, invalid code/byte offset, or
validated payload span/next position. Encoding appends only validated spans.
Limits are caller-visible and bounded by 1,048,576 payload bytes. Index admission
accepts at most 4,096 entries with keys of at most 256 bytes; its checked builder
validates source spans and rejects duplicates. Sorting is stable byte lexical
merge sorting; exact and prefix queries use binary bounds. Search correctness
applies to builder-produced, unmodified indices; source-visible record fields
are not a new opaque-type mechanism.

Applications reject source larger than 1,048,576 bytes after ordinary host read;
this is a logical admission bound, not a pre-read physical memory bound. Catalog
admission is 4,096 records with nonempty keys of at most 256 bytes. Weights are
complete canonical signed decimal I64 values; query aggregation rejects overflow.
Work plans admit 4,096 tasks, 65,536 dependency edges, names of at most 64 ASCII
alphanumeric/underscore/dash/dot bytes, and nonnegative I64 costs. Dependencies
are comma-separated valid names; duplicate, absent and self dependencies are
errors. Lexically first ready task is selected. Cycles and critical-path I64
addition overflow are explicit errors. Limits never silently truncate output.
All error reports use `error CODE at POSITION\n`; application exit 65 denotes
invalid data, 64 invocation error, and 66 host-read failure. The maintained API
notes specify stable code meanings and offset selection.

## Compiler and runtime design

Use the sole production compiler and existing byte/vector/ownership/effect APIs.
There is no syntax, semantic fallback, runtime ABI, dependency, implicit import,
parent path escape, hidden copying or synchronization. Allocation and partiality
remain declared; no selfhost integration occurs without separate review.

## Compatibility and migration

Existing APIs and source contracts are unchanged. New modules remain experimental.
No stable standard-library export or package-resolution contract is accepted.

## Diagnostics and failure cases

Test exact error codes and offsets for malformed headers, truncation, limits,
duplicate keys/tasks/edges, absent dependencies, cycles and integer overflow.
Ownership and effect misuse receives existing compiler diagnostics.

## Performance and complexity

Index construction is O(n log n) comparisons with bounded key lengths and O(n)
explicit scratch storage. Catalog query uses binary bounds plus returned records.
The planner resolves names by binary lookup and selects ready lexical ranks
through a private minimum heap. Complete load/resolution/schedule/report work
is O(B + (n + edges) log(max(2,n))) for admitted bounded key lengths, with B
input bytes and O(n + edges) explicit storage. The heap preallocates n I64
slots and admits each task once at indegree zero; this allocation is observable.
No default compiler pass changes. Preserve every existing budget; add the established 2.00
application emit/check ratio gate for each new corpus application under
RFC-0030: only two workload rows extend its maintained corpus; metric definition
and every existing limit are unchanged. RFC-0030 owns these budgets, as for all
existing library corpus members. RFC-0160 owns the ordinary source consumers. Record source,
artifact identities, geometric work and independent native costs separately.

## Alternatives and drawbacks

Specialized parsers and repeated scans reduce initial module count but prevent
shared API dogfooding. New compiler primitives or generics are unnecessary here.

## Test and acceptance plan

Use an independent Python framing/catalog/graph model, finite generated and
malformed matrices, exact deterministic output checks, sanitized native builds,
allocation faults, public-boundary diagnostic fixtures, and the library corpus.
The root coordinator runs all mandatory compiler/benchmark release gates.
A test-only observer counts direct generated byte reads, vector read/write
accesses and vector pushes across
whole load/query-or-schedule/report. With B input bytes, N records/tasks, E
dependencies, K longest key and L=ceil(log2(max(1,N))), freeze conservative
work bounds 64B+32N(K+1)(L+1)+8192 for catalogs and
32N*N+64B+32(N+E)(K+1)(L+1)+8192 for plans. Each counter has a hard checked
10,000,000,000 cap; saturation is unknown and fails the gate. Geometric sizes
64..4096 include reversed/common-prefix keys, chains, fan-in/out and the
65,535/65,536/65,537 density boundaries. Oracle output parity precedes work
acceptance. These are operation bounds, not latency or physical memory claims.
Preserve that original quadratic planner gate and add the tighter whole-pipeline
gate 64B+32(N+E)(K+1)(L+1)+8192. Add paired-frontier graphs that release a
lower-rank task into an existing ready queue. Each enqueue uses at most 2L+1
observed heap operations; each removal uses at most 3L+1. Index merge and
duplicate checks contribute (2K+6)NL+(2K+2)N, edge lookup/load/release contributes
E((2K+1)(L+2)+16), and remaining task/queue/report accesses are bounded by 48N.
These sums fit the tighter bound for admitted nonempty names; byte loops and
fixed reports use its 64B+8192 reserve. Sift depth and child arithmetic are
bounded by the 4,096-task admission; left can be 8,191 and unused right 8,192.
Keep both prior gates and additionally check the direct-loop sum
64B+N((2K+11)L+2K+52)+E((2K+1)(L+2)+16)+8192. Report byte work is bounded by
a constant times B: task/path names and fixed-width start/finish decimals use
at most 4K_i+82 operations per task, below 8(K_i+10); source records occupy
at least K_i+10 bytes. Framing/name/integer/dependency scans use at most 6B,
and fixed successful/error text uses fewer than 128 operations. Thus the byte
reserve 64B+8192 conservatively covers complete and rejected-prefix work.
Independent oracle, allocation faults and
sanitizer checks precede same-host balanced native comparisons.
Held-out agent repair tasks are authored independently after interfaces freeze.

## Ratings and evidence

All ratings and score are zero: ordinary-source usefulness and costs require
measurements, and metadata neutrality proves no benefit or absence of cost.

## Decision

Accepted by the coordinator on 2026-10-02 under the maintainer's explicit
delegated overnight implementation direction and RFC-0112 authorization. The
accepted boundary is this ordinary experimental source slice and two additive
corpus budgets; it supplies no source/ABI change or performance-gate waiver.

## Implementation

Implemented as ordinary SLIM framing, byte indexing, catalog and dependency
planning. Independent finite ordinary/sanitized behavior, diagnostics, allocation
failure and direct-work gates pass. The heap optimization preserves all prior
budgets and adds the stricter whole-pipeline bound above. Source tree
`3ec37262c81a26ad9c91833f69fd06946bcd538e` passed repository, reproducible release
and website verification and is pushed as `90de8e9`. Evidence remains bounded
to its named domains; general model effectiveness and physical bounds are unknown.

## Removal and supersession

Experimental modules can be revised without retaining aliases. Preserve all
maintained acceptance fixtures and measured evidence if scope changes.
