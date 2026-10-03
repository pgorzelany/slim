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
10:00 Europe/Warsaw. Published code [`710bc29`](https://github.com/pgorzelany/slim/commit/710bc29c4dade7e2194ac11ea678c43f0d1ac461) passed source9 repository/release/website gates
over 1,944 files with Rust tools built there.
[Integration identities](../benchmarks/results/development-operation-cost-current.json)
retain hashes/times. Later metadata notes are separate.
Historical Rust-root binding and model benefit remain unknown.
The frozen evaluator retains all 24 rows: 13 accepted participants, six accepted pairs,
one pre-participant infrastructure interruption and ten undispatched trials.
Elapsed time includes coordinator/delivery/queue delays; model calls/tokens, active time
and general effectiveness remain unknown.
Current [source-bound records](../benchmarks/results/README.md) retain scope and
identities. M2/full M3 remain pending.

Ledger transactions reject overflow before mutation. The observed checked-edit
workflow captured stopped projects, computed declared-import impact, selected
three explicit sources, then checked the entire candidate. Context sufficiency
and model benefit remain unknown.

The [roadmap](../ROADMAP.md) retains M3-M7 exit gates and the unchanged 0.9
language version.
