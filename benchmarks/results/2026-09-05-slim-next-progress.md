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
| M1: compiler substrate | in progress | Checkpoint `49ea4a0` validates internal retained parsing, typing, memory plans, range queries and C function fragments with transactional recovery. Typed identities are implemented; RFC-0141 adds validated shared control descriptions and continuation-based ownership orchestration. Public host-bound sessions, retained global analyses, native backend caching, the recorded emission repair and full closure remain. |
| M2: expressive safe core | pending | Successor ownership, borrowing, allocation, and generics not implemented. |
| M3: agent and debugger interface | pending | Semantic service and source debugger not implemented. |
| M4: component laboratory | pending | Deterministic providers and replay not implemented. |
| M5: systems contracts | pending | Resource capabilities and successor resource contracts not implemented. |
| M6: kernel substrate | pending | No freestanding kernel claim. |
| M7: OS research slice | pending | No end-to-end OS or productivity claim. |

## Current M1 closure ledger

This ledger includes the validated RFC-0141 fixed point
`59d895508fa9011dea2d833738e4ea6a7da72829412130de48d790685b001706`
on top of checkpoint `49ea4a0`; dated sections below preserve historical costs
and checkpoint-specific results. An implemented internal query
is not evidence that the public session already provides that query.

| Obligation | Current result | Remaining closure work |
| --- | --- | --- |
| Revision-owned source, declaration, node and span identities | RFC-0125/0126 implement checked ownership, exact revision maps and source relocation; RFC-0134 adds retained typed place identities. | Preserve these contracts through the public session lifecycle and all configuration changes. |
| Branch/recurrence ownership foundation | RFC-0129/0141 share canonical form and lexical-scope descriptions between the optional graph and explicit checking continuations. The normal checker remains the sole producer of move, availability and loan decisions; all expression families are migrated. | Preserve the validated transitions and source-relative bounds through public session integration. The separate recursive name-resolution prepass retains its documented sanitizer depth limit. |
| Reusable declaration queries | RFC-0130/0131/0132/0135 retain typing and parsing with validated dependencies and current source links. | Exercise the same production queries through real public updates and preserve locality through the remaining analysis integration. |
| Analysis dependencies | RFC-0137/0138 retain memory plans and range queries. Complete current global scans and parallel analysis still execute. | Retain eligible global results with all body/call-site, ordering and budget dependencies; observe real producers and imports separately. |
| Deterministic C emission | RFC-0139 stabilizes private identities; RFC-0140 retains prototypes, bodies and wrappers. The 96 accepted-fixture differential is raw-C exact. | Keep output assembly/copying and external backend work visible when exposed publicly; repair the baseline literal-Bytes storage-address emission defect reproduced during RFC-0141 validation. |
| Transactional session and corruption recovery | RFC-0133 implements bounded internal epochs, admitted usage, last-good snapshots and failed-update recovery; RFC-0140 tests malformed fragment metadata and damaged bytes. | Bind compiler/runtime/target/options to the actual host, implement public transport and owning-epoch cleanup/reset, and retain explicit last-good revision identities. |
| Native artifact cache | Existing public cache retains whole generated C; the new internal session does not retain native backend artifacts. | Validate complete generated C, runtime, backend, target, flags and link inputs; publish native results transactionally and treat invalid artifacts as misses. |
| Differential, locality and release evidence | Every completed child has measured domains and required checkpoint gates. RFC-0140 passes the full session sanitizer campaign, 6,144 fault ordinals and unchanged durable budgets. | Run the integrated public-service matrix, geometric locality/latency and full `scripts/verify-0.9.sh` release closure on the final identified checkpoint. |

The public `session` command still reports invalidation estimates. The complete
M1 goal remains active; none of the remaining rows is waived by the C-fragment
checkpoint. RFC-0141 removes recursive expression inference; the remaining
sanitizer stack limit in `check.find_unknown_expr` is recorded separately.

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


## M1 compact retained storage — 2026-09-06

RFC-0131 replaces repeated expanded identities in immutable saved rows with one
typed declaration owner and nominal StoredType/StoredLink words. A word is decoded
only against its original source index, then materialized as a typed semantic
handle for the existing checked revision map. Owner equality includes epoch,
revision, file and declaration slot. Source-node decoding checks owner-table and
view bounds; invalid tags/links cause misses. The packed-link decoder rejects
out-of-domain magnitudes before negation, including I64 minimum. It distinguishes
invalid references from genuine missing references. All rows are validated before
the first imported fact or token write. The sole normal checker remains the
producer of new accepted semantic facts.

[Fixed storage measurements](2026-09-06-m1-compact-retained-storage.tsv) show Saved
shrinking from 264 to **64 bytes** and transient pre-inference links from 104 to
**eight bytes**. These ceilings are permanent native-probe compilation gates.
Saved occupied payload falls from 22,181,808 to 5,377,408 bytes for the 4,000-helper
family (84,022 canonical nodes). It excludes vector capacity slack, original input,
copied declaration/interface bytes, index storage and allocator overhead. It is
not peak RSS or a service storage bound. TypeId/BindingId/Link remain 48/40/104
bytes at semantic boundaries; Index and Cache remain 248 and 296 bytes.

Validation and observations:

- The production retained probe passes all 94 accepted conformance/native files,
  with exact clean facts, token fields, diagnostics and C. Unchanged updates
  execute no function-inference calls. Existing edit/dependency/recovery gates
  remain intact. Added boundary trials cover wrong revision and declaration
  owners, missing/excess type positions, extreme packed/source links, invalid
  fact tags and all-before-write fallback when a final row is invalid.
- [Geometric actual work](2026-09-06-m1-compact-retained-work.tsv) still observes
  zero updated checks and N+1 reused functions on unchanged input, or one check
  and N reused functions on a helper-body edit. Imported counts remain 21N+18
  and 21N-3. N=125 through 4,000, plus small boundary sizes, repeat identically
  with ASan/UBSan and complete clean-result comparison. No metric is redefined.
- The ordinary and sanitized probes pass all storage/revision/capacity boundaries
  and [512 allocation-fault ordinals](2026-09-06-m1-compact-retained-faults.tsv):
  509 status-71 failures with empty stdout, three successes, and identical
  status/stdout/stderr. Native phase returns are observations of control flow,
  not publication certificates. The corpus includes clean checking and C
  comparison outside the retained operation.
- All 193 rejected fixtures keep exact status/stdout/stderr relative to 4dfd31d.
  All 20 native applications keep complete identical analysis, unchanged resource
  rows and [byte-identical generated C](2026-09-06-m1-compact-retained-native.tsv).
- Bootstrap reaches **3,861,132 C bytes**, 15,203 above 4dfd31d, SHA-256
  `e8accfbc15ab9436213e8648ba31b434c2124bdc87a8578a5572f98d36c1caff`.
  All 10 unit and 66 integration tests pass. Conformance passes 333 fixtures and
  2,000 deterministic malformed mutations. Governance, formatting, Clippy,
  required performance/reduction/parallelism/comparison/agent gates, resources,
  parallel-runtime, incremental and the existing 44-counter ordinary work
  campaign pass. No existing gate is relaxed.

[Paired uninstrumented timings](2026-09-06-m1-compact-retained-latency.tsv) use
O2 binaries, two warmup pairs and eleven alternating measured pairs after other
CPU-intensive checks finish. At 4,000 helpers, old/new retained medians are
59.125/49.764 ms for unchanged input and 59.907/49.782 ms for a body edit.
These improvements accompany substantially smaller stored payload. Separately,
two clean checks versus compact retained construction plus update take
31.419/50.190 ms unchanged and 30.539/49.226 ms with a body edit. The retained
operation is still slower overall for this family. All these totals include
startup, I/O, parsing, linking and complete checking; they are not isolated warm
query timings. Default-check medians are 17.433/17.361 ms at 4,000; no portable
ordinary-check speedup is claimed. Compile +1, other dimensions zero, score 10,
records the measured retained-storage improvement. Detailed logs and recipes are
under `build/slim-next-m1/compact-retained/`.

M1 remains **in progress**. This checkpoint reduces storage overhead; it does not
complete ownership orchestration over flow, typed place facts, retained parsing,
public transactional query/session integration and reclamation, cached analysis/C/
backend work, or the full differential/locality/release closure. The recorded
minimum-I64 literal emission bug and recursive-checker sanitizer stack limit
remain open. The complete M1 goal and historical evidence are unchanged.


## M1 retained project preparation and complete source keys — 2026-09-07

RFC-0132 connects project preparation to actual retained inference. One shared
loader/validator/flattener supplies both ordinary PreparedProject and retained
ProjectAttempt. Current manifest version/order, module identity, entry, cycle,
import/export visibility and source diagnostics are checked before retained typing
can run. An attempt includes the full prepared result, eligible immutable typing
history and independent inference-work counters. Failed preparation cannot replace
the caller's previous good history. The public session command is still the
historical estimate operation; this entry point is its production prerequisite.

The project differential discovered and fixed a source-key defect in the earlier
retained implementation. A synthetic closing canonical node can end at the callee
anchor, before its arguments. The stored main body therefore ended at
`data_helper` in a nested `data_helper(read(data_Box(value: 1)))` expression.
Changing its argument to `false` could reuse old typing even though a clean check
rejected it. Both retained typing and source snapshot maps now derive the complete
declaration extent up to the next declaration or module-source end, excluding
trailing separator whitespace. They do not derive an enclosing source extent from
a synthetic closing node. Parser tokens and diagnostic spans are unchanged.

Permanent regressions cover changed literals, strings, operators, nested calls
and members, including rejected updated source. Declaration insertion, deletion
and reordering keep the established inference-reuse counts; separator whitespace
is not mistaken for changed function content. This newly tested domain strengthens
the earlier evidence rather than retroactively claiming those mutations had been
covered by RFC-0130's initial corpus.

Validation and observations:

- The production project probe passes all 94 accepted conformance/native sources
  wrapped in manifests. It compares complete source/manifest bytes, every token
  field, original-module origins, facts, layout order, issues, every memory-plan
  field and byte-identical emitted C with clean preparation. Unchanged project
  updates execute zero function checks.
- Multi-module edits cover body, signature, aggregate layout, effect and ownership
  modes; valid module/declaration insertion, deletion and renaming; declaration
  reordering; forward aggregate layout; path relocation; comments and CRLF.
  Removed imports/exports, module identity mismatch, wrong entry, cycles,
  unsorted manifests, wrong versions, missing modules, malformed source,
  duplicates and semantic rejection keep ordinary status and complete diagnostic
  output. Reusing unchanged good history after each rejected update executes zero
  checks and restores exact clean results. Stale and zero-capacity attempts
  execute ordinary checking and cannot publish eligible history.
- [Geometric native work](2026-09-07-m1-retained-project-work.tsv) uses two modules
  and N helpers plus main, through N=4,000. Unchanged updates execute zero checks,
  reuse N+1 functions and import 15N+22 nodes. A helper-body edit executes one
  check, reuses N functions and imports 15N+7 nodes. Each observation repeats
  identically under ASan/UBSan with full prepared-result/C comparison. The smaller
  sizes and imported-node formulas are permanent integration gates.
- The project sanitizer campaign passes all 94 wrapped files, stale/capacity and
  rejected-then-recovered boundaries, plus [2,048 allocation-fault ordinals](2026-09-07-m1-retained-project-faults.tsv).
  It observes 693 status-71 failures with empty stdout and 1,355 later successes;
  ordinary/sanitized status, stdout and stderr agree. These phases count actual
  checker calls within retained project preparation, including zero calls for
  preparation failures. Phase returns do not certify publication. The earlier
  source-level campaign still passes 94 files and all 512 fault ordinals, with
  its 509 failures and three successes unchanged.
- All 193 rejected single-file fixtures keep exact status/stdout/stderr relative
  to 53cb428. All 20 native applications retain complete identical analysis,
  unchanged resource rows and [byte-identical generated C](2026-09-07-m1-retained-project-native.tsv).
- Bootstrap reaches **3,873,212 C bytes**, 12,080 above 53cb428, SHA-256
  `252560991d434a503c9e1c60a016f5fdefec6cf7633d3672cc9d817159c766de`.
  All 10 unit and 67 integration tests pass, including strengthened targeted
  content/module tests. Conformance passes 333 fixtures and 2,000 deterministic
  malformed mutations. Governance, formatting, Clippy, required performance/
  reduction/parallelism/comparison/agent gates, resources, parallel-runtime,
  incremental and the existing 44-counter ordinary work campaign pass. The
  governed project preparation hook and every existing budget remain intact.

