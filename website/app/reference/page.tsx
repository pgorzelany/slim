import type { Metadata } from "next";
import Link from "next/link";
import { PageIntro } from "../_components/SiteShell";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Reference",
  description: "The precise SLIM language reference, canonical contracts, and accepted surface.",
};

const categoryLabels: Record<string, string> = {
  syntax: "Syntax",
  type: "Types",
  effect: "Effects",
  entry: "Entry point",
  builtin: "Built-ins",
  backend: "Backend",
};

export default function ReferencePage() {
  const categories = [...new Set(content.surface.entries.map((entry) => entry.category))];

  return (
    <main className="index-page shell" id="main">
      <PageIntro
        eyebrow="Reference"
        title="One canonical language surface"
        description="Use the language chapters for precise lookup and the maintained contracts for normative boundaries beyond Core."
      />

      <section className="index-section" aria-labelledby="language-reference-title">
        <header>
          <p className="eyebrow">Language reference</p>
          <h2 id="language-reference-title">Grammar, types, semantics, and tools</h2>
        </header>
        <ol className="chapter-grid chapter-grid--compact">
          {content.languageReference.map((chapter) => (
            <li key={chapter.slug}>
              <Link href={chapter.route}>
                <span>{String(chapter.order).padStart(2, "0")}</span>
                <h3>{chapter.title}</h3>
                <p>{chapter.summary}</p>
              </Link>
            </li>
          ))}
        </ol>
      </section>

      <section className="index-section" aria-labelledby="contracts-title">
        <header>
          <p className="eyebrow">Canonical contracts</p>
          <h2 id="contracts-title">Maintained repository specifications</h2>
        </header>
        <div className="reference-index">
          {content.reference.map((document) => (
            <Link
              href={document.route}
              id={`reference-${document.id}`}
              key={document.id}
            >
              <strong>{document.title}</strong>
              <span>{document.summary}</span>
            </Link>
          ))}
        </div>
      </section>

      <section className="surface-section" aria-labelledby="surface-title">
        <div className="section-heading">
          <p className="eyebrow">Generated ledger</p>
          <h2 id="surface-title">Accepted language surface</h2>
          <p>This table is generated directly from <code>design/surface.tsv</code>.</p>
        </div>
        {categories.map((category) => (
          <div className="surface-group" key={category}>
            <h3>{categoryLabels[category] ?? category}</h3>
            <div className="table-scroll" role="region" aria-label={`${categoryLabels[category] ?? category} surface`}>
              <table>
                <thead><tr><th scope="col">Name</th><th scope="col">Semantic role</th><th scope="col">Decision</th></tr></thead>
                <tbody>
                  {content.surface.entries
                    .filter((entry) => entry.category === category)
                    .map((entry) => (
                      <tr key={`${entry.category}-${entry.name}`}>
                        <td><code>{entry.name}</code></td>
                        <td>{entry.semanticRole}</td>
                        <td><code>{entry.decision}</code></td>
                      </tr>
                    ))}
                </tbody>
              </table>
            </div>
          </div>
        ))}
        <p className="machine-link">
          Machine-readable: <Link href="/reference/surface.json">surface.json</Link>
        </p>
      </section>
    </main>
  );
}
