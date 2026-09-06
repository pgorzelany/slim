# RFC-0112: Agent development and an OS foundation

Status: accepted
Implementation: pending
Process: 1
Audience: both
Author: Codex, at the project maintainer's request
Created: 2026-09-05
DecisionDate: 2026-09-05
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Propose a substantial successor to SLIM 0.9, provisionally called **SLIM Next**.
The objective is to reduce the time and cost for an agent to deliver a correct,
safe program meeting an explicit performance contract. Source token count is a
secondary measurement. An eventual operating system is the integration target.

The proposed identity is an expressive native systems language with a compiler
that exposes its semantic understanding, a reproducible debugging environment,
and versioned evidence about program changes. The language remains useful to a
human and builds without an LLM, model service, network, or agent framework.

Choose explicit ownership and checked borrows, ordinary generic libraries,
local type inference, conventional control flow, explicit resource authority,
and a small audited hardware boundary. Build a real incremental compiler service,
source debugging, property testing, fault simulation, and profiling around one
semantic authority. Defer broader automatic parallelization and equivalence
search until this development loop demonstrates value.

The maintainer approved this design and its full implementation on 2026-09-05.
This RFC accepts the direction and implementation sequence; its proposed surface
becomes an implemented contract only when the corresponding milestone passes.
Detailed language, runtime, process, and compatibility decisions are recorded in
child RFCs under this implementation authorization. New out-of-scope decisions
remain separate acceptance units. No performance budget is relaxed here.

The principal recommendation is a staged replacement, not a fresh compiler in
another language. Preserve the current compiler as a bootstrap and behavioral
reference, repair its demonstrated defects, and replace subsystems behind
measured boundaries. A source-breaking migration is justified; losing the test
corpus and starting with fewer trustworthy capabilities is not.

## Motivation

An agent's development cycle includes context retrieval, editing, compilation,
execution, interpretation of failures, and validation. Reducing compilation by
20 ms is less useful than removing a speculative edit and a full failed cycle.
All of these costs must be measured separately, together with completion rate.

SLIM 0.9 already provides valuable foundations: explicit effects, deterministic
output, checked arithmetic, a self-hosted compiler, a C seed, application
fixtures, and conservative bounded evidence. Its present abstraction and memory
boundaries make an expressive systems library difficult. Its tools also lack a
complete connection between an edit, actual incremental work, runtime behavior,
and a reproducible explanation of a failure.

The repository review at revision `97412bf` supplies immediate counterexamples.
These are review observations, not results of a new scan in this RFC:

| Observation | Evidence classification | Consequence |
| --- | --- | --- |
| A local affine vector assignment leaves the source usable; growing the destination and reading the source produces an AddressSanitizer heap-use-after-free. | exact, reproduced | Memory safety is not established by current conformance success. |
| Unconditional `recur`, direct recursion, and mutual recursion pass without `partial`. | exact, reproduced | The documented termination-effect ceiling is not enforced. |
| `session.run` computes snapshot changes and prints synthetic parse/lower/check/generate counts. | exact, inspected | Current counters do not establish actual incremental compilation. |
| Duplicate-declaration validation scans all earlier declarations. | exact, inspected | Default work includes a quadratic pass. |
| Consuming one owner in both exclusive branches is rejected. | exact, reproduced | Ownership state is not forked and merged correctly. |
| A by-value self-recursive struct passes checking and fails C compilation. | exact, reproduced | The frontend lacks the necessary layout-cycle rejection. |
| A valid cache truncated to 35 bytes traps during key comparison. | exact, reproduced | Cache corruption is not consistently a recoverable miss. |

The declaration probe measured medians of 18.65, 53.23, 188.50, and 653.85 ms
for 2,000, 4,000, 8,000, and 16,000 declarations. This is **bounded measurement**:
three runs at each size on the review host. It is not a portable timing budget.
The existing quick performance gate passed. The existing 181-fixture conformance
suite and 2,000 malformed-input mutations also passed. All 48 integration tests
passed across the initial run and reruns of three socket-permission failures.
Passing these suites did not establish the properties violated by the probes.

The earlier generics experiment in [RFC-0109](0109-bounded-parametric-generics.md)
is relevant contrary evidence: its compiler grew 13.6%, while compiler dogfooding
removed only 85 source bytes. This proposal does not reinterpret that experiment
as a success. It evaluates a complete library abstraction package against new
application requirements, after repairing collection ownership.

## Guide-level explanation

### The intended development experience

An agent opens a versioned compiler session. It retrieves the relevant public
contracts and semantic context, applies an ordinary source patch, and receives
updated diagnostics plus the affected declaration set. An incomplete file can
provide useful local information without becoming an executable program.

The agent runs a focused check or test. A failure records source/build identity,
inputs, and the captured boundary events needed to reproduce it. The tool can
minimize the input or event sequence while retaining the named failure. The
debugger reports source locations, values, and recorded state transitions.

