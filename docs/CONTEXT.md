# Bounded semantic context

[RFC-0159](../design/rfcs/0159-bounded-semantic-context.md) specifies
`slimc context SOURCE EXPECTED_SOURCE MODULE.DECLARATION`; this page fixes schema 1.
Current capture must equal independently retained expected bytes: the complete
module, or manifest and every ordered project module, including unrelated modules.
Capture is sequential, not atomic. Normal production checking rejects invalid
source without context; last-good reports concern only that snapshot. Clients
retain expected bytes and compiler identity, and recheck bytes before repairs.
Paths and checksums are not authority; canonical ordinals are revision/compiler
specific.

Strings use `json-byte-escapes-v1`: non-ASCII bytes become `\u00XX`; controls,
quotes and backslashes are JSON-escaped. Encode decoded strings as Latin-1 to
recover original bytes, including UTF-8. Names preserve source spellings.

## Schema 1

Fields are mandatory; `null` never implies a semantic negative. Arrays are lexical
unless specified otherwise. Extensions require an accepted versioned contract;
reject unknown schemas before interpreting facts.

| Root field | Type and meaning |
|---|---|
| `schema` | Integer `1` |
| `encoding` | String `json-byte-escapes-v1` |
| `identity_evidence` | String `exact-expected-input-bytes` |
| `selector` | Original qualified declaration string |
| `input`, `limits`, `work` | Objects below; limit/work values are integers |
| `selected` | Declaration object with exact complete `source` |
| `providers` | Section of distinct direct source-provider declarations |
| `facts` | Section of selected checked binding/expression facts |
| `references` | Section of selected semantic call/type/construction occurrences |

`input` contains `kind` (`module` or `project`), `manifest` (module: `null`;
project: `{bytes: integer, weighted_checksum: integer}`), and `files`, an array of
`{slot: integer, module: string, path: string, bytes: integer, weighted_checksum: integer}`.
Slots are zero-based project order, zero for standalone; paths describe current
capture. Identity requires the client's complete expected bytes.

`limits` contains:

```json
{"input_bytes":16777216,"modules":128,"canonical_nodes":1000000,
 "selected_source_bytes":65536,"providers":64,"facts":512,"references":512,
 "type_bytes":4096,"type_nodes":128,"signature_bytes":8192,"report_bytes":1048576,
 "operand_bytes":4096}
```

A span is `{file: integer, start: integer, end: integer}`: zero-based half-open
captured original-file byte offsets. Checked synthetic mappings can be zero-width
or cover an operation; neither makes a synthetic callee independently replaceable.
Null means unsupported mapping, never offset zero.

A declaration object contains:

| Field | Type |
|---|---|
| `evidence` | String `exact` |
| `provenance` | String `checked-declaration` |
| `node` | Integer canonical declaration ordinal |
| `module`, `name` | Original strings |
| `kind` | `function`, `struct` or `enum` |
| `name_span`, `span`, `signature_span` | Span objects |
| `source` | Exact complete selected source string; providers: `null` |
| `signature` | Exact original string; over cap: `null` |
| `signature_evidence` | `exact` or `unknown` |
| `signature_reason` | Empty string, or `signature-budget` |
| `parameters` | Parameter array; empty for non-functions |
| `return_type` | Type object for functions; `null` otherwise |
| `effect_ceiling` | Canonical-order effect-name strings; empty for non-functions |
| `effect_evidence` | String `declared-ceiling` |
| `fields` | Field array for structs; empty otherwise |
| `cases` | Case array for enums; empty otherwise |

Function signatures end at the header's final colon, excluding trailing whitespace;
struct/enum signatures cover their complete declaration. Unknown signature text
does not invalidate exact spans/declarations. Effect ceilings prove capabilities,
not events.

Parameters are `{node, name, name_span, type, declared_mode, mutable, borrow_mode, evidence}`:
integer nodes, string names, Boolean mutable, evidence `exact`; spans/types follow
this schema. Declared modes:
`ordinary`, `exclusive`, `owned`; checked modes: `value`, `shared`, `exclusive`.
Fields are `{node, name, name_span, type, evidence}`; cases are
`{node, name, name_span, payload: [Type], evidence}`, with the same field types.
Modes prove neither arbitrary-point loan state nor move permission.

Types are `{evidence, kind, text, span, reason}`. Kind: `scalar`, `source-form`,
`unknown`; text: string/null; span: span/null. Existing checked scalars/original
type forms are `exact`, otherwise `unknown`. Reasons: empty, `missing-checked-fact`,
`type-budget`, `unsupported-source-span`. Over-budget text can retain a validated
span; text never uses flattened names. Inferred scalars remain exact with null
type spans; expression/literal spans belong to fact rows. Declared scalar
parameter/field/return types retain actual type-form spans.

Sections are `{evidence, limit, reason, rows}`: `exact` with empty reason iff every
eligible row was traversed/emitted; otherwise `bounded`, reason `row-limit`, fixed
limit. Prefix rows retain independent exact/unknown evidence; omitted totals are
not invented. Providers follow first-reference order, deduplicate checked target
ordinals and omit selected itself; provider bodies are not traversed.

Facts are `{node, span, name, provenance, type, borrow_mode, evidence}`. Bindings
have string names/checked modes and provenance `checked-binding-fact`; expressions
have null names/modes and `checked-expression-type`. Existing checked types make
facts `exact` despite independently unknown type text. No facts are invented.

References are `{node, span, role, provenance, evidence, reason, target_kind, target_node, target_module, target_name}`.
Only semantic AST roles qualify: `call`/`recur` use `checked-call-link`;
`type`/`construction` use `checked-type-link`. Target kinds: `source` (integer
ordinal, original module/name strings), `builtin` (null ordinal/module, checked
canonical name, e.g. `i64.lt` for `<`), `unknown` (null target fields,
`unsupported-reference`). Exact references have empty reasons. Duplicates remain
separate; the list proves neither a transitive graph nor observed effects.

`work` contains integers `inspected_nodes`, `fact_rows`, `reference_rows`,
`provider_rows`, `source_bytes_copied`, `report_bytes`. Inspected nodes count only
the selected lexical scan, excluding checking, selection and repeated
descendant/provider/type work; they do not establish total linear work.
Descendant span scans can scale quadratically. Copied bytes count originals
encoded into strings; report bytes equal output length, including JSON framing
and final newline.

## Diagnostics and limits

| Identity | Meaning | Exit status |
|---|---|---:|
| `E0450@0:0` | Stale expected complete input | 1 |
| `E0451@0:0` | Context input/node/source/report limit exceeded | 1 |
| `E0452@0:0` | Selected original qualified declaration absent | 1 |
| `E0453@0:0` | Invalid context arguments or input-kind mismatch | 64 |

Read/manifest/check failures retain existing diagnostics/statuses. No partial
context is published; row caps produce complete reports with bounded sections.
Appends check capacity before growth. Input limits apply on each side after full
capture, not before-read allocation or peak-RSS limits. Context is cold;
retained-query latency, total context work/memory/allocations and model repair
benefit remain separately unproved. Detailed capture, provenance and cost rules
remain in RFC-0159; no language or performance budget changes.
