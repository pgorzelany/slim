import { readFile, readdir, mkdir, writeFile } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { marked } from "marked";

const scriptDirectory = path.dirname(fileURLToPath(import.meta.url));
const siteRoot = path.resolve(scriptDirectory, "..");
const repositoryRoot = path.resolve(siteRoot, "..");
const generatedRoot = path.join(siteRoot, "generated");
const publicReferenceRoot = path.join(siteRoot, "public", "reference");
const basePath = process.env.PAGES_BASE_PATH ?? "";

if (basePath !== "" && (!basePath.startsWith("/") || basePath.endsWith("/"))) {
  throw new Error("content generation: invalid PAGES_BASE_PATH");
}

const contractSources = [
  ["core", "docs/CORE.md", "Canonical syntax, semantics, built-ins, diagnostics, and tooling boundary."],
  ["compatibility", "docs/COMPATIBILITY.md", "Major-version source, project, diagnostics, schema, and ABI promises."],
  ["diagnostics", "docs/DIAGNOSTICS.md", "Stable diagnostic identities, byte spans, ordering, and JSON schema."],
  ["memory", "docs/MEMORY.md", "Ownership, compiler-selected regions, destruction, and allocation failure."],
  ["projects", "docs/PROJECTS.md", "Manifests, imports, interfaces, caches, and deterministic checking."],
  ["quality", "docs/QUALITY.md", "Bounded program evidence, finite equivalence, proofs, and structural edits."],
  ["reduction", "docs/REDUCTION.md", "Direct canonical-source rewrites and semantic analysis contracts."],
  ["performance", "docs/PERFORMANCE.md", "Permanent compilation, runtime, and parallel regression contracts."],
  ["parallelism", "docs/PARALLELISM.md", "Reorder-safety, bounded plans, worker ABI, and guarded execution."],
  ["resources", "docs/RESOURCE_BOUNDS.md", "Recurrence profiles, exact or unknown call work, and explicit limits."],
  ["host", "docs/HOST.md", "Effect-gated host services, typed failure, and capability limits."],
  ["release", "docs/RELEASE.md", "Reproducible source packaging, clean bootstrap, and supported targets."],
].map(([id, sourcePath, summary], order) => ({
  id,
  order: order + 1,
  path: sourcePath,
  route: `/reference/contracts/${id}`,
  summary,
}));

function requireMatch(source, pattern, label) {
  const match = source.match(pattern);
  if (!match) throw new Error(`content generation: missing ${label}`);
  return match[1].trim();
}

function escapeHtml(value) {
  return value
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;");
}

function decodeEntities(value) {
  return value
    .replaceAll("&amp;", "&")
    .replaceAll("&lt;", "<")
    .replaceAll("&gt;", ">")
    .replaceAll("&quot;", '"')
    .replaceAll("&#39;", "'");
}

function stripTags(value) {
  return decodeEntities(value.replace(/<[^>]*>/g, " "))
    .replace(/\s+/g, " ")
    .trim();
}

function plainMarkdown(value) {
  return value
    .replace(/\[([^\]]+)\]\([^)]+\)/g, "$1")
    .replace(/[`*_]/g, "")
    .replace(/\s+/g, " ")
    .trim();
}

function slugify(value) {
  return stripTags(value)
    .toLowerCase()
    .replace(/[^a-z0-9]+/g, "-")
    .replace(/^-+|-+$/g, "");
}

function splitDocument(markdown, sourcePath) {
  const titleMatch = markdown.match(/^#\s+(.+)\r?\n/);
  if (!titleMatch) {
    throw new Error(`content generation: ${sourcePath} needs one leading title`);
  }
  const body = markdown.slice(titleMatch[0].length).replace(/^\s+/, "");
  const summaryParagraph = body.split(/\r?\n\s*\r?\n/, 1)[0];
  if (!summaryParagraph || /^(#|[-*+] |\d+\. |```)/.test(summaryParagraph)) {
    throw new Error(`content generation: ${sourcePath} needs an opening summary paragraph`);
  }
  return {
    title: titleMatch[1].trim(),
    summary: plainMarkdown(summaryParagraph),
    body,
    bodyWithoutSummary: body.slice(summaryParagraph.length).replace(/^\s+/, ""),
  };
}

