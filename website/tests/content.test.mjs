import assert from "node:assert/strict";
import { execFileSync, spawnSync } from "node:child_process";
import { access, readFile, readdir } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";
import test from "node:test";

const siteRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const repositoryRoot = path.resolve(siteRoot, "..");
const generated = JSON.parse(
  await readFile(path.join(siteRoot, "generated/content.json"), "utf8"),
);
const search = JSON.parse(
  await readFile(path.join(siteRoot, "generated/search.json"), "utf8"),
).search;
const generatedSurface = JSON.parse(
  await readFile(path.join(siteRoot, "public/reference/surface.json"), "utf8"),
);

function runtimeExpectation(expectation) {
  const decode = (value) =>
    (value ?? "").replaceAll("\\n", "\n").replaceAll("\\t", "\t").replaceAll("\\\\", "\\");
  return {
    exit: Number(expectation.match(/(?:^|;)exit=([^;]*)/)?.[1] ?? 0),
    stdout: decode(expectation.match(/(?:^|;)stdout=([^;]*)/)?.[1]),
    stderr: decode(expectation.match(/(?:^|;)stderr=([^;]*)/)?.[1]),
  };
}

async function collectFiles(directory, relative = "") {
  const entries = await readdir(directory, { withFileTypes: true });
  const files = [];
  for (const entry of entries) {
    if (
      entry.isDirectory()
      && [".git", ".next", "node_modules", "out", "target"].includes(entry.name)
    ) {
      continue;
    }
    const nextRelative = relative ? `${relative}/${entry.name}` : entry.name;
    const absolute = path.join(directory, entry.name);
    if (entry.isDirectory()) {
      files.push(...await collectFiles(absolute, nextRelative));
    } else if (entry.isFile()) {
      files.push(nextRelative);
    }
  }
  return files.sort();
}

test("Handbook inventory derives stable metadata from canonical filenames", async () => {
  const directory = "docs/book/handbook";
  const names = (await readdir(path.join(repositoryRoot, directory))).sort();
  assert.equal(names.length, 12);
  assert.equal(generated.handbook.length, 12);
  assert.deepEqual(
    generated.handbook.map((chapter) => path.basename(chapter.path)),
    names,
  );
  for (const [index, chapter] of generated.handbook.entries()) {
    const match = names[index].match(/^(\d{2})-([a-z0-9]+(?:-[a-z0-9]+)*)\.md$/);
    assert.ok(match);
    assert.equal(chapter.order, index + 1);
    assert.equal(chapter.slug, match[2]);
    assert.equal(chapter.route, `/handbook/${chapter.slug}`);
    assert.ok(chapter.title.length > 0);
    assert.ok(chapter.summary.length > 30);
    assert.ok(chapter.html.length > 500);
    assert.ok(chapter.headings.length >= 3);
    await access(path.join(repositoryRoot, chapter.path));
  }
  assert.equal(generated.appendices.length, 5);
  assert.equal(new Set(generated.routes).size, generated.routes.length);
});

test("every Markdown source is published once or explicitly excluded", async () => {
  const markdown = (await collectFiles(repositoryRoot))
    .filter((sourcePath) => sourcePath.endsWith(".md"));
  const published = generated.coverage.published.map((entry) => entry.sourcePath);
  const excluded = generated.coverage.excluded.map((entry) => entry.sourcePath);
  assert.deepEqual([...published, ...excluded].sort(), markdown);
  assert.equal(new Set(published).size, published.length);
  assert.equal(new Set(generated.coverage.published.map((entry) => entry.route)).size, published.length);
  assert.deepEqual(generated.coverage.excluded, [{
    sourcePath: "AGENTS.md",
    reason: "Internal operational policy for repository agents.",
  }]);
  assert.equal(generated.stats.publishedMarkdown, published.length);
  assert.equal(generated.stats.excludedMarkdown, excluded.length);
});

test("documentation budgets report distinct current and historical archives", () => {
  assert.ok(generated.stats.sequentialHandbookWords > 0);
  assert.ok(generated.stats.algorithmWalkthroughWords > 0);
  assert.ok(generated.stats.handbookWords <= 10_000);
  assert.ok(generated.stats.maintainedCurrentWords <= 16_000);
  assert.ok(generated.stats.rfcArchiveWords > generated.stats.maintainedCurrentWords);
  assert.ok(generated.stats.evidenceArchiveWords > 0);
});

