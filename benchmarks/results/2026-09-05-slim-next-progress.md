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
| M1: compiler substrate | in progress | RFC-0124 defines revision identities, derived control flow, dependency completeness, and transactional reuse. Implementation and measured incremental reuse remain pending. |
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

## Local borrow loan checkpoint (2026-09-06)

RFC-0116 extends existing indexed loan records to the lexical lifetime of a
local initialized from borrowed affine storage. It also checks affine assignment
as a restricted access to its target. Shared reads, independent known owners,
scalar copies, and mutation or transfer after scope exit remain accepted.
Unknown origins remain conservative; shadowing an alias does not end its scope.

The retained production compiler from `d6eeede` accepts
[the local-borrow reproducer](../reproducers/local_borrow_loan.slim). Growing its
exclusive vector parameter reallocates storage while the local alias still
refers to the old buffer. ASan reports an eight-byte heap-use-after-free. The
new production checker rejects the exact witness at `E0349@282:287`, before C
generation. Seven additional formerly accepted rejection fixtures cover builtin
growth, nested aliases, shadowing, affine replacement, moving an outer collection
while an element is borrowed, and unknown-origin mutation and transfer.

Eight positive native fixtures cover shared nesting, independent owners,
scalar copies, sibling scopes, scope exit, unknown shared origins, initializer
mutation before a local loan starts, and collection transfer after scope exit.
All eight are formatter-stable, emit C byte-identical to the retained compiler,
and execute with exit zero under ASan/UBSan. This is exact fixture evidence,
not a complete ownership-safety result. Entire native challenge analysis reports
are also byte-identical to `d6eeede`; no blocker/resource baseline is weakened.

The sanitized compiler checks its own source successfully. Allocation-fault
injection into `local_loan_initializer.slim` covers ordinals 1 through 128:
ordinals 1 through 62 are reached and fail cleanly with exit 71, no generated
output, and the allocation-failure diagnostic; 63 through 128 are not reached
and the check succeeds. No ASan/UBSan diagnostic appears. These are the exact
allocation points of this fixture, not all checker paths. Raw evidence is in
ignored `build/slim-next-local-loans/`.

The permanent nested-local and disjoint-scope series measure 125, 250, 500,
and 1,000 aliases/scopes. Their check exponents are 0.342 and 0.692, below the
unchanged 1.25 ownership ceiling. These small fixtures include process startup;
the values do not establish sublinear checking. Existing scaling gates pass,
including the 1.30 owned-transfer normalized-cost ceiling (measured 0.945).

The same-host frontend comparison uses one warmup and seven alternating AB/BA
pairs per size against the retained `d6eeede` compiler. Binary identities and
every sample are in [the dated TSV](2026-09-06-local-loan-frontend-pair.tsv).

| Common-prefix declarations | Baseline median | Candidate median | Candidate / baseline |
| --- | ---: | ---: | ---: |
| 2,000 | 5.747 ms | 5.888 ms | 1.025 |
| 4,000 | 8.955 ms | 9.304 ms | 1.039 |
| 8,000 | 16.087 ms | 16.166 ms | 1.005 |
| 16,000 | 30.050 ms | 29.841 ms | 0.993 |

These measurements describe the named declaration fixture, not incremental
reuse, general compiler throughput, or agent effectiveness.

The portable fixed point is 2,952,691 C bytes, SHA-256
`22f421bfbc7209c7826155ed43d824af393c723d37dbe4f093e70a2492da556c`.
Bootstrap, governance, formatting, Clippy, all 7 unit and 52 integration tests,
248 conformance fixtures plus 2,000 malformed-input mutations, quick performance,
quick reduction, parallelism, resources, quick native comparison, agent checks,
and quick parallel runtime pass. Native SLIM/C and SLIM/Rust geometric means
across 20 challenges are 1.071 and 1.012. Generated parallel/serial ratios are
0.727 for `state_machine` and 0.693 for `signal_network`, within existing gates.

This checkpoint adds no syntax, runtime operation, dependency, or allocation.
Owned projections, borrowed enum payload scopes, definite reinitialization,
the formatter defect below, actual-work counters, and successor process decisions
remain before M0 closure. M1-M7 and controlled agent outcomes remain pending.

### Additional formatter defect discovered during validation

The retained `d6eeede` compiler accepts
[this source](../reproducers/formatter_leading_group.slim), but its formatted
output fails parsing with `E0102@144:146`. A local initializer ending in a bare
name precedes an expression that the formatter wraps in leading parentheses.
The parser treats the next line as continuation of the initializer. Direct
arithmetic without that preceding local does not reproduce the failure.
The same failure occurs in `e884563`. At that checkpoint it remained an explicit
M0 round-trip repair; the local-borrow fixtures use ordinary intermediate scalar
bindings to remain canonical. The following checkpoint addresses this witness.

## Call-prefix boundary checkpoint (2026-09-06)

RFC-0117 requires an ordinary call, struct constructor, or recurrence opener
to share its callee's line; an enum constructor's `Type::Case(` prefix stays
on one line. Arguments inside the parentheses may span lines. The parser uses
existing indexed line metadata. No formatter output change, semantic fallback,
new syntax form, dependency, or runtime operation is introduced.

The durable formatter witness now checks before and after formatting. A
permanent bounded matrix crosses three preceding local/assignment forms with
eight grouping/operator expressions, then adds the borrowed-name witness and
multiline function/struct/enum/recurrence arguments: 26 complete cases.
For every case, checking and C generation succeed before and after formatting,
generated C is byte-identical, and a second format is byte-identical to the
first. This proves those round trips, not every possible source program.

Six formerly accepted split prefixes now have exact rejection fixtures:
ordinary function, struct, recurrence, and enum splits before `::`, before the
case name, and before `(`. This source restriction affects noncanonical inputs;
canonical programs and multiline argument lists are preserved. Native execution
also covers the witness and multiline arguments. The witness returns 2 under
the conformance runner's two-element argument vector, and 0 without an extra
argument; its result is exactly `2 * vec.len(args) - 2` in this tested domain.

ASan/UBSan pass on compiler self-check, formatting and checking the witness,
and native execution of both positive fixtures. Fault injection into the
formatted witness check tests ordinals 1 through 128: 1 through 51 are reached
and produce exit 71, no generated output, and the exact allocation-failure
diagnostic. Ordinals 52 through 128 are not reached and the check succeeds.
This covers this fixture's allocation sequence, not every parser path.
Entire native challenge analysis reports remain byte-identical to `e884563`.
Ignored logs and sanitizer artifacts are in `build/slim-next-call-boundaries/`.

The existing separator-dense series retains its exact structural counts:
250/500/1,000/2,000 declarations have 6,771/13,521/27,021/54,021 lexical tokens,
1,000/2,000/4,000/8,000 commas, and 1,005/2,005/4,005/8,005 expression nodes.
Its measured check exponent is 0.554, below the unchanged 1.25 ceiling.
All previous scaling fixtures remain; the owned-transfer normalized ratio is
0.962 against its unchanged 1.30 ceiling.

`slim-bench frontend-pair BASELINE CANDIDATE --separator-lists` now selects
the same durable call-heavy generator for paired measurements. The default
declaration workload is preserved. One warmup and seven alternating AB/BA pairs
per size compare the retained `e884563` compiler against this checkpoint.
Binary identities and every sample are in
[the dated TSV](2026-09-06-call-boundary-frontend-pair.tsv).

| Call-heavy declarations | Baseline median | Candidate median | Candidate / baseline |
| --- | ---: | ---: | ---: |
| 2,000 | 12.061 ms | 12.215 ms | 1.013 |
| 4,000 | 21.382 ms | 21.480 ms | 1.005 |
| 8,000 | 43.190 ms | 42.786 ms | 0.991 |
| 16,000 | 87.559 ms | 88.773 ms | 1.014 |

These native process latencies describe this workload only; no incremental
reuse, general speedup, or controlled agent-effectiveness result is inferred.

The portable fixed point is 2,954,387 C bytes, SHA-256
`910af12f0002f2e9c0f8ee92522aa71eec27951889a9f8606fe9fb4abd541f19`.
Bootstrap, governance, formatting, Clippy, all 7 unit and 53 integration tests,
256 conformance fixtures plus 2,000 malformed-input mutations, quick performance,
quick reduction, parallelism, resources, quick native comparison, agent checks,
and quick parallel runtime pass. Native SLIM/C and SLIM/Rust geometric means
across 20 challenges are 1.115 and 1.025; generated parallel/serial ratios are
0.717 for `state_machine` and 0.678 for `signal_network`. No budget or native
analysis/resource baseline changed.

M0 remains in progress: owned projections, borrowed enum payload scopes,
definite reinitialization, actual checker/generator/cache work counters, and
successor process decisions still need implementation evidence. M1-M7 and
controlled agent outcomes remain pending.

## Enum match ownership checkpoint (2026-09-06)

RFC-0118 repairs enum match consumption and affine payload loans. An owning
match consumes its named scrutinee owners before checking the arms; selected
payloads receive ownership. Borrowed affine payloads retain shared access to
their origin throughout the arm, including when the enum parameter is
exclusive. Origin replacement, transfer, and exclusive access during the loan
are rejected. Scalar copies and cases without affine payloads reserve no loan.
The implementation reuses canonical move scopes and indexed loan records.

