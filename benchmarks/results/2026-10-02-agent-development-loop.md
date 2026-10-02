# Early agent development loop, 2026-10-02

Status: complete for the bounded RFC-0158/0159 outcome. General agent
effectiveness and M2/full M3 remain pending.

## Outcome and decision

The production compiler now produces bounded semantic context for one selected
declaration in a complete, accepted SLIM program. The implementation is SLIM in
`selfhost/context.slim`; ordinary checking remains the sole semantic authority.
No language syntax, ownership rule, runtime ABI or dependency was added.

Decision: narrow. Retain schema 1 as an opt-in development tool, freeze its
breadth, and improve evaluation transport before a larger held-out experiment.
The first feasibility pilot cannot establish an effectiveness gain. Tool
footprint, contract gaps and transport confounds justify holding further scope.
This adopts the bounded technical contract, not a general productivity claim.

## Supported contract

`slimc context SOURCE EXPECTED_SOURCE MODULE.DECLARATION` compares the complete
captured source, or manifest and every ordered project module, byte-for-byte
with an independently retained expected input. Checksums are explanatory; they
cannot authorize an edit. Capture is sequential, not an atomic filesystem
snapshot. A client must recheck input bytes and compiler identity before editing.

The JSON contains original selected source, checked declaration signatures,
parameter modes, types, declared effect ceilings and supported checked reference
links. Facts and section completeness retain independent exact, bounded and
unknown labels. It does not infer that a declared effect happened, provide
arbitrary program-point loan permissions, or accept an invalid editor snapshot.
The complete contract and encoding are in [CONTEXT.md](../../docs/CONTEXT.md).

Admission is limited to 16 MiB per captured side, 128 modules and one million
canonical nodes. The input-byte limit applies after existing file capture; it
is not a pre-read or memory limit. Selected source is limited to 65,536 bytes;
providers, facts and references have independent 64/512/512 row limits. Type
depth/text, signatures and total report bytes have separate fixed bounds.
Oversized reports are rejected without publishing partial JSON. Unsupported or
bounded-away facts remain unknown or explicitly incomplete.

The `inspected_nodes` counter measures the selected lexical scan only. Repeated
descendant scans for source spans and provider/type work are outside that
counter. It cannot prove approximately linear total context work. Existing
default compilation and permanent performance gates remain unchanged.

## Independent correctness and cost evidence

Permanent production-path tests cross exact input, module, canonical-node,
selected-source, provider, fact, reference and output boundaries. They also
check same-length stale changes and checksum collisions, unselected-module
changes, read-error precedence, UTF-8/control-byte round trips, inferred versus
declared type spans, enum payloads, linked builtins and complete original source
spans. All 15 conformance groups passed on the final compiler. Transport tests
remain independent of participant success.

Two earlier boundary-test failures were fixture-count errors. Small independent
node probes established the actual counts before correcting the fixtures; no
production budget was relaxed. Earlier failing logs are retained.

Native measurements use seven alternating same-host samples and a fresh control
built from the pre-change C seed with the same captured runtime and flags:
`-std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror`. The older September binary has a
different runtime and is not the attribution control. Build flags are recorded
build provenance, not facts proved by an executable hash.

| Artifact | Before | After | Change |
| --- | ---: | ---: | ---: |
| Native compiler executable | 487,456 bytes | 625,392 bytes | +28.3% |
| Portable C seed | 4,877,338 bytes | 5,118,812 bytes | +4.95% |

The final seed SHA-256 is
`7cb21bc57d173a5ab25982dee1a9b8c8f6ffb3d63eb2de2017a9f7033187b340`;
the measured native compiler is
`a79d575982b541994a46e18df5a2e849effd0745fda73b04d23c19545b80ce98`.
Runtime bytes match between control and candidate.

Across four geometric helper workloads, the HTTP project and the complete
self-hosted compiler project, paired median ordinary-check ratios ranged from
0.984 to 1.037; ordinary-emission ratios ranged from 0.969 to 1.013. All measured
ordinary outputs were identical. This is no compilation-speed improvement claim.

Cold context medians were 5.224–14.153 ms for 32–1,024 repeated calls,
4.667–10.567 ms for 16–1,024 let continuations, and 5.205–10.606 ms for
16–1,024 nested expression elements. HTTP parser context took 9.042 ms;
`context.run` selected within the complete compiler project took 243.791 ms.
Selected-source and report-limit rejection medians were 4.768 and 23.371 ms.
These are dated observations, not portable absolute timing gates.

The geometric selected-scan assertion passed. Span-byte sums can grow roughly
quadratically before row limits; the nested-let fixture also has quadratic
indentation growth, so its size is not a linear source-byte proxy. Phase costs,
total node work, peak memory and allocation counts remain unknown. All raw
samples, input identities and geometric exponents belong in the evidence archive.

## Frozen feasibility pilot

The [protocol](../agent-development/PROTOCOL.md) froze three task pairs, six fresh
Sol 6.1 `xhigh` participants, ordinary public docs, task/reference/oracle material,
the production compiler and runtime before dispatch. The task oracle uses
independent finite Python arithmetic/list models and production-compiled SLIM
clients, exact targeted diagnostic spans with positive controls, and separately
labelled lexical source constraints. References establish feasibility only.

