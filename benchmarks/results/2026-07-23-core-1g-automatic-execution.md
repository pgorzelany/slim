# Core 1G guarded automatic execution

Date: 2026-07-23
Decision: D0071
Status: accepted

## Accepted boundary

Analysis schema 5 distinguishes candidate, selected, reported, executable, and
executed sites. Execution requires a selected leading-chain pair of direct
user calls, atomic typed captures, complete reorder-safety and totality, and
two exact recurrence workloads of at least 1,000,000 iterations.

The production SLIM code generator emits two typed stack contexts and wrappers
for each executable site. The first wrapper may run on one POSIX worker while
the parent runs the second in worker scope. A failed or disabled spawn calls the
same first wrapper inline, a successful spawn joins exactly once, and both
results are installed before the original continuation. Worker scope on both
branches blocks transitive nested creation. Unselected programs emit no worker
marker or machinery.

## Application evidence

The permanent application corpus advances to schema 5 and fourteen programs:

| application | task work | candidate/selected/reported | executable/executed |
|---|---:|---:|---:|
| `state_machine` | 2,000,000 + 2,000,000 | 1/1/1 | 1/1 |
| `signal_network` | 2,000,000 + 2,000,000 | 1/1/1 | 1/1 |
| other twelve applications | unavailable or below threshold | 0/0/0 | 0/0 |

`signal_network` is a distinct eight-Boolean transition network with twice the
state width and more Boolean dependencies than `state_machine`. Its SLIM, C11,
and Rust versions use the same algorithm and data scale and must agree on
output in the normal comparison suite.

## Runtime measurements

The quick `parallel-runtime` run on the acceptance host reported:

| generated application | forced serial median | parallel median | ratio |
|---|---:|---:|---:|
| `state_machine` | 4,059 us | 2,938 us | 0.724 |
| `signal_network` | 5,592 us | 3,884 us | 0.695 |

The same run retained the manual state-machine reference. Its
two-million-iteration parallel/serial ratio was 0.766, and the measured
crossover remained near 100,000 iterations per task. The production threshold
is ten times that crossover. Same-host generated ratios are now checked on
every full verification run with permanent budgets.

## Negative and failure evidence

- `SLIM_TASK_FAIL_AT=1` and `SLIM_TASK_DISABLE=1` preserve output through the
  inline path.
- `SLIM_TASK_JOIN_FAIL_AT=1` reaches the defined runtime trap with exit 70.
- Worker and fallback scope decline nested creation.
- A profitable site nested below a `match` remains reported but not executable,
  matching the current code generator's supported leading chain.
- `examples/hello.slim` contains no parallel macro, task type, wrapper, or link
  requirement.

No source syntax, primitive, scheduler, dependency, lock, future, channel,
annotation, separate IR, hidden allocation, or unbounded task tree was added.
The portable self-hosted compiler reproduces a byte-identical 1,973,458-byte C
seed.
