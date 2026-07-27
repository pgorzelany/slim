import type { Metadata } from "next";
import Link from "next/link";
import { BookSearch } from "../_components/BookSearch";
import { PageIntro } from "../_components/SiteShell";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Learn",
  description: "A chaptered, fixture-backed guide to the experimental SLIM 0.9 programming model.",
};

const legacyAnchors: Record<string, string[]> = {
  "getting-started": ["run-and-format-a-program"],
  "modules-and-source-layout": [],
  "scalar-values-and-checked-operations": ["scalar-values-and-checked-operations"],
  "bindings-evaluation-and-mutation": ["bindings-evaluation-mutation-and-repetition"],
  "functions-entry-and-effects": ["program-shape-functions-and-entry", "effects-and-failure"],
  "control-flow-and-recur": [],
  "structs-enums-and-patterns": ["records-variants-construction-and-projection"],
  "bytes-vectors-arenas-and-ids": ["bytes-vectors-arenas-and-ids"],
  "ownership-moves-and-borrowing": ["ownership-borrowing-and-lifetimes"],
  "traps-failure-and-allocation-exhaustion": [],
  "host-io": ["host-io"],
  "projects-imports-and-interfaces": ["projects-and-qualified-references"],
  "deterministic-parallelism": ["automatic-and-explicit-parallelism"],
  "diagnostics-and-compiler-tools": ["diagnostics-and-compiler-tools"],
};

export default function LearnPage() {
  return (
    <main className="index-page shell" id="main">
      <PageIntro
        eyebrow="The SLIM Guide"
        title={content.learn.title}
        description={content.learn.summary}
      />

      <section className="index-search" aria-labelledby="learn-search-title">
        <div>
          <p className="eyebrow">Current release</p>
          <h2 id="learn-search-title">Find a language concept</h2>
          <p>Search runs locally over the generated current-release book.</p>
        </div>
        <BookSearch />
      </section>

      <ol className="chapter-grid" aria-label="Guide chapters">
        {content.guide.map((chapter) => (
          <li key={chapter.slug}>
            {(legacyAnchors[chapter.slug] ?? []).map((id) => (
              <span className="legacy-anchor" id={id} key={id} aria-hidden="true" />
            ))}
            <Link href={chapter.route}>
              <span>{String(chapter.order).padStart(2, "0")}</span>
              <h2>{chapter.title}</h2>
              <p>{chapter.summary}</p>
            </Link>
          </li>
        ))}
      </ol>

      <section
        className="index-note"
        id="complete-accepted-surface"
        aria-labelledby="learn-canonical-title"
      >
        <p className="eyebrow">Canonical boundary</p>
        <h2 id="learn-canonical-title">Examples and facts stay in the repository</h2>
        <p>
          Every substantial example names a conformance fixture. The reference
          surface is generated from the accepted ledger, and{" "}
          <code>docs/CORE.md</code> remains normative.
        </p>
        <Link href="/reference">Open the language reference</Link>
      </section>
    </main>
  );
}