[Paired uninstrumented timings](2026-09-07-m1-retained-project-latency.tsv) use O2,
two warmup pairs and eleven alternating pairs after other CPU-intensive checks.
At 4,000 helpers the default project-check medians are 20.940 ms baseline and
20.705 ms candidate; no portable default speedup is claimed. Two clean project
preparations versus retained construction plus update take 38.638/54.731 ms
unchanged and 39.248/54.664 ms for a helper-body edit. These process totals include
I/O, module parsing/validation, flattening/reparse and all checking, and exclude C
emission. They are not isolated warm-query timings; retention remains slower for
this workload. Analysis +2, other dimensions zero, score 15, records actual project
inference reuse rather than an overall feedback improvement. Detailed logs and
recipes are under `build/slim-next-m1/retained-project/`. On this host,
PreparedProject occupies 320 bytes and ProjectAttempt 648 bytes before their
referenced vector/source payloads. These fixed record sizes are not peak RSS or
service storage limits; the existing 64-byte saved-row budget is unchanged.

M1 remains **in progress**. Public transactional session integration, original
input/parse retention, storage lifecycle and reclamation, cached analysis/C/backend
work, flow-based ownership orchestration, typed place facts and complete M1
locality/differential/release closure remain required. The known minimum-I64 literal
emission bug and recursive-checker sanitizer stack limitation remain open. This
checkpoint does not replace any of those requirements.


## M1 transactional project snapshots — 2026-09-07

RFC-0133 adds the internal owner of retained project history. Captured ProjectInput
contains original manifest bytes, checked manifest metadata and ordered module
buffers with read outcomes. Preparation consumes those exact buffers through the
same module validation and checking operations as ordinary preparation. It never
reopens paths after choosing an input key. Ordinary preparation preserves its
interleaved read/parse order and existing diagnostics. Capturing all module reads
first is limited to the new session path; read failures are subsequently reported
in source-module order.

Session State owns attempted revisions, cumulative admission accounting and a
last-good Snapshot. Configuration uses four separate bounded compiler/runtime/
target/options fingerprints. Each admitted update receives a fresh attempted
revision even on rejection; publication selects a complete checked candidate only
after generation and payload admission succeed. An exact raw-input/configuration
hit retains the previous published revision, all prepared fields and C. It does
not reinterpret old typed nodes under a new revision. Changed raw input with
identical checked canonical source can reuse whole C after current validation.
Missing structural metadata forces cold preparation. Missing or checksum-damaged
optional C regenerates from checked history without source rechecking.

This is a compiler-owned in-process core, not a serialized semantic cache. The
host must bind configuration fingerprints to the actual toolchain in the future
public transport. The existing public estimate wrappers are unchanged. No backend
was executed by this core and no avoided-backend claim is made. The internal
optional C checksum is an integrity check within trusted compiler ownership, not
authentication for imported executable content.

Admission limits are 1..64 attempts, 1..67,108,864 input bytes, 1..1,000,000
canonical nodes and 1..67,108,864 C bytes per epoch. Usage is cumulative; checked
remaining capacity prevents arithmetic overflow and failed work cannot reset the
budget. Strict preparation declines an over-budget canonical input before function
checking, instead of taking the earlier optional-cache fallback. Node/C exhaustion
preserves good history and still permits unchanged reuse if input/attempt capacity
remains. Source reads precede admission and transient parser/checker/emitter
allocation follows existing contracts: these are not RSS limits.

The language remains unchanged. A one-element checked vector owns usage counters;
an optional one-element vector owns generated C and its checksum. Scalar values
are not passed to affine mem.replace. The session epoch probe returns an I64 and
has no exclusive output parameters, so its existing generated function region is
destroyed on return. Native observation checks that the child allocation list is
empty and the parent's list is unchanged. A second epoch starts only after that
return, using identity.reset. An API user retaining State outside such an owner
has not established the future public service lifecycle contract.

Validation and observations:

- Complete clean-versus-session comparison covers source and manifest bytes,
  tokens and original diagnostic origins, typing facts, layout order, issues,
  every memory-plan field and generated C. All 94 accepted conformance/native
  fixtures pass unchanged updates with zero program parses, function checks and
  C generations. Input capture/framing, full-byte comparison and optional-C
  integrity scans remain real work outside those query counts.
- Permanent edit cases cover arguments and helper bodies, interfaces, declared
  and actual effects, copyability/layout changes, exclusive/owned parameter
  changes, missing moves, insertion/deletion/renaming/reordering, module namespace
  changes, relocation, comments/CRLF and recurrence changing body-derived facts.
  Rejected project inputs preserve normal status and exact diagnostic output,
  including malformed source before a later read failure. Recovery retains the
  same last-good revision and exact clean output.
- Tests independently change all four configuration fields, remove several
  metadata vectors, remove/damage C, cross input/node/C/attempt capacities, reject
  invalid limits/configuration and exhausted revision IDs, issue exactly 64
  successful attempts and decline the next, and destroy two complete epochs.
  A native test replaces a file after capture but before preparation; the checked
  snapshot still matches the captured bytes and the untouched reference project.
- [Observed geometric work](2026-09-07-m1-session-work.tsv) repeats identically
  through 4,000 helpers in two modules. Cold preparation parses three program
  representations, checks N+1 functions and generates C once. Unchanged update
  does none of these operations, reuses N+1 functions and imports zero nodes.
  One helper-body edit still parses three representations, checks one function,
  imports 15N+7 nodes and regenerates whole C. These remaining parses and whole-C
  generation are explicit M1 locality work, not complete granular retention.
- The final [sanitized matrix](2026-09-07-m1-session-matrix.tsv) and all
  [2,048 allocation-fault ordinals](2026-09-07-m1-session-faults.tsv) pass. There
  are 426 status-71 failures with empty stdout and 1,622 successes; ordinary and
  ASan/UBSan status/stdout/stderr agree. Observed update returns are not claims of
  successful publication. The earlier retained project campaign still passes
  94 files and 2,048 ordinals (693 failures/1,355 successes); the retained source
  campaign still passes 94 files and 512 ordinals (509 failures/three successes).
- Relative to afc0482, all 193 rejected source fixtures retain exact status and
  diagnostics. All 20 native applications preserve complete analysis, resource
  rows and [byte-identical C](2026-09-07-m1-session-native.tsv).
- The final bootstrap fixed point is **3,985,480 C bytes**, 112,268 above afc0482,
  SHA-256 `aaf9a4305bc3516b8a6cbe367a7eb057584fdf2ae8ccb050e4e88304ef64e1d8`.
  All 10 unit and 68 integration tests pass. Conformance passes 333 fixtures and
  2,000 malformed mutations. Governance, formatting, Clippy, required performance/
  reduction/parallelism/comparison/agent checks, resources, parallel-runtime,
  incremental and the existing ordinary-work gates pass without a relaxed budget.

[Paired O2 timings](2026-09-07-m1-session-latency.tsv) use two warmup pairs and
11 alternating measured pairs after other CPU-intensive checks. At 4,000 helpers,
ordinary project-check medians are 20.986 ms for afc0482 and 20.850 ms for this
checkpoint. Two clean preparations with C generation take 65.847 ms versus
43.816 ms for a cold session followed by unchanged reuse. A helper-body edit takes
66.979 ms clean versus 90.513 ms with retained session preparation. These are
whole-process totals, including startup, captured input, checking, C generation
and integrity work; they are not isolated warm-query timings or portable budgets.
Whole unchanged reuse improves this measured sequence, while changed preparation
remains slower and requires the remaining granular M1 work. Compile remains zero
in the RFC rating; Analysis +2 records the new verified session facts.

On this host the fixed records occupy 104 bytes for ProjectInput, 656 for
ProjectAttempt, 944 for State, 872 for Snapshot, 48 for Usage, 24 for Artifact and
112 for Report; PreparedProject remains 320 bytes. These sizes exclude referenced
buffers. The permanent 64-byte saved-row and eight-byte stored-link limits still
pass. Recipes and detailed logs are in `build/slim-next-m1/transactional-session/`.

M1 remains **in progress**. Public transactional transport and actual host identities,
service lifecycle integration, typed place facts and flow-based ownership
orchestration, retained declaration parsing/global analysis/C fragments/backend
work, and complete M1 differential/locality/release closure remain required. The
minimum-I64 literal emission bug and recursive-checker sanitizer stack limitation
remain open. This checkpoint preserves the full goal and does not complete M1.

## M1 checkpoint: retained typed place identities (2026-09-07)

RFC-0134 adds a nominal PlaceId and bounded read-only queries over existing checked
canonical nodes, binding links and types. The flow adapter accepts matching
function/revision owners and exposes applicable operation facts. Exact results
identify the lexical root occurrence, binding declaration and nominal binding,
root/value types, checked borrow mode and projection depth. They do not establish
allocation identity, alias disjointness, availability or permission to mutate or
transfer. Computed bases remain unknown. Old handles are rejected; cross-revision
reuse requires an independently established exact canonical node map.

No retained row, allocation or ordinary compilation pass is added. Saved remains
at most 64 bytes and StoredLink at most eight bytes. Each point query has constant
auxiliary storage and an explicit 1..1,000,000 node budget. A future batch ownership
consumer must bound aggregate traversal or memoize repeated projection paths;
repeated independent queries over all nested projections can otherwise repeat work.

Validation and observations:

- The [sanitized matrix](2026-09-07-m1-places-matrix.tsv) covers all 94 accepted
  conformance/native sources, lexical shadowing, owned/shared/exclusive parameters,
  local aliases, enum bindings, nested projections, missing individual type and
  binding facts, malformed metadata, invalid identities and flow owners. A normal
  compiler diagnostic rejects passing NodeId where PlaceId is required.
- Current node facts compare exactly against fresh checked caches after unchanged,
  inserted, relocated, CRLF, renamed, body, layout and ownership-signature inputs.
  Mapped old identities are separately checked through the existing exact node-map
  API. A zero mapping count does not imply missing current checked facts.
- [Observed native work](2026-09-07-m1-places-work.tsv) is depth + 1 for full
  projection queries through depth 256, and exactly one visited node before a
  one-step bound. Both repetitions and ordinary/ASan/UBSan outputs agree. These
  counts exclude preparation and constant terminal binding/type metadata lookups.
- All [2,048 allocation-fault ordinals](2026-09-07-m1-places-faults.tsv) pass:
  153 status-71 failures with empty stdout and 1,895 successes. Ordinary and
  sanitized status/stdout/stderr agree, including the final missing-facts tests.
- Relative to 07d721d, 193 rejected source fixtures retain exact status and
  diagnostics. All 20 native applications preserve complete analysis, resource
  rows and [byte-identical generated C](2026-09-07-m1-places-native.tsv).
- The bootstrap fixed point is **4,033,892 C bytes**, 48,412 above 07d721d, SHA-256
  `dddb3865357a149932bc4c2cd2b09e05e822c65cb16d0bee6413d0412a320b82`.
  All 10 unit and 69 integration tests pass; the final fixture expansion also
  passes its targeted integration test. Conformance passes 333 fixtures and 2,000
  malformed mutations. Governance, formatting, Clippy, required performance,
  reduction, parallelism, comparison and agent gates, plus resources,
  parallel-runtime, incremental and ordinary-work gates pass without relaxation.

[Paired O2 default-check timings](2026-09-07-m1-places-latency.tsv) use two warmup
pairs and 11 alternating measured pairs. At 4,000 helpers, medians are 20.383 ms
for 07d721d and 21.112 ms for this checkpoint (approximately 3.6% higher). These
are whole-process observations including startup and I/O, not isolated query
costs or portable budgets. Default compilation does not call the optional place
query; this observation is not a claim of improved compile performance. Recipes
and detailed logs are in `build/slim-next-m1/place-identities/`.

M1 remains **in progress**. Actual flow-based ownership orchestration and loan
state, public transactional transport with actual host identities and service
lifecycle, retained declaration parsing/global analysis/C fragments/backend work,
and complete differential/locality/release closure remain required. The known
minimum-I64 literal C emission bug and recursive-checker sanitizer stack limitation
remain open. This checkpoint completes RFC-0134, not the parent M1 goal.

## M1 checkpoint: retained declaration parsing (2026-09-07)

RFC-0135 is implemented in production SLIM. Original modules and the flattened
project now retain successful declaration parses under current typed FileId
owners. Reuse requires exact raw source, lexical context and the first unconsumed
lookahead; it imports only syntax tags and translated spans, resets links and
rebuilds current canonical boundaries. The ordinary lexer, header and declaration
grammar remain the sole authority for new syntax. Both histories publish only
with the complete checked snapshot. Failed edits preserve last-good history.

The portable fixed point is **4,142,232 C bytes**, SHA-256
`c0359c9f031c5182e2637a5681b9952de8c60273a5881a0155edc1a96be1eb6b`.
This adds 108,340 bytes (2.69%) over the 4,033,892-byte RFC-0134 seed at 29ce920.
The runtime ABI and ordinary language acceptance are unchanged. Detailed recipes
and logs are under `build/slim-next-m1/retained-parsing/`.

