# RFC-0167: Linear duplicate-path manifest validation

Status: accepted
Implementation: pending
Process: 1
Audience: developer
Author: Codex, under delegated overnight SLIM development direction
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

Replace only the private quadratic duplicate-path scan in
`selfhost/project.slim` with one temporary byte trie over original raw quoted
path spans. Reuse unchanged `syntax.NameNode`, `NameEdge`, `append_name_node`
and `insert_name_chars`. Keep every original name/membership search, unknown
import traversal, reciprocal-cycle lookup/list scan and cycle call placement.
A scalar-returning private owner helper must physically release the trie before
remaining validators execute, on successful and duplicate returns. No public
API, parser, grammar, source authority, runtime ABI or dependency changes.

This is an accepted narrowing of the previously accepted identity/membership
scope, not adoption of its unsuccessful implementation. The original candidate
and exact failed attempt1 remain held. The independently modeled increasing-E
family, then observed through emitted production C, crossed unchanged exponent
1.15 with 2.7855285159802077. All 41 work rows matched their old oracle; runtime
parity, resources, retained comparisons and adoption were not completed. Do
not relabel that invocation, reinterpret its measured domain or widen budgets.
This replacement withdraws the linear membership/cycle-target claim and
removes that candidate's name root, ManifestLookup enum and altered lookup
interfaces. A query cache or edge-chain rewrite is outside the narrowed scope.

Only duplicate-path index construction plus its conservative eligibility work
has a local approximately linear bound in original key bytes with a finite
byte alphabet. Original E*N searches and cycle target-list work remain explicit
algorithm debt. No whole-manifest or end-to-end project linearity is claimed.
Acceptance requires a separately held path-only oracle before new code and all
unchanged work, parity, resource, same-host and repository gates.

## Motivation

Source inspection proves that N distinct paths reach exactly N(N-1)/2 current/
prior comparisons in `find_duplicate_path`/`find_prior_path`. Span lengths and
equal-length bytes are examined by those comparisons. Reuse of the existing
production SLIM trie can remove this local default-clean quadratic work with a
smaller representation and lifetime than the abandoned membership scope.

The held old compiler and corpus establish actual old outputs/work/timing,
not hypothetical speed. The first candidate's failed geometry is a measured
reason to narrow the architecture. Native benefit, allocations, copying,
peak storage and retained impact of the narrowed candidate remain unknown.
Neutral ratings are not a claim of zero cost. The optional RFC165 full-project
N=4095/E=65536 campaign is separate and supplies no adoption evidence here.

## Guide-level explanation

Projects continue to be accepted solely from canonical manifest/source data by
the ordinary production SLIM compiler. Exact raw quoted-path bytes, including
quotes, define duplicate identity. No normalization, unescaping, filesystem
resolution or file content identity is added. Insert paths in original module
order, remember the first terminal value, and return the earliest repeated
path-node ordinal. Discard the index before reporting that duplicate or doing
later original validation. A proved eligibility result permits optimization;
a decline uses the same existing path validator without a new diagnostic.

## Reference-level specification

### Domain, staging and exact unchanged operations

Preservation applies to immutable source-produced canonical spans in the
positively proved eligibility domain, plus unchanged legacy behavior on a
preflight decline. Arbitrarily forged/mutated public records are not certified;
source-produced malformed token data is explicitly included in the decline
argument below. No source/compiler/session admission is widened.

Keep the exact original order:

1. Existing lexing and version handling, including E0402.
2. Original linear entry lookup before index allocation, retaining E0403.
3. Original `find_unsorted_module`. On its first unsorted node, print E0406,
   evaluate the entire original `find_duplicate_module` scan, and print E0408
   iff its selected node equals that unsorted node. Preserve the scan's partial
   behavior and allocation/output order; do not shorten this error branch.
4. Nonallocating eligibility preflight. On decline call unchanged
   `find_duplicate_path`/`find_prior_path`. On success invoke one private owner
   helper that constructs one path root, inserts raw quoted paths, returns only
   I64 duplicate ordinal or -1, and frees its owned vectors before the caller
   continues. The earliest second original path remains E0408.
