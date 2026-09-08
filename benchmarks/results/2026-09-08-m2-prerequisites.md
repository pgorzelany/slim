# M2 prerequisite decisions — 2026-09-08

Status: specification and verification preparation; M2 language implementation
has not started. M3 remains pending behind M2. Base: `bead09c`, branch
`codex/slim-next`; the pre-commit receipt identifies the tested working-tree
files individually, before subsequent reporting additions.

| Work | Result / next boundary |
| --- | --- |
| [RFC-0147](../../design/rfcs/0147-scoped-m2-feature-experiments.md) | Proposed exact AGENTS/FEATURE_POLICY changes and governance contract for scoped M2 experiments. Explicit policy approval requested; current rules remain unchanged. |
| [RFC-0148](../../design/rfcs/0148-staged-successor-compatibility.md) | Proposed prerelease sequence, source/schema/ABI cutovers, production bootstrap bridge and behavior-preserving migration obligations. No version or runtime change. |
| [RFC-0149](../../design/rfcs/0149-uniform-moves-and-lexical-references.md) | Proposed transfer/reference grammar, lexical loan and initialization rules, mandatory checker limits and a 576-cell ownership test specification. Bounds are uncalibrated; no test pass is inferred. |
| [RFC-0150](../../design/rfcs/0150-checked-slices-and-owned-bytes.md) | Proposed checked view operations, owned Bytes, invalidation and freeze/replacement contracts, with named bounds matrices. |
| [RFC-0151](../../design/rfcs/0151-memory-domain-and-cleanup-abi.md) | Proposed domain ABI, failure-atomic growth and bounded-stack cleanup. Concrete provider and source allocation-API decisions remain required before acceptance. |
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
constitute the complete `scripts/verify-0.9.sh` milestone invocation. Clean
package/install verification remains pending until this preparation is committed.
No compiler/runtime behavior, performance budget or verification obligation
changed. This evidence does not replace M1's identified closure run.

Next: resolve the explicit policy decision, review the coupled ownership/slice
drafts, specify the concrete provider and allocation API, and freeze application
oracles under [slice 00](2026-09-08-m2-plan.md). No prototype or favorable rating
substitutes for those prerequisites.
