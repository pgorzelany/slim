# M2 prerequisite decisions — 2026-09-08

Status: specification and verification preparation; M2 language implementation
has not started. M3 remains pending behind M2. Base: `bead09c`, branch
`codex/slim-next`; the pre-commit receipt identifies the tested working-tree
files individually, before subsequent reporting additions.

| Work | Result / next boundary |
| --- | --- |
| [RFC-0147](../../design/rfcs/0147-scoped-m2-feature-experiments.md) | Explicitly approved on 2026-09-08; scoped policy edits and governance checks implemented with six focused regression tests. Language child decisions remain separate. |
| [RFC-0148](../../design/rfcs/0148-staged-successor-compatibility.md) | Proposed prerelease sequence, source/schema/ABI cutovers, production bootstrap bridge and behavior-preserving migration obligations. No version or runtime change. |
| [RFC-0149](../../design/rfcs/0149-uniform-moves-and-lexical-references.md) | Proposed transfer/reference grammar, lexical loan and initialization rules, mandatory checker limits and a 576-cell ownership test specification. Bounds are uncalibrated; no test pass is inferred. |
| [RFC-0150](../../design/rfcs/0150-checked-slices-and-owned-bytes.md) | Proposed checked view operations, owned Bytes, invalidation and freeze/replacement contracts, with named bounds matrices. |
| [RFC-0151](../../design/rfcs/0151-memory-domain-and-cleanup-abi.md) | Proposed domain ABI, failure-atomic growth and bounded-stack cleanup. Provider/API/worker/host children are drafted below; their coupled acceptance and evidence remain required. |
| [RFC-0152](../../design/rfcs/0152-bounded-hosted-pool-provider.md) | Proposed fixed buddy pool: frozen capacity/header/index limits, bounded split/coalesce/index work and an independent small-pool oracle. The RFC-0154 native block component is verified separately; source/provider and RFC-0156 task adoption remain pending. |
| [RFC-0153](../../design/rfcs/0153-explicit-allocation-authority-and-reservation.md) | Proposed opaque allocator capability and explicit reserve/nonallocating insertion. Ordinary concrete source wrappers return the unconsumed value; no magic generic Result is required. Host-adapter contracts and wrapper inventory remain pending. |
| [RFC-0154](../../design/rfcs/0154-native-pool-component.md) | Accepted native block component, tested against the independent oracle in ordinary/sanitized builds. [Costs and scope](2026-09-08-m2-native-pool.md) remain separate from provider/source adoption. |
| [RFC-0155](../../design/rfcs/0155-bounded-host-adapter-storage.md) | Proposed charged argv descriptors, borrowed host inputs and bounded nonallocating I/O into reserved buffers. [Boundary matrix](2026-09-08-m2-host-contract.md) frozen; native adapters and source migration remain pending. |
| [RFC-0156](../../design/rfcs/0156-reserved-buffer-structured-tasks.md) | Proposed parent reservation and whole-buffer move/return tasks, replacing unimplemented child-pool machinery. Native pool/task witness passes 192 bounded cases; source eligibility, trap and application evidence remain pending. |
| Existing release ABI gate | Removed literal ABI 1→2 assumption. Compile a matching-header control; validate and mutate exactly one current ABI definition; require the actual mismatch diagnostic after rejection. |

The gate repair passed exact assertions within ten header cases: ABI 1, 2 and
2,147,483,646; missing/duplicate definitions; zero, negative, oversized,
nonnumeric and extra-token definitions. The extracted production release-test
block also passed with actual `hello.slim` generated C against matching and
mismatched runtime headers. This is eleven named checks, not a full release run.
[The receipt](archive/2026-09-08-m2-abi-check.json.gz) records the script hash,
case outcomes, actual mismatch diagnostic and reproduction inputs.

Governance and all 18 website tests passed for RFC-0147–0151 and their links;
shell syntax and diff checks passed for the gate repair. All eight AGENTS.md
pre-commit commands then passed sequentially: bootstrap, governance, Cargo tests,
performance, reduction, parallelism, compare and agent. Every captured source
hash remained unchanged throughout. The [pre-commit receipt](archive/2026-09-08-m2-prerequisite-gates.json.gz)
preserves exact commands, logs, source identities and exits; these checks do not
constitute the complete `scripts/verify-0.9.sh` milestone invocation.

Clean preparation checkpoint `450156c944024289a539c7a63dbcc7078e758531` then
passed `scripts/verify-release.sh`: byte-identical packages, source hashes,
fresh bootstrap, installed frontend/native sessions and examples, and the repaired
ABI rejection test. Both archives have SHA-256
`55375f64adbe439e37c457786ac6c669a0206e8e01d7ab13bfc0e5fd2222d72b`.
The [release receipt](archive/2026-09-08-m2-prerequisite-release.json.gz) identifies
that clean invocation separately from these later evidence-only additions.
That preparation checkpoint changed no compiler/runtime behavior, performance
budget or verification obligation. This evidence does not replace M1's identified closure run or complete M2.

Next: verify the proposed host and task contracts, review/test the coupled
ownership/slice/API package, and freeze application oracles under [slice 00](2026-09-08-m2-plan.md). No prototype or favorable rating
substitutes for those prerequisites.

## Approved experiment policy

The maintainer explicitly approved RFC-0147 on 2026-09-08. AGENTS.md and
FEATURE_POLICY.md now contain its exact scoped edits. Governance checks both
evaluation fields, policy acceptance and child eligibility; six regression
tests preserve ordinary ratings, safety, arithmetic/ranges, dispositions,
primitive uniqueness and surface activation. RFC-0109 and all performance
budgets remain unchanged. RFC-0148–0151 remain proposed.

All eight required pre-commit commands passed on the captured policy source;
no captured file changed during the run. The earlier sandbox attempt passed
72 integration tests and failed three loopback socket binds with permission
errors; the rerun passed all 75 with socket access. Both attempts, source hashes,
exact commands and logs are preserved in the [policy receipt](archive/2026-09-08-m2-policy-gates.json.gz).
This is pre-commit verification, not a full milestone/release invocation.
The status corrections and this evidence paragraph are later reporting additions.

## Provider-model prerequisite

RFC-0112's first implementation sequence requires the ownership/lifetime/allocator
contract to be specified and tested before source-syntax changes, including Unit.
The provider model now compares 1,015 transitions across 104 reachable states
through depth four with an independent occupied-cell oracle. Each state uses an
eight-quantum pool and fixed request/failure/release actions. It also checks 281
arithmetic cases and 268 free-index operations at bitmap hierarchy boundaries.
Maximum observed per-index-operation work is four hierarchy-word visits.

These are bounded model results, not native allocator, lifetime-checker or M2
completion evidence. The native index metadata bound is 4,261,024 bytes at a
1 GiB pool, separate from pool payload; the candidate default is 256 MiB, frozen
before native measurements. Three Cargo tests retain oracle agreement, exact
model-budget boundaries, invalid-limit rejection and rejection with disabled
assertions. Worker adoption, hosted adapters, native sanitizer/accounting results
and source wrapper failure oracles remain required.

All eight required pre-commit commands passed with unchanged captured source,
including all 75 production integration tests and the three new model tests.
The [provider-model receipt](archive/2026-09-08-m2-pool-prerequisites.json.gz)
retains the exact commands, source identities, model result and logs. This
checks preparation tooling; it is not native-provider adoption or the full
milestone release gate. The receipt link and this paragraph are reporting additions.
