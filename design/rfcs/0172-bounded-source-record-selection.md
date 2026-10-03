# RFC-0172: Bounded source-record selection

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, under delegated ordinary-library development direction
Created: 2026-10-03
DecisionDate: 2026-10-03
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Add one ordinary SLIM application that selects complete opaque source records
from the existing RFC165 capture. It reads one transport file and exact module
arguments, then emits one canonical source-data artifact. No source or manifest
parser, path dereference, compiler primitive, dependency or RFC170 command change.

## Motivation

The current impact/context export identifies candidate names and supplied byte
weights. A person still needs the corresponding source bytes. RFC165 already
captures checked source once. Selecting records from those bytes makes that
explicit workflow concrete without reopening data-selected paths. Actual utility,
model context use, sufficient/minimal context and saved work remain unknown.

## Guide-level explanation

Canonical invocation:
`./slimc run library/source-context.project -- CAPTURE.ns MODULE...`.
Arguments are exact, nonempty byte keys of at most64 bytes, with at most4095
queries in strictly ascending unsigned-byte order. Do not normalize, sort or
deduplicate. POSIX argv cannot express NUL; OS argument capacity is separate
from the logical bounds. Capture keys may contain NUL or non-UTF8 bytes.

Validate the complete capture even with zero queries. Resolve every requested
key before building output. A missing key rejects the whole selection. Output
preserves requested name/path/body bytes in query order. It omits the manifest
and graph. Paths are labels; no captured path is opened or executed.

## Reference-level specification

Input is canonical netstrings: identity `slim-project-input-1`, module counter N,
edge counter E, manifest frame, graph frame, then exactly N name/path/body
triples and no trailing bytes. Parse the first identity frame with the general
1MiB payload cap, then compare the literal identity bytes; there is no special
20B frame cap. Counters are canonical unsigned decimal: `0`,
or a nonzero ASCII digit followed by ASCII digits. Counter frames have at most
16 payload bytes. Admit N<=4095, E<=65536. E and graph contents are opaque
measurements; no dependency association, graph or source checking is performed.

Names have1..64 arbitrary bytes, paths0..256 arbitrary bytes. Every manifest,
graph and body payload has at most1MiB. Manifest plus all bodies has at most
4MiB; graph has its separate1MiB cap. Input is at most8MiB after the ordinary
whole-file read. N=0 and empty manifest/graph are structurally valid. Capture
order is arbitrary, but keys must be unique. These rules admit arbitrary source
bytes, including invalid SLIM. Program acceptance remains exclusively the
successful production checker/producer invocation and matching retained receipt.
Absent that authority is unknown, not inferred from transport structure.

Validate arguments first: required capture argument, query count, then each
query's width and comparison with its predecessor in left-to-right order.
Read the capture once. Visit identity, N, E, manifest, graph, then each triple
in physical order. Each required frame must exist. Check counter syntax before
its value; check name nonemptiness immediately after its frame. Check aggregate
source admission immediately after each body's frame, before the next triple.
Then reject trailing bytes, build the byte index and reject duplicates, resolve
queries in argument order, measure, build and verify the complete output.

Output is canonical netstrings: identity `slim-source-context-1`, selected
counter Q, then exactly Q requested name/path/body triples. Preserve every
payload byte. Premeasure exact size, enforce8MiB, build the whole buffer and
confirm its measured length before the single stdout publication. Allocation
failure during preparation publishes no successful prefix. A later host output
failure retains the runtime's partial-write/trap behavior; stdout is not atomic.

Prepared records, spans, ordinals and index values must be validator-built and
unmodified. Forged public values, invalid spans/vectors and mismatched owners
retain checked partiality; this proposal does not establish opaque types.

## Compiler and runtime design

Implement under `library/applications/source_context/` with the ordinary project
manifest. Compose std_netstring, std_decimal, std_byte_index, std_bytes,
std_text and project_input_limits. framed_records has a numeric middle field
and is unsuitable for name/path/body triples. Do not change its contract.

