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

  assert.deepEqual(
    examples.map((match) => match[1].trim()),
    [
      "examples/hello.slim",
      "conformance/pass/scalars.slim",
      "examples/countdown.slim",
      "conformance/pass/data.slim",
      "examples/bytes.slim",
      "conformance/pass/storage.slim",
      "examples/inout.slim",
      "conformance/pass/file_input.slim",
      "conformance/pass/monotonic_clock.slim",
      "conformance/pass/tcp_exchange.slim",
      "conformance/pass/structured_fork.slim",
    ],
  );
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

  const projectPath = "conformance/projects/basic/slim.project";
  assert.match(learnSource, new RegExp(projectPath.replaceAll(".", "\\.")));
  await access(path.join(repositoryRoot, projectPath));
  const projectCheck = spawnSync("./slimc", ["check", projectPath], {
    cwd: repositoryRoot,
    encoding: "utf8",
  });
  assert.equal(
    projectCheck.status,
    0,
    `${projectPath} failed to check:\n${projectCheck.stdout}${projectCheck.stderr}`,
  );
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
  const statusMilestone = status.match(/^Status:\s*(.+)$/m)?.[1].trim();
  const statusVersion = status.match(/^Compiler version:\s*(.+)$/m)?.[1].trim();
  const nextMilestone = status.match(/^Next milestone:\s*(.+)$/m)?.[1].trim();
  const roadmapStatus = roadmap.match(/^Status:\s*(.+)$/m)?.[1].trim();
  const roadmapMilestone = roadmap
    .match(/^Current milestone:\s*(.+)$/m)?.[1]
    .trim();

  assert.equal(generated.meta.milestone, designStatus);
  assert.equal(statusMilestone, designStatus);
  assert.equal(roadmapStatus, designStatus);
  assert.equal(roadmapMilestone, nextMilestone);
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
  const renderedLearnKeys = [
    ...generated.learn.html.matchAll(/data-surface-key="([^"]+)"/g),
  ].map((match) => match[1]);
  assert.deepEqual(
    renderedLearnKeys,
    expected.map((entry) => `${entry.category}:${entry.name}`),
  );
  assert.match(
    generated.learn.html,
    new RegExp(`data-surface-count="${expected.length}"`),
  );

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
  assert.equal(generated.reference.length, 12);
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
  assert.match(llms, /SLIM 1.0 released/);
  assert.match(llms, /\/reference\/surface\.json/);
  assert.doesNotMatch(rootCargo, /website|vinext|react|marked/);
  assert.match(websitePackage, /"marked": "16\.4\.2"/);
});
