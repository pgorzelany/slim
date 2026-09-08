# RFC-0146: Native builds in compiler sessions

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

Connect RFC-0145's production SLIM native queries to the public compiler session.
A checked revision selects accepted C in SLIM. A narrowly defined host adapter
captures the actual native toolchain, executes object/link misses against those
captured inputs, and returns a complete framed native artifact. No source program
executes inside the service. Ordinary one-shot build/run behavior is unchanged.

## Motivation

The public session already returns accepted C. The verified native query library
retains complete native inputs and outputs but is not used by public builds.
Private experiments establish native behavior across all twenty applications,
all three fixed header profiles and captured-toolchain execution on the current
Apple arm64 host. Per-build hashing of large backend images can erase savings;
a private native context therefore remains fixed for the loaded connection.

## Guide-level explanation

After a successful `U`, a client can request a native artifact for that exact
published revision. The first native build captures its native context and builds
program/runtime objects and their executable. An unchanged request can retrieve
all three artifacts. A source edit normally recompiles the complete generated-C
translation unit while reusing the runtime object. No per-function object reuse
is promised.

The response carries the executable bytes, like `S` carries generated C. Clients
accept only a complete response and publish files with a fresh same-directory
staging inode followed by atomic rename. The server does not accept output paths
or overwrite client files. This avoids pretending that a file rename and a stream
acknowledgement form one atomic transaction. A failed or partial response leaves
previously received artifacts and client outputs intact.

Reset destroys frontend and native query storage. It preserves the captured
read-only native context, just as it preserves the loaded SLIM compiler and paired
runtime. Restart the connection to capture a changed native toolchain. Native tools
and paired runtime do not hot-reload within a connection. There is no hidden daemon, imported
executable cache, general process/FFI feature, source debugger or M2 language change.

## Reference-level specification

### Source authority and private generated boundary

Add `nativebuild.select` in production SLIM. It validates the requested positive
epoch/serial, worker selection, current successful snapshot ownership, complete
snapshot framing and C-artifact integrity. It returns either an explicit rejection
with empty C, or the exact accepted artifact and actual worker profile. A handle
for an earlier successful revision becomes stale after a new success. A rejected
source update does not authorize its attempted handle; the explicitly named
last-good handle remains usable until replaced or reset.

The selection derives the worker profile from the accepted generated prefix;
it does not reanalyze or broaden parallel execution. With no parallel prefix,
the native profile is 0. Otherwise serial and POSIX requests select RFC-0145
profiles 1 and 2. Unknown profiles are rejected before native work.

Extend RFC-0143's private adapter boundary only with this selection operation and
RFC-0145 start/lookup/publish. The host may construct request/key scalars and supply
opaque owned native inputs. It may not construct checked source records, mutate
query entries, bypass a rejected selection or infer source acceptance from a
native artifact. The SLIM compiler and portable seed remain the sole semantic
path. Keep the native table in a separately accounted owning region; a failed
allocation terminates the connection and neither failed region is reused.

### Captured native context

The first provider is explicitly `apple-clang-arm64-v1`: the tested Apple clang
21 C11/O3 toolchain and macOS SDK execution shape. Other providers remain an
explicit unsupported native-context result until they have a complete capture
contract and native differential evidence. This does not change any existing
one-shot target or source conformance row, and does not claim universal native
cache support. The capture must recognize actual tools, not accept a request's
fingerprints or imported directory as authority.

A native context binds the loaded SLIM host identity, its paired embedded runtime
C/header, actual captured clang/ld and non-system helper images, resource and SDK
header closure, SDK settings, complete System stubs and compiler runtime archive,
ordered effective flags/target/environment, and host OS/shared-cache image.
The recipe and embedded input bytes participate in the host build identity.
Capture executes the same embedded recipe even if checkout files later change.
All native execution uses those captured files. Hashes identify the captured
bytes; no old digest authorizes execution of a mutable original pathname.

Use existing host facilities and explicit compiler-specific shell/C glue under
`compiler/` and `scripts/`; no mandatory Python, Rust, third-party library or new
source primitive is introduced. The installed source package includes the recipe.
Discovery utilities are trusted host orchestration, never source acceptance.

Copy tool images into their original relative bin/lib layout. Resolve and verify
non-system dynamic dependencies and loader search paths. Reject unsupported
absolute non-system dependencies or unresolved search behavior; copying a file
without redirecting its actual loader reference is insufficient. Bind system
libraries to the current boot/OS and dyld shared-cache identity. This is a native
host trust assumption, not a proof against malicious kernel changes, a same-user
process rewriting private files, or arbitrary memory corruption.