5. Original invalid-path, import/export order, self-import and unknown-import
   gates, in the same order: E0407, E0406, E0412, E0411.
6. In `parse_project_manifest`, after exactly a zero `report_manifest_rules`
   result, call original `find_reciprocal_cycle` exactly once and preserve E0413.

Restore the original signatures/bodies of `find_manifest_module`,
`find_unknown_import_in`, `module_imports_name`, `module_cycle_imports`,
`find_reciprocal_cycle` and their traversals. Restore the original cycle call
inside `parse_project_manifest`, outside `report_manifest_rules` and any new
helper. The entry caller, unknown-import caller, unused reciprocal caller and
explicit cycle target caller all retain their original linear lookup. The
unused `module_imports_name` result still requires its original partial call;
both target import-list scans and their exact order remain. No ManifestLookup
or name-index/cache dispatcher, name-chain mutation or retained index remains.

### Malformed-source eligibility and preservation

`syntax.lex_data` tokenizes and indexes form boundaries without a project-shape
gate. `path_is_project` checks the initial spelling, and
`parse_project_manifest` checks version then rules. The separate
`session.manifest_shape_valid` is not a predecessor of ordinary project
checking/capture. Source-produced malformed nodes therefore cannot be excluded
merely by excluding forged AST records. No new structural acceptance rule or
diagnostic is authorized here.

There is a concrete sorted-route boundary: bytes
`(project 1 (entry app) x ) app "foo` followed by one final backslash and EOF.
The module walk treats token 7 (`x`) as a module cursor, its name at token 9
(`app`) matches entry, and token 8 ends the walk. Its path at token 10 is an
unterminated string; `scan_string` can give it end=S+1. The old no-pair path
scan proceeds to the short invalid-path diagnostic without reading that extra
byte. Eager trie insertion could introduce a bounds trap instead. Its exact old native status/output is included in the
held baseline malformed controls; the source argument and finite observation are separate
evidence.

Index eligibility is an optimization fact, not source acceptance. A private
nonallocating preflight proves the byte/AST field domain needed by the index;
failure declines to the same existing production SLIM validator. It neither
parses source again nor diagnoses or accepts a manifest. A positive exact eligibility result selects only the path index;
otherwise call the unchanged original duplicate-path scan. This retained algorithm debt
is not a second grammar, IR, checker or acceptance authority, and must not be
presented as support through a semantic fallback.

The preflight runs after entry and module-order gates, before index mutation,
allocation or output. It must be nontrapping even for malformed source-produced
token data. `token_kind/start/end/next` currently range-check and return sentinel
values on invalid ordinals; `ast_next` can invoke `skip_nested` for an unindexed
open. Do not invoke that partial traversal in the preflight. Read the original
`Token.next` field only after explicitly proving its Vec ordinal. Read no source
bytes and check no head spelling; inspect only the node kinds, links and span
relations needed for the indexed operation. Every failed relation declines.

For S=source byte length and P=token count, use this exact proof geometry:

1. Before any Vec read, prove 0<=cursor<P with explicit `if` branches. Only
   after that proof can P-cursor be computed. Before cursor+K, prove the needed
   remaining range exceeds K; do not rely on eager Bool operators to protect
   access or arithmetic. Before link-1, prove the link is positive and <=P.
2. Check the outer open at 0 has indexed next=P and its close at P-1 is kind 1;
   check the entry open at 3 has indexed next=7 and close 6 is kind 1. All four
   reads are individually guarded. Preserve the existing initial module cursor
   and require it equals 7 for this optimized domain.
3. Visit each module cursor. A kind-1 token completes only at the proved outer
   close. Otherwise require a kind-0 indexed form with next>cursor, next<=P
   and next-cursor>=11. Its close ordinal next-1 is then in range; check its
   kind last in the fixed metadata order below. Only after the range proof form
   +1/+2/+3/+4/+5 ordinals. Require the header/name kinds 2 and raw path kind 3;
   name and path spans must satisfy 0<=start<=end<=S.