[Complete parser comparisons](2026-09-07-m1-parsing-matrix.tsv) cover all token
fields and diagnostic fields in 1,695 cases derived from 339 source inputs, plus
2,000 deterministic malformed edits. Native accessor observation checks every
successful item parse stays within entry..returned-next, including successful
items before malformed source. Current output prefixes differ from previous
prefixes, and stale epochs/configurations or missing optional vectors miss.
Permanent capacity cases cross 64 MiB source, 1,000,000 canonical tokens,
1,000,000 lexemes and 4,096 configuration bytes. The canonical-token over-case
contains 1,000,003 tokens. A lexeme over-case reports zero retained lexemes because
retention was declined. These huge fixtures prove parser equality, not type or
ownership acceptance. Aggregate module history token/lexeme budgets are separately
crossed at the required size and one below, with current checked acceptance and
ordinary/ASan/UBSan agreement.

[Observed geometric parser work](2026-09-07-m1-parsing-work.tsv) through 4,000
helpers is zero declaration grammar calls for unchanged source and one for a body
edit; at 4,000 helpers these cases import 44,018 and 44,007 canonical nodes.
Both repetitions agree. [Parser fault injection](2026-09-07-m1-parsing-faults.tsv)
covers 2,048 ordinals: 94 status-71 failures and 1,954 successes with exact
ordinary/sanitized status, stdout and stderr equality.

[The integrated session matrix](2026-09-07-m1-parsing-session-matrix.tsv) compares
all prepared fields and generated C over 94 accepted fixtures and the edit,
recovery, configuration and metadata-withdrawal matrix. Current original-module
cache owners and all cached syntax tokens are compared with fresh parses.
Manifest ordering retains its existing E0406 rejection; sorted module insertion,
deletion/reinsertion and declaration reordering establish current owners.
[All 2,048 session fault ordinals](2026-09-07-m1-parsing-session-faults.tsv) agree
under ordinary and ASan/UBSan execution: 471 status-71 failures and 1,577 successes.
Epoch teardown checks pass. The four final aggregate-history boundary cases ran
in an additional targeted ordinary/sanitized matrix on this same seed.

Native session schema 2 records program lexings, checked functions, C generations,
declaration grammar executions and parsed-node imports. For N helpers plus main
in two modules, cold preparation executes 2(N+1) declaration grammars. A body edit
executes **two declaration grammars and one function check**, independent of N in
the measured range through 4,000. It imports 30N+14 syntax nodes and 15N+7 checked
nodes. Missing one class of parse history executes N+2 grammars. Unchanged snapshot
reuse skips lexical, grammar, check and generation operations. Source capture,
matching, indexing, linear assembly and integrity work are not free; changed input
still runs three lexical passes, global analysis and whole-C generation.

[Paired default-project checks](2026-09-07-m1-parsing-latency.tsv), with two warmup
pairs and 11 alternating measured pairs, give medians of 20.721 ms at 29ce920 and
21.039 ms on this seed at 4,000 helpers. [Separate session measurements](2026-09-07-m1-parsing-session-latency.tsv)
use the same O2 host and sample counts. At 4,000 helpers, prior/current session
cold-plus-unchanged medians are 44.208/46.131 ms, and cold-plus-body-update medians
are 91.166/95.838 ms. Compared separately with two clean current preparations,
retained unchanged takes 47.148 versus 65.920 ms; retained body update takes
95.593 versus 64.716 ms. These are complete process workloads, not isolated warm
query latencies. Metadata costs currently exceed the saved grammar/inference work
for the two-revision body workload. No speedup or budget relaxation is claimed.

Measured fixed records are 120 bytes for ModuleParses, 264 for ParseCache, 32 for
ParsedDeclaration, 40 per Lexeme, 48 per Token, 1,064 for ParsedProject, 1,256 for
Snapshot and 1,328 for State. Referenced buffers are additional storage. Separate
aggregate module token/lexeme bounds constrain retained records, not peak RSS;
candidate buffers remain owned by their epoch. Saved-row/stored-link gates remain.

Bootstrap, governance, 10 unit and 70 integration tests, 333 conformance fixtures
and 2,000 malformed conformance mutations pass. Required quick performance,
reduction, parallelism, comparison and agent gates, plus resources, parallel-runtime,
incremental, ordinary-work, formatting and Clippy pass. [Native differential evidence](2026-09-07-m1-parsing-native.tsv)
preserves diagnostics for 193 rejected fixtures and full analysis/generated C for
20 native applications against 29ce920. A historical pre-integration quick
comparison failed prefix_sum at 2.450 versus its 2.000 budget despite identical C;
its full 15-sample comparison and subsequent quick check passed. Those logs remain
preserved. All current required gates pass without changing a budget.

M1 remains **in progress** with its original scope. Flow-based availability and
loan orchestration, public transport with actual host identity binding/lifecycle,
retained global analysis/C fragments/backend artifacts, complete locality and
differential evidence, and full release closure remain required. The minimum-I64
C literal emission bug and recursive-checker sanitizer stack limitation remain
open. This checkpoint completes RFC-0135, not the parent milestone.

## M1 checkpoint: portable decimal I64 literals (2026-09-07)

RFC-0136 repairs the minimum-I64 C emission bug recorded during source-identity
work. It also repairs decimal leading-zero lowering: the prior compiler copies
source digits into C, so `0010` becomes octal eight and `0009` fails compilation.
The production checker now rejects literals outside signed I64 with E0361; before
this change `9223372036854775808` passes SLIM checking and fails strict native C.
The RFC records this correction of erroneous checker acceptance explicitly.
Representable decimal source keeps its documented meaning and original spans.

The shared source-span helper skips optional sign/leading zeros without changing
canonical source. The normal checker uses digit count and at most 19 significant
digit comparisons against the signed bounds, never overflowing an accumulator.
Minimum I64 emits a parenthesized expression whose constants and intermediate
values are all representable. Ordinary non-minimum, unpadded literals retain their
C output. No runtime ABI, dependency, retained table, parsed representation or
source primitive is added. The range analyzer's proof domain is unchanged.

The fixed point is **4,153,396 C bytes**, SHA-256
`c3c8af2d0cc1c3164b068d1fbc1f0d651bdadfcb2eca94fd31915d53e29262a7`.
This is 11,164 bytes (0.27%) above ab7758c. Logs and timing recipes are under
`build/slim-next-m1/integer-emission/`.

[The literal matrix](2026-09-07-m1-integer-matrix.tsv) checks 456 spellings against
independent expected decimal values through strict C11 at O0/O2 and ASan/UBSan.
It includes signed boundaries, adjacent values, deterministic random values,
negative zero and 65,536 leading zeros. Sixteen out-of-domain cases, including
very long magnitudes, compare exact compiler diagnostics and public JSON spans.
[The 512-ordinal fault campaign](2026-09-07-m1-integer-faults.tsv) gives 116 failures
with status 71 and no partial C, plus 396 successes; ordinary and sanitized status,
stdout and stderr agree. These are named finite test domains, not exhaustive I64
execution. Range correctness additionally follows from the lexicographic bound
comparison on validated decimal digits and its fixed 19-digit comparison bound.

[The complete session matrix](2026-09-07-m1-integer-session.tsv) covers 95 accepted
fixtures and includes valid minimum/leading-zero body edits, out-of-range rejection,
and last-good recovery. All prepared fields and generated C match fresh compilation.
[Its 2,048 fault ordinals](2026-09-07-m1-integer-session-faults.tsv) retain the
471 failures / 1,577 successes with exact ordinary/ASan/UBSan agreement. Geometric
body updates still execute two declaration grammars and one function check through
4,000 helpers; this repair does not claim additional incremental reuse.

The previous 74 accepted conformance sources retain identical checking and C.
The 193 previous rejected fixtures preserve exact status and diagnostics.
[All 20 native applications](2026-09-07-m1-integer-native.tsv) retain byte-identical
C and complete analysis. Bootstrap, governance, 10 unit and 71 integration tests,
338 conformance fixtures and 2,000 deterministic malformed mutations pass.
Required performance/reduction/parallelism/comparison/agent checks, resources,
parallel-runtime, incremental, ordinary-work, formatting and Clippy pass. No gate
or performance budget is relaxed.

[Paired default checks](2026-09-07-m1-integer-latency.tsv) and
[paired C emission](2026-09-07-m1-integer-emission-latency.tsv) use two warmup pairs
and 11 alternating measured pairs against ab7758c, after other validation finishes.
At 4,000 helpers, baseline/current check medians are 20.178/20.239 ms; emission
medians are 37.877/37.574 ms. These complete process timings include I/O and are
not a speedup claim or a portable absolute budget.

M1 remains **in progress**. Actual flow-based availability/loan orchestration,
public session transport with actual host fingerprints/lifecycle, retained global
analysis and stable C fragments/backend artifacts, complete locality/differential
validation and full release closure are still required. The minimum-I64 emitter
blocker is resolved; the recursive-checker sanitizer stack limitation remains open.

## M1 checkpoint: retained memory plans (2026-09-07)

RFC-0137 is accepted and **implemented**. The production query now reuses the sole memory planner's function results after the
existing exact body/interface dependency checks. Plans carry typed declaration
owners and validated local token/byte positions. The 64-value boundary's legacy
byte-end versus token-end representation is explicit. Candidate publication moves
planning history with typing history, and missing plan metadata recomputes planning
without discarding otherwise reusable typing. The ordinary path still runs the
same planner. This is real retained planning, not estimated invalidation work.

The first implementation uses nested per-function saved vectors. Its reproducible
working seed is **4,219,674 C bytes**, SHA-256
`72097f2d90cad0b383bb231bd4b2a98bec89e01311e1baef2c6db7f88bd6fccb`.
This is 66,278 bytes (1.60%) above the validated literal-repair seed at 1fca486.
That prototype was withheld from commit pending storage/cost work and review.

[The nested-vector matrix](2026-09-07-m1-planning-nested-matrix.tsv) compares every
memory-plan and prepared-state field and complete C with fresh preparation. All
95 accepted corpus fixtures additionally force preparation after a trivia change
and import all function plans. Tests cover transitive storage-layout and explicit
ownership-mode changes, source/node relocation at 63/64/65 planned values, missing
entries/slot maps/value rows, invalid owner/count/source length/node/byte/tag data,
and history capacity at and below the required size. Independent function/value/
allocation/destruction budget checks preserve the ordinary plan on a history miss.
Native observation schema 3 counts actual plan construction and imports separately.
Through 4,000 helpers, a body edit builds one memory plan and imports 4,000; an
unchanged snapshot performs neither operation. Global scans and C emission remain.

[All 2,048 fault ordinals](2026-09-07-m1-planning-nested-faults.tsv) agree between
ordinary and ASan/UBSan execution: 476 status-71 failures and 1,572 successes.
Bootstrap, governance, 10 unit and 71 integration tests, 338 conformance fixtures,
2,000 malformed mutations, required benchmark gates, resources, parallel-runtime,
incremental, ordinary-work, formatting and Clippy pass. The 197 existing rejected
fixtures retain exact diagnostics. [All 20 native applications](2026-09-07-m1-planning-nested-native.tsv)
retain complete analysis and byte-identical C. The first full harness run stopped
at a stale schema-2 assertion before fault injection; the corrected schema-3 run
above completes. No production failure was hidden or performance budget relaxed.

Measured fixed sizes are 256 bytes per PlanEntry, 80 for PlanCache, 376 for Cache,
568 for Analyzed and 1,336 for Snapshot. Saved typing rows remain 64 bytes.
FunctionPlan is 152 bytes and each value row 56 bytes, excluding vector buffers.
The nested representation adds several saved vector allocations per function.

[Default check timings](2026-09-07-m1-planning-nested-latency.tsv) at 4,000 helpers
give 20.639/20.471 ms for baseline/candidate. [Session workload timings](2026-09-07-m1-planning-nested-session-latency.tsv)
give 46.170/49.266 ms for cold-plus-unchanged and 94.394/96.205 ms for
cold-plus-body-update. In the separate two-clean comparison, the retained body
workload takes 95.884 ms versus 64.458 ms. [Denser functions](2026-09-07-m1-planning-nested-dense-latency.tsv)
with 64 planned values each improve the prior/candidate cold-plus-body workload
from 359.729 to 328.184 ms at 512 helpers; two clean current preparations take
309.432 ms versus 328.489 ms retained in that separate comparison. All measurements
use O2, two warmup and 11 alternating pairs after other validation, and include
process startup, capture, checking, analysis and C generation. These are independent
observations, not a universal speedup score or a warm-query-only measurement.

The approximately 7% cold-plus-unchanged penalty blocked committing this prototype.
Its measurements are preserved as contrary cost evidence. The subsequent pooled
implementation below addresses that cost without relaxing a gate.

### Validated pooled planning

The candidate saves normalized value, allocation and destruction rows in three
snapshot-wide pools. A copyable function entry contains scalar facts, an exact
owner and three checked slice ranges. Complete contiguous framing is validated
once before reuse; each imported function additionally validates every local node,
byte end and last-use discriminant. Range validation subtracts before adding, so
extreme malformed counts and starts cannot overflow. Current working plans retain
the ordinary per-function vectors and sole producer. Independent record budgets
and transactional publication are unchanged.

