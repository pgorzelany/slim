import { readFile, mkdir, writeFile } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { marked } from "marked";

const scriptDirectory = path.dirname(fileURLToPath(import.meta.url));
const siteRoot = path.resolve(scriptDirectory, "..");
const repositoryRoot = path.resolve(siteRoot, "..");
const generatedRoot = path.join(siteRoot, "generated");
const publicReferenceRoot = path.join(siteRoot, "public", "reference");

const referenceSources = [
  {
    id: "core",
    path: "docs/CORE.md",
    summary: "Canonical syntax, semantics, built-ins, diagnostics, and tooling boundary.",
  },
  {
    id: "memory",
    path: "docs/MEMORY.md",
    summary: "Ownership, compiler-selected regions, destruction, and allocation failure.",
  },
  {
    id: "projects",
    path: "docs/PROJECTS.md",
    summary: "Manifests, imports, interfaces, caches, and deterministic checking.",
  },
  {
    id: "quality",
    path: "docs/QUALITY.md",
    summary: "Bounded program evidence, finite equivalence, proofs, and structural edits.",
  },
  {
    id: "reduction",
    path: "docs/REDUCTION.md",
    summary: "Direct canonical-source rewrites and semantic analysis contracts.",
  },
  {
    id: "performance",
    path: "docs/PERFORMANCE.md",
    summary: "Permanent compilation, runtime, and parallel regression contracts.",
  },
  {
    id: "parallelism",
    path: "docs/PARALLELISM.md",
    summary: "Reorder-safety, bounded plans, worker ABI, and guarded execution.",
  },
  {
    id: "resources",
    path: "docs/RESOURCE_BOUNDS.md",
    summary: "Recurrence profiles, exact or unknown call work, and explicit limits.",
  },
];

const referenceByBasename = new Map(
  referenceSources.map((entry) => [path.basename(entry.path), entry.id]),
);

function requireMatch(source, pattern, label) {
  const match = source.match(pattern);
  if (!match) {
    throw new Error(`content generation: missing ${label}`);
  }
  return match[1].trim();
}

function escapeHtml(value) {
  return value
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;");
}

function stripTags(value) {
  return value.replace(/<[^>]*>/g, "").replaceAll("&amp;", "&");
}

function slugify(value) {
  return value
    .toLowerCase()
    .replace(/<[^>]*>/g, "")
    .replace(/[^a-z0-9]+/g, "-")
    .replace(/^-+|-+$/g, "");
}

function splitTitle(markdown, sourcePath) {
  const match = markdown.match(/^#\s+(.+)\r?\n/);
  if (!match) {
    throw new Error(`content generation: ${sourcePath} needs one leading title`);
  }
  return {
    title: match[1].trim(),
    body: markdown.slice(match[0].length),
  };
}

function shiftHeadings(markdown, amount) {
  if (amount === 0) return markdown;
  return markdown.replace(/^(#{1,5})\s/gm, (match, marks) => {
    return `${"#".repeat(Math.min(6, marks.length + amount))} `;
  });
}

function rewriteLinks(html) {
  return html.replace(
    /<a href="([^"]+)">([\s\S]*?)<\/a>/g,
    (match, href, label) => {
      if (
        href.startsWith("https://") ||
        href.startsWith("http://") ||
        href.startsWith("#") ||
        href.startsWith("/")
      ) {
        return match;
      }

      const withoutFragment = href.split("#", 1)[0];
      const referenceId = referenceByBasename.get(path.basename(withoutFragment));
      if (referenceId) {
        return `<a href="#reference-${referenceId}">${label}</a>`;
      }

      return `<span class="source-reference" title="${escapeHtml(href)}">${label}</span>`;
    },
  );
}

function addHeadingIds(html, prefix) {
  const counts = new Map();
  return html.replace(
    /<h([1-6])>([\s\S]*?)<\/h\1>/g,
    (match, level, contents) => {
      const base = slugify(stripTags(contents)) || "section";
      const seen = counts.get(base) ?? 0;
      counts.set(base, seen + 1);
      const suffix = seen === 0 ? "" : `-${seen + 1}`;
      return `<h${level} id="${prefix}${base}${suffix}">${contents}</h${level}>`;
    },
  );
}

async function expandExamples(markdown) {
  const pattern =
    /<!--\s*slim-example:\s*([^|]+?)(?:\s*\|\s*output:\s*(.*?))?\s*-->/g;
  const matches = [...markdown.matchAll(pattern)];
  let expanded = markdown;

  for (const match of matches) {
    const sourcePath = match[1].trim();
    const output = match[2]?.trim() ?? null;
    if (!sourcePath.endsWith(".slim") || sourcePath.includes("..")) {
      throw new Error(`content generation: invalid example path ${sourcePath}`);
    }

    const absolutePath = path.resolve(repositoryRoot, sourcePath);
    if (!absolutePath.startsWith(`${repositoryRoot}${path.sep}`)) {
      throw new Error(`content generation: example escapes repository ${sourcePath}`);
    }
    const source = await readFile(absolutePath, "utf8");
    const outputMarkup = output
      ? `<div class="example-output"><span>Output</span><code>${escapeHtml(output)}</code></div>`
      : "";
    const markup = [
      `<figure class="code-example" data-example-source="${escapeHtml(sourcePath)}">`,
      `<figcaption>${escapeHtml(sourcePath)}</figcaption>`,
      `<pre><code class="language-slim">${escapeHtml(source.trimEnd())}</code></pre>`,
      outputMarkup,
      `</figure>`,
    ].join("");
    expanded = expanded.replace(match[0], markup);
  }

  return { markdown: expanded, examples: matches.map((match) => match[1].trim()) };
}

