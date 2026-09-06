# SLIM Next implementation and evaluation

Status: in progress
Decision: RFC-0112 accepted by the maintainer on 2026-09-05
Baseline source: 97412bf

The active implementation goal covers the full M0-M7 roadmap and measurement
of compiler latency and agent effectiveness. Approval does not complete any
milestone. Production semantics remain in SLIM and the portable C seed.

## Milestone status

| Milestone | Status | Current evidence |
| --- | --- | --- |
| M0: repair and establish truth | in progress | Cache framing, name lookup, named/conditional transfers, branch joins, finite layouts, call-argument loans, and termination-effect enforcement validated. Field transfers, longer-lived aliases, and actual-work instrumentation remain. |
| M1: compiler substrate | pending | No actual incremental-reuse claim yet. |
| M2: expressive safe core | pending | Successor ownership, borrowing, allocation, and generics not implemented. |
| M3: agent and debugger interface | pending | Semantic service and source debugger not implemented. |
| M4: component laboratory | pending | Deterministic providers and replay not implemented. |
| M5: systems contracts | pending | Resource capabilities and successor resource contracts not implemented. |
| M6: kernel substrate | pending | No freestanding kernel claim. |
| M7: OS research slice | pending | No end-to-end OS or productivity claim. |

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

## First repair batch

RFC-0113 implements complete cache-frame validation before key comparison,
canonical first-declaration lookup, invalidation on named owning transfers,
and branch-scoped move components. Transfers into locals, aggregates,
collections, and assignments now invalidate the source owner. Exclusive arms
can each transfer the owner; a use after their join is rejected. Union by rank
and path compression avoid copying the full binding table at every branch.

This is a partial safety repair. Projection moves, conditional-result
transfers, definite reinitialization, and other M0 findings still require work.
The progress report does not claim the complete safe-core contract is repaired.

The incremental documentation now describes the actual implementation:
`session` computes invalidation estimates, with no retained parsing, checked
bodies, or generated fragments. Its historical operation labels are not
instrumentation. The whole-project artifact cache is a separate mechanism.

## Same-host compiler comparison

The durable `slim-bench frontend-pair BASELINE CANDIDATE` command measured the
baseline native binary retained from commit 97412bf against the combined
RFC-0113 compiler. One warmup per compiler/size preceded seven alternating
AB/BA pairs. Both binaries were unchanged during measurement. The host was
macOS 26.6.1 on arm64; measurements include native process startup and checking,
and exclude external C compilation. Binary identities and every sample are in
[the raw TSV](2026-09-05-slim-next-frontend-pair.tsv).

| Fixed-width common-prefix declarations | Baseline median | Candidate median | Baseline / candidate |
| --- | ---: | ---: | ---: |
| 2,000 | 77.033 ms | 6.287 ms | 12.25x |
| 4,000 | 287.026 ms | 9.265 ms | 30.98x |
| 8,000 | 1,145.870 ms | 15.948 ms | 71.85x |
| 16,000 | 4,756.574 ms | 29.883 ms | 159.17x |

This fixture deliberately exposes the old quadratic declaration-name scan.
The speedup is not a representative application average, an incremental-build
measurement, a comparison with other languages, or an LLM productivity result.
An earlier shorter-name fixture gave approximately 28x at 16,000 declarations;
it is a different workload and must not be conflated with this table.

The permanent quick gates include both common-prefix declarations (2,000 to
16,000; measured exponent 0.755) and branch transfers with many live owners
(125 to 1,000; exponent 0.707). Both retain a 1.25 ceiling. Sublinear measured
exponents reflect fixed process overhead; they do not imply a sublinear
checking algorithm. No existing budget was relaxed.

## Verification at the first checkpoint

- Portable bootstrap fixed point: 2,894,710 generated C bytes; seed SHA-256
  `bab8c46be9d41cea200dbc3d686d3783fcb5040807d93c0b8489e38cf5e7fb39`.
- Governance and canonical selfhost formatting passed.
- 190 production conformance fixtures and 2,000 malformed-input mutations
  passed. Cache corruption now crosses every incomplete key prefix.
- The existing 7 Rust unit tests and 48 integration tests passed, with three
  loopback integration cases rerun outside the socket-restricted sandbox.
- An additional independent bounded path oracle passed all 486 generated
  ownership programs: no-op/read/move actions, two nested branch orientations,
  prefix and suffix actions, and local/call transfers. This is exact for that
  enumerated test domain, not a proof for all ownership programs.
- Quick performance, quick reduction, parallelism, quick native comparison,
  and agent-tool gates passed. Native comparison over 20 challenges reported
  SLIM/C geometric mean 1.112 and SLIM/Rust 0.964 on this run; those remain
  benchmark-specific runtime ratios, independent of agent effectiveness.

