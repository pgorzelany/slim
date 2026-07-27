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

test("every canonical route renders with valid internal links", async () => {
  for (const pathname of generated.routes) {
    const html = await render(pathname);
    assert.match(html, /<title>[^<]*SLIM[^<]*<\/title>/i, pathname);
    assert.match(html, /<main\b[^>]*\bid="main"/i, pathname);
    assert.match(html, new RegExp(`href="${basePath}/handbook/?"`), pathname);
    assert.match(html, new RegExp(`href="${basePath}/development/?"`), pathname);
    assert.match(html, new RegExp(`href="${basePath}/rfcs/?"`), pathname);
    assert.match(html, new RegExp(`href="${basePath}/status/?"`), pathname);
    assert.match(html, /name="robots" content="index, follow"/i, pathname);
    assert.match(
      html,
      new RegExp(`${siteUrl.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")}og-indented\\.png`),
      pathname,
    );
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
        assert.match(
          html,
          new RegExp(`id="${href.slice(1)}"`),
          `missing ${href} on ${pathname}`,
        );
      }
    }
  }
});

test("Handbook index exposes levels, search, examples, appendices, and RFC-owned surface", async () => {
  const html = await render("/handbook");
  assert.match(html, /id="book-search-input"/);
  assert.match(html, /aria-live="polite"/);
  assert.match(html, />Foundations</);
  assert.match(html, />Systems programming</);
  assert.match(html, />Advanced use</);
  assert.match(html, new RegExp(`${basePath}/handbook/getting-started/`));
  assert.match(html, new RegExp(`${basePath}/handbook/examples/`));
  assert.match(html, new RegExp(`${basePath}/handbook/appendix/core/`));
  assert.match(html, /id="accepted-surface"/);
  assert.match(html, /u8\.to_i64/);
  assert.match(html, new RegExp(`${basePath}/rfcs/0107-experimental-familiar-source/`));
  assert.match(html, new RegExp(`${basePath}/reference/surface\\.json`));
});

test("Handbook chapters expose navigation and exact fixture-backed examples", async () => {
  const first = await render("/handbook/getting-started");
  const middle = await render("/handbook/ownership-borrowing-and-memory");
  assert.match(first, /aria-label="Breadcrumb"/);
  assert.match(first, /aria-label="The SLIM Handbook chapters"/);
  assert.match(first, /aria-label="On this page"/);
  assert.match(first, /Browse (?:<!-- -->)?The SLIM Handbook/);
  assert.match(first, /rel="next"/);
  assert.doesNotMatch(first, /rel="prev"/);
  assert.match(first, /data-fixture-id="example-hello"/);
  assert.match(first, /data-fixture-id="scalars"/);
  assert.match(first, /data-fixture-id="type-mismatch"/);
  assert.match(first, /Exact diagnostics/);
  assert.match(middle, /rel="prev"/);
  assert.match(middle, /rel="next"/);
  assert.match(middle, /affine/i);
  assert.match(middle, /borrow/i);
});

test("algorithm gallery and pages expose metadata, source, and featured walkthroughs", async () => {
  const gallery = await render("/handbook/examples");
  assert.match(gallery, /20(?:<!-- -->)? algorithms/);
  assert.match(gallery, /Search algorithms/);
  assert.match(gallery, /Category/);
  assert.match(gallery, /Binary search/);
  assert.match(gallery, /Levenshtein edit distance/);

  const featured = await render("/handbook/examples/binary-search");
  assert.match(featured, /Problem and result/);
  assert.match(featured, /Ownership and effects/);
  assert.match(featured, /Complexity analysis/);
  assert.match(featured, /What differs from loop-oriented languages/);
  assert.match(featured, /Why the result is deterministic/);
  assert.match(featured, /Comparative benchmark evidence/);
  assert.match(featured, /canonical tested source/);
  assert.match(featured, /module binary_search/);

  const concise = await render("/handbook/examples/sieve");
  assert.match(concise, /This gallery entry is concise/);
  assert.match(concise, /canonical tested source/);
  assert.match(concise, /module sieve/);
});

test("Development exposes all collections and separate word statistics", async () => {
  const development = await render("/development");
  assert.match(development, />Project</);
  assert.match(development, />Current contracts</);
  assert.match(development, />Subsystems</);
  assert.match(development, new RegExp(`${basePath}/development/evidence/`));
  assert.match(development, /Sequential Handbook/);
  assert.match(development, /Featured walkthroughs/);
  assert.match(development, /Maintained current docs/);
  assert.match(development, /RFC archive/);
  assert.match(development, /Evidence archive/);

  const evidence = await render("/development/evidence");
  assert.match(evidence, /27 reports|Find a measurement/);
  assert.match(evidence, /historical/);
  assert.match(evidence, /2026-07-27/);
});

test("RFC index filters records and detail pages expose disposition and ratings", async () => {
  const index = await render("/rfcs");
  assert.match(index, /Search RFCs/);
  assert.match(index, /All statuses/);
  assert.match(index, /All kinds/);
  assert.match(index, /RFC-0108/);
  assert.match(index, /99 accepted/);
  assert.match(index, /8 rejected/);

  const detail = await render("/rfcs/0108-documentation-and-rfc-process");
  assert.match(detail, /implementation complete/);
  assert.match(detail, /aria-label="RFC weighted ratings"/);
  assert.match(detail, /weighted score/);
  assert.match(detail, /Guide-level explanation/);
  assert.match(detail, /Reference-level specification/);
});

test("legacy documentation routes are static noindex redirects", async () => {
  const routes = [
    ["/learn", "/handbook/"],
    ["/learn/ownership-moves-and-borrowing", "/handbook/ownership-borrowing-and-memory/"],
    ["/reference", "/handbook/"],
    ["/reference/language/lexical-structure", "/handbook/appendix/core/"],
    ["/reference/contracts/memory", "/development/contracts/memory/"],
  ];
  for (const [pathname, target] of routes) {
    const html = await render(pathname);
    assert.match(html, /name="robots" content="noindex, follow"/i);
    assert.match(html, /http-equiv="refresh"/i);
    assert.match(html, new RegExp(
      `${basePath}${target}`.replace(/[.*+?^${}()|[\]\\]/g, "\\$&"),
    ));
  }
});

test("public indexing and machine-readable artifacts cover canonical routes", async () => {
  const [llms, surface, robots, sitemap] = await Promise.all([
    readFile(path.join(siteRoot, "out", "llms.txt"), "utf8"),
    readFile(path.join(siteRoot, "out", "reference", "surface.json"), "utf8"),
    readFile(path.join(siteRoot, "out", "robots.txt"), "utf8"),
    readFile(path.join(siteRoot, "out", "sitemap.xml"), "utf8"),
    access(path.join(siteRoot, "out", "404.html")),
  ]);
  assert.match(llms, /\/handbook\/examples/);
  assert.match(llms, /\/development/);
  assert.match(llms, /\/rfcs/);
  assert.equal(JSON.parse(surface).schemaVersion, 2);
  assert.match(robots, /Allow: \//);
  assert.match(
    robots,
    new RegExp(`${siteUrl.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")}sitemap\\.xml`),
  );
  for (const route of generated.routes) {
    const absolute = new URL(
      route === "/" ? "." : `${route.slice(1)}/`,
      siteUrl.endsWith("/") ? siteUrl : `${siteUrl}/`,
    ).toString();
    assert.match(sitemap, new RegExp(absolute.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")));
  }
  assert.doesNotMatch(sitemap, /\/learn\/|\/reference\/language\/|\/reference\/contracts\//);
});