The reproducible candidate seed is **4,233,117 C bytes**, SHA-256
`d4e6b04ffcaee43b6384c3055e3f516e0ee7b7a7910dd978ee0be85571dfff06`.
Saved PlanEntry size falls from 256 to 112 bytes; PlanCache is 200, Cache 496,
Analyzed 688 and Snapshot 1,456 bytes. These are fixed record sizes excluding
buffers. Saved typing rows remain 64 bytes, ordinary ValuePlan 56 and FunctionPlan
152. Pool storage eliminates the per-function saved vector allocations.

[The pooled matrix](2026-09-07-m1-planning-matrix.tsv) compares complete plans,
prepared state and C with fresh preparation for all 95 accepted corpus fixtures.
It covers node/byte relocation at 63/64/65 values, transitive layouts and ownership
modes, damaged pool metadata and optional capacity boundaries. Geometric scalar
projects through 4,000 helpers and dense projects through 512 helpers with 64
planned values observe one plan construction and all unaffected plan imports on
a body edit. No-change snapshots perform zero observed planning operations.

[Two bounded fault campaigns](2026-09-07-m1-planning-faults.tsv) each inject ordinals
1..2,048 and compare exact status/stdout/stderr under ordinary O1 and ASan/UBSan O1.
Unchanged source gives 475 status-71 failures and 1,573 successes. A two-module body
edit gives 948 failures and 1,100 successes; every successful update observes one
plan construction and one import. Completed-phase counts omit unfinished work.
The complete corpus/geometry sanitizer run and the added changed-project fault
campaign use the same compiled production probe. The host fault loop is shared by
the full verification command; a subsequent quick run validates its integration.
These are bounded tested domains, not proof about arbitrary programs or faults.

Bootstrap, governance, 10 unit and 71 integration tests, 338 conformance fixtures,
2,000 malformed mutations, required performance/reduction/parallelism/comparison/
agent checks, resources, parallel-runtime, incremental, ordinary-work, formatting
and Clippy pass. The 197 existing rejected fixtures retain exact diagnostics.
[All 20 native applications](2026-09-07-m1-planning-native.tsv) retain complete
analysis and byte-identical C. No performance budget is relaxed.

[Final default checks](2026-09-07-m1-planning-latency.tsv) at 4,000 helpers give
20.705/21.273 ms baseline/current medians. That noisy 2.7% difference prompted
[a 31-pair repeat with an identical-binary control](2026-09-07-m1-planning-default-repeat.tsv):
prior/current medians are 21.084/21.400 ms, and the mean paired difference is +0.70%.
The baseline-against-itself paired interquartile range is -2.50% to +2.30%.
The observed small difference is within this variation, not evidence of zero cost.
Both measurement sets remain recorded.

[Final session timings](2026-09-07-m1-planning-session-latency.tsv) at 4,000 helpers
give 46.278/46.776 ms for prior/current cold-plus-unchanged and 96.168/95.981 ms for
cold-plus-body. The [first pooled run](2026-09-07-m1-planning-pooled-initial-session-latency.tsv)
gave 45.360/45.543 and 92.160/92.525 ms respectively. Individual cold/unchanged pairs
straddle equality in both runs; the earlier nested-vector penalty is reduced to
small differences within observed variation. In the final separate two-clean
comparison, the retained body workload still takes 97.029 ms versus 65.580 ms.

[Dense final timings](2026-09-07-m1-planning-dense-latency.tsv) at 512 helpers give
359.836/327.927 ms for prior/current cold-plus-body, about 9% improvement. The
[first pooled dense run](2026-09-07-m1-planning-pooled-initial-dense-latency.tsv)
gave 354.683/321.806 ms. The final separate two-clean comparison gives 309.962 ms
versus 328.217 ms retained. Except the explicit 31-pair default repeat, timings use
two warmup and 11 alternating pairs at O2 with other validation finished. They
include process startup, source capture, checking, global analysis and C generation;
no general speedup or portable absolute latency budget is inferred.

The checkpoint implements function memory-plan reuse. M1 remains **in progress**:
actual flow-based availability/loans, public host-bound sessions and lifecycle,
retained global analyses, stable C fragments/backend artifacts, and complete
locality/differential/release closure remain required. The known recursive-checker
sanitizer stack limitation is still open.

## M1 retained range queries — validated scratch storage

Accepted [RFC-0138](../../design/rfcs/0138-retained-range-queries.md) starts from
validated planning checkpoint `6c39e40`; its accepted contract is committed at
`ae86ddf`. The implementation below is complete for this child. M1 remains
active and incomplete. Earlier storage variants have a measured scalar-session regression; the final
scratch-reuse implementation passes the complete child validation. No performance gate or
noise band has been relaxed.

The candidate keeps `ranges.analyze_function` as the sole function producer.
Its key combines the existing complete declaration/body/interface/prior-link
invalidation with every parameter and entry fact field and the incoming global
refinement count. Five ordinary passes and eight global parameter scans remain.
Results use snapshot-wide sparse pools, typed declaration owners and local node
ordinals. Prior history receives complete framing, owner, key and result-domain
validation before any import. Missing or malformed optional metadata causes a
miss. The present range view feeds ordinary termination validation and the sole
C emitter. Parallel analysis remains current and uncached.

The directly copied saved-row variant stores zero or one owned history object in a vector, reducing
the containing Cache header from the first prototype's 776 bytes to 536. On the current 64-bit host, History
is 280 bytes, an entry 144, key 56, saved fact 32 and saved refinement 40, excluding
vector buffers. Analyzed is 872 bytes and Snapshot 1,640. Sparse admission scans
newly produced facts once, checking remaining record capacity before each append;
an unfinished slice never acquires a published entry, and capacity failure drops
the entire optional candidate history. On a validated prior hit, normalized rows
are copied directly into new pools with a current owner and new slice starts,
without rescanning the reconstructed function to save it again.

That reproducible experimental compiler is **4,421,267 C bytes**, SHA-256
`85a6beed8e26ccb8b01bbf6452c5c41cdf5280e8ed1e9d61b477da9e546b4b0d`.
It reproduces its own C exactly. At this earlier stage, the checked-in portable seed and official build
remained the validated `6c39e40` baseline; it was not a bootstrap or release claim.
[All 197 rejected fixtures and 20 native applications](2026-09-07-m1-range-copy-native.tsv)
retain exact diagnostics, complete analysis and byte-identical C. Governance and
patch whitespace checks pass. Formatting is deterministic and idempotent on the
changed SLIM sources, fixture and manifest; original comments and source layout
have not been replaced by formatter output.

### Preserved cost experiments

Each cell below is prior/current median milliseconds. Scalar workloads contain
4,000 helpers; dense workloads contain 512 helpers with 64 planned values each.
Measurements use O2, two warmup pairs and 11 alternating pairs with other
validation finished. They include process startup, source capture, checking,
analysis, generation and artifact integrity. Earlier variants are preserved as
contrary evidence, not treated as current implementations or gate passes.

| Variant | Cold + unchanged | Cold + body edit | Dense cold + body edit |
|---|---:|---:|---:|
| Initial full validation | 46.685 / 48.171 | 94.222 / 97.402 | 323.111 / 321.182 |
| Once-per-candidate validation, reverted | 46.691 / 47.957 | 94.312 / 96.897 | 322.049 / 319.431 |
| Current-only lookup, reverted | 47.368 / 48.838 | 95.977 / 98.841 | 324.486 / 322.812 |
| Small cache header | 46.277 / 46.998 | 95.800 / 98.488 | 327.672 / 324.674 |
| One sparse admission scan | 47.223 / 48.154 | 94.704 / 96.661 | 322.335 / 319.336 |
| Validation at admission, reverted | 46.302 / 47.299 | 94.644 / 98.150 | 323.644 / 321.129 |
| Current direct saved-row copy | 46.767 / 47.671 | 95.777 / 97.466 | 323.976 / 320.966 |

Raw paired session measurements are retained for the
[initial](2026-09-07-m1-range-initial-session-latency.tsv),
[once-validated](2026-09-07-m1-range-validated-session-latency.tsv),
[current-only](2026-09-07-m1-range-current-fast-session-latency.tsv),
[small-header](2026-09-07-m1-range-header-session-latency.tsv),
[single-scan](2026-09-07-m1-range-stream-session-latency.tsv),
[admission-validation](2026-09-07-m1-range-admission-session-latency.tsv), and
[current copy](2026-09-07-m1-range-copy-session-latency.tsv) variants. Corresponding
`dense-latency.tsv` files retain the independent dense dimension; dated default
check measurements remain separate. The current separate two-clean comparison
still favors clean preparations: 65.065 versus 97.056 ms for the scalar body
workload and 305.289 versus 318.701 ms for dense bodies. Neither retained-query
counts nor dense improvements establish a universal latency improvement.

[A 31-pair repeat with an identical-binary control](2026-09-07-m1-range-copy-session-repeat.tsv)
confirms the unresolved scalar cost. Cold-plus-unchanged medians are
46.158/47.427 ms; the mean paired difference is +2.58%, with 29/31 candidate runs
slower. Cold-plus-body medians are 94.255/96.197 ms; the mean paired difference is
+2.03%, with 30/31 slower. The respective baseline-against-itself interquartile
ranges are -0.50%..+1.31% and -0.64%..+1.25%, centered near zero. This is a real
penalty, not a passed noise check. Further optimization and a fresh paired run
are required before a production checkpoint can be committed.

[Three peak-memory observations per workload](2026-09-07-m1-range-copy-memory.tsv)
use Darwin `wait4` resident bytes separately from latency. Scalar body workloads
have prior/current median peaks of 170,016,768/173,490,176 bytes; dense bodies have
543,473,664/555,286,528 bytes. These absolute host observations include runtime
regions and buffers; the one-million-record admission bound is not a physical RSS
or epoch-reclamation guarantee.

The initial, now superseded implementation passed complete prepared/range/C
comparison on 95 accepted corpus fixtures, scalar geometry through 4,000 helpers,
dense geometry through 512 helpers and two bounded ordinary/ASan/UBSan campaigns
at allocation ordinals 1..2,048. The unchanged campaign gave 581 status-71 failures
and 1,467 successes; the body-update campaign gave 1,094 failures and 954 successes.
Those results do not validate later storage changes. The expanded current run
adds every pool, extreme slice metadata, key/value domains, sparse order, parent
references, recurrence/counting metadata, source relocation, exact/one-under
admission, integer-domain edges, propagation-depth boundaries, synthetic incoming
refinement prefixes 63/64 and a valid five-entry versus invalid six-entry prior
history. Its completion is recorded separately below.


### Saved-row copy validation completed; scratch reuse under evaluation

[The completed saved-row-copy matrix](2026-09-07-m1-range-copy-matrix.tsv) passes
under ordinary O1 and ASan/UBSan O1: complete range/prepared/C equality for all
95 accepted fixtures, scalar geometry through 4,000 helpers and dense geometry
through 512 helpers. Every unchanged snapshot has zero observed work; canonical
trivia updates across the complete corpus execute zero range function producers.
The expanded metadata, integer-domain, recurrence/counting, propagation, incoming
refinement-prefix and bounded-history tests pass. A test-mode naming collision
initially stopped the full harness before fault injection; distinct slice-start
and counted-start mode names correct the test, and the repeated full run passes.

[Both allocation-fault campaigns](2026-09-07-m1-range-copy-faults.tsv) cover ordinals
1..2,048 with exact status/stdout/stderr agreement. Unchanged input gives 582
status-71 failures and 1,466 successes; the body update gives 1,096 failures and
952 successes. Successful body updates retain the required plan import. No compiler
failure is suppressed, and these remain bounded tested domains.

The subsequent scratch variant reuses three scratch fact vectors across the same
five passes. It clears every parameter binding in both input vectors before each
pair of ordinary scans, then clears every output fact only after both scans have
consumed the previous pass. Non-parameter input slots stay exactly default. The
last output vector moves into the current view. Saved history remains an owned
copy. This follows the producer input audit and preserves all five passes, eight
scans, proof limits and import validation.

An exploratory clock profile identified repeated full-vector initialization as
the dominant range-analysis cost. The scratch variant produces **4,428,841 C
bytes**, SHA-256
`ab37ad428815071d092729612bb29317da654a50ec0a3033de0f8213e1365124`,
and reproduces that C exactly. It passes the expanded quick differential matrix.
Full scratch-variant sanitizer/fault checks are still pending here;
saved-row-copy results above do not substitute for them. Measurement schema 5
adds actual full-vector initialization and reset counts, requiring three initial
fills and four complete output resets per retained query. This is a permanent
work gate for the proposed storage improvement.


