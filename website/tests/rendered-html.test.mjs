import assert from "node:assert/strict";
import { access, readFile } from "node:fs/promises";
import path from "node:path";
import test from "node:test";
import { fileURLToPath } from "node:url";

const siteRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const generated = JSON.parse(
  await readFile(path.join(siteRoot, "generated/content.json"), "utf8"),
);
const basePath = process.env.PAGES_BASE_PATH ?? "";
const siteUrl =
  process.env.NEXT_PUBLIC_SITE_URL ?? "https://pgorzelany.github.io/slim/";
const allowedInternalLinks = new Set([
  ...generated.routes,
  "/reference/surface.json",
]);

async function render(pathname) {
  const relativePath =
    pathname === "/" ? "index.html" : `${pathname.slice(1)}/index.html`;
  return readFile(path.join(siteRoot, "out", relativePath), "utf8");
}

function canonicalInternalPath(href) {
  let pathname = new URL(href, "https://example.invalid").pathname;
  if (basePath && pathname.startsWith(basePath)) {
    pathname = pathname.slice(basePath.length) || "/";
  }
  if (pathname !== "/" && pathname.endsWith("/")) pathname = pathname.slice(0, -1);
  return pathname;
}

const routeTitles = new Map([
  ["/", "SLIM — Small Language for Intelligent Machines"],
  ["/learn", "Learn · SLIM"],
  ["/reference", "Reference · SLIM"],
  ["/status", "Status · SLIM"],
  ...generated.guide.map((chapter) => [chapter.route, `${chapter.title} · SLIM`]),
  ...generated.languageReference.map((chapter) => [chapter.route, `${chapter.title} · SLIM`]),
  ...generated.reference.map((document) => [document.route, `${document.title} · SLIM`]),
]);

for (const [pathname, title] of routeTitles) {
  test(`statically renders ${pathname}`, async () => {
    const html = await render(pathname);
    assert.match(html, new RegExp(`<title>${title.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")}</title>`, "i"));
    assert.match(html, /<main\b[^>]*\bid="main"/i);
    assert.match(html, new RegExp(`href="${basePath}/learn/?"`));
    assert.match(html, new RegExp(`href="${basePath}/reference/?"`));
    assert.match(html, new RegExp(`href="${basePath}/status/?"`));
    assert.match(html, /name="robots" content="index, follow"/i);
    assert.match(html, new RegExp(`${siteUrl.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")}og-indented\\.png`));
    assert.doesNotMatch(html, /codex-preview|react-loading-skeleton|Starter Project/);

    for (const match of html.matchAll(/<a\b[^>]*\bhref="([^"]+)"/g)) {
      const href = match[1];
      if (href.startsWith("/") && !href.includes("/_next/")) {
        assert.ok(
          allowedInternalLinks.has(canonicalInternalPath(href)),
          `unexpected internal link ${href} on ${pathname}`,
        );
      }
      if (href.startsWith("#")) {
        assert.match(html, new RegExp(`id="${href.slice(1)}"`), `missing ${href} on ${pathname}`);
      }
    }
  });
}

test("guide index exposes chapters, search, and legacy fragments", async () => {
  const html = await render("/learn");
  assert.match(html, /id="book-search-input"/);
  assert.match(html, /aria-live="polite"/);
  assert.match(html, /\/learn\/getting-started\//);
  assert.match(html, /\/learn\/diagnostics-and-compiler-tools\//);
  for (const id of [
    "run-and-format-a-program",
    "scalar-values-and-checked-operations",
    "bindings-evaluation-mutation-and-repetition",
    "complete-accepted-surface",
  ]) {
    assert.match(html, new RegExp(`id="${id}"`));
  }
});

test("chapter pages expose book navigation and fixture-backed examples", async () => {
  const first = await render("/learn/getting-started");
  const middle = await render("/learn/ownership-moves-and-borrowing");
  assert.match(first, /aria-label="Breadcrumb"/);
  assert.match(first, /aria-label="The SLIM Guide chapters"/);
  assert.match(first, /aria-label="On this page"/);
  assert.match(first, /Browse (?:<!-- -->)?The SLIM Guide/);
  assert.match(first, /rel="next"/);
  assert.doesNotMatch(first, /rel="prev"/);
  assert.match(first, /data-fixture-id="scalars"/);
  assert.match(first, /data-fixture-id="type-mismatch"/);
  assert.match(first, /Exact diagnostics/);
  assert.match(middle, /rel="prev"/);
  assert.match(middle, /rel="next"/);
});

test("reference index preserves contracts and exact generated surface", async () => {
  const html = await render("/reference");
  assert.match(html, /\/reference\/language\/lexical-structure\//);
  assert.match(html, /id="reference-core"/);
  assert.match(html, /id="reference-compatibility"/);
  assert.match(html, /design\/surface\.tsv/);
  assert.match(html, /i64\.add/);
  assert.match(html, /lexical-two-call-fork/);
});

test("public indexing and machine-readable artifacts cover every route", async () => {
  const [llms, surface, robots, sitemap] = await Promise.all([
    readFile(path.join(siteRoot, "out", "llms.txt"), "utf8"),
    readFile(path.join(siteRoot, "out", "reference", "surface.json"), "utf8"),
    readFile(path.join(siteRoot, "out", "robots.txt"), "utf8"),
    readFile(path.join(siteRoot, "out", "sitemap.xml"), "utf8"),
    access(path.join(siteRoot, "out", "404.html")),
  ]);
  assert.match(llms, /\/reference\/contracts\/core/);
  assert.equal(JSON.parse(surface).schemaVersion, 1);
  assert.match(robots, /Allow: \//);
  assert.match(robots, new RegExp(`${siteUrl.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")}sitemap\\.xml`));
  for (const route of generated.routes) {
    const absolute = new URL(
      route === "/" ? "." : `${route.slice(1)}/`,
      siteUrl.endsWith("/") ? siteUrl : `${siteUrl}/`,
    ).toString();
    assert.match(sitemap, new RegExp(absolute.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")));
  }
});
