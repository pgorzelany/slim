# RFC-0148: Staged successor compatibility

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: Codex, pursuing the SLIM Next M3 goal through its M2 prerequisite
Created: 2026-09-08
DecisionDate: pending
Approver: project-maintainer
Kind: compatibility
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Propose executable, independently verified M2 cutovers on an experimental
0.10 prerelease line. Preserve the clean 0.9 compiler and all regression intent,
use one canonical source language per checkpoint, and invalidate incompatible
derived artifacts. M2 completion would release 0.10.0; it would not freeze 1.0
or establish M3 completion. No version changes when this proposal is accepted
alone.

## Motivation

M2 combines source, lifetime, allocator and control changes that cannot safely
be deployed as unchecked renames. The current seed must exactly reproduce the
selfhost project, runtime ABI 1 defines region-backed copyable Bytes, and the
release verifier deliberately rejects a mismatched runtime header. A transition
that bootstraps only through Rust, retains two production parsers or drops an
old regression would violate the accepted architecture.

## Guide-level explanation

Each accepted source cutover has a preceding executable compiler, an offline
migration receipt, a successor compiler that accepts only the new canonical
form, a migrated corpus and a freshly reproduced portable seed. Releases of
intermediate experiments identify their exact prerelease contract; they do not
claim the complete successor package.

Automatic migration applies only proven local conversions. An ambiguous old
byte-view lifetime or ownership destination receives a source-located rejection
for explicit repair. Old/new behavior comparisons preserve the complete declared
domain and enumerate intentional changes in traps, moves, cleanup and failure.

## Reference-level specification

### Version and artifact transitions

Keep 0.9.0 until the first source or documented-behavior cutover. That cutover
uses `0.10.0-dev.1`; each later incompatible checkpoint increments the positive
decimal prerelease ordinal. A behavior-preserving implementation revision can
keep the ordinal but always has its complete compiler/source identity. These
prereleases have exact-checkpoint compatibility, not a patch-stability promise.
At complete M2 acceptance, replace the prerelease with 0.10.0. Thereafter existing
pre-1.0 patch-preservation rules apply. M3 chooses any further necessary changes
in its own child RFCs.

| Artifact | Current | Required transition |
| --- | --- | --- |
| Source surface | 3 | First changed accepted source uses 4; each later incompatible accepted surface advances monotonically. Update its complete owning ledger at cutover. |
| Project interface | 3 | First changed represented signature/type contract uses 4; later incompatible shapes or meanings advance again. Consumers reject unknown versions. |
| Persistent cache | 3 | First changed framing or interpreted content uses 4; incompatible meanings advance again. Old/corrupt entries miss and rebuild. |
| Runtime ABI | 1 | First changed layout/calling convention/ownership-runtime contract uses 2; later incompatible changes advance again. Never mix old/new runtime objects. |
| Manifest | 1 | Retain unless its structure or meaning actually changes through a child decision. |
| Diagnostic and evidence schemas | Current release ledger | Retain for compatible additions only under each schema's existing rules; otherwise increment before emission and reject unknown meanings. |

Do not preassign one final schema/ABI number to all unfinished M2 changes.
Each child must name exactly which row changes and its concrete next value at
implementation. Generated C, worker protocols and native/session artifacts
include the consumed compiler/runtime/target/options identities. A same-width
field with changed ownership meaning still requires invalidation. Session
transport changes require their own concrete schema decision; a loaded process
never silently changes its compiler contract in place.

VERSION, Cargo package version, source metadata, release ledger and current
documentation change together. Replace hardcoded 0.9 assertions with assertions
of the accepted current contract, while retaining historical checks as scoped
historical evidence. This is not permission to remove assertion coverage.
The package script currently accepts only digits and dots in VERSION. Before
the first prerelease, extend that validator to recognize the exact selected
`0.10.0-dev.N` grammar with positive decimal N and no leading zeros, while
retaining rejection of empty, malformed and unsafe archive names.

### Production bootstrap at a source cutover

1. Identify and preserve the last passing source revision, seed digest and
   matching runtime. For M1, use the full closure checkpoint `4bdb675`; later
   reporting edits are not a different compiler baseline.
2. Implement the successor reader/checker/emitter in the preceding SLIM source
   language. Compile that source using the preceding production compiler and
   matching runtime to obtain the transition compiler. No Rust semantics or
   separately accepted IR enters this chain.
3. Run the offline source migration over the compiler, library, conformance,
   benchmarks, examples and executable documentation. The transition compiler
   checks the migrated compiler and emits the new seed. Any required runtime
   bridge belongs to the accepted runtime child and must be specified before
   this step; do not adapt pointers or ownership ad hoc.
4. Compile the successor seed with its matching runtime; reproduce the migrated
   compiler twice and require byte-identical C plus the normal seed digest and
   bootstrap fixed-point checks. The final compiler rejects removed syntax.
5. Run all per-change gates before commit and the complete release gate at
   milestone closure. Preserve source hashes, exact transition command and
   failure history in the compact evidence archive.

The transition compiler implements one successor grammar; its own source was
written in the preceding grammar. This does not require a dual grammar flag in
the shipped compiler. Keep the reproducible transition recipe and old compiler
revision as evidence, not a runtime fallback or accepted old-source frontend.

### Ownership and runtime integration boundaries

Before any ownership syntax or byte representation changes, accept the complete
ownership, slices/bytes and memory-runtime contracts, including this sequence's
intermediate states. Shared old Bytes views must remain valid until their last
use; do not enable prompt reclamation while such views can escape unchecked.

