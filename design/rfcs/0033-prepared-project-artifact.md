# RFC-0033: Prepared project artifact

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

Project validation, compilation, and cache misses previously treated the same
project as several unrelated inputs. They re-read module files, rebuilt a
flattened source, re-lexed it, and discarded the checked view before emission.
Besides wasted work, flattening erased the module identity and original byte
span needed by semantic diagnostics.

SLIM now prepares one compiler-owned project artifact. It retains the validated
manifest, one flattened source and token stream, the token-to-origin map, and
the type facts and issues produced by checking. Scheduling and C emission use
that artifact, and a persistent-cache miss emits from the same artifact that it
checked. This is an internal data-flow rule and adds no source-language feature.

## Alternatives

Re-reading files in each pass keeps individual functions superficially small
but makes compilation work proportional to the number of consumers and permits
the filesystem to change between checking and emission. Persisting flattened
source as a trusted project IR would create another input format and stale-data
boundary. Checking each module independently cannot validate qualified calls or
whole-project entry semantics.

The prepared artifact instead remains derived, in memory, and valid for one
compiler invocation. The manifest and module sources remain the only authority.

## Costs

The compiler retains flattened bytes, lexical tokens, source origins, and typed
facts simultaneously. All are linear in project size. The origin table stores
one small record per flattened token; synthetic wrapper tokens use a sentinel
module identity. Consumers must not mutate retained token links incompatibly.

Structured issues now retain inclusive token intervals under RFC-0034, so the
prepared origin table can preserve both point and whole-expression ranges.
RFC-0035 gives the checked artifact a finalized issue stream and migrates
missing-effect diagnostics into it. Legacy exhaustiveness and ownership
diagnostics are still emitted directly by their passes. Their spans are exact
in standalone compilation, but completing project projection for every
diagnostic requires moving those remaining reports into the same structured
issue channel.

## Evidence

The self-hosted project loader now links each already-loaded module range and
flattens those ranges without further file reads. A dedicated conformance
project returns `E0344@app@56:60`, proving that a nested type issue is projected
back to the exact module-local source span. The project compiler and cache miss
both emit from `PreparedProject`. All 86 external fixtures, 2,000 deterministic
malformed-input mutations, the portable C build, and the byte-identical
bootstrap fixed point pass with the retained artifact.

Project scaling remains guarded by the existing wide and deep geometric
budgets. The prepared representation is intentionally usable by future dense
typed queries and incremental compilation rather than being a temporary cache.

## Removal

Replace this artifact only with a smaller single-invocation representation that
preserves one-read project semantics, exact module-local diagnostics, retained
typed facts, deterministic output, linear storage, cache behavior, and the same
bootstrap and performance evidence. Do not restore consumer-specific reload or
recheck paths.
