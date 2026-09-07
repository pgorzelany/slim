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
| M1: compiler substrate | in progress | The current byte-literal checkpoint includes validated internal retained parsing, typing, memory plans, range queries, C fragments, shared checking continuations and the literal-Bytes address repair. RFC-0142 defines pending parameter-input retention. Hexadecimal/trigraph byte literals pass the recorded composite verification and same-host controls. Public host-bound sessions, retained global analyses, native backend caching and full closure remain. |
| M2: expressive safe core | pending | Successor ownership, borrowing, allocation, and generics not implemented. |
| M3: agent and debugger interface | pending | Semantic service and source debugger not implemented. |
| M4: component laboratory | pending | Deterministic providers and replay not implemented. |
| M5: systems contracts | pending | Resource capabilities and successor resource contracts not implemented. |
| M6: kernel substrate | pending | No freestanding kernel claim. |
| M7: OS research slice | pending | No end-to-end OS or productivity claim. |

## Current M1 closure ledger

This ledger includes the validated byte-literal fixed point
`614ac65c7350c3c131d5393571344ead9e298f54dcf1576f91a195d31635675a`
following checkpoint `0f19fbc`; the [evidence index](README.md) preserves dated
costs and checkpoint-specific results. An implemented internal query
is not evidence that the public session already provides that query.

| Obligation | Current result | Remaining closure work |
| --- | --- | --- |
| Revision-owned source, declaration, node and span identities | RFC-0125/0126 implement checked ownership, exact revision maps and source relocation; RFC-0134 adds retained typed place identities. | Preserve these contracts through the public session lifecycle and all configuration changes. |
| Branch/recurrence ownership foundation | RFC-0129/0141 share canonical form and lexical-scope descriptions between the optional graph and explicit checking continuations. The normal checker remains the sole producer of move, availability and loan decisions; all expression families are migrated. | Preserve the validated transitions and source-relative bounds through public session integration. The separate recursive name-resolution prepass retains its documented sanitizer depth limit. |
| Reusable declaration queries | RFC-0130/0131/0132/0135 retain typing and parsing with validated dependencies and current source links. | Exercise the same production queries through real public updates and preserve locality through the remaining analysis integration. |
| Analysis dependencies | RFC-0137/0138 retain memory plans and range queries. RFC-0142 accepts a complete parameter-input query contract, with implementation pending. Current global scans and parallel analysis still execute. | Implement the accepted input-query contract and retain eligible global results with all body/call-site, ordering and budget dependencies; observe real producers and imports separately. |
| Deterministic C emission | RFC-0139 stabilizes private identities; RFC-0140 retains prototypes, bodies and wrappers. Literal Bytes have valid vector/arena insertion addresses and preserve hexadecimal/trigraph boundaries; the existing 96-fixture differential remains raw-C exact. | Keep output assembly/copying and external backend work visible when exposed publicly. |
| Transactional session and corruption recovery | RFC-0133 implements bounded internal epochs, admitted usage, last-good snapshots and failed-update recovery; RFC-0140 tests malformed fragment metadata and damaged bytes. | Bind compiler/runtime/target/options to the actual host, implement public transport and owning-epoch cleanup/reset, and retain explicit last-good revision identities. |
| Native artifact cache | Existing public cache retains whole generated C; the new internal session does not retain native backend artifacts. | Validate complete generated C, runtime, backend, target, flags and link inputs; publish native results transactionally and treat invalid artifacts as misses. |
| Differential, locality and release evidence | The current seed passes required commands and extended stages through an identified interrupted prefix and successful resumed suffix, including 6,144 session fault ordinals and unchanged durable budgets. Quiet baseline controls and exact C/analysis comparisons are recorded. | Run the integrated public-service matrix, geometric locality/latency and full `scripts/verify-0.9.sh` release closure on the final identified checkpoint. |

The public `session` command still reports invalidation estimates. The
M1 milestone remains in progress; none of the remaining rows is waived by the
C-fragment checkpoint. RFC-0141 removes recursive expression inference; the remaining
sanitizer stack limit in `check.find_unknown_expr` is recorded separately.

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

RFC-0142 has an isolated draft, with no production integration or claimed latency
benefit. Its current-candidate input reuse and final integrity checks pass the
96-fixture, six-limit ordinary and sanitizer domain. Previous-snapshot import,
corruption tests, observers, integration and measured costs remain required.

Next: integrate and validate RFC-0142, then complete the public session, global
analysis, native cache and release obligations
in the closure ledger. M2-M7 remain outside this goal.

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
