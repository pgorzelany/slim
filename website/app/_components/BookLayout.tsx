import Link from "next/link";
import type { BookChapter, ReferenceDocument } from "../_lib/content";
import { RenderedMarkdown } from "./RenderedMarkdown";
import { BookSearch } from "./BookSearch";

type Document = BookChapter | ReferenceDocument;

function ChapterList({
  documents,
  currentRoute,
}: {
  documents: Document[];
  currentRoute: string;
}) {
  return (
    <ol className="book-nav__list">
      {documents.map((document) => (
        <li key={document.route}>
          <Link
            href={document.route}
            aria-current={document.route === currentRoute ? "page" : undefined}
          >
            <span>{String(document.order).padStart(2, "0")}</span>
            {document.title}
          </Link>
        </li>
      ))}
    </ol>
  );
}

export function BookLayout({
  document,
  documents,
  sectionTitle,
  indexRoute,
}: {
  document: Document;
  documents: Document[];
  sectionTitle: string;
  indexRoute: string;
}) {
  const position = documents.findIndex((item) => item.route === document.route);
  const previous = position > 0 ? documents[position - 1] : null;
  const next = position + 1 < documents.length ? documents[position + 1] : null;

  return (
    <main className="book-page" id="main">
      <div className="book-page__shell">
        <aside className="book-sidebar" aria-label={`${sectionTitle} chapters`}>
          <Link className="book-sidebar__title" href={indexRoute}>
            {sectionTitle}
          </Link>
          <BookSearch id="sidebar-book-search" />
          <ChapterList documents={documents} currentRoute={document.route} />
        </aside>

        <article className="book-article">
          <nav className="breadcrumbs" aria-label="Breadcrumb">
            <ol>
              <li><Link href="/">SLIM</Link></li>
              <li><Link href={indexRoute}>{sectionTitle}</Link></li>
              <li aria-current="page">{document.title}</li>
            </ol>
          </nav>

          <details className="book-mobile-nav">
            <summary>Browse {sectionTitle}</summary>
            <BookSearch id="mobile-book-search" />
            <ChapterList documents={documents} currentRoute={document.route} />
          </details>

          <header className="book-article__header">
            <p className="eyebrow">
              {sectionTitle} · {String(document.order).padStart(2, "0")}
            </p>
            <h1>{document.title}</h1>
            <p>{document.summary}</p>
          </header>

          <RenderedMarkdown html={document.html} />

          <nav className="chapter-pagination" aria-label="Chapter navigation">
            {previous ? (
              <Link href={previous.route} rel="prev">
                <span>Previous</span>
                <strong>{previous.title}</strong>
              </Link>
            ) : <span />}
            {next ? (
              <Link href={next.route} rel="next">
                <span>Next</span>
                <strong>{next.title}</strong>
              </Link>
            ) : <span />}
          </nav>
        </article>

        <aside className="book-toc" aria-label="On this page">
          <p>On this page</p>
          <ol>
            {document.headings.map((heading) => (
              <li className={`book-toc__level-${heading.level}`} key={heading.id}>
                <a href={`#${heading.id}`}>{heading.title}</a>
              </li>
            ))}
          </ol>
        </aside>
      </div>
    </main>
  );
}