After a correction, the agent sees which tests actually ran, which cached facts
were reused, which contracts changed, and what remains unknown. It profiles the
real executable and optimizes a measured cost against the same behavioral oracle.
It cannot obtain a correctness claim by editing the evidence text.

These proposed operations are tool roles, not committed command spellings:

```text
context(snapshot, expression, byte_budget)
explain(snapshot, diagnostic_or_fact)
apply_patch(expected_snapshot, edits)
check(snapshot, affected_scope)
test(build, test_ids, seed, event_budget)
replay(failure_artifact)
minimize(failure_artifact, failure_predicate, work_budget)
profile(build, workload)
compare(baseline_build, candidate_build, acceptance_contract)
```

### What stays, changes, and waits

| Keep | Replace or extend | Defer |
| --- | --- | --- |
| Static safety, no implicit numeric conversion | Region-dependent byte-view ownership | General refinement/dependent types |
| Left-to-right evaluation, checked default arithmetic | Eager Boolean guards | Unbounded solver work |
| Structs, enums, exhaustive matching | Type-specific library duplication | Macros and arbitrary compile-time execution |
| Deterministic compiler artifacts | Synthetic incremental counters | Broad automatic parallelization |
| Source-mapped bounded evidence | Broad ambient host access | General channels, async syntax, detached tasks |
| One production semantic authority and bootstrap proof | Opaque diagnostics and magic integer encodings | A new backend before measuring the C bottleneck |
| Permanent regression fixtures | Blanket bans on helpful elaboration | Self-modifying or LLM-dependent compiler semantics |

### Familiar expression with explicit ownership

Keep the indentation-based `fn`/`struct`/`enum` surface initially. There is no
evidence that another punctuation migration would improve the development loop.
Use `let` and `var`, infer local types when unambiguous, and keep public signatures
explicit. Adopt ordinary `while`, `break`, `continue`, and early `return`.
Short-circuit `&&` and `||`; admit ordinary `!=` and unary negation. These are
specified control and arithmetic operations, with checked lowering and source
locations, rather than hidden compiler heuristics.

Replace `@` and `^` with reference types and explicit moves across **all** owning
destinations. The design sketch below is **not accepted SLIM 0.9 syntax**:

```text
fn length[T](values: &Vec[T]) -> USize:
  vec.len(values)

fn consume[T](values: Vec[T]) -> Unit:
  # values is owned by this invocation and released on its normal exit
  ()

let next = move values
consume(move next)
```

An affine identifier used as an owning value requires `move`, including local
initializers, aggregate fields, collection insertion, and return. A freshly
produced temporary can flow directly into its destination. Read borrows use `&`,
exclusive borrows use `&mut`, and signatures name those types. There is no
implicit conversion between shared, exclusive, and owned parameters.

This intentionally trades some tokens for uniform transfer rules. Evaluate the
diagnostic and agent-repair consequences before freezing the spelling.

## Reference-level specification

The following is a target specification to split into child RFCs. It is not a
new accepted language contract.

### 1. Types and abstraction

- Nominal structs and enums; module-private fields and explicit public APIs.
- Generic functions, structs, and enums. Generic bodies are checked once
  against explicit requirements, not accepted only after convenient instantiation.
- A small interface mechanism: finite method sets, explicit implementations,
  unique resolution, no specialization, overlapping blanket implementations,
  implicit conversion, or open-ended trait search.
- Static callable parameters with an explicit context value support collection
  algorithms. Capturing closures and implicit heap environments wait.
- Monomorphized concrete code is the initial implementation. Repeated type
  combinations reuse one specialization; changing-type polymorphic recursion
  is rejected. Work and output limits fail with diagnostics, never type erasure
  or semantic fallback. Limits are part of the build configuration.
- `Unit` is a storable zero-information value, so `Result[Unit, E]` is ordinary
  library data. Replace `Void`; do not retain two unit types.
- `Result[T, E]` and `Option[T]` are source-library enums. Exhaustive matching
  works without a compiler-recognized error container. Start with explicit
  propagation; evaluate one propagation form only after measuring that burden.
- Fixed-width signed/unsigned integers, `USize` for target-sized indexing,
  fixed arrays, slices, and defined bit operations support systems code.
  Narrowing is explicit and fallible. Default arithmetic traps on invalid
  operations; named wrapping operations describe modular arithmetic explicitly.
  Checked-result arithmetic is available for recoverable input processing.
- Finite layouts are checked through a memoized dependency graph. Reject
  by-value cycles, overflowing layout calculations, and target-size excess.
  Layout/calling-convention annotations have one target-qualified contract.
- Newtypes preserve domain distinctions. Type parameters are not required to
  occupy storage, allowing typed IDs without erasing their domain.

