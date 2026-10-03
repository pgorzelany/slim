# RFC-0170: Checked project-impact context export

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, under delegated overnight SLIM development direction
Created: 2026-10-03
DecisionDate: 2026-10-03
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

Propose one fixed collector/export command, `project-impact-context`, that turns
two successful checked-project captures and the ordinary RFC-0168 impact report
into a canonical JSON context-selection document. Build the fixed RFC-0165
producer and RFC-0168 application through the production SLIM compiler, capture
each snapshot once, reuse the existing inventory adapter, and run the impact
application once. Python interprets their versioned transport/report data only.

The resulting candidate names, path labels, digests and byte weights help a
developer choose source context. They confer no build authority or proof that
the selected context is sufficient. No compiler operation, dependency, runtime
ABI, SLIM parser, manifest parser, reconciliation or closure algorithm is added.

## Motivation

RFC-0168 has a useful bounded four-file core, but its canonical netstrings and
separate producer evidence require several manual steps. A single collector
can make the complete checked workflow reviewable while retaining each
authority boundary. A receipt that merely asserts successful source checking
or contains matching input hashes cannot establish that an invocation occurred.
This command performs and retains the actual fixed invocations; it has no
passive receipt-to-acceptance mode.

Record module counts and supplied catalog byte weights as independent scope
facts. Saved checking time, application quality, agent effectiveness and minimal
context sufficiency remain unknown. The rejected RFC-0169 models and current
RFC-0167 compiler implementation are unrelated to this ordinary tooling proposal
and remain unchanged.

## Guide-level explanation

The proposed command is:

```text
python3 scripts/project-impact-context.py BEFORE.project AFTER.project --output build/context-review-001
```

Optional `--compiler PATH` and `--cc PATH` select trusted tools explicitly; defaults
are the repository's `build/toolchain/slimc` and the operator's resolved `cc`.
No budget, command-template, prebuilt-producer, prebuilt-impact, supplied-report,
receipt-import or alternate-output mode exists. Native children never select
executable paths from input records, reports or receipts.

The operator supplies stopped-writer project snapshots. Python does not open
their manifests or module bodies. The production producer performs its ordinary
capture/check operation. Python hashes only fixed tooling sources, generated
artifacts and supplied serialized bytes. It never follows a candidate path or
loads candidate source text. Captured source bytes are not embedded in JSON.

On success inspect `context.json` and the matching complete collector receipt.
The candidate list is the exact SLIM report selection. Paths are byte labels,
not executable instructions or permission to read/build files. A manifest
change remains visible alongside the report's conservative wider selection.
On failure inspect the retained phase outputs and receipt; no complete context
result is claimed.

## Reference-level specification

### Fixed workflow and authority

Resolve the repository root from the collector's own location, not the caller's
working directory. Admit each CLI path argument at at most 4096 filesystem bytes.
Require every resolved filesystem argument, including ROOT-derived paths and
the output directory, to remain at most4096 bytes before dispatch. Fixed source
registry paths remain relative and at most256 bytes.
Resolve explicitly selected compiler/CC executables once, require ordinary files,
and pin their bytes before and after the run. Record Python identity separately.
Each direct compiler, CC and Python executable is admitted at at most128MiB,
using a fixed64KiB streaming hash buffer and a checked byte count. Refuse before
hashing an observed file-size above the limit, and read at most the remaining
allowance plus one byte to detect growth; refuse an over-limit observed count.
No whole executable buffer is allocated.
The exact accepted identity length stays below the1B integer counter cap.
This records observed direct tool identities, not their transitive SDKs or loaded
code. The collector has a fixed seven-child plan, in this exact order:

1. `[COMPILER, ROOT/project-input.project]` emits checked producer C.
2. Trusted CC builds that captured C with the fixed ordinary flags below.
3. `[COMPILER, ROOT/library/project-impact.project]` emits checked impact C.
4. Trusted CC builds that captured C with the same fixed ordinary flags.
5. The freshly built producer receives the before project argument once.
6. The same producer receives the after project argument once.
7. The freshly built impact application receives before catalog, before graph,
   after catalog and after graph, in that order.

