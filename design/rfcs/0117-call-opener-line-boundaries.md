# RFC-0117: Call opener line boundaries

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next roadmap
Created: 2026-09-06
DecisionDate: 2026-09-06
Approver: project-maintainer
Kind: compatibility
Primitive: none
Safety: 1
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 25

## Summary

Keep a call's opening parenthesis on the callee's line. Restore formatter
round trips when a local initializer ends in a name and the next statement
starts with a grouped expression. This repairs the existing indented canonical
source; it introduces no alternate expression form.

## Motivation

The production compiler at e884563 accepts the durable
`benchmarks/reproducers/formatter_leading_group.slim` witness. Its formatter
emits a leading group after a local initializer ending in a name. The parser
recognizes that parenthesis across the newline as a call on the initializer's
name and rejects its own output with E0102. The same defect exists at d6eeede.

## Guide-level explanation

Write a function, builtin, struct constructor, enum constructor, or recurrence
with its opening parenthesis on the callee's line. Arguments may span lines
inside those parentheses. A grouped expression on a following statement line
does not call a name in the preceding local initializer or assignment.
An enum constructor's `Type::Case(` prefix stays on one line.

## Reference-level specification

The familiar expression parser recognizes an ordinary call or struct
construction only when the next lexeme is `(` on the name lexeme's line.
Apply the identical opener condition to `recur`. For an enum construction,
require `::`, its case name, and `(` on the type-name lexeme's line. Whitespace
within the line remains governed by the existing lexer. Comments and blank
lines do not join two lines into a call prefix.

Once `(` has been consumed, preserve the existing argument parser, comma
requirements, grouping, precedence, evaluation order, and ownership markers.
Do not change how declarations, parameter lists, or binary operators parse.
The existing checker still rejects a bare function or type name used as a
value; parser/checker diagnostics for split prefixes remain deterministic.

## Compiler and runtime design

Use the existing Lexeme line field and indexed `lexeme_on_line` helper in
selfhost/syntax.slim. Add constant work at callee recognition. Do not add a
parser, persistent representation, scan, dependency, or runtime operation.
Regenerate the portable seed and verify the production fixed point.

## Compatibility and migration

Previously accepted split call prefixes no longer denote a call. Put `(` on
the callee's line; multiline argument lists remain available. The formatter
already emits that form, so canonical programs need no migration. This is an
accepted pre-1.0 compatibility repair under RFC-0112, not a stability claim.

## Diagnostics and failure cases

Preserve E0102 for malformed constructor prefixes and normal checker diagnostics
for invalid bare callee values. Add exact rejection fixtures for ordinary,
struct, enum, and recurrence split prefixes, and positive multiline argument
fixtures. Preserve the reported formatter witness and check its formatted output.

## Performance and complexity

Callee recognition adds a fixed number of indexed line comparisons. Preserve
separator-dense frontend work counts and exponent budgets, ownership/declaration
scaling fixtures, and all native runtime ratios. Measure same-host frontend
latency before and after without claiming agent productivity from token proxies.
Extend the existing paired frontend command with an optional `--separator-lists`
workload using the durable call/argument-list generator. Keep its default
declaration workload, sample order, binary identity checks, and all gates.

## Alternatives and drawbacks

Removing formatter parentheses would lose precedence in other expressions.
Guessing whether a name denotes a value or function during parsing would
couple syntax to semantic lookup and leave statements ambiguous. Retaining
split prefixes at matching indentation keeps the reproduced ambiguity.
Restricting every multiline expression is broader than this repair.
The source compatibility cost is confined to noncanonical split call prefixes.

## Test and acceptance plan

Check/format/check/format and compare generated C across a bounded matrix of
scalar operators, nesting, borrowed collection reads, local bindings, and
assignments. The second format must equal the first. Run positive multiline
calls, constructions, and recurrence through the native production compiler.
Add exact diagnostics for rejected split prefixes. Check the compiler itself
with ASan/UBSan and exercise the witness with allocation-fault injection.
Run bootstrap, governance, Cargo, conformance/malformed-input, and all required
performance, reduction, parallelism, comparison, and agent checkpoint gates.

## Ratings and evidence

Safety +1 restores a failed representation round trip. Compile 0 and Runtime 0
make no speed claim. Minimal 0 adds no surface form. Analysis +2 restores a
reliable source boundary. Dogfood 0 makes no unmeasured productivity claim.
Weighted score: 25.

## Decision

Accepted under the maintainer's RFC-0112 implementation authorization and
request for validated checkpoint commits. This is delegated implementation
authority, not a separate maintainer review. No hard gate is relaxed.

## Implementation

Complete for the call-prefix boundary. All 26 round-trip matrix cases retain
byte-identical C and idempotent formatting. Six split-prefix negatives have
exact diagnostics; multiline calls and the original witness execute natively.
The compiler reaches a 2,954,387-byte C fixed point. All required checkpoint
gates, 256 conformance fixtures, and 2,000 malformed-input mutations pass.

The existing separator-dense exponent is 0.554 under its unchanged 1.25 ceiling.
Seven paired native measurements per size put the candidate/baseline median
ratios at 1.013, 1.005, 0.991, and 1.014 for 2,000 through 16,000 declarations.
The progress report records raw measurements, sanitizer/fault coverage,
unchanged native analysis reports, and remaining M0 work.

## Removal and supersession

Any successor parser must preserve canonical round trips and the durable
witness. Replace this line check only with a specified unambiguous statement
boundary, retaining diagnostic, multiline-argument, and scaling coverage.