Each participant had 900 observed elapsed seconds and 24 wrapper operations;
native operations had a 30-second timeout. Trials ran serially without full
compiler/performance work overlapping them. Context access was permitted rather
than forced. No repair hints or retries replaced an outcome. Advisory isolation
on a shared filesystem is not strong blinding; unobserved bypass/leakage remains
unknown. Model tokens, actual model tool calls and repair iterations were not
independently captured.

All six trials submitted within the observed budgets. Corpus hashes matched
the preregistration after the last submission. Frozen results are:

| Trial | Condition | Frozen acceptance | Task oracle | Observed seconds | Wrapper operations | Context operations |
| --- | --- | --- | --- | ---: | ---: | ---: |
| Effects report | Baseline | Accepted | Pass | 204.694 | 9 | 0 |
| Effects report | Context | Constraint failure | Pass | 153.031 | 11 | 2 |
| Buffer drain | Context | Accepted | Pass | 160.921 | 11 | 1 |
| Buffer drain | Baseline | Accepted | Pass | 120.974 | 9 | 0 |
| Range API | Baseline | Correctness failure: export order | Fail: export order | 344.128 | 10 | 0 |
| Range API | Context | Accepted | Pass | 198.708 | 13 | 2 |

Frozen strict acceptance is 2/3 baseline and 2/3 context. Task-oracle acceptance
is 2/3 baseline and 3/3 context. Every candidate passed original application,
finite behavior matrix, targeted negative diagnostics with positive controls,
and the declared lexical source constraints. The observations do not show a
strict success-rate advantage for context; the contract difference in one pair
cannot establish general tool utility. Treatment participants used one or two
context requests and more wrapper operations than their paired controls.

The effects-context participant recovered from a concurrent-wrapper rejection,
but the frozen evaluator treats any rejection as a constraint failure. Its
participant prompt does not state that simultaneous wrapper requests are
forbidden, so this is a transport/prompt confound rather than evidence of an
incorrect final program. The wrapper restriction and failed outcome remain
unchanged in this pilot.

The range-baseline candidate violates the sorted-export requirement in the
supplied project documentation: `Limits Decision classify` replaces the required
`Decision Limits classify`. The task does not repeat that requirement, and the
production checker accepts the unsorted candidate. Its behavior checks pass;
the frozen contract failure remains supported by the public docs. The
checker/docs discrepancy and task salience are separate from behavioral
correctness. No post-submission correction earns a replacement success.

Static review traced the discrepancy to existing `selfhost/project.slim`:
`report_manifest_rules` checks module ordering, while import/export lists receive
shape/name checks and `append_interface_exports` preserves their supplied order.
The byte comparator itself orders `Decision` before `Limits` correctly. The file
is byte-identical to the working base (SHA-256
`62218fbbc20151de1bde872a7d7284154f73b30a6a55edf4e80dc84b2f28a5ba`).
Import/export uniqueness also lacks an explicit list validation in that static
path; this latter observation is not a newly executed negative test. Current
manifest conformance crosses module ordering, not these list boundaries. This
pre-existing checked-contract gap is a concrete follow-up, not silently repaired
inside the frozen experiment.

Observed elapsed time includes coordinator dispatch and submission-observation
latency. It is an upper-bound workflow observation, not model active time or a
reliable productivity estimate. Three pairs cannot establish a general success
rate or causal improvement.

## Verification and source scope

The working base is `7fbebd3fce266b845cf068c8fa27e0ebc0dcb024` plus this change
and eleven pre-existing dirty files. Their captured hashes remain unchanged.
The runtime clock patch and staged native-host experiments are included in
composite validation, not approved or closed by this agent-loop decision.

All eight required AGENTS.md commands passed on the final production code:
bootstrap, governance, Cargo tests, performance quick, reduction quick,
parallelism, comparison quick and agent evidence. Formatting and Clippy passed.
The initial restricted Cargo run failed three loopback tests on permissions;
the approved local-loopback rerun passed. A subsequent complete final Cargo run
passed all tests, including semantic-context conformance.

The first full `scripts/verify-0.9.sh` invocation used an isolated exact source
snapshot, temporary commit `5a61305f6e077a37295d0c46efa41c62b8a976d7`, preserving
the working tree without staging or committing unrelated files. It passed the
compiler, native-budget, sanitizer, session and cache checks preceding native
platform validation, then failed the native-host cold smoke. Apple Clang
21.0.0 (`clang-2100.3.34.2`, Darwin 27) emits
`-fdepfile-entry=CAPTURE/sdk/SDKSettings.json`; the existing strict driver
validator rejects this operand despite already capturing and hashing the file.
This does not arise from SLIM command dispatch. The failed log and source
inventory remain retained.