The native production compiler emits C by its manifest argument. The public
launcher's `emit-c` spelling is not passed to `build/toolchain/slimc`.
Each emission must finish with return code 0, nonempty complete stdout and empty
stderr. CC uses `-std=c11 -Wall -Wextra -Werror -O2 -DNDEBUG`, fixed runtime include
and `runtime/slim_rt.c`, and `-x c` for captured emitter stdout. Its stdout and
stderr must be empty on success. No shell, parallel/jobs option or compiler seed
replacement occurs. Producer and impact children require complete return code 0
and empty stderr before their stdout becomes accepted stage data.

After each producer succeeds, execute the existing inventory adapter from
captured, expected-hash-validated Python source with `compile(...,
dont_inherit=True)` and `exec` in a fresh module. Do not use a loader that can
substitute cached bytecode. Reuse `inventory`/`convert`, `Frames`, and `number`
where needed; do not copy the adapter's transport validators. Its expected
source SHA at proposal time is
`004b176cdd01aba975e53f3292cd13a759bba645831279c42fa0dabcb8003c00`.
The source revision is registered explicitly when that helper changes.

The exact before/after producer stdout hashes bind actual successful production
source-checking invocations to the two captures. Adapter measurements bind each
capture to its catalog and unchanged graph. Four frozen file hashes, actual
successful impact invocation, and exact report stdout SHA bind impact authority.
No hash, tag, JSON Boolean or external receipt alone establishes either source
or report acceptance. A historical export requires trusted collector execution
and custody; hashes do not cryptographically attest execution. Reproduction
reruns the fixed collector through production SLIM rather than trusting proof
text. Source-byte equality is not inferred merely from SHA equality.

Pin fixed tooling manifests and a literal union of their declared source paths,
runtime C/header, collector, and inventory adapter before any child and after
publication. The tooling manifests are registered at their accepted identities:
`project-input.project` SHA
`b31b63bab95ddac1d49e719e3537c0f202031347e794c58f7b07b7196e1a9f3f`, and
`library/project-impact.project` SHA
`86a2c62e3e631b35b7b38b23d6f69c994fea38b70e0ab5cbc1dc7d0fec9f5b68`.
Body hashes are collected freshly per run, not permanently tied to historical
body bytes. The fixed path registry is checked against those literal declarations
by an independent control; Python does not parse manifests to discover sources.
Registering a changed tooling manifest/read set requires review before dispatch.
The current producer has27 declared sources and impact has20, sharing four
experimental source files; their union is43. With two manifests, two runtime
files, collector and adapter the fixed source registry has49 entries. Every
registry-relative path is <=256 bytes and each opaque body is <=1 MiB. No caller
project source path is added to this registry or read by Python.
The companion oracle specification enumerates all49 relative registry paths;
pin order is that exact literal order, never source-selected discovery or sorting.

### Report transport validation

Read only the actual successful impact stdout. Preserve RFC-0168's exact
`slim-project-impact-1` grammar, field order, all D/G rows, four root/closure lists,
and current candidate triples. Canonical lengths/counts, strict byte order,
known control spellings, required/absent record slots and exact nested triple
EOF are transport checks. Preserve arbitrary byte values in paths/dependency
fields; do not decode them as text. Key/path/hash/weight shapes use the already
specified RFC-0168 domains. Verify a D row's present nested keys equal its row
key; added/removed/modified slot presence is fixed by the tag.

Do not compare catalogs to discover changes, resolve dependency names, check
graph association, form roots, recompute closures, normalize paths or reinterpret
SLIM. These remain checked SLIM operations. Report roots/closures are copied
even when one is redundant for candidate selection. No unrecognized version,
trailing byte, duplicate/unsorted row, malformed present number, negative value,
Boolean-as-number or missing required observation is accepted.

Frame read caps are exact: tag64 bytes, flag1, every decimal count/weight20,
module/control classification8, every module/list/nested key64, and all nested
record/dependency/value payloads1048576. Classification is one of added/removed/
modified; flag one of0/1. Record names reuse the inventory helper's existing
valid-name alphabet; path/digest shape validation remains nonsemantic transport
validation. A name/path shape does not authorize reading that path.

Use inherited limits unchanged: report 8388608 bytes; frame payload 1048576;
D<=8190; G<=4095; each root/closure/candidate list<=4095; keys 1..64 bytes;
path labels 1..256 non-NUL bytes; digests exactly 64 lowercase hexadecimal bytes;
weights 0..1048576. Reserved keys are exactly `@project`, with path `slim.project`.
An absent record is JSON null, an empty dependency field is hex `""`, and unknown
evidence is an explicit unknown reason. These spellings are never conflated.

