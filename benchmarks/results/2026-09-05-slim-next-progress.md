# SLIM Next implementation and evaluation

Status: M0 complete; M1 in progress; M2-M7 pending
Decision: RFC-0112 accepted by the maintainer on 2026-09-05
Baseline source: 97412bf

The accepted roadmap covers M0-M7. M0 is complete; the current goal is to finish
M1 from checkpoint 0265c6d. M2-M7 implementation is outside this goal. Approval
does not complete a milestone. Production semantics remain in SLIM and the
portable C seed.

## Milestone status

| Milestone | Status | Current evidence |
| --- | --- | --- |
| M0: repair and establish truth | complete | Current-contract repairs, permanent regressions, actual-work counters, claim audit, and RFC-0123 decision boundaries are validated. The complete repository, reproducible release, clean-install, ABI, and website gates pass. |
| M1: compiler substrate | in progress | Retained parsing, typing, memory/range/input/parallel analysis and C fragments pass their recorded query and public-session campaigns. RFC-0146 native object/runtime/link retention now passes the default host's complete native campaign. Edited native builds are faster in paired measurements, with substantial connection setup cost. A measured startup optimization and full integrated/release closure are under verification. |
| M2: expressive safe core | pending | Successor ownership, borrowing, allocation, and generics not implemented. |
| M3: agent and debugger interface | pending | Semantic service and source debugger not implemented. |
| M4: component laboratory | pending | Deterministic providers and replay not implemented. |
| M5: systems contracts | pending | Resource capabilities and successor resource contracts not implemented. |
| M6: kernel substrate | pending | No freestanding kernel claim. |
| M7: OS research slice | pending | No end-to-end OS or productivity claim. |

## Current M1 closure ledger

The current default seed is
`b418ef1cd9983e82bac293ffb50d76e144a765c319fe201d23b025340e93d2d0`.
The [evidence index](README.md) and dated reports preserve costs and exact
checkpoint identities. The initial native aggregate passed before the subsequent
capture-validation optimization; final integrated verification is in progress.

| Obligation | Current result | Remaining closure work |
| --- | --- | --- |
| Revision-owned source, declaration, node and span identities | RFC-0125/0126 implement checked ownership, exact revision maps and source relocation; RFC-0134 adds retained typed place identities. | Preserve these contracts through the public session lifecycle and all configuration changes. |
| Branch/recurrence ownership foundation | RFC-0129/0141 share canonical form and lexical-scope descriptions between the optional graph and explicit checking continuations. The normal checker remains the sole producer of move, availability and loan decisions; all expression families are migrated. | Preserve the validated transitions and source-relative bounds through public session integration. The separate recursive name-resolution prepass retains its documented sanitizer depth limit. |
| Reusable declaration queries | RFC-0130/0131/0132/0135 retain typing and parsing with validated dependencies and current source links. | Exercise the same production queries through real public updates and preserve locality through the remaining analysis integration. |
| Analysis dependencies | RFC-0137/0138 retain memory plans and range queries. RFC-0142 retains complete parameter-input queries with validated caller contributions. RFC-0144 retains the complete bounded parallel result when its selected checked/range/work inputs agree. | Preserve complete blockers, ordering, budget dependencies and independently observed producer/import work through native-cache integration. |
| Deterministic C emission | RFC-0139 stabilizes private identities; RFC-0140 retains prototypes, bodies and wrappers. Literal Bytes have valid vector/arena insertion addresses and preserve hexadecimal/trigraph boundaries; the existing 96-fixture differential remains raw-C exact. | Keep output assembly/copying and external backend work visible when exposed publicly. |
| Transactional session and corruption recovery | RFC-0133 implements bounded internal epochs and last-good snapshots; RFC-0140 tests damaged fragments. RFC-0143 now exposes framed host-bound updates, physical reset, source admission and atomic executable publication. | The public edit/limit/fault/resource matrix passes; clean installation passed at `2ffc37b`. Preserve these and existing internal corruption oracles through full M1 release closure. |
| Native artifact cache | RFC-0145's production SLIM query is connected through RFC-0146 public B/N frames. Default ordinary/sanitized campaigns pass complete input capture, actual backend reuse, all 20 applications, corruption, failures, byte/record capacity, concurrency and physical cleanup. | Revalidate the capture-validation optimization, final public costs and installed native behavior in the complete release gate. |
| Differential, locality and release evidence | The current seed passes all required checkpoint commands, 1,024 retained and 6,144 session fault ordinals, complete parallel-view/report comparisons and quiet timing controls. The prior byte-literal extended verification remains identified separately. | Run the integrated public-service matrix, geometric locality/latency and full `scripts/verify-0.9.sh` release closure on the final identified checkpoint. |

RFC-0143 exposes the public framed session. The 96-source/30-project corpus and
32 edit pairs pass ordinary and ASan/UBSan differential checks. The independent
1,024 cold and 1,024 update/recovery allocation-fault domains pass, together with
all default epoch limits, diagnostic-growth faults and partial output failures.
Actual query observation confirms zero producer/import entries on unchanged
updates. Repeated resource histories prove zero measured live storage after
reset/exit, while exposing retained allocation and copying costs. Real native
target builds, loaded identities and concurrent publication are checked.

