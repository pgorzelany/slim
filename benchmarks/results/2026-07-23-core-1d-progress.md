# Core 1D typed-view progress

Date: 2026-07-23
Status: progress checkpoint, not milestone completion
Host: Apple Silicon arm64, macOS 26.3 (25D125), Apple Clang 21.0.0

## What changed

- Standalone compilation now reads and lexes a valid input once, returns one
  checked artifact containing the typed view, and retains that artifact through
  C emission.
- The type analyzer builds scoped lexical links before expression validation;
  downstream lookups use tagged token links instead of walking every enclosing
  binding.
- Effect and ownership passes resolve user calls through the same declaration
  links. Three repeated whole-program declaration scans and their unused lookup
  functions were removed.
- Syntax and structural checks run before memory or type analysis. The existing
  deterministic multi-diagnostic recovery remains intact.
- The permanent performance workload now makes every generated declaration
  call a user function and adds a geometric deeply nested lexical-binding
  series. Both are governed by versioned exponent budgets.

No syntax, type, effect, built-in, runtime ABI, dependency, LLVM component, or
alternate executable representation was added.

## Measured result

Before linked call resolution, nine idle-host checks of the 468,568-byte
self-host project centered around 3.91 seconds. After the change, nine checks
had a 0.20-second median (one 0.33-second outlier), approximately 19.6 times
faster. This is still process-level project checking rather than incremental
edit latency, and it is not described as a millisecond clean build.

The full permanent performance run reported:

| workload | first | last | first check | last check | exponent |
|---|---:|---:|---:|---:|---:|
| user-call declarations | 1,000 | 8,000 | 10,176 us | 68,486 us | 0.916 |
| nested lexical bindings | 250 | 2,000 | 6,564 us | 50,412 us | 0.980 |

At 8,000 declarations, deterministic emission took 111,491 us and the
emit/check ratio was 1.628. Both check series and emission remain under the
permanent 1.25 scaling-exponent limit, and the ratio remains under 4.0.

Reproduce with:

```text
cargo run --release --quiet --bin slim-bench -- performance
./bootstrap.sh
cargo run --quiet --bin slim-conform -- check
```

## Rejected dense-index experiment

A first dense `expression token -> fact` vector was implemented and then
reverted. The existing compiler took more than 13 seconds to check the modified
self-host project, compared with about 0.21 seconds after the candidate was
removed. The candidate had not yet been bootstrapped, so this measures a
source-shape performance cliff in the current compiler rather than the runtime
cost of constructing the proposed index. It is not evidence against dense
lookup in principle; it is evidence that this particular change cannot land
without profiling, a geometric fixture, and a same-host regression result.

## Token-interval diagnostic checkpoint

D0034 replaces the structured issue's single token with inclusive start and
end token indices. Standalone and prepared-project reporters consume the same
interval, while one-token type issues set both endpoints identically. The
self-host check remained about 0.22 seconds and the compiler bootstrapped to a
byte-identical 1,652,703-byte C seed.

An attempted all-at-once migration of the effect, recursive-borrow, and move
walkers threaded another mutable issue vector through their recursive call
graphs. Even after removing the move portion, self-validation exceeded 40
seconds. The experiment was reverted rather than accepting a compiler
performance regression. The representation remains; collectors will migrate in
smaller slices with source-shape measurements before each checkpoint.

## Finalized issue stream and first migrated family

D0035 makes `Checked.issues` the completed diagnostic stream. It starts with
the analyzer snapshot and is extended by later phases without rebuilding the
typed view. This avoids both the ownership bug in mutating an extracted vector
and the measured view-reconstruction performance cliff.

Missing-effect `E0343` is the first migrated family. The non-recursive effect
coordinator appends the complete call interval; standalone output is unchanged
and `project-effect-error` pins `E0343@app@72:94`. Self-validation remained
about 0.22 seconds.

The same bounded pattern then migrated temporary-inout `E0348` and
aliased-inout `E0349`. Their project fixtures pin `E0348@app@164:178` and
`E0349@app@211:217` while standalone output stays unchanged. Inline issue
construction initially crossed the source-shape cliff; extracting small
whole-form and one-token leaf helpers restored the approximately 0.22-second
self-check. Final seed size and corpus totals are recorded after the release
gate: a byte-identical 1,653,846-byte C seed, 89 fixtures, and 2,000 malformed
mutations.

