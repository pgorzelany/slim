import type { Metadata } from "next";
import Link from "next/link";
import { BookSearch } from "../_components/BookSearch";
import { PageIntro } from "../_components/SiteShell";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Development",
  description: "SLIM architecture, compiler contracts, subsystem documentation, and dated evidence.",
};

const groups = [
  ["Project", content.development.overview],
  ["Current contracts", content.development.contracts],
  ["Subsystems", content.development.subsystems],
] as const;

export default function DevelopmentPage() {
  return (
    <main className="index-page shell" id="main">
      <PageIntro
        eyebrow="Compiler development"
        title="Architecture, contracts, and evidence"
        description="Current implementation documents link to normative language rules; dated measurements remain available without appearing as required Handbook reading."
      />
      <section className="index-search">
        <div><p className="eyebrow">Developer search</p><h2>Find a subsystem or contract</h2></div>
        <BookSearch scope="development" />
      </section>
      {groups.map(([title, documents]) => (
        <section className="index-section" key={title}>
          <header><p className="eyebrow">Development</p><h2>{title}</h2></header>
          <div className="reference-index">
            {documents.map((document) => (
              <Link href={document.route} key={document.route}>
                <strong>{document.title}</strong><span>{document.summary}</span>
              </Link>
            ))}
          </div>
        </section>
      ))}
      <section className="index-section">
        <header><p className="eyebrow">Historical measurements</p><h2>Dated evidence</h2></header>
        <p>{content.development.evidence.length} reports preserve the host, commands, measurements, and rationale behind accepted gates.</p>
        <Link className="button" href="/development/evidence">Browse all evidence</Link>
      </section>
      <section className="index-section">
        <header><p className="eyebrow">Documentation budget</p><h2>Current prose stays bounded</h2></header>
        <dl className="documentation-stats">
          <div><dt>Sequential Handbook</dt><dd>{content.stats.sequentialHandbookWords.toLocaleString()}</dd></div>
          <div><dt>Featured walkthroughs</dt><dd>{content.stats.algorithmWalkthroughWords.toLocaleString()}</dd></div>
          <div><dt>Maintained current docs</dt><dd>{content.stats.maintainedCurrentWords.toLocaleString()}</dd></div>
          <div><dt>RFC archive</dt><dd>{content.stats.rfcArchiveWords.toLocaleString()}</dd></div>
          <div><dt>Evidence archive</dt><dd>{content.stats.evidenceArchiveWords.toLocaleString()}</dd></div>
        </dl>
        <p>
          {content.stats.publishedMarkdown} Markdown sources are published and{" "}
          {content.stats.excludedMarkdown} internal source is explicitly excluded.
        </p>
      </section>
    </main>
  );
}
