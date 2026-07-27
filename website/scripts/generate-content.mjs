import { mkdir, readFile, readdir, writeFile } from "node:fs/promises";
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

const appendixSources = [
  ["core", "docs/CORE.md", "Canonical grammar and semantics"],
  ["host", "docs/HOST.md", "Built-ins and host signatures"],
  ["diagnostics", "docs/DIAGNOSTICS.md", "Diagnostic records and schemas"],
  ["compatibility", "docs/COMPATIBILITY.md", "Compatibility and version policy"],
  ["migration-0.9", "docs/MIGRATION-0.9.md", "Migration to SLIM 0.9"],
].map(([slug, sourcePath, label], index) => ({
  slug,
  order: index + 13,
  path: sourcePath,
  route: `/handbook/appendix/${slug}`,
  label,
  kind: "appendix",
}));

const developmentSources = [
  ["overview", "project-overview", "README.md"],
  ["overview", "design", "DESIGN.md"],
  ["overview", "roadmap", "ROADMAP.md"],
  ["overview", "feature-policy", "design/FEATURE_POLICY.md"],
  ["contracts", "incremental", "docs/INCREMENTAL.md"],
  ["contracts", "integer-proofs", "docs/INTEGER_PROOFS.md"],
  ["contracts", "memory", "docs/MEMORY.md"],
  ["contracts", "parallelism", "docs/PARALLELISM.md"],
  ["contracts", "performance", "docs/PERFORMANCE.md"],
  ["contracts", "projects", "docs/PROJECTS.md"],
  ["contracts", "quality", "docs/QUALITY.md"],
  ["contracts", "reduction", "docs/REDUCTION.md"],
  ["contracts", "release", "docs/RELEASE.md"],
  ["contracts", "resource-bounds", "docs/RESOURCE_BOUNDS.md"],
  ["subsystems", "benchmarks", "benchmarks/README.md"],
  ["subsystems", "bootstrap", "bootstrap/README.md"],
  ["subsystems", "conformance", "conformance/README.md"],
  ["subsystems", "self-hosted-compiler", "selfhost/README.md"],
  ["subsystems", "website", "website/README.md"],
].map(([group, slug, sourcePath], order) => ({
  group,
  slug,
  order: order + 1,
  path: sourcePath,
  route: `/development/${group}/${slug}`,
  kind: "development",
}));

const walkthroughByChallenge = new Map([
  ["binary_search", "docs/algorithms/binary-search.md"],
  ["merge_sort", "docs/algorithms/merge-sort.md"],
  ["bfs", "docs/algorithms/breadth-first-search.md"],
  ["edit_distance", "docs/algorithms/edit-distance.md"],
  ["n_queens", "docs/algorithms/n-queens.md"],
  ["game_of_life", "docs/algorithms/game-of-life.md"],
]);

const legacyGuideRoutes = {
  "getting-started": "/handbook/getting-started",
  "modules-and-source-layout": "/handbook/modules-source-and-declarations",
  "scalar-values-and-checked-operations": "/handbook/values-types-and-operators",
  "bindings-evaluation-and-mutation": "/handbook/bindings-evaluation-and-mutation",
  "functions-entry-and-effects": "/handbook/functions-effects-and-control-flow",
  "control-flow-and-recur": "/handbook/functions-effects-and-control-flow",
  "structs-enums-and-patterns": "/handbook/structs-enums-and-patterns",
  "bytes-vectors-arenas-and-ids": "/handbook/storage-and-allocation",
  "ownership-moves-and-borrowing": "/handbook/ownership-borrowing-and-memory",
  "traps-failure-and-allocation-exhaustion": "/handbook/failure-and-host-services",
  "host-io": "/handbook/failure-and-host-services",
  "projects-imports-and-interfaces": "/handbook/projects-imports-and-interfaces",
  "deterministic-parallelism": "/handbook/deterministic-parallelism",
  "diagnostics-and-compiler-tools": "/handbook/compiler-tools-and-evidence",
};

