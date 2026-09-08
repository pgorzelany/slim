# RFC-0155: Bounded host-adapter storage

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-08
DecisionDate: pending
Approver: project-maintainer
Kind: language
Primitive: none
Evaluation: m2-experiment
EvaluationRFC: RFC-0147
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 0

## Summary

Complete the hosted-storage part of RFC-0153: explicitly charged command-line
argument descriptors, bounded nonallocating file/TCP operations into reserved
storage, and output without buffered-stdio allocation. This is an M2 owned-byte
and operation-effect migration of existing operations, not a new general host
interface. It introduces no file/socket handle, FFI, DNS, asynchronous I/O or
extra result primitive. Worker domains and the complete ownership package still
require their own decisions. This draft changes no current runtime or source.

## Motivation

Current generated entry constructs a Vec of argv views through implicit region
allocation. The proposed allocator-only entry omitted that required compiler
input. `slim_read_file` allocates a NUL-terminated path, opens buffered stdio and
grows output. `slim_tcp_exchange` repeatedly grows an intermediate response,
then grows and copies into output. Printing uses printf/fwrite/fputc. Removing
`alloc` from these signatures without changing their implementations would be
false evidence. Owned Bytes also makes old copyable argument/response views an
invalid migration strategy.

## Guide-level explanation

Hosted programs choose their entry's resource requirements explicitly:

```text
fn main() -> I64
fn main(allocator: Allocator['host]) -> I64
fn main(allocator: Allocator['host], args: &'host [&'host [U8]]) -> I64
```

These are the three permitted hosted signatures, with their checked effect lists.
Parameter names are ordinary names; parameter order and types identify the
contract. `'host` is supplied by entry and is not a user lifetime binder. There
is no implicit argument vector, allocation capability or injected ordinary call
argument. An entry requesting args also requests the allocator that pays for
their descriptors. The compiler migrates to the third signature. A program
needing neither resource constructs no pool.

Before a host read, an ordinary source wrapper explicitly reserves its chosen
maximum output. The host operation cannot grow that storage. A successful
operation appends complete bytes; failure leaves the existing initialized output
unchanged. Application-owned result enums distinguish reservation exhaustion
from the existing Bool host-failure class. The compiler recognizes neither the
enum nor its variant names.

## Reference-level specification

### Hosted arguments and startup

Preserve argv order, including argv[0], exact non-NUL bytes and empty arguments.
Do not copy argument byte payloads. The host keeps their immutable native backing
valid throughout entry; source obtains only shared views with lifetime `'host`.
Source cannot mutate argv, destroy its backing, extend its lifetime or use a
cast from native pointers. Retained source data that must survive a host session
input needs its own explicit owned copy; an argv view is not a session owner.

After a successful requested root-pool reservation, validate argc and every
argument length against I64 and native extent limits. Allocate one contiguous
descriptor block of `argc * sizeof(SlimSlice)` using the root provider, with
checked multiplication and its ordinary plain header. Fill the descriptors
directly; no geometric vector growth or byte copying. Empty argc needs no block.
The descriptor allocation is the first positive source-domain provider attempt
when args are requested. Root host-reservation failure is a separate startup
event. Both failures report the named startup stage and exit 71 before entry;
there is no partially supplied args value or sticky failure after entry.

The host retains the descriptor block outside source ownership and releases it
after entry's normal local cleanup and joined workers, before root teardown.
Its storage remains charged to live root bytes throughout entry. No source
owner can free it. The root lifetime covers args and the requested capability;
main returns only I64 in 0..255, preserving the current checked exit contract.
Freeze descriptor layout and capture it in the changed ABI identity. Zero-sized
or unvalidated C slices never authorize pointer arithmetic.

### Canonical host signatures

Lifetimes below are explicit signature metavariables, not generic source code:

| Operation | Successor contract |
| --- | --- |
| `io.read_file(path, limit, output)` | `path: &'p [U8]`, `limit: I64`, `output: &'o mut Vec['d, U8]`; returns Bool, no allocation. |
| `io.tcp_exchange(address, port, request, response_limit, timeout_ms, output)` | Shared byte slices for address/request, I64 bounds, exclusive U8 vector reference for output; returns Bool, no allocation. |
| `io.print_i64(value)` | I64 to Unit, fixed native decimal scratch, checked output failure. |
| `io.print_bytes(value)` / `io.println(value)` | Shared byte slice to Unit; no owned byte copy or heap buffer. |
| `io.monotonic_ms()` | Preserve I64, nondecreasing per-thread observations and the current failure/clamping contract. |

