# RFC-0135: Retained declaration parsing

Status: accepted
Implementation: complete
Process: 1
Audience: developer
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-07
DecisionDate: 2026-09-07
Approver: project-maintainer
Kind: architecture
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 0
Score: 15

## Summary

Memoize successful declaration parses produced by the existing canonical parser.
Retain exact source, lexical context and unlinked canonical tokens under a typed
file/revision owner. Reuse only after complete matching, then reconstruct current
spans and canonical boundaries. Introduce no alternative accepted representation.

## Motivation

Transactional snapshots still parse unchanged declarations on changed input.
Typed checking reuse cannot remove that work. Parser lookahead is a dependency:
body text equality alone is insufficient to reuse a declaration result.

## Guide-level explanation

A retained parse checks all current lexical and indentation rules, parses the
module header normally, and looks up each declaration by name in an indexed
previous successful module parse. A match imports its canonical tokens under the
new revision. A miss calls the same declaration parser as clean compilation.
Syntax success does not establish type, ownership or project acceptance.

## Reference-level specification

A compiler-owned ParseCache records its FileId, parser configuration fingerprint,
original source, lexer results, unlinked canonical tokens, declaration consumed
lexeme/token intervals and a name trie. Duplicate names disable cache publication;
they do not introduce a new parsing diagnostic. The normal checker remains
responsible for duplicate declarations. Module name and declaration kind/name
must match exactly, without hash-only acceptance. Independent epochs cannot reuse
records. A valid current owner is required to publish a new cache.

Lex and check indentation globally using the ordinary lexer before considering
reuse, preserving lexical error priority. Parse the module header normally. At
an actual declaration parser entry, a prior consumed lexeme count identifies one
candidate interval. Match its complete raw source from first lexeme start through
last consumed lexeme end, every consumed lexeme's kind, relative start/end/line
and indentation, and the first unconsumed lookahead lexeme with the same fields
and exact text. EOF matches only EOF. Missing or mismatched context is a miss.
No speculative boundary locator may create accepted nodes.

Successful current declaration parsing reads no lexeme before its entry or after
its returned next index. The first unconsumed lexeme is therefore an explicit
read dependency. Type/name/argument lookahead either consumes the inspected
lexeme or terminates at that lexeme; block/member/arm continuations inspect the
returned next lexeme. Successful item parsing never consults total source length.
Retain this invariant with native accessor observation, including EOF reads and
all successful item calls inside malformed modules. Parser changes that break
this invariant must expand the dependency contract before reuse remains enabled.

Before import, validate old declaration intervals and every token span against
its matched consumed source interval. Copy only token kind/tag and translated
source spans; reset links and recompute canonical next boundaries normally.
Never import typing/name links or old absolute token positions. Matching uses
checked differences and lengths before addition. Missing vectors or invalid
optional records produce misses. Only compiler-owned records may be reused;
arbitrary serialized records cannot approve syntax.

Cache admission is bounded to 1,000,000 lexemes and canonical tokens, 64 MiB source
and a nonempty configuration fingerprint of at most 4,096 bytes. Exceeding an
optional retention capacity runs the ordinary parser and returns no eligible
cache, preserving the language's existing acceptance boundary. Explicit work
reports distinguish grammar executions, reused declarations and imported nodes;
lexing, exact comparisons, indexing and token assembly remain real linear work.

### Original module history

Retain original module ParseCache records separately from the flattened project
cache. Index previous records by the validated manifest module name using the
existing exact byte trie; parser matching also checks the complete source module
name and declaration context. Paths remain current provenance, not reuse keys.
Assign current original-module FileId slots in manifest order starting at one;
slot zero belongs to the flattened source. Reordering establishes new current
owners and never imports old token positions or checked links.

All module syntax, identity, visibility and diagnostic checks remain in their
original order. Append each parse into the shared current module token vector;
its private cache stores local token intervals regardless of that output prefix.
Rebuild declaration name links only on current working tokens. Publish original
module history only with the successful checked project snapshot.

For one preparation, original-module history has separate aggregate token and
lexeme budgets equal to min(1,000,000, the remaining configured canonical-node
budget). Check remaining capacity before adding sizes. If any optional cache is
ineligible or these budgets are exceeded, decline the entire new original-module
history and parse later modules normally. Continue normal project validation;
optional history admission never invents a source-size rejection. Previously
allocated candidate buffers remain subject to the owning epoch's lifetime; these
are retained-record bounds, not an RSS bound or allocator recovery promise.
Aggregate work is unknown if an uncached capacity path has no production count;
native observations still count actual grammar calls. Old successful history is
unchanged on rejection, and missing cache/index vectors produce ordinary misses.

## Compiler and runtime design