4. Its imports open at +4 must be kind 0 with a proved indexed next and
   distance >=3. Require its head kind 2, closing kind 1, and next before the
   module close. Treat that next as the exports open; require its head kind 2,
   proved distance >=3, closing kind 1 and indexed next equal to the module
   close. Every read/offset is range-proved first. No export-atom walk is needed
   because this index does not consume export identities.
5. Walk imports one original atom at a time. A close completes only at the
   proved imports close; other visited tokens must have kind 2 and admitted
   spans. Advance only after proving the next cursor remains within that list.
   Continue modules using the already-proved stored link, never a new AST walk.
6. A shared scalar visit budget admits at most P module/import headers. Check
   before increment, so malformed progress cannot overflow or loop forever in
   the preflight. Successful complete geometry is N+1 module headers and E+N
   import headers, hence 2N+E+1<=P. A bad link, kind, range, span or exhausted
   budget returns decline without mutation/output/allocation.

Read module metadata in this fixed order: module open, head, name, path,
imports open/head/close, exports open/head/close, module close; then visit import
atoms/close. Together with four root/entry reads and the terminating outer
close, a fully eligible input performs exactly 12N+E+5 guarded token reads and
2N+E span checks. Retain this conservative existing preflight without adding
query checks: the narrowed optimization has no indexed name/import queries.
Its name/import metadata checks cost actual observed work and retain the
reviewed progress/domain proof; no extra work may be introduced to improve a
measured exponent.

The preflight may inspect later malformed metadata before an earlier duplicate
path would be diagnosed. All such inspection is proved nontrapping and has no
alloc/io effect, so decline preserves the old earlier diagnostic and partial
walk order. No malformed AST is repaired or normalized. The trailing-escape
counterexample declines on form/span eligibility and retains its original
validator behavior. Successful byte-admitted path keys use exact trie equality; unproved domains
retain the existing operation, without a new success claim or changed
parser/diagnostic contract.

### Private path owner and local preservation

The private scalar-returning helper has original source/tokens/module cursor
inputs and `effects[alloc, partial]`; it constructs and owns checked ordinary
`Vec[syntax.NameNode]` and `Vec[syntax.NameEdge]`. Append one root at ordinal 0
with value -1. Do not return a vector/record, adopt the trie into the caller,
create placeholder vectors on decline, or attach facts to mutable token links.
Original module/path ordinals and original `ast_node_start`/`ast_node_end` spans
supply keys/values. `insert_name_chars` preserves the first terminal; its
nonnegative prior value selects the current path node as the duplicate.

Old equal-length span/byte comparison and trie terminal identity agree exactly
on admitted bytes. Original insertion order preserves the earliest second
occurrence; distinct terminal prefixes remain distinct. No extra source byte
is read outside its positively checked span. The preflight is bounded and
nonallocating and can inspect later metadata without adding a trap/effect;
its decline then restores the exact original path walk, preserving earlier
E0408 before later malformed/invalid data. The unchanged entry/order scans,
subsequent name/list/cycle operations and semantic authority preserve their
original partiality and ordering. This is a local proof, not a certification
of unrelated compiler passes or forged ASTs.

The helper's ownership/lifetime is an independent review condition. A lexical
block, unused owner, scalar result or source-level expectation alone does not
establish physical cleanup. Inspect emitted production C and/or instrument
actual resource events to prove a callee-owned region is destroyed before the
caller reports E0408 or reaches the original invalid-path validator, on BOTH
success and duplicate returns. Reject adoption if it is adopted into the
caller or survives until later validators. Borrowed manifest/source storage
must remain live and unchanged. Do not add a destructor primitive or runtime
ABI to satisfy this condition.

## Compiler and runtime design

Production code stays in `selfhost/project.slim`, reusing current syntax
helpers. Keep public project declarations, fields and effects exact; syntax
manifests/helper interfaces stay unchanged. The existing rules/parser ceilings
remain alloc/io/partial; the new owner helper explicitly declares alloc/partial,
preflight retains its guarded partial ceiling with no alloc/io, and all
membership/cycle helpers retain original effects. Only private path construction
and the reviewed preflight are added. The compiler remains SLIM, not Rust.

