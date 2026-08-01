# RFC-0110: Default-shared ownership modes

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: SLIM project
Created: 2026-07-29
DecisionDate: 2026-07-29
Approver: project-maintainer
Kind: language
Primitive: default-shared-ownership-modes
Safety: 2
Compile: 0
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 50

## Summary

Affine parameters become shared, read-only, nonescaping borrows by default.
`@` marks an exclusive read-write borrow and `^` marks ownership transfer
across a call, recurrence, or consuming built-in boundary. The same explicit
marker is required on an exclusive or owned call argument:

```slim
fn length(values: Vec[I64]) -> I64:
  vec.len(values)

fn append(value: I64, values: @Vec[I64]) -> Void effects[alloc]:
  vec.push(@values, value)

fn freeze(bytes: ^Vec[U8]) -> Bytes:
  bytes.freeze(^bytes)

length(values)
append(42, @values)
freeze(^bytes)
```

Plain copyable parameters retain ordinary by-value copying. A plain affine
parameter grants only shared read access for the lexical call. An `@`
parameter grants exclusive read-write access for the lexical call while the
caller retains ownership. A `^` parameter receives the whole affine owner and
invalidates a named caller binding. No form creates a storable reference.

The existing `inout` spelling is removed atomically and is not retained as an
alias. This RFC changes parameter and call ownership semantics; it does not add
general references, lifetime syntax, raw pointers, reference counting, or
garbage collection.

## Motivation

The current unqualified affine parameter consumes its argument even when the
callee only reads it. Avoiding that transfer requires `inout`, which grants
exclusive mutation whether or not the function mutates. This makes the most
powerful temporary capability shorter than shared access, marks read-only
functions as exclusive parallelism blockers, and makes ownership loss implicit
at call sites.

The self-hosted compiler threads many vectors through read-only queries and
uses `inout` by immutable convention at some boundaries. The current
twenty-application parallelism corpus reports 60 exclusive-parameter primary
reasons. That count alone does not justify a feature, but it establishes a
measurable application baseline for the experiment.

SLIM should make the least authority the shortest form:

- ordinary access is shared and read-only;
- mutation requires visible `@`; and
- ownership loss across a call boundary requires visible `^`.

Ownership transfer remains necessary when a callee stores, returns, freezes,
forwards, or destroys an affine owner. Making that boundary explicit preserves
zero-copy deterministic ownership without making every read consume its
source. Ordinary affine moves into a visible owning destination remain the
existing unmarked move operation.

## Guide-level explanation

Copyable values and immutable views continue to pass normally:

```slim
fn add(left: I64, right: I64) -> I64:
  left + right
```

An affine parameter without a mode is a shared borrow:

```slim
fn first(values: Vec[I64]) -> I64 effects[partial]:
  vec.get(values, 0)

let value: I64 = first(values)
let again: I64 = first(values)
```

The callee may read `values`, pass it to another shared parameter, and use
read-only collection operations. It may not mutate, move, return, or store the
borrow. Multiple shared arguments may name the same owner.

Exclusive mutation uses `@` in the declaration and call:

```slim
fn clear_first(values: @Vec[I64]) -> Void effects[partial]:
  vec.set(@values, 0, 0)

clear_first(@values)
```

The caller retains ownership after the call. The exclusive argument must be a
named binding. It cannot overlap another shared or exclusive argument in the
same call. An exclusive parameter may be reborrowed as shared or exclusive,
but it cannot be moved.

Ownership transfer uses `^`:

```slim
fn into_bytes(values: ^Vec[U8]) -> Bytes:
  bytes.freeze(^values)

let frozen: Bytes = into_bytes(^values)
```

After the call, the caller cannot use `values`. The callee owns its parameter
and may store, return, forward, mutate, or discard it. Recursive forwarding of
an owned parameter also writes `^`.

`^` accepts a whole named owner or a freshly produced affine owner such as
`^vec.new()`. It is not written on ordinary moves into a local binding,
aggregate, enum payload, or owning collection slot. A field projection such as
`^owner.values` and a branch-selected existing owner are rejected: the current
language does not track partial or conditional moves of an existing owner.