The [first scratch-reuse session timings](2026-09-07-m1-range-scratch-session-latency.tsv)
give prior/current medians of 46.720/41.745 ms for 4,000-helper cold-plus-unchanged
and 95.538/85.082 ms for cold-plus-body. [Dense body timings](2026-09-07-m1-range-scratch-dense-latency.tsv)
at 512 helpers give 322.085/278.912 ms. Thus the observed scalar penalty is removed
in this run. The separate two-clean comparison remains 66.642/84.671 ms for scalar
bodies and improves to 314.166/279.248 ms for dense bodies. [Default checks](2026-09-07-m1-range-scratch-latency.tsv)
give 22.026/20.664 ms at 4,000 helpers; the ordinary path is unchanged, so that
single-run difference is not presented as a general default-path speedup.
All these timings use O2, two warmups and 11 alternating pairs after other running
validation had finished. Final confirmation is still required after the gates.

[Scratch-reuse peak memory](2026-09-07-m1-range-scratch-memory.tsv), measured
separately with three observations, has prior/current medians of
170,688,512/114,196,480 bytes for scalar bodies and
543,801,344/358,858,752 bytes for dense bodies. These remain absolute Darwin
observations rather than portable RSS gates.

Seed refresh from the preceding portable seed and the strict bootstrap now pass
at the scratch-reuse SHA above. The tracked seed and digest and `build/toolchain`
contain that candidate. Governance, Rust formatting and Clippy pass.
[Native and diagnostic comparisons](2026-09-07-m1-range-scratch-native.tsv) preserve
all 197 rejection diagnostics and the complete analysis and C of all 20 native
applications. The complete scratch corpus and geometric sanitizer matrix has
passed; its allocation-fault campaigns and the remaining required checks are
running. Implementation remains pending until all those results are checked.


### Scratch-reuse correctness and work gates completed

[The complete scratch matrix](2026-09-07-m1-range-scratch-matrix.tsv) passes under
ordinary O1 and ASan/UBSan O1, including all expanded boundary tests, 95 accepted
fixtures, 4,000-helper scalar geometry and 512-helper dense geometry. Each completed
retained query observes exactly three full-vector initializations, four complete
output resets, five function passes and eight parameter scans. Unchanged snapshots
observe zero work. For every corpus fixture, a canonical-trivia update performs
zero range-function production and imports each function result in all five passes.

[Both scratch allocation-failure campaigns](2026-09-07-m1-range-scratch-faults.tsv)
pass at all ordinals 1..2,048 with exact ordinary/sanitized status, stdout and stderr.
The unchanged campaign yields 546 status-71 failures and 1,502 successes; the
body-update campaign yields 1,000 failures and 1,048 successes. The changed campaign
requires a retained memory-plan import on every successful update. These results
apply to the named bounded fixtures and fault ordinals, not arbitrary programs.

All required checkpoint gates pass: bootstrap; governance; 10 unit and 71 integration
tests; 338 conformance fixtures and 2,000 malformed mutations; quick performance,
reduction, comparison and agent checks; parallelism; resources; parallel-runtime;
incremental; ordinary-work; Rust formatting and Clippy. Exact diagnostic and native
baselines remain as recorded above. The final independent quiet timing confirmation
is running before checkpoint acceptance. M1 itself remains open.


### Final confirmation and checkpoint scope

The [final quiet session run](2026-09-07-m1-range-scratch-final-session-latency.tsv)
confirms prior/current medians of 46.332/41.404 ms for 4,000-helper cold-plus-unchanged
and 94.417/84.443 ms for cold-plus-body, approximately 10.6% lower in both cases.
[Final dense body measurements](2026-09-07-m1-range-scratch-final-dense-latency.tsv)
give 323.942/278.361 ms at 512 helpers, about 14.1% lower. These reproduce the
improvement in the independent first scratch run. Both use O2, two warmups and
11 alternating pairs; all other validation had completed before timing.

The final separate two-clean comparison is 66.003/84.641 ms for scalar bodies and
313.289/279.337 ms for dense bodies. Retention therefore remains slower than two
ordinary clean preparations for the scalar workload; query reuse is not a general
latency claim. [Final default checks](2026-09-07-m1-range-scratch-final-latency.tsv)
give 21.383/20.519 ms at 4,000 helpers without a default-path algorithm change.
No default-path speedup or portable absolute timing budget is inferred.

RFC-0138 implementation is complete. The checkpoint retains range function queries,
reconstructs exact current facts and proof records, shares the current view with
termination and emission, and preserves the initialization-work gate alongside
all previous tests and performance gates. M1 remains **in progress** for actual
flow-based availability/loans, public host-bound sessions and lifecycle, retained
parallel/global analysis, stable C fragments/backend artifacts, and complete
locality/differential/release closure. The known recursive-checker sanitizer stack
limitation remains open.

### M1 stable declaration-local C identifiers (RFC-0139, 2026-09-07)

Starting from d157792, the sole SLIM emitter now names source bindings and
expression temporaries relative to the owning function's parameter-list node.
Checked binding links still distinguish shadowed names. Prototypes, definitions,
exclusive references, match payloads, replacement places, recurrence updates and
parallel captures all receive that same origin. Private parallel contexts and
runners also contain the escaped qualified owner function name and a reserved
`_s` separator. At most 64 executable-site owner scans use checked declaration
extents; a missing or non-function owner takes the existing internal-trap path.
No global owner vector, source feature, runtime primitive or second emitter was
added. Unchanged lowering inputs produce unchanged fragments after relocation;
changed range, plan or parallel facts may legitimately change them.

The previous seed regenerates a strict fixed point at **4,090,266 C bytes**, SHA-256
`323f36cd855a58204aee41cf1615aaa58ec0cefc76ca5e572ffaf5407872b76b`.
This is 338,575 fewer bytes than the preceding seed, principally from shorter
private identifiers. `generation-1.c` and the new fixed point compile the same
updated SLIM sources with the preceding and current naming schemes: their full
C token streams differ in 103,340 consistent private identifier occurrences and
no other tokens. This comparison treats strings, character literals and comments
as opaque; it does not rewrite production C.

The permanent `generated_private_identifiers_survive_unrelated_relocation` test
runs `scripts/verify-codegen-identities.py`. Across **96 accepted programs** (75
conformance inputs, all 20 native applications and one dedicated fixture), each
original fragment survives preceding insertion, preceding body growth, moving
that declaration to the end and deleting it. A qualified module file relocation
with preceding module insertion and a complete declaration reversal are tested
separately. Coverage includes shadowing, shared/exclusive/owned parameters,
computed matches, replacement, counted recurrence, automatic and explicit
parallel execution, similar escaped helper names and generated-looking text inside
string literals. A changed body must change its emitted fragment. Native checks
exercise serial lowering, POSIX workers, injected spawn fallback and an observed
successful join through its injected failure hook.

The [complete identifier comparison](2026-09-07-m1-stable-c-identities.tsv) records
every changed native and conformance C row. In this input domain only consistent,
injective private identifier renaming is allowed, independently per C function
scope and across the translation unit for helper names. Complete analysis reports
remain exact. All 197 rejection fixtures preserve status, stdout and stderr.
These are bounded regression domains, not a proof over every accepted program.

Validation passed bootstrap, governance, Rust formatting, Clippy, 10 unit tests,
72 integration tests, 338 conformance cases and 2,000 deterministic malformed
mutations. Nine separate benchmark gates passed: performance quick, reduction
quick, parallelism, compare quick, agent, resources, parallel-runtime quick,
incremental quick and work quick. The native compare aggregate was 1.086 versus C
and 0.986 versus Rust; the generated parallel/serial ratios were 0.870 for
`state_machine` and 0.571 for `signal_network`. No gate was removed or relaxed.
The SLIM formatter remained deterministic and idempotent.

[Ordinary O1 versus ASan/UBSan O1](2026-09-07-m1-stable-c-sanitizers.tsv) produces
exact status/stdout/stderr for all 96 accepted inputs. Two
[2,048-ordinal allocation-fault campaigns](2026-09-07-m1-stable-c-faults.tsv)
compare the same three channels exactly: `hello` has 116 status-71 failures and
1,932 successes; the mixed naming/ownership/parallel fixture has 345 failures and
1,703 successes. Eight independent process-pair workers report in ordinal order.
An earlier interrupted run is retained in the ignored work directory; the dated
artifact records the complete replacement campaign. This does not close the
separate known recursive-checker sanitizer stack limitation.

The [first frontend samples](2026-09-07-m1-stable-c-frontend.tsv),
[external backend samples](2026-09-07-m1-stable-c-backend.tsv),
[native execution samples](2026-09-07-m1-stable-c-runtime.tsv) and
[per-application output/binary sizes](2026-09-07-m1-stable-c-native.tsv) keep the
cost categories separate. `scripts/measure-codegen-identities.py` preserves the
geometric input generator and measurement protocol: complete O2 frontend process
time, then external O2 C compilation/link, then native execution with exact output
agreement. Warmups and alternating repetitions are recorded in each artifact.

The first 500-helper frontend median was 2.1% higher; `merge_sort` and
`state_machine` native medians were 13.8% and 6.0% higher. These observations are
retained, not discarded. [31-pair controls](2026-09-07-m1-stable-c-confirmation.tsv)
did not reproduce the native increases; the next frontend comparison still had
cohort drift. A final [66-group frontend control](2026-09-07-m1-stable-c-frontend-control.tsv)
rotates all six permutations of two identical baseline runs and one candidate
inside each group. Its 500-helper baseline/candidate medians are 9.952/9.974 ms;
control ratio IQR is 0.9773–1.0345 and candidate ratio IQR is 0.9710–1.0187.
The measured change falls within the observed noise. Other sizes likewise show
no repeatable increase outside their interleaved controls:

| Helpers | Baseline frontend ms | Candidate frontend ms |
|---:|---:|---:|
| 125 | 4.895 | 4.756 |
| 250 | 6.663 | 6.510 |
| 500 | 9.952 | 9.974 |
| 1,000 | 16.345 | 16.129 |
| 2,000 | 29.412 | 29.092 |

At 2,000 helpers, emitted C decreases from 1,233,074 to 1,172,186 bytes. Five
alternating [peak-memory samples](2026-09-07-m1-stable-c-memory.tsv), using Darwin
`wait4` process RSS in bytes, have medians 64,094,208/63,078,400 at that size.
The 125-helper medians are both 5,931,008 bytes; 500-helper medians are
17,514,496/17,498,112. No portable absolute memory or latency budget is inferred.

All 20 native programs preserve exact output and their measured
[Mach-O machine-code text sections](2026-09-07-m1-stable-c-machine-text.tsv) are
byte-identical on this host. External backend median ratios range from 0.963 to
1.015. The repeated native checks and unchanged machine text give no evidence of
a native execution regression; they are not a cross-target equivalence proof.
No frontend or native speedup claim is needed for the naming contract.

M1 remains **in progress**. Actual flow availability/loans, public host-bound
sessions and lifecycle, retained parallel/global analysis, C-fragment and native
backend artifact retention, and complete locality/differential/release closure
remain. This naming prerequisite does not itself retain function fragments.

### RFC-0140 completed checkpoint: retained C function fragments

Internal successful sessions now retain function prototypes, definitions and
private parallel wrappers in their existing immutable C artifact. The sole
emitter records and assembles exact byte intervals; no second program
representation or semantic compiler is introduced. The checker’s revision-bound
transition supplies declaration correspondence and complete dependency
invalidation. Reuse requires equal source/checked facts, consumed memory-plan
fields, all local range fields, counted records and complete ordered parallel
sites. Current global analysis remains authoritative. Optional-history misses
use ordinary emission, and failed updates preserve the last successful snapshot.

The full M1 goal remains active. Public host-bound sessions, actual flow/loan
orchestration, retained global analyses, native backend caching and the full M1
release/locality closure remain unfinished. The public `session` command still
reports estimates. The earlier recursive-checker sanitizer stack limitation is
not resolved by this checkpoint.

The preceding production checkpoint is **7dc0737**, with a 4,090,266-byte seed.
The final seed was regenerated mechanically from that checked-in seed and passes
strict bootstrap at **4,320,037 C bytes**, SHA-256
`b37bc2ad21e91f5d3975d0455319127322c510b73cc64e1a25a3a03fcb2353d4`.

Completed checker imports carry internal execution evidence after validated
owner/name correspondence, exact source/shape comparisons, relocation checks and
actual imported writes. False evidence uses the full comparison; incomplete
evidence misses. The native observer independently repeats the complete
ownership/source/checked-row comparison for every positive eligibility result.
Imported bytes receive their new positional checksum during copying, after old
span integrity passed. Disjoint fresh span sums plus prefix/separator/footer
checksums establish the complete artifact checksum. An independent native oracle
recomputes that complete checksum; allocation-failed default returns stay in the
failure differential. No old checksum or proof text authorizes current source.

