# SLIM Next implementation and evaluation

Status: M0 and M1 complete; M2-M7 pending
Decision: RFC-0112 accepted by the maintainer on 2026-09-05
Baseline source: 97412bf
M1 starting checkpoint: 0265c6d

The accepted roadmap covers M0-M7. Production semantics remain in SLIM and the
portable C seed. M1 passed the complete clean release gate at
`4bdb6750656eeeac48b0d68cffc3423600434168`; closure reporting preserves that identity.

## Milestone status

| Milestone | Status | Current evidence or remaining scope |
| --- | --- | --- |
| M0: repair and establish truth | complete | Current-contract repairs, permanent regressions, actual-work counters and RFC-0123 boundaries passed repository, release, installation, ABI and website checks. |
| M1: compiler substrate | complete | Typed identities, bounded control/ownership, retained compiler queries and public frontend/native sessions pass the full repository, release, installed-tool and website gate. |
| M2: expressive safe core | pending | [Dependency slices](2026-09-08-m2-plan.md) and [proposed prerequisite decisions](2026-09-08-m2-prerequisites.md); language implementation has not started. |
| M3: agent and debugger interface | pending | Semantic service and source debugger. |
| M4: component laboratory | pending | Deterministic providers and replay. |
| M5: systems contracts | pending | Resource capabilities and successor resource contracts. |
| M6: kernel substrate | pending | No freestanding kernel claim. |
| M7: OS research slice | pending | No end-to-end OS or productivity claim. |

## Current M1 closure ledger

The current seed is
`b418ef1cd9983e82bac293ffb50d76e144a765c319fe201d23b025340e93d2d0`;
the host identity is
`749c2213fa11cb06d4a81fd6a1af539326c88e37307c0d96d5aff81806800447`.
The [closure report](2026-09-08-m1-closure.md) maps exit criteria to the complete
passing invocation, raw evidence, exact measurements and remaining limitations.

| Obligation | Implemented contract and current verification |
| --- | --- |
| Typed source identities | RFC-0125/0126/0134 provide checked revision/file/declaration/node/span and retained type/binding/place ownership, exact content maps, relocation and stale-link rejection. Extreme-value and complete mapping oracles pass. |
| Control and ownership | RFC-0127/0129/0141 share canonical control and scope descriptions with explicit checking continuations and a bounded optional flow view. The sole normal checker supplies ownership decisions. Extended transition/fault campaigns pass. |
| Declaration queries | RFC-0130/0131/0132/0135 retain typing/parsing through current project validation and complete dependencies, using reverse checked-link adjacency for invalidation. Public updates execute this production path. |
| Analysis dependencies | RFC-0137/0138/0142/0144 retain memory, range, caller-input and complete parallel results, including body-derived facts and ordering/budget inputs. Full-field and independent producer/import comparisons pass. |
| Deterministic C | RFC-0139/0140 retain stable prototypes, bodies and wrappers against consumed lowering inputs. Corrupt metadata misses; clean/retained C equality and caller invalidation tests pass. |
| Transactional sessions | RFC-0133/0143 bind complete loaded identities, publish successful snapshots, preserve explicit last-good artifacts after rejection and physically reset owned storage. Public ordinary/sanitized edit, transport, identity, limit, fault and resource matrices pass. |
| Native artifacts | RFC-0145/0146 retain exact program/runtime/link bytes under captured inputs. Both 20-application corpora, exec observation, corruption, failure, capacity and cleanup checks pass. Ordinary/sanitized selection and the full record/byte-capacity campaigns pass inside the complete release invocation. |
| Release closure | Clean `4bdb675` passes the complete `scripts/verify-0.9.sh`: repository checks, two byte-identical archives, source hashes, clean installation/frontend/native behavior, ABI rejection and all 18 website tests. Earlier failed attempts remain identified in the candidate report. |

## Measured benefits and costs

The identified `4bdb675` same-host public measurements show edited native builds taking
22.79–51.58% less time and build-plus-run taking 8.24–31.42% less time than paired
ordinary builds across ten cases. Unchanged build medians are 0.813–7.008 ms.
Median setup is 6.402 seconds, requiring an estimated 61–96 edited builds to recover
in these workloads, excluding reset cost. Short sessions can lose overall.

Frontend-only body updates at 4,000 helpers remain 17.74% slower than one-shot
compilation. Unchanged updates take 2.104 ms versus 26.300 ms cold. All three
geometric latency exponents and the warm/cold ratio pass their unchanged gates.
Input admission, metadata translation, complete C assembly/copying and whole-C
backend compilation remain visible work. The separate name-resolution prepass
retains its documented sanitizer depth limit.

The native byte-boundary campaign observes 63,464,207 charged bytes, 125,988,576
allocated bytes including headers, and 209,371,136 bytes optimized peak RSS.
Sanitizer overhead is separate. Reset frees all owned native-query storage while
preserving captured tool inputs. File block counts can include shared clone
extents and are not unique physical disk storage.

Earlier sandboxed native measurements and the later authorized full-run timings
have distinct execution contexts. Their absolute setup difference cannot be
attributed solely to manifest batching. The separate matched capture profile
records that optimization's measured effect; all previous results remain identified.

## Measurement discipline

Record source/compiler/configuration identities and actual work. Keep native
latency, resource use, agent time/tokens/tool calls, accepted tasks and source/edit
proxies separate. This development trace is observational; its effect on agent
productivity remains unknown. Controlled comparisons require fixed tasks,
independent acceptance tests, equivalent tools/libraries and held-out cases.
No safety or performance gate was relaxed for the reported results.

## Historical checkpoints

The [evidence index](README.md) links dated reports and immutable compressed records.
The [release-candidate archive](archive/2026-09-08-m1-release-candidate.json.gz)
preserves the prior detailed version of this report and the incremental guide.
Earlier failed attempts, regressions, interruption/resumed-suffix evidence and
costs remain available; they are not relabelled as current uninterrupted passes.

### Checked field replacement checkpoint (2026-09-06)

RFC-0119 evidence remains under this heading in the
[development history](archive/2026-09-07-slim-next-development-history.md.gz).

### Enum match ownership checkpoint (2026-09-06)

RFC-0118 evidence remains under this heading in the
[development history](archive/2026-09-07-slim-next-development-history.md.gz).

### Definite reinitialization checkpoint (2026-09-06)

RFC-0120 evidence remains under this heading in the
[development history](archive/2026-09-07-slim-next-development-history.md.gz).
