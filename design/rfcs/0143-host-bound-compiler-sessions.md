# RFC-0143: Host-bound compiler sessions

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-08
DecisionDate: 2026-09-08
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

Expose the existing transactional retained SLIM engine through one public
`slimc session` stream. A compiler-specific C adapter supplies bounded transport,
real build identity, diagnostic capture and physical epoch ownership. The adapter
calls the generated SLIM engine; it does not parse, check, analyze or emit source.
Retire the old public invalidation-estimate command during this implementation.
Native artifact caching and global-analysis retention remain M1 obligations.

## Motivation

At checkpoint 7ce3fdf, retained parsing, typing, memory plans, range/input queries
and C fragments exist, but the public session command still prints estimates.
The source language intentionally lacks general process, interactive stream and
file-publication primitives. Adding those language features to expose a compiler
service would cross M1's boundary. The existing shell launcher already provides
host orchestration for one-shot builds.

## Guide-level explanation

A client starts `slimc session`, reads its versioned identity greeting, and sends
complete update requests over stdin. Each update names a project manifest; the
SLIM engine captures its module bytes once and validates the whole candidate.
A complete successful response contains actual checking/reuse data and the exact
current C artifact. A rejected update returns current diagnostics and the
last-good revision identity without presenting old C as the rejected result.

A reset closes the owning epoch, releases its storage and invalidates its handles.
The next update is cold. EOF or quit closes the service. No source program is
executed by this command. Semantic/debugger queries remain M3 work.

## Reference-level specification

### Sole engine and generated boundary

Build a separate host executable in `build/toolchain/` from `compiler/session.c`,
the verified generated C seed and the existing C runtime. The adapter includes
the seed in the same translation unit with its ordinary `main` renamed. This is
a private build boundary, not a new public generated-code ABI or parsed IR.
Changes to generated record/function names must fail compilation rather than
select another semantic implementation.

The initial adapter may call only the generated equivalents of `session.start`,
`session.update_path`, `session.artifact` and
`session.usage`. It stores the returned owning state and borrows it only through
these operations. It may read the public Report/Usage/configuration scalars and
borrow the selected artifact; it may not construct semantic records, mutate
snapshots, transplant vectors or edit cached facts. Normal SLIM ownership and
checker acceptance remain authoritative. No production Rust path is added.

The SLIM engine retains responsibility for capture, parsing, linking, checking,
dependency invalidation, analysis, C production and candidate publication.
Its `update_path` entry checks the attempt admission limit before capture. On
exhaustion it submits an empty owned input to the normal `update` operation,
which remains the authority for the capacity rejection and last-good identity.
Repeated rejected requests must not read files or accumulate captured inputs
after all 64 attempts have been consumed. Reset remains available.
The public adapter does not use the estimate-only query API. The bare seed remains
the reproducible one-shot engine; the shell launcher selects the host executable
for the session command, as it already orchestrates native build/run commands.

### Build identity

Bootstrap produces a private identity header alongside the host build. Its
compiler digest includes verified seed bytes, adapter bytes, runtime header/source,
native compiler identity/version, target and exact build flags. Runtime identity
includes its exact source/header and ABI version. Target and options describe the
actual compiled host context and fixed session operation. Use the existing
bootstrap SHA-256 facilities; no new mandatory library dependency is introduced.

The generated header is consumed by that same build. Compile captured source
inputs in a private directory. Publish the completed host executable through a
unique same-directory staging file and an atomic rename; simultaneous builders
must not share a temporary executable filename. Sidecar build records are
diagnostic data; the loaded worker greeting is its authoritative context. Configuration is immutable
for the lifetime of the loaded worker. Requests cannot supply fingerprint strings
or import history. Rebuilding/restarting with different compiler, runtime, target
or build options creates a new context and cold state. Changing files on disk
does not replace code already loaded in a worker. Report that loaded build's
identity; do not pretend it hot-reloads. This construction binds in-memory
history to one actual compiled engine. It does not authorize reuse of arbitrary
persisted executable artifacts; their complete include/link/backend identities
remain part of the subsequent native-cache contract.

### Transport version 1

Each frame is one ASCII tag byte, one unsigned 32-bit big-endian payload length,
and exactly that many payload bytes. Unknown tags, invalid lengths, trailing
operation fields and truncated frames are terminal protocol errors. Read a whole
request before invoking the engine. Empty EOF between frames is clean shutdown.

