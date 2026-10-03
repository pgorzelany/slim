# SLIM 0.9 diagnostics

Schema: 1
Status: SLIM 0.9 — experimental, pre-1.0

Rejected programs produce deterministic diagnostics: stable `E` code, severity,
source identity and half-open primary byte span. Offsets address captured bytes,
independent of display width. Whole-project canonical admission spans lack an
original source; `repair.source` is unavailable.

Human output is for reading. For agents and tools:

```text
slimc --message-format=json check SOURCE
```

The command writes one JSON object per line to standard error and no partial
program to standard output. Schema 1 fields are:

```text
schema, code, severity, message, file, span, labels, notes, fixes
```

`schema` is `1`; `span` contains `start`, `end`, `line`, and `column`.
`labels`, `notes`, and `fixes` are arrays even when empty. Additive fields may
be ignored; existing types, byte spans and code meanings require a new schema
when changed. [RFC-0173](../design/rfcs/0173-repair-oriented-diagnostics.md)
defines repair reports with concrete rule explanations, captured context and
retained checker facts. The production checker supplies these fields. Excerpts
cannot certify a complete revision or authorize an edit without rechecking.

`repair.status` concerns primary source and detail admission; optional facts
retain their own evidence. Null related locations carry no location claim.
The type-depth budget includes the leaf. `json-byte-escapes-v1` escapes each
non-ASCII byte separately; decode escaped values as byte values when recovering
original source or path bytes.

Malformed user input must not panic the compiler. Multiple independent issues
are retained within documented analyzer bounds. Project diagnostic identities
retain module labels; `repair.source` identifies the captured source owning the
span. Treat an unknown schema as unsupported. Code-family explanations can
cover multiple conditions; retained producer facts identify narrower causes.

The compiler reserves these stable conditions:

- `E0102`: malformed structure/block or canonical-node capacity
- `E0103`: source tab outside a byte string
- `E0104`: odd indentation width
- `E0105`: skipped indentation level
- `E0106`: forbidden brace or semicolon
- `E0107`: unterminated byte string
- `E0108`: missing list separator or closing delimiter
- `E0109`: leading, doubled, or trailing comma
- `E0357`: discarded non-Void result
- `E0358`: Void used in a storable position
- `E0359`: assignment to an immutable binding
- `E0361`: integer literal outside the signed I64 range
- `E0450`: context expected complete input is stale
- `E0451`: context admission, selected-source, canonical-node or report limit
- `E0452`: context original qualified declaration selector is absent
- `E0453`: invalid context arguments or mismatched input kinds

Each points at the offending byte interval, or the zero-width location where
required structure is missing.

The context tooling conditions use the zero-width request span `0:0` and are
fixed by [semantic context schema 1](CONTEXT.md) and its production command tests.
They do not imply source rejection. Its read, manifest and source-check failures
retain the existing source diagnostic identities and mapped byte spans.

The exact code-to-condition mapping is executable: failure fixtures in
`conformance/manifest.tsv` and `conformance/projects/manifest.tsv` pin codes
and primary spans. Those manifests, rather than a copied prose catalog, are
the canonical diagnostic inventory.
