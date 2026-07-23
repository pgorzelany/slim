# Core 1F acceptance

Date: 2026-07-23
Decisions: D0062 through D0069
Status: complete evidence boundary; production execution deliberately disabled

## Outcome

Core 1F proves a bounded deterministic parallel plan without adding language
surface or changing generated program behavior. The SLIM-written compiler now:

- retains complete direct and graph-boundary blocker sets;
- shares bounded integer and exact-node totality facts;
- proves one canonical finite tail recurrence;
- recognizes independent adjacent total computations;
- selects one lexical, pairwise non-overlapping plan; and
- reports the execution boundary and missing profitability evidence explicitly.

The permanent `state_machine` application contributes the first substantial
positive site: two distinct owned states each run a two-million-step total
recurrence and join before comparison. Application baseline schema 4 retains
one candidate, one selected site, and one reported site.

## Failure and execution decision

Eligible tasks cannot allocate, perform I/O, trap, mutate, borrow exclusively,
diverge under the accepted proof, or cross an unknown callee. Consequently,
task failure and cancellation do not exist inside the accepted subset.
Worker-creation failure would use serial fallback, and structured ownership of
the sole join handle excludes a wait cycle.

The host C11 compiler lacks `<threads.h>`. A manual POSIX reference is therefore
evidence, not a production backend. With repeated pairs to amortize process
startup, the measured results were:

| iterations per task | repetitions | serial us | manual parallel us | ratio |
|---:|---:|---:|---:|---:|
| 0 | 200 | 1,280 | 5,523 | 4.314 |
| 1,000 | 200 | 1,451 | 5,949 | 4.099 |
| 10,000 | 100 | 2,036 | 4,533 | 2.227 |
| 100,000 | 20 | 2,645 | 2,826 | 1.068 |
| 1,000,000 | 4 | 4,011 | 3,070 | 0.765 |
| 2,000,000 | 2 | 3,999 | 3,044 | 0.761 |

The result demonstrates a profitable application but rejects a host-derived
fixed threshold as portable compiler policy. Production reports
`execution disabled` with reason `no-portable-runtime-or-cost-model`.

## Permanent gates

`slim-bench parallelism` checks all thirteen application rows and complete
schedule counts. `slim-bench parallel-runtime` checks reference output at every
work size and permanently gates the largest parallel/serial ratio at 2.00.
Generated 65-selection evidence must remain bounded at exactly 64 printed
sites. The normal full gate continues to cover bootstrap fixed point,
conformance, 2,000 deterministic malformed inputs, sanitizers, allocation
failure, compiler scaling, reduction, incremental projects, native challenge
performance, and agent fixtures.

The portable compiler reaches a byte-identical fixed point at 1,887,556
generated C bytes with SHA-256
`3071014e068157c9bdf56869d28a2ee0cfe499321da8483f8d73bf4289656898`.

## Final release gate

`./scripts/verify.sh` passed in full:

- 23 end-to-end tests;
- 109 conformance fixtures and 2,000 deterministic malformed-input mutations;
- governance, Clippy, formatting, bootstrap fixed point, sanitizers, and
  allocation-failure injection;
- compiler/reduction/incremental/project scaling gates;
- schema-4 analysis over all thirteen applications;
- the permanent parallel-runtime probe, with a 0.798 quick-gate ratio at two
  million iterations;
- all native SLIM/C/Rust challenge ratios; and
- all agent diagnostic fixtures.

The final line was `verification: all gates passed`.