For original source length S and canonical node count P, admit every ordinal
below P and span 0<=start<=end<=S before byte traversal. A byte increment
follows cursor<end, so remains within S. For Bp inserted raw path bytes, nodes
<=1+Bp and edges<=Bp. Canonical framing provides the extra non-key byte for
1+Bp<=S; production need not compute an unchecked multiplied work estimate.
NameNode has two I64 fields and NameEdge three, so occupied logical trie cells
<=2+5Bp I64 fields; allocation capacity/layout is separately observed. Preserve
checked vectors/regions and all existing safety checks. Derived arithmetic and
observer counters must be admitted before addition/subtraction/multiplication.
Eager Bool function operands cannot protect partial reads or sentinel indices;
use explicit `if` branches. No overflow, silent truncation or unsafe indexing.

Allocation exhaustion exits 71. New allocation after entry/order and before
path/later gates may preempt a later logical diagnostic and changes failure
ordinals. Do not catch exhaustion and retry a different acceptance route.
Exact diagnostic/failure ordering under resource exhaustion, pre-read physical
bounds and end-to-end RSS/CPU are not promised. Diagnostic printing can itself
allocate/fail; preserve any already-published prefix and host short-write
behavior. No success artifact/output is published before complete validation.

## Compatibility and migration

On unchanged input snapshots, require exact check status/diagnostics and exact
successful formatting, interface bytes, captured source identities and emitted
C. The future compiler's own changed source and regenerated seed naturally have
new identities; compare output for held unchanged user/selfhost inputs instead
of claiming those implementation artifacts stay identical. Cold and retained
routes continue to recompute from the same captured canonical manifest. Rejected
updates cannot publish accepted retained facts, and recovery must match a cold
check of the recovered current source.

## Diagnostics and failure cases

No new source diagnostic is introduced. Under available resources, preserve
code, `-` file label, start/end bytes, count, stdout/stderr channels and ordering
for every reached manifest diagnostic and normal command. Existing reference
rows include:

| Fixture | Exact diagnostic stdout |
|---|---|
| `project-cycle` | `E0413@-@29:30` |
| `project-unknown-import` | `E0411@-@55:62` |
| `project-duplicate-module` | `E0406@-@75:78` then `E0408@-@75:78` |
| `project-duplicate-path` | `E0408@-@83:93` |
| `project-unsorted-modules` | `E0406@-@89:92` |
| `project-list-duplicate-path-precedence` | `E0408@-@89:100` |

Each line retains its existing trailing newline. New tests fix category
precedence when several errors coexist, including an earlier invalid path and
later duplicate raw path, and a first unsorted nonduplicate followed by a later
duplicate module. Keep all existing conformance expectations unchanged.

## Performance and complexity

Define N module records, E imports, Bm declared-name bytes, Bp raw quoted-path
bytes, Be import-name bytes and B=Bm+Bp+Be from disjoint original spans. The
new local scope is path insertion plus the unchanged conservative preflight;
there are no indexed name/import queries. Record all original entry/order,
legacy declined/unsorted, unknown lookup and both target-list scans separately.
Their costs and traces remain exact fixed-data observations; original E*N and
E*target-degree terms are outside the new path-local exponent and linear claim.

### Permanent independent geometry and checked work

The fixed old data oracle SHA-256 492508410de8ed99455f2ab44066449556664e2f6aa316333482a5722a768cf7,
its oracle.json SHA-256 8589f7476ab274ceac3273db35d0cb6fa5cd1f27405ca9a612994e8c671fa82f
and all 3456 fixture-file identities remain immutable. A portable new pure-data
path oracle validates the old oracle SHA BEFORE importing it, recreates the
exact old fixture-data SHA before deriving expectations, and never parses or
accepts supplied SLIM. Importing that new oracle must not write files or launch
native children. ROOT is derived from repository location, with no absolute
host path or ignored timestamp custody required. Keep the new path expectations
separately SHA-pinned and independently held BEFORE narrowed production edits.
The complete original manifests/files/legacy traces remain; only the three
replaced path counters are zero when eligible path construction is reached:
`path_module_headers`, `prior_path_headers`, `path_pairs`. Entry/order cases
remain not-reached; malformed geometry remains unknown until actually observed.