Both durable native invalidation witnesses are now production rejections:
[owned match](../reproducers/enum_owned_match.slim) reports E0315 at 533:538;
[borrowed match](../reproducers/enum_borrowed_match.slim) reports E0347 and
E0349 at 414:419. The retained `c976c5c` compiler accepts both, and its native
ASan/UBSan runs report heap-use-after-free. This is exact evidence for the two
witnesses, not a proof of general memory safety. The existing shared payload
mutation fixture retains E0347 and additionally reports its overlapping loan
with E0349 at the same span; no previous rejection is removed.

Positive scope-exit tests exposed invalid C for assignment to exclusive
parameters. Ordinary and counted lowering now write through the existing
checked parameter place. Counted assignment recursively lowers its right-hand
expression, preserving conditional and call evaluation and allocation-failure
checks. The counted integration fixture verifies three specialized stages,
then observes the updated vector from the caller, so successful compilation
alone cannot satisfy the regression test.

Twenty-four added conformance rows cover the two invalidation witnesses,
consumption inside and after a match, branch joins, collection origins,
mutation and transfer conflicts, nested and unknown origins, multiple payloads,
owned transfer, scalar and empty cases, scope exit, and ordinary/counted
exclusive assignment. All 12 new positive fixtures execute with exit zero under
ASan/UBSan. Seven emit C byte-identical to the baseline; five containing
exclusive assignment repair invalid baseline C. The final counted fixture was
regenerated and sanitized after its caller-observation assertion was added.

The sanitized compiler checks its own source successfully. Compiler allocation
fault injection for `enum_scope_exit.slim` checks ordinals 1 through 128:
1 through 85 are reached and produce exit 71, empty output, and the exact
allocation-failure diagnostic; 86 through 128 are not reached and succeed.
The final counted runtime fixture checks ordinals 1 through 64: 1 through 4
fail cleanly with exit 71 and the exact diagnostic; 5 through 64 succeed.
No ASan/UBSan diagnostic appears. These bounded experiments cover the named
fixtures' allocation sequences, not every checker or generated-program path.

The new permanent enum-match series grows 125, 250, 500, and 1,000 cases with
both borrowed and owning matches. Its measured check exponent is 0.421 against
the unchanged 1.25 ownership ceiling. Existing scaling gates remain, including
the 1.30 owned-transfer normalized-cost ceiling (measured 0.948). Fixed process
startup contributes to these fixture times; the exponent is not a claim of a
sublinear checking algorithm.

The paired frontend experiment retains one warmup and seven alternating AB/BA
pairs per size on the call-heavy separator workload. The retained baseline is
`c976c5c`; both binary identities still match
[the raw TSV](2026-09-06-enum-ownership-frontend-pair.tsv) after bootstrap.

| Call-heavy declarations | Baseline median | Candidate median | Candidate / baseline |
| --- | ---: | ---: | ---: |
| 2,000 | 12.384 ms | 12.174 ms | 0.983 |
| 4,000 | 21.911 ms | 22.091 ms | 1.008 |
| 8,000 | 43.861 ms | 43.982 ms | 1.003 |
| 16,000 | 86.980 ms | 88.772 ms | 1.021 |

These measurements establish no general speedup, incremental reuse, or agent
effectiveness result. Complete analysis reports for all 20 native challenges
are byte-identical to the retained baseline; all exact analysis/resource rows
remain unchanged.

The portable fixed point is 2,957,449 C bytes, SHA-256
`27521e806c19bb4358f4a6a18b62eda856edd19fc5d5c92f4687ffe671247868`.
Bootstrap, governance, formatting, Clippy, all 7 unit and 54 integration tests,
280 conformance fixtures plus 2,000 malformed-input mutations, quick performance,
quick reduction, parallelism, resources, quick native comparison, agent checks,
and quick parallel runtime pass. Three localhost tests require socket access
outside the restricted sandbox; the complete suite passes with that access.
Native SLIM/C and SLIM/Rust geometric means across 20 challenges are 1.090 and
1.052. Generated parallel/serial ratios are 0.872 for `state_machine` and 0.634
for `signal_network`, within existing budgets. Logs and sanitizer artifacts are
retained in ignored `build/slim-next-enum-ownership/`; `commit-*` files record
the final verification. No gate or baseline is relaxed.

M0 remains in progress: general owned projections, definite reinitialization,
actual checker/generator/cache work counters, and successor process decisions
still require evidence. M1-M7 and controlled agent outcomes remain pending.

## Checked field replacement checkpoint (2026-09-06)

RFC-0119 makes affine struct projections shared reads and introduces the one
explicit extraction operation `mem.replace(@place, replacement)`. It reserves
the named root, evaluates the equally typed replacement once, installs it in
the actual place, and returns the old owner. Nested struct fields and whole
affine names are supported. Shared, moved, unknown, temporary, and collection
places cannot grant replacement access. Root-level loans conservatively exclude
simultaneous access to disjoint fields. No partial-move state, raw pointer,
buffer copy, runtime ABI change, or allocation by the exchange is introduced.

The durable [field-alias witness](../reproducers/owned_field_alias.slim) was
accepted by de031be and produces heap-use-after-free under ASan/UBSan. Its
canonical source generates C byte-identical to the retained failing native
artifact. The repaired compiler rejects the mutating alias with E0347 and
E0349 at 426:431. Projected returns, aggregates, collection insertion,
assignment, enum construction, and freezing are also permanent rejections.
`bytes.freeze` now uses the same owned-argument check as user calls, closing
its previously unchecked projected-argument path.

The compiler's checker issue-vector extraction and project preparation now
use explicit replacement. The existing `ownership_modes` conformance fixture
also uses replacement before forwarding the extracted vector. Empty replacements
create no element buffers. The checked SLIM source and regenerated portable
seed reach the same fixed point; ignored transition compilers are not production
fallbacks.

Thirty-two added conformance rows comprise 24 rejection cases and eight native
positive cases. They cover nested places, shared/moved/unknown roots, active
loans, markers, arity and types, replacement ownership, scalar copies, lexical
scope exit, evaluation order, nested independent exchanges, result assignment
back to the replaced name, vectors, arenas, enum storage, and byte freezing.
The dedicated counted-loop integration test requires three specialized stages
and verifies caller-visible contents, mutation counts, and the parallel mutation
blocker independently of its exclusive-parameter blocker.

All eight positive fixtures pass ASan/UBSan. Runtime fault injection checks
ordinals 1 through 64 for each of `replacement_order`, `replacement_counted`,
`replacement_types`, and `replacement_nested_calls`: respectively 4, 3, 4, and
3 ordinals are reached. Reached ordinals fail with exit 71, empty output, and
the exact allocation-failure diagnostic; later ordinals succeed. The final
sanitized compiler checks its own source and emits replacement/payload analysis
without sanitizer diagnostics. Compiler checking of `replacement_nested_calls`
covers ordinals 1 through 128: 1 through 77 fail cleanly and 78 through 128
succeed. These are bounded experiments on the named fixtures, not a proof of
all memory paths.

### Retained ownership evidence

The audit found that binding reports inferred local ownership from storage type
and labelled enum payloads shared with unknown type. The checker now retains
each binding's actual type and mode on its stable canonical name fact. Reports
consume those facts and enumerate every payload. Missing binding evidence has
stable reason `missing-checked-binding`; the existing 64-binding report limit
remains explicit. Permanent tests cover local field borrows, owned/shared/scalar
payloads, multiple payloads, and 65 payloads crossing the reporting limit.

Only two complete native report records change: `variants.apply` bindings 131
and 143, both named `amount`, change from unknown/shared to `I64`/copy. Their
uses, scopes, last uses, and dependencies are unchanged. Every other complete
native report field is byte-identical. All 20 native analysis/resource summary
rows, complete blocker sets, and parallel site counts remain unchanged.
[The exact changed rows](2026-09-06-field-ownership-analysis-changes.tsv) are
retained separately. The existing `lifetimes` report assertion now checks its
known `Vec[I64]` payload type while preserving the same shared mode and spans.

The first representation added a third I64 to every fact. All seven paired
16,000-declaration comparisons were slower, with a 1.040 median latency ratio;
[those contrary samples](2026-09-06-field-replacement-wide-facts-pair.tsv) remain
recorded. That representation was replaced before committing. The final tagged
fact retains the original two-I64 size and encodes only the fixed type/mode
domain 0 through 30, whose arithmetic cannot overflow. The normal accessor
decodes types; no second ownership analysis or parsed representation is added.

The final same-host frontend comparison uses one warmup and seven alternating
AB/BA pairs against de031be on the call-heavy separator workload. Binary
identities and all samples are in
[the final TSV](2026-09-06-field-replacement-frontend-pair.tsv).

| Call-heavy declarations | Baseline median | Candidate median | Candidate / baseline |
| --- | ---: | ---: | ---: |
| 2,000 | 12.473 ms | 12.855 ms | 1.031 |
| 4,000 | 22.197 ms | 22.459 ms | 1.012 |
| 8,000 | 44.778 ms | 45.220 ms | 1.010 |
| 16,000 | 88.982 ms | 90.108 ms | 1.013 |

