# SLIM website

Static, chaptered documentation site for SLIM — Small Language for Intelligent
Machines. The production site is published from this repository at
<https://pgorzelany.github.io/slim/>.

The site does not own language facts. `npm run generate` reads the canonical
repository documents, accepted surface ledger, compiler version, and tested
examples, then creates the build inputs for:

- `/`
- `/learn`
- `/learn/<guide-chapter>`
- `/reference`
- `/reference/language/<reference-chapter>`
- `/reference/contracts/<contract-id>`
- `/status`
- `/reference/surface.json`
- `/llms.txt`
- `/robots.txt`
- `/sitemap.xml`

Guide chapters come from `docs/book/guide/NN-slug.md`; language-reference
chapters come from `docs/book/reference/NN-slug.md`. The filename fixes order
and route, while the leading title and opening paragraph provide page metadata.
Generation rejects malformed inventories, unresolved local Markdown links, and
unknown conformance fixture markers.

Search is a bounded browser-local index generated from current chapter titles,
summaries, headings, and text. It has no server or network dependency.

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
and runtime do not depend on this project. `docs/CORE.md` remains normative;
the website owns no language semantics or `.slim` example.