Capture the union of headers used by the complete generated prefix and paired
runtime in profiles 0/1/2. Compare both preprocessed tokens and macro definitions
against the selected original context for every profile after capture. Compile
with `-nostdinc` and explicit captured resource/SDK roots. New files in ambient
include directories cannot change native execution within the loaded connection.
Check the captured compiler's actual header dependency reports as well: each
positive header input must be a captured manifest member or the exact paired
runtime/local probe source. Token equality alone would not detect an absolute
include that still reads the original SDK.
The generated C has only the fixed compiler-controlled prefix and no arbitrary
user includes or foreign library directives. A future emitter/runtime change
that breaks this closure requires revisiting the contract.

Link with `-nostdlib`, the ordered complete program/runtime objects, and explicit
captured System/compiler-rt inputs. Preserve the entire compiler-rt archive and
complete transitive SDK stub closure, not just members selected by one example.
Check actual driver jobs and link dependency outputs; reject unsupported outside
inputs. Do not treat a linker's negative-lookup report as a complete filesystem
watch list. Eliminate ambient library selection in the accepted execution shape.
Disable default compiler configuration files with `--no-default-config` for all
discovery, preprocessing, compile and link invocations. Validate the actual
`-###` commands for both C units in every worker profile and for the link. Admit
one known clang `-cc1` or linker job only, with fixed file operands and captured
paths; reject unknown switches, response files, extra jobs or ambiguous quoting.
Reports are at most 1 MiB, with at most 512 arguments of at most 4096 bytes each.
The recognized non-file target/version options remain explicit in the captured
job records and therefore in the context identity. This validation supplements
actual preprocessing equality and positive link-input checks; it cannot replace
them or establish trust in an arbitrary compiler binary.
Retain the validated captured-file manifest with the connection, bounded to 512
rows of 64 hexadecimal digest bytes, a tab, at most 4096 relative-path bytes and
a newline. Every positive native link input must be an exact manifest member or
one of the two request-owned objects. Require both objects, System and compiler-rt
in the report, exactly one leading version record and the exact requested output.
A path merely inside a private SDK/toolchain subtree is insufficient evidence.

Use a fresh private directory owned by the session, with a maximum of 512 captured input
files and 536,870,912 logical captured bytes. Reject path escapes, ambiguous or
unsupported dependency framing, changing copies and incomplete closures. Capture
failure publishes no context and removes its temporary files. A declined context
remains declined until reset; repeated builds do not accumulate failed captures.
Discovery has at most 8192 provisional input references, 512 tool images, and
1 MiB per dependency/profile report. A captured C helper copies the complete file
list in one process, compares original/copied bytes, and hashes copied bytes with
the platform System/CommonCrypto SHA-256 API. Its executable is embedded in the
host build, not loaded from a mutable sidecar. This adds no third-party dependency.
The final private manifest binds that helper through the loaded host identity.
File limits are operational limits, distinct from RFC-0145's retained-byte limit
and the agent's unbudgeted implementation goal. Measure physical storage, startup,
copy/hash work and steady-state work separately.

Native subprocesses receive a fixed documented environment and argument vector;
no request is interpreted by a shell. The context identifies the effective target,
compiler flags, worker options and environment. It uses canonical C11/O3 flags
and existing serial/POSIX worker semantics. Bound and drain subprocess diagnostics
and enforce a 180-second wall/CPU limit per operation, a 64 MiB output-file limit
(512 MiB during input capture), and the 1 MiB diagnostic limit; overflow or failure never
publishes a partial native result. Clean up/reap each successfully started process
and its children before another request. No application executable is run.
Keep each process-group leader unreaped until its group has no live members, then
reap the leader. A completed leader may leave descendants with either open or
closed output streams. The Apple provider checks at most 512 group members using
the system process API, including a sentinel to detect truncation. Missing process
details mean exited only for ESRCH; other lookup failures remain unknown. A group
signal's EPERM is insufficient evidence of either a live group or an empty one.
After normal exit or an operation limit, allow at most five seconds to confirm
group termination and reap the leader. Failure to confirm is terminal H0007,
not a recoverable build miss or a successful shutdown acknowledgement.

### Transport extension

Preserve the exact protocol-1 greeting and every existing `U/R/Q`, `S/A/Q/E` frame.
Add `B` with exactly 17 payload bytes: unsigned 64-bit big-endian epoch and serial,
then one worker-selection byte (0 serial, 1 POSIX). Values outside positive I64 or
unknown worker selections produce a native request rejection. Malformed framing
remains terminal H0001. The server reads the complete frame before selection.

A native response uses tag `N`. Its fixed 116-byte prefix is:

