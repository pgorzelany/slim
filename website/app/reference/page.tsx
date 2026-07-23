import type { Metadata } from "next";
import { PageIntro } from "../_components/SiteShell";
import { RenderedMarkdown } from "../_components/RenderedMarkdown";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Reference",
  description: "Canonical SLIM language, memory, project, analysis, and execution references.",
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
    <main className="reference-page shell" id="main">
      <PageIntro
        eyebrow="Reference"
        title="One canonical surface"
        description="Generated from the accepted language ledger and the repository's maintained contracts."
      />

      <nav className="reference-index" aria-label="Reference documents">
        {content.reference.map((document) => (
          <a href={`#reference-${document.id}`} key={document.id}>
            <strong>{document.title}</strong>
            <span>{document.summary}</span>
          </a>
        ))}
      </nav>

      <section className="surface-section" aria-labelledby="surface-title">
        <div className="section-heading">
          <p className="eyebrow">Generated ledger</p>
          <h2 id="surface-title">Accepted language surface</h2>
          <p>
            This table is generated directly from <code>design/surface.tsv</code>.
          </p>
        </div>
        {categories.map((category) => (
          <div className="surface-group" key={category}>
            <h3>{categoryLabels[category] ?? category}</h3>
            <div className="table-scroll" role="region" aria-label={`${categoryLabels[category] ?? category} surface`}>
              <table>
                <thead>
                  <tr>
                    <th scope="col">Name</th>
                    <th scope="col">Semantic role</th>
                    <th scope="col">Decision</th>
                  </tr>
                </thead>
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
          Machine-readable: <a href="/reference/surface.json">surface.json</a>
        </p>
      </section>

      <div className="reference-documents">
        {content.reference.map((document) => (
          <article className="reference-document" id={`reference-${document.id}`} key={document.id}>
            <header>
              <p className="eyebrow">{document.path}</p>
              <h2>{document.title}</h2>
              <p>{document.summary}</p>
            </header>
            <RenderedMarkdown html={document.html} />
          </article>
        ))}
      </div>
    </main>
  );
}