Read/TCP retain `io` during the initial storage cutover and require `io, block`
when the separate effect child introduces `block`. Printing also requires those
ceilings at that later cutover and retains its exact checked trap hazard.
Clock reads remain observable I/O; lack of a source allocation is not evidence
of purity, a guaranteed event, total progress or a portable no-block proof.
Remove `alloc` only after the complete native path is audited and tested. An
ordinary wrapper that reserves storage still declares `alloc` independently.

Input slices must remain valid through the call. In particular, reject input
borrowed from the output owner while passing that owner exclusively. Require
the ordinary RFC-0149/0150 argument reservation rules even when capacity happens
to be sufficient. Capacity does not prove disjointness. No raw slice is created
over uninitialized spare storage for source use.

### File transaction

The candidate path bound is 4,095 bytes excluding the terminating NUL, fixed
before native implementation measurements. Reject embedded NUL, greater lengths,
negative limit, or `limit > capacity - length` before opening a file. The native
path buffer is exactly 4,096 bytes on the stack. An empty path reaches the normal
host failure class; it is never a successful empty file. This is a deliberate
new admission bound requiring corpus migration evidence, not a portable claim
that every path of that size is accepted by every filesystem.

Use an audited target-tier descriptor open/read/close implementation, not
fopen/stdio. Read at most limit bytes directly into output's unused capacity.
On reaching the bound, read at most one further byte into a separate scalar:
EOF means exact-bound success; any extra byte means failure. Thus a zero limit
accepts only an empty file. Loop interruption handling preserves progress already
made; no helper grows a buffer. Native transfers use checked chunks bounded by
both remaining capacity and the target call's maximum signed count.

Publish the new length only after EOF and successful close. Before publication,
any open/read/close/limit failure returns false. Existing length, capacity,
allocation identity and initialized prefix remain byte-identical; uninitialized
spare bytes may have been written and are never readable through source length.
Do not claim a snapshot of a concurrently changing file. Observed EOF defines
this read's boundary. Files may block; no file deadline or real-time guarantee
is introduced. Preserve descriptor-close obligations on every opened path.

### TCP transaction

Retain numeric IPv4/IPv6 only, address length 1..45 excluding NUL, port 1..65535,
nonnegative response limit and positive timeout. Additionally reject insufficient
spare output capacity before socket creation or transmission. Preserve the
connect/send/write-shutdown/receive/close protocol and its deadline. No resolver,
TLS, socket handle or intermediate growable response is added.

Receive directly into spare output bytes, with the same exact-bound one-byte
EOF probe as file input. Publish length only on complete protocol and successful
close. On invalid input, timeout, extra response byte, transport/close failure
or unsupported target, return false with output metadata and initialized prefix
unchanged. Network side effects already performed are not rolled back. Preserve
SIGPIPE containment and every existing timeout, partial response and closure
witness; do not mistake unchanged output for an unchanged peer.

Deadline construction and time conversion use saturating checked arithmetic,
including the final millisecond fraction when seconds are at the I64 boundary.
An unchanged or failed clock observation is not a proof that an external
operation completed within its wall-clock timeout.

### Output and abnormal termination

Use a target-tier complete-write loop with fixed scratch; handle short writes
and interruptions. Convert I64 using unsigned magnitude so INT64_MIN never
overflows; at most 20 bytes include the sign. Println writes its input then one
newline in source order, without an allocation or a promised cross-thread atomic
line. Existing failure diagnostics and trap exit 70 remain required.

Trap reporting must use the same audited unbuffered boundary followed by process
termination. It must not walk/free live pools while another worker can use them.
It promises neither normal cleanup nor successful output on a broken stderr.
Normal return still joins every successful worker and releases initialized
owners. This does not introduce user OS resources or destructor callbacks.

### Target boundary and resource claims

POSIX hosted builds use their existing target-tier native I/O family. Portable
tiers lacking the required descriptor operations return false for file/network
operations and retain an explicitly identified output/clock provider; do not
fall back silently to allocating stdio while claiming no allocation. Before
acceptance, each supported compiler target must identify an audited output
provider or retain its conservative capability ceiling. Do not drop a supported
installation target to get a passing result.

The no-allocation claim concerns SLIM-owned/provider and user-space scratch
storage in the audited adapter. File/socket calls can acquire kernel resources;
their explicit I/O boundary includes that work. Account for it separately, not
as a zero-cost or no-resource operation. Dynamic loader/libc provider assumptions
must be named in the native receipt. Merely observing no malloc call in one run
does not prove every external implementation allocation-free.

## Compiler and runtime design