test("embedded Handbook fixtures execute through production SLIM", async () => {
  const fixtures = generated.handbook.flatMap((chapter) => chapter.fixtures);
  assert.ok(fixtures.length >= 40);
  const unique = new Map(fixtures.map((fixture) => [
    `${fixture.manifestPath}:${fixture.id}`,
    fixture,
  ]));
  for (const required of [
    "example-hello",
    "example-countdown",
    "example-data",
    "example-vector-sum",
    "example-inout",
    "example-bytes",
    "structured-parallel",
  ]) {
    assert.ok([...unique.values()].some((fixture) => fixture.id === required));
  }

  for (const fixture of unique.values()) {
    await access(path.join(repositoryRoot, fixture.path));
    if (fixture.mode === "check-fail") {
      const checked = spawnSync("./slimc", ["check", fixture.path], {
        cwd: repositoryRoot,
        encoding: "utf8",
      });
      assert.equal(checked.status, 1, `${fixture.id} unexpectedly checked`);
      const actual = `${checked.stdout}${checked.stderr}`
        .trim()
        .split(/\r?\n/)
        .filter(Boolean)
        .map((line) => {
          const match = line.match(/^error\[(.+)\]: rejected by the SLIM compiler$/);
          assert.ok(match, `${fixture.id} emitted unexpected diagnostic output: ${line}`);
          return match[1];
        });
      assert.deepEqual(actual, fixture.expectation.split(","), fixture.id);
      continue;
    }

    const checked = spawnSync("./slimc", ["check", fixture.path], {
      cwd: repositoryRoot,
      encoding: "utf8",
    });
    assert.equal(
      checked.status,
      0,
      `${fixture.id} failed to check:\n${checked.stdout}${checked.stderr}`,
    );

    if (fixture.mode === "run" || fixture.mode === "trap") {
      const expected = runtimeExpectation(fixture.expectation);
      const fixtureArguments =
        fixture.id === "storage"
          ? ["--", "conformance/pass/file_input.slim"]
          : fixture.id === "file-input"
            ? ["--", "conformance/pass/empty.input"]
            : [];
      const run = spawnSync("./slimc", ["run", fixture.path, ...fixtureArguments], {
        cwd: repositoryRoot,
        encoding: "utf8",
      });
      assert.equal(run.status, expected.exit, fixture.id);
      assert.equal(run.stdout, expected.stdout, fixture.id);
      assert.equal(run.stderr, expected.stderr, fixture.id);
    }
  }
});

test("canonical version and status metadata agree", async () => {
  const [design, cargo, status, roadmap, version] = await Promise.all([
    readFile(path.join(repositoryRoot, "DESIGN.md"), "utf8"),
    readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
    readFile(path.join(repositoryRoot, "docs/STATUS.md"), "utf8"),
    readFile(path.join(repositoryRoot, "ROADMAP.md"), "utf8"),
    readFile(path.join(repositoryRoot, "VERSION"), "utf8"),
  ]);
  const designStatus = design.match(/^Status:\s*(.+)$/m)?.[1].trim();
  const canonicalVersion = version.trim();
  assert.equal(generated.meta.milestone, designStatus);
  assert.equal(generated.meta.compilerVersion, canonicalVersion);
  assert.equal(status.match(/^Status:\s*(.+)$/m)?.[1].trim(), designStatus);
  assert.equal(status.match(/^Compiler version:\s*(.+)$/m)?.[1].trim(), canonicalVersion);
  assert.equal(roadmap.match(/^Status:\s*(.+)$/m)?.[1].trim(), designStatus);
  assert.equal(cargo.match(/^version\s*=\s*"([^"]+)"$/m)?.[1], canonicalVersion);
});

test("surface JSON exactly projects accepted RFC ownership", async () => {
  const ledger = await readFile(path.join(repositoryRoot, "design/surface.tsv"), "utf8");
  const rfcRoutes = new Map(generated.rfcs.map((rfc) => [rfc.id, rfc.route]));
  const expected = ledger
    .split(/\r?\n/)
    .filter((line) => line.trim() && !line.startsWith("#"))
    .map((line) => {
      const [category, name, semanticRole, rfc] = line.split("\t");
      return { category, name, semanticRole, rfc, rfcRoute: rfcRoutes.get(rfc) };
    });
  assert.equal(generatedSurface.schemaVersion, 2);
  assert.deepEqual(generatedSurface.entries, expected);
  assert.deepEqual(generated.surface.entries, expected);
  for (const entry of expected) {
    const owner = generated.rfcs.find((rfc) => rfc.id === entry.rfc);
    assert.equal(owner?.status, "accepted");
    assert.equal(owner?.implementation, "complete");
  }

  const builtins = execFileSync("./slimc", ["builtins"], {
    cwd: repositoryRoot,
    encoding: "utf8",
  }).trim().split(/\r?\n/);
  assert.deepEqual(
    expected.filter((entry) => entry.category === "builtin").map((entry) => entry.name),
    builtins,
  );
});

