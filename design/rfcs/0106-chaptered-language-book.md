# RFC-0106: Chaptered language book

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 0
Compile: 1
Runtime: 0
Minimal: 1
Analysis: 2
Dogfood: 2
Score: 40

## Need

The repository-coupled website accepted by RFC-0072 and RFC-0105 proves that the
published site describes the same commit as the compiler. Its four-route
presentation, however, places the complete tutorial and every maintained
contract on two long pages. That boundary is difficult to navigate and does
not provide enough room to teach the language's types, effects, ownership,
failure model, project system, and tools without turning one document into an
unstructured catalogue.

SLIM needs a language book with a learner-oriented guide and a precise language
reference. The book must remain a projection of canonical repository Markdown,
tested conformance fixtures, accepted surface records, and maintained
contracts. `docs/CORE.md` remains the normative language definition.

## Alternatives

A separately authored documentation portal could optimize every page
independently, but it would create a second semantic source. A database,
server-rendered search service, hosted compiler, playground, analytics system,
or version selector would add runtime state and deployment surface unrelated to
teaching the current language. Keeping two very long pages avoids more routes
but makes cross-linking, local navigation, and focused search unnecessarily
weak.

The smaller model is a finite set of Markdown chapters. Guide chapters live at
`docs/book/guide/NN-slug.md`; language-reference chapters live at
`docs/book/reference/NN-slug.md`. The filename determines stable order and
route slug. Each file supplies one leading title and one opening summary
paragraph. Generation rejects malformed names, duplicate slugs, missing
metadata, unresolved local Markdown links, and fixture identifiers absent from
the conformance manifests.

## Costs

The static export gains `/learn/<slug>/`,
`/reference/language/<slug>/`, and `/reference/contracts/<id>/` routes.
The existing four routes remain indexes and stable entry points. Existing
`/learn#...` and `/reference#reference-...` targets remain on their index cards.
The route count now grows linearly with the checked chapter and contract
inventories instead of being fixed at four.

The website generator builds headings, breadcrumbs, chapter navigation, a
compact local search index, `llms.txt`, `reference/surface.json`, `robots.txt`,
and `sitemap.xml` from the same commit. Search executes only in the browser,
returns at most twelve results, and has no service, persistence, analytics, or
network request. The site still has no live compiler, server API, account,
database, version selector, or playground, and adds no dependency.

Maintaining chapters is permanent editorial work. Guide explanations may teach
and connect existing rules but may not override `docs/CORE.md` or the focused
contracts. Substantial executable examples are embedded from repository
fixtures and retain their manifest expectations; the website owns no `.slim`
copy.

## Evidence

Generation enumerates every canonical chapter, derives order and slugs from
filenames, extracts the title, summary, and second- and third-level headings,
and fails on malformed or broken input. Static generation enumerates every
chapter and contract route for both root and `/slim` base paths. Export tests
check breadcrumbs, side navigation, table-of-contents links, previous/next
links, legacy fragments, accessible search controls and bounded results,
machine-readable artifacts, public indexing metadata, and every sitemap route.

Referenced positive fixtures are checked or run by the production `slimc` and
their output is compared with the manifest. Referenced negative fixtures are
checked with JSON diagnostics and compared with the manifest's exact diagnostic
codes and byte spans. The accepted surface remains an exact projection of
`design/surface.tsv`, and contract pages remain projections of their canonical
repository documents.

## Removal

Remove chapter routing and local search if maintaining them requires duplicated
language facts, a runtime service, or website-owned program examples. Preserve
the canonical Markdown and tested fixtures. Collapse navigation back to
repository documents without changing the compiler, language, runtime,
conformance corpus, or Pages deployment.