Implement in production SLIM using ordinary records/vectors and the existing
syntax parser. Share header parsing, lexical validation and final result creation
between clean and retained entry points. Keep raw token positions private to
current working views. Integrate retained module and flattened-source parses into
the transactional project path under the same configuration/publication rules;
failed candidates must not overwrite successful session history. No new runtime
primitive, dependency, Rust semantics or parsed IR is permitted.

## Compatibility and migration

Preserve 0.9 accepted source, lexical/parser diagnostics and exact current token
spans, tags and boundary structure. Name links are rebuilt normally. Ordinary
compilation remains available through the same grammar operations. The public
session migration, ownership flow orchestration and retained analysis/emission
remain separate required M1 work.

## Diagnostics and failure cases

Retained parsing returns the ordinary ProgramParse result. Bad owners, mismatched
configuration, missing metadata and admission limits are cache misses, not source
errors. Rejected source never publishes an eligible new parse cache. Invalid
source after a reused declaration must report the same first error as clean.

## Performance and complexity

Indexed matching and full source/context comparison are approximately linear in
source plus token count. A successful match skips declaration grammar and its
expression-node allocations but still imports tokens. A changed declaration and
any predecessor whose actual lookahead changed are reparsed. Native observation
must count actual grammar entry and imports separately. Measure geometric edits,
ordinary-path timing, retained latency and storage without claiming scan-free
updates. No existing gate may be relaxed.

## Alternatives and drawbacks

Whole-module source equality fails declaration locality. Matching only a body's
text ignores parser lookahead. A second declaration parser duplicates semantics.
Retaining lexical and canonical buffers adds storage and copy costs that must be
measured. This contract intentionally does not claim avoided whole-source lexing
or final token assembly. It must not become an unused replacement for required
transactional integration.

## Test and acceptance plan

Compare complete ProgramParse diagnostics and every canonical token field with
clean parsing over accepted, rejected and deterministically malformed inputs.
Cover body/header/kind edits, insertion/deletion/reordering/rename, module names,
leading/trailing trivia, comments/CRLF, EOF, multiline arguments, shared-line
boundary errors, malformed delimiters, empty records and enums. Test stale epochs,
configuration changes, missing intervals/tokens/lexemes and capacity misses.
Observe accessor bounds, actual grammar calls and imports on geometric sources.
Exercise project/session recovery, clean generated C, sanitizer and allocation
faults, all required checkpoint gates and complete M1 closure requirements.

## Ratings and evidence

Analysis +2 records explicit parser dependencies and observed reuse boundaries.
Compile remains zero until measured evidence warrants a separate rating change.
No default or incremental latency improvement is presumed.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit M1 goal. This is
not external review and does not waive safety, compatibility or performance gates.

## Implementation

Implemented in production SLIM. The parser query and both original-module and
flattened-project retention use the same ordinary grammar. Transactional checked
publication owns both caches; failure preserves last-good history. Current module
slots follow the validated sorted manifest. Declaration reordering, module
insertion/deletion, relocation, changed output prefixes and missing metadata are
covered without importing previous links or absolute positions.

The component passes 1,695 complete token/diagnostic comparisons, 2,000 malformed
edits and 2,048 allocation-fault ordinals (94 failures, 1,954 successes) with exact
ordinary and ASan/UBSan agreement. Source, token, lexeme and configuration retention
boundaries are crossed by permanent tests. Aggregate original-module token and
lexeme history budgets are tested at capacity and one below the required size;
optional misses preserve ordinary acceptance. Large capacity fixtures establish
syntax equality only, not semantic acceptance.

The integrated session passes complete prepared-state/C comparisons over 94
accepted fixtures, edit/recovery/configuration cases and 2,048 allocation-fault
ordinals (471 failures, 1,577 successes). Native schema 2 separates lexing, function
checks, C generations, declaration grammar executions and parsed-node imports.
For N helpers plus main, a body edit executes two declaration grammars and one
function check through N=4,000. It still lexes three representations, imports
30N+14 syntax nodes and regenerates whole C. Unchanged snapshot reuse skips these
operations. These are bounded fixture observations, not zero total update work.

The reproducible seed is 4,142,232 bytes, SHA-256
`c0359c9f031c5182e2637a5681b9952de8c60273a5881a0155edc1a96be1eb6b`.
Required checkpoint and auxiliary gates pass. Retention has a measured storage
and latency cost: the paired 4,000-helper cold-plus-body-update median is 95.838 ms
versus 91.166 ms at 29ce920; two clean current preparations take 64.716 ms in the
separate comparison. No latency improvement or budget relaxation is claimed.

See the [SLIM Next progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
for current evidence, fixed-record sizes and remaining parent M1 obligations.

## Removal and supersession

Preserve sole canonical parser authority, complete source/lookahead dependencies,
current spans, exact diagnostics, typed revision ownership and durable work gates.