The sole selfhost checker owns the changed signature, domain, loan and effect
facts. Code generation directly lowers the existing operations with their
checked arguments. No Rust host implementation or semantic fallback accepts
source. Retained queries depend on all changed signatures, layouts, effects,
entry requirements and consumed runtime/target files. Increment incompatible
surface/interface/runtime identities under RFC-0148 at their actual cutover.

Host authority and argument startup must be specified in the production
bootstrap bridge as well as ordinary executables. The compiler's native session
host has its own input ownership: argv views cannot replace copied/framed session
input, and a reset cannot invalidate last-good artifacts still owned by the host.

## Compatibility and migration

Migrate old main argument vectors to borrowed argument slices, old Bytes host
inputs to explicit checked views, and growing file/TCP calls to ordinary source
reservation wrappers. Remove the previous read-file signature and growing host
semantics at the executable storage cutover. No alternate spelling or mode keeps
both contracts. Before the later generic cutover, use one concrete byte-result
family in the library, for example ok/host_failure/exhausted; these are ordinary
source variants with exhaustive callers.

The native low-level Bool does not distinguish transport subcauses. Allocation
exhaustion is handled by the preceding reserve and never disguised as a transport
success. A failed wrapper reservation leaves the original output unchanged. A
successful reserve may increase capacity even if the later host operation fails;
the wrapper must document that difference from the host operation's own atomic
contract. Do not claim failure-atomic full wrapper capacity unless the wrapper
uses a separate owned result buffer and commits it explicitly.

## Diagnostics and failure cases

Allocate stable codes/spans for invalid entry shape, escaping argument reference,
missing allocator authority, host-input/output loan overlap, wrong element type
and missing capability at implementation. Invalid dynamic host bounds and native
failures are Bool data, not compiler diagnostics. Missing analysis facts remain
unknown and cannot discharge trap/block or enable worker execution.

## Performance and complexity

Argument setup is linear in argument count plus inspected bytes, one descriptor
allocation, zero argument-payload copies. Reads perform linear bounded transfer
work and one overflow-byte probe. Output uses constant scratch; completion can
depend on the host. Record calls, bytes, scratch, provider attempts, startup/RSS,
code size and elapsed time separately. Removing intermediate response copying
is an intended cost change requiring before/after measurements, not an assumed
speedup. Preserve the 1.03 inactive network binary ratio and every application,
compiler, allocation-failure and parallel budget.

## Alternatives and drawbacks

Keeping growing native adapters hides resource admission and doubles response
storage. Returning a new owner directly from a compiler-recognized enum adds
unnecessary result semantics. A preallocated output retains bounded storage and
ordinary source composition, but worst-case reservation can fail when the actual
response would fit. The fixed path bound is a compatibility cost. Unbuffered
small writes may be slower; measurements must retain that cost. The source
library may explicitly batch output using its own reserved storage.

## Test and acceptance plan

Freeze the [host inventory and boundary matrix](../../benchmarks/results/2026-09-08-m2-host-contract.md)
before native implementation. Test every stated boundary, old output prefix,
EOF/extra byte, interruption/short transfer, opened-descriptor closure, startup
and wrapper allocation failures. Validate exact current compiler argv behavior,
empty arguments, descriptor-size arithmetic and no authority/no-pool programs.
Use independent expected byte streams and native syscall-fault fixtures; a
simulated provider alone is insufficient. Verify ASan/UBSan and real provider
allocation observations separately. Preserve the old negative fixtures even
when explicit reservation changes their allocation ordinals; explain each map.

Require ordinary and forced-serial dual_fetch/dual_health source migration with
the worker child, unchanged ratio gates, source compiler/library/application
tests, retained-query recovery and every AGENTS.md command. Complete M2 also
requires both substantial applications and the full clean release/installation
gate. Documentation validation does not satisfy this acceptance plan.

## Ratings and evidence

All ratings are neutral under the scoped owned-byte/effect experiment. Current
native scratch and startup behavior are inspected facts. Successor behavior and
costs remain unknown until implemented and measured. This RFC grants no new
performance exception or general concurrency scope.

## Decision

Proposed. Accept with the coupled memory and target-provider contracts only after
the frozen boundary oracle, bootstrap argument plan and worker migration agree.

## Implementation

Pending. Current source, runtime ABI, seed and installed host behavior are unchanged.

## Removal and supersession

At migration, replace the corresponding entry, file/output and allocating TCP
contracts of RFC-0009/0024/0076 with this bounded storage contract. Preserve their
failure and performance gates. Reject or revise the experiment if source lifetime
safety, complete normal closure, target support or unchanged budgets fail.