Floating point is a later workload decision; any addition must name IEEE
behavior, optimization restrictions, and replay limitations. Dynamic dispatch
is an explicit later choice, not a fallback when specialization is expensive.

### 2. Ownership, borrows, and destruction

Every place has a definite initialization/move state. Branches start from the
same incoming state and merge conservatively. Loops check a fixed point or
reject when a documented checked work limit prevents establishing safety.
Missing evidence never enables an alias or shortens a lifetime.

`&T` gives shared read access. `&mut T` gives exclusive access. Borrow validity
includes the owner, the borrowed place, and the interval in which the borrow is
usable. A live element or slice borrow prevents operations that may invalidate
its storage. References cannot outlive backing storage. Public APIs returning
borrows express the relationship to input lifetimes; local relationships are
inferred. Begin with lexical borrow regions, then introduce more precise local
end points only when required by representative programs.

Scalars, references with suitable permissions, and explicitly declared
structurally copyable aggregates can copy. An exclusive reference is not
copyable. Owning storage is affine. Cloning storage is an explicit, potentially
fallible operation. General partial moves and self-referential movable structs
are initially rejected. An exclusive `replace` operation can transfer a field
or collection element while leaving a valid replacement.

`Bytes` becomes an immutable **owner**, produced by consuming an owned byte
buffer. A byte slice is a lifetime-checked borrow; static literals have static
backing storage. Borrowing or freezing does not require copying. This removes
the present need to keep arbitrary returned copyable byte views alive through
conservative caller/root regions.

Release ordinary owned storage at its defined destruction boundary on return,
branch exit, loop iteration exit, replacement, and explicit drop. Generate
reverse-order cleanup from ownership facts. Deallocation must be infallible and
nonblocking. Do not add arbitrary user destructors in the first implementation.
Cleanup is deterministic runtime work and must be included in cost reports.

An allocator is an explicit memory-domain capability. Containers retain a
checked relationship to its lifetime; a domain cannot be destroyed while its
owners or borrows remain usable. A process heap is supplied at the hosted entry
boundary, rather than discovered inside arbitrary functions. Region arenas
remain an explicit library allocation strategy. Recoverable insertion failure
returns the unconsumed value and preserves the container's defined state.
The allocator ABI and lifetime representation require a separate runtime RFC.

OS resources that require an effectful close use a later non-discardable resource
contract: every normal path transfers or explicitly closes the resource. A trap
is not disguised as a successful close. A supervisor or audited teardown path
owns resources when a component terminates abnormally. This contract must be
implemented and tested before claiming safe source ownership of such handles.

### 3. Effects, failure, and progress are separate facts

Use an initial operation-effect ceiling of `alloc`, `io`, and `block`.
Exclusive mutation is visible in the borrow signature. External nondeterminism
is described at the I/O boundary and in replay records. Public signatures name
effect ceilings; local inference cannot silently enlarge a published ceiling.

Remove the overloaded source `partial` permission in the successor. Purity
means absence of the declared external/allocation/blocking effects; it does
not imply termination or absence of traps. General recursion and loops are
allowed in ordinary safe code, with their totality reported conservatively.
This is an intentional semantic change from the 0.9 documentation.

`total`, `no_trap`, and bounded stack/work are independently requested contracts.
An exact proof can satisfy a static contract; unknown or exhausted proof budgets
cannot. Ordinary code does not become invalid merely because optional precision
changes. Contract checking may reject code whose required property is unknown.

Expected failures use results. Arithmetic/bounds bugs produce defined traps.
Allocation exhaustion becomes a result that a caller can recover from. Hosted
entry points may deliberately convert it to process failure. A kernel cannot
inherit the current unconditional exit-71 policy.

Services may run indefinitely. Verify individual transitions and handlers;
state liveness, fairness, device-completion, and scheduling assumptions separately.
A finite handler does not prove that a service eventually processes every request.

### 4. Authority and concurrency

An effect states what kind of action may occur. A capability value states which
resource and rights the caller possesses. Authority is supplied at entry and
passed explicitly. Source code cannot forge capabilities by converting an
integer. Hardware/service implementations establish the trusted boundary.

Use opaque library types to narrow rights and express state transitions. The
type checker does not infer which capabilities an application ought to have.
Process isolation and kernel mediation must enforce rights against untrusted
native code; a source type alone is not an OS security boundary.

Keep scoped tasks for independent work. Borrowed captures must remain valid
through join, mutable captures must be disjoint, and owned captures transfer
explicitly. Specify cancellation and cleanup before adding cancellable tasks.
Effectful task results may depend on external event order; lexical result
installation does not establish externally deterministic behavior.

Defer general shared-memory concurrency until an atomics and synchronization
contract exists. That contract must define memory order, data-race exclusion,
unsafe synchronization implementations, and supported targets. Race freedom
does not establish deadlock freedom, progress, or bounded latency.

