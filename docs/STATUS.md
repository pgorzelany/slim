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
is complete for its bounded scope. Production self-hosted schema-1
[semantic context](CONTEXT.md), independent conformance, native costs, all six
frozen Sol 6.1 xhigh trials and all release components are recorded in the
[closure report](../benchmarks/results/2026-10-02-agent-development-loop.md).
Strict task acceptance is 2/3 in each condition; context benefit remains unknown.
The decision retains opt-in context and freezes breadth. The next priority is
the existing manifest sorting/uniqueness contract, then reliable transport and
held-out compiler/library repair tasks. This accepted branch does not complete
M2 or the full M3 agent/debugger interface.

M3-M7 and general agent effectiveness remain pending. A bounded pilot cannot
establish a general effectiveness claim. The
[current roadmap](../ROADMAP.md) records deliverables, dependencies and exit
evidence. The 0.9 language version is unchanged.