Observe emitted ordinary production C at unique function/loop anchors with
exact anchor counts. Scope syntax insertion counters only to this private path
build, excluding namespace/retained tries. Require observed/uninstrumented
status/output parity. No observation can authorize source acceptance. Count
these seven independent units, including terminal header visits:

| Path counter | Complete bound |
|---|---:|
| builder module headers | N+1 |
| insert calls | N |
| insertion character headers | Bp+N |
| edge headers including misses | 257Bp |
| nodes appended including root | 1+Bp |
| edges appended | Bp |
| terminal value reads | N |

Each node has at most 256 distinct byte edges. A miss includes its terminal
edge header; finite alphabet bounds are deterministic, without hash assumptions.
Their sum W<=260Bp+4N+2. Preflight adds 2N+E+1 header visits,
12N+E+5 guarded token reads and 2N+E span checks. Thus path-local work
<=260Bp+20N+3E+8<=800(B+N+E+1). No query-span counters remain.

Every counter, admitted operand and sum/product has hard cap 1,000,000,000;
check before every increment/addition/product. Refuse out-of-domain/overflow
before evaluating arithmetic, without a successful work report. Permanently
cross cap-1/cap/cap+1 and UINT64_MAX; record actual S/P/N/E/B operands before
native dispatch. The separately derived model publication is at most 128 KiB;
refuse before writing rather than truncating. Exact per-row geometry is primary;
the envelope is an independent ceiling over named units, not all instructions.
Keep all existing project-list gates unchanged. Require measured path-local
endpoint exponent<=1.15 against the SAME B+N+E convention for all six fixed
geometric families. Do not move legacy work into that domain or add dummy work.

Geometric families use N=16,32,64,128,256, with fixed generators and finite
per-family E/key-byte definitions. Use byte-sorted names `m0000` onward, a
constant private function in each module and `main` in the final entry module:

- Five distinct-path and five final-duplicate-path inputs with no imports.
  The duplicate variant gives the final module the first module's raw path.
- Ten known-import inputs: each module after the first imports `m0000` in the
  front family, or its immediate predecessor in the back family. E=N-1.
- Five independently increasing E=16,32,64,128,256 inputs at N=256. Fill edges
  in module order, then target order, using only lower-numbered targets until
  E edges have been placed. Empty remaining import lists stay explicit.
- Four key-length inputs at N=16/E=15 with predecessor imports. Raw quoted
  path lengths are 32,64,128,256 bytes: an `a` prefix of length K-13 followed
  by `/m0000.slim` (vary the five-byte module name) and both quotes.

This fixes 29 geometric inputs. Ordinary paths are `"lib/m0000.slim"` and the
generator varies the five-byte module name. Separately hold twelve diagnostic/
byte controls: prefix-only names/paths; non-ASCII raw path; unknown entry;
first/last unknown import; first unsorted duplicate; first unsorted nonduplicate
with a later duplicate; duplicate path before/after an invalid path; duplicate
path before a list error; self-import; and a reciprocal cycle. Each named
control has one fixed input. Extra variants require a separate prospective
campaign receipt and do not enlarge this completed-matrix claim.

Campaign admission is N<=256, E<=32,768, manifest/each source<=1 MiB,
aggregate manifest plus sources<=4 MiB and parsed node entries<=1,000,000.
These verifier caps do not change normal compiler acceptance. Hold the exact
fixtures, all S/P/N/E/B operands and expected counts before candidate code.
Require exact counts, both checked W/path-local envelopes and path-local
exponent <= 1.15 against B+N+E over each geometric family. Keep the existing
project-list observer/gates unchanged and passing.

### Held original baseline and source-bound campaigns

The completed original baseline is held independently in
`build/overnight-manifest/baseline-held-2` (43 identities), with
`baseline-attempt1/receipt.json`
SHA-256 e65852275f64cd8cd56f19c30a2405140264b68485064a23d5f36c5c5765484a.
It ran 2026-10-02 23:24:13.580032 to 23:24:35.771132 UTC, measured native
elapsed 22.206439750s: 41 work rows, 268 old command observations
(66*4 plus 4 held-selfhost), 31 admitted full timing/noise controls. All 25
malformed inputs*4 commands completed, including sorted-distinct extra-close
acceptance. No unfinished old malformed observation is treated as equivalence.
Old distinct-path pairs are 120/496/2016/8128/32640. Selfhost emission matches
old seed SHA d199 below; source/tool/fixtures stayed unchanged. These results
are exact for that fixed corpus, not broader success or model-active time.