Store bounded frame spans and build one existing stable byte index over names.
Each query uses index.find once; do not scan all records for every query.
Preparation/report helpers explicitly declare alloc/partial; the file wrapper
declares alloc/io/partial. Return structured failure before printing. Existing
checked arithmetic, runtime status propagation and region destruction remain.
No second accepted program representation, public compiler API or semantic
fallback. The emitted artifact is non-executable tooling data.

## Compatibility and migration

This is a separate record-selection operation. RFC165 producer, adapter,
RFC168 impact and RFC170 public collector CLI remain unchanged. No accepted
source format is added. Stop snapshot writers during checked capture and retain
the trusted invocation/receipt separately; this application cannot make a live
filesystem snapshot atomic or authenticate supplied bytes.

## Diagnostics and failure cases

One application diagnostic on stdout is
`error CODE in arguments|capture|queries|output at POSITION\n`.
Exit64 invocation,65 invalid transport/selection,66 failed capture read. Argument
code2 and read code1 use position0. Input width above8MiB is code10/capture/0,
exit65, before transport validation. No successful report accompanies an error.

Preserve std_netstring codes1..7 and byte offsets. Identity1MiB and N/E16B frame
length admission precedes tag comparison or counter syntax/value checking.
Its End result when a frame
is required becomes code8 at that cursor. Code30 wrong identity uses its payload
start;31 noncanonical counter and32 counter cap use that counter's payload
start;33 empty name uses its payload start;35 aggregate overflow uses the first
excess body's payload start;36 trailing bytes uses the complete-transport cursor.
Code34 duplicates uses the existing byte_index result: second physical occurrence
of the lexical first duplicated key, due to stable sorting. It does not promise
the first duplicate encountered in original order. Duplicates precede missing
queries; code40/queries uses the first missing query's zero-based ordinal.
Code37/output/0 is the defensive output-size/build disagreement rejection.

Malformed later frames and trailing bytes precede duplicate diagnostics. Invalid
arguments prevent reading even a capture FIFO; no embedded path is ever read.
Allocation failure retains71 with empty stdout and the runtime allocation
diagnostic. Host output failure remains partial. Physical RSS, libc allocation,
host toolchain closure, concurrent writer/ABA behavior and selection of a NUL
key through argv remain unknown or unrepresentable, as appropriate.

## Performance and complexity

For total transport bytes B, records N and queries Q, use bounded key width to
state O(B + N logN + Q logN), plus explicit output copying. Inherited byte-index
sorting/lookup costs retain their own permanent tests. No new general work or
performance-improvement claim follows from fixture dimensions or elapsed time.
Keep the existing ordinary-library application emit/check budget2.00 unchanged.

For a payload of length x, canonical framing adds decimal_digits(x)+2 bytes.
A maximal name frame is68B, path frame261B, body overhead at most9B: at most
338B per selected record besides body bytes. Output header is25+7=32B. Thus
output<=4,194,304+4095*338+32=5,578,446B<8MiB. This is conservative, not a claim
that its maximum is attained. The output cap crossing is unreachable for
validator-built admitted inputs; retain the guard/proof without fabricated
positive crossings or an override to admit forged prepared values.

The capture's remaining headers are24+7+8+9+9=57B. With manifest included in
the4MiB aggregate and graph separate, valid capture<=4,194,304+1,048,576+
4095*338+57=6,627,047B<8MiB. Input8MiB-/=/+ tests therefore use fixed malformed
trailing bytes: the first two reach trailing-data rejection; +1 reaches read
admission. These are not valid-source boundary positives.

Fixed prospective budgets:900s whole campaign,60s per native child;16MiB live
stdout for C emitters,8MiB otherwise,256KiB stderr,128MiB native artifact;
unconditional killpg/direct wait and bounded drain; no retries or overrides.
Independent data cap64MiB,192 files including receipt,4MiB model,64 constructor
cards plus separately4 geometric inputs and3 real bundles:71 distinct scenarios
within a total cap72. This prospective revision of the unaccepted, unmaterialized
TEXT001 scope precedes every observation; accepted/passing budgets are unchanged.
Before/after source, tool, fixture and artifact identities and UTC plus
monotonic times are retained. Publication/endpoint work stays within the whole
deadline; receipt's own write/shutdown are outside inner elapsed, included by
the outer task-time wrapper. Existing budgets are not relaxed.