Logs for this checkpoint are in ignored `build/slim-next-baseline/`. The
committed TSV preserves raw compiler comparison samples. The portable baseline
binary and seed are also retained locally in that directory.

## Agent effectiveness observations

No controlled agent success-rate or development-speed claim is established.
The existing agent command checks fixed broken/repaired sources and records
feedback latency and source/edit proxies; it does not run an LLM experiment.

During this batch an initial branch-state implementation incorrectly placed a
join after `recur`. The focused negative join fixture caught it. Moving the
join to the terminal arm-list case fixed it; the 486-case oracle followed.
This is one observed failed implementation attempt and repair, not an inferred
error rate. It motivates unreachable-code diagnostics and explicit control-flow
queries in the successor. Qualitative motivation must not be counted as a
measured improvement before those tools exist.

Controlled language comparisons remain pending M2-M4 foundations and the M7
held-out evaluation. The current task is the implementation trace, with its
own failures retained as evidence, not the control group for itself.

## Finite-layout follow-up

The checker now rejects direct, mutual, enum-payload, and mixed inline storage
cycles with E0354 before C generation. A visited declaration table makes this
structural check linear in the parsed graph; it does not enumerate dependency
paths. Collection and typed-ID indirections break inline recursion. A recursive
`Vec[Tree]` layout is a positive native execution fixture. Invalid typed input
now skips memory-plan construction, so later analysis is not asked to interpret
a type graph the checker has already rejected.

This follow-up restores the finite-storage requirement; it introduces no type
constructor or recursive-type feature. The production C fixed point is now
2,909,082 bytes with SHA-256
`31e7448f0de1f95752873a75e997ec30e1f775a628124b5c03c8f6aab5a499f5`.
All 195 conformance fixtures and 2,000 malformed-input mutations passed, as did
governance, quick performance, quick reduction, parallelism, quick native
comparison, and agent-tool gates. The complete Rust suite passed all 7 unit
tests and 49 integration tests, including the 486-program ownership oracle.
The first-checkpoint paired TSV above still
identifies its original compiler binaries; it is not silently relabelled as a
measurement of this later candidate.

## Next M0 work

- Close projected-field transfer gaps and migrate valid extraction patterns
  toward the approved explicit-replacement design. General partial moves are
  initially rejected by the successor; do not build a different ownership
  model merely to preserve every legacy spelling.
- Declared termination-effect enforcement is now repaired in RFC-0115; retain
  its exact/unknown boundary through the successor's effect migration.
- Finish the audit of claims against production behavior, including actual
  incremental work counters and compiler identity in cached artifacts.
- Carry every repaired case and performance workload into M1-M7. M0 remains
  in progress, and no successor-language or OS milestone is complete yet.

## Conditional-result and borrow-origin follow-up

Owning local bindings and assignments now invalidate all named owners that
can supply a conditional result. Result tracing follows branch alternatives
and let/set continuations, while preserving the separate behavior of statement
prefixes. Nested alternatives and a local alias returned from one arm execute
correctly in the positive native fixture. Borrow origins also survive local
statements and structured `parallel` bodies: both previously accepted borrowed
return reproducers now report E0347. Newly owned parallel results and ordinary
borrowed reads remain valid.

The ownership-return check now uses the same origin query as other transfers,
removing a separate incomplete traversal. Six negative and two positive
production fixtures retain these cases. The additional conditional-result
scaling variant independently obeys RFC-0113's existing 1.25 branch-move
exponent budget; the original explicit-transfer fixture remains unchanged.

The current bootstrap fixed point is 2,911,719 C bytes, SHA-256
`a3d440e0c5afdf25dace60d0333fd1207493995c2f806187a903178fe2cda873`.
All 203 conformance fixtures and 2,000 malformed-input mutations pass. Logs for
this follow-up are in ignored `build/slim-next-ownership/`. This improves the
checker on reproduced cases; it does not establish complete ownership safety
or an LLM productivity gain. Verification passed bootstrap, governance, all 7 Rust unit tests and 49
integration tests, quick performance/reduction/native comparison, parallelism,
and agent-tool gates. The new conditional-result scaling exponent was 0.800
against the unchanged 1.25 branch ceiling. ASan/UBSan reported no errors while
the compiler checked its own source or while the positive conditional-result
program ran. Injection at each of that program's nine allocation ordinals
produced the expected exit 71, with normal success at ordinal 10; the checked
experiment budget was 64 ordinals.

## Newly reproduced call-argument loan violation

Status at discovery: unresolved. RFC-0114 repairs this case in the checkpoint
below. The durable [reproducer](../reproducers/nested_call_loan.slim) calls
`observe(value, grow(^value))`. The pre-repair production compiler accepted it. Growing the
owned argument reallocates its vector while the earlier argument retains the
old buffer. A native ASan/UBSan build reports **heap-use-after-free**, an
8-byte read from the freed buffer, and exits 134 on the recorded host.

