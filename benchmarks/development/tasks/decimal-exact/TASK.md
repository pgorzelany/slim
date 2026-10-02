Extend actual `std_decimal` with `parse_i64_exact(source: Bytes, start: I64,
end: I64) -> Parsed effects[partial]`. Preserve existing permissive prefix APIs.
The complete specified slice must be canonical signed decimal I64: `0`, positive
nonzero digits without a sign, or `-` followed by a nonzero digit and digits.
Reject +, whitespace, leading zeros, and -0. Invalid bounds/empty report start;
lone - reports end; first bad/noncanonical byte reports its absolute source offset
(second digit for a leading zero, zero digit for -0). Trailing nondigit reports
its offset. Overflow reports the first digit whose addition exceeds I64 range.
After a first malformed/canonicality failure, later overflow is irrelevant.
Value carries the complete slice end, including I64 minimum. Initial wrapper
incorrectly delegates to prefix parsing. Context anchor: `std_decimal.parse_i64`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