## Alternatives and drawbacks

Opening selected paths would discard captured-byte custody. Adding bodies to
RFC170 changes a separate accepted operation. Reusing a semantic context query
would conflate record selection with checked declaration evidence. Full capture
handoff is simpler but emits unwanted bodies; subset selection adds maintenance,
index allocation and explicit copying. No automatic selection policy is proposed.

## Test and acceptance plan

Review the import-safe pure-data oracle and fixed campaign specification before
data hold, acceptance or source implementation. It constructs literal frames and
explicit expected selected triples; it parses no input/source/manifest, executes
no checker or future application, and imports no production helper. Hold all
bytes/expected statuses/offsets before native. Sixty-four fixed constructor cards cover zero,
full/subset/empty selection, arbitrary keys/path/body bytes, all rejection
priorities and reachable N/Q/name/path/payload/source bounds. Selected empty
path/body, empty/middle-nondigit counters, exact16-digit vs17-byte N/E frame
admission and identity1MiB+1 header-only diagnostics are explicit cards.
Four geometric
record/query fixtures64/128/256/512 run both ordinary and address/undefined
sanitizers. A fixed1..32 allocation ordinal campaign runs both builds; an
unreached ordinal may only produce the complete independently expected report.
No allocation coverage beyond32 is inferred.

The fixed plan has218 native leaves:6 check/emit/build,1 descendant cleanup,
128 data observations,2 argument-before-FIFO-read controls,8 geometry,64 OOM,
and9 real-workflow leaves. Real compiler35, ledger9 and catalog13 captures are
produced by the actual production producer, compared byte-for-byte with literal
declarations and independently held current opaque bodies, then each selected
subset runs ordinary and sanitized application. Checker acceptance is actual;
oracle hashes alone establish only data identity. Keep source/body/tool pins
before/after, exact complete raw receipts and all failure/unknown later labels.
Add one small corpus golden and its unchanged2.00 row only after focus passes.

## Ratings and evidence

All ratings0/score0 are neutral metadata, not no-cost evidence. The ordinary
library composition and exact preservation relation are prospectively specified.
Source checking, native behavior, resource observations, generated C cleanup,
formatting, full integration and practical usefulness remain unknown before
their respective executions/reviews. No agent/model efficacy claim is made.

## Decision

Accepted on 2026-10-03 under the maintainer's explicit delegated authority
to choose ordinary SLIM improvements during the overnight window. Independent
text, freezer and held-data reviews precede this decision. The fixed model is
`bcfb7299e2fef214287b97a42b2cfd7089a33108d244c9e1bdc7e3fc5a2e0320`;
the161-file hold is
`cd40f322564807a48f0857942cf7a639698bc78e298ef93a4d11d908c20badd1`.
These identities establish prospective expected-data custody only. All source,
native and full integration acceptance remains pending; no budget is relaxed.

## Implementation

The ordinary SLIM selector is implemented in `library/source-context.project`.
Independently audited focused and canonical fresh-current campaigns passed all 218
leaves across 64 literal cards, 4 geometries and 3 actual production captures,
including ordinary/sanitized selection, bounded allocation faults and group
cleanup. Complete held data and original artifact/source/control/tool endpoints
matched; canonical formatting also passed. Source9 code 710bc29 passed repository, release and website gates.
Later metadata is separate from that code tree. The
[current record](../../benchmarks/results/source-context-current.json) distinguishes
both tested RFC identities from this later note. Context sufficiency, model
benefit and physical memory bounds remain unknown.

## Removal and supersession

Reject or remove this operation if record preservation/custody, whole-preparation
failure behavior, fixed resource bounds or unchanged corpus budget cannot be
maintained. Any later source-aware selection or compiler acceptance authority
requires a separate accepted contract and independent evidence.
