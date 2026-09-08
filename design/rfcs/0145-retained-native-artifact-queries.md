# RFC-0145: Retained native artifact queries

Status: accepted
Implementation: complete
Process: 1
Audience: developer
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

Implement native artifact retention as a production SLIM query library. Its
opaque artifacts come only from successful external C compilation and linking
of accepted generated C. Exact input bytes, artifact roles, worker configuration,
owner epoch and actual native context distinguish entries. Bounded framing and
integrity checks turn damaged optional history into misses.

This decision specifies the query engine, not an executable-cache import format
or a second compiler. Complete host context capture, external execution, public
session integration and atomic file publication remain required M1 work and need
an accepted companion host architecture contract before production integration.
Completing this library alone does not complete native caching or M1.

## Motivation

RFC-0144 closes retained bounded parallel analysis. The public session now
retains accepted deterministic C, but native build/run still compile generated C
and the runtime each time. A native artifact query must retain actual objects
and link results with complete inputs, rather than report estimated reuse.

Private experiments on the current Apple toolchain reproduce native output for
all twenty application fixtures in serial and POSIX worker configurations. They
also reproduce all three runtime header profiles, including their complete
preprocessed tokens and macro definitions. These observations motivate the
interface; the Python experiment is not production implementation evidence.

The same experiments expose costs: two ordinary native builds take approximately
136-138 ms, while hashing the 141 MB C compiler takes approximately 67 ms on a
warm read. Repeatedly rebuilding context identities may erase cache savings.
A complete, stable native context is therefore a separate implementation problem;
this query never invents one from a user-supplied version string.

## Guide-level explanation

A successful native compiler result can be retained and retrieved for the exact
same inputs. A generated-C object, runtime object and linked executable are
separate roles. Objects are actual external translation units: retaining the
runtime object does not imply per-function native compilation.

A hit returns the complete retained artifact. Changed context, epoch, role,
worker configuration or any input byte misses. Missing entries and capacity
exhaustion leave ordinary native compilation available. Corrupted optional
history disables further retention until reset; it never changes source
acceptance or presents an old executable as the new source result.

## Reference-level specification

### Sole authority and boundary

Implement the cache, matching, framing, integrity, admission and publication of
query records in `selfhost/nativecache.slim`. The normal checker remains the
sole authority for source acceptance. No Rust or Python production semantics,
new source language operation, separately parsed IR or native backend is added.
A host adapter may later drive exported query functions and supply opaque native
bytes. It may not construct or mutate retained entries or semantic source facts.

Only a successful native operation over accepted current C may publish an
artifact. A public build must independently validate its requested successful
source revision through the SLIM session. Native cache entries are bound to one
epoch and cannot authorize access to a stale source handle.

### Context and keys

A context is a nonempty bounded identity of at most 1,024 bytes, constructed by
the owning host from actual captured inputs. It must distinguish the loaded SLIM
compiler and paired runtime, actual external compiler/linker and required helper
images, target, ordered effective options, header/preprocessing state, complete
link inputs and declared host ABI context. Missing or unsupported context evidence
must remain unknown and cannot authorize a cache hit. A checksum or version
string supplied in a build request is not an accepted context.

The host contract must bind actual execution to the same inputs it identifies.
If a context is fixed for an epoch, changing live files must not silently change
what its backend executes. New contexts and reset must be explicit. Shared or
imported executable caches are outside this decision.

Each key contains the owner epoch, context, role, worker configuration and two
ordered byte strings. The roles are generated-C object (0), runtime object (1)
and linked executable (2). Worker configurations are no structured-worker code
(0), structured code with serial workers (1), and POSIX workers (2). These values
name existing native configurations; they introduce no new execution shape.

For role 0, the first input is complete accepted generated C and the second is
empty. For role 1, inputs are the complete paired runtime C and header. For role
2, inputs are the complete program and runtime object bytes in link order. The
actual context supplies all remaining dependencies. Both role-1/2 inputs are
nonempty. All roles have a nonempty first input and nonempty output artifact.
No input digest replaces exact byte equality in the final hit decision.

### Framing and integrity

The table holds at most 256 records and at most 67,108,864 admitted key/artifact
bytes per epoch, plus a context of at most 1,024 bytes and bounded record metadata.
Smaller limits may be supplied for tests; they may not exceed these hard maxima.
Charge cloned first input, second input and output bytes before admission.
Retention does not evict entries or reset its charged-byte count during an epoch.

Validate table limits, owner, context, entry counts, individual byte lengths,
role/configuration shapes and total admitted bytes before indexing or arithmetic
that depends on them. Keep arithmetic validity dependencies in explicit branches;
SLIM Boolean operators are eager. Every integrity computation must remain inside
I64 even for corrupted scalar metadata. The combined byte domain is below 64 MiB;
its weighted byte sum is below 5.75e17. Fold arbitrary scalar metadata through a
bounded modular word operation before combining it with byte checksums.

Protect the table's structural metadata and every entry's complete key/output.
An eligible hit must pass both exact key matching and complete entry integrity.
These records are private, process-owned explanatory data, not cryptographic
proof of hostile executable bytes. No external serialized cache is accepted.

