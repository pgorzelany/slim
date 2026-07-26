# SLIM website

Minimal four-route documentation site for SLIM — Small Language for Intelligent
Machines. The production site is published from this repository at
<https://pgorzelany.github.io/slim/>.

The site does not own language facts. `npm run generate` reads the canonical
repository documents, accepted surface ledger, compiler version, and tested
examples, then creates the build inputs for:

- `/`
- `/learn`
- `/reference`
- `/status`
- `/reference/surface.json`
- `/llms.txt`

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
and runtime do not depend on this project.