The raw native C, binary, stdout, and sanitizer stderr are retained locally in
`build/slim-next-ownership/nested-call-loan.*`. The compiler's current
same-call overlap check does not preserve an earlier argument's loan across
nested argument evaluation. Fix call-argument loan lifetimes and retain this
source as a permanent rejection test before claiming the ownership repair is
complete. Also test nested exclusive mutation without an ownership transfer,
shared/shared nesting, and mutation that occurs before a later borrow.

This extra probe changed the next implementation priority despite the existing
suite being green. Passing fixed fixtures and a bounded ownership oracle does
not establish general memory safety. Agent productivity remains unmeasured.

## Call-argument loan checkpoint (2026-09-06)

RFC-0114 now retains an earlier argument's loan throughout nested argument
evaluation. The original UAF reproducer is a permanent production rejection
fixture: emission returns E0349 at bytes 463:468 and emits diagnostics only.
Seven positive loan fixtures run successfully under ASan/UBSan. The five
original positive cases emit C byte-identical to the retained pre-loan compiler
in `build/slim-next-loans/baseline`. This evidence is exact for those fixtures;
general ownership safety and agent productivity remain unknown.

Verification exposed six additional source migrations: the LZ4 overlapping
copy reads its scalar byte before `vec.push` reserves the vector; five native
challenges compute scalar arguments before exclusive call/recurrence arguments.
The checksum loops compute the next index before the next total, retaining the
original order of checked arithmetic and reads. Binary search computes its
target before its length query. These moves introduce no storage copies,
allocation, or algorithm change. The LZ4 overlap and round-trip tests pass.

The exact native analysis and resource baselines change only as follows:

| Challenge | Source bytes | Expression nodes | Reported total checked sites |
| --- | --- | --- | --- |
| merge_sort | 2824 → 2898 | 247 → 251 | unchanged |
| bytefreq | 1235 → 1309 | 107 → 111 | 3 → 1 |
| binary_search | 1164 → 1222 | 105 → 109 | unchanged |
| arena_sum | 817 → 891 | 68 → 72 | unchanged |
| image_convolution | 2473 → 2547 | 241 → 245 | 30 → 29 |

Each migrated challenge gains two explicit scalar bindings. In bytefreq the
two checksum `index + 1` sites lose their inferred bounds; in image_convolution
the checksum increment loses its inferred bound. The existing recurrence
analysis does not recover those bounds through the newly bound controller.
The retained pre-loan compiler and current compiler produce identical complete
analysis reports for both migrated sources, isolating this conservative
precision loss to source shape. No checker precision feature is added here.
All complete blocker sets, primary reasons, checked-site counts, recurrence
resource profiles, allocation counts, and candidate/selected/executable/executed
site counts remain unchanged. Other baseline rows remain byte-identical.

The permanent nested-loan series checks 125, 250, 500, and 1,000 live owners,
each with nested shared calls followed by mutation after the outer call ends.
Its measured exponent is 0.742 against the existing owned-transfer ceiling of
1.25. Common-prefix declarations, branch moves, and conditional-result moves
measure 0.741, 0.702, and 0.769 against their unchanged 1.25 ceilings. The
owned-transfer normalized ratio is 0.985 against its unchanged 1.30 budget.
These are same-host fixture measurements, not universal compiler speed claims.

The portable fixed point is 2,929,135 C bytes with SHA-256
`5626ff8fc6ffe1d0595cd4ff2e66ccf26881a30a70ce6836b00770d617413224`.
Production conformance passes 220 fixtures and 2,000 deterministic malformed
inputs. The complete Rust suite passes 7 unit tests and 49 integration tests,
including the 486-program ownership oracle and local loopback cases. Logs and
sanitized fixture binaries are retained in ignored `build/slim-next-commit/`.
Bootstrap, governance, quick performance, quick reduction, parallelism,
resources, quick native comparison, agent-tool checks, and quick parallel
runtime gates pass. Across 20 native challenges, the SLIM/C geometric mean is
1.042 and SLIM/Rust is 1.000. Generated parallel/forced-serial ratios are 0.797
for `state_machine` and 0.726 for `signal_network`, within existing budgets.
M0 remains in progress: projected-field transfers, longer-lived aliases,
termination checking, and actual incremental work are still outstanding.

## Termination-effect checkpoint (2026-09-06)