## Checked emission links

D0036 removes declaration relinking from `codegen/emit_program`. Standalone and
project compilation already retain the exact token stream linked during type
analysis, so the backend now consumes that checked evidence directly. The
fixed-point seed shrank from 1,653,846 to 1,653,599 C bytes.

A same-host quick series did not show a clean latency signal. The 2,000
declaration median moved from 32.279 ms to 31.987 ms, the 1,000 declaration
point improved from 22.132 ms to 16.344 ms, and smaller points varied in both
directions. This is recorded as one proven linear pass removed, not as a stable
percentage speedup. Permanent exponent and emit/check-ratio budgets remain the
performance authority.

## Bound variant type links

D0037 closes a checker/backend gap exposed by an adversarial aggregate fixture.
A variant copied from a parameter into a lexical local passed type checking but
the backend's parameter-only type scan emitted invalid C. Variant-match emission
now decodes the scoped type link already attached to the bound scrutinee. The
obsolete parameter scan was removed.

`variant-local-match` compiles, runs, and prints `42`; the previous parameter
fixture remains green. Self-validation stays near 0.22 seconds and the
byte-identical seed shrank from 1,653,599 to 1,650,892 C bytes. The corpus now
passes 90 fixtures and 2,000 malformed mutations.

A broader trial annotated composite expressions and lowered a direct
variant-producing match scrutinee. The annotation itself could be kept small,
but generalized nested-expression lowering pushed self-validation beyond 12
seconds. It was reverted completely. This checkpoint claims bounded scrutinees,
not arbitrary expression lowering.

## Aggregate construction links

D0038 removes record construction's whole-declaration scan by consuming the
record type token's checked declaration link. Canonical field checking now also
publishes the matching field definition on the supplied field form. A new
`record-wide` executable fixture constructs sixteen ordered fields, projects
the last, and prints `42`.

Two consumers of the field link were tested and reverted. Advancing record
definitions in lockstep and directly reading the retained link from the
recursive emitter each pushed self-validation beyond the five-second abort
guard, versus about 0.21 seconds of user CPU time for the accepted producer.
The compatibility field-name scan therefore remains and no bounded-field-lookup
claim is made. The byte-identical seed is 1,647,978 C bytes; 91 fixtures and
2,000 malformed mutations pass.

## Variant construction links

D0039 removes both remaining whole-declaration scans from variant construction
and match emission. The checker now publishes validated constructor and match
case definitions on their structural opening tokens. This producer-first
checkpoint retains the case-name scan for one bootstrap generation so the
checked-in seed can build the future link consumer normally.

`variant-wide` constructs the last of sixteen cases, passes it through exhaustive
ordered matching, and prints `42`. Self-validation remains about 0.21 seconds
of user CPU time, all 92 fixtures plus 2,000 malformed mutations pass, and the
fixed-point seed shrinks to 1,645,415 C bytes. The case-link consumer is recorded
in the next checkpoint.

## Bounded variant members

D0040 consumes the case links published by D0039 for both construction and
match-arm emission. The recursive textual case scanner is gone, making those
queries independent of the number and spelling length of cases. The staged
producer seed builds the consumer through the normal bootstrap path.

The sixteen-case fixture and existing local-match fixture compile and print
`42`; self-validation remains about 0.22 seconds of user CPU time. All 92
fixtures and 2,000 malformed mutations pass, and the byte-identical seed shrinks
again to 1,643,001 C bytes. Record-field lookup remains the only aggregate
member scan and retains D0038's explicit negative result.

## Typed recursive-inout diagnostics

D0041 moves `E0350` into the existing typed call-argument traversal. A small
non-recursive leaf compares recursive `inout` arguments with their parameters;
the traversal appends every mismatch before normal type recovery. This avoids
the measured issue-vector recursion cliff and removes the legacy recur argument,
body, declaration, and direct-reporting functions.