async function renderDocument(sourcePath, options = {}) {
  const source = await readFile(path.join(repositoryRoot, sourcePath), "utf8");
  const { title, body } = splitTitle(source, sourcePath);
  const expanded = options.examples ? await expandExamples(body) : { markdown: body, examples: [] };
  const shifted = shiftHeadings(expanded.markdown, options.headingShift ?? 0);
  const parsed = await marked.parse(shifted, {
    async: true,
    gfm: true,
    mangle: false,
    headerIds: false,
  });
  const linked = rewriteLinks(parsed);
  return {
    title,
    html: addHeadingIds(linked, options.headingPrefix ?? ""),
    examples: expanded.examples,
  };
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

const [design, cargo, hello, surfaceSource, learn, status, ...referenceDocuments] =
  await Promise.all([
    readFile(path.join(repositoryRoot, "DESIGN.md"), "utf8"),
    readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
    readFile(path.join(repositoryRoot, "examples/hello.slim"), "utf8"),
    readFile(path.join(repositoryRoot, "design/surface.tsv"), "utf8"),
    renderDocument("docs/LEARN.md", { examples: true }),
    renderDocument("docs/STATUS.md"),
    ...referenceSources.map(async (entry) => {
      const rendered = await renderDocument(entry.path, {
        headingPrefix: `${entry.id}-`,
        headingShift: 1,
      });
      return { ...entry, ...rendered };
    }),
  ]);

const meta = {
  name: "SLIM",
  expansion: "Small Language for Intelligent Machines",
  milestone: requireMatch(design, /^Status:\s*(.+)$/m, "DESIGN.md status"),
  compilerVersion: requireMatch(cargo, /^version\s*=\s*"([^"]+)"$/m, "Cargo version"),
  updated: requireMatch(design, /^Last updated:\s*(.+)$/m, "DESIGN.md update date"),
};

const statusMilestone = requireMatch(
  await readFile(path.join(repositoryRoot, "docs/STATUS.md"), "utf8"),
  /^Status:\s*(.+)$/m,
  "STATUS.md milestone",
);
const statusVersion = requireMatch(
  await readFile(path.join(repositoryRoot, "docs/STATUS.md"), "utf8"),
  /^Compiler version:\s*(.+)$/m,
  "STATUS.md compiler version",
);
if (statusMilestone !== meta.milestone || statusVersion !== meta.compilerVersion) {
  throw new Error("content generation: STATUS.md disagrees with DESIGN.md or Cargo.toml");
}

const surface = {
  schemaVersion: 1,
  milestone: meta.milestone,
  compilerVersion: meta.compilerVersion,
  generatedFrom: "design/surface.tsv",
  entries: parseSurface(surfaceSource),
};

const content = {
  meta,
  hello: {
    source: hello.trimEnd(),
    sourcePath: "examples/hello.slim",
    output: "hello from SLIM",
  },
  learn,
  status,
  reference: referenceDocuments,
  surface,
};

const llms = `# SLIM — ${meta.expansion}

> Experimental systems language for AI-generated programs.

Current milestone: ${meta.milestone}
Compiler version: ${meta.compilerVersion}

Canonical source uses one S-expression form for each operation. SLIM has strong static typing, explicit effects, affine ownership, exclusive inout borrows, checked arithmetic and indexing, deterministic C11 generation, no tracing garbage collector, bounded program analysis, and guarded automatic fork/join execution.

Start:
- ./slimc check examples/hello.slim
- ./slimc run examples/hello.slim
- ./slimc emit-c examples/hello.slim -o /tmp/hello.c
- ./slimc analyze examples/countdown.slim

Website:
- /learn — compact tested tutorial
- /reference — canonical language and tooling reference
- /status — exact implemented, bounded, and missing capabilities
- /reference/surface.json — accepted syntax, type, effect, builtin, and backend surface

The repository documents and design/surface.tsv are authoritative. Unknown or bounded compiler evidence is never equivalent to proof.
`;

await mkdir(generatedRoot, { recursive: true });
await mkdir(publicReferenceRoot, { recursive: true });
await Promise.all([
  writeFile(
    path.join(generatedRoot, "content.json"),
    `${JSON.stringify(content, null, 2)}\n`,
  ),
  writeFile(
    path.join(publicReferenceRoot, "surface.json"),
    `${JSON.stringify(surface, null, 2)}\n`,
  ),
  writeFile(path.join(siteRoot, "public", "llms.txt"), llms),
]);

console.log(
  `generated ${referenceDocuments.length} references, ${surface.entries.length} surface rows, and ${learn.examples.length} tested examples`,
);