See the [public acceptance report](2026-09-08-m1-host-closure.md) for exact domains,
costs and new durable latency gates. Large body edits remain 22.7% slower than
fresh compilation. The reproducible source package and clean installed session
passed at `2ffc37b`; full M1 release closure is still required. M1 remains in
progress. RFC-0141 removes recursive expression inference; the separate
`check.find_unknown_expr` sanitizer stack limit remains documented.

## Measurement discipline

- Record source/compiler/configuration identity and work actually performed.
- Compare native compiler latency on the same host with interleaved baseline
  and candidate samples, including warmup; preserve geometric scaling fixtures.
- Record agent time, tokens, tool calls, failed attempts, and task acceptance
  independently. Source token counts and repair edit size remain proxies.
- This ongoing implementation is an observational development trace, not a
  controlled language comparison. Its productivity effect is unknown.
- Controlled agent comparisons need fixed tasks, independent acceptance tests,
  equivalent tools/libraries, held-out cases, and a declared execution budget.
- Existing performance budgets remain in force. A faster result does not
  establish safety, nor does a passing safety fixture establish complete safety.

## Latest checkpoint and next work

The byte-literal checkpoint seed is
`614ac65c7350c3c131d5393571344ead9e298f54dcf1576f91a195d31635675a`.
Its exact native byte oracle, C/analysis differential and 26 same-host timing cells
pass. The original full verification run was interrupted at the maintainer's request
with exit 143 during place fault ordinal 1,850. A resumed suffix exits zero after
rerunning the complete place campaign and every remaining stage. All required
commands and extended stages pass across these two identified runs; there is no
claim of one uninterrupted full invocation. See the
[byte-literal checkpoint report](2026-09-07-m1-byte-literals.md).

RFC-0142's validated input-query seed is
`64107135f2677bcada015bfe5cd292bb1c3a8f41216fe2c92ebf7793c9df5f61`.
Complete input/corruption/edit oracles, geometric actual-work gates, all required
checkpoint commands, complete analysis-consumer comparisons, 1,024 retained
faults and 6,144 session faults pass. Clean and retained timing controls pass after
two measured cost refinements. Earlier regressions, physical memory/copy costs,
source identities and verification logs are preserved in the
[checkpoint report](2026-09-07-m1-input-queries.md) and compressed evidence.
No significant latency or agent-productivity improvement is inferred.

RFC-0144's verified parallel-query seed is
`9959671c67bdce89f1826230aa90827a722319dbf5fb787bb7ad2a23180df963`.
Its 323 complete-result comparisons, corruption and allocation-fault cases,
public-session matrix, native analysis/resource baselines and required checkpoint
commands pass. Matched call-heavy outside-prefix edits measure about 10.8% to
1.4% faster across 125-4,000 helpers; simple edits show little benefit and some
small overhead. The generated seed grows 1.96%. Large body edits still take
longer than fresh frontend compilation; no agent-productivity gain is claimed.
See the [checkpoint report](2026-09-08-m1-parallel-query.md).

The [RFC-0145 native query checkpoint](2026-09-08-m1-native-query.md) now
passes private artifact, corruption, ownership and geometric work tests. All
required checkpoint checks pass; no public native reuse is claimed.

RFC-0146's uncommitted host integration now passes native frame and full
20-application corpus campaigns through private candidate host
`e4c06b7abb687d09944dcd13ebb3b50579d8207d8100e3486e84b8f6f94d07be`.
Ordinary and ASan/UBSan hosts each pass 80 application/tier/epoch results against
ordinary serial/POSIX builds, with exact repeat/reset executable bytes. The
instrumented host independently observes every backend start; unchanged results
start none. The two parallel applications also match both argument branches.
Basic edit/revert/rejection/last-good transitions continue to pass. A sanitized
host fixture checks exact captured-file membership, required link inputs, report
framing and fixed manifest/report limits without allocation. Two initial corpus
harness failures and the passing campaign are preserved in the
[compressed evidence](archive/2026-09-08-m1-native-host-corpus.json.gz), with source
hashes and the candidate's build identity. These are correctness/work observations,
not matched latency measurements or a demonstrated agent-productivity gain.
The default bootstrap/installed host has not been refreshed for this integration.
Full context/fault/cleanup/resource verification and paired native costs remain
pending; this does not close native integration or M1. Captured native inputs are immutable
for the connection; reset physically clears query storage, and a new connection
refreshes tools. This separates expensive setup from repeated source epochs.

The subsequent lifecycle candidate
`c7909ce99db526ff010b432f4fce991d2592223c577ca566b9f4cf896ed212b6`
passes the same corpus, plus tool-group termination before leader reaping,
actual file/diagnostic limits, process-query 512/513 bounds, malformed and
interrupted frames, and six cold-path native allocation failures with observed
context cleanup. Ordinary and ASan/UBSan tests pass; deadline tests explicitly
advance a verification clock rather than measure a 180-second wait. The
[compressed lifecycle evidence](archive/2026-09-08-m1-native-host-lifetime.json.gz)
retains the failed attempts, passing campaigns, source and host identities.
This remains an uncommitted candidate with the integration and cost work above open.