The three modes are parameter capabilities, not reference types. `@Vec[I64]`
and `^Vec[I64]` are valid only in parameter type position. `@value` and
`^value` are valid only as direct call, recurrence, or consuming built-in
arguments. They cannot be stored, returned as markers, nested in arbitrary
expressions, or used as type arguments or fields.

## Reference-level specification

The parameter and application grammar becomes:

```text
parameter      = "var"? identifier ":" parameter-mode? type
parameter-mode = "@" | "^"
argument       = argument-mode? expression
argument-mode  = "@" | "^"
```

`var` retains its existing binding-reassignment role. `@` and `^` describe
ownership capability. A parameter may not combine `var` with `@`; an owned
parameter may use `var` only when rebinding is otherwise valid. The formatter
emits the mode immediately before the type and immediately before the argument.

For a copyable parameter, the mode must be absent and the value is copied.
For an affine parameter:

- no mode means a shared, read-only, nonescaping borrow;
- `@` means an exclusive, read-write, nonescaping borrow; and
- `^` means owned transfer.

Calls require an exact mode match. A shared or copyable parameter receives an
unmarked argument. An exclusive parameter receives `@name`. An owned affine
parameter receives `^expression`. `@` requires a named binding. `^` may consume
a whole named owner or transfer a freshly produced affine value. A projection,
borrow-derived value, or control-flow selection among existing owners is not a
whole-owner transfer source.

The `^` marker is scoped to calls, `recur`, and consuming built-ins. Existing
affine moves into a local binding, record field, enum payload, or owning
collection element remain unmarked. Those constructs already expose the new
owner as their destination, so adding `^` there would create a second spelling
for the same move.

Shared borrows may alias each other. An exclusive or owned argument may not
overlap any other shared, exclusive, or owned argument in the same call or
recurrence. Checks compare canonical root-owner identity, so a differently
named local or pattern alias cannot hide overlap. Argument evaluation remains
left to right. The checker reports the first deterministic overlapping
ownership conflict in lexical argument order.

Borrowed parameters cannot escape, be moved, or be returned as owned storage.
That provenance is preserved through local aliases, assignments, projections,
control-flow joins, enum payload patterns, and affine collection reads. Shared
parameters cannot be mutated. Exclusive parameters retain the current checked
unique-mutation capability and nonescape rules. Owned parameters retain the
current plain affine-parameter behavior after the explicit transfer.

Read-only collection built-ins take shared operands. `vec.push`, `vec.set`,
`arena.add`, file output, and network output take exclusive operands marked
with `@`. `bytes.freeze` consumes an owned vector marked with `^`. Other
built-ins retain their current copy, effect, trap, and allocation contracts.

The borrow operator set is closed at shared and exclusive access. Field or
slice precision may later extend the set of accepted places, but uses the same
shared or exclusive mode. Escaping references, output-only references,
pinning, atomics, weak references, and raw pointers are separate features and
are not implied by this RFC.

## Compiler and runtime design

Canonical parsed SLIM retains one fixed argument-mode fact on the exact
argument root. No separately parsed ownership IR is introduced. Parameter mode
queries and linked argument checks consume the same stable nodes already used
for type, effect, ownership, memory, analysis, and code generation.

The lexer adds fixed `@` and `^` tokens. Parsing, mode validation, alias checks,
and ownership transitions remain linear in source size plus the existing
bounded call and binding work. Canonical local links provide constant-time
binding and root-owner lookup. Per-call access epochs enforce overlap during
the typed argument walk; there is no second whole-program ownership scan. No
whole-program ownership inference is added. A function body never determines
its public parameter mode.

Shared affine parameters lower initially to the existing direct aggregate
representation with mutation and escape forbidden by the checker. Exclusive
parameters retain the existing pointer representation and checked no-alias
contract. Owned parameters retain the existing by-value affine
representation. The experiment compares that direct shared lowering with a
read-only pointer lowering only if measured applications demonstrate a need;
there is no runtime mode object.

The bootstrap uses a bounded transition compiler. It first accepts both the
old declaration spelling and new modes while preserving old semantics, then
compiles the atomically migrated source and final ownership checker. The final
compiler, portable seed, formatter, and tests reject `inout` and contain no
transition reader or legacy mode.

## Compatibility and migration

This is a pre-1.0 breaking source change. Every source file migrates atomically:

- `inout name: T` becomes `name: @T`;
- every call to that parameter adds `@`;
- every plain affine parameter that needs ownership becomes `name: ^T`;
- every call transferring ownership to it adds `^`; and
- plain affine parameters that only read remain unmarked.

The formatter emits only the new form. `inout` is rejected after the cutover,
and no compatibility alias, implicit argument marker, or legacy parser remains.
The runtime ABI version remains unchanged unless measured lowering evidence
requires an ABI-visible representation change. Project interface and cache
schemas advance because exported parameter modes change.

## Diagnostics and failure cases

Existing ownership diagnostics are generalized:

- `E0347` rejects moving, storing, or returning a shared or exclusive borrow,
  and rejects a `^` source that is not a whole named or freshly produced owner;
- `E0348` rejects a temporary exclusive borrow;
- `E0349` rejects overlapping restricted arguments in calls and recurrence; and
- `E0315` continues to reject use after an owned transfer.

`E0360` reports a missing, unexpected, or mismatched `@` or `^` argument mode.
Its primary span is the exact argument. Notes identify the declared parameter
mode. The formatter never repairs an ownership-mode mismatch.

Negative tests cover missing and extra user and built-in markers, `@` on a
temporary, `^` from a borrow or projection, mutation through a shared
parameter, direct and derived borrow escape, assignment and storage escape,
control-flow and pattern provenance, differently named root aliases, every
shared/exclusive/owned overlap combination, recursive owned overlap, recursive
mode mismatch, copyable `^`, removed `inout`, and invalid marker placement.

## Performance and complexity

The clean baseline on 2026-07-29 is:

| Measure | Baseline |
| --- | ---: |
| Self-hosted SLIM source | 751,413 bytes |
| Self-hosted SLIM lines | 17,454 |
| Fixed-point compiler C | 2,743,494 bytes |
| 1,000-declaration check | 13,014 microseconds |
| 1,000-declaration emit | 21,284 microseconds |
| 1,000 inout-read emit | 13,004 microseconds |
| 1,000 owned-transfer check | 8,192 microseconds |
| Application exclusive-parameter primary reasons | 60 |

Acceptance requires before/after source bytes, lexical tokens, model-token
proxy, lines, fixed-point C bytes, native compiler bytes, clean check and emit
times, geometric frontend exponents, application blocker sets, generated C,
binary sizes, and same-host runtime ratios. Results are reported independently;
there is no aggregate readability or quality score.

Default checking and emission must remain approximately linear. A reproducible
frontend or native-runtime regression outside the recorded noise band rejects
the candidate. Runtime budgets are not relaxed. Programs without affine
parameters receive no generated runtime mechanism.

Readability evaluation reports exact marker counts and representative
before/after compiler functions. It separately records removed `inout` tokens,
added `@` and `^` tokens, parameter-mode diagnostics, and ownership-sensitive
lines rather than claiming that fewer bytes alone prove readability.

### Measured result

Measurements were repeated on the same host after warmup. Timed old/new
comparisons alternate the two compilers; portable gates retain their geometric
budgets.

| Measure | Baseline | RFC-0110 | Change |
| --- | ---: | ---: | ---: |
| Self-hosted SLIM source | 751,413 bytes | 758,582 bytes | +0.954% |
| Self-hosted SLIM lines | 17,454 | 17,808 | +2.028% |
| Self-hosted neutral lexical tokens | 161,897 | 166,758 | +3.003% |
| Self-hosted byte/4 model-token proxy | 187,854 | 189,646 | +0.954% |
| Fixed-point compiler C | 2,743,494 bytes | 2,836,209 bytes | +3.379% |
| Native compiler executable | 387,400 bytes | 437,856 bytes | +13.024% |
| Twenty-application SLIM source | 30,685 bytes | 30,493 bytes | -0.626% |
| Twenty-application neutral lexical tokens | 8,691 | 8,914 | +2.566% |
| Twenty-application byte/4 model-token proxy | 7,672 | 7,624 | -0.626% |

The old self-host contained 1,747 `inout name:` declarations. The completed
compiler contains 682 `name: @T` declarations, three `name: ^T` declarations,
2,841 direct `@name` call/recur operands, and 50 direct `^name` operands.
Static least-authority migration therefore removed 1,065 exclusive
declarations (61.0%) instead of mechanically preserving their authority.
Current analysis reports 1,110 shared and 682 exclusive compiler bindings.