1. One schema byte, value 1.
2. Five signed 64-bit words: requested epoch, requested serial, status, actual
   worker profile (-1 when unavailable), and total native-request nanoseconds.
3. Three unsigned 64-bit observed process-start counts: program object, runtime
   object, link. These count actual backend starts in this request, independently
   of lookup results; context setup is separate.
4. Three bytes for program/runtime/link hits (0 or 1).
5. Four unsigned 64-bit nanosecond values: context setup, program process,
   runtime process and link process. Setup includes its own external discovery
   and validation work; it is not reported as zero-cost object compilation.
6. Four unsigned 32-bit lengths: context identity, reason, diagnostics, executable.
7. Those four byte strings in order, following the fixed prefix.

Status 0 carries a complete nonempty executable for the named accepted revision.
Status 1 is invalid/stale source selection, 2 unsupported/failed context, 3 native
process or I/O failure, and 4 native resource limit. Failure carries zero executable
bytes. Context identity is at most 1024 bytes, reason at most 4096, diagnostics at
most 1 MiB, and executable at most 64 MiB. Terminal host/SLIM allocation and
transport failures preserve RFC-0143 behavior. Bounded/unknown native evidence is
an explicit reason, never a successful empty artifact or fabricated zero work.

Successful responses report any retained-query failure as `cache-ROLE:REASON`
in the reason field (semicolon-separated in program/runtime/link order). Preserve
the original corruption reason rather than replacing it with a later disabled
lookup. A cache-capacity miss may still produce a successful fresh native artifact; it
does not expand or silently reset retention. Corrupt/conflicting query history
follows RFC-0145: disable retention, keep charged storage, and execute actual
native work if possible. No stale bytes may be returned as a fresh revision.
Native output buffers and temporary objects are request-owned unless the SLIM
query successfully clones them. Release all request storage on every outcome.

### Publication and reset

Only successful actual native operations admit query records. Consume complete
external output before admitting an object or link result. A failed later link
may leave valid object records available; it never publishes an executable for
that failed request. Restored object bytes must be written completely before
linking. Work files remain private and use stable basenames; concurrent sessions
have distinct directories. Client output publication uses fresh staging inodes,
not overwrites of an already executed Mach-O inode.

A complete `N` frame is the artifact publication boundary. On broken output,
terminate and reclaim both epochs without appending an error inside a partial
frame. Every native response removes request work files. A reset physically destroys
native query storage before acknowledging the new epoch. Its next build is cold
with respect to program/runtime/link records, while using the identical immutable
connection-owned tools and headers. EOF, quit and terminal failures also remove
the captured context. Existing client artifacts remain independently owned bytes.
Cleanup failure is terminal H0007; do not acknowledge a completed connection
shutdown or epoch cleanup when the required cleanup failed.

## Compiler and runtime design

Keep request/source authorization and native query semantics in SLIM. The C host
owns only transport, native capture/process/file orchestration, timing and physical
region lifetime. Embed the paired runtime and capture recipe from the same
captured inputs used to build the host; sidecar files are not execution authority.
Extend build/install/release identity and source inventories coherently. Preserve
the reproducible portable seed and existing runtime ABI.

## Compatibility and migration

This adds a native artifact request to the current compiler session, not a second
public native compiler or source execution service. Existing one-shot `build`,
`run`, optimization levels, flags and target behavior remain unchanged. Existing
clients need not issue `B`; their greeting and responses remain byte-compatible.
No native target support is inferred from successful parsing or cached C alone.

## Diagnostics and failure cases

Record stable reasons for invalid request, stale/missing/corrupt accepted source,
unsupported/incomplete native context, capacity, process failure, malformed output,
I/O/publication failure and diagnostic exhaustion. Test failed updates followed
by explicitly requested last-good builds, successful replacement and stale-handle
rejection, reset, compiler/link failures and recovery. No malformed request may
start native tools before SLIM source authorization.

## Performance and complexity

Measure context setup separately from cold, unchanged, edited and reverted builds.
Observe backend starts and query execution independently. Retain clean native
controls, geometric generated-C/source sizes, memory/storage and copied bytes.
Report complete public latency, including framing/output and context amortization.
A hit count or a private Python experiment cannot establish a user-facing speedup.
Do not weaken frontend, native runtime, analysis/resource or release gates.

## Alternatives and drawbacks

Rehashing live backend images on every build is expensive and still does not bind
execution to the hashed bytes. Capturing a complete private context has substantial
startup/storage cost. Sharing imported executable caches or adding a hidden daemon
would enlarge the trust and lifecycle problem. Returning framed artifacts matches
the existing service and avoids an unachievable atomic transaction across client
files and stdout. It requires clients to publish complete returned bytes correctly.

