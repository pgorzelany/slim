# RFC-0120: Definite owner reinitialization

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Restore availability of a moved mutable whole owner after a checked assignment
installs a valid replacement. Join definite availability across every lexical
branch arm, without copying binding tables or adding partial moves.

## Motivation

The M0 reinitialization witness moves a vector and assigns a fresh vector, but
the following read is rejected with E0315. RFC-0113 deliberately made move
events irreversible. Clearing its event on assignment would incorrectly let a
reset in one arm hide an unavailable value on a sibling path.

## Guide-level explanation

Assignment to an existing mutable owner evaluates and checks its right-hand side
before restoring the destination. Reading or moving the old destination inside
that expression still requires an available owner. A name is available after a
branch only if every arm leaves it available. Reinitializing a field of an
unavailable aggregate is not supported; replace an available field explicitly
with RFC-0119's operation.

## Reference-level specification

Availability is a two-state fact: available or possibly moved. Owning transfer
sets the latter. A valid whole-name assignment sets available after the source
value has transferred. Type, mutability, ownership, and loan checks continue to
apply. An invalid assignment never supplies positive initialization evidence.
Assignment to an exclusive parameter retains its existing caller-visible
behavior and does not create permission to consume that borrowed parameter.

Every arm starts with its branch-entry state. At a join, possibly moved is the
disjunction of all arm exits. Untouched arms retain the entry state. Even a
constant condition does not remove a syntactic arm from this conservative
contract. A one-case enum has one arm and no additional alternative.

## Compiler and runtime design

Replace the irreversible move-component interpretation with a lazy per-owner
state stack in the production SLIM checker. The same checked binding identity
stores its top frame. Owners without a move need no state stack. A frame stores
its canonical control scope, entry and exit states, parent frame, and the number
of represented arms. This is the existing checker's availability fact, not a
second parser or ownership checker.

The control traversal creates explicit branch and arm scopes. At the end of a
scope, union its component with its enclosing scope and retain that enclosing
scope as the component's lexical ancestor. Union by rank and path compression
give the nearest still-open ancestor of a previously visited scope: the lowest
common ancestor of the owner's previous event and its current event in the
depth-first traversal.

On the next access, close only that owner's completed frames to this common
ancestor, inserting an intermediate frame when the ancestor was previously
skipped. Open a frame for the current scope. Unrepresented paths contain no
events for this owner. A prefix count of multi-arm branches identifies skipped
alternatives; their entry state must participate in the join. Explicit branch
frames count represented arms and include the entry state for every untouched
arm. Intermediate frames inherit the entry state of their existing child.

There are at most two new frames per event after the initial frame, and each
frame closes at most once. Each control scope is created and unioned once.
For N scopes and relevant owner events, aggregate work is O(N alpha(N)) and
space O(N). No branch copies all bindings and no nested join replays descendant
event lists. The compiler retains immediate E0315 reporting at the checked
source use. No application allocation policy or runtime ABI change is required.

Conditional assignment exposes a native lowering defect when an atomic result
names its own destination: generated `x = x` is rejected by the strict C build.
For that exact checked binding identity, emit a C no-op. Both endpoints must
resolve to the same canonical declaration; result slots, temporaries, literals,
and distinct bindings retain normal assignment lowering. A checked atomic name
has no effect, trap, allocation, or computation to discard, and storing its value
back into that identical place changes no runtime state. The checker still
performs transfer and initialization checks. This is destination lowering, not
a new source-reduction rule or an exemption for an unavailable source read.

## Compatibility and migration

The former false rejection becomes accepted. Existing unavailable reads,
borrowed escapes, mutation conflicts, conditional transfers, and field rules
remain rejected. No syntax or language surface is added. The portable seed
must reach the same checked fixed point as the selfhost compiler.

## Diagnostics and failure cases

Preserve E0315 for unavailable reads and existing type, mutability, and loan
diagnostics. Cover self-reading replacement expressions, invalid and borrowed
replacements, sibling contamination, missing-arm initialization, nested joins,
one-case and multiple-case enums, and a second move after initialization.

## Performance and complexity

Retain every existing ownership scaling gate. Add wide and deeply nested
reinitialization series under the same 1.25 check-exponent ceiling. Measure the
compiler before and after on the same host; record contrary results. No
performance budget exception is authorized.

## Alternatives and drawbacks

Clearing the old move event is unsound across branches. Full environment
snapshots and repeated sparse-log propagation can be quadratic. Persistent
maps with naive recursive union can also revisit unchanged branch state at
every nesting level. The lazy stack adds explicit checker state and needs
independent path-oracle and adversarial scaling evidence.

## Test and acceptance plan

Execute positive cases under ASan/UBSan and bounded allocation-fault injection.
Compare the checker with a bounded independent enumerated-path oracle including
read, transfer, reset, prefix/suffix, and nested-arm combinations. Preserve exact
negative fixtures and native analysis/resource baselines. Run bootstrap,
governance, conformance and malformed inputs, and all required per-commit
verification and benchmark gates before declaring implementation complete.
Update governance's structural checks from the removed irreversible helper
names to the lazy binding entry points and the new module's root, scope, frame,
and state operations. Preserve the behavioral oracle and every existing
diagnostic and performance gate; helper-name checks are not behavioral proofs.

## Ratings and evidence

Analysis +2 models definite availability at structured joins. Other ratings
are zero: the fix adds no primitive and makes no unmeasured safety, compiler
speed, application speed, or agent effectiveness claim. Weighted score: 15.

## Decision

Accepted as an M0 implementation detail under RFC-0112's delegated maintainer
authorization and the active M0 goal. This is not a separate maintainer review
or an exception to a hard gate.

## Implementation

Implemented in the production SLIM checker and portable seed, with exact
same-binding destination lowering in the C generator. Thirteen conformance
rows, the 15,552-program independent path oracle, and 16 deep multi-owner
programs cover the repaired contract while preserving the original 486-program
move-only oracle. Five native positive cases and the self-host compiler pass
ASan/UBSan; bounded allocation-fault campaigns retain clean failure behavior.

The fixed point is 3,020,129 C bytes. All 7 unit and 58 integration tests,
325 conformance fixtures and 2,000 malformed-input mutations, governance,
formatting, Clippy, bootstrap, and required benchmark gates pass. Wide and nested
reinitialization exponents are 0.776 and 0.645 under the unchanged 1.25 ceiling.
Warm paired measurements retain ordinary-transfer and frontend samples; no
general speedup or agent effectiveness claim is made. All 20 complete native
analysis reports and C outputs remain byte-identical to checkpoint 87b2adf.
See [the dated implementation report](../../benchmarks/results/2026-09-05-slim-next-progress.md#definite-reinitialization-checkpoint-2026-09-06)
for hashes, measurements, bounded validation domains, and remaining M0 work.

## Removal and supersession

A successor place/CFG representation may replace these ephemeral facts while
preserving the availability contract, exact regressions, and scaling gates.