The final large-input median difference lies within the observed run variation;
no general speedup or incremental-reuse claim is made. The wide-fact regression
was addressed without a budget exception. Native compiler measurements and
source/edit proxies remain independent of unmeasured agent effectiveness.

The portable fixed point is 2,999,465 C bytes, SHA-256
`fc76bf91cb8e0b0fdf3628a1a84ef03307e33c84b9f5695a92c86123fa7899b1`.
Bootstrap, governance, formatting, Clippy, all 7 unit and 56 integration tests,
312 conformance fixtures plus 2,000 malformed-input mutations, quick performance,
quick reduction, parallelism, resources, quick native comparison, agent checks,
and quick parallel runtime pass. The complete test suite uses authorized local
socket access for its three localhost tests. The new field-replacement check
series has exponent 0.814 under the unchanged 1.25 ceiling; the existing owned
transfer normalized ratio is 0.973. Native SLIM/C and SLIM/Rust geometric means
across 20 challenges are 1.087 and 1.012. Generated parallel/serial ratios are
0.634 for `state_machine` and 0.618 for `signal_network`. Logs and sanitizer
artifacts are retained in ignored `build/slim-next-fields/`. No gate or baseline
is relaxed.

### Remaining M0 work

The [definite-reinitialization witness](../reproducers/ownership_reinitialization.slim)
moves a vector, assigns a fresh vector, and then reads it. The current compiler
still rejects that read with E0315 at 206:212. This is an exact false rejection,
not evidence of an accepted unsafe execution. Its repair needs branch-sensitive
initialization evidence and must retain all prior move/loan gates. Real
checker/generator/cache work counters, the remaining claim audit, and successor
process decisions also remain. M0 and M1-M7 are not declared complete here.

## Definite reinitialization checkpoint (2026-09-06)

RFC-0120 replaces the irreversible move-component interpretation with a lazy
per-owner availability stack. A valid whole-name assignment restores the
destination only after its right-hand side has checked and transferred. Every
branch arm starts from the entry state, and every arm must leave the owner
available before a following read. Missing arms retain the entry state; a
one-case enum introduces no extra alternative. General partial initialization
of moved aggregates remains unsupported.

The production SLIM checker owns these facts. Its `ownership` module records
canonical branch/arm scopes, and union by rank with path compression identifies
the nearest still-open ancestor of a previously visited scope. Only owners with
a move acquire frames. Accesses close the owner's completed frames, preserve
untouched alternatives, and enter the current scope. Each frame closes once;
no branch copies the binding table or replays an entire descendant event list.
This is an O(N alpha(N)) availability algorithm, not a second checker or parser.

The original [false-rejection witness](../reproducers/ownership_reinitialization.slim)
now checks and runs through the production compiler. Thirteen added conformance
rows comprise eight exact rejections and five native positive cases, including
that original witness. They cover nested and incomplete branches, enum matches,
mutable owned parameters and payloads, vectors/structs/enums/arenas, a second
move, conditional owning results, unavailable right-hand sides, borrowed values,
immutable destinations, wrong types, and outstanding loans.

The original 486-program move-only oracle remains unchanged. The new independent
path oracle checks all 15,552 programs in a fixed domain: no-op, read, move,
reset, move-then-reset, or reset-then-move at five positions, in both nested-tree
orientations. It enumerates each complete path rather than reproduce compiler
state machinery. A separate 16-program stress domain uses depths 1, 8, 64, and
128, one or 17 owners, and complete or incomplete reset trees. Incomplete trees
must retain an E0315 for every owner. These experiments are exact for their
enumerated domains and are not a proof of all ownership programs.

The conditional-result execution fixture exposed C `value = value`, rejected
by the strict native build. Lowering now emits a no-op only when both atomic
endpoints resolve to the same checked binding declaration; result slots,
temporaries, and other bindings retain ordinary assignment. No computation,
effect, trap, or allocation is skipped. Checked move and initialization rules
still apply before generation. Governance's structural checks now require the
new availability operations in place of the removed irreversible helpers.
The new scaling series use the existing 1.25 branch-move ceiling; no budget is
relaxed, and the original series remain permanent.

All five native positive cases pass ASan/UBSan. Allocation-fault ordinals 1
through 64 reach respectively 1, 13, 4, 2, and 4 failures in the original witness,
`reinit_branches`, `reinit_enum`, `reinit_transfer`, and `reinit_results`.
Reached ordinals exit 71 with empty output and the exact allocation diagnostic;
later ordinals succeed. The final sanitized compiler checks its own source.
Checking `reinit_branches` under fault ordinals 1 through 128 reaches ordinals
1 through 107 with the same clean failure behavior; 108 through 128 succeed.
These are bounded campaigns on the named operations.

All 20 complete native analysis reports and generated C outputs remain
byte-identical to checkpoint 87b2adf. No analysis/resource row, complete blocker
set, or parallel site count changes.

The paired ownership measurements use one warmup and seven alternating AB/BA
pairs on this host. At 1,000 owners, branched transfer medians are 22.768 ms
baseline and 22.979 ms candidate (ratio 1.009); straight transfer medians are
12.848 and 12.989 ms (ratio 1.011). Binary identities and all samples are in
[the ownership TSV](2026-09-06-reinitialization-ownership-pair.tsv).
These small differences lie within the observed variation and establish no
speedup. Compiler cost and source/edit proxies remain independent of unmeasured
agent effectiveness.

The final call-heavy frontend comparison uses the same warmup and pairing
protocol against 87b2adf. Its binary identities and samples are in
[the frontend TSV](2026-09-06-reinitialization-frontend-pair.tsv).

| Call-heavy declarations | Baseline median | Candidate median | Candidate / baseline |
| --- | ---: | ---: | ---: |
| 2,000 | 12.336 ms | 12.543 ms | 1.017 |
| 4,000 | 22.489 ms | 22.153 ms | 0.985 |
| 8,000 | 43.957 ms | 43.997 ms | 1.001 |
| 16,000 | 88.629 ms | 88.278 ms | 0.996 |

Wide and nested reinitialization check exponents are 0.776 and 0.645, below the
unchanged 1.25 ceiling. The existing owned-transfer normalized ratio is 1.067
under its 1.30 limit. Sublinear measured exponents include process overhead and
do not imply a sublinear algorithm. No incremental compiler reuse is inferred.

The portable fixed point is 3,020,129 C bytes, SHA-256
`71c1ecfd9a2abbff28e13ce1f0e87e37c382284a3072c2493a7ecfb579cf6e4f`.
Bootstrap, governance, formatting, Clippy, all 7 unit and 58 integration tests,
325 conformance fixtures plus 2,000 malformed-input mutations, quick performance,
quick reduction, parallelism, resources, quick native comparison, agent checks,
and quick parallel runtime pass. The full suite uses authorized localhost
socket access. Native SLIM/C and SLIM/Rust geometric means over 20 challenges
are 1.106 and 1.014. Generated parallel/serial ratios are 0.660 for
`state_machine` and 0.589 for `signal_network`, within their existing budgets.
Logs and sanitizer artifacts are in ignored `build/slim-next-reinitialization/`;
the pre-final logs retain the earlier missing-budget-mapping failure and the
frontend measurement preceding the lowering correction. The final measurements
ran after the tests and sanitizer campaigns completed, without that contention.

M0 still needs actual checker/generator/cache work counters, the remaining
claim audit and process decisions, and the full release gate. No M1-M7
completion is claimed.

## Observed compiler-work checkpoint (2026-09-06)

RFC-0121 adds an opt-in measurement binary made from the byte-verified production
seed. Thirty-two counters observe actual native function entries, loop-header
visits, parse input bytes, and runtime file reads. The ordinary compiler, runtime,
and seed are unchanged and incur no instrumentation cost. Counter values are
exact for each observed process up to the fixed 1,000,000,000 cap; overflow reports
bounded evidence, and missing or incomplete reports are unknown, never zero.
The cap-7 native UBSan test crosses saturation with `UINT64_MAX` without overflow.
Missing and duplicated observation anchors fail closed.

The [full-size records](2026-09-06-observed-compiler-work.tsv) contain 214 compiler
operations, each compared with ordinary production output/diagnostics/exit status
and repeated twice with identical counters. The 6,858 measurement rows comprise
32 counters for each operation plus ten separately counted external C builds.
The generated native project programs also run successfully. Setup compilation,
seed reproduction, backend compilation, and frontend observations are distinct.
All 20 native challenge C outputs match the ordinary compiler byte for byte.
Compiler, seed, input, output, instrumentation, and configuration identities are
recorded; their FNV hashes are identity aids, not authentication.

| Two-module operation | Program parses | Checker calls | Generator calls | File-read calls |
| --- | ---: | ---: | ---: | ---: |
| Clean emission | 3 | 1 | 1 | 4 |
| Cold cache miss | 3 | 1 | 1 | 7 |
| Unchanged cache hit | 0 | 0 | 0 | 4 |
| Unchanged snapshot comparison | 4 | 0 | 0 | 6 |

