Implement reusable `std_hex.encode(source: Bytes, start: I64, end: I64,
maximum: I64, output: @Vec[U8]) -> Bool effects[alloc, partial]` and same-signature
`decode`. maximum is the additional OUTPUT bytes, admitted 0..1048576; admitted
source length <=1048576. Validate bounds and resource budget before mutation.
encode uses exactly two lowercase hex digits per source byte. decode requires
an even whole slice of ASCII hexadecimal, accepts either letter case, and emits
one byte per pair. No separators/whitespace/signs are allowed. Empty slice succeeds
at zero budget. Reject odd/malformed/invalid/over-budget inputs preserving the
complete output prefix. Do not consume output ownership or add implicit resources.
Compose actual std_ascii classification/hex conversion rather than changing it.
Initial code loses an exclusive mutation marker and copies input instead of encoding.
Context anchors: `std_ascii.is_hex_digit`, `std_ascii.hex_value`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
