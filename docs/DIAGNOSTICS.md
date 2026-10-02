# SLIM 0.9 diagnostics

Schema: 1
Status: SLIM 0.9 — experimental, pre-1.0

Every rejected program produces one or more diagnostics in deterministic
source order. A diagnostic has a stable `E` code, severity, source identity,
and half-open primary byte span. Byte offsets refer to the exact input bytes;
they do not depend on display columns or Unicode width.

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
`labels`, `notes`, and `fixes` are arrays even when empty. Unknown additive
fields may be ignored. Removing a field, changing its type, changing byte-span
semantics, or reusing a code for a different condition requires a new schema.

Malformed user input must not panic the compiler. Multiple independent issues
are retained within documented analyzer bounds. Project diagnostics use the
manifest module identity as their source identity. A consumer must treat an
unknown schema as unsupported rather than inferring a layout.

The compiler reserves these stable conditions:

- `E0102`: malformed source structure or block
- `E0103`: tab indentation
- `E0104`: odd indentation width
- `E0105`: skipped indentation level
- `E0106`: forbidden brace or semicolon
- `E0107`: unterminated byte string
- `E0108`: missing comma, with an insertion fix where unambiguous
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