Clean project loading parses both modules and reparses the flattened source.
The first campaign expected two parses and failed; observing three exposed that
additional production work, and the assertion now names the real behavior.
An unchanged snapshot prints `0 0 0 0` invalidation estimates while still doing
four source parses. A real artifact hit avoids the checker and generator but
still reads source and lexes its key manifest. These results establish no M1
retained-query implementation. Failed-then-recovered session comparisons actually
check their inputs; invalidation estimates are not a substitute for those calls.

Permanent deterministic gates cover geometric declarations (1,000 through 8,000
in full mode; 125 through 1,000 in quick mode), lexer and name lookup work, and
owner reinitialization. At 1,000 owners, the availability campaign records 3,000
frame closes and 5,006 root calls, inside its fixed 3N and 16N+32 bounds. Edits
cover body, signature, layout, effects, borrow mode, insertion, deletion, rename,
and ordering; relocation, corrupt frames, rejected source, and recovery retain
clean-output equivalence. The 128 allocation-fault ordinals observe partial work
and agree with the ordinary compiler; 107 reach the named failure point.

The work campaign runs in ordinary quick/full modes and quick ASan/UBSan mode.
It is part of `scripts/verify.sh`. Instrumented timing is deliberately not used
as ordinary compilation latency or evidence of agent effectiveness. No existing
performance budget, analysis baseline, or accepted source behavior is relaxed.

Bootstrap retains the 3,020,129-byte seed with SHA-256
`71c1ecfd9a2abbff28e13ce1f0e87e37c382284a3072c2493a7ecfb579cf6e4f`.
Governance, formatting, Clippy, all 10 unit and 58 integration tests, 325 conformance
fixtures plus 2,000 malformed mutations, quick performance/reduction, parallelism,
resources, quick native comparison, quick parallel runtime, and agent gates pass.
The native SLIM/C and SLIM/Rust means are 1.172 and 1.042; parallel/serial ratios
are 0.761 (`state_machine`) and 0.743 (`signal_network`). The earlier part of the
benchmark batch overlapped other verification, so these passing gate values
establish no latency comparison. Raw verification logs are in ignored
`build/slim-next-work/`. The initial Rust classification failure was resolved by
adding the measurement module to the infrastructure ledger, without a production
Rust exception or budget relaxation.

### Remaining M0 defect

The new [shadowed-initializer witness](../reproducers/shadowed_initializer.slim)
checks and builds, but its native ASan/UBSan execution returns 2 instead of the
source result 42. Code generation gives both lexical bindings the same C name
and declares the inner zero-initialized C local before evaluating its initializer.
The checked initializer refers to the outer binding, so this is an exact
lowering defect. The witness is preserved for repair; it is not accepted as a
passing conformance row with the wrong result. M0 also retains its claim audit,
successor process decisions, and full release gate. M1-M7 remain pending.

## Lexical binding identity checkpoint (2026-09-06)

RFC-0122 repairs the shadowed-initializer witness: native execution now returns
42. Every generated source variable carries its checked declaration node as a
suffix. References, declarations, mutation destinations, payload extraction,
recurrence arguments, and parallel captures share that emitter. The seven-line
production change adds no checker pass, metadata table, runtime operation,
source syntax, or semantic fallback. Compiler-generated anonymous Void bindings
also receive distinct names.

An initial attempt to mark only shadowed declarations was overwritten by the
checker's binding indexing; its witness still returned 2. It was removed. The
final implementation uses the use-to-declaration links already retained by the
checker, including the original unlinked node for declarations. Source underscores
are mangled as `_95`, so source names cannot collide with the `_n` node suffix.

The six permanent conformance rows comprise three executions and three exact
rejections: original initializer, nested scalar/owned/enum/exclusive/recurrence
bindings, structured parallel captures, undefined initializer, moved initializer,
and wrong type. Unsupported scalar exclusive parameters and non-effectful
explicit parallel tasks were rejected while preparing the fixtures; positive
cases use the already accepted source contract. A separate 32-program domain
checks shadowed and alpha-renamed variants across depths 1, 2, 8, and 32, both
branch values, and parameter/local roots. Its expected arithmetic results are
independent of the backend. This is exact for that domain, not a universal proof.

All positives pass ASan/UBSan. The parallel fixture passes both inline and POSIX
worker tiers. Across application fault ordinals 1 through 64, the witness reaches
one failure, the mixed-binding fixture three, and each parallel tier one. Other
ordinals produce the expected result with empty output. The sanitized compiler
checks itself; all 128 fault ordinals on the mixed-binding source reach clean
exit-71 failure. This bounds the tested failure domain, not all allocations.

All 20 complete native analysis reports remain byte-identical, including every
blocker and resource fact. Generated C differs only in local identifiers, with
[every native size change recorded](2026-09-06-binding-identity-native-changes.tsv).
Existing ABI and arithmetic-lowering assertions ignore only those suffixes; their
other predicates and native execution requirements remain unchanged.

The seed grows from 3,020,129 to 3,516,365 C bytes (16.4%). This representation cost
is explicit. Same-host checking on 2,000/4,000/8,000/16,000 call-heavy declarations
has baseline/candidate medians of 11.757/11.663, 21.588/21.031, 44.208/42.969, and
86.539/87.401 ms. The final ratio is 1.010, with no general checking-speed claim.
[Frontend pairs](2026-09-06-binding-identity-frontend-pair.tsv) identify both
binaries and retain all seven alternating AB/BA pairs after one warmup.

[Emission pairs](2026-09-06-binding-identity-emission-pair.tsv) offer the same
current compiler source to both binaries and exclude external C compilation.
All seven candidate runs are slower; medians are 176.049 versus 181.235 ms,
ratio 1.029. RFC-0122 therefore records Compile -1 and score 10. No budget is
relaxed, and the generated size increase is not disguised as a performance gain.
No agent productivity conclusion follows from compiler timings.

Bootstrap, governance, formatting, Clippy, all 10 unit and 59 integration tests,
331 conformance fixtures plus 2,000 malformed mutations, quick performance and
reduction, parallelism, resources, quick native comparison, quick parallel runtime,
and agent gates pass. The work campaign passes with the new seed in ordinary and
sanitizer modes; input-byte hooks resolve suffixed formals in exact generated
signatures. Native SLIM/C and SLIM/Rust means are 1.066 and 1.025; parallel/serial
ratios are 0.642 (`state_machine`) and 0.567 (`signal_network`). The seed SHA-256 is
`a7e4b2f8de6696c27880ebaf170392c7f491b73bdd9157846bf5a9ddc05ae9e4`.
Logs, failed attempts, and sanitizer artifacts are in ignored
`build/slim-next-shadow/`. M0 still requires the final claim audit, successor
process decisions, and full release gate. M1-M7 remain pending.

## M0 exit audit and successor boundary (2026-09-06)

RFC-0123 records the M0 acceptance ledger and the boundary before M1. No change
to AGENTS.md, FEATURE_POLICY, source syntax, production authority, ABI, or any
performance limit is needed for the next compiler-substrate experiment. M1 typed
identities, derived views, and retained queries still need their own accepted
architecture specifications. The M2 policy and compatibility conflicts listed
in RFC-0112 remain unresolved until their dependent experiments. M0 approval
cannot serve as approval for those changes.

### Review-observation ledger

| RFC-0112 observation | Permanent production evidence | Result/domain |
| --- | --- | --- |
| Local affine assignment leaves its source available | `local-move`, `assignment-move`, `aggregate-move`, `collection-move`, conditional and field-transfer conformance rows | Exact E0315/E0347/E0349 rejections; owning destinations transfer rather than retain usable aliases |
| Recurrence and direct/mutual recursion lack `partial` enforcement | `termination-unconditional`, `termination-direct`, `termination-mutual`, and the guard/controller/step/prefix boundary rows | Exact E0343 rejections; positive structural descent and declared-partial cases execute |
| Snapshot estimates presented as actual work | RFC-0121 `work` campaign, including unchanged snapshots, cache hits/misses, edits and recovery | 32 real counters, fixed cap 1,000,000,000; estimates stay distinct; missing observations unknown |
| Quadratic duplicate-declaration scan | Permanent common-prefix geometric check series and native name-lookup observations | Existing 1.25 exponent budget retained; observed lookup work bounded by the stated source-size formulas |
| Valid moves in exclusive arms rejected | `branch-moves`, branch-join negatives, 486-path move oracle, 15,552-program reset oracle | Exact results for the enumerated domains; incomplete joins stay unavailable |
| Infinite by-value storage accepted | Four `inline-*-cycle` rejections and `recursive-collection-layout` execution | Exact E0354 rejection of impossible inline cycles; indirection remains supported |
| A 35-byte truncated cache traps | Production `project-cache-corruption`, every incomplete key prefix, framing/checksum/schema corruption, RFC-0121 clean-output comparisons | Exact miss/rebuild behavior for the enumerated corruptions, including length 35 |

All eight subsequently preserved witnesses in `benchmarks/reproducers/` occur
in `conformance/manifest.tsv`: nested-call and lexical loans, both enum-match
ownership cases, owned field aliasing, definite reinitialization, formatter
leading grouping, and shadowed initialization. Their expected diagnostics or
native output are permanent production checks. Follow-up positive/negative
cross-products and sanitizer/fault bounds are recorded in the preceding
checkpoint sections; there is no known unclosed reproducer in this ledger.