### Canonical context document

Emit exactly one ASCII JSON object with one final LF, no insignificant whitespace,
`sort_keys=True`, `ensure_ascii=True`, and no NaN, infinity or floating point fields.
All program/transport bytes and filesystem argument bytes are lowercase hex;
fixed schema/version/control strings and validated digest strings remain ASCII.
Arrays preserve the strict original byte order. JSON object keys are unique.

The top-level keys are exactly `evidence`, `facts`, `format`, `impact`, `selection`,
and `unknown`; `format` is `slim-project-impact-context-1`. Nested fields are fixed
below; the independent oracle checks this exact schema before implementation:

- `evidence`: exactly `collector_sha256`, `inventory_adapter_sha256`,
  `source_set_sha256`, `compiler`, `cc`, `tool_arguments_hex`, `producer`,
  `impact`, `before_capture`, `after_capture`, `inputs`, `report`,
  `before_project_argument_hex`, `after_project_argument_hex`, `successful_roles`.
  `compiler`, `cc`, the captures and `report` are identity objects. `producer`
  and `impact` each contain exactly identity objects `c` and `executable`.
  `tool_arguments_hex` has exactly `compiler` and `cc`, containing the resolved
  operator-selected executable path bytes. `inputs` has exactly identity objects
  `before_catalog`, `before_graph`, `after_catalog`, `after_graph`.
  `successful_roles` is exactly `["emit-producer","build-producer","emit-impact",
  "build-impact","capture-before","capture-after","impact"]`.
  Identity objects contain exactly integer `bytes` and lowercase `sha256`;
  `source_set_sha256` hashes the canonical complete source-pin document bytes.
  Timing stays in the collector receipt, outside this deterministic data object.
- `facts`: exactly integer fields `before_module_count`, `after_module_count`,
  `before_direct_import_edges`, `after_direct_import_edges`, `before_manifest_bytes`,
  `after_manifest_bytes`, `before_module_catalog_weight_bytes`,
  `after_module_catalog_weight_bytes`, `module_change_count`, `import_change_count`,
  `before_root_count`, `after_root_count`, `before_closure_count`,
  `after_closure_count`, `current_candidate_count`,
  `current_candidate_catalog_weight_bytes`.
  Obtain full snapshot dimensions from matching inventory measurements; candidate
  totals are guarded sums of the report's record weights. No fraction, deduplication,
  compiler-work prediction or universal goodness score is emitted.
- `impact`: `manifest_changed` as a JSON Boolean, `before_project`/`after_project`
  records, `module_changes`, `import_changes`, `before_roots_hex`, `after_roots_hex`,
  `before_closure_hex` and `after_closure_hex`. Each module change has exactly
  `change`, `name_hex`, `before`, `after`; each import change has `name_hex`,
  `before_dependencies_hex`, `after_dependencies_hex`.
- `selection`: exactly `snapshot`, `catalog_sha256`, `graph_sha256`, `candidates`;
  `snapshot` is `after`, with the matching after catalog/graph SHA.
  Each record is exactly `name_hex`, `bytes`, `path_hex`, `source_sha256`.
  Reassembling path bytes, one NUL and the ASCII digest recovers the original
  record value. The after reserved record is referenced through `impact`, not
  copied into a second representation. This is a context selection, not a SLIM
  project manifest, build plan or accepted-program representation.
- `unknown`: exactly these key/string pairs:
  `application_invariants`: `not analyzed`;
  `agent_effectiveness`: `not measured`;
  `context_sufficiency`: `declared-import selection is not a sufficiency proof`;
  `saved_compiler_work`: `not measured`;
  `physical_source_deduplication`: `counts and weights count supplied catalog rows`;
  `atomic_live_capture`: `no atomic or final-live identity attestation`;
  `loaded_code_aba_host_boot`: `before/after observed bytes only`;
  `transitive_toolchain`: `direct tools pinned; SDK/linker/transitive identity not established`;
  `physical_memory`: `logical caps are not RSS or libc allocation bounds`.
  Zero selected modules or bytes does not resolve any of these quality questions.

