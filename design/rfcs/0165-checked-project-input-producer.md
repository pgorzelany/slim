# RFC-0165: Checked ordinary project input producer

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, under delegated overnight ordinary-library direction
Created: 2026-10-02
Approver: project-maintainer
DecisionDate: 2026-10-02
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

Add an experimental ordinary SLIM `project-input SNAPSHOT/slim.project`
application that captures and checks one project through the existing production
project implementation, then emits lossless source bytes and workplan input.
A fixed Python measurement adapter hashes those serialized bytes into catalog
input. Neither tool adds language operations, compiler semantics, dependencies,
or a second parser/checker. Existing catalog and workplan consumers remain the
authorities for their respective admitted data contracts.

## Motivation

Repeat compiler dogfooding from maintained tools rather than one-off inventory
recipes. Exact captured bytes permit source identity and edit reconciliation;
the existing direct-import graph permits planning. Byte weights are evidence of
source extent, not compile time, model tokens, or successful development.

## Guide-level explanation

Build the root `project-input.project` with the production compiler. Run it
against a stopped-writer project snapshot, redirect its successful framed output
to a capture file, and pass that file to the fixed inventory adapter. Feed the
resulting catalog and graph to the existing ordinary SLIM applications. Retain
the successful producer invocation receipt separately from transport metadata.

## Reference-level specification

The root manifest reuses the existing selfhost project modules and their exact
import/export interfaces with root-relative paths, plus ordinary ASCII, byte,
text and netstring modules and the application entry. Do not modify production
selfhost APIs, copy their implementations, or add a compatibility fallback.
Capture once with `project.capture_project_input`; prepare and completely check
the same `ProjectInput` using the existing cold caches, revision and configuration.
Require complete successful preparation and read flags for every module.
Capture status alone does not establish checked acceptance. Inspect canonical
manifest tokens and existing checked structures; never parse project syntax
independently or reread sources to form a successful transport.

The transport contains netstring frames in exactly this order:

1. `slim-project-input-1`.
2. Canonical decimal module count N.
3. Canonical decimal direct-import edge count E.
4. Exact captured manifest bytes.
5. Exact workplan input bytes.
6. N triples of module name, manifest-relative path and exact captured source,
   in strict manifest byte order, followed by exact EOF.

Workplan input uses its existing canonical triples: module name, exact source
byte length, and comma-separated direct imports in the checked manifest's order.
Do not impose a new acyclicity condition on accepted SLIM projects. A graph
accepted by the compiler can still be declined by the workplan consumer with
its existing cycle diagnostic; that consumer decision is separate.

Admit N <= 4,095 (reserve one catalog row for `@project`), E <= 65,536,
module names <= 64 bytes, paths <= 256 bytes, manifest and each source <= 1 MiB,
aggregate manifest plus source bytes <= 4 MiB, flattened canonical parsed node
entries <= 1,000,000 (including structural markers, not lexical or model tokens),
workplan and anticipated catalog serializations each <= 1 MiB, and
complete transport <= 8 MiB. Every netstring payload retains its existing 1 MiB
limit. Preflight all counts, lengths and arithmetic with I64-safe admitted
operands before emitting successful output. Use the existing checker limits;
do not raise or relax any compiler, consumer or performance budget.

## Compiler and runtime design

All source capture, checking, graph extraction, validation and serialization
are ordinary SLIM with visible alloc, io and partial effects. The Python adapter
performs measurement-only hashing and transport validation. No source form,
runtime ABI, production compiler path or cryptographic primitive is introduced.
Build the complete report before its single print. Allocation failure retains
exit 71. Ordinary stdout may expose a prefix on host write failure; this is not
transactional I/O.

Capture currently reads complete files before these logical admissions.
Preparation has its existing parsing/checking work and storage costs. These
logical limits do not promise preread physical bounds, RSS bounds, symlink or
realpath containment, atomic live capture, or a new end-to-end CPU budget.
The accepted result describes the exact bytes supplied to the existing checker.
Public component records are not opaque. Collection and report guarantees apply
to the unmodified captured `ProjectInput` and its derived view after successful
source admission and complete preparation. Forged ordinals/spans or changed
records retain checked partiality; component exports alone do not certify source
acceptance. The fixed application enforces this ordering.

## Diagnostics and failure cases

Invocation error exits 64. Capture failures precede source extent admission.
Failures reached during capture or complete source preparation retain their
existing diagnostic codes and source offsets and exit 65 for this tool.
For a missing manifest, explicit project capture emits `E0409@-@0:0`.
The normal CLI content-sniffs input and dispatches a missing path as a single
source, emitting `E0409@0:0` instead; these distinct existing operations do not
promise byte parity. After source extent admission, readable manifest failures
reached by complete preparation retain full diagnostic parity. Source extent
admission precedes preparation: an oversized source receives logical error 10
even when the normal checker would report a type error in its body. Logical
admission failure is `error 10 at 0`, exit 65. Internal capture/count/
serialization disagreement is `error 11 at 0`, exit 65. Before successful
serialization, validation or allocation failure publishes no successful framed
prefix. Existing checker diagnostics remain authoritative and are not replaced
by a different SLIM semantic interpretation.

