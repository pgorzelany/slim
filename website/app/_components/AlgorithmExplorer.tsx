"use client";

import Link from "next/link";
import { useState } from "react";

type AlgorithmCard = {
  slug: string;
  route: string;
  title: string;
  category: string;
  summary: string;
  time: string;
  space: string;
  features: string[];
  featured: boolean;
};

export function AlgorithmExplorer({ algorithms }: { algorithms: AlgorithmCard[] }) {
  const [query, setQuery] = useState("");
  const [category, setCategory] = useState("all");
  const categories = [...new Set(algorithms.map((algorithm) => algorithm.category))].sort();
  const terms = query.toLowerCase().split(/\s+/).filter(Boolean);
  const filtered = algorithms.filter((algorithm) => {
    if (category !== "all" && algorithm.category !== category) return false;
    const haystack = [
      algorithm.title,
      algorithm.summary,
      algorithm.category,
      algorithm.time,
      algorithm.space,
      ...algorithm.features,
    ].join(" ").toLowerCase();
    return terms.every((term) => haystack.includes(term));
  });

  return (
    <>
      <div className="catalog-controls">
        <label>
          Search algorithms
          <input
            type="search"
            value={query}
            onChange={(event) => setQuery(event.target.value)}
            placeholder="Graph, dynamic programming, O(n)…"
          />
        </label>
        <label>
          Category
          <select value={category} onChange={(event) => setCategory(event.target.value)}>
            <option value="all">All categories</option>
            {categories.map((value) => <option value={value} key={value}>{value}</option>)}
          </select>
        </label>
      </div>
      <p className="catalog-count" aria-live="polite">{filtered.length} algorithms</p>
      <div className="catalog-grid">
        {filtered.map((algorithm) => (
          <Link className="catalog-card" href={algorithm.route} key={algorithm.slug}>
            <div>
              <span className="status-badge">{algorithm.category}</span>
              {algorithm.featured && <span className="status-badge status-badge--accent">Walkthrough</span>}
            </div>
            <h2>{algorithm.title}</h2>
            <p>{algorithm.summary}</p>
            <dl>
              <div><dt>Time</dt><dd>{algorithm.time}</dd></div>
              <div><dt>Space</dt><dd>{algorithm.space}</dd></div>
            </dl>
          </Link>
        ))}
      </div>
    </>
  );
}