### 5. Specifications and evidence

Properties are ordinary checked predicates with named input domains, or explicit
state-transition models. Prefer executable source models over a new specification
language. An independent simple reference implementation can serve as an oracle
for a performance-oriented implementation.

Testing and proof retain different scopes:

| Evidence | Required scope |
| --- | --- |
| exact | The precisely named complete domain and all assumptions; ordinary type safety is conditional on the language/runtime trust boundary. |
| bounded | Fixed input, event, schedule, search, or proof-work limits and whether the domain was exhausted. |
| unknown | A stable reason, unsupported operation, missing dependency, or exhausted budget. |

Also record **method** separately: static checking, exhaustive verification,
runtime enforcement, sampled testing, or measurement. Runtime enforcement of a
memory quota is not proof that a request succeeds within it. A passing fuzz run
is not an exact semantic proof. A proven bounded subdomain does not cover all
inputs. Keep witnesses and reachability checks to detect vacuous specifications.

## Compiler and runtime design

### A single semantic authority with useful derived views

Use a lossless source index for edits/comments, a canonical AST, a declaration
index, and typed identities for nodes, declarations, types, bindings, and places.
Source revisions own their ASTs. Links are meaningful within a revision; cross-edit
identity is re-established, never guessed from a stale byte offset.

Derive an ephemeral per-function control-flow view for move/borrow joins,
initialization, effects, and cleanup. It references canonical nodes and retained
types; it does not parse, retype, or independently decide source semantics. It is
not accepted as an input language or serialized executable IR. Bound expensive
dataflow work and fail safely when mandatory checks cannot complete.

Make checking ownership and effects one reusable service. Debugging, generation,
simulation builds, and optional analysis consume its checked result. Replace
packed integers with nominal compiler types as the successor can express them.
Specialized generated instances link back to the checked generic declaration.

### Actual incremental computation

Memoize declaration queries with tracked dependencies and explicit fingerprints
for bodies, interfaces, layouts, effect ceilings, target, and compiler version.
Publish a new snapshot atomically only after its required checks succeed. Keep
the last good executable separate from an incomplete editor snapshot.

Instrument query execution, invalidation, cache hits, C emission, and backend
compilation at their actual entry points. Tests compare an incremental update
with a clean build after body edits, interface edits, deletions, renames,
reordering, configuration changes, and failed updates. A body change can also
invalidate optimized callers that consumed its body-derived facts; interface
stability alone is insufficient for optimized-code reuse.

Cache entries include compiler/runtime/target/options and source identities.
Validate framing before content access. Corruption is a miss. Shared or imported
executable caches require an explicit trust policy; a checksum is not authority
to accept hostile native code. Use content-addressed local artifacts and avoid
arbitrary executable build scripts in the initial package model.

### Backend and build modes

Retain C initially, with deterministic dependency-aware translation-unit/object
reuse. Measure preprocessing, C compilation, optimization, and linking separately.
Keep a low-optimization debug mode with source mappings and an optimized mode.
Both use the same checked semantics, traps, and required cleanup. No mode disables
safety checks without an exact checked justification.

Investigate a fast native backend only if external C compilation still dominates
two representative warm iteration workloads after caching. A new backend needs
its own accepted architecture/dependency RFC and complete production conformance;
it cannot silently delegate unsupported source to a different semantic compiler.
The C path is retained as an explicit reference during any authorized transition.

### Compiler service and structured editing

Expose versioned request/response schemas over a local process transport. Provide
diagnostics, expected types, ownership state, symbol search, references, relevant
contracts, impacted declarations, and explanation paths. Every response names its
snapshot, completeness, and fixed output/work budget. Unknown fields follow a
documented version policy; unknown meanings are never guessed.

Edits are normal source changes with snapshot preconditions. Stale edits fail
atomically. A structural edit is non-executable tooling data and still requires
the normal checker. LSP and agent clients share this service. An agent's narrative
or requested patch cannot authorize a source invariant or prove a result.

### Debugging, replay, and minimization

Start with source-mapped breakpoints, stack/local inspection, trap operands,
allocation identities, and events for important resource transitions. Emit native
debug information; preserve inline/source provenance where the backend can.
Mark optimized-away values unavailable. Instrumented replay can supply additional
evidence, but is a separately identified build and may perturb behavior.

A failure artifact includes source/build/configuration identities, seed, inputs,
captured events, environment-provider versions, failure predicate, and completeness.
Record time, randomness, I/O results, and scheduler decisions at controlled
boundaries. Set byte/event limits; overflow makes the recording incomplete rather
than silently claiming reproducibility. Do not promise replay of arbitrary
unrecorded hardware behavior or weak-memory executions.

Minimization runs within an explicit work budget and preserves the named failure
predicate. The result is a smaller reproduced witness, not necessarily a globally
minimal input. Both witness and original recording remain available.