| Original held artifact | SHA256 |
|---|---|
| project.slim | f08e6c788a25aeb8124dfb9008f0671ade45d66305714bc149db686aa925d875 |
| syntax.slim | dcc1e3d2f05647b2c6b0bbcda3873e0facec36e6ee169d30a3fd8ba22a43e832 |
| old production compiler | ee6264e4482e45256064079759b110b6eba99dc73bb06657cdc204b6457c97d6 |
| generated C/seed | d199910449a78e0ec3afb05bd4e18c357389ebe27ed198f9241b241fbd96b711 |

Preserve original accepted RFC/artifacts and failed full-scope attempt1 in
ignored storage before this amendment. That failure receipt is
SHA-256 d3192ca6668fe89db0f3c7a012f1884767bdfafd8e4c78e5fa7cf8fad2059ba7;
all 41 exact geometry rows passed then increasing-E exponent2.7855285159802077
failed. No compare or later native parity/resource/retained run was completed.
The prospective path-only model predicts exponents paths 0.998105037181,
duplicate 1.001268862382, front/back 0.994496653501, edges 0.172335512337,
bytes 0.928878718455; these are predictions, not new native passes.

A permanent portable --current mode regenerates only the fixed 66 data from
SHA-pinned oracles and literal old 264 fixture status/stdout/stderr signatures;
no ignored timestamp/held43 receipt is required in a fresh checkout. Verify
current selfhost source, runtime, compiler executable/generated C, observer,
CC and all fixture identities before/after each campaign, including failures.
Current compiler self-emission must reproduce its supplied generated C;
unchanged held old inputs must preserve original output identities. A pin
failure blocks adoption, not a silently replaced baseline. The ordinary
compiler remains acceptance authority; literals are finite regression data.

Explicit same-host --compare requires user-supplied SHA-pinned old executable
and its corresponding original source/generated-C/runtime set, plus the
current set. Keep 31 fixed fresh timing/control rows (the29 geometric inputs,
ordinary prefix project and held old selfhost). Use 2 warmups per artifact/input,
then 11 balanced alternating baseline/candidate pairs. Record all samples,
medians, ratios and identities. Baseline A/B control ratio must lie within
[1/1.10, 1.10]; a miss is inconclusive, preserves observations and requires a
separate quiet repeat. For EACH predeclared family/row require candidate/old
median<=1.10. A reproducible crossing blocks adoption; no widening after results.
Fresh processes do not imply cold filesystem caches. Report frontend/CC/program
runtime, generated-C/compiler sizes, allocation/storage/copying and memory
separately when observed; unmeasured dimensions, loaded-code ABA and host-boot
uncertainty remain unknown. Passing a ratio does not establish useful speedup.

Retained acceptance uses the unchanged production SLIM retained-project probe
against separately pinned old/current source bundles. Fixed six controls are
warm, edit, reject, recovery, stale and capacity; hold all original outcomes
first, compare exact statuses/output, and require the probe's original cold/
retained equality of manifest/source bytes, tokens/origins/facts/layouts/issues/
plans/C. Rejected updates cannot publish accepted facts. Use the same 2 warmups,
11 balanced pairs, A/B noise admission and 1.10 per-control guard. Probe process
timings include initial cold loading and cold comparison; isolated warm-update
CPU is unknown. Using current source modules for both probes would measure the
same algorithm and is forbidden. Source bundle<=128 files, each <= 1 MiB,
aggregate <= 4 MiB; drift/refused/unfinished controls cannot establish adoption.

Every focused baseline/current/compare campaign has a pre-dispatch monotonic
wall deadline 1800 s, child deadline 60 s, live stdout cap 8 MiB and stderr 256 KiB.
Stop dispatch on breach, kill/reap all native process groups and retain partial
rows plus remaining unknowns. Never extend caps automatically. Required full
repository/release gates have separate root-coordinated execution scopes.

