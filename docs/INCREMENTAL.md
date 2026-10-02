# Incremental compilation status

`slimc session` retains production parsing/checking/analysis/C emission/native
artifacts; `check`/`emit-c`/`build` are one-shot. M1 release verification is complete;
[implementation report](../benchmarks/results/2026-09-05-slim-next-progress.md):
verification/costs/limits.

## Public framed session

Argument-free `./slimc session` bootstraps the adapter from verified portable
seed/paired runtime. Frame = tag + four-byte big-endian payload length + exact
payload; accept complete responses only. [RFC-0143](../design/rfcs/0143-host-bound-compiler-sessions.md):
frontend layouts/admission limits/errors; [RFC-0146](../design/rfcs/0146-native-builds-in-compiler-sessions.md):
native requests/results.

| Frame | Operation |
| --- | --- |
| `H` | Identifies loaded compiler/runtime/target/options; no client identities/cached facts. |
| `U` / `S` | Manifest-path update: success → actual work/deterministic C; rejection → diagnostics/last-good revision without C. |
| `B` / `N` | Exact selected successful epoch/revision: complete executable bytes or explicit failure. |
| `R` | Physically releases frontend/native query storage; cold epoch, captured native tools retained. |
| `Q` | Connection closes; inter-frame EOF also closes. |

Fully checked replacement only; rejection preserves selectable last-good artifacts.
Identities never wrap. Publish via fresh staging inode/atomic rename. Source
capture/response transmission/output assembly remain work.

## Retained compiler substrate

[RFC-0124](../design/rfcs/0124-retained-compiler-substrate.md): substrate/child
contracts. Source approval requires canonical SLIM/normal checking, never damaged
caches/missing evidence.

- Typed revision/file/declaration/node/span identities validate owners/exact content
  before position translation. Complete declaration keys preserve relocation/
  reordering, never resurrect deleted handles.
- Declaration parsing validates lexical/lookahead dependencies. Checked maps/complete interface
  dependencies permit successful function-fact reuse; misses use the same isolated
  checker. Shared canonical control descriptions support explicit checking continuations/
  optional bounded flow; separate name-resolution prepass retains documented sanitizer
  depth limits.
- Memory plans/range facts/ordered caller-input contributions/complete bounded parallel
  results validate dependencies. Changed body-derived recurrence/work/selection facts
  invalidates callers; missing/bounded-away history uses ordinary producers.
- Declaration-local C identities retain prototypes/bodies/parallel wrappers. Imports
  validate owners/framing/checksums/complete checked/memory/range/selected-site
  inputs. Checksums detect accidental corruption, cannot authenticate arbitrary C.

Current-revision work: manifest/import/export validation, flattening, layout/global
checks, termination validation, complete C assembly. History-capacity misses check
ordinarily; admission limits cannot bound peak RSS. Former Rust incremental APIs
supply no production semantics.

## Native artifact query foundation

[RFC-0145](../design/rfcs/0145-retained-native-artifact-queries.md): complete native
keys/owned storage; RFC-0146: checked-source selection. Reference connection-owned
captures: Darwin/arm64 Apple clang 21/required dependencies/SDK inputs/paired runtime.
Unsupported contexts explicitly decline native session builds. Restart refreshes
tools; reset clears only history. Body edits compile whole generated C units;
eligible runtime objects remain reusable.

## Observed execution

`sh scripts/verify-session-host.sh`: complete clean/retained comparisons, ordinary/
sanitized actual work, corpus/edits/recovery/identities/corruption/resources/faults/
native applications/selections. Measurements: `scripts/measure-session-host.py`
(frontend latency), `scripts/measure-native-session.py` (public native workflows).

Zero-producer/import no-change updates still read/validate/copy. Independent evidence:
instrumented work/ordinary latency/external backend starts/memory/artifact bytes;
[measurement contract](PERFORMANCE.md#observed-compiler-work), dated reports.
Speed proves no agent-task success rate.

## Historical invalidation-estimate driver

`tests/fixtures/session_estimate.slim`: snapshot comparison;
`parsed lowered checked generated` estimate classified changes/invalidated
declarations, never executed operations. Unchanged: both projects parsed; recovery:
clean checks, no retained checked state. `slim-bench incremental`/`slim-bench work` model gates
remain separate regression infrastructure from public sessions. Historical operation-labelled columns
cannot establish production reuse; [RFC-0121](../design/rfcs/0121-observed-compiler-work.md)/performance
contract preserve observation boundaries/formulas.