Requests are `U` with a nonempty project-path payload of at most 4,096 bytes and
no NUL, `R` with zero payload to reset, and `Q` with zero payload to quit. Paths
are byte strings, including spaces/newlines; no shell evaluates them. No request
contains source facts, options, executable code or a serialized snapshot.

The initial `H` greeting is ASCII TSV containing protocol/version and the compiled
compiler/runtime/target/options identities. Revision handles are meaningful only
on this connection. An `A` reset acknowledgement contains the new epoch as an
unsigned 64-bit big-endian word. Quit returns an empty `Q` acknowledgement.

An update `S` response payload contains these fields in order:

1. Eight signed 64-bit big-endian words: attempted epoch/serial, published
   epoch/serial, status, executed checks, reused checks and imported typed nodes.
2. One byte each for capacity and whole-snapshot reuse (0 or 1).
3. One signed 64-bit word for actual whole-artifact generations, then one byte
   for whole-artifact reuse.
4. Three unsigned 32-bit lengths: reason, diagnostics and generated C.
5. Those three byte strings in that order.

The fixed prefix is 87 bytes. Negative work retains its existing unknown meaning;
it is never changed to zero. Source rejection carries zero C bytes even if a
last-good artifact exists. A success carries the artifact for its published
revision. Whole-artifact copying/output remains work even on an internal hit.
Detailed query counters remain independently observed rather than inferred from
these aggregate fields.

An `E` transport error contains a stable ASCII code: H0001 malformed/truncated
frame, H0002 invalid path, H0003 diagnostic/frame capacity, H0004 output failure,
or H0005 inconsistent engine response. Terminal host allocation failure uses
H0006 and status 71. Protocol/capacity errors use status 65; invalid CLI use is
64. Broken output may prevent sending an error frame.

A client accepts a result only after receiving the complete outer frame. The
server cannot read a next request until writing/flushing the current response
succeeds. A partial response, capture failure or output failure terminates the
connection and destroys the epoch; no later request can reuse an unacknowledged
candidate. A previously received artifact retains only its own revision identity.
The server does not publish files or native executables in this contract.

## Diagnostics and failure cases

The compiler's current diagnostic functions write through the runtime print
operations. For the host executable only, link bounded diagnostic-capture
implementations of those same three print symbols. Compile the ordinary runtime
print definitions under private unused names; do not change runtime source,
source built-ins, application output semantics or the public runtime ABI.

Capture preserves every byte and order from the existing compiler operations.
Its cap is 1 MiB. Overflow or allocation failure is terminal, never a truncated
successful diagnostic report. Runtime traps/status-71 stderr retain their current
meaning. The host capture buffer and request storage are measured separately from
SLIM epoch allocations. No interception exists in ordinary application builds.

Use existing Limits: 64 attempts, 64 MiB captured input, 1,000,000 canonical nodes
and 64 MiB C bytes per epoch. Existing smaller internal limits remain independently
tested. The C adapter owns one root SlimRegion/SlimAllocStatus; reset calls its
normal shutdown before initializing a new root and incrementing the epoch.
Epoch overflow terminates without reuse. Do not silently reset on saturation.

A failed SLIM allocation terminates the connection with status 71 after normal
cleanup. It does not clear failure flags and continue with partially constructed
state. Diagnostic/request host allocations have a separate bounded fault-injection
domain. Source rejection is recoverable through the existing last-good snapshot.

## Compiler and runtime design

Keep host framing/capture/build glue under `compiler/`; keep production semantic
changes in `selfhost/`. Bootstrap and source release/install include the adapter
and its generated identity recipe. The seed fixed-point gate still runs against
the unwrapped engine. No seed text is patched to add semantics after generation.
Extend the exact frozen source-package root inventory with `compiler` for this
adapter; retain the existing roots and the gate against any unapproved addition.
The shared Rust estimate-fixture builder is classified as verification
infrastructure. It invokes the production compiler and contains no semantics.

This child permits the narrow host adapter beyond RFC-0124's original runtime-only
orchestration sketch. It adds no general source host/FFI/process/allocator feature,
new runtime ABI, native backend or mandatory dependency. Native compile/cache
publication is intentionally still a separate required M1 implementation.

## Compatibility and migration

