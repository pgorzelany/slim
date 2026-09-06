# RFC-0126: Exact revision maps

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

Extend RFC-0125 with explicit exact-content declaration maps between revisions
and indexed full-key lookup, as the source-import foundation for RFC-0124 retained
queries. Adopt the map in production session comparison before semantic reuse.

## Motivation

Typed handles reject stale positions but do not establish how an unchanged
canonical declaration can be imported into a different revision. The current
fallback declaration matcher also scans all prior declarations after reordering.

## Guide-level explanation

A map is produced only for an unchanged declaration under the same compiler/parser
in one session epoch, from an older revision to a newer revision. It translates
an old declaration-local node or contained byte span to a new typed handle.
An old handle itself remains invalid in the new view. Source or key changes,
missing declarations, epoch reset, and incomplete views do not produce a map.

## Reference-level specification

Reuse the existing canonical name trie's byte lookup/insert operations in
`syntax.slim`; do not add another program parser or a hash-only identity test.
A snapshot key traverses module bytes, a kind edge with code 256, 257, or 258,
then name bytes. These three separators cannot equal a source byte, so the full
key is injective without allocating an encoded key buffer. Each declaration kind
has exactly one separator, and out-of-domain kinds invalidate the index. Full key equality is
checked on lookup. Duplicate full keys invalidate the source index with Q0001.
The first-value behavior of the shared trie is unchanged for existing name linking.
Insertion returns the previous value so duplicate checking does not repeat a lookup;
shared prefix traversal helpers also allow full-key lookup without temporary bytes.

Each source state owns an optional lazy declaration index and the actual flat token
count used to validate its views. Aligned keys use direct full equality. On the first
misalignment, build the previous-state index once and retain it for further lookups;
never fall back to repeated whole-list scans. The current canonical name linker
already records the first declaration for a name: require each declaration name
link to name itself, so duplicate names invalidate a state even before lazy index
construction. This uses checked current-revision links, not old semantic facts.
Creating an index and looking up all declarations costs
O(total key bytes), with at most 259 byte/kind edges inspected per character. No new
source-size limit is added; vector allocation failure remains the runtime's
explicit failure. No optional index is silently truncated. Retained service
storage capacities remain a later RFC-0124 implementation obligation.

`prepare_mapping` validates both snapshots' views and source/interface spans,
full module/kind/name equality, exact declaration body bytes, equal canonical
node count, equal epoch, and strictly increasing positive revision serials.
All source validation precedes byte subtraction or comparison. The compiler and
parser context is the current in-process SLIM 0.9 parser; maps cannot be imported
from another process or used after a parser/configuration reset.

An immutable `Mapping` records the two declaration views, their total token counts,
and their body origins. It is compiler-owned explanatory data constructed only
after the preceding checks, not a serialized certificate or semantic authority.
Mapping result enums distinguish missing maps and mapped results explicitly.
A mapped node preserves its local ordinal and changes its complete declaration
owner. A mapped span must be contained in the old body and name its exact file;
subtract the old body start and add the new body start only after containment and
both body bounds are validated. Zero-width body-end spans are supported.

Mapping operations recheck handle ownership and bounds before returning a typed
handle. Incomplete or stale handles return the appropriate missing-result case;
no sentinel becomes a successful position. No map follows deleted declarations
through later history: maps connect the actual adjacent source states only, and
old revision handles cannot address a map whose prior owner is different.

Session comparison resolves an unchanged declaration root and body span through
the prepared map. It uses aligned full-key comparison and indexed fallback instead of linear fallback
search. Optional index construction is deferred until lookup needs it. Changed
interfaces still drive the historical invalidation estimator.
This change does not claim that its syntactic dependency edges are complete or
that any checker, analysis, or generator query is retained.

## Compiler and runtime design

Implement in SLIM using the existing nominal identity records, canonical parser,
byte trie, and runtime. The shared trie exports expose compiler infrastructure
without changing source-language built-ins or name-linking behavior. Reproduce
the portable C seed. Rust supplies independent test expectations and observations.

## Compatibility and migration

Valid session estimate output stays in its existing four-field form. The normal
checker remains the only producer of accepted semantics. No source syntax,
compatibility, runtime ABI, or dependency changes are authorized. Subsequent
retained queries must use these maps before importing current flat views, and
must additionally validate every semantic dependency.

## Diagnostics and failure cases

Invalid source indexes retain Q0001 and status 65. Missing keys, duplicate keys,
changed bytes, wrong epochs/revisions/owners, negative or oversized ordinals,
out-of-body spans, and absent mappings never produce a reusable result.

## Performance and complexity

Measure unchanged and reordered geometric declaration sets before and after.
Count actual index insert/lookup character work and map attempts separately from
semantic queries; fixed observation caps remain explicit under RFC-0121. Index
construction has a storage/latency cost, especially for aligned unchanged source;
record it alongside reordering improvement. No established gate may be relaxed.

## Alternatives and drawbacks

Trusting equal absolute offsets would permit stale source identity reuse. Hash-only
matching would not prove equality. Rechecking a complete declaration body for each
mapped node would turn bulk import quadratic; prepare one immutable exact map per
declaration and use constant-work checked handle translation. An internal map is
not an authentication boundary and is never accepted as user tooling input.

## Test and acceptance plan

Execute production mapping code on adjacent revisions with insertion, deletion,
renaming, reordering, relocation, comments, whitespace, and CRLF. Check every mapped
node and span against the newly parsed current source, including stale handles,
wrong owners, bounds, failed map construction, and deletion/reinsertion history.
Use adversarial full keys and duplicate keys. Preserve all source-identity tests,
bootstrap, governance, Cargo, conformance, required benchmarks, actual-work gates,
and sanitizer/allocation-fault checks. A 128-ordinal reordered-input campaign must
observe a failure after lazy key insertion starts and cross beyond all allocations
of its fixed fixture, with no partial estimate on failure. Record geometric work and uninstrumented
same-host costs; complete M1 only under all RFC-0124 obligations.

## Ratings and evidence

Analysis +2 supplies an explicit exact-content source import relation and full-key
index. Compile -1 records added source metadata and the observed aligned-session cost;
other ratings are zero. The improved reordered lookup is not a claim of free
incremental compilation. Weighted score 5; no performance gate is relaxed.

## Decision

Accepted under the maintainer's RFC-0112 delegation and explicit M1 completion
goal, following RFC-0123 and RFC-0124. This is not independent external review.

## Implementation

Implemented in production `query`, `session`, and the shared `syntax` name trie.
Exact mapping and stale-history tests pass for LF and CRLF inputs. The source-key
index is lazy and retained in its source state; its geometric work gate passes.
Bootstrap, governance, 62 integration tests, 331 conformance fixtures, required
benchmarks, sanitizer and allocation-fault checks pass. See the
[checkpoint evidence](../../benchmarks/results/2026-09-05-slim-next-progress.md),
[paired measurements](../../benchmarks/results/2026-09-06-m1-revision-maps.tsv), and
[actual work](../../benchmarks/results/2026-09-06-m1-revision-map-work.tsv).
This child contract is complete; RFC-0124 and M1 remain incomplete.

## Removal and supersession

Future storage and query orchestration must preserve full-key equality, exact
source ownership, stale-handle regressions, and measured locality. Maps alone do
not discharge semantic dependencies or complete M1.