### Deterministic component simulation

Define typed providers for clock, storage, network, allocation, and scheduling.
Native and simulated providers satisfy the same interface and documented failure
model. The same component source and checker run in both environments. Provider
substitution occurs at a declared composition boundary, not by intercepting
unrelated calls or adding an interpreter for alternate SLIM semantics.

Begin with single-threaded event-driven components. Inject latency, exhaustion,
partial operation results, disconnection, cancellation where supported, and
storage faults from an explicit model. A storage provider must distinguish
acknowledged writes, durable writes, flush ordering, and allowed crash outcomes.
Replay covers recorded/modelled behavior. Emulator and real-hardware tests are
separate required evidence; simulation is not a hardware proof.

### Evidence attached to changes

An evidence manifest names the exact source, compiler, runtime, target, flags,
dependencies, acceptance-contract revision, test IDs, runs, reused facts, and
unverified scope. The checker recomputes proofs from source; report text is not
trusted. Distinguish affected tests from tests actually executed. Dependency-based
selection cannot claim untracked integration or external behavior is covered.

Track specification, test, and performance-budget changes explicitly. A repair
must not appear successful solely because it also weakened its acceptance
contract. Separate those changes in the report, without inventing a universal
quality score or making approval prompts part of ordinary language execution.

### OS architecture target

Target a small capability-oriented kernel with isolated services as the research
architecture. This makes authority, failure containment, and replaceable components
observable. IPC and isolation overhead are measured, not assumed negligible.

Start with hosted core components, then one freestanding QEMU target:
RISC-V 64-bit, single core, serial console, page allocation, address spaces,
traps, and a minimal capability IPC path. RISC-V is a scope choice, not a claim
of superior performance. A separate bare-metal entry contract replaces hosted
`main`, supplies memory explicitly, and requires no libc, filesystem, threads,
environment variables, or POSIX allocation/shutdown services.

Use a small audited machine layer for assembly, MMIO, interrupts, page tables,
and architectural barriers. Raw addresses and unchecked operations are confined
to explicitly unsafe modules/functions with stated obligations. Safe callers
receive checked abstractions. Volatile access is not synchronization. DMA and
device ownership require stable/pinned memory and explicit completion/coherency
rules before safe APIs are exposed. Defer SMP until the atomic memory model,
interrupt interaction, and synchronization library have their own gates.

Source capability checks do not contain arbitrary machine code; page protection,
kernel object validation, and revocation enforce the OS boundary. A service trap
can terminate/restart that service according to policy. Kernel paths require
stronger contracts or explicit fatal handling; arbitrary fault recovery is not
provided by stack unwinding through partially updated kernel state.

## Compatibility and migration

SLIM Next is a breaking experimental successor, not a patch to 0.9. A future
accepting compatibility RFC selects its version, ABI, schemas, and migration.
Keep 0.9 documents accurate about the implemented compiler while successor implementation is pending.

The following conflicts must be resolved explicitly before dependent work:

| Existing decision | Proposed change | Required decision boundary |
| --- | --- | --- |
| Blanket sugar/alias prohibition and nonnegative feature-rating gates | Admit specified elaboration when total development cost improves; publish real tradeoffs | Process RFC updating FEATURE_POLICY and AGENTS; do not manipulate ratings to pass |
| RFC-0110 default shared parameters and `@`/`^` | Explicit reference types and uniform owning moves | Ownership/syntax RFC, negative tests, whole-corpus migration |
| Region-backed copyable `Bytes`, ABI 1, exit-71 exhaustion | Owned bytes, lifetime-checked slices, explicit memory domains, recoverable allocation | Memory/runtime RFC and ABI migration |
| `partial`, eager Boolean operators, `recur`, unstorable `Void` | Separate progress contracts; short-circuit guards; ordinary loops; storable Unit | Control/effect/type RFCs and semantic migration |
| Rejected RFC-0109 generics | Generic library abstraction with borrowing, visibility, and callable requirements | Fresh RFC and application evidence; rejection remains historical evidence |
| Whole-operation host boundary | Resource capabilities and a privileged machine layer | Host/unsafe/target RFCs with explicit trust assumptions |
| One canonical parsed representation | Derived typed control-flow view with no input syntax or second checker | Architecture RFC defining invariants; no separately parsed IR |
| Self-hosted production authority | Preserve it during staged replacement | No production Rust exception requested |
| Permanent performance/parallel baselines | Preserve fixtures, measured history, and semantic intent through migration | Any relaxed budget needs its existing +60 decision and quantified impact |

Migration is an explicit offline tool and never a second parser in the final
successor compiler. It must reject ambiguous ownership/lifetime conversions.
Old eager Boolean expressions with observable operands require explicit
left-to-right temporary evaluation to preserve behavior under short-circuiting.
New byte ownership cannot be established by blindly renaming the old view type.
Trap, output, move, cleanup, and failure changes are documented individually.

