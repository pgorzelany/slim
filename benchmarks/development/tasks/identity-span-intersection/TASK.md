Add `identity.Overlap` cases `Invalid`, `Empty`, `Ready(Span)` and pure
`intersect_span(file: FileId, source: Bytes, left: Span, right: Span) -> Overlap`.
Both input spans must resolve in exactly the supplied valid file revision/slot.
Invalid provenance or 0<=start<=end<=source length failure yields Invalid before
intersection. Otherwise intersect half-open windows: max starts/min ends; any
empty or touching result yields Empty, including zero-width input. Nonempty
Ready retains the supplied FileId. Never manufacture a span from stale source
identity. This must stay pure and cannot perform arithmetic on invalid bounds.
Existing resolve_node/view/revision APIs must retain their contracts. Initial
implementation ignores provenance and accepts invalid spans. Context anchor:
`identity.resolve_span`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
