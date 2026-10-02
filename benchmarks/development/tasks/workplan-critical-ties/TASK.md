Repair actual workplan scheduling so results depend on task/dependency bytes,
not input order. At each step choose the lexicographically smallest ready task.
A task starts at max dependency finish. Equal maxima choose lexicographically
smallest dependency as parent. Critical task is largest finish, ties smallest
task name, and emitted critical path follows these checked parents. Costs are
nonnegative I64. Before start+cost overflow report code22 at task.name_start;
cycles report code21 at the lexicographically first remaining task name.
Empty graph has critical=-1/span0. Preserve input ordinal identity in steps,
all loader validation/limits and original report format. Initial code confuses
input order and lexical rank and chooses reverse tie winners. Context anchors:
`workplan_schedule.ready`, `workplan_schedule.release`, `workplan_schedule.run`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