Replace the old public `session INITIAL UPDATED [RECOVERED]` estimate output with
the framed session command. Reject old arguments explicitly; do not silently
reinterpret them. Remove the estimate dispatch from the bare engine and document
that the public launcher owns session transport. Keep one public operation.

Preserve durable invalidation-estimate fixtures and work budgets by moving their
old SLIM driver into measurement fixtures compiled by the production compiler.
Their metrics remain labelled estimates. Do not relabel old synthetic counts as
actual session work or drop a gate because its public command was replaced.
Add independent public-session work/latency gates. One-shot source/diagnostic/C
behavior, library/runtime ABI, safety rules and all existing budgets remain.

## Performance and complexity

Framing/capture is linear in transmitted bytes with fixed checked bounds.
Connection setup fingerprints/build work, request capture, retained queries,
response copying and external backend work are reported separately. No-change
updates must execute zero parsing/checking/generation producers while still
reporting real capture/comparison/output costs. Measure source/seed/host growth,
allocations, live memory, cleanup and quiet same-host cold/warm edit latency.
The public generated-helper fixture uses 125, 250, 500, 1,000, 2,000 and
4,000 helpers, nine measured samples after warmup, complete response comparison
and separate startup/reset/one-shot timings. Preserve all samples. New durable
budgets, owned by RFC-0030’s durable performance contract, are a maximum 1.25 endpoint geometric exponent for cold, unchanged and
body-edit request medians, and a maximum 0.25 unchanged/cold same-host ratio at
4,000 helpers. These use the existing approximately-linear exponent convention;
the initial unchanged ratio is about 0.075, leaving explicit transport/noise
headroom. They do not assert faster body edits than clean compilation or agent
success. Existing gates remain unchanged. No performance improvement is assumed
outside the measured domain.

## Alternatives and drawbacks

General source stream/process primitives would expand language scope. A Rust or
C checker would duplicate semantic authority. Starting a fresh compiler for every
update would discard the requested retained state. Passing arbitrary caller
fingerprints would not bind actual host context. Keeping the estimates under the
public session label would still leave the intended workflow unavailable.
The adapter adds a native build and a private generated-entry dependency; both
must remain reproducible, tested and explicit.

## Test and acceptance plan

Exercise the public launcher and installed release. Decode complete frames and
compare diagnostics and raw C with clean production runs across the full corpus,
body/interface/layout/effect/borrow-mode/configuration edits, insertion/deletion/
reordering/relocation and rejection/recovery. Observe actual query calls/imports,
not report labels alone. Cross each real build identity with distinct builds.

Test fragmented/truncated/oversized requests, NUL and unusual paths, unknown tags,
wrong reset/quit lengths, partial/broken output, diagnostic capacity, every epoch
limit, source/host allocation faults and physical cleanup/reset. A failed frame
must never become an accepted result. Keep ordinary and sanitizer differentials,
source-span identities, exact native analysis/resource baselines and all existing
performance gates. Run required checkpoint checks and full M1 release closure.

## Ratings and evidence

Analysis +2 exposes actual checked snapshot identity and transactional reuse;
score 15. Other ratings remain zero until measured. Productivity and public
latency benefits are unknown. This is an implementation contract, not evidence
that the public service is complete.

## Decision

Accepted under the maintainer's RFC-0112 delegation and the active M1 goal.
No performance, safety or language-feature hard gate is relaxed. This is a child
host-architecture decision, not a new source primitive or external review.

## Implementation

Implemented and verified from baseline checkpoint 7ce3fdf. The public adapter,
source admission entry and legacy-driver migration pass the
[initial checkpoint](../../benchmarks/results/2026-09-08-m1-host-session.md) and
[public acceptance campaign](../../benchmarks/results/2026-09-08-m1-host-closure.md):
edit/identity/default-limit/transport differentials, source and host allocation
faults, physical resource accounting and durable public latency gates. The
unchanged production implementation passed clean installation at `2ffc37b`.
Measured large body edits remain slower than fresh compilation. Global analysis
retention, native object/link caching and full M1 release closure remain required;
this child completion does not close its parent milestone.

## Removal and supersession

Preserve one semantic compiler, complete host/source identity, connection-scoped
revision handles, bounded framing, last-good source recovery, explicit physical
epoch reset, conservative failures and all durable work and performance gates.