function shiftHeadings(markdown, amount) {
  if (amount === 0) return markdown;
  return markdown.replace(/^(#{1,5})\s/gm, (_match, marks) => {
    return `${"#".repeat(Math.min(6, marks.length + amount))} `;
  });
}

function routeHref(route, fragment = "") {
  return `${basePath}${route}/${fragment ? `#${fragment}` : ""}`;
}

function addHeadingIds(html, prefix) {
  const counts = new Map();
  return html.replace(
    /<h([1-6])>([\s\S]*?)<\/h\1>/g,
    (_match, level, contents) => {
      const base = slugify(contents) || "section";
      const seen = counts.get(base) ?? 0;
      counts.set(base, seen + 1);
      const suffix = seen === 0 ? "" : `-${seen + 1}`;
      return `<h${level} id="${prefix}${base}${suffix}">${contents}</h${level}>`;
    },
  );
}

function extractHeadings(html) {
  return [...html.matchAll(/<h([23]) id="([^"]+)">([\s\S]*?)<\/h\1>/g)].map(
    (match) => ({
      level: Number(match[1]),
      id: match[2],
      title: stripTags(match[3]),
    }),
  );
}

function parseManifest(source, manifestPath, sourceColumn) {
  const lines = source.split(/\r?\n/).filter((line) => line.trim());
  const header = lines.shift().replace(/^#\s*/, "").split("\t");
  return lines.map((line, index) => {
    const fields = line.split("\t");
    if (fields.length !== header.length) {
      throw new Error(`content generation: malformed ${manifestPath} row ${index + 2}`);
    }
    const row = Object.fromEntries(header.map((name, field) => [name, fields[field]]));
    return {
      ...row,
      manifestPath,
      fixturePath: row[sourceColumn],
    };
  });
}

function decodeExpectation(value) {
  return value.replaceAll("\\n", "\n").replaceAll("\\t", "\t").replaceAll("\\\\", "\\");
}

function expectationMarkup(row) {
  if (row.expectation === "-") return "";
  if (row.mode === "check-fail") {
    return `<div class="example-output example-output--diagnostic"><span>Exact diagnostics</span><code>${escapeHtml(row.expectation)}</code></div>`;
  }
  const stdout = row.expectation.match(/(?:^|;)stdout=([^;]*)/)?.[1] ?? "";
  const stderr = row.expectation.match(/(?:^|;)stderr=([^;]*)/)?.[1] ?? "";
  const exit = row.expectation.match(/(?:^|;)exit=([^;]*)/)?.[1] ?? "";
  const parts = [];
  if (stdout) parts.push(`stdout: ${decodeExpectation(stdout).trimEnd()}`);
  if (stderr) parts.push(`stderr: ${decodeExpectation(stderr).trimEnd()}`);
  if (exit && exit !== "0") parts.push(`exit: ${exit}`);
  if (parts.length === 0) return "";
  return `<div class="example-output"><span>Expected</span><code>${escapeHtml(parts.join("\n"))}</code></div>`;
}

async function fixtureMarkup(row) {
  const absolutePath = path.resolve(repositoryRoot, row.fixturePath);
  if (!absolutePath.startsWith(`${repositoryRoot}${path.sep}`)) {
    throw new Error(`content generation: fixture escapes repository ${row.fixturePath}`);
  }
  const source = await readFile(absolutePath, "utf8");
  return [
    `<figure class="code-example" data-fixture-id="${escapeHtml(row.id)}" data-fixture-mode="${escapeHtml(row.mode)}" data-example-source="${escapeHtml(row.fixturePath)}">`,
    `<figcaption><span>${escapeHtml(row.fixturePath)}</span><span>${escapeHtml(row.mode)}</span></figcaption>`,
    `<pre><code class="language-slim">${escapeHtml(source.trimEnd())}</code></pre>`,
    expectationMarkup(row),
    `</figure>`,
  ].join("");
}

async function expandFixtures(markdown, fixtureMaps) {
  const pattern = /<!--\s*slim-(project-)?fixture:\s*([a-z0-9-]+)\s*-->/g;
  const matches = [...markdown.matchAll(pattern)];
  let expanded = markdown;
  const replacements = [];
  const fixtures = [];

  for (const [index, match] of matches.entries()) {
    const isProject = Boolean(match[1]);
    const id = match[2];
    const row = (isProject ? fixtureMaps.project : fixtureMaps.core).get(id);
    if (!row) {
      throw new Error(`content generation: unknown ${isProject ? "project " : ""}fixture ${id}`);
    }
    const placeholder = `SLIMFIXTUREPLACEHOLDER${index}END`;
    expanded = expanded.replace(match[0], placeholder);
    replacements.push({ placeholder, markup: await fixtureMarkup(row) });
    fixtures.push(row);
  }
  return { markdown: expanded, replacements, fixtures };
}

function rewriteLinks(html, sourcePath, routeBySource, strictLinks) {
  return html.replace(
    /<a href="([^"]+)">([\s\S]*?)<\/a>/g,
    (match, href, label) => {
      if (/^(https?:|mailto:|#)/.test(href)) return match;
      if (href.startsWith("/")) {
        const [route, fragment] = href.split("#", 2);
        return `<a href="${routeHref(route.replace(/\/$/, ""), fragment)}">${label}</a>`;
      }
      const [target, fragment] = href.split("#", 2);
      const resolved = path.normalize(path.join(path.dirname(sourcePath), target));
      const route = routeBySource.get(resolved);
      if (route) return `<a href="${routeHref(route, fragment)}">${label}</a>`;
      if (strictLinks && target.endsWith(".md")) {
        throw new Error(`content generation: broken link ${href} in ${sourcePath}`);
      }
      return `<span class="source-reference" title="${escapeHtml(href)}">${label}</span>`;
    },
  );
}

async function renderDocument(sourcePath, routeBySource, fixtureMaps, options = {}) {
  const source = await readFile(path.join(repositoryRoot, sourcePath), "utf8");
  const { title, summary, body, bodyWithoutSummary } = splitDocument(source, sourcePath);
  const renderBody = options.omitSummary ? bodyWithoutSummary : body;
  const expanded = options.fixtures
    ? await expandFixtures(renderBody, fixtureMaps)
    : { markdown: renderBody, replacements: [], fixtures: [] };
  const shifted = shiftHeadings(expanded.markdown, options.headingShift ?? 0);
  let html = await marked.parse(shifted, {
    async: true,
    gfm: true,
  });
  for (const { placeholder, markup } of expanded.replacements) {
    const paragraph = `<p>${placeholder}</p>`;
    if (!html.includes(paragraph)) {
      throw new Error(`content generation: fixture placeholder ${placeholder} was not preserved`);
    }
    html = html.replace(paragraph, markup);
  }
  html = rewriteLinks(html, sourcePath, routeBySource, options.strictLinks ?? false);
  html = addHeadingIds(html, options.headingPrefix ?? "");
  return {
    title,
    summary,
    html,
    headings: extractHeadings(html),
    fixtures: expanded.fixtures.map((row) => ({
      id: row.id,
      mode: row.mode,
      path: row.fixturePath,
      expectation: row.expectation,
      manifestPath: row.manifestPath,
    })),
  };
}

async function enumerateChapters(directory, kind, routePrefix) {
  const names = await readdir(path.join(repositoryRoot, directory));
  const chapters = names.map((name) => {
    const match = name.match(/^(\d{2})-([a-z0-9]+(?:-[a-z0-9]+)*)\.md$/);
    if (!match) {
      throw new Error(`content generation: malformed chapter filename ${directory}/${name}`);
    }
    return {
      kind,
      order: Number(match[1]),
      slug: match[2],
      path: `${directory}/${name}`,
      route: `${routePrefix}/${match[2]}`,
    };
  }).sort((left, right) => left.order - right.order);

  const orders = new Set();
  const slugs = new Set();
  for (const chapter of chapters) {
    if (orders.has(chapter.order) || slugs.has(chapter.slug)) {
      throw new Error(`content generation: duplicate chapter metadata in ${directory}`);
    }
    orders.add(chapter.order);
    slugs.add(chapter.slug);
  }
  chapters.forEach((chapter, index) => {
    if (chapter.order !== index + 1) {
      throw new Error(`content generation: non-contiguous chapter order in ${directory}`);
    }
  });
  return chapters;
}

function parseSurface(source) {
  return source
    .split(/\r?\n/)
    .filter((line) => line.trim() && !line.startsWith("#"))
    .map((line, index) => {
      const fields = line.split("\t");
      if (fields.length !== 4 || fields.some((field) => field.length === 0)) {
        throw new Error(`content generation: malformed surface row ${index + 1}`);
      }
      const [category, name, semanticRole, decision] = fields;
      return { category, name, semanticRole, decision };
    });
}

const [
  design,
  cargo,
  hello,
  surfaceSource,
  statusSource,
  coreManifestSource,
  projectManifestSource,
  guideInventory,
  languageInventory,
] = await Promise.all([
  readFile(path.join(repositoryRoot, "DESIGN.md"), "utf8"),
  readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
  readFile(path.join(repositoryRoot, "examples/hello.slim"), "utf8"),
  readFile(path.join(repositoryRoot, "design/surface.tsv"), "utf8"),
  readFile(path.join(repositoryRoot, "docs/STATUS.md"), "utf8"),
  readFile(path.join(repositoryRoot, "conformance/manifest.tsv"), "utf8"),
  readFile(path.join(repositoryRoot, "conformance/projects/manifest.tsv"), "utf8"),
  enumerateChapters("docs/book/guide", "guide", "/learn"),
  enumerateChapters("docs/book/reference", "language-reference", "/reference/language"),
]);

const coreRows = parseManifest(coreManifestSource, "conformance/manifest.tsv", "source");
const projectRows = parseManifest(projectManifestSource, "conformance/projects/manifest.tsv", "manifest");
const fixtureMaps = {
  core: new Map(coreRows.map((row) => [row.id, row])),
  project: new Map(projectRows.map((row) => [row.id, row])),
};

const routeBySource = new Map([
  ...guideInventory.map((chapter) => [chapter.path, chapter.route]),
  ...languageInventory.map((chapter) => [chapter.path, chapter.route]),
  ...contractSources.map((document) => [document.path, document.route]),
  ["docs/LEARN.md", "/learn"],
  ["docs/STATUS.md", "/status"],
]);

const [learn, status, guide, languageReference, reference] = await Promise.all([
  renderDocument("docs/LEARN.md", routeBySource, fixtureMaps, { strictLinks: true }),
  renderDocument("docs/STATUS.md", routeBySource, fixtureMaps),
  Promise.all(guideInventory.map(async (chapter) => ({
    ...chapter,
    ...await renderDocument(chapter.path, routeBySource, fixtureMaps, {
      fixtures: true,
      strictLinks: true,
      omitSummary: true,
    }),
  }))),
  Promise.all(languageInventory.map(async (chapter) => ({
    ...chapter,
    ...await renderDocument(chapter.path, routeBySource, fixtureMaps, {
      strictLinks: true,
      omitSummary: true,
    }),
  }))),
  Promise.all(contractSources.map(async (document) => ({
    ...document,
    ...await renderDocument(document.path, routeBySource, fixtureMaps, {
      headingPrefix: `${document.id}-`,
    }),
    summary: document.summary,
  }))),
]);

const meta = {
  name: "SLIM",
  expansion: "Small Language for Intelligent Machines",
  milestone: requireMatch(design, /^Status:\s*(.+)$/m, "DESIGN.md status"),
  compilerVersion: requireMatch(cargo, /^version\s*=\s*"([^"]+)"$/m, "Cargo version"),
  updated: requireMatch(design, /^Last updated:\s*(.+)$/m, "DESIGN.md update date"),
};
const statusMilestone = requireMatch(statusSource, /^Status:\s*(.+)$/m, "STATUS.md milestone");
const statusVersion = requireMatch(statusSource, /^Compiler version:\s*(.+)$/m, "STATUS.md compiler version");
if (statusMilestone !== meta.milestone || statusVersion !== meta.compilerVersion) {
  throw new Error("content generation: STATUS.md disagrees with DESIGN.md or Cargo.toml");
}