### Fixed resource observation extension

Reuse unchanged `benchmarks/instrumentation/host_resource.{h,c}` and the exact
runtime observer anchors in `scripts/verify-session-host.sh`, only in held
measurement copies of generated C/runtime. Require each original anchor count:
one attempt after atomic attempt increment, one successful payload/header add,
two frees, one realloc-copy anchor. Never edit production runtime or its ABI.
Add a uniquely matched generated-C main observer initialization and snapshots
immediately before/after its successful final root shutdown; refuse ambiguous
anchors. Fresh ordinary/sanitized observer builds use separately pinned old/
current C and runtime sources, strict same tool/flags within each comparison.

Prospectively fix only three inputs: the prefix control, paths-256 and held old
selfhost source. Old/current times ordinary/sanitized gives 12 successful check
observations, each exact status/output parity with its unobserved compiler.
Use 2 snapshots per input/event sequence0 then2, epoch1/serial0. The existing
TSV has exactly14 columns (3 identity and 11 resource fields), row cap 256 and
explicit exact/bounded header. Admit unsigned fields<=UINT64_MAX, nonnegative
I64 identities and supported events before arithmetic; reject malformed data.
Read at most 128 KiB/report, at most 256 rows, and require exactly 2 intended rows for
these controls. Reuse fixed 255/256/257 observer boundary controls; bounded or
missing rows remain unknown and cannot pass the resource obligation. Keep
children/output and overall 1800 s within the focused campaign limits above.

Record runtime attempts, requested payload, live payload and live storage with
headers, separate peak payload/storage-with-headers, and runtime realloc copies.
Host attempted/requested/live/peak storage remains a separate observer dimension.
Requested payload is the existing observer's requested value (size-zero storage
normalization is separate); header-inclusive storage excludes libc bookkeeping.
Only same-snapshot live-with-headers minus live-payload gives live header bytes;
subtracting independent peaks does NOT give peak header storage. Cumulative
requested-header bytes, RSS, libc allocation cost and other source/transport
copying remain unknown. Do not invent them from these fields. Require final
live payload/header/host storage zero; preserve any failure and partial report.

These whole-command resource rows complement, rather than replace, emitted-C
proof that the path owner is destroyed before later validators. Check that
proof on successful AND duplicate helper returns using existing fixed path/
duplicate fixtures. No new allocation budget or native advantage is claimed
before measurement. Unacceptable resource lifetime/cost blocks adoption.

## Test and acceptance plan

All original fixed 66 source/manifests and held 3456 identities remain unchanged:
29 geometric +12 positive/negative/byte controls +25 malformed controls. Hold
new path geometry independently before code. Under ordinary and sanitized
production compiler builds require exact check/fmt/interfaces/emit status,
stdout/stderr signatures for all 66; preserve original conformance/goldens,
client interfaces, captured source/hash identity, current-source self-emission
and unchanged held-source generated C. Every structured work row must match
original traces/all legacy counters except the three replaced eligible path
counters, plus exact new path/preflight geometry and unchanged bounds/exponents.

Preserve prefix/raw non-ASCII paths, empty lists, known front/back imports,
unknown entry/first-last import, unsorted duplicate versus later duplicate,
early/late raw duplicate priority over invalid paths/list errors, self import,
cycle selection and list partial calls. The twelve fixed controls remain the
original plans, not regenerated semantic expectations. All new byte identity
is observed through production SLIM, never accepted by the Python oracle.

The malformed matrix keeps eight suffix mutations after an unsorted distinct
pair, the same eight after a sorted distinct pair, eight after sorted distinct
names with duplicate paths, and the trailing-escape counterexample. Keep
intended module files. Suffixes are truncated module head, head/name,
head/name/path without lists, truncated imports, truncated exports, extra atom,
extra close plus atom and unterminated trailing-escape string. Exact old status/
stdout/stderr prefixes for check/fmt/interfaces/emit are held; candidate may
neither drop an E0406 prefix nor trap before earlier E0408. Each input <= 4 KiB,
child <= 5 s, output <= 64 KiB; malformed subcampaign deadline 900 s starts at first
malformed row, within overall 1800 s. A timeout yields bounded observation with
termination unknown; matching timeouts cannot establish equivalence/adoption.

