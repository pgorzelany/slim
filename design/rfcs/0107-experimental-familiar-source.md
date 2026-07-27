# RFC-0107: Experimental familiar canonical source

Status: accepted
Implementation: complete
Process: legacy
Kind: compatibility
Primitive: none
Safety: 1
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 60

## Need

SLIM has no external users, but its released-looking `1.0.0` label and several
source forms still communicate a stability and familiarity level the project
has not earned. Whitespace-separated parameter and argument lists, named
integer operations, explicit `make`, `get`, `case`, and `set` forms, and dummy
`Unit` bindings make otherwise conventional programs unnecessarily difficult
to scan.

The project returns to an explicitly experimental `0.9.0` version and replaces
the source surface atomically. The canonical syntax uses `struct`, `enum`,
parenthesized named-field construction, postfix field projection, qualified
enum cases, conventional fixed-type operators, required commas, `let`/`var`
binding modes, Boolean `if`/`else`, `parallel`, and non-storable `Void`
statements. Identifiers use ASCII letters, digits, and underscore; declaration
qualification uses `.` and enum cases retain `::`.

The semantic language is unchanged. Operators name the existing checked scalar
and Boolean operations, `if` is the existing exhaustive Boolean branch,
assignment is the existing unique rebinding operation, `parallel` is the
existing lexical two-call fork, and `Void` retains the existing internal unit
representation and runtime lowering.

## Alternatives

Keeping the current syntax retains the smallest parser delta but leaves the
largest human-reading costs in every application and in the self-hosted
compiler. Adding aliases would preserve source compatibility that nobody needs
while permanently duplicating surface roles.

Brace record literals are unambiguous but make values visually unlike calls.
Parenthesized named fields are equally linear to parse. A zero-field
application is classified during the existing declaration-kind lookup, and the
single declaration namespace rejects a conflicting function/type name.

Keeping `Unit` is type-theoretically precise but forces visible one-value
bindings for ordinary sequencing. A true uninhabited `Void` would not describe
successful completion. The selected surface therefore treats `Void` as a
non-storable successful result while retaining `SlimUnit` internally.

## Costs

The lexer gains fixed punctuation tokens and validates standard identifiers.
The parser consumes commas without retaining them in the canonical AST and
uses a linear precedence parser before emitting the existing preorder nodes.
Non-final source expressions produce explicit sequencing nodes and must check
as `Void`. Parser, formatter, diagnostics, source spans, fixtures, documentation,
interfaces, caches, and the complete self-hosted corpus change together.

The +1 compilation rating is conditional on the permanent separator-dense
frontend series: its lexeme and canonical-node counts grow linearly, commas add
no canonical nodes, and the measured parse/check series remains below the
accepted 1.25 geometric exponent budget.

`VERSION` becomes `0.9.0`. Interface and cache schemas become 2 because
qualified source identities change. Project manifest schema 1 and runtime ABI
1 remain unchanged. RFC-0082 and RFC-0083 remain historical evidence, but this
decision supersedes their active 1.0 release-status and major-version source
compatibility promises.

Pre-1.0 minor releases may make an accepted, documented breaking change.
Patch releases preserve accepted source and documented behavior within their
minor line. A future 1.0 requires a new explicit compatibility freeze.

## Evidence

Acceptance requires an atomic migration of every production, conformance,
example, benchmark, and documentation program; rejection of every removed
spelling; exact positive and negative coverage for commas, precedence,
construction, projection, cases, Boolean branching, mutability, and Void
sequencing; deterministic idempotent formatting; and a self-hosted compiler
fixed point from the portable C seed.

Every existing semantic, ownership, effect, range, reduction, parallelism,
resource, and native-behavior baseline must remain exact except for recorded
source spans, source bytes, names, and syntax work. A separator-dense fixture
records source bytes, lexemes, canonical nodes, and frontend time. Commas add
no canonical AST node. Geometric frontend scaling and every existing
same-host runtime budget remain mandatory without relaxation.

## Removal

Remove a punctuation form, keyword, statement form, or formatter branch if it
creates a second spelling, requires semantic name guessing, permits a non-Void
discard, or causes a reproducible performance regression outside the recorded
noise band. The transition reader and migration tooling must not survive the
cutover. Revert the complete RFC-0107 source change if the final compiler cannot
reject legacy syntax, preserve direct canonical AST construction, reach the
bootstrap fixed point, or pass the complete release gate.