The schema uses all report facts, without default detail tables or optional
formats. A complete source-set inventory and all actual native command observations
remain in fresh attached collector artifacts; their hashes bind the document.

## Compiler and runtime design

Production semantic code remains RFC-0165/RFC-0168 SLIM. The proposed new tool is
one standard-library Python collector with one bounded process runner and one
versioned nonsemantic report projection. Reuse the existing inventory module.
Apply the already verified process discipline: new session/process group,
concurrent bounded pipe draining, remaining whole deadline, and unconditional
group SIGKILL/direct wait even after a leader exits with closed pipes. Mask the
whole alarm through owned-child handoff and bounded cleanup; restore the remaining
absolute deadline without extending model/native execution limits.

The seven roles and executable paths are fixed after operator tool selection.
No recorded command is replayed as code. Child core dumps are disabled; retain
the existing 128 MiB per-file resource bound. Clear existing SLIM fault-injection
environment variables in normal collection and record that policy. Record
external cache/SDK and loaded-code uncertainty rather than claiming hermeticity.

## Compatibility and migration

This is a new experimental tooling command with one canonical output schema.
Existing producer, catalog, workplan, impact, compiler and inventory interfaces,
diagnostics, budgets and source acceptance remain unchanged. There is no source
migration or public runtime/compiler API change. It adds no parser or semantic
acceptance fallback. Existing netstring consumers remain the sole semantic path;
JSON is descriptive tooling data and is never accepted as SLIM source or IR.

## Diagnostics and failure cases

Collector stderr is one fixed ASCII line,
`project-impact-context: CODE in STAGE at POSITION\n`; it never prints source,
candidate path contents, arbitrary exception text or child output. Success stdout
is exactly `project-impact-context: ok\n`. Failure stdout is empty.

Invocation/unsupported option errors exit 64 in `arguments` at 0. Missing/unreadable
tooling data exit 66; invalid tool/source identity or an admitted-byte/count/schema
failure exits 65. Native incomplete/nonzero/timeout/output-limit/cleanup failures
exit 74 in their fixed role stage at 0; actual process status, return code and
bounded diagnostic bytes are retained separately. Collector MemoryError exits71;
a child's exit71 remains that recorded child status, not successful source data.
Output freshness/containment/publication failure exits73 in `output` at0.

The collector code set is `invalid-arguments`, `unsupported-host`, `unsafe-output`,
`tool-error`, `source-error`, `source-identity`, `native-incomplete`, `json-limit`,
`arithmetic-limit`, `publication-error`, `memory-error`. Unsupported host exits64
in `tools`; ordinary tool/source reads use66, pin/domain failures65, as above.
Native process status is independently one of `ok`, `native-error`, `timeout`,
`output-limit`, `infrastructure-error`, `cleanup-error`; successful roles require
`ok`, returncode0, complete pipes and completed group cleanup/direct wait.

Reuse inventory transport error codes and positions at the corresponding capture
stage. Report frame/number failures reuse `leading-zero`, `payload-limit`,
`framing-truncated`, `framing-header`, `framing-comma`, `number-canonical`,
`number-limit`; whole report admission is `report-limit` at0. New report codes
are `report-format` at the tag payload, `report-control` at the offending control
or present/absent slot payload, `report-order` at the first offending key payload,
`report-record` at the first malformed nested field payload, and `report-end` at
the first trailing byte. All exit65 in `report` and nested positions include
their absolute origin. Validate in wire order: tag, flag, reserved records,
D count/rows, G count/rows, four count/list pairs, candidates, final EOF. Within a
row validate its control/name, each nested record in field order, then key/slot
association. The independent oracle freezes exact competing-error positions.
Native
SLIM diagnostics retain their exact bytes in phase outputs. A capped/incomplete
diagnostic is identified as incomplete rather than silently declared equivalent.

The output directory must be a new non-symlink descendant of the repository's
ignored `build/` after parent resolution. Reject an existing directory or aliased
tool/input/output destination before children. Create it exclusively; each artifact
path is fixed, ordinary and fresh. No overwrite, cleanup of older attempts, retry,
silent truncation or directory selected from report paths is permitted.
The before and after project arguments may identify the same stopped-writer
project for an unchanged comparison; output/tool alias refusal does not reject
that ordinary use.