The standalone fixture retains two diagnostics and the project fixture pins
`E0350@app@99:104` plus `E0350@app@105:109`. Self-validation remains about 0.22
seconds of user CPU time, and the byte-identical seed shrinks from 1,643,001 to
1,634,840 C bytes. Multiple structured issues retain exit status one; all 93
fixtures and 2,000 malformed mutations pass.

## Checked memory type links

D0042 orders standalone type analysis before memory planning and removes the
planner's recursive textual declaration lookup. Named record and variant
storage classification now consumes the declaration link on the checked type
token; code generation already satisfies the same checked-token precondition.

The lifetime and wide aggregate fixtures remain executable. Nine warm
self-checks recorded user times of 0.10, 0.11, 0.12, 0.10, 0.11, 0.11, 0.12,
0.11, and 0.11 seconds, with a 0.11-second median; wall-time median was 0.12
seconds. That is about twice as fast as the preceding 0.22-second boundary.
The byte-identical seed shrinks from 1,634,840 to 1,631,678 C bytes. This is a
bounded named-type query, not yet a claim that the complete memory plan is
retained in `typing/Checked`.

The durable quick scaling series places `Payload` after 125, 250, 500, and
1,000 functions that each use it as a parameter type. Checks took 4,701, 6,099,
7,551, and 8,239 microseconds, an endpoint exponent of 0.270 under the permanent
1.25 limit. This adversarial declaration order would expose restoration of the
old per-reference declaration scan.

## Remaining Core 1D blockers

- Project checking, scheduling, ordinary emission, and cache misses now share
  one prepared artifact. Each flattened token retains its module and original
  byte span; the `project-type-error` fixture pins `E0344@app@56:60`.
- Boolean recovery and ownership diagnostics now use the finalized structured
  channel; every accepted project semantic diagnostic uses the same origin
  projection.
- Code generation still derives per-call allocation-effect and expression
  facts from token structure instead of consuming bounded typed-view queries.
- Adversarial expression tests and geometric source-shape budgets still need to
  join the existing aggregate, allocation-failure, sanitizer, and release gates.

## Retained memory plan

D0043 makes the memory plan a field of `typing/Checked` and carries it into the
prepared project. Ordinary code generation now requires that plan and consumes
function summaries in declaration order. The consumer verifies the stored
function token before using its `local_region` decision; the direct backend
call to `memory/function_uses_local_region` is gone.

This removes a second authority for region placement rather than claiming a
large latency win. The quick 250/500/1,000/2,000-declaration series emitted in
8,735, 11,008, 21,777, and 41,141 microseconds, with a 1.410 emit/check ratio at
2,000 declarations. Self-validation remains about 0.11 seconds. All 93 fixtures
and 2,000 mutations pass, and the fixed-point seed is 1,635,534 C bytes.

The plan does not yet retain allocation-effect or recurrence facts, and
expression lowering still reads token structure. Those remain bounded
typed-view work rather than being hidden inside this checkpoint.

An immediate follow-up tried to add allocation-effect and recurrence fields to
`FunctionPlan` while folding recurrence discovery into the allocation walk.
The old compiler required more than 8.11 seconds of user CPU to validate that
source, versus about 0.11 seconds at the accepted boundary. The candidate was
interrupted and reverted completely. This is another source-shape result, not
evidence against retaining those facts through a differently factored change.

## Structured nonexhaustive diagnostics

D0044 changes the existing Boolean exhaustiveness coordinator from direct byte
reporting to one `typing/Issue` carrying the complete match token interval.
Standalone `nonexhaustive` remains `E0336@66:87`, and `multiple` preserves its
four-code order. The prepared-project fixture projects the same family as
`E0336@app@56:77`.

Self-validation remains about 0.11 seconds. The byte-identical seed is
1,635,539 C bytes, and the corpus contains 94 fixtures plus 2,000 deterministic
mutations. Secondary recovery inside an already invalid Boolean match remains
direct in standalone mode and is not claimed as migrated.

## Rejected ownership diagnostic stream shapes

Several attempts moved legacy use-after-move discovery into a typed event
stream before issue projection. A `MoveEvent` carried inside `check.slim`
pushed self-validation beyond nine seconds. Moving the event into a separate
ownership module still exceeded eight seconds, and a clean split between a
token-state vector and a final issue vector exceeded fifteen seconds.