RFC-0115 restores the current 0.9 capability ceiling. An unproven recurrence
now reports E0343 unless its function declares `partial`. A positive totality
fact must name that exact recurrence node. A linear iterative DFS rejects
cycles among functions lacking `partial`, including unused functions and
cycles beyond the parallel analyzer's 64-function reporting boundary. The
existing effect checker continues to reject callers missing a callee's
declared capability. This is current-contract repair; the successor's separate
operation effects and progress contracts remain pending.

During integration, the negative prefix test exposed two faulty totality
walkers. Their `let rest = recur(...); current && rest` shape transferred
control before combining the current fact, so a trapping prefix or recurrent
argument could be ignored. Both walkers now reject an unproven current element
before continuing. The original trapping-prefix source was accepted and
reported with an exact recurrence profile; the repaired compiler rejects its
missing `partial`, and its declared-partial variant reports no such profile.
This is a reproduced repair, not a general proof of analyzer correctness.

Three library declarations gain `partial`: `std_i64_vec.filled`,
`std_u8_vec.filled`, and `lz4_codec.append_extension`. The two call-cycle
functions in the parallel-analysis fixture gain the same capability. Generated
library interfaces record the change. Algorithms, argument evaluation, and
storage behavior are unchanged. Existing library/native execution tests pass.
The declared-recursion conformance fixture retains a nonterminating branch
without provoking Clang's unconditional-self-recursion warning; a separate
fixture executes terminating direct and mutual recursion under `partial`.

The 12 new production fixtures cover unconditional/direct/mutual recursion,
unknown or zero steps, wrong controllers, out-of-domain steps, trapping prefixes
and arguments, declared recursion, and a proven countdown. An independent
transitive-closure oracle exhausts all 512 directed graphs on three functions;
the checker agrees on acyclicity throughout that complete domain. Additional
acyclic/cyclic chains of 65 and 2,048 functions cross reporting and native-stack
boundaries. None of this evidence establishes termination for arbitrary code.

The fixed range budget stays at 64 refinements. Thirty-two countdown functions
pass; adding the 33rd reports E0343 at bytes 2978:2983, and 65 functions retain
the same diagnostic. The first proposed scaling fixture exceeded this budget
and correctly failed. The permanent timing fixture instead grows the context
around one proven recurrence; the exhaustive-budget rejection remains a
separate permanent test. Missing evidence never becomes an acceptance fact.

The new check-exponent measurements are 0.318 for acyclic chains, 0.324 for
shared-dependency DAGs, and 0.380 for the pure-recurrence context. Each obeys
the unchanged 1.25 ceiling. Existing ownership/declaration fixtures and the
1.30 owned-transfer normalized-cost gate also pass (measured ratio 1.026).
Small-fixture exponents include fixed process startup, not sublinear algorithm
claims. No performance budget or native analysis/resource baseline changed.

The permanent paired frontend command compares the retained compiler from
checkpoint `4a858de` with this candidate, with one warmup and seven alternating
AB/BA pairs per size. Raw samples and binary identities are in
[the dated TSV](2026-09-06-termination-frontend-pair.tsv).

| Common-prefix declarations | Baseline median | Candidate median | Candidate / baseline |
| --- | ---: | ---: | ---: |
| 2,000 | 6.130 ms | 5.982 ms | 0.976 |
| 4,000 | 8.936 ms | 9.020 ms | 1.009 |
| 8,000 | 15.457 ms | 15.731 ms | 1.018 |
| 16,000 | 29.282 ms | 30.002 ms | 1.025 |

These same-host native process latencies measure the named declaration fixture,
not general compiler latency, incremental work, or agent productivity.

The portable fixed point is 2,950,748 C bytes, SHA-256
`f2583b347c3770128da17dcdc739d0c9b8f3942451e81fb2f357a46b0ef396be`.
Bootstrap, governance, formatting, Clippy, all 7 unit and 52 integration tests,
232 conformance fixtures plus 2,000 malformed-input mutations, quick performance,
quick reduction, parallelism, resources, quick native comparison, agent checks,
and quick parallel runtime pass. Native SLIM/C and SLIM/Rust geometric means
across 20 challenges are 1.092 and 1.002; generated parallel/serial ratios are
0.891 for `state_machine` and 0.654 for `signal_network`, within existing gates.

ASan/UBSan pass while checking the compiler itself and the positive termination
fixtures, and while running both positive native executables. Allocation-fault
injection into the sanitized proven-countdown check covers exactly ordinals
1 through 128: every injected failure returns exit 71, no source output, and
the expected failure diagnostic. All 128 ordinals are reached, so this is a
bounded prefix of the allocation sequence, not exhaustive allocation coverage.
Logs, binaries, and the injection record are in ignored
`build/slim-next-termination/`.

M0 remains in progress. Projected ownership, longer-lived alias tracking,
actual checker/generator/cache work counters, and the needed successor policy
decisions remain before milestone closure. M1-M7 and controlled agent outcomes
are still pending.
