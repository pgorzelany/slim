import assert from "node:assert/strict";
import { access, readFile } from "node:fs/promises";
import path from "node:path";
import test from "node:test";
import { fileURLToPath } from "node:url";

const routes = [
  ["/", "SLIM — Small Language for Intelligent Machines"],
  ["/learn", "Learn · SLIM"],
  ["/reference", "Reference · SLIM"],
  ["/status", "Status · SLIM"],
];
const siteRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const basePath = process.env.PAGES_BASE_PATH ?? "";
const siteUrl =
  process.env.NEXT_PUBLIC_SITE_URL ?? "https://pgorzelany.github.io/slim/";
const allowedInternalLinks = new Set([
  "/",
  "/learn",
  "/reference",
  "/status",
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
  if (pathname !== "/" && pathname.endsWith("/")) {
    pathname = pathname.slice(0, -1);
  }
  return pathname;
}

for (const [pathname, title] of routes) {
  test(`statically renders ${pathname}`, async () => {
    const html = await render(pathname);
    assert.match(html, new RegExp(`<title>${title}</title>`, "i"));
    assert.match(html, /<main\b[^>]*\bid="main"/i);
    assert.match(html, new RegExp(`href="${basePath}/learn/?"`));
    assert.match(html, new RegExp(`href="${basePath}/reference/?"`));
    assert.match(html, new RegExp(`href="${basePath}/status/?"`));
    assert.match(html, /name="robots" content="noindex, nofollow"/i);
    assert.match(html, new RegExp(`${siteUrl.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")}og-indented\\.png`));
    assert.doesNotMatch(html, /codex-preview|react-loading-skeleton|Starter Project/);

    for (const match of html.matchAll(/<a\b[^>]*\bhref="([^"]+)"/g)) {
      const href = match[1];
      if (href.startsWith("/") && !href.includes("/_next/")) {
        assert.ok(
          allowedInternalLinks.has(canonicalInternalPath(href)),
          `unexpected internal link ${href}`,
        );
      }
    }
  });
}

test("home exposes the current boundary and tested hello source", async () => {
  const html = await render("/");

  assert.match(html, /SLIM 1.0 released/);
  assert.match(html, /examples\/hello\.slim/);
  assert.match(html, /hello from SLIM/);
  assert.match(html, /Small language\./);
  assert.match(html, /Explicit guarantees\./);
});

test("learn renders multiline examples as valid standalone figures", async () => {
  const html = await render("/learn");
  const hello = html.match(
    /<figure class="code-example" data-example-source="examples\/hello\.slim">[\s\S]*?<\/figure>/,
  );

  assert.ok(hello, "missing rendered Hello example");
  assert.match(hello[0], /module hello\n\nfn main/);
  assert.doesNotMatch(hello[0], /<p>/);
  assert.doesNotMatch(html, /<p><figure class="code-example"/);
  assert.match(html, /Scalar values and checked operations/);
  assert.match(html, /Bytes, vectors, arenas, and IDs/);
  assert.match(html, /Automatic and explicit parallelism/);
  assert.match(html, /Complete accepted surface/);
  assert.match(html, /data-generated-from="design\/surface\.tsv"/);
  assert.match(html, /data-surface-key="builtin:i64\.add"/);
  assert.match(html, /data-surface-key="syntax:fork"/);

  for (const match of html.matchAll(/href="#([^"]+)"/g)) {
    assert.match(html, new RegExp(`id="${match[1]}"`), `missing target for #${match[1]}`);
  }
});

test("reference renders the generated surface and canonical documents", async () => {
  const html = await render("/reference");

  assert.match(html, /Accepted language surface/);
  assert.match(html, /design\/surface\.tsv/);
  assert.match(html, /reference-core/);
  assert.match(html, /reference-compatibility/);
  assert.match(html, /reference-diagnostics/);
  assert.match(html, /reference-memory/);
  assert.match(html, /reference-projects/);
  assert.match(html, /reference-quality/);
  assert.match(html, /reference-reduction/);
  assert.match(html, /reference-performance/);
  assert.match(html, /reference-parallelism/);
  assert.match(html, /reference-release/);
  assert.match(html, /i64\.add/);
  assert.match(html, /guarded automatic and explicit structured execution/i);
});

test("static export includes the machine-readable artifacts", async () => {
  const [llms, surface] = await Promise.all([
    readFile(path.join(siteRoot, "out", "llms.txt"), "utf8"),
    readFile(path.join(siteRoot, "out", "reference", "surface.json"), "utf8"),
    access(path.join(siteRoot, "out", "404.html")),
  ]);

  assert.match(llms, /Small Language for Intelligent Machines/);
  assert.equal(JSON.parse(surface).schemaVersion, 1);
});
