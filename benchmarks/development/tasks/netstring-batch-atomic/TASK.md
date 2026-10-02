Add `std_netstring.append_batch(source: Bytes, frames: Vec[Frame], maximum:
I64, output: @Vec[U8]) -> Bool effects[alloc, partial]`. Each Frame identifies a
payload range [start,end); `next` is irrelevant to appending. maximum is the sum
of payload bytes over this batch, in 0..1048576. At most4096 frames are admitted.
Validate all bounds and the cumulative payload budget before writing any byte.
Every valid payload is rendered using the existing canonical netstring append
rules in input frame order, including empty payloads. Empty batch succeeds with
no mutation at any admitted maximum. Any bad range/count/budget preserves the
complete existing output. Payload duplicates are allowed. Do not consume frames
or output ownership. Initial code has a missing effect ceiling and writes before
validation. Context anchor: `std_netstring.append`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