The application corpus became 192 bytes smaller because `name: @T` is shorter
than `inout name: T`, even after adding visible call markers. Its lexical-token
count grew by 223 because those call markers are independent tokens. This is
the central readability tradeoff: capabilities are more visible locally, but
token count does not decrease.

The same feature-free 2,000-declaration program emitted in median 36.435 ms
with the baseline compiler and 36.395 ms with the RFC compiler
(`0.999x`, within noise). Full self-host emission grew from 160.365 ms to
168.594 ms (`1.051x`) while compiling the larger, ownership-annotated compiler.
A synthetic 1,000-transfer program grew from 18.518 ms with implicit moves to
25.360 ms with checked `^` declarations and calls (`1.369x`). The added work is
local and retained approximately-linear scaling; every durable quick
performance budget passed without relaxation.

Reduction at 2,000 declarations changed from 34,277/42,824/42,260 microseconds
for reduce/analyze/proof to 34,266/41,434/42,189 microseconds in the final
run. The complete twenty-application native gate passed at a SLIM/C runtime
geomean of `1.170`. A separate alternating old/new run over the 18 serial
applications produced a new/old runtime geomean of `0.996`; all 18 native
binaries retained identical byte sizes. No runtime ownership object, copy,
reference count, or ABI-version change was introduced.

The durable parallelism blocker sets are unchanged for the mechanically
migrated application corpus; its programs still require the same exclusive
mutation. The self-hosted compiler is the useful least-authority result: its
read-only compiler paths are now shared rather than false exclusive
capabilities.

### Production hardening result

The merge-readiness pass on 2026-08-01 replaced repeated lexical binding walks
with canonical constant-time binding links and moved overlap enforcement into
the typed call/recur walk. It also closed borrow-provenance gaps found by the
new negative matrix. Same-host quick medians were:

| Measure | Experiment candidate | Hardened candidate | Change |
| --- | ---: | ---: | ---: |
| Self-hosted SLIM source | 758,582 bytes | 755,958 bytes | -0.346% |
| Self-hosted SLIM lines | 17,808 | 17,750 | -0.326% |
| Fixed-point compiler C | 2,836,209 bytes | 2,828,789 bytes | -0.262% |
| Native compiler executable | 437,856 bytes | 438,048 bytes | +0.044% |
| 1,000 explicit owned transfers | 14,328 microseconds | 7,543 microseconds | -47.355% |
| Owned/nested normalized check ratio | not gated | 1.017 | limit 1.300 |

The hardened fixed-point C remains 3.109% above the pre-feature baseline, but
is smaller than the first complete experiment. The concentrated transfer
workload is now approximately the same check cost per source byte as the
existing nested-binding workload. A permanent normalized ratio gate joins the
existing owned-transfer scaling exponent, so a linear-but-expensive binding
lookup cannot silently return.

The safety audit found that call-only name scans were insufficient: recurrence,
derived aliases, branch results, pattern payloads, and assignment could hide a
borrow or repeated owner. The final checker carries borrow provenance and one
canonical root-owner identity on local bindings. Calls and `recur` share the
same deterministic access-epoch check, and whole-owner `^` sources are
distinguished from projections and conditional moves. These are compile-time
facts only; generated program ABI, runtime ownership mechanisms, application
binary sizes, and runtime behavior remain unchanged.

## Alternatives and drawbacks

Keeping owned transfer as the default preserves conventional by-value `T`
semantics and avoids `^` in ownership pipelines. It also keeps harmless reads
exclusive or consuming and leaves ownership loss implicit at calls.

Using `&` for shared and `@` for exclusive keeps every borrow explicit, but
makes the least-powerful and most common access mode longer. Using `&mut`
communicates mutation conventionally but repeats two tokens at every exclusive
call. Retaining `inout` only at declarations keeps calls short but hides
exclusive access locally.

Inferring shared versus owned behavior from the function body was rejected.
Changing a body could silently change callers, interfaces would not state
their ownership capability, and incremental checking would become less local.

Making all affine values copyable would require hidden deep copies, reference
counting, or unchecked aliases. General lifetime-bound references would add a
larger type and analysis surface than the demonstrated call-scoped need.

