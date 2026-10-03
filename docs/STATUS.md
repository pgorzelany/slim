# SLIM status

Status: SLIM 0.9 — experimental, pre-1.0

The current release is `0.9.0`. SLIM has no external compatibility obligations
yet. Its familiar syntax is the only accepted source form; the transition
parser used to migrate the repository has been removed.

Compiler version: 0.9.0

The production compiler is self-hosted SLIM with the portable C seed. Runtime
ABI 1 is unchanged. Project manifests use schema 1, while the ownership-mode
source surface, interfaces, and caches use schema 3.

Core 1L is retained as an internal stabilization milestone, not an active
public 1.0 freeze. The conditions for a future compatibility freeze are in
`docs/COMPATIBILITY.md`.

SLIM Next M0 (current-contract repairs and truthful evidence) is complete. Its
[repair and verification record](../benchmarks/results/2026-09-05-slim-next-progress.md)
identifies the compiler, regressions, measured costs, and passing full release
gate. M1 retained checking and native sessions pass all release components.
M2 source cutovers remain pending; the
[M2 dependency plan](../benchmarks/results/2026-09-08-m2-plan.md) records separately
staged memory and host contracts and native component evidence.

The [early agent development loop](../design/rfcs/0158-early-agent-development-loop.md)
completed its bounded context/pilot scope; strict acceptance was 2/3 in each
condition and general context benefit remains unknown. The
[closure report](../benchmarks/results/2026-10-02-agent-development-loop.md)
retains original outcomes and costs.

The [current roadmap](../ROADMAP.md) authorizes continuous work until 2026-10-03
10:00 Europe/Warsaw. Checkpoint `112f6ed` is pushed to `codex/slim-next`.
Fresh source8b passes repository, release and website verification in44m05s,
with Rust verification tools rebuilt in the captured checkout. Historical copied-cache
source binding remains unknown. The [ordinary development tools](../library/COMPONENTS.md),
fixed million-node/aggregate-source crossings, context export and ledger overflow
preflight pass that invocation. RFC-0172 explicit source selection passes218 focused
ordinary/sanitized observations on three real projects and its fresh current-source
gate also passes; combined source9 closure is pending.
The frozen evaluator has eight accepted participants, one infrastructure interruption and fifteen undispatched trials.
Fresh-agent capacity, model calls/tokens and general effectiveness remain unresolved.
Current [source-bound records](../benchmarks/results/README.md) retain scope and
identities. M2/full M3 remain pending.

The [roadmap](../ROADMAP.md) retains M3-M7 exit gates and the unchanged 0.9
language version.