Keep the old compiler revision/seed and old/new fixtures through cutover. Compare
program behavior for the preserved domain; record intentional semantic changes.
Preserve all discovered safety regressions as permanent successor tests even if
their exact syntax changes. Do not delete a failing gate to make migration pass.

## Diagnostics and failure cases

Required diagnostic families include use-after-move, conflicting loans, escaping
borrows, invalid branch joins, unsafe field moves, allocator-lifetime escape,
unconsumed resources, recursive layouts, unsupported generic requirements,
specialization budgets, effect-ceiling violations, unproved mandatory contracts,
stale edit snapshots, corrupt cache misses, incomplete recordings, and unsupported
target operations. Allocate stable codes in the child implementations, not here.

Report the operation, violated rule, relevant origin/use spans, and available
repair information. Offer an automatic fix only when applicability is established;
otherwise explain alternatives without claiming one is behavior-preserving.
Retain independent errors within a named report bound. Never emit an executable
from malformed or partly checked source.

A sample desired debug result, not an existing schema:

```text
failure: bounds violation
source: revision + declaration + expression span
operands: index=8, length=8
provenance: recorded index update at <span>; buffer length observed at <span>
replay: artifact identity; complete for this provider/event model
evidence: reproduced witness; other inputs and schedules unexamined
```

## Performance and complexity

### Cost model

Measure context retrieval, cold checking, warm incremental checking, debug build,
optimized build, execution, failed repair cycles, and final validation separately.
Record p50/p95 latency, memory, source/generated size, and actual query work.
Use a persistent service to remove process startup where applicable, but also
retain cold-start and command-line measurements.

Target approximately O(source + dependency edges) ordinary frontend work over
supported shapes. Count generic instances and generated output explicitly:
monomorphization is not generally O(original source bytes). Checked hard work and
output limits contain adversarial inference, dataflow, and specialization;
exhaustion is a diagnostic. Bounds do not justify silently accepting unverified
code. Global invalidation may be necessary after a widely used interface change.

### Initial research targets, not accepted budgets

Calibrate on a named reference host and commit the workload before setting gates.
These targets are hypotheses for the roadmap, not measured capabilities:

| Operation | Initial target |
| --- | --- |
| Warm semantic-context query | p95 at most 20 ms for a bounded local response |
| Warm private body edit | p95 at most 50 ms on a 1 MiB project with 2,000 declarations |
| Warm edit affecting a small interface closure | p95 at most 150 ms on that project; report closure size |
| Clean check | at most 250 ms for that 1 MiB workload; retain geometric larger/adversarial series |
| Warm debug edit to runnable test executable | p95 at most 500 ms on that project |
| Replay of a minimized component failure | at most 1 second for the committed small-event corpus |
| Native efficiency | aim within 1.25x matched C runtime on the initial OS-component corpus; publish every outlier and preserve existing per-program gates |

The C comparison is conditional on target, workload, algorithm, layout, and
equivalent observable inputs/outputs. Also compare against a checked C reference
and Rust to expose safety costs. Do not disable SLIM checks to match unchecked C.
Allocation count, peak retained memory, binary size, and tail latency remain
independent measurements. No aggregate may hide a failing existing budget.

## Alternatives and drawbacks

### Keep the language and add only agent tools

This is the lowest-risk alternative and should be the first empirical baseline.
It may capture much of the benefit. It leaves region retention, library
abstraction, resource handles, and kernel representation needs unresolved.
If tools on repaired 0.9 outperform the successor on the relevant tasks, retain
the tools and reject unnecessary successor surface.

### A complete rewrite in Rust or C

This would make some compiler implementation techniques easier immediately, but
abandons the production self-host boundary and creates migration/trust costs.
It is not selected. Reconsider only through a separate architecture RFC with a
bounded bootstrap plan and measurements showing the staged approach is failing.

### Reuse an established language and build the toolchain around it

This is the strongest product alternative, not a straw man. Many proposed agent
benefits are tooling features. Compare against an established-language workflow
with comparable tooling; syntax novelty alone cannot justify SLIM. A new language
earns its cost only if ownership, effects, capabilities, and evidence integration
produce a material end-to-end advantage or enable a required OS contract.

### Start with a kernel or a new native backend

Either may produce an impressive demonstration while hiding basic development
friction. Neither is the first milestone. Hosted component correctness,
reproducibility, and compiler reuse can be established before boot machinery.
A backend changes only part of the iteration cost.

### Costs of the chosen direction

References and lifetime relationships increase checker complexity. Generics
increase compile work and binary size. Exact cleanup adds runtime work compared
with bulk region destruction. Capability composition can add API arguments.
Simulation requires explicit environment models. Debug instrumentation has
overhead and observes sensitive program data, so recording is explicit and
bounded. A small isolated-service OS may pay IPC costs. These costs must be
measured rather than assigned optimistic feature scores.