Fully prepare/admit context JSON before writing it exclusively. Require all source,
tool, generated-C, binary, capture, input and report identities stable through
publication; final complete receipt records the context SHA and is written last.
Failure may leave fresh partial files, a context file without a complete receipt,
or retained failed phase outputs. No atomic multi-file publication is claimed.
A successful-looking file alone is insufficient: the caller needs successful
fixed collector completion and the matching complete receipt. Unfinished later
roles remain explicitly unexecuted/unknown, with no manufactured observations.
Source/tool observation endpoints cover context and phase artifact publication,
ending before the final receipt's own write. Its scope is stated explicitly;
before/after hashing cannot exclude a transient or subsequent change.

## Performance and complexity

Bounds are fixed, with no CLI widening: 900 seconds overall, <=60 seconds per child,
live stdout16 MiB only for the two C emitters and8 MiB for other children, stderr
256 KiB each, and <=128 MiB per native output file. Input producer/inventory caps
and the four <=1 MiB impact inputs remain inherited. Logical caps are not preread
physical memory or host RSS bounds.

Context JSON has a checked24 MiB cap. If R is complete report bytes, D/G/C its
row counts, and L the sum of four root/closure counts, a conservative projection
envelope is `2R +256D +128G +128C +16L +262144`. Hex doubles source payload
at most; fixed row punctuation/field overhead is covered by the corresponding
coefficient. The header includes fixed evidence/fact/control data and bounded
argument hex. At admitted maxima this is20,446,400 bytes, below25,165,824.
The independent schema oracle must validate each fixed overhead coefficient,
including maximum CLI argument bytes, before any serializer exists. If it cannot
establish this envelope, refuse the proposal rather than reduce admitted domains.

The static decomposition is explicit. For a record with name length n, decimal
weight width w and path length p, its JSON length is `121+w+2n+2p`. Its original
nested triple contains at least `n+w+p+65` payload bytes, so the JSON record is
already shorter than twice those payloads; triple/outer framing only adds slack.
A D row adds at most47 fixed punctuation/key bytes plus its classification and
duplicated name outside its two nested records. Twice its four frame payloads
covers the classification and name;256 additional bytes cover that fixed47 and
null slots. A G row adds73 fixed bytes, below128; candidate commas add at most1,
below128. A root/closure name adds at most3 punctuation bytes, below16. Reserved
records use the same record bound. The fixed document header has at most256 fixed
ASCII strings of at most256 bytes each,64 integer values of at most10 digits,
four argument-hex values of at most8192 bytes each, and8192 bytes reserved for
their punctuation. This bounds it by107,136 bytes, below262,144. Digests and
fixed strings require no JSON escape expansion. Independent review must count
the actual schema occurrences and verify these ceilings, rather than assume them.

Collector receipt <=2 MiB; fixed source/tool pin documents <=64 KiB each; phase
metadata <=128 KiB each. With seven stdout/stderr pairs and phase records, two
native binaries, four inventory inputs, two adapter receipts, pin documents,
context and final receipt, <=35 fixed artifacts and <=384 MiB retained output
are admitted. No duplicate copy of large emitter/capture/report stdout is needed:
their phase files are the role artifacts. The conservative byte envelope is
368.875 MiB. Bound names/file cardinality and aggregate observed sizes before
claiming completion; these do not bound CC temporary files outside the output tree.

The exact35 permitted relative file names are the21 names obtained by replacing
NN with01 through07 in `phase-NN.stdout.bin`, `phase-NN.stderr.bin`, and
`phase-NN.json`; then `producer`, `impact`, `before-catalog.ns`,
`before-graph.ns`, `after-catalog.ns`, `after-graph.ns`,
`before-inventory.json`, `after-inventory.json`, `sources-before.json`,
`sources-after.json`, `tools-before.json`, `tools-after.json`, `context.json`,
and `receipt.json`. Phase01/03 stdout are the C files,05/06 stdout the captures,
and07 stdout the impact report. The four pin documents are at most64KiB each;
source documents have exactly49 registered entries, tool documents compiler,
CC and Python entries. Tool path bytes are bounded by4096, registry paths by256;
full values are hex, not exception messages. All phase metadata is prepared
within128KiB per phase; final receipt within2MiB. No extra success artifact,
duplicate C/capture/report copy, unbounded command log or emitted candidate file
is admitted. A failed run may retain a subset of these fixed names.

