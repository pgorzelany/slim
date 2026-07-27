# SLIM website

The static SLIM documentation site publishes all repository Markdown except
the explicitly internal `AGENTS.md`. Its canonical entry points are:

- `/handbook` for the progressive programming model and exact appendices
- `/handbook/examples` for twenty benchmark-backed algorithm pages
- `/development` for current architecture and compiler contracts
- `/development/evidence` for dated benchmark reports
- `/rfcs` for accepted, rejected, and superseded proposals
- `/status` for the current implemented boundary
- `/reference/surface.json` for the machine-readable accepted surface

The site does not own language facts or duplicate `.slim` programs.
`npm run generate` discovers repository collections, reads `VERSION`, renders
canonical Markdown, embeds conformance fixtures, and reads algorithm source
directly from `benchmarks/challenges/`. It fails when a Markdown file is
unclassified, an internal link is unresolved, an RFC or challenge manifest is
malformed, or a documentation word budget is exceeded.

Search is a bounded browser-local index with separate current, development,
RFC, and evidence scopes. Legacy `/learn/*` and `/reference/*` HTML pages are
static redirects; `/reference/surface.json` remains a stable public artifact.

## Commands

```sh
npm install
npm run dev
npm test
npm run lint
npm run build
```

`npm run build` creates a static export in `out/`. To reproduce the GitHub Pages
subpath build locally:

```sh
PAGES_BASE_PATH=/slim \
NEXT_PUBLIC_SITE_URL=https://pgorzelany.github.io/slim/ \
npm test
```

The repository workflow publishes the tested `out/` artifact after changes
reach `main`. Generated content and deployment output are ignored. The compiler
and runtime do not depend on this project. `docs/CORE.md` remains normative.
