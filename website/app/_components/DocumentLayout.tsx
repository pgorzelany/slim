import Link from "next/link";
import type { ReactNode } from "react";
import type { Document } from "../_lib/content";
import { RenderedMarkdown } from "./RenderedMarkdown";

export function DocumentLayout({
  document,
  eyebrow,
  indexRoute,
  indexLabel,
  notice,
  metadata,
}: {
  document: Document;
  eyebrow: string;
  indexRoute: string;
  indexLabel: string;
  notice?: string;
  metadata?: ReactNode;
}) {
  return (
    <main className="book-page" id="main">
      <div className="book-page__shell book-page__shell--document">
        <article className="book-article">
          <nav className="breadcrumbs" aria-label="Breadcrumb">
            <ol>
              <li><Link href="/">SLIM</Link></li>
              <li><Link href={indexRoute}>{indexLabel}</Link></li>
              <li aria-current="page">{document.title}</li>
            </ol>
          </nav>

          <header className="book-article__header">
            <p className="eyebrow">{eyebrow}</p>
            <h1>{document.title}</h1>
            <p>{document.summary}</p>
          </header>

          {notice && <p className="document-notice">{notice}</p>}
          {metadata}
          <RenderedMarkdown html={document.html} />
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
