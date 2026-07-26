# D0105: Repository-hosted documentation

Status: accepted
Kind: architecture
Primitive: none
Safety: 0
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 1
Dogfood: 1
Score: 40

## Need

The documentation website accepted by D0072 is a generated static site, but its
initial deployment path wrapped those static routes in a Cloudflare worker and
kept a second source repository for hosting. That runtime and repository do not
provide a documented capability: the site has no request-time data, accounts,
persistence, server API, or dynamic route.

The canonical repository now builds one static export under `website/out/`.
A GitHub Actions workflow checks that export and publishes the exact artifact to
GitHub Pages from the default branch. The default project URL is
`https://pgorzelany.github.io/slim/`; the build receives the Pages base path
explicitly so links and metadata remain valid below `/slim`.

## Alternatives

Keeping the worker deployment would preserve a redundant runtime and hosting
repository. Committing generated HTML to a `gh-pages` branch would add a second
maintained representation of the site. Moving the sources to a separate Pages
repository would break the requirement that documentation be generated and
checked with the compiler source that it describes.

A repository Actions workflow is the smaller boundary. It builds from the
canonical Markdown, ledgers, examples, and compiler in the same commit, uploads
only the tested static artifact, and requires no server process.

## Costs

GitHub Pages is public and serves this project site below the repository name
unless a custom domain is configured. Publishing depends on GitHub Actions and
the Pages service. The website cannot use request headers, server actions,
runtime route handlers, redirects, or other server-only Next.js facilities
without a new accepted decision and a different host.

The website retains pinned Next.js, React, Tailwind, TypeScript, ESLint, and
Markdown dependencies. Vinext, Vite, Wrangler, the Cloudflare adapter, the
worker entry point, and Sites metadata are removed. Compiler and runtime
dependencies, behavior, artifacts, and release contents are unchanged.

## Evidence

The initial static export emitted HTML for `/`, `/learn/`, `/reference/`, and
`/status/` plus `llms.txt` and `reference/surface.json`. D0106 adds only
build-time-enumerated chapter and contract routes, `robots.txt`, and
`sitemap.xml`; every output remains part of the same static artifact. Permanent
tests read the exported files directly, validate route titles and internal
links under both the root and the `/slim` Pages base path, compile every
referenced conformance example, compare the generated surface with
`slimc builtins`, and reject canonical-source drift.

The deployment workflow uses the repository lockfile, runs the complete website
test before upload, grants the build job read-only repository access, and grants
Pages and identity permissions only to the deployment job.

## Removal

Remove the Pages workflow if GitHub Pages can no longer serve the complete
tested static artifact or cannot preserve the repository-coupled build. Replace
it with one accepted deployment path; do not retain simultaneous production
worker and Pages implementations. Preserve the generated static-site boundary
and canonical repository inputs from D0072.