Smaller isolation experiments found two sensitive compiler-source shapes:
classification followed by mutation is fast when split across helpers, while
branching and mutation in the same function crosses the cliff; recursively
converting events while carrying both event and issue vectors also crosses it.
All candidates were reverted. No token sentinel, overloaded issue field, or
rendered-diagnostic shortcut was retained. D0049 later resolved the blocker by
placing state on the existing typed binding, distinguishing non-blocking issues
explicitly, and packing type plus declaration identity into the existing local
link. Its permanent owned-transfer series is the required geometric
reproducer.

A separate compact-plan experiment replaced the existing `local_region Bool`
with a same-width `summary I64` intended to retain the allocation effect. Even
without widening `FunctionPlan`, self-validation exceeded six seconds. That
candidate was also reverted; record width alone is not the cause of the
source-shape cliff.

## Bounded record members

D0046 consumes the record-field links published by D0038 and removes the last
recursive textual aggregate-member scanner. The checked link is verified
against the first record-field token and the existing field-name source span,
so lookup is independent of declared field count without blindly trusting an
unchecked index. There is no fallback scan.

The accepted change intentionally preserves the surrounding recursive emitter.
Extracting a leaf and calling it through a new recursive wrapper accumulated
more than 94 seconds of CPU time before interruption. Removing three apparently
redundant scalar bindings from the existing recursive source exceeded eight
seconds. An uncalled leaf, a non-recursive wrapper, and the final in-place
lookup replacement each remained near 0.10--0.11 seconds. These results are
recorded as compiler source-shape evidence, not rationalized away as runtime
cost.

`record-wide` still constructs sixteen ordered fields and prints `42`. The
complete release gate passes 94 fixtures, 2,000 deterministic malformed-input
mutations, quick performance budgets, sanitizers, allocation-failure injection,
and a byte-identical 1,635,270-byte C bootstrap seed.

## Retained allocation boundaries

D0047 completes the function-level allocation consumer without widening
`FunctionPlan`. Its existing allocation-site vector now records all five
allocating built-ins and allocation-capable user calls using checked declaration
links. Function emission derives the presence of its failure label from that
retained vector instead of reading the declared effect list again.

The previously duplicated built-in effect tables were not copied into the
planner. A new internal `effects` module owns list, built-in, function, and call
queries for diagnostics, planning, and emission. This adds no language or
runtime surface. The remaining per-call postcondition check uses the same
canonical query; passing planned site cursors through every expression emitter
is explicitly later work.

`allocation-user-failure` injects failure at allocation two inside a called
SLIM function and requires the caller to return status 71 with the exact
allocation report. All 95 fixtures and 2,000 mutations pass, self-validation
remains near 0.11 seconds, and the complete release gate fixes the bootstrap at
1,631,733 generated C bytes.

## Structured Boolean recovery

D0048 converts the complete secondary recovery sequence for an invalid Boolean
match from direct rendering to finalized `typing/Issue` intervals. The existing
walk appends unknown-name `E0314`, duplicate-arm `E0335`, and arm-type `E0344`
after the primary nonexhaustive `E0336`, then renders once. The shallow recovery
rules and accepted language are unchanged.

The standalone `multiple` fixture preserves its four-code order and byte spans.
`project-boolean-recovery` pins the same sequence at
`E0336@app@56:96`, `E0314@app@74:81`, `E0335@app@83:95`, and
`E0344@app@89:94`. Obsolete byte-span locals were removed from the token-based
collector. Self-validation remains around 0.10--0.11 seconds.

All 96 fixtures and 2,000 deterministic mutations pass. The complete release
gate records a byte-identical 1,631,074-byte generated C seed. Move diagnostics
are now the remaining legacy semantic family outside project projection.

## Typed ownership diagnostics

D0049 moves aggregate transfer state onto the typed lexical `Binding`. Local
links pack the checked type token and exact declaration token, and declaration
tokens point to their binding record during inference. User-call transfers and
`bytes.freeze` therefore update move state in constant time without a second
token walk or a name-spelling vector. Borrowed owned returns are checked at the
same typed function boundary.