Report projection is O(R+J+row count), with no sort, closure traversal or source
read. Keep exact frame/row visit expectations and separate input/output byte-volume
facts. At most65,531 outer and61,431 nested frame reads occur under the conservative
report count limits; terminal headers are included in the held oracle. All published
counts/sums use exact integer admission, reject Booleans/negatives, and stay within
the existing1,000,000,000 counter cap. JSON preparation/refusal precedes output.
These named units are not all CPU instructions or an end-to-end compiler cost.
The native pipeline's costs are observed separately by role.

The whole deadline applies through final publication/completion. The receipt's
elapsed observation ends immediately before its own final write and shutdown;
the independent campaign records complete CLI wrapper elapsed through publication
and cleanup. Child timing covers dispatch, fresh process, drain, group cleanup
and direct wait; artifact writes are separately scoped. Filesystem cache is
uncontrolled. No
durations are called model-active time or added as independent attribution.
Permanent source/compiler/runtime gates and existing performance budgets remain
mandatory and unchanged; a new collector has no historical speedup baseline.

## Alternatives and drawbacks

Keep the current manual producer/inventory/impact workflow: no extra tooling,
but less convenient evidence binding. Passive report export is smaller but
cannot establish checked source/report authority from asserted receipts alone;
do not keep a second permanent passive mode. Prebuilt caller-provided producer
or impact executables would enlarge the trust seam. Reconciliation/closure in
Python would create a second semantic path and is rejected.

Building the two tools per invocation has a real cost. Fixed guarded execution,
retained failure diagnostics and sufficient provenance also add maintenance.
Do not add a cache, daemon, source loader, automatic build executor or user-selected
output formats to compensate before measuring this narrow command. If measured
ordinary utility does not justify its cost, decline/remove the collector while
preserving the existing SLIM applications and their acceptance evidence.

## Test and acceptance plan

Before implementation, independently freeze an import-safe pure-data oracle,
complete schema/field order, output-size geometry, diagnostic code/offset matrix,
fixed source/argument/report tuples, and expected projections. The oracle neither
loads the future collector nor parses SLIM/manifests. Hold its source SHA and all
model/fixture hashes separately; Python expected data is never source authority.
Use literal report records and the existing independent RFC-0168 expected reports;
native tool/artifact hashes are separately measured inputs to the expected evidence
projection, not input to expected reconciliation or closure decisions.

Prospective finite data campaign:64 transport/scalar controls plus9 geometric
rows, <=128 total cases, oracle model<=4 MiB and fixture aggregate<=16 MiB. Fourteen
positive cases cover empty/unchanged snapshots, modifications, addition/removal,
deletion closure, rename, graph-only/manifest widening, cycles, empty dependencies,
binary/newline path labels, maximum record domains and overlapping root sets.
Twenty-six negative cases cover framing/version/flag/count canonicality and limits,
strict order/duplicates, nested triple/record shape, slot presence and trailing data.
Twelve boundary controls cross private report/JSON admission at size-1/size/size+1
and name/path at limit-1/limit/limit+1. Twelve arithmetic controls retain cap-1/cap/
cap+1, UINT64_MAX, Boolean, negative, guarded increment/sum/product observations.
The independent oracle must enumerate every row and exact expected outcome before
collector code. Geometric rows use N16..256 and path bytes32..256, preserving the
same report facts and testing bounded projection work/output; no native-work or
agent-benefit inference follows from their byte-volume scaling.

Run six fixed production source pairs through the actual seven-child collector:
unchanged tiny, changed tiny, deletion/rename tiny, manifest widening tiny, compiler
body-newline pair and existing catalog body-newline pair. The latter two inherit
RFC-0168's fixed declarations/manifest pins and fresh opaque current-body hashes.
Expect all source/report facts from independently held reports. Require source
checking through the production compiler, exact byte projections, complete four
input/report associations, before/after pins and preservation of inherited gates.
Do not call a fixed control executable production source acceptance.

Negative workflow controls cover an invalid before and invalid after project,
each failed build/emitter stage, timeout/output-limit, source/artifact identity
change, malformed successful-output transport, existing output/alias refusal,
and host partial-publication failure. A fixed closed-pipe descendant control
checks unconditional group cleanup and bounded reap. Later roles must remain
unexecuted after their first failed prerequisite. Exact native labels and total
campaign count are independently sealed before dispatch; focus is hard-bounded
900 s overall/60 s each with fresh outputs and no retries. Test mocks establish
collector control behavior only; real source/impact acceptance rows use production
SLIM. Keep every failure/inconclusive observation.

