"use client";

import Link from "next/link";
import { useMemo, useState } from "react";
import { content, type SearchEntry } from "../_lib/content";

type Result = {
  entry: SearchEntry;
  heading?: { id: string; title: string };
  score: number;
};

function searchEntry(entry: SearchEntry, query: string): Result | null {
  const terms = query.toLowerCase().split(/\s+/).filter(Boolean);
  if (terms.length === 0) return null;
  const title = entry.title.toLowerCase();
  const summary = entry.summary.toLowerCase();
  const text = entry.text.toLowerCase();
  let score = 0;
  let bestHeading: Result["heading"];

  for (const term of terms) {
    let termScore = 0;
    if (title === term) termScore = 120;
    else if (title.includes(term)) termScore = 60;
    if (summary.includes(term)) termScore += 24;
    const heading = entry.headings.find((item) =>
      item.title.toLowerCase().includes(term),
    );
    if (heading) {
      termScore += 40;
      bestHeading ??= heading;
    }
    if (text.includes(term)) termScore += 4;
    if (termScore === 0) return null;
    score += termScore;
  }
  return { entry, heading: bestHeading, score };
}

export function BookSearch({ id = "book-search" }: { id?: string }) {
  const [query, setQuery] = useState("");
  const results = useMemo(
    () =>
      content.search
        .map((entry) => searchEntry(entry, query))
        .filter((result): result is Result => result !== null)
        .sort((left, right) =>
          right.score - left.score ||
          left.entry.title.localeCompare(right.entry.title),
        )
        .slice(0, 12),
    [query],
  );
  const resultLabel =
    query.trim() === ""
      ? "Type to search the current SLIM book."
      : `${results.length} result${results.length === 1 ? "" : "s"}.`;

  return (
    <div className="book-search">
      <label htmlFor={`${id}-input`}>Search the book</label>
      <input
        id={`${id}-input`}
        type="search"
        value={query}
        onChange={(event) => setQuery(event.target.value)}
        placeholder="Effects, borrowing, diagnostics…"
        autoComplete="off"
      />
      <p className="book-search__status" aria-live="polite">
        {resultLabel}
      </p>
      {query.trim() !== "" && (
        <ol className="book-search__results">
          {results.map(({ entry, heading }) => (
            <li key={`${entry.route}#${heading?.id ?? ""}`}>
              <Link href={`${entry.route}${heading ? `#${heading.id}` : ""}`}>
                <strong>{heading?.title ?? entry.title}</strong>
                <span>{heading ? entry.title : entry.summary}</span>
              </Link>
            </li>
          ))}
        </ol>
      )}
    </div>
  );
}