`Issue.blocks_inference` distinguishes ownership findings from type failures:
move errors do not hide later type or ownership diagnostics, while the existing
first blocking type error still prevents cascades. The old `report_*move*`,
`moved_has`, and direct `E0315`/`E0347` paths are removed.

`project-ownership` retains four ordered module-local ranges covering repeated
user-call transfer, transfer from `inout`, repeated `bytes.freeze`, and returning
an owned value through `inout`. The new permanent owned-transfer scaling series
measured 4.836, 8.102, 14.275, and 26.535 milliseconds for 125, 250, 500, and
1,000 transfers, below its 1.25 exponent budget. All 97 fixtures and 2,000
deterministic malformed-input mutations pass. The release checkpoint records a
byte-identical 1,619,795-byte fixed point with SHA-256
`f4021a1524dcb4dcfaa45aff76d0875b3df0ec9bcc7e55d7f447e10bbd4e5f71`.

## Bounded project module validation

D0050 moves structural source validation ahead of declaration linking for each
module loaded from a project. Validation receives that module's half-open range
inside the shared token vector; malformed source is never retained or exposed
to semantic consumers. `index_names_from` also stops at the vector length as an
independent invariant rather than relying on a closing token to appear.

`project-malformed-module` removes the final module delimiter and requires the
stable projected result `E0102@app@0:59`. The first accepted conformance run
passes 98 fixtures and 2,000 deterministic malformed-input mutations. The
existing project geometric gate measures the one added linear validation pass;
the change adds no language primitive and changes no generated-program runtime
path. The fixed-point seed is 1,622,171 C bytes with SHA-256
`7678b680cf808a216bc9b1077f28802434f04bc03b810cde5b9b1f39a213441f`.
The complete release gate passes project scaling and regression budgets,
sanitizers, allocation-failure injection, native challenges, and deterministic
bootstrap verification.

## Retained recurrence boundaries

D0051 moves function-level recurrence classification into the checked memory
plan. Planning records a Boolean only for an actual form headed by `recur`, and
function emission consumes it instead of scanning the body for an atom with
that spelling. This removes one backend semantic authority and makes the
classification structurally precise without changing the language or runtime.

The earlier combined allocation-and-recurrence summary exceeded eight seconds
of self-validation; the isolated fact remains around 0.10 seconds and reaches a
byte-identical candidate fixed point. The `recur` fixture continues to print
`55`, and all 98 fixtures plus 2,000 deterministic malformed-input mutations
pass. The portable fixed point is 1,622,521 C bytes with SHA-256
`65d23caa0a0b70e7b4e5db8892538596f2a63e89a73ba89b273f013254a8025a`.
The complete release gate passes geometric and same-host performance budgets,
sanitizers, allocation-failure injection, native challenges, and deterministic
bootstrap verification.

## Dense expression facts

D0052 replaces the typed view's sparse postorder fact bag with a dense table
indexed directly by token identity. Each slot stores only its `TypeRef`;
non-expression tokens begin with the invalid sentinel, and successful inference
updates the slot in constant time. `finish_type` rereads each result through the
guarded `fact_type` query, exercising the permanent path throughout self-hosting
and geometric performance workloads.

The self-check remains near 0.10 seconds, the candidate reaches a byte-identical
fixed point, and all 98 fixtures plus 2,000 mutations pass. A repeat quick run
measured 2.471, 2.816, 4.221, and 6.184 milliseconds for named-type parameters
and 4.404, 6.398, 11.333, and 22.383 milliseconds for nested bindings at 125,
250, 500, and 1,000 units. A noisy first final named-type sample of 34.162
milliseconds tripped the 1.25 budget at 1.269; the budget was preserved and the
complete release run remains the acceptance measurement.
The portable fixed point is 1,625,465 generated C bytes with SHA-256
`d57893e96f6321aa3b5b1df6a66d28ce31ff9ced4393b3956fa642494ee3adf0`.
The authoritative release run measures 2.665, 2.994, 3.975, and 6.309
milliseconds for named-type parameters and 4.222, 6.752, 11.723, and 22.265
milliseconds for nested bindings. The complete gate passes all performance
budgets, sanitizers, allocation-failure injection, native challenges, and
deterministic bootstrap verification.
