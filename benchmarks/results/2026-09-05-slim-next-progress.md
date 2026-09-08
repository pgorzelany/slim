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
| M1: compiler substrate | in progress | The current byte-literal checkpoint includes validated internal retained parsing, typing, memory plans, range queries, C fragments, shared checking continuations and the literal-Bytes address repair. RFC-0142 parameter-input retention now passes its complete query, cost, analysis and fault checks. Hexadecimal/trigraph byte literals pass the recorded composite verification and same-host controls. Public host-bound sessions now pass their edit/identity/limit/fault/resource campaign. RFC-0144 now retains bounded parallel analysis with complete-result, corruption, fault, public-session and cost verification. Native backend caching and full closure remain. |
| M2: expressive safe core | pending | Successor ownership, borrowing, allocation, and generics not implemented. |
| M3: agent and debugger interface | pending | Semantic service and source debugger not implemented. |
| M4: component laboratory | pending | Deterministic providers and replay not implemented. |
| M5: systems contracts | pending | Resource capabilities and successor resource contracts not implemented. |
| M6: kernel substrate | pending | No freestanding kernel claim. |
| M7: OS research slice | pending | No end-to-end OS or productivity claim. |

## Current M1 closure ledger

This ledger includes the validated input-query fixed point
`64107135f2677bcada015bfe5cd292bb1c3a8f41216fe2c92ebf7793c9df5f61`
following checkpoint `6705582`; the [evidence index](README.md) preserves dated
costs and checkpoint-specific results. An implemented internal query
is not evidence that the public session already provides that query.

| Obligation | Current result | Remaining closure work |
| --- | --- | --- |
| Revision-owned source, declaration, node and span identities | RFC-0125/0126 implement checked ownership, exact revision maps and source relocation; RFC-0134 adds retained typed place identities. | Preserve these contracts through the public session lifecycle and all configuration changes. |
| Branch/recurrence ownership foundation | RFC-0129/0141 share canonical form and lexical-scope descriptions between the optional graph and explicit checking continuations. The normal checker remains the sole producer of move, availability and loan decisions; all expression families are migrated. | Preserve the validated transitions and source-relative bounds through public session integration. The separate recursive name-resolution prepass retains its documented sanitizer depth limit. |
| Reusable declaration queries | RFC-0130/0131/0132/0135 retain typing and parsing with validated dependencies and current source links. | Exercise the same production queries through real public updates and preserve locality through the remaining analysis integration. |
| Analysis dependencies | RFC-0137/0138 retain memory plans and range queries. RFC-0142 retains complete parameter-input queries with validated caller contributions. RFC-0144 retains the complete bounded parallel result when its selected checked/range/work inputs agree. | Preserve complete blockers, ordering, budget dependencies and independently observed producer/import work through native-cache integration. |
| Deterministic C emission | RFC-0139 stabilizes private identities; RFC-0140 retains prototypes, bodies and wrappers. Literal Bytes have valid vector/arena insertion addresses and preserve hexadecimal/trigraph boundaries; the existing 96-fixture differential remains raw-C exact. | Keep output assembly/copying and external backend work visible when exposed publicly. |
| Transactional session and corruption recovery | RFC-0133 implements bounded internal epochs and last-good snapshots; RFC-0140 tests damaged fragments. RFC-0143 now exposes framed host-bound updates, physical reset, source admission and atomic executable publication. | The public edit/limit/fault/resource matrix passes; clean installation passed at `2ffc37b`. Preserve these and existing internal corruption oracles through full M1 release closure. |
| Native artifact cache | Existing public cache retains whole generated C; the new internal session does not retain native backend artifacts. | Validate complete generated C, runtime, backend, target, flags and link inputs; publish native results transactionally and treat invalid artifacts as misses. |
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

Next: implement native object/link artifact caching, then integrated locality,
performance and release closure. Profile the observed
body-edit copying/global costs and preserve the full acceptance matrix. M2-M7 remain outside this goal.

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