## Test and acceptance plan

Test selection and each native role through the production SLIM compiler. Exercise
all source handle transitions, key/configuration changes, real unchanged/edit/revert
reuse, failed builds and recovery, byte-identical artifacts, bad native input/frame
and output paths inside the private workspace, allocation/diagnostic/process limits,
partial transport, reset/EOF cleanup and concurrent sessions. Observe zero backend
starts on unchanged builds rather than trusting the response flags alone.

Validate captured input closure, changes to original tools/headers/link inputs,
all fixed preprocessing profiles, the complete native application corpus and
serial/POSIX behavior against ordinary builds. Preserve all native analysis/resource
rows, required checkpoint commands, installed-source behavior and the full M1
release gate. Document unsupported contexts and actual physical costs. No library-
only or private-client test closes this public integration contract.

## Ratings and evidence

Analysis +2 exposes native execution, cache reuse and complete-context ownership;
score 15. Other dimensions stay zero until measured public costs justify a claim.
The RFC-0145 report records the private feasibility and query evidence. Acceptance
of this implementation contract is not evidence of its completion or productivity.

## Decision

Accepted under the maintainer's RFC-0112 child-decision delegation and active M1
goal. This authorizes the specified compiler-specific host integration, including
its platform capture glue, and preserves the full original M1 exit criteria.

## Implementation

Complete at clean release checkpoint `4bdb6750656eeeac48b0d68cffc3423600434168`;
[the closure report](../../benchmarks/results/2026-09-08-m1-closure.md) records
full verification and current public costs. Tests pass selection and public cold/unchanged/
edit/revert/reject/reset artifact round trips. All 20 native applications pass
ordinary and ASan/UBSan corpus checks with independent backend-start observation.
The process wrapper now retains its leader until group quiescence and reaping;
its permanent tests cover surviving descendants, closed streams, actual file and
diagnostic limits, failed process queries, and accelerated-clock deadline checks.
Public malformed/partial frames, interrupted executable output and six cold-path
native allocation failures pass with observed cleanup. Default compiler configuration
is disabled consistently. Six actual compile jobs, one link job and captured header
dependencies pass bounded command/input validation; permanent rejection cases cover
unrecorded paths, plugins, response files, extra jobs and report/argument limits.
The copy helper passes ordinary and sanitizer byte/path/failure tests, including
512/513 files, exact 512MiB storage and one-byte overflow, forced fallback and
changed copies. The large storage boundary runs in the ordinary variant.
Private original-tool/SDK removal tests preserve five freshly compiled and executed
application/profile results; this is an input-independence test, not a public
latency measurement. Five public request-path failure/recovery cases pass under
ASan/UBSan with independent exec observation: failed source writes, program and
runtime compilers, linker output and cleanup after a full cache hit. Failures
return no executable; retries reuse completed valid objects and execute exact
recovered bytes. Public state tests now also corrupt metadata and each retained
artifact role, verify exact conservative miss reasons, preserve disabled history
until reset, and cross the real 256-record limit using both worker profiles within
64 source updates. A separate reduced-byte-limit fixture verifies capacity fallback
and reset; it does not measure the physical 64MiB boundary. Verification-only
process-query and per-role SLIM publication-allocation faults produce terminal
H0007/H0006 without an artifact or trailing frame, with actual teardown observed.
Public resource campaigns cross the real 64MiB byte limit, observe native-region
allocation headers and actual frees, and count cloned bytes in executed SLIM loops.
Geometric history and the byte boundary pass in optimized and ASan/UBSan observers;
unchanged native builds clone no bytes and reset frees the entire native region.
Sanitized concurrent capture/edit/reset/quit histories preserve separate files and
query storage. A one-shot unavailable-provider fixture proves decline persistence
without tool starts and successful retry after reset. These observers are
verification-only; their process RSS includes frontend/allocator costs and their
per-file block totals do not measure unique disk ownership of cloned extents.
The first capture approach cost
about 16 seconds, and batch capture observations still cost about 9 seconds.
This measured cost led to separating connection-owned immutable tool inputs from
resettable query storage before acceptance closure. No performance gate is relaxed,
no query result crosses a reset, and refreshing tools requires an explicit new
connection. The complete context/failure/resource campaigns, integrated costs
and release verification passed. The closure report and immutable archive retain
public behavior, complete identities, measurements and failure/recovery evidence.

## Removal and supersession

Any replacement must preserve the SLIM source authority, exact native query keys,
actual captured execution inputs, observed backend work, complete framed artifact
publication, bounded storage, explicit unsupported contexts and physical cleanup.