### Current-claim audit

The design overview and project handbook still described implemented incremental
checking; they now describe artifact caching and snapshot estimates, with links
to observed work. README distinguishes the two measurements and names the full
release gate. The historical Core 0.1 roadmap result now identifies Rust reuse
as historical, outside the current production compiler. INCREMENTAL, PROJECTS,
PERFORMANCE, QUALITY, status, and compatibility summaries retain the distinction
between current behavior, goals, bounded evidence, and unmeasured agent outcomes.
No historical measurement or RFC is silently relabelled for the current compiler.

The website audit exposed two existing unclassified library documents and stale
archive counts frozen at 109 RFCs. Both experimental-library documents are now
published; RFC status and search counts are checked against canonical source
files, while exact legacy disposition checks remain. Build output is excluded
from source discovery, consistently with the existing ignored generated-output
roots. Publication still accounts for every actual Markdown source exactly once.
RFC-0111 remains proposed: the experimental library is a repository corpus,
not newly added to the 0.9 release manifest.

### Gate status before the closure run

The complete `scripts/verify-0.9.sh` gate remains pending on the closure revision.
Its package step requires committed, clean included sources. M0 is not marked
complete until repository verification, two-archive reproducibility, clean
installation/bootstrap/execution, ABI mismatch rejection, and website checks
all pass. Bounded campaigns establish their named domains; universal safety and
agent effectiveness remain unknown, and M1-M7 implementation remains pending.

## M0 completed (2026-09-06)

The full `./scripts/verify-0.9.sh` command passed on committed, clean revision
44062be, with compiler repair checkpoint ea59312 and portable seed SHA-256
`a7e4b2f8de6696c27880ebaf170392c7f491b73bdd9157846bf5a9ddc05ae9e4`.
It reported `SLIM 0.9 verification: repository, release, and website gates passed`.
The two source archives were byte-identical with SHA-256
`580e557ab454f4c88bec983b00457acc6c20481cda3caf8ecac860ff879e6dee`.
This archive identity belongs to 44062be, before this closing documentation
record, and is not relabelled as the digest of subsequent source archives.

The complete gate includes bootstrap fixed point, formatting, Clippy, governance,
10 unit and 59 integration tests, 331 production conformance fixtures and 2,000
malformed-input mutations, ten library projects plus twelve valid generated
programs and twelve rejected mutants, every existing compiler/runtime/tooling
benchmark gate, the new actual-work campaign, sanitized compiler and generated
application execution, and allocation-failure checks. It then passed repeated
archive generation, safe extraction, clean bootstrap/build/run, runtime ABI
mismatch rejection, and all 18 website tests. The earlier focused ASan/UBSan,
worker-tier, bounded fault, ownership-path, and alpha-renaming campaigns remain
recorded above; the release gate does not substitute for those named domains.

The complete release log is retained locally in ignored
`build/slim-next-closure/verify-0.9.log`. Its SHA-256 is
`bc6a93082ed6374b97467965fe5c5ca8d4772289629bd4564c8be9a1373e2eca`.
Website preparation initially found unclassified library documents and stale
archive assertions; those failures were repaired before the complete gate.
The default sandbox blocked the website build worker's local port, so the final
suite ran with authorized localhost access. No acceptance check, performance
budget, diagnostic, native analysis baseline, or language hard gate was removed
to obtain a pass.

All RFC-0112 M0 exit obligations are satisfied for the recorded repair and
validation domains. No listed review reproducer remains open. M1-M7 are pending;
there is no retained incremental checking, successor syntax/lifetime system,
agent productivity result, or OS implementation implied by M0 completion.
The observed 16.4% seed-size increase and 1.029 compiler-source emission ratio
from RFC-0122 remain explicit costs. Future counterexamples require new repairs;
passing this bounded evidence is not a universal safety proof.

## M1 architecture checkpoint (2026-09-06)

The maintainer started a new goal to finish M1 from the clean M0 checkpoint
0265c6d. RFC-0124 records the retained-substrate contract before production changes.
It covers the whole M1 scope; individual stages do not substitute for completion.

The code audit establishes the following implementation constraints:

- `session.run` retains two source snapshot models and prints estimates. It has
  no retained checked result or generated fragment.
- `query.find_snapshot` can scan all previous declarations, provider lookup scans
  snapshots by qualified spelling, and invalidation scans the edge list for each
  provider. These routines are not the new semantic dependency authority.
- `typing.analyze` mutates canonical links and produces facts for the whole source.
  Reusable checking needs explicit declaration-owned results and complete mapping
  into any transitional flat view.
- `project.PreparedProject` owns flattened source/tokens, facts, diagnostics and
  a memory plan for one operation. Cached results need session-owned immutable
  records and revision-checked references, rather than aliases into these mutable
  buffers.
- Generated C names include global canonical node numbers. Insertion and reordering
  change those numbers. Retained emission needs stable owner-local naming or typed
  emitter relocation records, with clean and updated output using the same rules.
- Optimization dependencies include caller argument facts, callee body facts,
  lexical order, and shared analysis/refinement limits. Interface-only invalidation
  is insufficient; globally consumed bounds cannot be relabelled local constants.

The first implementation stage is lossless declaration/source mapping and typed
revision-owned identities, followed by the derived function view and reusable
checker entry points. Retained dependency queries, successful-only publication,
analysis/emission/backend reuse, corruption recovery, and full differential and
scaling evidence remain required stages. No production code, runtime, seed,
dependency, language syntax, or performance budget changes at this checkpoint.


## M1 source identity checkpoint — 2026-09-06

RFC-0125 implements the first source ownership boundary under RFC-0124. Production
`selfhost/identity.slim` now defines nominal revisions, files, declarations, nodes,
spans, and flat-view adapters. Node ordinals are local to a declaration version;
full file/declaration identity and bounds are checked before index arithmetic.
Span resolution retains the original bytes and validates half-open bounds. Revision
advance/reset reject invalid or exhausted counters instead of wrapping.

`query.Snapshot` stores typed declaration views and source spans. State construction
validates them before dependency scanning, which resolves a declaration-owned root
before traversing current canonical tokens. The session invocation owns its three
possible revision numbers. No identity is serialized or accepted from another
process. This is a transitional checked boundary, not retained semantic authority.

The session path also validates manifest token shape before loading modules and
propagates failed reads/parses instead of reporting estimates from a partial index.
Invalid source-index state returns status 65 and `Q0001: invalid source identity`.
Valid historical output remains unchanged. Existing module/source errors continue
to carry their diagnostics. No source syntax, type rule, runtime ABI, dependency,
or performance budget changed.

Validation at this checkpoint:

- Exact within the named boundary domain: 2,401 node-range cases, 49 byte-span
  cases, 2,401 owner combinations checked for both nodes and spans, and 98 revision
  advance/reset results: **7,350 results** compared with an independent Rust oracle
  using I128 arithmetic. Cases cross I64 minimum/maximum, stale epochs/revisions,
  wrong files/declarations, negative slots, empty ranges, and zero-width EOF spans.
  Nominal file-to-node substitution is rejected with E0344 through the production
  compiler. This is a bounded test domain, not a universal semantic proof.
- Production sessions preserve comparisons after relocation, leading comments,
  declaration reordering, and identical CRLF source; a changed body changes its
  estimate. Missing manifests/modules, malformed manifests/modules, and recovery
  are permanent regressions. The existing conformance recovery fixture still passes.
- Bootstrap reproduced **3,566,205 C bytes**, SHA-256
  `b59a9272485a643c922f797c16cec4348e9341700fe86a1418f4f51c8e13b629`.
  Seed growth from M0 is 49,840 bytes (1.42%); global generated identifiers explain
  much of the textual diff. The ordinary clean path allocates no new per-node table.
- Cargo passes all 10 unit and 61 integration tests; conformance passes all 331
  fixtures and 2,000 deterministic malformed-input mutations. Governance, canonical
  formatting, Clippy, and the required performance, reduction, parallelism,
  comparison, and agent gates pass. Resource, parallel-runtime, incremental, and
  actual-work gates also pass; native baseline gates were not relaxed.
- ASan/UBSan compile and run the actual production identity module and match ordinary
  output. A permanent source-index fault campaign crosses 128 allocation ordinals:
  **109 injected failures** exit 71 with exact runtime diagnostics and no estimate;
  19 ordinals beyond the input's allocations preserve normal output. The gate lives
  in `scripts/verify.sh`. These counts describe this input and runtime only.
- All 18 website tests pass with the new RFC. The full release/package gate remains
  required at M1 closure; this checkpoint does not claim a new complete release.

The [same-host measurements](2026-09-06-m1-source-identities.tsv) retain two warmups
and 11 alternating-order pairs per operation (warmups discarded). Median candidate
ratios to the M0 binary are 1.0084 for emitting the current compiler source, 1.0038
for checking 1,000 declarations, and 0.9816 for an unchanged 1,000-declaration
session. Every paired stdout is identical. These observations establish no speedup
claim or cross-machine timing budget. The actual-work gate continues to show fresh
parsing and zero retained checker/generator queries for the session estimator.

