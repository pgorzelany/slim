import assert from "node:assert/strict";
import { execFileSync, spawnSync } from "node:child_process";
import { access, readFile } from "node:fs/promises";
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

test("tutorial embeds only existing production-checked SLIM files", async () => {
  const learnSource = await readFile(
    path.join(repositoryRoot, "docs/LEARN.md"),
    "utf8",
  );
  const pattern =
    /<!--\s*slim-example:\s*([^|]+?)(?:\s*\|\s*output:\s*(.*?))?\s*-->/g;
  const examples = [...learnSource.matchAll(pattern)];

  assert.equal(examples.length, 4);
  for (const match of examples) {
    const sourcePath = match[1].trim();
    const expectedOutput = match[2]?.trim() ?? null;
    assert.ok(sourcePath.endsWith(".slim"));
    assert.ok(!sourcePath.includes(".."));
    await access(path.join(repositoryRoot, sourcePath));

    const checked = spawnSync("./slimc", ["check", sourcePath], {
      cwd: repositoryRoot,
      encoding: "utf8",
    });
    assert.equal(
      checked.status,
      0,
      `${sourcePath} failed to check:\n${checked.stdout}${checked.stderr}`,
    );

    if (expectedOutput !== null) {
      const run = spawnSync("./slimc", ["run", sourcePath], {
        cwd: repositoryRoot,
        encoding: "utf8",
      });
      assert.equal(
        run.status,
        0,
        `${sourcePath} failed to run:\n${run.stdout}${run.stderr}`,
      );
      assert.equal(run.stdout.trimEnd(), expectedOutput);
    }
  }

  assert.deepEqual(
    generated.learn.examples,
    examples.map((match) => match[1].trim()),
  );
});

test("status metadata agrees with DESIGN.md and Cargo.toml", async () => {
  const [design, cargo, status] = await Promise.all([
    readFile(path.join(repositoryRoot, "DESIGN.md"), "utf8"),
    readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
    readFile(path.join(repositoryRoot, "docs/STATUS.md"), "utf8"),
  ]);
  const designStatus = design.match(/^Status:\s*(.+)$/m)?.[1].trim();
  const cargoVersion = cargo.match(/^version\s*=\s*"([^"]+)"$/m)?.[1].trim();
  const statusMilestone = status.match(/^Status:\s*(.+)$/m)?.[1].trim();
  const statusVersion = status.match(/^Compiler version:\s*(.+)$/m)?.[1].trim();

  assert.equal(generated.meta.milestone, designStatus);
  assert.equal(statusMilestone, designStatus);
  assert.equal(generated.meta.compilerVersion, cargoVersion);
  assert.equal(statusVersion, cargoVersion);
});

test("surface JSON is an exact projection of the accepted ledger", async () => {
  const ledger = await readFile(
    path.join(repositoryRoot, "design/surface.tsv"),
    "utf8",
  );
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
  })
    .trim()
    .split(/\r?\n/);
  assert.deepEqual(
    expected
      .filter((entry) => entry.category === "builtin")
      .map((entry) => entry.name),
    builtins,
  );
});

test("every rendered reference retains a canonical repository source", async () => {
  assert.equal(generated.reference.length, 7);
  for (const document of generated.reference) {
    await access(path.join(repositoryRoot, document.path));
    assert.ok(document.html.length > 100);
    assert.ok(document.title.length > 0);
    assert.ok(document.summary.length > 0);
  }
});

test("agent summary and dependency boundary remain explicit", async () => {
  const [llms, rootCargo, websitePackage] = await Promise.all([
    readFile(path.join(siteRoot, "public/llms.txt"), "utf8"),
    readFile(path.join(repositoryRoot, "Cargo.toml"), "utf8"),
    readFile(path.join(siteRoot, "package.json"), "utf8"),
  ]);

  assert.match(llms, /Small Language for Intelligent Machines/);
  assert.match(llms, /Core 1G guarded automatic parallel execution/);
  assert.match(llms, /\/reference\/surface\.json/);
  assert.doesNotMatch(rootCargo, /website|vinext|react|marked/);
  assert.match(websitePackage, /"marked": "16\.4\.2"/);
});