const legacyLanguageRoutes = {
  "lexical-structure": "/handbook/appendix/core",
  "declarations-and-program-grammar": "/handbook/modules-source-and-declarations",
  "types-and-value-categories": "/handbook/values-types-and-operators",
  "expressions-and-evaluation-order": "/handbook/bindings-evaluation-and-mutation",
  "patterns-and-exhaustiveness": "/handbook/structs-enums-and-patterns",
  "effects-termination-and-failure": "/handbook/functions-effects-and-control-flow",
  "ownership-storage-and-lifetimes": "/handbook/ownership-borrowing-and-memory",
  "builtins-and-host-signatures": "/handbook/appendix/host",
  "executables-projects-and-visibility": "/handbook/projects-imports-and-interfaces",
  "diagnostics-tools-and-derived-artifacts": "/handbook/compiler-tools-and-evidence",
};

const legacyContractRoutes = {
  core: "/handbook/appendix/core",
  compatibility: "/handbook/appendix/compatibility",
  diagnostics: "/handbook/appendix/diagnostics",
  host: "/handbook/appendix/host",
  memory: "/development/contracts/memory",
  projects: "/development/contracts/projects",
  quality: "/development/contracts/quality",
  reduction: "/development/contracts/reduction",
  performance: "/development/contracts/performance",
  parallelism: "/development/contracts/parallelism",
  resources: "/development/contracts/resource-bounds",
  release: "/development/contracts/release",
};

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

function proseWordCount(markdown) {
  const prose = markdown
    .replace(/```[\s\S]*?```/g, " ")
    .replace(/<!--[\s\S]*?-->/g, " ")
    .replace(/`[^`]*`/g, " ")
    .replace(/\[([^\]]+)\]\([^)]+\)/g, "$1")
    .replace(/https?:\/\/\S+/g, " ");
  return prose.match(/[A-Za-z0-9]+(?:[-'][A-Za-z0-9]+)*/g)?.length ?? 0;
}

function splitDocument(markdown, sourcePath) {
  const titleMatch = markdown.match(/^#\s+(.+)\r?\n/);
  if (!titleMatch) {
    throw new Error(`content generation: ${sourcePath} needs one leading title`);
  }
  const body = markdown.slice(titleMatch[0].length).replace(/^\s+/, "");
  const paragraphs = body.split(/\r?\n\s*\r?\n/);
  const summaryParagraph = paragraphs.find((paragraph) => {
    const value = paragraph.trim();
    return value
      && !/^(#|[-*+] |\d+\. |```|\|)/.test(value)
      && !/^(Status|Implementation|Process|Audience|Author|Created|DecisionDate|Approver|Kind|Primitive|Safety|Compile|Runtime|Minimal|Analysis|Dogfood|Score):/m.test(value);
  }) ?? titleMatch[1];
  return {
    title: titleMatch[1].trim(),
    summary: plainMarkdown(summaryParagraph),
    body,
    bodyWithoutSummary: body.replace(summaryParagraph, "").replace(/^\s+/, ""),
  };
}

function sectionSummary(markdown, heading) {
  const pattern = new RegExp(
    `^## ${heading}\\s*\\n+([\\s\\S]*?)(?=\\n## |$)`,
    "m",
  );
  const section = markdown.match(pattern)?.[1] ?? "";
  const paragraph = section.split(/\r?\n\s*\r?\n/).find((value) => value.trim());
  return paragraph ? plainMarkdown(paragraph) : "";
}

function parseFields(markdown) {
  return Object.fromEntries(
    markdown.split(/\r?\n/).flatMap((line) => {
      const match = line.match(/^([A-Za-z]+):\s*(.+)$/);
      return match ? [[match[1], match[2].trim()]] : [];
    }),
  );
}

