"use client";

import Link from "next/link";
import { useState } from "react";

type RfcCard = {
  id: string;
  route: string;
  title: string;
  summary: string;
  status: string;
  implementation: string;
  kind: string;
  audience: string;
  score: number;
};

export function RfcExplorer({ rfcs }: { rfcs: RfcCard[] }) {
  const [query, setQuery] = useState("");
  const [status, setStatus] = useState("all");
  const [kind, setKind] = useState("all");
  const kinds = [...new Set(rfcs.map((rfc) => rfc.kind))].sort();
  const terms = query.toLowerCase().split(/\s+/).filter(Boolean);
  const filtered = rfcs.filter((rfc) => {
    if (status !== "all" && rfc.status !== status) return false;
    if (kind !== "all" && rfc.kind !== kind) return false;
    const haystack = `${rfc.id} ${rfc.title} ${rfc.summary} ${rfc.audience}`.toLowerCase();
    return terms.every((term) => haystack.includes(term));
  });

  return (
    <>
      <div className="catalog-controls catalog-controls--three">
        <label>
          Search RFCs
          <input
            type="search"
            value={query}
            onChange={(event) => setQuery(event.target.value)}
            placeholder="RFC-0108, ownership, syntax…"
          />
        </label>
        <label>
          Status
          <select value={status} onChange={(event) => setStatus(event.target.value)}>
            <option value="all">All statuses</option>
            {["accepted", "rejected", "proposed", "withdrawn", "superseded"].map((value) =>
              <option value={value} key={value}>{value}</option>)}
          </select>
        </label>
        <label>
          Kind
          <select value={kind} onChange={(event) => setKind(event.target.value)}>
            <option value="all">All kinds</option>
            {kinds.map((value) => <option value={value} key={value}>{value}</option>)}
          </select>
        </label>
      </div>
      <p className="catalog-count" aria-live="polite">{filtered.length} RFCs</p>
      <div className="rfc-list">
        {filtered.map((rfc) => (
          <Link href={rfc.route} key={rfc.id}>
            <div className="rfc-list__meta">
              <strong>{rfc.id}</strong>
              <span className={`status-badge status-badge--${rfc.status}`}>{rfc.status}</span>
              <span>{rfc.kind}</span>
              <span>score {rfc.score}</span>
            </div>
            <h2>{rfc.title.replace(/^RFC-\d+:\s*/, "")}</h2>
            <p>{rfc.summary}</p>
          </Link>
        ))}
      </div>
    </>
  );
}