Allocator-domain preparation may precede owned Bytes only while the current
reclamation contract remains intact. Owned Bytes migration covers host APIs,
compiler input/caches, literals, worker captures and returned results. Prompt
cleanup follows only when backing ownership and lexical references are checked
through every such boundary. Every successful worker still joins exactly once;
declined spawn uses the identical body. Unsupported safe scheduling stays serial.

Concrete result enums can support recoverable allocation before generic enums
exist. Their later replacement is another explicit source migration, with no
permanent aliases or hidden result-container fallback. Exhaustion must preserve
the specified container state and unconsumed value; old exit-71 conversions
remain named hosted policy until the relevant accepted child replaces them.

### Source and behavior migration

Uniform moves require the old checker's actual destination/ownership facts;
lexical text replacement cannot distinguish ownership from a shared view.
Reference-return migrations must establish a relationship to live input storage
or produce an owned result. Do not infer either from a type name.

For eager Boolean expressions, evaluate observable operands once in the original
left-to-right order into explicit temporaries before using the successor operator.
The child migration must preserve traps, mutation, moves, allocation and effect
order, including nested expressions. Direct replacement is allowed only with a
positive fact that the skipped work is total and unobservable for that exact node.

Replacing recur must preserve simultaneous argument evaluation, checked transfer,
cleanup and constant-stack behavior for the preserved domain. Store Unit only
under its accepted new layout contract. Reclassifying effects does not manufacture
termination/no-trap facts or relax parallel execution conditions.

## Compiler and runtime design

Production changes stay in selfhost/ and the portable seed. Offline migration
uses the preceding production checker's facts or rejects unsupported cases;
it cannot approve a successor program. The normal successor checker remains
the sole authority. Missing maps, stale nodes, incomplete dependencies and
unknown ownership facts cannot enable conversion, reuse or code generation.

## Compatibility and migration

This decision would supersede RFC-0107's active version/surface transition only
when the first successor cutover completes. Its historical evidence remains
unchanged. The existing `docs/MIGRATION-0.10.md` describes RFC-0110 ownership
modes, not this successor migration; explicitly retitle/relocate that historical
guide and update links before publishing the new 0.10 guide. Do not overwrite
the old migration semantics in place or silently reinterpret ABI 1.

## Diagnostics and failure cases

Migration reports must identify source revision and span, violated conversion
precondition, and whether human repair is required. Reject ambiguous lifetime
conversion, conditional/partial ownership not supported by the new checker,
stale source identity and unknown source version. Migration publishes no partial
replacement project after failure. Runtime/schema mismatch fails explicitly;
rebuildable cache corruption remains a miss, not a trap or accepted artifact.

## Performance and complexity

Ordinary compilation remains approximately linear in source and dependency
edges; generic instance/output work later has its own hard bounds. Record
migration work separately from normal compilation. Keep same-host cold/warm
frontend/backend/runtime, allocation, peak memory and code-size measurements.
No numerical budget, noise rule, timeout or permanent fixture is relaxed here.

## Alternatives and drawbacks

One final atomic M2 rewrite would make failures harder to isolate. Supporting
both source languages permanently would duplicate the accepted surface. Keeping
0.9 identifiers during semantic changes would mislabel artifacts. Prerelease
ordinals and repeated corpus migration add bookkeeping and review cost, but
make each executable checkpoint's contract explicit.

## Test and acceptance plan

At each cutover compare the preceding and successor production compilers over
the named preserved domain and record intentional differences individually.
Every discovered safety witness becomes a permanent successor regression, even
if its source changes. Require positive/negative/diagnostic migration cases,
idempotent formatting, two-generation seed equality, clean/retained complete
result and C equality, rejection/recovery, corruption/reset and stale identities.

Maintain the full native analysis/resource baseline and complete parallel blocker
sets; explain each changed row. Preserve both state_machine and signal_network
parallel/serial ratio gates. Run every AGENTS.md command before compiler/runtime/
benchmark/tool commits and the full `scripts/verify-0.9.sh` at milestone closure.
Its historical filename may remain until an accepted rename preserves all hooks.

At planning base `bead09c`, the ABI-mismatch release test rewrites literal ABI 1
to 2. Before the first ABI bump it must derive a distinct unsupported value from the current
header and verify that a real replacement occurred before compiling. Preserve
the expected rejection, reproducible package pair, source hashes, clean install,
frontend/native sessions, examples and website tests. A test that no longer
mutates the ABI header is not evidence of mismatch rejection.

## Ratings and evidence

All ratings and score are zero; no measured implementation benefit is claimed.
The current VERSION, runtime ABI, release schema table, seed fixed-point
algorithm and literal ABI test are exact source observations. Successor safety,
cost and completion remain unknown until the named implementation evidence exists.

## Decision

Proposed. RFC-0112 authorizes detailed compatibility decisions within its staged
successor direction; this draft selects the transition contract for review.
No source/runtime implementation begins under an unaccepted child contract.

## Implementation

Pending. Version, seed, runtime ABI and schemas are unchanged. An independent
current-gate repair now derives the ABI mutation from the header and requires
both successful matching-header compilation and the actual mismatch diagnostic.
Its [focused validation](../../benchmarks/results/2026-09-08-m2-prerequisites.md)
does not implement this compatibility proposal or claim a clean release pass.
The first dependent source cutover is Unit after the M2 prerequisite contract
package and any applicable policy decision are accepted.

## Removal and supersession

Revise this proposal before implementation if the ownership/runtime child package
cannot supply a safe executable intermediate state. Preserve old checkpoints,
failing witnesses, gate intent and contrary measurements through any revision.
A failed slice is repaired or reverted as a complete source/runtime transition;
it is not hidden by a compatibility fallback.
