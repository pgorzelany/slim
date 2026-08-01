# Incremental compilation

Core 0.1 exposes a reusable in-memory compilation session in
`slim::incremental::IncrementalSession`. It does not add language syntax.

## Reuse boundary

A declaration identity is the tuple `(module, kind, declared name)`. The
session keeps two stable 64-bit fingerprints:

- syntax: the declaration's normalized token structure, excluding whitespace,
  comments, and source offsets;
- interface: the callable signature, effects, parameter modes, or complete
  struct/enum layout.

The FNV-1a encoding is defined in the compiler and never uses Rust's
process-randomized hash state. Dependency edges cover user-function calls and
named-type use in signatures, data layouts, and expressions. Ordered maps and
sets make graph output reproducible.

## Update algorithm

The session lexically indexes top-level declarations, then parses and lowers
only new declarations or declarations whose raw layout changed. Cached ASTs
are span-shifted when an earlier edit merely moves an unchanged declaration.

A body edit selects that declaration. An interface edit additionally selects
the reverse transitive dependency closure. Selected function bodies are
checked against a rebuilt declaration lookup; unselected checked bodies are
retained. C definitions are generated only for selected declarations and the
complete translation unit is deterministically reassembled from cached and
fresh fragments.

An unsuccessful structural, lowering, or semantic update does not replace the
last good cache. Structurally malformed input uses the clean compiler's
recovery path and is reported as `fallback_clean` rather than being counted as
incremental work.

## Core 0.4 memory-plan invalidation

Memory plans are declaration-local derived queries. They are not persisted as
a second cache artifact: a function body fingerprint owns its value liveness,
allocation sites, and destruction boundary, while the existing interface
fingerprint already owns return storage, parameter modes, named layouts, and
the `alloc` effect. A private body edit therefore rebuilds that function's plan
with the existing one-declaration work item. A storage-bearing return type,
shared/`@`/`^` parameter mode, data layout, or effect edit follows the existing reverse
interface dependency closure. Failed checking never publishes a plan, so the
transactional last-good rule also prevents stale destruction plans.

## Evidence and current cost

Run the falsifiable work-count benchmark with:

    cargo run --release --bin slim-bench -- incremental

The committed result is
[2026-07-21-incremental.tsv](../benchmarks/results/2026-07-21-incremental.tsv).
For 8,001 declarations, a private body edit parses, lowers, checks, and
generates one declaration; a leaf interface edit checks two; a central edit
checks 4,001; and a no-change update performs zero declaration parse, lower,
check, or generation operations.

Wall time is not yet constant: lexical indexing, lookup reconstruction, AST
assembly, graph reconstruction, and output concatenation still scan the
program. The same 8,001-declaration body edit measured 66 ms on the recorded
machine. Sub-file edit ranges, persistent metadata, and output ropes are
therefore future compiler work; Core 0.1 does not claim millisecond updates on
very large programs yet.
