Restore the actual formatter's existing canonical argument modes and layout.
Emit exactly @ for exclusive argument mode1, ^ for owned mode2, and no marker
for ordinary arguments. Keep inline argument order and comma-space separators,
including nested calls, zero/one/many args and recur. Canonicalize accepted
multiline ordinary argument lists to the established inline form. Formatting
must parse again and be idempotent. No new spelling or ownership semantics is
requested. Preserve all other declarations/type/effect/module formatting.
Initial renderer swaps mode markers and loses inline separators.
Context anchors: `format.emit_module`, `format.emit_type`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