const surfaceEntries = parseSurface(surfaceSource);
const surface = {
  schemaVersion: 1,
  milestone: meta.milestone,
  compilerVersion: meta.compilerVersion,
  generatedFrom: "design/surface.tsv",
  entries: surfaceEntries,
};

const searchable = [...guide, ...languageReference, ...reference];
const search = searchable.map((document) => ({
  route: document.route,
  title: document.title,
  summary: document.summary,
  headings: document.headings.map((heading) => ({
    id: heading.id,
    title: heading.title,
  })),
  text: stripTags(document.html).slice(0, 6000),
}));
const routes = [
  "/",
  "/learn",
  "/reference",
  "/status",
  ...searchable.map((document) => document.route),
];

const content = {
  meta,
  hello: {
    source: hello.trimEnd(),
    sourcePath: "examples/hello.slim",
    output: "hello from SLIM",
  },
  learn,
  status,
  guide,
  languageReference,
  reference,
  surface,
  search,
  routes,
};

const chapterList = [...guide, ...languageReference]
  .map((chapter) => `- ${chapter.route} — ${chapter.title}`)
  .join("\n");
const contractList = reference
  .map((document) => `- ${document.route} — ${document.title}`)
  .join("\n");
const llms = `# SLIM — ${meta.expansion}

> Small systems language for AI-generated programs.

Current milestone: ${meta.milestone}
Compiler version: ${meta.compilerVersion}

Canonical source uses one two-space significant-indentation form for each operation. SLIM has strong static typing, explicit effects, affine ownership, exclusive inout borrows, checked arithmetic and indexing, deterministic C11 generation, no tracing garbage collector, bounded program analysis, and guarded structured execution.

Start:
- ./slimc check examples/hello.slim
- ./slimc run examples/hello.slim
- ./slimc emit-c examples/hello.slim -o /tmp/hello.c

Indexes:
- /learn — chaptered learner guide
- /reference — language reference, contracts, and accepted surface
- /status — exact implemented, bounded, and missing capabilities
- /reference/surface.json — accepted syntax, type, effect, builtin, and backend surface

Book:
${chapterList}

Contracts:
${contractList}

The repository documents and design/surface.tsv are authoritative. docs/CORE.md is normative. Unknown or bounded compiler evidence is never equivalent to proof.
`;

await mkdir(generatedRoot, { recursive: true });
await mkdir(publicReferenceRoot, { recursive: true });
await Promise.all([
  writeFile(path.join(generatedRoot, "content.json"), `${JSON.stringify(content, null, 2)}\n`),
  writeFile(path.join(publicReferenceRoot, "surface.json"), `${JSON.stringify(surface, null, 2)}\n`),
  writeFile(path.join(siteRoot, "public", "llms.txt"), llms),
]);

console.log(
  `generated ${guide.length} guide chapters, ${languageReference.length} language reference chapters, ${reference.length} contracts, ${surface.entries.length} surface rows, and ${search.length} search entries`,
);