Two existing generated-C limitations were exposed while constructing the probe:
inline aggregate definitions follow source order (the new leaf identity module
must precede its consumers), and spelling the minimum I64 directly emits an
`INT64_C` literal rejected by the strict C warning gate. The probe constructs the
same minimum value by checked subtraction, so the boundary domain still includes
I64 minimum. Neither C limitation is claimed repaired here; both are explicit
follow-up obligations for the M1 emission work.

M1 remains **in progress**. Cross-revision declaration/node/span translation is not
yet enabled; full keys and source bytes still drive only historical comparisons.
Typed semantic identities, derived function control flow, reusable checker queries,
complete dependency observation, transactional retained publication, cached
analysis/C/backend work, and their differential/locality evidence remain required.
RFC-0125 is complete as a child boundary; RFC-0124 remains implementation pending.
Ignored detailed logs are under `build/slim-next-m1/source-identity/`.

## M1 exact revision maps — 2026-09-06

RFC-0126 adds explicit exact-content maps and removes the quadratic fallback
search from declaration matching. A map requires complete key/body equality,
validated views/spans, equal node counts, one session epoch, and an increasing
revision. Translation produces new typed owners while preserving local ordinals
and contained byte intervals. The old handle remains invalid in the new view.
These are compiler-owned source maps, not imported certificates or semantic facts.

Full keys traverse module bytes, one of three non-byte kind separators, and name
bytes in the existing name trie. The compiler compares complete keys on lookup.
Shared insertion/lookup traversal retains the normal linker's first-declaration
behavior; baseline and candidate compilers emit byte-identical C for the current
compiler source. The current canonical name links also detect duplicate declaration
names before optional index construction, retaining Q0001/status 65 for invalid
source indexes.

Aligned declarations use direct full equality. A mismatch builds one prior-state
index, which remains in that state for later lookups and calls. No key-encoding
buffer is allocated. An early prototype that allocated keys and built both indexes
showed a 14.5% unchanged-session slowdown at 1,000 declarations; it was replaced
before this checkpoint. The final implementation preserves explicit mapping checks
and their measured cost instead of claiming zero source work.

Validation and observations:

- The production mapping module is exercised on every node ordinal and every byte
  interval around three small declarations, with insertion, reordering, shifted
  origins, retained comments, LF and CRLF. It compares mapped intervals with the
  new source bytes. Wrong owners, stale prior revisions, epoch reset, backwards
  or equal revisions, changed bodies, changed keys, truncated views, and
  deletion/reinsertion history reject mapping. Repeated update calls exercise
  retention of the prior-state index. Adversarial compound keys and duplicate
  keys have separate regressions. This is a named finite test domain, not a
  universal semantic safety claim.
- The RFC-0125 7,350-result boundary oracle remains permanent; its decoder now
  distinguishes an invalid enum case from an erroneously successful negative
  position. Duplicate source declarations are also tested through the real session
  command, rather than only through direct index construction.
- Actual observation adds six counters for key insertion/lookup, map construction,
  node/span translation, and source-span comparison. The two existing trie-step
  counters follow their shared traversal helpers; normal linker work is still
  counted. The fixed cap remains 1,000,000,000 with exact/bounded/unknown meanings.
- Eight geometric campaigns at 125, 250, 500, and 1,000 functions preserve ordinary
  and instrumented output and repeated counter equality. Unchanged aligned input
  builds no fallback index. Reordering inserts exactly N+1 keys once; lookup count
  reflects the directly aligned middle declaration/main. Every matched declaration
  attempts one map and translates one root/span. Character/byte work remains under
  permanent linear source-byte budgets. The complete work campaign passes;
  [304 mapping-work rows](2026-09-06-m1-revision-map-work.tsv) are retained.
- Bootstrap reproduces **3,609,454 C bytes**, SHA-256
  `56989ebfd4cfcce36a0bf8e842339d12c1db269d3c4cf8cc4355a00e794ad41c`:
  43,249 bytes (1.21%) above checkpoint 35a8afd. On this host, a snapshot is 192
  bytes (previously 184), a transient mapping 192, an optional name-index record
  88, and a source-state record 184. Trie nodes/edges allocate only when needed;
  these record sizes are not peak-memory measurements or portable ABI promises.
- All 10 unit and 62 integration tests pass, plus the expanded LF/CRLF and duplicate
  source tests. Conformance remains 331 fixtures and 2,000 malformed-input mutations.
  Bootstrap, governance, formatting, required performance/reduction/
  parallelism/comparison/agent gates, and resource/parallel-runtime/incremental/work
  gates pass. ASan/UBSan execute the production mapping fixture; the 128-ordinal
  source-index campaign still observes 109 failures with exit 71 and no estimate,
  plus 19 unchanged successes beyond the allocations made. The full actual-work
  campaign also passes under ASan/UBSan. Its additional reordered-input campaign
  crosses all allocations in 128 ordinals: 76 failures and 52 successes. Five
  failures occur after lazy key insertion has begun (ordinals 72–76), with exit 71
  and no estimate. The gate asserts both index-stage failure coverage and success
  beyond the fixture's allocations. [Per-ordinal observations](2026-09-06-m1-revision-map-faults.tsv)
  are retained. No gate is relaxed.

[Paired uninstrumented measurements](2026-09-06-m1-revision-maps.tsv) retain eleven
alternating-order pairs after two warmups at each of six sizes. At 1,000 functions,
unchanged sessions are 5.800 ms versus 5.794 ms; reordered sessions are 6.063 ms
versus 8.213 ms. At 4,000 functions, unchanged sessions are 11.826 ms versus
11.373 ms (1.040 ratio), while reordered sessions are 13.315 ms versus 49.433 ms
(0.269 ratio). The new exact mapping work has an aligned-path cost; it is not
represented as free or as a cross-machine latency guarantee. RFC-0126 records
Compile -1 and Analysis +2, weighted score 5, for these separate costs/benefits.

M1 remains **in progress**. The session still parses both versions and reports
historical invalidation estimates. Its syntactic dependency discovery is not the
complete semantic dependency engine. Retained checked results, typed semantic
identities, the derived control-flow/ownership foundation, transactional candidate
publication, cached analysis/emission/backend work, corrupt-cache recovery across
the complete configuration identity, and full M1 differential/reuse/release gates
remain required. The previously recorded C type-definition ordering and minimum
I64 literal emission issues remain open for the emission stage. Detailed logs are
under `build/slim-next-m1/revision-mapping/`.


## M1 independent function checking — 2026-09-06

RFC-0127 makes the existing production function checker independently callable.
Each call creates its own binding, loan, ownership-scope, and branch-frame scratch
state; parameter/body/borrowed-return rules remain in the same SLIM implementation.
Binding facts are materialized into current canonical-source slots before the
operation returns. The whole-program driver retains its lexical order, global
link/layout/interface checks, diagnostic stop policy, and downstream analyses.
There is one inference implementation and no retained semantic result yet.

The positive scratch links on binding-name tokens now start locally in each
function. Canonical declaration links and retained facts remain the interface to
downstream consumers. The current conservative memory planner uses the caller
region for a function with exclusive output parameters, so physical scratch
allocations remain until that region closes. Logical independence is implemented;
per-function physical reclamation is not. This distinction is explicit in the
accepted contract and must remain visible during retained-session storage design.

Validation:

- The production SLIM checker probe checks all 92 accepted conformance/native
  files in reverse and then forward order, comparing every type/mode fact and all
  six fields of every canonical token with ordinary checking. This covers owned
  parameters, loans, branch reinitialization, and recurrence. Cargo and the
  permanent ASan/UBSan verification script run the same probe.
- Baseline 2a37677 and candidate produce identical status/stdout/stderr for all
  193 rejected fixtures. All 20 native applications produce identical generated
  C and complete analysis. The existing M0 ownership path oracles remain passing.
- Bootstrap reproduces **3,610,169 C bytes**, SHA-256
  `bcca014c1c2ac92a45d4189e7377d447297dcd12843f6a4a91ec1111ecbbcd36`,
  715 bytes above 2a37677. All 10 unit and 63 integration tests, 331 conformance
  fixtures plus 2,000 deterministic mutations, governance, formatting, Clippy,
  required performance/reduction/parallelism/comparison/agent gates, and
  resources/parallel-runtime/incremental/work gates pass.
- Clippy found `revision_mapping_campaign` placed after the test module in the
  previous checkpoint. Moving the test module last fixes it without changing
  behavior. The previous section's blanket Clippy-pass claim was inaccurate for
  that final file ordering; this checkpoint supplies the corrected passing result.
- The work observer now has 42 counters. The generated family checks exactly N+1
  bodies and visits 2(N+1) binding-materialization headers. A permanent bound of
  16N+128 allocation attempts holds for check and emission. Allocation counters
  include an injected failed attempt and exclude subsequent declined calls;
  requested bytes exclude runtime metadata/libc overhead and do not mean peak
  memory. [504 geometric work rows](2026-09-06-m1-function-work.tsv) retain
  ordinary-versus-sanitized output equality and repeated exact observations.