Observation schema 6 retains all 16 earlier counters and adds six for prototype,
body and wrapper producers, fragment imports, copied bytes and counted cursor
lookups. The three-initialization/four-reset range scratch budget remains intact.
At 2,000 counted functions plus 2,000 callers and main, cold/warm counted lookups
are 4,001/8,002. A main-body edit produces one prototype/body and imports 8,000
fragments. The ordinary emitter also uses the constant-work counted cursor.
Complete C assembly, copying and integrity checks remain linear byte work.

The permanent matrix includes body/interface/layout/effect/borrow-mode and
configuration changes; actual insertion/deletion, relocation/reordering and
recovery; every new metadata/span field and payload corruption; capacity
boundaries; mixed implicit/explicit wrappers; and 63/64/65 graph/site boundaries.
One graph-bound case inserts a worker before `work62`: typing, memory and ranges
remain equal, but its executable site disappears and the body must regenerate.
Callee recurrence-step edits likewise regenerate unchanged callers when consumed
work changes or crosses the execution threshold.

Final validation passes strict bootstrap, 10 unit and 73 integration tests,
338 conformance fixtures and 2,000 deterministic malformed-input mutations,
format/Clippy checks, and the 10-project library corpus. The exact C/analysis
comparison covers 96 accepted fixtures with four relocation edits each and 197
exact rejected results. The final ASan/UBSan session run covers 96 corpus inputs
and all three 2,048-ordinal allocation-failure campaigns:

| Campaign | Failures | Successes |
|---|---:|---:|
| Unchanged input | 550 | 1,498 |
| Retained-plan update | 1,008 | 1,040 |
| Actual fragment imports | 982 | 1,066 |

Every successful fragment-update run observes real imports. The completed-import
bitmap adds one allocation per checked attempt, explaining the +1/+2/+2 failure
ordinals relative to the initial candidate. The final byte-copy optimization
changes none of these counts. The full frozen-shell run exits successfully.

The initial implementation regressed scalar edits by about 10–13%, dense edits
5–7% and counted edits about 8%; those results blocked that version. The
[initial rows](2026-09-07-m1-fragments-initial-session-latency.tsv),
[intermediate refinements](2026-09-07-m1-fragments-refinement-latency.tsv) and
[byte-copy prototype](2026-09-07-m1-byte-append-latency.tsv) remain recorded.
The existing runtime append helper now copies one-byte elements directly after
its unchanged capacity/allocation checks; other widths retain the generic copy.
This changes no interface, ABI, allocation sequence or source semantics. A
pre-adoption ASan/UBSan old/new differential checks eight widths, all 256 byte
values, growth through 512 capacity and 64 fault ordinals: 56 failures and nine
successful runs including the uninjected run match exactly. A permanent native
fixture and integration test preserve that domain. Loop unrolling and generated-C
type guards were measured and not adopted.

The [final session measurements](2026-09-07-m1-fragments-session-latency.tsv) retain
660 samples across 60 cohorts, including comparison with both the preceding
session and two independent ordinary compilations. These are uninstrumented
whole-process totals, not isolated warm-query latency or portable budgets:

| Body-edit geometry | Baseline median ms | Final median ms | Median paired ratio |
|---|---:|---:|---:|
| 4,000 scalar functions | 78.392 | 75.300 | 0.960 |
| 512 dense functions | 248.388 | 241.435 | 0.967 |
| 2,000 counted functions plus callers | 154.786 | 145.170 | 0.934 |

Of 30 preceding-session cohorts, 28 have lower median paired ratios. The other
two have mixed signs (six slower pairs out of eleven): scalar-500 unchanged has
a +0.109-ms median paired difference and counted-64 body edits +0.003 ms. No
speedup is claimed for those small noisy cases, or uniformly against two-clean.

The paired [frontend](2026-09-07-m1-fragments-frontend.tsv),
[external backend](2026-09-07-m1-fragments-backend.tsv),
[native runtime](2026-09-07-m1-fragments-runtime.tsv) and
[native identity](2026-09-07-m1-fragments-native.tsv) reports use separate old/new
runtime implementations and record their hashes. All 20 applications retain
exact C and native output. Native timings are not uniformly lower. Three suspect
runtime cases received balanced same-binary controls: `knapsack` and `arena_sum`
show +0.030/+0.029-ms group-median shifts against 0.256/0.210-ms median absolute
baseline self-pair differences; `binary_search` is lower in the confirmation.
The [runtime controls](2026-09-07-m1-fragments-native-confirmation.tsv) preserve all
samples. The [backend controls](2026-09-07-m1-fragments-backend-confirmation.tsv)
show near-zero shifts for `variants` and `knapsack`. `n_queens` retains a small
positive build-time shift (+1.1%, +1.676 ms, 21/24 slower groups), below its
1.934-ms median absolute baseline self-pair spread. Record this observed cost;
do not claim zero native compilation overhead. No performance budget is relaxed.

Final [resource measurements](2026-09-07-m1-fragments-final-resources.tsv) read the
production allocation counter and process peak RSS separately from timing. For
scalar/dense/counted body edits above, allocation attempts are respectively
119,306/119,386, 102,850/102,914 and 159,468/159,548 baseline/final. Median peak RSS
is 108.31/110.16, 339.39/340.53 and 195.44/193.52 MiB. These three-sample process
peaks include transient memory; they are not retained-live-storage measurements.
The [pre-byte-copy resource rows](2026-09-07-m1-fragments-refined-resources.tsv)
remain available independently.

All unchanged required and durable gates pass: performance, work, reduction,
parallelism, resources, host, parallel-runtime, incremental, project, applications,
compare and agent. Generated parallel/serial ratios are 0.746 for `state_machine`
and 0.592 for `signal_network`. The [gate record](2026-09-07-m1-fragments-gates.tsv),
[complete deterministic work rows](2026-09-07-m1-fragments-work.tsv) and
[budget outputs](2026-09-07-m1-fragments-budgets.txt) retain the evidence. These
checkpoint results do not complete M1 or establish an LLM success rate.

## RFC-0141 working implementation: structured checking continuations

The accepted contract is checkpoint `dfbb722`, based on production `49ea4a0` and
the corrected parent ledger at `17373e5`. This section records unfinished working
implementation evidence, not a completed compiler checkpoint or M1 closure.
The checked-in seed and installed compiler remain at the validated RFC-0140 code.

The new canonical `control` descriptors supply binding and assignment children to
both the normal checker and optional `flow` graph. The checker replaces those two
families' recursive orchestration with explicit initializer/value and body phases.
It preserves initializer move/loan registration, destination checks, successful
assignment restoration, blocking-error behavior and enclosing fact completion.
The active frame is a scalar machine parameter; only suspended parents enter the
reused vector. A one-frame expression therefore needs no vector backing allocation.
The depth bound is the current root expression's canonical node count, not the
optional graph's one-million-record reporting limit. Other expression families
still use their existing orchestration and remain required RFC-0141 work.

The initial active-frame experimental compiler's generated C is 4,347,076 bytes, SHA-256
`9825ea437268d5c42770fc65175882a160e0e5ef7ebc6a84451730adfeadef2d`.
It is generated by the baseline compiler and reproduces those exact bytes itself.
It has not replaced the portable seed. Two earlier binding-only and vector-stack
binding/assignment candidates are retained with their source images and binaries
under `build/slim-next-m1/ownership-flow/` for comparison.

### Intermediate compatibility evidence

- The current candidate matches raw C and complete `analyze` output on 96 accepted
  fixtures, passes four declaration relocation edits per fixture plus module
  relocation/reordering, and preserves tested native serial/worker/fallback results.
  All 197 rejected fixtures have exact ordinary status/stdout/stderr.
- The new `tests/fixtures/checking_continuations.slim` probe dumps all canonical
  token fields, fact rows, ordered view/final issues and layout rows. It is compiled
  against both source checkpoints and is never consumed as compiler authority.
  The current candidate matches the baseline over 293 fixtures: 269 reach checked
  state and 24 report parser rejection without attempting the checker. These are
  complete comparisons of the named records, not proof of every possible program.
- The preceding vector-stack candidate passes this 293-fixture probe under
  ASan/UBSan and two 512-ordinal ordinary/sanitized fault differentials. Branch
  reinitialization observes 125 status-71 failures and 387 successes; the borrowed
  initializer observes 76 failures and 436 successes. Every successful dump equals
  the uninjected dump and failures emit no partial dump. These ordinal counts
  belong to that candidate; the active-frame version is checked separately.
- The active-frame version also passes the full 293-fixture sanitizer comparison.
  Its two 512-ordinal campaigns observe 122 failures/390 successes for branch
  reinitialization and 74 failures/438 successes for the borrowed initializer,
  totaling 196 failures and 828 successes. Complete successful dumps and all
  failure outputs match ordinary/sanitized execution. The changed allocation
  counts are consistent with fewer vector backing allocations; no ordinal is
  assumed to identify the same allocation across different compiler versions.
  [Checked-state rows](2026-09-07-m1-continuations-state.tsv) and
  [fault rows](2026-09-07-m1-continuations-faults.tsv) identify this candidate.

### Initial costs and remaining work

Two warmup pairs and eleven alternating uninstrumented process pairs cover
binding spines, small functions and assignment spines at 125, 500, 2,000 and 4,000
items. Source inspection/output work is included. C equality is checked before
measurement. The raw reports include binary/C/runtime identities:
[binding-only measurements](2026-09-07-m1-continuations-bindings-initial.tsv),
[vector-stack assignments](2026-09-07-m1-continuations-assignments-initial.tsv), and
[active-frame measurements](2026-09-07-m1-continuations-inline-initial.tsv).

The initial binding-only 4,000-small-function check has median paired ratio 1.030;
adding assignments with every frame in the vector gives 1.036. Keeping the active
frame directly reduces the measured ratio to 1.017, with the corresponding C
emission ratio 1.008. The 4,000-assignment spine's latest check/emission ratios are
0.971/0.989; the binding spine is 1.000/1.003. These are intermediate same-host
measurements, not a general speedup or permission to accept a reproducible cost
outside the recorded noise band. The earlier regressions remain part of the
record. Full resource/backend/runtime costs and durable gates remain outstanding.

At that initial candidate, branch, call, aggregate, projection, recurrence and
parallel continuations were still pending. Subsequent working migration is recorded
below. Complete shared lexical boundaries, exact native work and stack observations,
permanent cross-phase tests, complete fault/session campaigns and all required
checkpoint checks remain open. No existing gate or semantic rule is relaxed.
The full M1 goal also retains public host-bound sessions, global-analysis retention,
native backend caching and integrated release closure.

### Branch, unary, aggregate and argument continuation migration

The corrected branch candidate emits 4,369,112 C bytes, SHA-256
`08334b726220c87e9eadc01fc4ba9eb3f7f1614349db0c410485ee2709b849c5`.
The subsequent projection/parallel candidate emits 4,376,110 bytes, SHA-256
`386874c869ea2acf6b6da43139a21f8eeb8f7ecab53bb005546a75e49dad71b7`.
Both reproduce their compiler C exactly. Each matches 293 complete checked-state
fixtures and 96 complete function-graph fixtures. The unary descendant additionally
passes the exact C/analysis, relocation and native-execution suite above.
[Branch state](2026-09-07-m1-continuations-branch-state.tsv),
[branch graph](2026-09-07-m1-continuations-branch-graph.tsv),
[unary state](2026-09-07-m1-continuations-unary-state.tsv) and
[unary graph](2026-09-07-m1-continuations-unary-graph.tsv) retain named comparisons.

Both candidates pass the 293-fixture ASan/UBSan checked-state comparison and two
512-ordinal fault domains. Each observes 123 failures/389 successes for branch
reinitialization and 76 failures/436 successes for the borrowed initializer:
199 status-71 failures and 825 successes overall. Ordinary/sanitized results match
exactly, successful dumps equal the uninjected result and failed dumps are empty.
[Branch faults](2026-09-07-m1-continuations-branch-state-faults.tsv) and
[unary faults](2026-09-07-m1-continuations-unary-state-faults.tsv) identify their
respective compiler fingerprints; ordinals are not cross-version allocation IDs.

The first branch implementation was rejected by the complete-state differential.
In `duplicate_variant_arm.slim`, fact 29 changed from baseline tag 5/form 34 to
tag 0/form 29, despite matching status, diagnostics and accepted generated C.
Inspection of baseline C established that the old arm loop's `recur` jumps before
its trailing validity expression. The corrected continuation retains the original
first-arm result, exhaustiveness result and outer scrutinee-move override, while
preserving all rejecting issues. The old record-member, variant-payload and call
argument loops have the same terminal-jump structure. Their migration preserves
executed pre-transfer work and final arity/name results; it does not introduce
post-jump validity aggregation. The rejected candidate and first mismatch remain
in the private experiment directory, and no seed was adopted from it.

