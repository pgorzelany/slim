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
const generatedSurface = JSON.parse(
  await readFile(path.join(siteRoot, "public/reference/surface.json"), "utf8"),
);

function expectedDiagnostics(expectation) {
  return expectation.split(",").map((entry) => {
    const match = entry.match(/^([^@]+)@(?:[^@]+@)?(\d+):(\d+)$/);
    assert.ok(match, `malformed diagnostic expectation ${entry}`);
    return { code: match[1], start: Number(match[2]), end: Number(match[3]) };
  });
}

function runtimeExpectation(expectation) {
  const decode = (value) =>
    (value ?? "").replaceAll("\\n", "\n").replaceAll("\\t", "\t").replaceAll("\\\\", "\\");
  return {
    exit: Number(expectation.match(/(?:^|;)exit=([^;]*)/)?.[1] ?? 0),
    stdout: decode(expectation.match(/(?:^|;)stdout=([^;]*)/)?.[1]),
    stderr: decode(expectation.match(/(?:^|;)stderr=([^;]*)/)?.[1]),
  };
}

test("chapter inventories derive stable metadata from canonical filenames", async () => {
  const expectations = [
    ["guide", "docs/book/guide", 14, "/learn/"],
    ["languageReference", "docs/book/reference", 10, "/reference/language/"],
  ];

  for (const [key, directory, count, routePrefix] of expectations) {
    const names = (await readdir(path.join(repositoryRoot, directory))).sort();
    assert.equal(names.length, count);
    assert.equal(generated[key].length, count);
    assert.deepEqual(
      generated[key].map((chapter) => path.basename(chapter.path)),
      names,
    );
    for (const [index, chapter] of generated[key].entries()) {
      const match = names[index].match(/^(\d{2})-([a-z0-9]+(?:-[a-z0-9]+)*)\.md$/);
      assert.ok(match);
      assert.equal(chapter.order, index + 1);
      assert.equal(chapter.slug, match[2]);
      assert.equal(chapter.route, `${routePrefix}${chapter.slug}`);
      assert.ok(chapter.title.length > 0);
      assert.ok(chapter.summary.length > 30);
      assert.ok(chapter.html.length > 500);
      assert.ok(chapter.headings.length >= 3);
      await access(path.join(repositoryRoot, chapter.path));
    }
  }

  assert.equal(new Set(generated.routes).size, generated.routes.length);
  assert.equal(generated.routes.length, 40);
});

test("book fixtures execute through production SLIM with exact expectations", async () => {
  const fixtures = [...generated.guide.flatMap((chapter) => chapter.fixtures)];
  assert.ok(fixtures.length >= 30);
  const unique = new Map(fixtures.map((fixture) => [
    `${fixture.manifestPath}:${fixture.id}`,
    fixture,
  ]));

  for (const fixture of unique.values()) {
    await access(path.join(repositoryRoot, fixture.path));
    const isProject = fixture.manifestPath.endsWith("/projects/manifest.tsv");

    if (fixture.mode === "check-fail") {
      const checked = spawnSync(
        "./slimc",
        ["--message-format=json", "check", fixture.path],
        { cwd: repositoryRoot, encoding: "utf8" },
      );
      assert.equal(checked.status, 1, `${fixture.id} unexpectedly checked`);
      const actual = `${checked.stdout}${checked.stderr}`
        .trim()
        .split(/\r?\n/)
        .filter(Boolean)
        .map((line) => {
          const diagnostic = JSON.parse(line);
          return {
            code: diagnostic.code,
            start: diagnostic.span.start,
            end: diagnostic.span.end,
          };
        });
      assert.deepEqual(actual, expectedDiagnostics(fixture.expectation), fixture.id);
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
    } else if (isProject) {
      assert.equal(fixture.mode, "check-pass");
    }
  }
});

test("status metadata agrees with design, roadmap, and Cargo.toml", async () => {
  const [design, cargo, status, roadmap] = await Promise.all([
    readFile(path.join(repositoryRoot, "DESIGN.md"), "utf8"),
    readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
    readFile(path.join(repositoryRoot, "docs/STATUS.md"), "utf8"),
    readFile(path.join(repositoryRoot, "ROADMAP.md"), "utf8"),
  ]);
  const designStatus = design.match(/^Status:\s*(.+)$/m)?.[1].trim();
  const cargoVersion = cargo.match(/^version\s*=\s*"([^"]+)"$/m)?.[1].trim();
  assert.equal(generated.meta.milestone, designStatus);
  assert.equal(status.match(/^Status:\s*(.+)$/m)?.[1].trim(), designStatus);
  assert.equal(roadmap.match(/^Status:\s*(.+)$/m)?.[1].trim(), designStatus);
  assert.equal(generated.meta.compilerVersion, cargoVersion);
  assert.equal(status.match(/^Compiler version:\s*(.+)$/m)?.[1].trim(), cargoVersion);
});

test("surface JSON is an exact projection of the accepted ledger", async () => {
  const ledger = await readFile(path.join(repositoryRoot, "design/surface.tsv"), "utf8");
  const expected = ledger
    .split(/\r?\n/)
    .filter((line) => line.trim() && !line.startsWith("#"))
    .map((line) => {
      const [category, name, semanticRole, decision] = line.split("\t");
      return { category, name, semanticRole, decision };
    });
  assert.deepEqual(generatedSurface.entries, expected);
  assert.deepEqual(generated.surface.entries, expected);

  const builtins = execFileSync("./slimc", ["builtins"], {
    cwd: repositoryRoot,
    encoding: "utf8",
  }).trim().split(/\r?\n/);
  assert.deepEqual(
    expected.filter((entry) => entry.category === "builtin").map((entry) => entry.name),
    builtins,
  );
});

test("contracts and local search retain canonical, bounded sources", async () => {
  assert.equal(generated.reference.length, 12);
  assert.equal(generated.search.length, 36);
  assert.equal(new Set(generated.search.map((entry) => entry.route)).size, 36);
  let contractsWithHeadings = 0;
  for (const document of generated.reference) {
    await access(path.join(repositoryRoot, document.path));
    assert.ok(document.html.length > 100);
    if (document.headings.length > 0) contractsWithHeadings += 1;
  }
  assert.equal(contractsWithHeadings, 11);
  for (const entry of generated.search) {
    assert.ok(generated.routes.includes(entry.route));
    assert.ok(entry.title.length > 0);
    assert.ok(entry.summary.length > 0);
    assert.ok(entry.text.length <= 6000);
  }
});

test("agent summary and dependency boundary remain explicit", async () => {
  const [llms, rootCargo, websitePackage] = await Promise.all([
    readFile(path.join(siteRoot, "public/llms.txt"), "utf8"),
    readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
    readFile(path.join(siteRoot, "package.json"), "utf8"),
  ]);
  assert.match(llms, /Small Language for Intelligent Machines/);
  assert.match(llms, /docs\/CORE\.md is normative/);
  assert.match(llms, /\/learn\/getting-started/);
  assert.match(llms, /\/reference\/language\/lexical-structure/);
  assert.match(llms, /\/reference\/contracts\/core/);
  assert.doesNotMatch(rootCargo, /website|vinext|react|marked/);
  assert.match(websitePackage, /"marked": "16\.4\.2"/);
  assert.doesNotMatch(websitePackage, /algolia|lunr|flexsearch|wrangler|vinext/);
});