The integration repair admits only that exact captured SDK-settings file with
positive manifest membership, retains its argument in context identity and
keeps unknown switches and outside operands rejected. It restores the existing
[RFC-0146 captured-input contract](../../design/rfcs/0146-native-builds-in-compiler-sessions.md),
not a new provider or semantic fallback. The compiler seed, measured executable
and frozen pilot remain unchanged. Independent review found no blocking issue.
Targeted verification passed 88 driver cases (11 new SDK boundary/control
cases), public native cold/warm/edit/revert/reset smoke, and five captured-input
independence configurations with original SDK/tool roots removed. All eight
required commands passed again after the repair.

A second full invocation failed the unchanged owned-transfer normalized-check
budget: 1.853 against 1.300. Compiler, seed, runtime, benchmark source and budget
hashes matched earlier passing runs. A bounded investigation retained 250
balanced measurements and 50 paired checks of a tiny valid source. Current
owned/nested ratio was 0.98249 versus 0.96093 for the matched pre-context control;
all five diagnostic five-sample groups were below 1.300. Paired largest-input
current/control medians were 0.9894 for nested bindings and 1.0062 for owned
transfers, with matching child CPU observations. Large cold wall/CPU differences
support process-latency disturbance as a hypothesis; the failed run lacks child
telemetry, so its particular cause remains unknown. Invalid setup and distinct
arity-only startup controls are retained and never subtracted to invent phase
costs. One unchanged permanent-gate check then passed at 0.969, justifying a
fresh full retry. No metric, fixture, sample count or performance budget changed.

The third complete invocation on temporary snapshot commit
`bbafdbe77f1b9c9cb27b5edb67f5ac591ebccd24` passed every repository/compiler/native
gate and reproducible-package/installed-compiler verification, then failed the
website maintained-prose budget: 16,996 words against 16,000. The recorded
log-creation to last-output interval is approximately 35 minutes 37 seconds, not
a directly measured monotonic process duration. All 1,430 source identities and
modes, reconstruction payloads, and the failed log remain retained.

All complete release components passed with explicit source scope. Documentation
compression preserved contracts and restored the unchanged prose budget. Compiler acceptance comes from the
completed, hash-bound repository checks; documentation alone cannot supply it.
The remaining website and final-package components passed on the updated source
with explicit scope, without repeating unchanged native work. The corrected
package snapshot is `be4354ca5da2759417d351dcddb81069471b70b3`; all production
compiler/runtime/tests/budgets retain their validated source identities. The
first isolated website retry failed on a dependency symlink outside Turbopack's
root. Its log is retained; copying the same installed dependencies locally
repaired the test setup without a source/dependency/budget change.
The complete component set passes compiler/conformance/library checks, permanent native
budgets, sanitizer and allocation-fault cases, retained sessions, native platform
execution and capture independence, reproducible source packages, installed
context/stale-source smoke, and website verification. Closure documentation and
evidence indexing receive a final governance/website check after this snapshot;
production sources remain unchanged. The full third command retains its website
failure; component receipts record the successful repair validation. Composite validation does not create a
released repository commit or complete M2/M3.

The lossless [evidence bundle](archive/2026-10-02-agent-loop-evidence.json.gz)
retains source inventories and modes, patches/untracked source against the base,
frozen corpus and six participant prompts/candidates/results/ledgers, all raw
native samples, reviewed SDK jobs and targeted tests, failing attempts and final
gate logs. Verify its bytes with `python3 scripts/archive-results.py verify`.
The archive inventory is not a pass ledger.

## Next decision gate

First repair and independently cross the existing manifest import/export
canonical-order and uniqueness contract through the production compiler,
including mixed-case names, prefix ordering, duplicates and unsorted lists. Keep
the frozen pilot on its original toolchain and do not replace its outcomes.

Before expanding context or language surface, prepare a new protocol that
supports ordinary concurrent tool requests or states its serialization contract,
aligns checker, documented canonical public contracts and task requirements,
captures immutable per-operation candidate inputs and independently observable
dispatch/submission times, and uses larger held-out compiler/library tasks.
Freeze new tasks and independent acceptance before participants see them. Retain
this pilot unchanged and choose its successor budget before observing outcomes.

M2 source cutovers and the full M3 debugger/service milestone retain their
existing dependencies and remain pending. Broader context, sessions and syntax
need demonstrated task utility sufficient to justify their permanent costs.

## Operator task timing

The coordinator's earlier “90%” described deliverables, not remaining time, and
misleadingly suggested imminent completion. Three full attempts consumed over an
hour of inferred wall spans before documentation repair. Failed runs are not
completed-release duration samples. The initial 74-record operator snapshot is
retained in the [evidence bundle](archive/2026-10-02-agent-loop-evidence.json.gz);
the subsequent ten records remain in Git history and the local retained log.
Future coarse records stay locally in ignored `build/task-times.jsonl`, rather
than growing a tracked historical journal.
[`scripts/task-time.py`](../../scripts/task-time.py) records future command
durations with a monotonic clock; manual coordination spans remain wall-clock
observations. It does not infer ETA, model active time or native performance.
The AGENTS.md rule requires consulting comparable history and reporting elapsed
time, current stage and unfinished gates before offering duration estimates.
