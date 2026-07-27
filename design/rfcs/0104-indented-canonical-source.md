# RFC-0104: Indented canonical source

Status: accepted
Implementation: complete
Process: legacy
Kind: compatibility
Primitive: none
Safety: 1
Compile: 0
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 50

## Need

SLIM's typed S-expression source made the initial compiler unusually small, but
it also makes ordinary programs difficult to scan and edit. The canonical AST
boundary in RFC-0103 now lets the source parser change without rewriting typing,
ownership, effects, memory planning, analysis, reduction, parallelism, or code
generation.

SLIM adopts one significant-indentation source syntax. Indentation is exactly
two ASCII spaces per level; tabs, odd indentation, skipped indentation levels,
commas, braces, semicolons, and alternative block spellings are rejected.
Blank lines and `#` line comments do not affect indentation. A colon ends every
header that owns an indented block.

The canonical forms are:

```text
module hello

record Pair:
  left: I64
  right: I64

variant Result:
  Ok(I64)
  Err(Bytes)

fn main(args: Vec[Bytes]) -> I64 effects[io]:
  let shown: Unit = io.println("hello from SLIM")
  0
```

Arguments and payloads are whitespace-separated inside delimiters, so
`f(a b)`, not `f(a, b)`, is the only call spelling. Pure functions omit the
effects clause; an effectful function uses one ordered
`effects[alloc io partial]` clause. Types use `Vec[T]`, `Arena[T]`, and `Id[T]`.
Records use `make T(field = value ...)`; projection is `get(value field)`;
variants use `case T::Case(payload ...)`; tail transfer is `recur(args ...)`.
There are no operators or method-call aliases.

An expression block is zero or more `let` or `set` continuation lines followed
by exactly one result expression. `match value:` owns ordered pattern headers,
and every pattern owns one expression block. `fork:` owns the existing
two-leading-call expression block; indentation changes no concurrency
semantics.

The parser constructs the canonical AST arena directly with original-source
spans. It may use a bounded ephemeral lexical view, but it may not lower
through generated S-expression text or expose a second executable
representation.

## Alternatives

Keeping S-expressions preserves the smallest byte count but leaves the
human-editing problem and makes structural changes unnecessarily broad.

Brace syntax is familiar but adds closing delimiters and invites competing
layout conventions. Tabs make visual depth dependent on editor settings.
Four-space indentation increased the measured source expansion without adding
information. Optional commas, optional effects clauses with an explicit empty
alias, infix operators, method syntax, and both old and new parsers would each
create multiple spellings for the same operation.

Comma-separated Swift or Rust-style calls were prototyped. They were readable
but spent one lexical token per separator. Whitespace-separated arguments keep
the same unambiguous delimiter structure and preserve SLIM's token-economy
goal.

## Costs

The frontend gains indentation-aware lexing and recursive descent, while the
semantic compiler remains unchanged behind the AST boundary. Canonical
formatting and reduction must emit the new syntax. Project manifests, proof
records, and agent patches remain explicitly versioned tooling data and are
not accepted as SLIM programs.

The indented form uses more source bytes because structure is visible as
whitespace. The one-shot migration of 172 repository programs changed 803,456
source bytes to 856,167 bytes: 52,711 bytes or 6.56% more. On the post-migration
canonical corpus of 175 programs, two-space indentation occupies 885,268 bytes.
Changing only leading indentation to four spaces would occupy 1,015,050 bytes
(14.66% more); encoding each level with one tab would occupy 820,377 bytes
(7.33% less). Tabs were rejected despite the byte saving because source depth
would depend on editor tab width and byte columns would no longer correspond
to visible columns. Two spaces are the measured smallest stable space-only
choice.

The neutral lexical-token proxy on that canonical corpus is 180,912 tokens;
two spaces, four spaces, and tabs have the same token count. The frontend must
therefore demonstrate approximately linear scaling and stay within the
existing compile budgets. Runtime code and runtime semantics do not change.

This is an atomic compatibility cutover. Normal compilation never accepts the
old program syntax after migration. Any one-shot repository conversion path is
removed before the cutover commit; the retained S-expression parser is named
`lex_data` and scoped only for versioned non-program tooling grammars:
projects, interfaces, caches, proof records, and agent patches. Project
flattening, reduction, and structural editing clone canonical AST nodes and
format them directly; none serializes an executable program through
`lex_data`.

## Evidence

Acceptance requires positive coverage for every declaration, type, effect, and
expression form; negative and diagnostic coverage for indentation, tabs,
commas, delimiters, strings, and malformed blocks; exact tutorial execution;
deterministic and idempotent canonical formatting; a self-hosted compiler fixed
point; and unchanged typed, ownership, effect, range, reduction, parallelism,
generated-code, and native behavior baselines after source-span updates.

Parser failures use exact half-open byte spans. `E0102` is malformed structure,
`E0103` a tab, `E0104` odd indentation, `E0105` a skipped indentation level,
`E0106` a comma, brace, or semicolon, and `E0107` an unterminated string.
Positive and negative fixtures pin every code and span.

The durable twenty-application parallelism and resource baselines changed only
their `source_bytes` columns to the measured indented files. Every function,
checked site, expression, effect, allocation, trap, owned binding, recurrence
profile, proof refinement, blocker, candidate, selected, executable, and
executed count remained identical; no analysis precision or execution baseline
changed.

The full compiler, governance, performance, reduction, parallelism,
native-comparison, and agent gates remain mandatory. Frontend scaling is
measured geometrically before and after the cutover. No performance budget is
relaxed for the larger source representation.

## Removal

Remove a punctuation form, parser branch, or formatter branch if it creates an
alias or cannot round-trip to one canonical AST. Revert this decision at the
independent RFC-0103 checkpoint if the new parser cannot preserve direct AST
construction, diagnostics, fixed-point self-hosting, or compile budgets.

The concrete syntax may change again only through another atomic, measured
compatibility decision. RFC-0103 keeps that future change local to parsing,
formatting, spans, fixtures, and documentation rather than semantic compiler
passes.
