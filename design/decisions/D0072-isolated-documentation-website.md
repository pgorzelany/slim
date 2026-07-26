# D0072: Isolated documentation website

Status: accepted
Kind: dependency
Primitive: none
Safety: 0
Compile: 0
Runtime: 0
Minimal: 2
Analysis: 1
Dogfood: 1
Score: 30

## Need

SLIM's implementation has outgrown a single README, but it is still changing
too quickly to support a separately maintained marketing site. A small
documentation website is useful only if repository Markdown, accepted surface
records, tested examples, and current milestone data remain its canonical
inputs.

The website is an isolated build-time consumer under `website/`. It may render
four public routes and generate machine-readable documentation artifacts. It
may not add compiler or runtime dependencies, language surface, executable
fallbacks, or a second description of SLIM semantics.

## Alternatives

Keeping only repository Markdown avoids a web toolchain but makes the growing
set of documents difficult to discover. A larger documentation portal,
playground, server-side compiler, database, search service, analytics system,
or hand-maintained feature catalog would add permanent surface and drift risk
before the language is stable.

A generated static site is the smaller boundary. It can fail when canonical
inputs are missing or inconsistent, and it can be removed without affecting
the compiler.

## Costs

The initial website had a pinned Node, vinext, React, Vite, Cloudflare, and
Markdown build toolchain. D0105 replaces the server wrapper and separate hosting
repository with a static Next.js export published from this repository. Website
dependencies remain confined to `website/package.json`; the root Cargo graph,
bootstrap, compiler latency, generated programs, and runtime remain unchanged.

The site has four routes: `/`, `/learn`, `/reference`, and `/status`. It has no
live compiler, accounts, persistence, analytics, search service, version
selector, or public package channel.

## Evidence

The build reads `docs/LEARN.md`, `docs/STATUS.md`, the existing reference
documents, `design/surface.tsv`, `DESIGN.md`, `Cargo.toml`, and tested `.slim`
examples. Permanent tests compile embedded examples, compare promised output,
compare the generated surface with `slimc builtins`, validate canonical status
metadata, check all routes, and reject broken source references.

The website dependencies are absent from the compiler and runtime manifests.
Removing the entire `website/` directory leaves every SLIM production and
verification command unchanged.

## Removal

Remove the website if its maintenance requires duplicated semantic facts, a
compiler service, new language aliases, or changes to compiler/runtime
dependencies. Preserve the canonical Markdown and surface ledgers; no language
artifact may depend on generated website output.