- The 128-ordinal ownership fault campaign reaches 111 failures, including 26
  after function checking begins (86–111), and 17 successful ordinals beyond the
  allocations made. Every failed attempt equals its injected ordinal and has
  no partial standard output. [Per-ordinal results](2026-09-06-m1-function-faults.tsv)
  are retained. Source-identity and revision-map sanitizer probes and both older
  allocation-fault campaigns remain passing; no gate is removed or relaxed.

[Uninstrumented paired measurements](2026-09-06-m1-function-checking.tsv) use eleven
alternating-order pairs after two warmups at six sizes from 125 to 4,000 functions.
At 1,000 functions, checking takes 7.500 ms versus 7.215 ms (1.040 ratio), while
emission takes 14.090 ms versus 13.967 ms (1.009 ratio). At 4,000, these ratios
are 1.042 and 1.034. Native output and diagnostics match for every timed run.

[Separate allocation measurements](2026-09-06-m1-function-allocation.tsv) record
attempts, cumulative requested payload, and peak live payload. At 1,000 functions,
checking makes 9,099 versus 7,106 attempts and requests 9,226,388 versus 8,412,692
payload bytes; peak live payload is 6,058,867 versus 5,172,019 bytes (1.171 ratio).
At 4,000, peak payload is 24,230,179 versus 20,684,515 bytes. Emission's peak
payload at 1,000 is 26,446,899 versus 25,560,051 bytes. The additional setup remains
linear but is a real cost: RFC-0127 records Compile -1, Analysis +2, score 5.

The separate memory observer increments attempts/requested bytes at the existing
runtime attempt increment, adds live payload after successful `calloc`, subtracts
the stored allocation size at both existing free sites, and reports at process
exit. Checked U64 arithmetic rejects observer overflow; every measured run ends
with zero live payload and the same standard output as the ordinary compiler.
These figures exclude runtime headers, allocator overhead, and external C backend
memory; they are not RSS measurements. Compiler binaries use the ordinary strict
O2 flags; the observer is compiled separately with assertions enabled. Timing runs
use uninstrumented binaries. Artifacts record compiler/runtime/observer identities.

M1 remains **in progress**. This checkpoint establishes a reusable body-check
operation, not a control-flow graph, typed persistent semantic facts, retained
queries, complete dependency observation, transactional publication, or cached
analysis/C/backend work. Those requirements, the two recorded C emission defects,
and full M1 differential/locality/release validation remain outstanding. Detailed
logs and the separate measurement recipe are under
`build/slim-next-m1/function-checking/`.

## M1 checked layout order — 2026-09-06

RFC-0128 resolves the recorded C aggregate-definition ordering defect before
introducing more nominal compiler records for the flow view. The old compiler
accepted a record/enum with an inline reference to a later definition, then emitted
C which failed strict native compilation with an incomplete-field-type error.
Forward typedefs did not supply the complete field layout.

The existing inline-layout checker already traverses each acyclic dependency before
marking its owner complete. It now retains that completion order in `typing.View`;
project preparation carries the same result, and both generator callers pass it
alongside checked facts and the memory plan. The backend consumes it once, with no
second graph traversal or type checker. Existing completion marks deduplicate shared
dependencies. Forward typedefs remain lexical; complete definitions follow checked
field/payload dependencies. Containers and Id continue to break inline dependencies.
Field/case order, tags, C representation, function bodies, and runtime ABI do not
change. This is current-source checked data, not a cross-revision cache entry.

Validation and observations:

- All 24 permutations of a four-type mixed record/enum diamond compile and run
  with result 42. Each definition occurs exactly once and after its inline
  dependencies. A collection wrapper is emitted before those definitions and
  supports its existing legal collection-mediated self-reference. Repeated C is
  byte-identical for every input. A separate three-module forward-inline case
  also compiles and runs through the production compiler.
- The new permanent `inline_forward_layouts.slim` conformance fixture reproduces
  the baseline C compiler failure and succeeds with the candidate. The verification
  script emits it with the sanitized compiler and runs the generated native
  program under ASan/UBSan. The independent function-checking probe now covers 93
  accepted files, including this fixture; its reverse/repeated fact comparisons
  remain passing.
- All 193 rejected fixtures retain exact status/stdout/stderr, including inline
  cycles, unknown types and malformed declarations. All 20 native applications
  retain byte-identical generated C and complete analysis. The layout order is
  not used to authorize invalid source; ordinary failure gates remain in force.
- The seed reaches **3,613,447 C bytes**, SHA-256
  `099228edbfc22e62cc81ff5ef29ea932ff9103f7c0f7389a391cc77c2cad4987`,
  3,278 bytes above 8d7b504. All 10 unit and 64 integration tests pass, together
  with 332 conformance fixtures and 2,000 deterministic malformed-input mutations.
  Bootstrap, governance, formatting, Clippy, required performance/reduction/
  parallelism/comparison/agent gates, resources, parallel-runtime, incremental,
  work and sanitizer gates pass. The existing memory-plan API gate is preserved.
- The observer adds inline-type visits and complete aggregate-emission calls, for
  44 total counters. At 125, 250, 500 and 1,000 four-type groups, it observes exactly
  5N inline-type visits and 4N complete definitions on emission, with no definitions
  on checking. Both commands stay within the permanent 16N+128 attempt budget.
  At 1,000 groups, check and emission make 125 and 384 allocation attempts.
  [352 geometric work rows](2026-09-06-m1-layout-work.tsv) retain ordinary versus
  sanitized output equality and repeated exact counts.
- The added [256-ordinal fault campaign](2026-09-06-m1-layout-faults.tsv) observes
  196 failures with status 71, exact attempted ordinals, and no partial C output;
  60 later ordinals succeed. Four failures (63–66) occur while inline traversal
  is incomplete, before function checking. Three failures (194–196) occur after
  aggregate emission has begun; those observations do not claim that all three
  allocations occur inside aggregate definitions rather than subsequent output.
  Older identity/map/ownership fault campaigns and sanitizers remain intact.

[Paired uninstrumented timings](2026-09-06-m1-layout-order.tsv) use eleven pairs
after two warmups, alternating compiler order. The layout timing fixture defines
dependencies first so both baseline and candidate C are valid and byte-identical;
the work/ordering regressions separately use forward definitions. At 1,000 groups
(4,000 aggregates), checking is 8.625 ms versus 8.793 ms, emission 18.818 ms versus
19.336 ms. At 4,000 groups, ratios are 0.985 and 1.008. The no-aggregate function
family has check/emission ratios 1.015/1.008 at 1,000 and 0.991/1.003 at 4,000.
These same-host observations do not establish a portable speed improvement.

The order adds one 40-byte vector descriptor to the checked view and project
preparation record on this host, plus 8 bytes per aggregate before capacity slack.
A source with no data declarations allocates no order-buffer payload. This compiler storage cost
is explicit; there is no generated-program runtime cost. RFC-0128 retains Compile
0, Analysis +1 and Dogfood +1, score 10. Detailed logs and the measurement recipe
are under `build/slim-next-m1/layout-order/`.

M1 remains **in progress**. The aggregate ordering blocker is repaired; the
minimum-I64 literal emission issue remains open. The derived function flow and
ownership foundation, typed persistent semantic identities, retained queries with
complete dependencies, transactional publication, cached analysis/C/backend work,
and full M1 differential/locality/release gates remain required. Retaining the
layout completion vector is not claimed to complete those obligations.

## M1 bounded function flow — 2026-09-06

RFC-0129 adds an optional, canonical-derived per-function flow view in production
SLIM. Blocks carry revision-owned declaration/node identities; edges retain source
evaluation order, branch alternatives/joins, binding and payload scope boundaries,
argument modes, and terminal recurrence. Recur stages arguments before one parameter
transition and has no ordinary fallthrough. Calls preserve an explicit unknown
outcome edge; the view neither certifies hazards nor discharges effects. Complete
means exact structural normal topology, not feasible paths or proved termination.
The normal checker remains the sole producer of semantic/ownership facts. Default
checking does not construct unused graphs.

Construction uses an explicit task stack with independent checked caps on tasks
processed, pending tasks, blocks and edges. The supported budget is 1..1,000,000.
Partial graphs report Bounded and cannot resolve block handles as complete. Invalid
budgets, extents and checked inputs have distinct statuses. Temporary task indices
are local; public block/node identities retain their declaration and revision.
Recur marks iteration exit and parameter transition: future ownership migration
must carry the checked transition, not reset availability at its back-edge.

Validation and observations:

- All 93 accepted conformance/native files produce deterministic valid views.
  Independent expected paths cover eager Boolean arguments, lexical scopes,
  branches and recurrence. Stale revisions/declarations, negative/out-of-range
  blocks, shortened extents, missing facts, and three actual rejected checker
  results are rejected. Exact required budgets complete; one below is bounded;
  all smaller budgets are tested for small graphs.
- The permanent ordinary native test covers binding chains from 125 through 4,000.
  [Geometric observed work](2026-09-06-m1-flow-work.tsv) is exactly 3N+4 blocks,
  3N+2 edges and 2N+1 processed tasks. A separate measurement-only native observer
  counts actual derive/walk entries and loop headers. Successful observations
  satisfy headers = reported steps + walk entries and repeat identically. Its
  checked counter cap is 1,000,000,000; it is not installed in the compiler/runtime.
