Repair and implement `text.append_quoted_slice(source: Bytes, start: I64,
end: I64, maximum: I64, output: @Vec[U8]) -> Bool effects[alloc, partial]`.
It appends a quoted byte slice atomically with respect to validation rejection.
Print ASCII 32..126 literally except quote/backslash, which use `\"`/`\\`;
LF/CR/tab use `\n`/`\r`/`\t`; every other byte uses lowercase `\xNN`.
Opening and closing quotes count toward `maximum`, the additional encoded bytes.
Accept only 0<=start<=end<=source length, source length<=1048576, and
0<=maximum<=1048576. Reject over-budget/invalid inputs before any output mutation.
Empty input needs two bytes. Existing output length does not consume maximum.
Allocation failure remains the runtime's declared behavior; no hidden allocation.
Preserve existing text writers, including I64 minimum. The initial function also
has an incomplete effect ceiling. Context anchor: `text.append_span`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