The new explicit-compiler `scripts/verify-continuation-ownership.py` harness runs
the independent 486-case move/read and 15,552-case move/reset path models. Both
the corrected branch candidate and the later user-call/recurrence candidate pass
all 16,038 cases: 7,230 accepted and 8,808 rejected, with exact baseline outputs.
The oracle evaluates bounded action paths independently of compiler facts.

Aggregate members and user-call/recurrence arguments are subsequently migrated.
Each intermediate candidate matches 293 complete checked-state fixtures, 96 full
graph fixtures and its self-emitted C. The argument candidate also passes all
96 accepted C/analysis/relocation/native comparisons and 197 rejected outputs.
The newest working source migrates built-ins and replacement operands; its
validation is still in progress. These are working experiments, not compiler
checkpoint commits or completed RFC-0141 acceptance.

The [branch measurements](2026-09-07-m1-continuations-branch-initial-latency.tsv)
and [unary measurements](2026-09-07-m1-continuations-unary-initial-latency.tsv)
retain two warmup pairs and eleven alternating pairs at each geometric size.
At 4,000 small branch functions, the branch candidate's median check/emission
ratios are 1.040/1.013; the unary descendant's are 1.044/1.020. At 4,000 projection
functions the unary ratios are 1.016/1.002. These measured costs remain open for
containment and repeat noise analysis before adoption. No regression is waived.

### Complete expression traversal candidate and measured refinements

The first candidate with all expression families migrated emits 4,405,570 C bytes,
SHA-256 `d2b6ff8e33a8cdb1057b74c13a7c168e38f4835cf4219d837aa33661e3a17bdf`.
Its only call to `infer_expr` is function entry; expression children resume the
explicit machine. It reproduces its compiler C exactly and passes 293 complete
state comparisons, 96 complete graph comparisons, all accepted C/analysis/
relocation/native checks and 197 exact rejected outputs. The
[state](2026-09-07-m1-continuations-builtin-state.tsv),
[graph](2026-09-07-m1-continuations-builtin-graph.tsv) and
[C/analysis](2026-09-07-m1-continuations-builtin-exact.tsv) rows identify it.

This candidate passes ASan/UBSan over the 293-fixture checked-state domain. Its
two 512-ordinal campaigns observe 128 failures/384 successes for branch
reinitialization and 79 failures/433 successes for the borrowed initializer,
totaling 207 failures and 817 successes. The
[fault rows](2026-09-07-m1-continuations-builtin-state-faults.tsv) preserve those
candidate-specific counts and exact ordinary/sanitized comparisons.

New native observation records actual entry/exit, continuation steps, requests,
pushes, finish events, live depths and each built-in operand phase. The counter
cap is one billion, with explicit saturation status; these binaries are excluded
from latency measurements. All 293 fixtures exercise every continuation phase
0–12 and every applicable built-in operand phase, with identical repeated work
and ordinary/observed outputs. Eight independent generated families—binding,
assignment, branch, user call, scalar built-in, record construction, variant
payload and recurrence—pass at sizes 8, 32, 128 and 512. Observed native machine
nesting is one, live depth stays within the source-relative bound, and the
fourfold-size step ratio stays below 4.5. The
[complete work rows](2026-09-07-m1-continuations-builtin-work.tsv) retain exact
measurements for this domain. These are traversal counts, not a count of all
existing source/origin scans or proof that every compiler helper is iterative.
An initial generated aggregate fixture used unsupported constructor projection;
both compilers rejected it. The corrected fixture composes a constructor with
an ordinary function returning its field, without adding syntax.

`scripts/verify-continuations.sh` now builds ordinary/observed current-source
compilers and runs the permanent work and independent ownership oracles. Its
full mode also builds ordinary/sanitized complete-state probes and runs the fixed
fault domains. The full release script invokes this gate. Source-based and
explicit-binary comparison tools retain failed sources for diagnosis. Integration
is still subject to its own execution and the required full checkpoint gates.

Governance initially rejected the removed `infer_tcp_exchange` source anchor.
RFC-0141 replaces that implementation with shared operand continuations. The
gate now requires the exact zero-operand I64 clock shape, six-operand TCP shape
and scalar/reservation/byte-output helpers. All prior runtime, effect, conformance
and host-execution checks remain. Governance passes with these updated anchors.

The [full-traversal timing sample](2026-09-07-m1-continuations-builtin-initial-latency.tsv)
still shows 1.049/1.019 median check/emission ratios for 4,000 small branch
functions. A subsequent single structural classifier shared with the graph
preserves all state/graph comparisons; its generated C is 4,404,535 bytes,
SHA-256 `2c76d1eab4b6602397b2bdd6a05415741d829cc50eced844698856a116b5f2ea`.
Its [timing sample](2026-09-07-m1-continuations-dispatch-initial-latency.tsv)
shows 1.041/1.026 for that shape, with mixed shifts elsewhere. This is not a
resolved performance regression or a general improvement claim. The next working
experiment keeps the active branch record directly and stores only suspended
branch ancestors. Adoption, shared lexical lifetime completion, counter-cap tests,
full resource/backend/session campaigns and all checkpoint checks remain open.

### Shared scopes and compact active continuation state

The later working candidate (`argument-top`) emits 4,432,453 bytes of compiler C,
SHA-256 `81d80e0176216530fa9f42cdd7c529e61eef60cd5e83a998417da3d8a175f33a`.
It keeps the active expression, branch and user-call argument state directly in
the machine; reused vectors hold their suspended ancestors. Unary/binary scalar
built-ins use compact continuation phases 13–17, avoiding a separate built-in
pool entry. Collection, I/O and replacement contexts retain their pool. Completed
forms finish and pop directly, and an atom or blocked entry resumes its parent
in the same iteration. These changes preserve the existing post-child checking
sequence. `control.Scope` now supplies both lexical loan membership and the
validated graph scope extent.

This candidate reproduces its own compiler C and passes the complete
[293-fixture state comparison](2026-09-07-m1-continuations-argument-top-state.tsv),
[96-fixture graph comparison](2026-09-07-m1-continuations-argument-top-graph.tsv),
and [C/analysis/relocation/native comparison](2026-09-07-m1-continuations-argument-top-exact.tsv).
The [fixed malformed-input domain](2026-09-07-m1-continuations-argument-top-mutations.tsv)
has 1,601 exact baseline comparisons: 115 accepted, 1,300 checker-rejected and
186 parser-rejected, within its fixed 4,096-case cap.

The full `verify-continuations.sh` gate also passes on this candidate. Observation
schema 2 crosses all 18 continuation phases and all applicable built-in operand
phases, including exact and saturated four-event counter tests. The eight
geometric families preserve native traversal depth one over sizes 8/32/128/512.
The independent ownership domains retain 16,038 exact cases (7,230 accepted and
8,808 rejected). All 293 complete states and 1,601 malformed cases agree between
ordinary and ASan/UBSan builds. The two 512-position allocation-fault campaigns
record 125 failures/387 successes for branch reinitialization and 77 failures/435
successes for the borrowed initializer: 202 failures and 822 successes in total.
Failure remains status 71 with the exact allocation diagnostic. These results
do not substitute for the separate full retained-session fault campaign.

The [initial six-shape timings](2026-09-07-m1-continuations-argument-top-initial-latency.tsv),
[additional four-shape timings](2026-09-07-m1-continuations-argument-top-additional-latency.tsv)
and [selfhost timings](2026-09-07-m1-continuations-argument-top-selfhost-latency.tsv)
retain two warmups and eleven alternating uninstrumented pairs. At 4,000 helper
functions, checking ratios are 1.012 for scalar built-ins, 1.023 for user calls,
1.003 for aggregate construction and 1.021 for recurrence. Selfhost check/emission
ratios are 1.011/1.012. Scalar and active-argument storage reduced earlier measured
costs, but these samples alone do not establish the noise band or adoption.
Interleaved identical-baseline controls, complete resource/backend/session costs
and the required checkpoint gates remain open. The checked-in portable seed and
installed compiler still identify the previous production checkpoint.

The completed [66-group control run](2026-09-07-m1-continuations-argument-top-control.tsv)
rotates all six orders of two identical baseline runs and one candidate, with
two warmups. Most candidate medians fall within the corresponding baseline
ratio interquartile interval. Three checking medians exceed that interval's
upper quartile: flat branches 1.01141 (control 0.98982–1.00590), user-call helpers
1.01869 (0.98405–1.01681), and selfhost 1.01054 (0.99063–1.00590). These small
remaining costs are recorded rather than declared noise. The follow-up
`root-completion` candidate returns a finished outermost expression directly,
removing its otherwise empty final machine iteration. Nested completions still
restore and resume their suspended parent. It emits 4,432,432 C bytes, SHA-256
`a80b4d897eaac295e8f53212df831277d2abe824c04d908b4ed04a18c1c01056`, reproduces
that C, and preserves all 293 complete states and 96 graph outputs. Full gates
and adoption measurements for this refinement are in progress.

`scripts/measure-continuations.py` retains all ten timing generators and the
rotating identical-baseline protocol, with bounded sizes/repetition counts and
compiler/input fingerprints. Its generated 4,000-size sources are byte-identical
to the inputs used above. Timing outputs remain evidence for explicit binaries,
not a source-acceptance path or an automatic performance waiver.

The root-completion candidate passes the complete
[state](2026-09-07-m1-continuations-root-completion-state.tsv),
[graph](2026-09-07-m1-continuations-root-completion-graph.tsv),
[malformed-input](2026-09-07-m1-continuations-root-completion-mutations.tsv) and
[C/analysis/native](2026-09-07-m1-continuations-root-completion-exact.tsv)
baseline comparisons. Its [native work rows](2026-09-07-m1-continuations-root-completion-work.tsv)
cover all 18 continuation phases and every applicable built-in operand phase,
with exact repeated outputs/counters across the 293-fixture corpus and all eight
geometric families. Native expression-machine depth remains one. The separate
flow-boundary E2E test passes against a probe built from the current source.

The [resource report](2026-09-07-m1-continuations-root-completion-resources.tsv)
contains 1,008 rows: three samples for each compiler variant across 168 measured
combinations. Ten shallow families use sizes 125/500/2,000/4,000 and eight nested
families use 8/32/128/512; clean checking/emission, selfhost, and unchanged/body-
edited sessions remain separate. Observers read the actual root allocation
attempt counter after shutdown and `getrusage` process peak RSS. They are excluded
from latency measurements. Attempts are deterministic across the three samples;
RSS values below are their medians, not retained-live-storage totals.

At 4,000 scalar, user-call, branch-helper, projection and construction functions,
checking allocation counts match the baseline. A 4,000-binding spine uses
4,145 versus 4,135 attempts; deeply nested continuations require scratch storage.
For selfhost checking, attempts are 109,815 versus 106,590 and median peak RSS is
171,261,952 versus 170,311,680 bytes. The 4,000-flat-branch case rises from
32,161,792 to 34,013,184 bytes. At 512 dense session bodies, unchanged/update
attempts are 87,762/104,967 versus 85,714/102,915, with candidate median peaks
202,833,920/358,334,464 versus 201,900,032/357,384,192 bytes. Scalar session
allocation counts remain equal at 4,000 helpers. These measured costs are not
hidden by traversal counters or described as a passed latency gate.

The [sanitized geometric observations](2026-09-07-m1-continuations-root-completion-sanitized-work.tsv)
pass all eight families at 8/32/128/512 under the host's default stack limit,
with exact ordinary/sanitized outputs and repeated counter rows. Full continuation
verification now includes this additional geometric sanitizer step; ordinary
corpus coverage still independently requires all 18 phases. The already completed
[gate results](2026-09-07-m1-continuations-root-completion-gates.tsv) retain the
ownership domain, checked-state sanitizer/fault/mutation results, retained-place
campaign (193 failures/1,855 successes in 2,048 positions), independent flow E2E
and governance. Final integrated checkpoint execution remains required.

The historical RFC-0129 4,000-binding source was also checked on this candidate.
Its SHA-256 is `4620a34ca44c1db4ee314cc4ee703438be8b765fabcd5ae6d5048020b78b71c3`.
Ordinary checking succeeds; O1 ASan/UBSan aborts with stack overflow under the
8,372,224-byte soft stack limit. `atos` resolves the repeated PCs to
`check.find_unknown_expr`, including generated observed-C lines 11529/11550.
This preceding name-resolution traversal is unchanged by RFC-0141. The abort
does not publish a counter report; no missing counters are interpreted as zero.
The old limit remains explicit, and expression-machine depth one is not a claim
that every compiler helper is iterative.

The candidate's [full retained-session campaign](2026-09-07-m1-continuations-root-completion-session.tsv)
passes the corruption, failed/recovered update, configuration, relocation,
insertion/deletion/reordering and corpus/geometric work matrix. All 6,144 fault
positions preserve ordinary/sanitized status, output and diagnostics. The cold
campaign records 550 failures/1,498 successes; changed-update records 1,008/1,040;
fragment publication records 982/1,066. Successful update/fragment cases retain
the expected query and fragment imports. Total failures/successes are
2,540/3,604. This validates the internal session on the candidate; public host
binding and transport remain separate M1 obligations. Its final latency controls
start after all these native validation jobs finish.

