# RFC-0173: Repair-oriented compiler diagnostics

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the maintainer's requested general diagnostic improvements
Created: 2026-10-03
DecisionDate: 2026-10-03
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 1
Compile: 0
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 0
Score: 35

## Summary

Improve the existing agent-facing diagnostic interface across syntax, names,
types, effects, ownership, project contracts and input failures. Preserve normal
program acceptance, native raw identities, diagnostic ordering and session
transports. Rich JSON comes from the production SLIM checker and captured input,
with a concrete rejected-rule explanation and small repair context.

## Motivation

The current launcher turns precise native identities into a generic rejection.
An agent must reconstruct facts already known at the rejecting decision. Type
operands are discarded before reporting, and an earlier move's location is not
retained. The maintainer explicitly requested implementation on 2026-10-03,
clarified that this covers diagnostics generally, and required the compiler to
explain its rejection rather than use missing data as a substitute for help.

## Guide-level explanation

Use the existing command:

```sh
./slimc --message-format=json check SOURCE
```

Rejected inputs produce schema-1 JSON lines on stderr and no program on stdout.
The report explains the violated rule, identifies the original source interval,
and includes bounded captured context. Examples include a missing comma, an
unresolved name/import, incompatible checked types, a missing declared effect,
an invalid ownership operation, and a missing manifest-referenced file.

The agent compares the reported context with its retained input, chooses a
change, and runs ordinary checking and application acceptance again. Reports
neither apply edits nor decide whether the program expresses the intended task.

## Reference-level specification

Preserve schema-1 field names/types: schema, code, severity, message, file, span,
labels, notes and fixes. Add a structured repair object. It reports the cause,
producer/category, source context and available causal facts. The primary cause
must describe the rejected rule, not merely repeat "rejected by the compiler".
Code-family explanations must not invent a more specific cause for overloaded
codes. Producer facts distinguish specific causes where the checker knows them.

Primary and related locations name their actual captured source kind: module,
manifest, request or unavailable. Keep raw identity separately when its module
label differs from the location source. In particular, missing-module reads and
missing exports can point into the manifest despite a module label. A failed read
has unavailable contents, not a known empty file. Byte spans remain zero-based,
half-open; positive line/column values are one-based byte locations. Zero means
that a location cannot be supplied within the declared budget.

Source excerpts preserve exact bytes using json-byte-escapes-v1. They are
context, not complete-source authority. No live path is reopened for reporting.
An excerpt, checksum, fabricated revision or caller-supplied hash may not certify
the complete captured input. The caller retains its complete input and compiler
identity and checks current bytes before applying a repair.

At a type mismatch, retain expected and actual checked TypeRef operands before
invalidating inference, and identify the requirement's original source where
present. Intrinsic requirements must not acquire fabricated source declarations.
Related locations for names, effects and ownership derive from the rejecting
decision and its existing linked canonical nodes. An earlier move anchor may be
retained when the actual availability update occurs; do not reconstruct it from
lexical proximity. Declared effects remain capability ceilings, not event logs.

Each fact is exact, bounded with its budget, or unavailable with a stable reason.
Unavailable optional facts do not erase the concrete primary error or turn an
incomplete input into an accepted program. Partial facts from rejected checking
are explanatory error data, not accepted semantic context under RFC-0159.
The root repair status describes primary source and detail admission, not an
aggregate guarantee about optional facts. Inspect each type's evidence and each
location's reason independently. Null optional locations mean that no location
was retained or admitted for that field. The 128-level type budget includes the
scalar or nominal leaf.

Rendering budgets are fixed before implementation: detail at most the first 64
diagnostic rows; excerpt and type-source text at most 256 original bytes;
composite type depth at most 128; line-position scan at most 1 MiB per detailed
row; identity text at most 256 original bytes; complete encoded row at most 8192
bytes. Rows beyond a detail limit retain code and byte interval and explicitly
identify the bounded detail. Oversized identity fields use a complete fallback
with unavailable identity text and original identity length, not silent
truncation. Build a complete row before publishing it; never publish a partial
JSON object. Allocation/output failures retain their existing runtime behavior;
earlier complete diagnostic rows are not an atomic transaction.

