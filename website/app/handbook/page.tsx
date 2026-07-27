import type { Metadata } from "next";
import Link from "next/link";
import { BookSearch } from "../_components/BookSearch";
import { PageIntro } from "../_components/SiteShell";
import { SurfaceTable } from "../_components/SurfaceTable";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Handbook",
  description: "One progressive, fixture-backed path through the complete SLIM 0.9 programming model.",
};

const levels = [
  ["Foundations", content.handbook.slice(0, 6)],
  ["Systems programming", content.handbook.slice(6, 10)],
  ["Advanced use", content.handbook.slice(10, 12)],
] as const;

export default function HandbookPage() {
  return (
    <main className="index-page shell" id="main">
      <PageIntro
        eyebrow="The SLIM Handbook"
        title={content.handbookIndex.title}
        description={content.handbookIndex.summary}
      />
      <section className="index-search" aria-labelledby="handbook-search">
        <div>
          <p className="eyebrow">Progressive disclosure</p>
          <h2 id="handbook-search">Learn in order or look up a concept</h2>
          <p>{content.stats.sequentialHandbookWords.toLocaleString()} sequential prose words, backed by executable fixtures.</p>
        </div>
        <BookSearch scope="current" />
      </section>
      {levels.map(([title, chapters]) => (
        <section className="index-section" key={title}>
          <header><p className="eyebrow">Handbook</p><h2>{title}</h2></header>
          <ol className="chapter-grid">
            {chapters.map((chapter) => (
              <li key={chapter.route}>
                <Link href={chapter.route}>
                  <span>{String(chapter.order).padStart(2, "0")}</span>
                  <h3>{chapter.title}</h3>
                  <p>{chapter.summary}</p>
                </Link>
              </li>
            ))}
          </ol>
        </section>
      ))}
      <section className="index-section">
        <header><p className="eyebrow">Examples</p><h2>Algorithms written in SLIM</h2></header>
        <p>Twenty tested comparative programs, including six detailed walkthroughs.</p>
        <Link className="button" href="/handbook/examples">Open the algorithm gallery</Link>
      </section>
      <section className="index-section">
        <header><p className="eyebrow">Appendices</p><h2>Exact rules and lookup material</h2></header>
        <div className="reference-index">
          {content.appendices.map((document) => (
            <Link href={document.route} key={document.route}>
              <strong>{document.label}</strong><span>{document.summary}</span>
            </Link>
          ))}
        </div>
      </section>
      <SurfaceTable entries={content.surface.entries} />
    </main>
  );
}