function shiftHeadings(markdown, amount) {
  if (amount === 0) return markdown;
  return markdown.replace(/^(#{1,5})\s/gm, (_match, marks) =>
    `${"#".repeat(Math.min(6, marks.length + amount))} `);
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
      return `<h${level} id="${prefix}${base}${seen === 0 ? "" : `-${seen + 1}`}">${contents}</h${level}>`;
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

function parseFixtureManifest(source, manifestPath, sourceColumn) {
  const lines = source.split(/\r?\n/).filter((line) => line.trim());
  const header = lines.shift().replace(/^#\s*/, "").split("\t");
  return lines.map((line, index) => {
    const fields = line.split("\t");
    if (fields.length !== header.length) {
      throw new Error(`content generation: malformed ${manifestPath} row ${index + 2}`);
    }
    const row = Object.fromEntries(header.map((name, field) => [name, fields[field]]));
    return { ...row, manifestPath, fixturePath: row[sourceColumn] };
  });
}

function parseChallengeManifest(source) {
  if (!source.startsWith("# schema=2\n")) {
    throw new Error("content generation: challenge manifest must use schema 2");
  }
  const lines = source.split(/\r?\n/).filter((line) => line.trim());
  const headerIndex = lines.findIndex((line) => line.startsWith("# challenge\t"));
  if (headerIndex < 0) throw new Error("content generation: challenge manifest header is missing");
  const header = lines[headerIndex].slice(2).split("\t");
  const rows = lines.slice(headerIndex + 1).map((line, index) => {
    const fields = line.split("\t");
    if (fields.length !== header.length) {
      throw new Error(`content generation: malformed challenge row ${index + 1}`);
    }
    return Object.fromEntries(header.map((name, field) => [name, fields[field]]));
  });
  if (rows.length !== 20 || rows.filter((row) => row.featured === "yes").length !== 6) {
    throw new Error("content generation: algorithm gallery requires twenty entries and six featured walkthroughs");
  }
  return rows;
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
  return parts.length === 0
    ? ""
    : `<div class="example-output"><span>Expected</span><code>${escapeHtml(parts.join("\n"))}</code></div>`;
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
    const row = (isProject ? fixtureMaps.project : fixtureMaps.core).get(match[2]);
    if (!row) throw new Error(`content generation: unknown fixture ${match[2]}`);
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
      const resolved = path.normalize(path.join(path.dirname(sourcePath), target)).replaceAll("\\", "/");
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
  const split = splitDocument(source, sourcePath);
  const renderBody = options.omitSummary ? split.bodyWithoutSummary : split.body;
  const expanded = options.fixtures
    ? await expandFixtures(renderBody, fixtureMaps)
    : { markdown: renderBody, replacements: [], fixtures: [] };
  let html = await marked.parse(shiftHeadings(expanded.markdown, options.headingShift ?? 0), {
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
    title: split.title,
    summary: options.summary ?? split.summary,
    html,
    headings: extractHeadings(html),
    wordCount: proseWordCount(source),
    fixtures: expanded.fixtures.map((row) => ({
      id: row.id,
      mode: row.mode,
      path: row.fixturePath,
      expectation: row.expectation,
      manifestPath: row.manifestPath,
    })),
  };
}

async function enumerateChapters(directory) {
  const names = await readdir(path.join(repositoryRoot, directory));
  const chapters = names.map((name) => {
    const match = name.match(/^(\d{2})-([a-z0-9]+(?:-[a-z0-9]+)*)\.md$/);
    if (!match) throw new Error(`content generation: malformed chapter filename ${directory}/${name}`);
    return {
      kind: "handbook",
      order: Number(match[1]),
      slug: match[2],
      path: `${directory}/${name}`,
      route: `/handbook/${match[2]}`,
    };
  }).sort((left, right) => left.order - right.order);
  chapters.forEach((chapter, index) => {
    if (chapter.order !== index + 1) {
      throw new Error(`content generation: non-contiguous chapter order in ${directory}`);
    }
  });
  return chapters;
}

async function enumerateMarkdown(directory, routePrefix, kind) {
  const names = (await readdir(path.join(repositoryRoot, directory)))
    .filter((name) => name.endsWith(".md"))
    .sort();
  return names.map((name, index) => ({
    kind,
    order: index + 1,
    slug: name.slice(0, -3),
    path: `${directory}/${name}`,
    route: `${routePrefix}/${name.slice(0, -3)}`,
  }));
}

async function collectMarkdown(directory, relative = "") {
  const entries = await readdir(directory, { withFileTypes: true });
  const paths = [];
  for (const entry of entries) {
    if (entry.isDirectory() && [".git", ".next", "node_modules", "out", "target"].includes(entry.name)) {
      continue;
    }
    const nextRelative = relative ? `${relative}/${entry.name}` : entry.name;
    const absolute = path.join(directory, entry.name);
    if (entry.isDirectory()) {
      paths.push(...await collectMarkdown(absolute, nextRelative));
    } else if (entry.isFile() && entry.name.endsWith(".md")) {
      paths.push(nextRelative);
    }
  }
  return paths.sort();
}

function parseSurface(source, rfcRouteById) {
  return source.split(/\r?\n/)
    .filter((line) => line.trim() && !line.startsWith("#"))
    .map((line, index) => {
      const fields = line.split("\t");
      if (fields.length !== 4 || fields.some((field) => field.length === 0)) {
        throw new Error(`content generation: malformed surface row ${index + 1}`);
      }
      const [category, name, semanticRole, rfc] = fields;
      const rfcRoute = rfcRouteById.get(rfc);
      if (!rfcRoute) throw new Error(`content generation: surface cites missing ${rfc}`);
      return { category, name, semanticRole, rfc, rfcRoute };
    });
}

const [
  design,
  versionSource,
  hello,
  surfaceSource,
  statusSource,
  coreManifestSource,
  projectManifestSource,
  challengeManifestSource,
  handbookInventory,
  rfcInventory,
  evidenceInventory,
] = await Promise.all([
  readFile(path.join(repositoryRoot, "DESIGN.md"), "utf8"),
  readFile(path.join(repositoryRoot, "VERSION"), "utf8"),
  readFile(path.join(repositoryRoot, "examples/hello.slim"), "utf8"),
  readFile(path.join(repositoryRoot, "design/surface.tsv"), "utf8"),
  readFile(path.join(repositoryRoot, "docs/STATUS.md"), "utf8"),
  readFile(path.join(repositoryRoot, "conformance/manifest.tsv"), "utf8"),
  readFile(path.join(repositoryRoot, "conformance/projects/manifest.tsv"), "utf8"),
  readFile(path.join(repositoryRoot, "benchmarks/challenges/manifest.tsv"), "utf8"),
  enumerateChapters("docs/book/handbook"),
  enumerateMarkdown("design/rfcs", "/rfcs", "rfc"),
  enumerateMarkdown("benchmarks/results", "/development/evidence", "evidence"),
]);

const challengeRows = parseChallengeManifest(challengeManifestSource).map((row, order) => ({
  ...row,
  order: order + 1,
  slug: row.challenge.replaceAll("_", "-"),
  route: `/handbook/examples/${row.challenge.replaceAll("_", "-")}`,
  sourcePath: `benchmarks/challenges/${row.challenge}/program.slim`,
  walkthroughPath: walkthroughByChallenge.get(row.challenge) ?? null,
}));

const publicationBySource = new Map();
const sourceByRoute = new Map();
function publish(sourcePath, route, kind) {
  if (publicationBySource.has(sourcePath)) {
    throw new Error(`content generation: duplicate publication source ${sourcePath}`);
  }
  if (sourceByRoute.has(route)) {
    throw new Error(`content generation: duplicate canonical route ${route}`);
  }
  publicationBySource.set(sourcePath, { route, kind });
  sourceByRoute.set(route, sourcePath);
}

publish("docs/HANDBOOK.md", "/handbook", "handbook-index");
publish("docs/STATUS.md", "/status", "status");
for (const document of handbookInventory) publish(document.path, document.route, document.kind);
for (const document of appendixSources) publish(document.path, document.route, document.kind);
for (const document of developmentSources) publish(document.path, document.route, document.kind);
for (const document of rfcInventory) publish(document.path, document.route, document.kind);
for (const document of evidenceInventory) publish(document.path, document.route, document.kind);
for (const algorithm of challengeRows) {
  if (algorithm.walkthroughPath) publish(algorithm.walkthroughPath, algorithm.route, "algorithm");
}

const excludedMarkdown = new Map([
  ["AGENTS.md", "Internal operational policy for repository agents."],
]);
const markdownFiles = await collectMarkdown(repositoryRoot);
const unclassified = markdownFiles.filter(
  (sourcePath) => !publicationBySource.has(sourcePath) && !excludedMarkdown.has(sourcePath),
);
if (unclassified.length > 0) {
  throw new Error(`content generation: unclassified Markdown: ${unclassified.join(", ")}`);
}
for (const sourcePath of publicationBySource.keys()) {
  if (!markdownFiles.includes(sourcePath)) {
    throw new Error(`content generation: published Markdown is missing ${sourcePath}`);
  }
}

const routeBySource = new Map(
  [...publicationBySource].map(([sourcePath, publication]) => [sourcePath, publication.route]),
);
const coreRows = parseFixtureManifest(coreManifestSource, "conformance/manifest.tsv", "source");
const projectRows = parseFixtureManifest(
  projectManifestSource,
  "conformance/projects/manifest.tsv",
  "manifest",
);
const fixtureMaps = {
  core: new Map(coreRows.map((row) => [row.id, row])),
  project: new Map(projectRows.map((row) => [row.id, row])),
};

const renderedWalkthroughs = new Map(
  await Promise.all(
    [...walkthroughByChallenge].map(async ([challenge, sourcePath]) => [
      challenge,
      await renderDocument(sourcePath, routeBySource, fixtureMaps, {
        omitSummary: true,
        strictLinks: true,
      }),
    ]),
  ),
);

const [handbookIndex, status, handbook, appendices, developmentDocuments, rfcs, evidence] =
  await Promise.all([
    renderDocument("docs/HANDBOOK.md", routeBySource, fixtureMaps, { strictLinks: true }),
    renderDocument("docs/STATUS.md", routeBySource, fixtureMaps, { strictLinks: true }),
    Promise.all(handbookInventory.map(async (chapter) => ({
      ...chapter,
      ...await renderDocument(chapter.path, routeBySource, fixtureMaps, {
        fixtures: true,
        strictLinks: true,
        omitSummary: true,
      }),
    }))),
    Promise.all(appendixSources.map(async (document) => ({
      ...document,
      ...await renderDocument(document.path, routeBySource, fixtureMaps, {
        strictLinks: true,
        omitSummary: true,
      }),
    }))),
    Promise.all(developmentSources.map(async (document) => ({
      ...document,
      ...await renderDocument(document.path, routeBySource, fixtureMaps, {
        strictLinks: true,
        omitSummary: true,
      }),
    }))),
    Promise.all(rfcInventory.map(async (document) => {
      const source = await readFile(path.join(repositoryRoot, document.path), "utf8");
      const fields = parseFields(source);
      const summary = sectionSummary(source, "Summary")
        || sectionSummary(source, "Need")
        || `Historical ${fields.Status ?? "unknown"} SLIM proposal.`;
      return {
        ...document,
        id: `RFC-${document.slug.slice(0, 4)}`,
        number: Number(document.slug.slice(0, 4)),
        status: fields.Status,
        implementation: fields.Implementation,
        process: fields.Process,
        audience: fields.Audience || "developer",
        kind: fields.Kind,
        primitive: fields.Primitive,
        score: Number(fields.Score),
        ratings: {
          safety: Number(fields.Safety),
          compile: Number(fields.Compile),
          runtime: Number(fields.Runtime),
          minimal: Number(fields.Minimal),
          analysis: Number(fields.Analysis),
          dogfood: Number(fields.Dogfood),
        },
        ...await renderDocument(document.path, routeBySource, fixtureMaps, {
          summary,
          omitSummary: false,
          strictLinks: false,
        }),
      };
    })),
    Promise.all(evidenceInventory.map(async (document) => ({
      ...document,
      lifecycle: "historical",
      ...await renderDocument(document.path, routeBySource, fixtureMaps, {
        summary: `Dated engineering evidence from ${document.slug.slice(0, 10)}.`,
        omitSummary: true,
        strictLinks: false,
      }),
    }))),
  ]);

const algorithms = await Promise.all(challengeRows.map(async (algorithm) => ({
  ...algorithm,
  features: algorithm.features.split(","),
  featured: algorithm.featured === "yes",
  source: (await readFile(path.join(repositoryRoot, algorithm.sourcePath), "utf8")).trimEnd(),
  walkthrough: renderedWalkthroughs.get(algorithm.challenge) ?? null,
})));

const rfcRouteById = new Map(rfcs.map((rfc) => [rfc.id, rfc.route]));
const compilerVersion = versionSource.trim();
const meta = {
  name: "SLIM",
  expansion: "Small Language for Intelligent Machines",
  milestone: requireMatch(design, /^Status:\s*(.+)$/m, "DESIGN.md status"),
  compilerVersion,
  updated: requireMatch(design, /^Last updated:\s*(.+)$/m, "DESIGN.md update date"),
  basePath,
};
const statusMilestone = requireMatch(statusSource, /^Status:\s*(.+)$/m, "STATUS.md milestone");
const statusVersion = requireMatch(statusSource, /^Compiler version:\s*(.+)$/m, "STATUS.md compiler version");
if (statusMilestone !== meta.milestone || statusVersion !== compilerVersion) {
  throw new Error("content generation: status metadata disagrees with VERSION or DESIGN.md");
}

const surface = {
  schemaVersion: 2,
  milestone: meta.milestone,
  compilerVersion,
  generatedFrom: "design/surface.tsv",
  entries: parseSurface(surfaceSource, rfcRouteById),
};

const development = {
  overview: developmentDocuments.filter((document) => document.group === "overview"),
  contracts: developmentDocuments.filter((document) => document.group === "contracts"),
  subsystems: developmentDocuments.filter((document) => document.group === "subsystems"),
  evidence,
};

function searchEntry(document, scope, extra = {}) {
  return {
    route: document.route,
    title: document.title,
    summary: document.summary,
    headings: document.headings.map(({ id, title }) => ({ id, title })),
    text: stripTags(document.html).slice(0, 6000),
    scope,
    ...extra,
  };
}

const search = [
  ...handbook.map((document) => searchEntry(document, "current", {
    audience: "user",
    lifecycle: "current",
    tags: [],
  })),
  ...appendices.map((document) => searchEntry(document, "current", {
    audience: "user",
    lifecycle: "current",
    tags: ["appendix"],
  })),
  ...algorithms.map((algorithm) => ({
    route: algorithm.route,
    title: algorithm.title,
    summary: algorithm.summary,
    headings: algorithm.walkthrough?.headings ?? [],
    text: `${algorithm.summary} ${algorithm.category} ${algorithm.time} ${algorithm.space} ${algorithm.features.join(" ")} ${algorithm.walkthrough ? stripTags(algorithm.walkthrough.html) : ""}`.slice(0, 6000),
    scope: "current",
    audience: "user",
    lifecycle: "current",
    tags: [algorithm.category, algorithm.time, algorithm.space, ...algorithm.features],
  })),
  ...developmentDocuments.map((document) => searchEntry(document, "development", {
    audience: "developer",
    lifecycle: "current",
    tags: [document.group],
  })),
  ...evidence.map((document) => searchEntry(document, "evidence", {
    audience: "developer",
    lifecycle: "historical",
    tags: ["evidence"],
  })),
  ...rfcs.map((document) => searchEntry(document, "rfc", {
    audience: document.audience,
    lifecycle: "historical",
    tags: [document.status, document.kind, document.id],
  })),
];

const markdownBySource = new Map(
  await Promise.all(markdownFiles.map(async (sourcePath) => [
    sourcePath,
    await readFile(path.join(repositoryRoot, sourcePath), "utf8"),
  ])),
);
const countSources = (sources) =>
  sources.reduce((total, sourcePath) => total + proseWordCount(markdownBySource.get(sourcePath) ?? ""), 0);
const handbookSources = [
  "docs/HANDBOOK.md",
  ...handbookInventory.map((document) => document.path),
  ...appendixSources.map((document) => document.path),
  ...walkthroughByChallenge.values(),
];
const maintainedSources = markdownFiles.filter((sourcePath) => sourcePath.startsWith("docs/"));
const stats = {
  sequentialHandbookWords: countSources(handbookInventory.map((document) => document.path)),
  algorithmWalkthroughWords: countSources([...walkthroughByChallenge.values()]),
  handbookWords: countSources(handbookSources),
  maintainedCurrentWords: countSources(maintainedSources),
  rfcArchiveWords: countSources(rfcInventory.map((document) => document.path)),
  evidenceArchiveWords: countSources(evidenceInventory.map((document) => document.path)),
  publishedMarkdown: publicationBySource.size,
  excludedMarkdown: excludedMarkdown.size,
};
if (stats.handbookWords > 10_000) {
  throw new Error(`content generation: Handbook prose budget exceeded: ${stats.handbookWords} > 10000`);
}
if (stats.maintainedCurrentWords > 16_000) {
  throw new Error(`content generation: maintained prose budget exceeded: ${stats.maintainedCurrentWords} > 16000`);
}

const rfcCounts = Object.fromEntries(
  ["proposed", "accepted", "rejected", "withdrawn", "superseded"].map((statusName) => [
    statusName,
    rfcs.filter((rfc) => rfc.status === statusName).length,
  ]),
);
const canonicalRoutes = [
  "/",
  "/handbook",
  "/handbook/examples",
  "/development",
  "/development/evidence",
  "/rfcs",
  "/status",
  ...handbook.map((document) => document.route),
  ...appendices.map((document) => document.route),
  ...algorithms.map((algorithm) => algorithm.route),
  ...developmentDocuments.map((document) => document.route),
  ...evidence.map((document) => document.route),
  ...rfcs.map((document) => document.route),
];

const content = {
  meta,
  hello: {
    source: hello.trimEnd(),
    sourcePath: "examples/hello.slim",
    output: "hello from SLIM",
  },
  handbookIndex,
  status,
  handbook,
  appendices,
  algorithms,
  development,
  rfcs,
  rfcCounts,
  surface,
  stats,
  coverage: {
    published: [...publicationBySource].map(([sourcePath, value]) => ({ sourcePath, ...value })),
    excluded: [...excludedMarkdown].map(([sourcePath, reason]) => ({ sourcePath, reason })),
  },
  redirects: {
    guide: legacyGuideRoutes,
    language: legacyLanguageRoutes,
    contracts: legacyContractRoutes,
  },
  routes: canonicalRoutes,
};

const searchPayload = { search };
const chapterList = handbook.map((chapter) => `- ${chapter.route} — ${chapter.title}`).join("\n");
const appendixList = appendices.map((document) => `- ${document.route} — ${document.title}`).join("\n");
const llms = `# SLIM — ${meta.expansion}

> Small systems language for AI-generated programs.

Current milestone: ${meta.milestone}
Compiler version: ${meta.compilerVersion}

Start:
- ./slimc check examples/hello.slim
- ./slimc run examples/hello.slim
- ./slimc emit-c examples/hello.slim -o /tmp/hello.c

Indexes:
- /handbook — progressive language handbook
- /handbook/examples — twenty tested algorithm programs
- /development — compiler contracts, architecture, and evidence
- /rfcs — accepted, rejected, and proposed design records
- /status — exact implemented, bounded, and missing capabilities
- /reference/surface.json — accepted language surface, linked to RFCs

Handbook:
${chapterList}

Appendices:
${appendixList}

Canonical parsed SLIM is the sole accepted program representation. docs/CORE.md is normative. Unknown or bounded compiler evidence is never equivalent to proof.
`;

await mkdir(generatedRoot, { recursive: true });
await mkdir(publicReferenceRoot, { recursive: true });
await Promise.all([
  writeFile(path.join(generatedRoot, "content.json"), `${JSON.stringify(content, null, 2)}\n`),
  writeFile(path.join(generatedRoot, "search.json"), `${JSON.stringify(searchPayload, null, 2)}\n`),
  writeFile(path.join(publicReferenceRoot, "surface.json"), `${JSON.stringify(surface, null, 2)}\n`),
  writeFile(path.join(siteRoot, "public", "llms.txt"), llms),
]);

console.log(
  `generated ${handbook.length} Handbook chapters, ${appendices.length} appendices, ${algorithms.length} algorithms, ${developmentDocuments.length} development documents, ${rfcs.length} RFCs, ${evidence.length} evidence reports, and ${publicationBySource.size} classified Markdown sources`,
);
console.log(
  `documentation words: Handbook ${stats.handbookWords}/10000; maintained ${stats.maintainedCurrentWords}/16000; RFC archive ${stats.rfcArchiveWords}; evidence archive ${stats.evidenceArchiveWords}`,
);
