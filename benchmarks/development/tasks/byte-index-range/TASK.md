Add `std_byte_index.range(index: Index, low: Bytes, high: Bytes) -> Bounds
effects[partial]` for an unmodified builder-produced Index. Return sorted index
positions selecting low<=key<=high in strict unsigned-byte lexicographic order.
Endpoints are complete keys, not prefixes. If low>high, return empty Bounds(0,0).
For other empty selections return equal insertion positions. Empty and binary
keys, strict prefix pairs and duplicate ordinals are legitimate. Preserve Entry
ordinal values and builder validation/find/prefix contracts. Use bounded binary
search and the existing comparison, not a copied/sorted index or linear scan.
The initial implementation confuses endpoint prefixes. Context anchor:
`std_byte_index.prefix`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