## Test and acceptance plan

### Roadmap and dependency order

Milestones are gate-driven, not calendar promises. The core sequence is
M0 -> M1 -> M2 -> M3 -> M4 -> M5 -> M6 -> M7. Evaluation infrastructure begins
at M0 and continues throughout. Each milestone can be split into small accepted
RFCs and independently reviewable implementation changes.

| Milestone | Deliverable | Exit evidence |
| --- | --- | --- |
| M0: repair and establish truth | Restore current ownership/effect/layout/cache contracts; correct incremental claims; instrument scaling; approve only the policy changes needed for subsequent experiments | Every review reproducer becomes a permanent regression; ASan/UBSan and fault injection; actual-work counters; clean bootstrap and existing full release gates |
| M1: compiler substrate | Typed source identities, reusable declaration queries, branch/loop ownership foundation, actual incremental checking and cached emission, corrupt-cache recovery | Clean/incremental differential tests through edits and failures; no-change reuse; geometric work/latency; source-span stability; byte-deterministic artifacts |
| M2: expressive safe core | Uniform moves, references/slices, deterministic storage cleanup, recoverable allocation, opaque data, Unit, inference/control-flow migration, then constrained generics/callables | Accepted child specs; migrated compiler/library corpus; move/borrow cross-product tests; memory and code-size budgets; two substantial reusable-library applications |
| M3: agent and debugger interface | Persistent semantic service, bounded context/explanations, source traps and native debug maps, edit/evidence manifests | Stale-snapshot rejection; incomplete-source queries without executable acceptance; debugger source/value tests; actual query latency; first paired agent tasks |
| M4: reproducible component laboratory | Executable properties, generated inputs and shrinking, deterministic providers, failure replay and event minimization | Native/simulated provider conformance; exact replay within model; recording-limit tests; injected allocation/storage failures; held-out component failures repaired by agents |
| M5: systems resource contracts | Resource capabilities, explicit close/transfer rules, no-alloc/no-block contracts, fixed-capacity containers, profiling and cost reports | Rights/lifetime/cleanup negative tests; exhaustion recovery; no allocator calls in no-alloc regions; blocking call-path rejection; maintained native performance fixtures |
| M6: freestanding kernel substrate | One RISC-V/QEMU target, audited machine layer, boot/traps/pages/address spaces/capability IPC, isolated user service | No hosted-runtime dependency; boot and ABI checks; invalid capability/access rejection; fault containment; repeatable emulator scenarios; stated trusted boundary |
| M7: end-to-end OS research slice | The same maintained storage service in the simulator and OS; crash recovery, bounded resources, agent-driven repair and optimization | Cross-environment behavioral evidence, fault/replay corpus, per-workload C/Rust comparisons where available, independent evaluation of agent outcomes |

M2 is a package of dependent experiments, not one enormous compiler commit.
Borrowing and private representation must precede generic container APIs.
Each slice stays executable through the production compiler. M4 begins with
event-driven single-threaded components; it does not wait for SMP or a general
concurrency runtime. The OS never becomes the only place library code can run.

### First implementation sequence

1. Preserve the local-move/reallocation witness and fix all owning destination
   paths, then branch joins; include aggregate, assignment, return, and collection
   variants rather than one special-case patch.
2. Restore the documented 0.9 recurrence/effect rule; reject impossible layouts;
   make cache truncation and all framing failures recover as misses.
3. Replace the quadratic duplicate-name scan; extend the permanent adversarial
   declaration series. Correct docs to distinguish invalidation estimates from
   executed incremental compilation.
4. Establish instrumentation for real checker/generator/cache calls and clean
   versus incremental behavioral comparisons.
5. Accept the small process/architecture decisions for the successor; introduce
   typed identities and a derived control-flow view while preserving 0.9 behavior.
6. Specify and test the successor ownership/lifetime/allocator contract before
   changing source syntax or byte representation.

Steps 1-4 are current-contract repairs and evidence work, not contingent on
accepting all of SLIM Next. Before committing compiler/runtime/benchmark/tool
changes, run the required bootstrap, governance, Cargo, performance, reduction,
parallelism, comparison, and agent gates, plus the full release gate at milestone
closure. Any discovered baseline failure remains recorded until resolved.

### The proving application

Build a bounded key/value storage service with a simple independent state model,
explicit request and response messages, memory and queue limits, typed storage
errors, and a documented durability/flush model. Inputs are deliberately small
enough to minimize and replay. It should recover from modeled interrupted writes
without violating the specified acknowledged-operation contract.

