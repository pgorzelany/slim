# Incremental compilation status

`slimc session` exposes retained production parsing, checking, analysis, C emission
and native artifacts. Ordinary `check`, `emit-c` and `build` remain one-shot
operations. M1 release closure is pending; see the
[implementation report](../benchmarks/results/2026-09-05-slim-next-progress.md)
for current verification, costs and limits.

## Public framed session

Start `./slimc session` without arguments. Bootstrap builds the adapter from the
verified portable seed and paired runtime. Each frame contains a tag, a four-byte
big-endian payload length and exactly that payload. Accept only complete responses.
[RFC-0143](../design/rfcs/0143-host-bound-compiler-sessions.md) specifies frontend
layouts, admission limits and errors;
[RFC-0146](../design/rfcs/0146-native-builds-in-compiler-sessions.md) specifies native
requests and results.

| Frame | Operation |
| --- | --- |
| `H` | Identifies the loaded compiler, runtime, target and options. Clients cannot supply identities or cached facts. |
| `U` / `S` | Updates a manifest path; returns actual work and deterministic C on success, or diagnostics and the last-good revision without C on rejection. |
| `B` / `N` | Builds the exact selected successful epoch/revision; returns complete executable bytes or an explicit failure. |
| `R` | Physically releases frontend/native query storage and starts a cold epoch; captured native tools remain. |
| `Q` | Closes the connection. EOF between frames also closes. |

Only a fully checked candidate replaces the successful snapshot. A rejected update
preserves explicitly selectable last-good artifacts. Exhausted identities never
wrap. Clients publish executable responses through a fresh staging inode and atomic
rename. Source capture, response transmission and output assembly remain work.

## Retained compiler substrate

[RFC-0124](../design/rfcs/0124-retained-compiler-substrate.md) defines the substrate
and links its child contracts. Canonical SLIM and the normal checker remain the
sole semantic authority; cache damage or missing evidence cannot approve source.

- Typed revision/file/declaration/node/span identities validate ownership and exact
  content before translating source positions. Complete declaration keys preserve
  relocation and ordering changes without resurrecting deleted handles.
- Retained declaration parsing validates lexical/lookahead dependencies. Function
  checking reuses successful facts through checked maps and complete interface
  dependencies; misses invoke the same isolated checker. Shared canonical control
  descriptions support explicit checking continuations and an optional bounded
  flow view. The separate name-resolution prepass retains its documented sanitizer
  depth limit.
- Memory plans, range facts, ordered caller-input contributions and complete bounded
  parallel results validate their consumed dependencies. Callers invalidate when
  body-derived recurrence, work or selection facts change. Missing or bounded-away
  history runs the ordinary producer.
- Declaration-local C identities permit retained prototypes, bodies and parallel
  wrappers. Imports validate owners, framing, checksums and complete checked,
  memory, range and selected-site inputs. Checksums detect accidental corruption;
  they do not authenticate arbitrary imported C.

Manifest/import/export validation, flattening, layout/global checks, termination
validation and complete C assembly remain current-revision work. Optional history
capacity misses preserve ordinary checking. Admission limits do not bound peak RSS.
The former Rust incremental API supplies no production semantics.

## Native artifact query foundation

[RFC-0145](../design/rfcs/0145-retained-native-artifact-queries.md) defines complete
native keys and owned storage. RFC-0146 connects these queries to checked-source
selection. The reference provider captures Apple clang 21 on Darwin/arm64, its
required dependencies, SDK inputs and paired runtime into connection-owned storage.
Unsupported contexts explicitly decline native session builds. Restart to refresh
tools; reset only clears query history. Body edits still compile the whole generated
C translation unit while eligible runtime objects remain reusable.

## Observed execution

`sh scripts/verify-session-host.sh` compares full clean/retained results and observes
actual work under ordinary and sanitizer builds. Its corpus, edit/recovery matrix,
identity, corruption, resource and fault checks are complemented by native
application and selection checks. `scripts/measure-session-host.py` measures frontend
latency; `scripts/measure-native-session.py` measures public native workflows.

No-change updates can execute zero producer/import entries while still reading,
validating and copying data. Instrumented work, ordinary latency, external backend
starts, memory and artifact bytes remain independent evidence. See
[the measurement contract](PERFORMANCE.md#observed-compiler-work) and dated reports.
Faster builds do not establish an agent-task success rate.

## Historical invalidation-estimate driver

`tests/fixtures/session_estimate.slim` retains the old snapshot-comparison model.
Its `parsed lowered checked generated` fields estimate classified changes and
invalidated declarations; they count no executed compiler operations. An unchanged
comparison still parses both input projects. Recovery performs clean checks rather
than retaining checked state. The `slim-bench incremental` and `slim-bench work`
model gates remain regression infrastructure, separately identified from public
sessions. Historical operation-labelled columns must not be cited as production
query reuse. Full observation boundaries and formulas remain in
[RFC-0121](../design/rfcs/0121-observed-compiler-work.md) and the performance contract.
