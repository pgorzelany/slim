Repair actual effect capability classification, preserving current checked
ceilings. builtin_requires/call_requires must identify alloc1, io2, partial3 from
the existing built-in contracts and linked function declarations. read_file and
tcp_exchange require both alloc/io; monotonic_ms requires io; vec/arena creation
and insertion require alloc. Existing built-in partial classification remains
false here: checked traps are handled by the original checker, not a new path.
Linked callees inherit exactly the declared ceiling, including unused declared
capabilities; unknown/unlinked names return false, not a guessed effect. list_has
must distinguish all three names. Complete body/call-graph analysis elsewhere
must remain intact; this utility does not prove that an event happened. Preserve
ordinary checker missing-capability diagnostics and accepted ceilings. Initial
code omits TCP/clock allocation/I/O and mistakes partial for alloc in list lookup.
Context anchors: `effects.builtin_requires`, `effects.call_requires`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