## Compatibility and migration

There is one transport spelling and no legacy fallback. The fixed adapter CLI
is `scripts/project-input-inventory.py --capture CAPTURE --catalog CATALOG
--workplan GRAPH --receipt RECEIPT`, with fresh distinct output paths. Read at
most 8 MiB plus one byte and reject overflow, malformed/noncanonical framing or
numbers, count mismatch, trailing bytes, duplicate/order violations, and
unsupported transport identity. Parse only transport data, never SLIM or project
manifest semantics. Do not execute commands selected by input data or reread
source paths from the transport.

Hash the captured manifest and each serialized module source with SHA-256.
Emit the existing compact catalog triple format, sorted by key, containing
`@project` and each module name: exact byte weight and value consisting of
manifest-relative path, NUL and 64 lowercase digest bytes. The manifest row uses
the fixed path label `slim.project`; module names cannot collide with `@project`.
Write the supplied workplan bytes unchanged after transport contract checks.
Preflight both existing 1 MiB consumer limits before output creation. A separate
receipt pins observed adapter source, capture, output catalog and graph identities
and captured manifest/module digests and lengths. Retain each original module
path as hexadecimal bytes. The receipt is compact canonical JSON capped at
4 MiB; omit artifact filesystem path labels, retaining their digests and extents.
At the admitted limits, a module record is at most 693 bytes and the complete
4,095-record array at most 2,841,931 bytes; fixed identity metadata fits within
the remaining allowance. Preflight the exact encoded receipt before creating
outputs. Outputs are data, not code. Host write failures can leave partial fresh
files; no atomic multi-file transaction is claimed.

The adapter alone does not prove that arbitrary supplied capture bytes passed
the compiler. Such a claim additionally requires the matching successful trusted
producer invocation. Transport identity spelling and a receipt do not substitute
for that authority. Writers remain stopped during capture and receipt collection;
observed source bytes do not attest interpreter bytecode or exclude ABA changes.

## Performance and complexity

Separate existing capture/checking work from additional transport serialization
and hashing. Additional collection visits source bytes and manifest records
approximately linearly; output size is explicitly capped. Do not infer complete
compiler scaling, physical resource bounds, source tokens, or model productivity
from these limits. Measure source-bound finite runs and geometric transport work
where supported. Keep all existing permanent gates and budgets unchanged.

## Alternatives and drawbacks

An inventory-only Python parser would duplicate project acceptance. A new hash
primitive or runtime filesystem API adds language cost without necessity.
Embedding all sources in the compact catalog exceeds its existing admission.
The lossless intermediate transport adds explicit buffering and I/O costs; it
allows the existing compiler to remain the sole semantic authority.

## Test and acceptance plan

Use the production compiler, ordinary and sanitized execution, independent
transport/data oracles and the real compiler project. Cover nested paths and
dotted names, same-size source edits, path moves, manifest import/export edits,
private/unimported references, type/effect/ownership rejection, missing files,
duplicate/order/parent-path diagnostics, compiler-accepted cycles, all declared
admission crossings, noncanonical/truncated/trailing transports and allocation
failure. Compare actual module names, raw source hashes/lengths and exact direct
imports independently; never treat success of the adapter as source acceptance.
Preserve earlier passing gates, required repository checks and one concise
current result; raw attempts belong in ignored build storage.

## Ratings and evidence

Ratings are neutral placeholders, not claims of no cost. No primitive is added.
Utility and general development effectiveness remain unknown until independently
observed; the intended benefit is repeatable source-bound measurement.

## Decision

Explicit coordinator acceptance on 2026-10-02 under the maintainer's delegated
direction to choose and implement useful overnight work. Acceptance precedes
implementation. The scope is ordinary experimental tooling with the same
production compiler and unchanged budgets. Full verification remains required.

## Implementation

Ordinary producer code belongs in `library/components/project_input_*.slim`
and `library/applications/project_input/main.slim`, exposed by the root
`project-input.project`. Fixed measurement and finite verification scripts belong
in `scripts/`. No production selfhost/compiler edit is authorized by this RFC.

The ordinary producer and adapter pass source-bound repository/release/website
integration at `915f836`. Permanent native boundary verifiers cover exact
4095-module/65536-edge admission and their single excesses, plus the fixed
999999/1000000/1000001-node and aggregate 4 MiB minus/exact/plus families.
The current prepare API supplies node counts; source excess is declined before
preparation and retains an unknown count. Ordinary and sanitized gates pass;
both explicit bounded verifier commands are wired into `scripts/verify.sh` for
the next coherent full invocation. Fresh fixtures are generated, never historical
gate inputs. Source identities and finite scope remain in the
[current record](../../benchmarks/results/project-input-current.json).

## Removal and supersession

Replace with smaller ordinary composition only if exact byte provenance,
existing checking authority, deterministic diagnostics and permanent gates are
retained. Never preserve a second parser, semantic fallback, or weakened budget.