Prospectively fix the native plan at99 child observations: six complete source
pairs42; one cleanup control1; invalid before/after pairs stop at child5/6, totaling11;
four emission/build failure controls stop at children1/2/3/4, totaling10;
one last-role timeout and one last-role output-limit control7 each, totaling14;
and source-identity drift, malformed last-role successful-output transport, and
partial publication controls7 each, totaling21. CLI/freshness/alias refusals add
no native children. Fail controls use fixed, pinned test tools/copies and never
claim production source acceptance. Their source mutation is confined to copied
tooling trees; current production/held source stays unchanged. The independent
oracle must hold every label, exact early-stop expectation and fixed fixture byte
limit before code. Any unavailable control remains unknown and blocks completion.

The companion prospective `oracle-campaign-spec.md` spells the exact P01..P14,
N01..N26, B01..B12, A01..A12 and G01..G09 rows, symbolic absolute frame positions,
and every99 native label/early-stop expectation. Its boundary-report/JSON controls
are explicitly scalar logical admissions, not claims that valid reports reach
default caps. Exact integer offsets, literal fixtures and overhead occurrence
counts are independently materialized and held before acceptance/implementation.

Maintainer acceptance and oracle hold precede implementation. Focused acceptance,
complete current-source repository/release checks and explicit adoption review
precede permanent integration. Record actual operator/native costs, code/artifact
size and scope facts independently. Quality and general usefulness stay unknown
without a separately authorized evaluation.

## Ratings and evidence

All six ratings are neutral0 and Score0 pending measured costs/utility. This is
not evidence of zero cost. Existing RFC-0165/0168 authority and bounded applications
are prerequisites; their evidence is reused only at unchanged source/artifact
identities and its exact scope is named. No claimed agent productivity score,
native speedup, new compiler linearity or universal quality metric is proposed.

## Decision

Accepted on 2026-10-03 by the coordinating maintainer under the owner's
explicit delegated development authority. Two independent static reviews and
an independently materialized data hold precede collector implementation.
The held model SHA is `37edfb1d4391ca3c871fd07fb99e61c9bf36685a76af31b6e263eab527143daa`;
freeze SHA is `9d3af4991f4abbf6fb56133e8271745cb7daa83ef51372e4087f266212817eac`.
It contains73 data cases, six source pairs and99 prospective native labels;
data materialization ran zero SLIM compiler/CC invocations. The single command,
schema,49-path registry,35-artifact envelope and fixed900/60-second native limits
are accepted. Actual native authority, failure controls, current-source release
verification and adoption remain pending. No compiler semantics, dependency,
ABI, syntax or existing budget change is accepted.

## Implementation

The canonical collector and independent current-source oracle/verifier are in
`scripts/`. Production SLIM sources, manifests, inventory semantics, compiler seed
and all existing budgets remain unchanged. The original held prototype passes;
the portable current-source campaign independently seals fresh expectations before
loading collector code or starting any child.

Canonical focused acceptance passes73 data rows,99 predeclared native observations,
eight zero-native refusals and six actual production source pairs. All source/tool
endpoints and descendant cleanup controls pass. Observation duration is151.932s;
actual seven-role collections take6.028..6.569s on this host. The current record is
[project-impact-context-current.json](../../benchmarks/results/project-impact-context-current.json).
Its identities preserve the original campaign scope; no candidate-byte result is
an agent effectiveness or saved compilation claim. `scripts/verify.sh` runs the
fixed current-source campaign. Fresh source8b passes repository, release and website verification with Rust tools
built in that exact checkout. The current record retains its source identities and
the historical copied-cache provenance caveat. Combined source9 remains pending.

## Removal and supersession

Decline or remove this collector if authority binding cannot be kept fixed,
transport projection duplicates semantics, any preserved gate/cap fails, or measured
permanent costs outweigh demonstrated ordinary use. Retain discovered failures,
the original SLIM workflow and independent acceptance evidence. A revised JSON
compatibility schema or larger scope requires a separate reviewed contract; no
silent format fallback, second mode or budget widening is allowed.