The input-isolation candidate
`ef953b79eee5a2235b047d4b4e6eb0b1217855dae523de89e8b558b202d1175f`
passes the complete ordinary/sanitizer host campaign, with all 80 corpus rows
equal across hosts. Its 63 driver/dependency cases validate captured jobs and
header inputs, disable default driver configuration, and reject unrecorded input
shapes. Copy-helper tests cross file/byte capacities and detect changed copies.
Removing every private original compiler/SDK path preserves five fresh build and
execution configurations. Five additional public I/O/compiler/linker/cleanup
failure cases pass under ASan/UBSan: failures return no executable, and retries
reuse valid objects and reproduce the expected bytes. The
[compressed input-isolation evidence](archive/2026-09-08-m1-native-host-inputs.json.gz)
records failed attempts, passing runs, exact input identities and test limits.
The aggregate passed before the two new independence/recovery constituents were
added; those constituents passed separately. The extended aggregate still needs
its release run. Default/installed integration, remaining fault/resource cases,
matched costs and M1 closure remain open. These results do not establish a
significant editing-cycle or agent-productivity improvement.

Ten additional public state/terminal campaigns pass under ASan/UBSan on the same
candidate through a verification-only observer. Metadata and per-role artifact
corruption cause conservative rebuilds; reset restores reuse. Both worker profiles
fill the real 256-record limit within 64 source updates, after which valid entries
remain reusable. A reduced-byte-limit fixture separately checks capacity fallback.
Process-query failure and SLIM allocation failure while retaining each native role
produce exact terminal errors and physical teardown. The first record fixture hit
the independent source-attempt gate; it was corrected without changing either
production limit. The [compressed state evidence](archive/2026-09-08-m1-native-host-state.json.gz)
preserves that failed attempt and the passing prefix/suffix campaigns. Production
sources and candidate identity are unchanged; physical resource and integrated
performance/release closure remain open.

The [public resource evidence](archive/2026-09-08-m1-native-host-resources.json.gz)
now crosses the real 64MiB native-byte limit in optimized and sanitized observers.
Seven distinct 2MiB-payload builds reach refusal with 63,464,207 charged bytes and
125,988,576 native-region allocation bytes; valid artifacts still execute. Repeated
builds clone no native-query bytes, and reset frees every owned native allocation
while preserving captured files. The optimized campaign peaks at 209,371,136 bytes
of RSS; sanitizer overhead is reported separately. One connection captures
224,339,295 input bytes (214 files including paired runtime); all context files
total 226,380,462 logical bytes. Block accounting is not unique physical storage
because captures can clone extents. Concurrent captures/edits and independent
reset/quit pass, as does reset recovery after a declined provider. No production
logic or performance budget changed. Default integration, paired latency and
complete release verification remain open.

Default integration now reproduces the candidate's exact seed and host, and the
entire expanded native aggregate passes against the default binary. The
[initial public timing report](2026-09-08-m1-native-session.md) records five paired
samples across ten cases: edited builds improve about 22–52%, and build-plus-run
improves about 13–32%, with stable executable identities on both sides. Median
context setup is 10.84 seconds, requiring an estimated 113–179 edited builds to
recover in these workloads. Short sessions can still lose overall. Main/release
verification wiring is present; remaining checkpoint/release gates are not complete.
Subsequent exact manifest batching reduces standalone capture from 10.80 to 8.77
seconds (three samples per version); all 77 driver/input cases pass. The measured
lookup cost falls from 2.24 seconds to 57 ms without changing capture limits.
The current integrated verification includes a new public timing run.

The clean gate at checkpoint `6f50b8b` subsequently found a concurrent auxiliary-file
publication race. The [publication repair](2026-09-08-m1-host-publication.md) stages
all files privately before atomic per-file publication, with the executable last.
Its complete identity suite and strengthened simultaneous-build check pass;
checkpoint and full clean release verification are being repeated for the repair.

Next: complete integrated locality/performance verification, make a validated
checkpoint, and pass the full release gate. Frontend copying/global costs and
native setup remain visible limitations. Preserve the full acceptance matrix;
M2-M7 remain outside this goal.

## Historical checkpoints

The [evidence index](README.md) locates raw measurements, checkpoint summaries and
the complete archived development history. That history is an immutable snapshot:
its final “verification running” entry was superseded by the interruption and
successful resumed suffix recorded above. Earlier failures, regressions, limitations
and measurements are preserved.
This file records current status; future checkpoint detail belongs in dated reports.

### Checked field replacement checkpoint (2026-09-06)

RFC-0119's historical evidence is retained under this same heading in the
[development history](archive/2026-09-07-slim-next-development-history.md.gz).

### Enum match ownership checkpoint (2026-09-06)

RFC-0118's historical evidence is retained under this same heading in the
[development history](archive/2026-09-07-slim-next-development-history.md.gz).

### Definite reinitialization checkpoint (2026-09-06)

RFC-0120's historical evidence is retained under this same heading in the
[development history](archive/2026-09-07-slim-next-development-history.md.gz).