The selected design adds `^` to ownership pipelines and makes plain parameter
behavior depend on the existing copyable/affine category. That category already
controls copy versus move today, but the migration and dogfood experiment must
show that the new default reduces total ownership surface enough to justify the
new marker.

## Test and acceptance plan

The implementation must add positive, negative, and diagnostic conformance
fixtures; deterministic idempotent formatting; project interface and cache
coverage; recursive forwarding coverage; shared/shared, shared/exclusive, and
owned overlap coverage; built-in mode coverage; memory-plan coverage; and
generated-C assertions for all three modes.

The self-hosted compiler, examples, conformance corpus, benchmarks, host
fixtures, agent cases, and documentation migrate to the canonical form. The
compiler must reach a fixed point from the refreshed portable seed.

Before completion run:

- `./bootstrap.sh`;
- `cargo run --bin slim-govern -- check`;
- `cargo test`;
- `cargo run --release --bin slim-bench -- performance --quick`;
- `cargo run --release --bin slim-bench -- reduction --quick`;
- `cargo run --release --bin slim-bench -- parallelism`;
- `cargo run --release --bin slim-bench -- compare --quick`; and
- `cargo run --release --bin slim-bench -- agent`.

If the final implementation cannot reject every removed spelling, preserve
deterministic ownership and generated behavior, stay within compile/runtime
budgets, or demonstrate useful dogfood reduction, the RFC is rejected and the
branch implementation does not merge.

## Ratings and evidence

Safety is `+2`: shared access is least authority, mutation and call-boundary
ownership loss become explicit, and existing nonescape and uniqueness checks
remain mandatory.

Compile is `0`: all work is local and linear, but the parser and checker gain
mode handling that must be measured. Runtime is `0`: no runtime ownership
mechanism is added, but shared lowering and newly eligible analysis may change
generated programs and require measurement.

Minimality is `+1`: one exclusive marker and one ownership marker replace
`inout` and implicit affine consumption, subject to dogfood marker counts.
Analysis is `+2`: shared functions expose a strictly narrower capability and
remove false exclusive-mutation blockers. Dogfood is `+2` provisionally,
conditional on a complete self-hosted migration and recorded source benefit.

The weighted score is
`(2*20 + 0*20 + 0*20 + 1*20 + 2*15 + 2*5) / 2 = 50`.

## Decision

Accepted by the project maintainer on 2026-07-29 for complete branch
implementation and dogfood evaluation. Acceptance is conditional on the
measured gates in this RFC. A failing experiment changes the disposition to
rejected and removes the implementation rather than weakening a safety,
compile, runtime, or minimality requirement.

## Implementation

Complete on `codex/shared-default-borrows`.

The canonical parser stores a fixed mode on parameter and argument roots.
Typing enforces exact call/recur/built-in modes, copyable-mode rejection,
shared mutation and escape rejection, exclusive named-place rules, overlap,
and owned use-after-move. Borrow escape checks cover returns, records, enum
payloads, nested collection storage, and affine element reads. Formatting,
reduction, structural edits, analysis, memory planning, project interfaces,
code generation, and parallelism consume the same parsed mode facts.

Shared and owned aggregate parameters use the direct aggregate ABI; exclusive
parameters use the existing pointer ABI. Project interface, source-surface,
and persistent-cache schemas advance to 3. Runtime ABI 1 is unchanged.

The portable seed reaches a 2,828,789-byte fixed point. The repository contains
179 conformance fixtures plus 2,000 deterministic malformed-input mutations,
including dedicated shared/exclusive/owned capability, exact user and built-in
mode, whole-owner transfer, derived nonescape, control-flow and pattern
provenance, call/recur overlap, formatter, removed-spelling, and three-mode
code-generation coverage. All commands in the test and acceptance plan pass
on the production candidate branch.

## Removal and supersession

On successful completion this RFC supersedes the `inout` source spelling and
plain affine-parameter move default accepted by RFC-0007 while retaining its
unique-mutation safety requirement. Remove the feature if shared default
requires hidden copying or runtime ownership, if explicit transfers grow the
dogfood surface without compensating safety or analysis evidence, or if the
final compiler cannot preserve bounded local checking.
