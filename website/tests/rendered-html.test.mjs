import assert from "node:assert/strict";
import test from "node:test";

const routes = [
  ["/", "SLIM — Small Language for Intelligent Machines"],
  ["/learn", "Learn · SLIM"],
  ["/reference", "Reference · SLIM"],
  ["/status", "Status · SLIM"],
];
const allowedInternalLinks = new Set([
  "/",
  "/learn",
  "/reference",
  "/status",
  "/reference/surface.json",
]);

async function render(pathname) {
  const workerUrl = new URL("../dist/server/index.js", import.meta.url);
  workerUrl.searchParams.set("test", `${process.pid}-${Date.now()}-${pathname}`);
  const { default: worker } = await import(workerUrl.href);

  return worker.fetch(
    new Request(`http://localhost${pathname}`, {
      headers: { accept: "text/html" },
    }),
    {
      ASSETS: {
        fetch: async () => new Response("Not found", { status: 404 }),
      },
    },
    {
      waitUntil() {},
      passThroughOnException() {},
    },
  );
}

for (const [pathname, title] of routes) {
  test(`server-renders ${pathname}`, async () => {
    const response = await render(pathname);
    assert.equal(response.status, 200);
    assert.match(response.headers.get("content-type") ?? "", /^text\/html\b/i);

    const html = await response.text();
    assert.match(html, new RegExp(`<title>${title}</title>`, "i"));
    assert.match(html, /<main\b[^>]*\bid="main"/i);
    assert.match(html, /href="\/learn"/);
    assert.match(html, /href="\/reference"/);
    assert.match(html, /href="\/status"/);
    assert.match(html, /name="robots" content="noindex, nofollow"/i);
    assert.match(html, /http:\/\/localhost(?::3000)?\/og-indented\.png/);
    assert.doesNotMatch(html, /codex-preview|react-loading-skeleton|Starter Project/);

    for (const match of html.matchAll(/<a\b[^>]*\bhref="([^"]+)"/g)) {
      const href = match[1];
      if (href.startsWith("/") && !href.startsWith("/_next/")) {
        assert.ok(allowedInternalLinks.has(href), `unexpected internal link ${href}`);
      }
    }
  });
}

test("home exposes the current boundary and tested hello source", async () => {
  const response = await render("/");
  const html = await response.text();

  assert.match(html, /SLIM 1.0 released/);
  assert.match(html, /examples\/hello\.slim/);
  assert.match(html, /hello from SLIM/);
  assert.match(html, /Small language\./);
  assert.match(html, /Explicit guarantees\./);
});

test("learn renders multiline examples as valid standalone figures", async () => {
  const response = await render("/learn");
  const html = await response.text();
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
  const response = await render("/reference");
  const html = await response.text();

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