A damaged table or matching entry returns a stable corruption reason and disables
further retention for that epoch. Existing charged storage remains charged until
reset, even when history becomes unusable. Do not repeatedly discard a corrupted
table and admit fresh storage into the same unreclaimed region.

### Ownership and transaction

The query library clones retained context, key inputs and output into its owning
SLIM allocation region. Borrowed host buffers need remain valid only during a
call. A successful return may not leave retained pointers into temporary host
buffers. Uncacheable results must not allocate retained artifact copies.

Publishing an already retained identical key/output is an idempotent success.
A different output for the same complete valid key is a conflicting result:
disable retention and report it rather than silently selecting one artifact.
No conflicted entry may subsequently produce a hit before reset.

Admission failure leaves existing valid records usable and ordinary production
compilation available. Allocation failure follows the existing terminal region
failure rule; the host must never reuse a failed region. No external executable
is published by this library. The companion host must preserve the last complete
output through backend, link, allocation, transport and file-publication errors.

## Compiler and runtime design

The cache is independent of frontend hot paths and is initialized only when
native work is requested. Keep native allocation/copy accounting separate from
frontend query work. Its outputs are opaque bytes; it does not inspect source
semantics or implement object/link semantics. Native host glue requires a separate
accepted architecture contract and must use the existing external C backend.
The portable seed and SLIM compiler remain the sole production compiler.

## Compatibility and migration

There is no source, runtime ABI, C emission, analysis or public protocol change
in this library decision. Preserve current `build`, `run`, session `U/R/Q`, all
native worker configurations and every safety/performance gate. Public native
integration remains required and cannot be claimed from a private test fixture.

## Diagnostics and failure cases

Distinguish invalid request/key, context or epoch mismatch, absent record,
invalid artifact, disabled/corrupt history, conflicting output and capacity.
These are native query outcomes, not new source diagnostics or quality scores.
Missing evidence remains unknown. Neither native failure nor a hit accepts
rejected source or upgrades an unsupported native configuration.

## Performance and complexity

Default frontend work and application runtime are unaffected when this library
is unused. A lookup scans bounded record metadata; only potentially matching
entries need complete key/output integrity and byte comparison. Its work is
linear in the checked bytes with a fixed 256-record bound. Publication clones
only after admission. Record actual lookup, entry validation, byte comparison,
copying, producer execution and external backend stages independently.

Measure native context capture, cold builds, unchanged builds, body edits,
repeated/reverted builds, memory, bytes copied and artifact sizes. Preserve
geometric frontend gates and matched native controls. A cache-hit count alone
is not evidence of latency improvement or agent productivity.

## Alternatives and drawbacks

Keeping query logic in the C host would duplicate compiler data ownership and
validation outside production SLIM. Trusting just generated-C digests omits
runtime, tool, header and link dependencies. A single undifferentiated artifact
cache cannot expose actual object versus link reuse. Unlimited caching hides
retained storage costs. These alternatives are rejected.

Complete input/output validation and cloning have real costs. This first query
retains complete translation units and may still recompile a whole generated-C
object after an edit. Finer native partitioning needs its own measured dependency
contract and is not implied by this implementation.

## Test and acceptance plan

Exercise every key field and role/configuration, exact hits and misses, context
and epoch transitions, byte differences and ordered link inputs, borrowed-buffer
ownership, duplicate publication, conflicting results, malformed/individually
damaged metadata and payloads, and every admission boundary. Check that failures
never return stale bytes, that corruption cannot reset charged storage, and that
existing hits survive ordinary capacity exhaustion.

Run ordinary and ASan/UBSan fixtures through the production compiler, bounded
allocation faults, actual backend-produced object/executable round trips and
geometric copy/validation measurements. Preserve native corpus behavior,
analysis/resource baselines and all required checkpoint commands. Full native
host integration and full M1 release closure remain separate required work.

## Ratings and evidence

Analysis +2 makes retained native artifacts a bounded, explicit query with
complete keys and ownership; score 15. Other dimensions remain zero until the
production integration and measurements justify stronger claims. The private
snapshot experiment is feasibility evidence only.

## Decision

Accepted under the maintainer's RFC-0112 child-decision delegation and active M1
implementation goal. This authorizes the production SLIM query library. It does
not authorize an incomplete context, executable-cache import, new language
primitive, weakened gate, different native optimization mode or general-purpose
host process API. The complete parent M1 objective remains unchanged.

## Implementation

Implemented in `selfhost/nativecache.slim` at seed
`e54b3c1ed37f3548ebad1d74ac6d6088a4b1f50fbda4906769193471a0e6550a`.
The [checkpoint report](../../benchmarks/results/2026-09-08-m1-native-query.md)
records ordinary/sanitized byte and corruption tests, cloned buffer ownership,
capacity boundaries, allocation faults, actual native artifact round trips,
geometric work/latency, source identities and required checks. The library alone
is complete; public native execution/publication integration and M1 remain
incomplete.

## Removal and supersession

Any replacement must preserve exact complete input matching, checked source
revision ownership, private artifact provenance, corruption misses, deterministic
conflict handling, bounded charged storage and independent observed native work.
