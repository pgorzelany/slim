Repair actual catalog selection/aggregation across input and sorted key order.
`select` returns sorted-index positions; each Entry.ordinal selects its original
Record. Exact mode admits the complete equal key only, not longer prefix keys;
absent exact matches return an empty interval at the lower insertion position.
Prefix mode keeps all complete byte keys starting with query, including empty
query. `accumulate` visits the selected sorted interval in order, counting only
successful checked I64 additions. On overflow return valid=false, offending
record.key_start, and the prior count/total without performing the bad addition.
Keep framed loading limits/validation and emitted selected records unchanged.
The initial code uses sorted positions as record ordinals and admits exact prefix
matches. Context anchors: `catalog_data.select`, `catalog_data.accumulate`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
