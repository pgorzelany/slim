import type { Metadata } from "next";
import Link from "next/link";
import { BookSearch } from "../../_components/BookSearch";
import { PageIntro } from "../../_components/SiteShell";
import { content } from "../../_lib/content";

export const metadata: Metadata = {
  title: "Benchmark evidence",
  description: "Dated SLIM compiler and runtime measurements preserved as historical evidence.",
};

export default function EvidencePage() {
  return (
    <main className="index-page shell" id="main">
      <PageIntro
        eyebrow="Historical measurements"
        title="Benchmark evidence"
        description="These dated reports preserve the machine, commands, measurements, and conclusions behind performance and acceptance gates. Current promises live in maintained contracts."
      />
      <section className="index-search">
        <div><p className="eyebrow">Evidence search</p><h2>Find a measurement</h2></div>
        <BookSearch scope="evidence" />
      </section>
      <div className="rfc-list">
        {[...content.development.evidence].reverse().map((document) => (
          <Link href={document.route} key={document.route}>
            <div className="rfc-list__meta">
              <strong>{document.slug.slice(0, 10)}</strong>
              <span className="status-badge">historical</span>
            </div>
            <h2>{document.title}</h2>
            <p>{document.summary}</p>
          </Link>
        ))}
      </div>
    </main>
  );
}