## Compiler and runtime design

Implement in selfhost/ SLIM. Use an explicit shared reporter at the existing
reporting boundaries, with raw defaults and a structured reporting route behind
the existing JSON launcher mode. One check/capture determines acceptance. Do not
add a second parser, type checker, executable representation, global mutable
reporting state, dependency, runtime ABI or production Rust semantics. The
launcher transports already-produced data; it does not infer semantic facts.

## Compatibility and migration

Default native check/compile outputs, conformance identities, generated program
C and existing session wire results retain their contracts. JSON additions are
optional schema-1 data; changing existing types or byte-span meanings requires a
new schema. Human defaults may retain their existing spelling during this slice.
The internal structured route must fail explicitly when absent; never quietly
substitute a different semantic implementation.

## Diagnostics and failure cases

Preserve every existing independent issue and rejection status within the
checker's current bounds. Report detail limits separately from source rejection.
Do not suggest an automatic change of a type/effect/ownership contract merely to
make a program pass. For incomplete input explain the failed grammar/lookup or
input condition; leave dependent facts unasserted.

## Performance and complexity

Capture small scalar/node facts at existing decisions. Bound serialization,
source scanning and type traversal before allocation/recursion. Default checking
remains approximately linear under its existing named domains. Generated user
programs acquire no reporting work. Preserve all permanent performance and
resource gates; measure baseline/candidate checking, diagnostics and explicit
structured reporting separately on the same host. Absolute times do not certify
other machines. No budget relaxation is authorized.

## Alternatives and drawbacks

Wrapper source rereads can attach stale or incorrect bytes; reject them.
Inferring facts from diagnostic text duplicates semantics; reject that approach.
A code catalog helps explain rules but cannot supply missing causal anchors.
Universal suggested patches would confuse program acceptance with user intent.
Retaining error metadata and threading a reporter adds compiler maintenance and
measured work; the bounded interface must justify that cost in real repairs.

## Test and acceptance plan

Freeze independent expected codes, spans, rules and repaired controls before
candidate execution. Cover parser errors and zero-width spans; unresolved names
and imports; return/annotation/argument/field/assignment/branch types; intrinsic
versus declared requirements; effect and ownership failures; move history;
manifest/module/read locations; Unicode-byte prefixes; relocated projects;
same-length changed source; multiple issues; malformed and successful programs.
Cross fixed rendering budgets and their saturation boundaries. Require stable
JSON, exact captured excerpts, source-map preservation, stderr-only public
output, old raw parity and normal checker acceptance of repaired controls.
Run all AGENTS.md checks and applicable full release/website gates before
committing the production change.

## Ratings and evidence

Safety +1: provenance, unchanged acceptance and independent rejection controls.
Compile 0: bounded added work; performance benefit is not assumed.
Runtime 0: generated applications have no diagnostic runtime work.
Minimal +1: reuse checked nodes, captured input and existing JSON mode.
Analysis +2: preserve facts at their authoritative decision instead of guessing.
Dogfood 0: expected compiler/library repair utility awaits the named acceptance;
broad productivity benefit remains unmeasured. Weighted sum is 70, normalized to 35. This is an
architecture decision with no language primitive, not a budget-relaxation RFC.

## Decision

Accepted from the maintainer's explicit implementation instruction on 2026-10-03
and subsequent clarification that the requested diagnostic improvement is
general. This grants the named diagnostic scope, not new language constructs,
host operations, weakened checks or performance-budget exceptions.

## Implementation

Implemented in the production SLIM checker; 65 independent diagnostic cases,
12 actual development tasks and source-bound compiler gates pass. Same-host
measurements preserve raw output and generated C. Reproducible release, clean install and website gates pass from committed
source. Agent repair benefit remains unmeasured. Operator timing and transient coordination stay in ignored build/.

## Removal and supersession

Replace this reporting design if it duplicates checking, changes acceptance,
loses original provenance, violates retained gates, or fails to help real
repairs. Keep independent acceptance cases and permanent performance controls.
Remove an inferior route rather than retaining two permanent semantic paths.