test("RFC migration preserves legacy history and validates the new process", async () => {
  assert.equal(generated.rfcs.length, 107);
  assert.deepEqual(generated.rfcCounts, {
    proposed: 0,
    accepted: 99,
    rejected: 8,
    withdrawn: 0,
    superseded: 0,
  });
  const legacy = generated.rfcs.filter((rfc) => rfc.process === "legacy");
  assert.equal(legacy.filter((rfc) => rfc.status === "accepted").length, 98);
  assert.equal(legacy.filter((rfc) => rfc.status === "rejected").length, 8);
  assert.ok(legacy.filter((rfc) => rfc.status === "accepted").every((rfc) =>
    rfc.implementation === "complete"));
  assert.ok(legacy.filter((rfc) => rfc.status === "rejected").every((rfc) =>
    rfc.implementation === "not-planned"));

  const current = generated.rfcs.find((rfc) => rfc.id === "RFC-0108");
  assert.equal(current?.status, "accepted");
  assert.equal(current?.implementation, "complete");
  assert.equal(current?.process, "1");
  assert.equal(current?.route, "/rfcs/0108-documentation-and-rfc-process");
  for (const rfc of generated.rfcs) {
    assert.match(path.basename(rfc.path), /^\d{4}-[a-z0-9-]+\.md$/);
    assert.equal(rfc.id, `RFC-${String(rfc.number).padStart(4, "0")}`);
    await access(path.join(repositoryRoot, rfc.path));
  }
});

test("algorithm gallery is generated from all comparative sources", async () => {
  assert.equal(generated.algorithms.length, 20);
  assert.equal(generated.algorithms.filter((algorithm) => algorithm.featured).length, 6);
  assert.deepEqual(
    generated.algorithms.filter((algorithm) => algorithm.featured).map((algorithm) =>
      algorithm.challenge),
    ["bfs", "merge_sort", "binary_search", "n_queens", "game_of_life", "edit_distance"],
  );
  assert.ok(generated.algorithms.filter((algorithm) => algorithm.featured).every((algorithm) =>
    algorithm.walkthrough && algorithm.walkthrough.wordCount > 100));
  assert.ok(generated.algorithms.filter((algorithm) => !algorithm.featured).every((algorithm) =>
    algorithm.walkthrough === null));

  for (const algorithm of generated.algorithms) {
    const source = await readFile(path.join(repositoryRoot, algorithm.sourcePath), "utf8");
    assert.equal(algorithm.source, source.trimEnd());
    assert.ok(algorithm.title.length > 0);
    assert.ok(algorithm.summary.length > 30);
    assert.ok(algorithm.time.length > 0);
    assert.ok(algorithm.space.length > 0);
    assert.ok(algorithm.features.length >= 2);
  }
  const websiteSlimSources = (await collectFiles(siteRoot))
    .filter((sourcePath) => sourcePath.endsWith(".slim"));
  assert.deepEqual(websiteSlimSources, []);
});

test("search keeps current, development, RFC, and evidence scopes distinct", () => {
  const counts = Object.fromEntries(
    ["current", "development", "rfc", "evidence"].map((scope) => [
      scope,
      search.filter((entry) => entry.scope === scope).length,
    ]),
  );
  assert.deepEqual(counts, {
    current: 37,
    development: 19,
    rfc: 107,
    evidence: 27,
  });
  assert.equal(new Set(search.map((entry) => `${entry.scope}:${entry.route}`)).size, search.length);
  for (const entry of search) {
    assert.ok(generated.routes.includes(entry.route));
    assert.ok(entry.title.length > 0);
    assert.ok(entry.summary.length > 0);
    assert.ok(entry.text.length <= 6000);
  }
  const binarySearch = search.find((entry) => entry.route.endsWith("/binary-search"));
  assert.ok(binarySearch.tags.includes("Searching"));
  assert.ok(binarySearch.tags.includes("O(log n)"));
  assert.ok(binarySearch.tags.includes("vector"));
});

test("agent summary and dependency boundary remain explicit", async () => {
  const [llms, rootCargo, websitePackage] = await Promise.all([
    readFile(path.join(siteRoot, "public/llms.txt"), "utf8"),
    readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
    readFile(path.join(siteRoot, "package.json"), "utf8"),
  ]);
  assert.match(llms, /Small Language for Intelligent Machines/);
  assert.match(llms, /docs\/CORE\.md is normative/);
  assert.match(llms, /\/handbook\/getting-started/);
  assert.match(llms, /\/handbook\/examples/);
  assert.match(llms, /\/development/);
  assert.match(llms, /\/rfcs/);
  assert.doesNotMatch(rootCargo, /website|react|marked/);
  assert.match(websitePackage, /"marked": "16\.4\.2"/);
  assert.doesNotMatch(websitePackage, /algolia|lunr|flexsearch|wrangler|vinext/);
});