The [root-completion controls](2026-09-07-m1-continuations-root-completion-control.tsv)
still show selfhost checking at 1.01154 against a baseline-control IQR of
0.99116–1.01041. The candidate remains unadopted. A subsequent `single-read`
experiment moved expression-family decoding into one checked canonical-token
read in `syntax`, preserving exactly the existing tag and legacy-head behavior.
It emitted 4,435,148 C bytes, SHA-256
`dc66cf17c94723f319cb84299c80a7138b3611e2cecf749977bd48565567375d`, reproduced
its C, and passed [293 state](2026-09-07-m1-continuations-single-read-state.tsv)
and [96 graph](2026-09-07-m1-continuations-single-read-graph.tsv) comparisons.
Its quick continuation gate passed, including 218,448 classifier combinations,
24 extreme tags and five invalid indices compared with the prior predicate
cascade. That functional compatibility did not establish a performance gain:
[66-group controls](2026-09-07-m1-continuations-single-read-control.tsv) still
show selfhost checking at 1.01191 (control IQR 0.99552–1.00993). The experiment
was reverted to the frozen root-completion source; its private source/probe and
measurements remain available. The unsuccessful read-consolidation experiment
does not justify changing the noise criterion or any existing performance gate.

The `operand-modes` refinement derives operand modes from the already decoded
built-in operation and restores the existing short-circuit lookup for
`mem.replace`. Mode/capability diagnostics retain their order; no source mode or
capability changes. Its 4,431,295-byte C has SHA-256
`bc1387b12b86fd494105c74360d6413c5d6e9ef4b13f99061cd43cb11ae2490c`.
Self-reproduction, complete state/graph comparisons, 1,601 malformed cases and
the quick continuation gate pass. Its
[66-group focused controls](2026-09-07-m1-continuations-operand-modes-focused-control.tsv)
show built-in checking at 1.00078 (control IQR 0.98604–1.01309), user-call
checking at 1.01384 (0.99475–1.01173) and selfhost checking at 1.00849
(0.98954–1.01124). The remaining user-call cost motivates retaining the
already-resolved parameter type in the unused argument continuation field.

That `argument-type` candidate emits 4,432,108 bytes, SHA-256
`42e0c7b6b4644df727a78cfe551395958c1479269f87325522c30128a2a7c12e`,
and reproduces its C exactly. Its
[293 complete states](2026-09-07-m1-continuations-argument-type-state.tsv),
[96 complete graphs](2026-09-07-m1-continuations-argument-type-graph.tsv) and
[1,601 malformed inputs](2026-09-07-m1-continuations-argument-type-mutations.tsv)
match `49ea4a0`. The parameter declaration cannot change during child checking;
the retained value is replaced when advancing each sibling argument. These
results do not substitute for final performance and integrated checkpoint gates.

The argument-type quick gate passes all 16,038 ownership cases. Its
[focused controls](2026-09-07-m1-continuations-argument-type-focused-control.tsv)
show call checking at 1.01440 against control IQR 0.98988–1.01047; the small
repeatable cost remains. Selfhost checking is 1.00847 against 0.99021–1.00934.
No performance exception is inferred from either result.

The `token-read` refinement consolidates three immutable token accesses inside
`syntax.token_equal`. It preserves both eager comparisons, including span traps
when a virtual spelling matches, and the invalid-index empty-span behavior.
It is separate from the rejected expression-classifier experiment. C size is
4,432,372 bytes, SHA-256
`59d895508fa9011dea2d833738e4ea6a7da72829412130de48d790685b001706`, with exact
self-reproduction, [293 states](2026-09-07-m1-continuations-token-read-state.tsv),
[96 graphs](2026-09-07-m1-continuations-token-read-graph.tsv), and
[1,601 malformed cases](2026-09-07-m1-continuations-token-read-mutations.tsv).
The permanent independent token oracle covers 116,952 results, including
encoded/negative/extreme tags, virtual and exact spellings, empty/reversed spans,
and invalid indices. Ordinary and ASan/UBSan probes also retain bounds and
subtraction-overflow traps after positive virtual matches.

The token fixture exposed an existing emission defect: passing a string literal
directly to `vec.push` can emit an undeclared C variable. The minimal program
`let values: Vec[Bytes] = vec.new(); vec.push(@values, "i64.add")`, written as
ordinary separate SLIM statements, is accepted by both `49ea4a0` and token-read;
both emit the identical undeclared variable and fail native C compilation. The
private reproducer and both diagnostic artifacts are retained in the ownership-
flow work directory. The boundary fixture uses named Bytes values to continue
its independent test. This defect remains a recorded follow-up for M1 closure;
it is not attributed to the token-read change or treated as supported execution.

The [token-read focused controls](2026-09-07-m1-continuations-token-read-focused-control.tsv)
complete 66 groups per operation. Candidate checking medians are 0.98921 for
built-in helpers, 1.00349 for user-call helpers and 0.99244 for selfhost, against
respective baseline-control IQRs 0.98335–1.00953, 0.98377–1.00683 and
0.99551–1.00969. Emission medians are 0.98839, 0.98752 and 0.99210. This
removes the observed call-checking regression in that measured domain. Geometric,
retained-session, resource and external-backend measurements and final integrated
checkpoint gates remain required. The quick gate passes with the independent
token boundary oracle, every continuation phase and all 16,038 ownership cases.

The final candidate's [geometric controls](2026-09-07-m1-continuations-token-read-geometric-control.tsv)
cover ten shallow families at 125/500/2,000/4,000;
[nested controls](2026-09-07-m1-continuations-token-read-nested-control.tsv) cover
eight families at 8/32/128/512. Each operation has 18 interleaved three-run
groups. All ten rows whose median initially exceeded both one and the control
upper quartile were selected explicitly for 66-group repeats;
[selection](2026-09-07-m1-continuations-token-read-repeat-selection.tsv) and all
repeat rows remain recorded. Nine repeats lie inside the measured noise band.
The 4,000-function checking repeat is borderline: median 1.01194, upper control
quartile 1.01142. A larger
[120-group confirmation](2026-09-07-m1-continuations-token-read-functions-confirmation.tsv)
records 1.00997 against control IQR 0.98713–1.01103. No criterion or durable
budget was relaxed; the earlier borderline samples remain visible rather than
being replaced by the larger confirmation. These are same-host measured domains,
not proof that every program has unchanged latency.

The [retained-session latency matrix](2026-09-07-m1-continuations-token-read-session-latency.tsv)
contains 660 rows over 15 geometries, unchanged/body updates, previous-session
and two-clean comparisons, with 11 alternating pairs after warmup. The only
previous-session median above 1.01 is the 125-scalar body edit at 1.01228. Its
[66-group control](2026-09-07-m1-continuations-token-read-session-confirmation.tsv)
is 0.99737 against IQR 0.97852–1.01745.

[Resource observations](2026-09-07-m1-continuations-token-read-resources.tsv)
retain 1,008 separate allocation/peak-RSS rows. Selfhost checking uses
109,840 versus 106,615 allocation attempts; median peak RSS is 170,573,824
versus 168,542,208 bytes. At 512 dense session functions, unchanged/update
attempts are 87,760/104,966 versus 85,712/102,914; candidate RSS medians
are 201,146,368/354,926,592 versus 199,049,216/356,417,536 bytes. Allocation
attempts remain exact within each three-sample case. RSS is process peak
storage, not retained-live-storage or an inferred allocation count.

Separate [frontend](2026-09-07-m1-continuations-token-read-frontend.tsv),
[external backend](2026-09-07-m1-continuations-token-read-backend.tsv),
[native runtime](2026-09-07-m1-continuations-token-read-runtime.tsv), and
[native identities](2026-09-07-m1-continuations-token-read-native.tsv) cover the
20 applications. Every application retains byte-identical C and exact output.
Four candidate Mach-O files are eight bytes smaller; all their named sections
remain [byte-identical](2026-09-07-m1-continuations-token-read-native-sections.json).
No executable-code size reduction is claimed from those container differences.
The complete [C/analysis/relocation differential](2026-09-07-m1-continuations-token-read-exact.tsv)
passes 96 accepted fixtures, four relocation edits each, module relocation and
reordering, native serial/worker/fallback checks, and 197 rejected fixtures.
Governance, formatting and diff checks pass. Seed refresh and the complete
checkpoint verification suite follow these candidate measurements; this entry
does not yet mark RFC-0141 or M1 complete.

Integrated checkpoint verification first encountered sandbox denial of loopback
listener creation in three existing E2E tests. The authorized local-network run
passed all 73 E2E tests. Its ordinary performance gate passed, then the older
work observer exposed a stale anchor: `expression_check_calls` observed only
126 function-root entries instead of the unchanged expected 253 expression
entries in the 125-helper fixture.

The test-only observer now adds actual child requests at the continuation loop
header to that same counter. It excludes parent resumes, declined arguments and
the already-counted depth-zero root, resolving the returning/depth formals from
the exact generated signature. Both entry anchors are checked for exactly one
occurrence. The metric names, schema, caps and all geometric expectations remain
unchanged. The [complete work campaign](2026-09-07-m1-continuations-token-read-work-gate.tsv)
passes, including ordinary/observed output equality, cap crossing and allocation
failure observations. Production SLIM and seed identities are unchanged by this
observer correction; the integrated suite is rerunning with it.

The next integrated run passed conformance, all benchmark stages, the full
continuation gate, source-identity/flow checks and the retained-typing corpus,
then stopped because all 512 retained-typing fault ordinals failed. The required
successful ordinal was outside that old campaign.
[Boundary measurements](2026-09-07-m1-continuations-token-read-retained-window.tsv)
show that `49ea4a0` already uses 569 root allocation attempts on this unchanged
probe and fails at ordinal 512; the candidate uses 572. Both fail at their last
allocation and succeed at the immediately following ordinal. This is an outdated
fault-coverage window, not a newly exceeded 512-allocation performance budget.
The fixed campaign now covers positions 1–1,024, retaining all original positions
and both failure/success assertions. Existing work/resource/performance budgets
are unchanged. The candidate adds three measured probe allocations; neither that
cost nor the baseline failure is hidden. The extended retained gate and the
remaining downstream gates are being validated before integrated closure.

The extended [retained-typing gate](2026-09-07-m1-continuations-token-read-retained-gate.tsv)
passes 95 fixtures and all 1,024 fault positions: 572 failures and 452 successes.
Verification resumes from the remaining `verify.sh` commands with a fresh
sanitized seed compiler. Already-passed bootstrap, Cargo, conformance, all
benchmark stages, continuation and flow results apply to the unchanged production
seed. The corrected test-only gate is independently reverified. The earlier
whole-script invocation remains recorded as stopped; it is not relabelled a
successful run. Full M1 release closure will still run the complete
`verify-0.9.sh` command on its final identified checkpoint.

## M1 shared checking continuation checkpoint — 2026-09-07

RFC-0141 is complete on seed
`59d895508fa9011dea2d833738e4ea6a7da72829412130de48d790685b001706`.
The [verification ledger](2026-09-07-m1-continuations-token-read-checkpoint-gates.tsv)
records all eight required commands, the additional work/host/resource/project
benchmarks and every applicable sanitizer/recovery gate. Verification consists
of the successful prefix on this seed, the corrected observer and extended
retained-typing gate, and the [remaining exact script steps](2026-09-07-m1-continuations-token-read-remaining-gates.tsv).
The remaining-step runner exited zero. This is explicit composite verification;
the earlier whole-script stops are retained and explained above.

The complete session campaign passes 550/1,498 cold, 1,008/1,040 update and
982/1,066 fragment failure/success counts. Retained projects pass 620/1,428
across 2,048 positions; places pass 193/1,855. Parsing passes 1,720 complete
comparisons, 2,000 mutations, geometric reuse, exact/beyond source/token/lexeme
capacities and 94/1,954 fault results. Integer lowering passes its full gate
with 117/395 fault results. Source indexing checks 109 failures in 128 ordinals;
the final compiler/native allocation checks and sieve/vector programs pass.

The implementation shares canonical control descriptions with the optional graph
and migrates every expression inference family onto source-bounded continuations.
Complete checked-state, diagnostic, graph, analysis and emitted-C comparisons
remain exact over the recorded baseline domains. Compiler scratch allocation,
RSS and seed growth are measured; pending frames are internal compiler storage.
The portable seed grows by 112,335 bytes (2.60%) from `49ea4a0`. Compiled user
applications retain exact C and output in the 20-application domain.

M1 remains in progress. Public host-bound sessions and owning-epoch lifecycle,
retained global/parallel analyses, native backend caching, the literal-Bytes
storage-address repair and integrated full release closure remain required.
