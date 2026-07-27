# RFC-0031: Shared typed compiler view

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 2
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 70

## Need

SLIM's checker previously validated only shallow scalar results while later
passes repeatedly rediscovered expression structure from token positions. That
left nested calls, records, variants, assignments, and recursive transfers
without one authoritative type result. It also made a disagreement between
checking, analysis, and code generation possible.

Core 1D introduces one compiler-owned typed view derived from canonical source.
It links declarations and lexical bindings, validates every declared type and
expression bidirectionally, and records stable expression-to-type facts plus
structured issues. The view is derived evidence rather than a second source or
executable language. Checking is its first consumer; code generation, memory
planning, analysis, and incremental queries must migrate to the same facts
before Core 1D is complete.

## Alternatives

Adding isolated checks to each existing pass would duplicate name and type
semantics. A general inference engine, constraint solver, traits, overloads, or
implicit conversions would enlarge the language and make compiler work less
predictable. A serialized trusted IR would add a second input boundary before
the in-memory model has proved sufficient.

SLIM instead keeps explicit parameter, binding, field, payload, and result
types. Expected types flow down and inferred types flow up through one bounded
tree walk. Stable token indices identify facts within the compilation; they are
not persisted as authority.

## Costs

The compiler retains type facts and lexical binding state proportional to the
checked input. Composite type comparison and member resolution must be indexed
or structurally bounded so deeply nested programs remain approximately linear.
Consumers need staged migration, during which differential tests must prevent
the old and new paths from disagreeing.

Project flattening originally lost module coordinates; RFC-0033 now retains one
origin per flattened token, while RFC-0034 gives each issue an inclusive token
interval that projects point and range diagnostics back to exact module-local
spans. RFC-0035 retains the finalized issue stream on the checked artifact so
later semantic passes can extend the analyzer snapshot without rebuilding the
view. The first self-host measurement exposed repeated whole-program call resolution
in legacy effect and ownership passes. Reusing declaration links removed those
scans and improved the recorded self-host project check from 3.91 seconds to a
0.20-second median. Member queries and project-artifact reuse remain acceptance
work.

## Evidence

The first slice is a separate SLIM `typing` module used by standalone and
flattened-project checking. It validates nested expressions, function and
builtin calls, declared types, records, variants, matches, `set`, `recur`, and
the exact executable signature. Twenty-three external negative fixtures pin
stable diagnostic codes and byte spans. The complete standalone corpus, twelve
matched challenge programs, 2,000 malformed mutations, and the self-hosting
compiler pass the stronger checker. The portable C seed reproduces at a
byte-identical fixed point.

Standalone and project compilation now retain one checked artifact through
ordinary C emission and read and lex each input once. Project cache misses emit
from the same checked artifact. Geometric user-call-heavy and
deeply nested binding workloads have permanent 1.25 exponent budgets; the first
full results are 0.916 and 0.980. The dated progress report preserves inputs,
host context, commands, and limitations.

Core 1D acceptance additionally requires dense or otherwise bounded fact lookup
for downstream consumers, geometric scaling evidence, and removal of redundant
semantic rediscovery from ordinary code generation. Remaining effect and
ownership diagnostics must join the structured source-projection channel.

## Removal

Replace the view only with a smaller single semantic artifact that preserves
the same type, effect, ownership, diagnostic, incremental, scaling, and
self-hosting evidence. Individual compatibility walks are removed as consumers
migrate; they must not survive as an alternative type system.