- The standard corpus passes ordinary versus ASan/UBSan output equality and
  repeated counter equality. The [512-ordinal fault campaign](2026-09-06-m1-flow-faults.tsv)
  observes 71 failures with status 71 and empty stdout, 34 after task walking
  begins, and 441 successful later ordinals. Ordinary/sanitized status, stdout
  and stderr agree. The older 128-ordinal source-index campaign still observes
  109 failures and remains intact.
- The 4,000-binding ASan/UBSan O1 probe exceeds the host's default 8,176 KiB stack
  in the preceding recursive `check.find_unknown_expr`, before flow construction.
  A separately built fa5c0c7 sanitized compiler reproduces that failure. The
  dated 4,000-binding observer row therefore records a diagnostic per-process
  stack of 65,520 KiB; smaller rows use the default. The ordinary 4,000-binding
  test passes. This is a preexisting checker limit, not a claim that the entire
  checking path is iterative. No production limit or existing gate was relaxed.
- All 193 rejected fixtures retain exact status/stdout/stderr. All 20 native
  applications retain byte-identical generated C and complete analysis. Bootstrap
  reaches **3,687,073 C bytes**, SHA-256
  `86eadc2e41200ad66cd64f5497bb81c706e7ecaa248b6d4706077aa4b58e8a46`,
  73,626 bytes above fa5c0c7. All 10 unit and 65 integration tests pass, along with
  332 conformance fixtures and 2,000 deterministic malformed-input mutations.
  Governance, formatting, Clippy, required performance/reduction/parallelism/
  comparison/agent gates, resources, parallel-runtime, incremental and work gates
  pass. The existing 44-counter ordinary work campaign remains unchanged.

[Paired uninstrumented checking timings](2026-09-06-m1-flow-default-check.tsv)
compare fa5c0c7 with this seed after two warmup pairs and eleven alternating pairs
per size. At 1,000 bindings, median times are 7.137 ms baseline and 6.941 ms
candidate; at 4,000 they are 18.162 ms and 18.281 ms. These same-host observations
measure ordinary checking, which does not build a flow graph, and do not establish
a portable speed improvement.

On this host, Block, Edge, Task and Graph occupy 56, 88, 32 and 152 bytes. Occupied
block/edge payload is 432,400 bytes at 1,000 bindings and 1,728,400 at 4,000. Those
figures exclude capacity slack, task storage, source/facts and allocator overhead;
they are not peak RSS. Physical scratch reclamation remains governed by existing
caller regions. There is no generated-program runtime cost. RFC-0129 retains
Analysis +2 and other dimensions zero, score 15. Detailed logs and measurement
recipes are under `build/slim-next-m1/flow/`.

M1 remains **in progress**. This checkpoint implements the bounded structural view;
ownership orchestration over it, typed persistent semantic identities, retained
queries with complete dependencies, transactional publication, cached analysis/C/
backend work, and full differential/locality/release gates remain required. The
minimum-I64 literal C emission issue and recursive-checker stack limitation remain
recorded. Neither the optional graph nor its measurements constitute incremental
semantic reuse.

## M1 retained function typing — 2026-09-06

RFC-0130 implements actual successful function-inference reuse in production SLIM.
`retained.slim` stores revision-owned node/type/binding references, source bytes,
pre-inference name resolution (including missing results), and canonical shape.
An indexed full-key match and reverse interface dependency adjacency distinguish
body changes from interface changes. Explicit complete-declaration or interface-
prefix maps translate fact forms, source links and packed local links. Every
imported reference is checked before any function result is installed.

`check_source_retained` shares the ordinary validation, termination and memory
path. Inference misses call `typing.check_function`; only complete successful
checks can publish eligible history. Failed candidates, stale revisions and missing
fact/name-index metadata cannot replace it. Source parsing, linking, global
checks and C generation still run. The public `session` command remains estimate-
only; this internal operation is not a second checker or a completed M1 service.

Validation and observations:

- All 94 accepted conformance/native files skip function inference on unchanged
  updates and match fresh checking for every fact, token field, diagnostic and
  generated C byte. A measurement-only native observer counts actual isolated
  checker calls within each retained-check invocation; it has 64 report phases
  and a 1,000,000,000 counter cap. Observations repeat identically.
- Permanent edits cover body/interface/layout/effect/ownership-mode changes,
  transitive aggregate copyability, insertion, deletion, renaming, reordering,
  comments, duplicate rejection and module identity. A callee body edit that
  introduces unproved recursion still reaches the ordinary termination rejection,
  including when callers' typing is reused. No body-derived analysis is cached.
- Stale revisions execute and cannot publish. An epoch change executes. Recovery
  from rejected source reuses the unchanged good history. Missing fact vectors
  or name-index roots force clean inference. The API takes a token limit in
  1..1,000,000: the exact current token count permits reuse; one below, zero and
  an out-of-range limit explicitly decline storage and perform ordinary checking.
  The token limit is not a source-byte or peak-process-memory bound.
- [Geometric native observations](2026-09-06-m1-retained-typing-work.tsv) cover
  N=125 through 4,000 helper functions, plus main. Unchanged updates execute zero
  function checks and reuse N+1 functions; one body edit executes exactly one and
  reuses N. The family imports 21N+18 and 21N-3 canonical nodes respectively.
  Each sanitizer observation repeats with exact clean fact/token/diagnostic/C
  equality. The smaller geometric sizes are permanent integration regressions.
- The ordinary/ASan/UBSan campaign passes all 94 accepted files, stale/recovery/
  capacity/metadata boundaries, and [512 allocation-fault ordinals](2026-09-06-m1-retained-typing-faults.tsv).
  It observes 509 status-71 failures with empty stdout and 3 successful later
  ordinals. Status/stdout/stderr agree. Phase returns describe observed control,
  not successful publication; this whole comparison probe includes checking and
  C comparison outside the retained query itself.
- All 193 rejected fixtures retain exact status/stdout/stderr. All 20 native
  applications retain complete byte-identical analysis and unchanged resource
  rows. [Every generated-C row](2026-09-06-m1-retained-typing-native.tsv) is recorded:
  19 are byte-identical; `variants` grows from 5,614 to 5,666 bytes solely through
  two unused-binding suppressions described below.
- Bootstrap reaches **3,845,929 C bytes**, SHA-256
  `5f232902750c04e1a58d76b399ad3030a551f7f947167de5271cf4c1f4c64c2a`,
  158,856 bytes above 1b6b7e4. All 10 unit and 66 integration tests pass,
  with 333 conformance fixtures and 2,000 deterministic malformed mutations.
  Governance, formatting, Clippy, required performance/reduction/parallelism/
  comparison/agent gates, resources, parallel-runtime, incremental and the
  existing 44-counter ordinary work campaign pass. No gate is relaxed.

The work also repairs an existing backend defect: unused enum payload bindings
lacked the unused-variable suppression already emitted for ordinary bindings.
`(void)binding;` now permits such checked source to compile under strict C flags.
A permanent native conformance fixture covers it. Payload evaluation, layout and
runtime behavior are unchanged. These intentional C text additions are separate
from clean-versus-retained equivalence, which remains byte-exact.

[Paired uninstrumented timings](2026-09-06-m1-retained-typing-latency.tsv) use two
warmup pairs and eleven alternating pairs per size. At 1,000 helpers, ordinary
checking has medians 6.881 ms baseline and
6.991 ms candidate; at 4,000 the medians are
17.540 and 17.219 ms.
There is no portable default-check speedup claim. The separate two-revision
operation deliberately compares two clean checks with construction of a retained
snapshot plus one update, including process startup, I/O, parsing, linking and all
checking. At 4,000 helpers, unchanged-input medians are
31.293 versus 60.643 ms;
one-body-edit medians are 30.842 versus
59.423 ms. These are not isolated warm-query
latencies. The additional snapshot construction and data-management cost outweighs
the inference savings in this workload. This internal capability is not presented
as a latency improvement or as the completed public incremental compiler.

On this host, TypeId, BindingId, Link, Saved, Shape, Declaration, Index and Cache
occupy 48, 40, 104, 264, 24, 112, 248 and 296 bytes. Saved payload alone occupies
5,549,808 bytes for the 1,000-helper family and 22,181,808 for 4,000, before vector
capacity slack, indices, copied body/interface bytes and allocator overhead. These
are not peak RSS measurements. Original input bytes are retained; the default
compiler does not allocate these optional snapshot payloads. Compile remains 0,
Analysis +2, score 15: actual reuse is established, not faster overall feedback.
Detailed logs and measurement recipes are under `build/slim-next-m1/retained-typing/`.

M1 remains **in progress**. Reducing retained storage/copying overhead and integrating
real queries into the public session remain necessary, alongside ownership
orchestration over flow, typed place facts, full transactional service limits and
reclamation, cached analysis/C/backend work, and final differential/locality/release
closure. The minimum-I64 literal emission issue and recursive-checker sanitizer
stack limit remain recorded. This checkpoint does not shrink those obligations.