Keep 12 arithmetic boundaries and observer counter cap-1/cap/cap+1/UINT64_MAX
controls, sparse allocation-failure probes on the original fixed prefix and
unsorted-duplicate controls: ordinals 1,2,4,8,16,32,64,128,256,512,1024,2048,
4096,8192 across ordinary/sanitized variants (56 fixed observations). Preserve
ordinary/sanitized execution and no successful publication after exit 71. Changed allocation ordinals/published diagnostic
prefixes are recorded, never claimed identical under resource failure. Require
actual private-owner cleanup on success and duplicate before later validation.
Keep the fixed resource rows, fresh and all six retained controls, 1.10 guard,
source/fixture invariance and all existing project-list gates.

Before adoption/commit run bootstrap, governance, Cargo tests, quick performance,
reduction, parallelism, compare and agent gates required by AGENTS.md, plus the
repository/release/website scopes required by maintained integration. Reuse
completed evidence only with unchanged source/artifact identities and disclose
that scope. No expected output, passing gate, budget, dependency or safety rule
may be changed to obtain acceptance. Partial campaigns remain incomplete.

## Alternatives and drawbacks

Keep old paths if useful measured benefit or preservation fails. It costs no
new allocation but retains exact quadratic pairs. A normalized-path/hash/sort
representation adds identity/ordering/collision policy where existing exact
byte trie composition suffices. The original name-membership scope crossed its
unchanged geometric gate; preserve the failed evidence and withdraw that scope.
Cost-free one-query/import projects exponent 1.28280458782; realistic cache units
project 1.31393940739, still failing. Adding edge-chain/cache work is outside this
proposal and must not pad a benchmark intercept. Full membership/cycle work
needs a separate measured architecture/preservation argument.

## Ratings and evidence

Safety/Compile/Runtime/Minimal/Analysis/Dogfood all 0; normalized weighted score 0.
Old baseline observations and first candidate work/failure are exact in named
finite domains. New path-local bound is bounded by source eligibility, finite
alphabet and hard arithmetic/campaign limits. Narrowed native timing, allocation,
storage/copying, retained effect, dogfooding value and acceptance remain unknown.
Update ratings only from completed independent evidence. Primitive: none; no
performance-budget relaxation or new source capability is requested.

## Decision

Accepted by the coordinating maintainer under the direct user delegation to
choose and implement the overnight roadmap. Acceptance precedes narrowed
production edits and preserves the independently held path expectations. Original accepted text/artifacts remain held with the
failed full-scope candidate. Acceptance preserves all prior budgets and malformed
behavior and requires scalar-owner cleanup before later validators, original
lookup interfaces/cycle placement and the independently held path expectations.

## Implementation

The path-only private construction is implemented in `selfhost/project.slim`.
Original membership helpers, lookup interfaces, malformed/unsorted routes and
cycle placement are restored. Source and emitted-C review confirm that the
scalar owner destroys its trie region before caller diagnostics and remaining
validation on both success and duplicate returns.

The [current focused result](../../benchmarks/results/manifest-validation-current.json)
records exact work, ordinary/sanitized parity, sparse allocation failures,
resource observations and retained comparisons. All 37 same-host rows passed
the unchanged 1.10 guard, and all six path-local exponents passed 1.15. The
pinned candidate C was adopted as the bootstrap seed; normal bootstrap verified
the fixed point and native hello smoke. Full current-source repository
integration remains pending, so `Implementation: pending` is retained. The
original failed scope and all failed invocations remain held.

## Removal and supersession

Withdraw the narrowed optimization if identity/diagnostic/effect/cleanup parity
fails, local work/exponent/envelope fails, resource cost is unacceptable, 1.10
same-host guard is crossed or usefulness is not demonstrated. Keep old validator
and original failed evidence rather than preserving two permanent indexed
semantic paths. Any later name/membership optimization needs separate acceptance
with an independently frozen scope and cannot silently expand this local claim.