Use the same component source in a hosted native test runner, deterministic
simulator, and eventually an isolated service on the research OS. Do not promise
identical device behavior merely because the source is shared. Validate provider
assumptions separately. A codec/parser and a fixed-capacity allocator/queue supply
additional workloads so one service cannot define every language decision.

### Evaluate whether an LLM actually benefits

Pre-register development, repair, optimization, and API-migration tasks with
independent acceptance oracles. Include unseen boundary conditions and preserve
the test/specification revision separately from the agent's patch. Do not count
compilation or self-authored passing tests alone as task success.

Pilot a fixed small task set, then freeze a larger held-out evaluation. Compare
repaired SLIM 0.9, SLIM Next, and established-language baselines with equivalent
library functionality. Include a baseline with comparable tooling so language
and toolchain gains can be separated. Use multiple model families, paired tasks,
fixed documented budgets, repeated trials, and versioned prompts/tool access.
Run ablations for semantic context, replay, and causal explanations.

Report completion rate, correctness failures, time-to-accepted-result, model
tokens, tool calls, repair iterations, runtime/resource quality, and human
intervention separately. Count timeouts and failures; do not compare only
successful runs. Keep native benchmarks independent. Model access and execution
costs are a future explicitly budgeted experiment, not authorized expenditure
by this proposal.

The adoption hypothesis is a material reduction in time/cost to correct programs
at comparable native/resource quality. Choose the numerical decision threshold
after a variance-estimation pilot and before held-out runs. No current evidence
establishes that SLIM improves LLM success rate.

### Stop, narrow, and reconsider conditions

- If semantic tools supply the advantage without new syntax, retain those tools
  and narrow the language migration.
- If generic libraries do not remove substantial real application complexity,
  or specialization exceeds declared budgets, shrink the abstraction proposal.
- If prompt reclamation and borrowing cannot satisfy the tested safety contract,
  do not progress to OS resource ownership.
- If native C compilation meets the iteration target, do not add a backend.
- If replay depends on events outside the provider model, expand the model or
  label the case unsupported; do not call it reproducible.
- If a hardware/unsafe boundary has unrecorded obligations, do not describe the
  resulting OS configuration as memory-safe or verified end to end.
- If an established-language workflow performs as well with less total cost,
  reconsider whether a new language is the appropriate product.

## Ratings and evidence

All ratings are zero because this umbrella proposal has no measured
implementation benefit and introduces no accepted primitive. Zero is a neutral
placeholder, not a negative quality finding and not an assertion of no cost.
The current arithmetic is `(0*20 + 0*20 + 0*20 + 0*20 + 0*15 + 0*5) / 2 = 0`.
Child RFCs must publish actual tradeoffs and evidence. A future process decision
should treat ratings as review aids rather than proof that every dimension can
improve simultaneously.

Implementation observations above have their stated exact/bounded classifications.
The proposed latency, native-performance, productivity, and OS-safety outcomes
are **unknown** until their named acceptance evidence exists. No timing target,
architectural sketch, model-generated explanation, or roadmap label is proof.

Relevant prior art supports mechanisms, not a novelty or SLIM-success claim:

- [Rust structured diagnostics](https://doc.rust-lang.org/rustc/json.html) show
  machine-readable spans and suggestion applicability.
- [rr](https://rr-project.org/) demonstrates record/replay and reverse debugging
  within its supported execution environment.
- [TigerBeetle architecture](https://github.com/tigerbeetle/tigerbeetle/blob/main/docs/ARCHITECTURE.md)
  describes deterministic simulation of real implementation code under faults.
- [Kani usage](https://model-checking.github.io/kani/usage.html) describes concrete
  playback of counterexamples as executable tests.
- [seL4 proof assumptions](https://sel4.systems/Verification/assumptions.html)
  illustrate the need to enumerate hardware and low-level trust boundaries.

## Decision

Accepted by the project maintainer on 2026-09-05: "approved, set a goal to
implement the full slim next. try to measure if it really increases you speed
and effectivenes." The implementation goal covers M0-M7 and empirical evaluation.
Implementation details within this design are authorized; child RFCs document
their contracts and evidence without claiming completed implementation early.

## Implementation

In progress; the machine-readable implementation state remains pending until
M0-M7 acceptance evidence is complete. Execution and measurement status are
tracked in `benchmarks/results/2026-09-05-slim-next-progress.md`. Current source,
runtime, and tooling contracts remain authoritative until their documented
successor migrations complete. No milestone is completed by approval alone.

## Removal and supersession

Withdraw or revise this proposal if controlled evaluation does not support its
development benefits, if a simpler toolchain achieves them, or if its costs exceed
the declared resource and complexity budgets. Preserve contrary experiments and
regression fixtures. If accepted in stages, each child RFC names exactly which
existing decision it replaces. Do not silently repurpose 0.9 syntax, reports,
runtime ABI, historical benchmark evidence, or completed milestone claims.
