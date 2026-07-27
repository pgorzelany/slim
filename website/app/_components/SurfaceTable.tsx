import Link from "next/link";
import type { SurfaceEntry } from "../_lib/content";

export function SurfaceTable({ entries }: { entries: SurfaceEntry[] }) {
  const categories = [...new Set(entries.map((entry) => entry.category))].sort();

  return (
    <section className="surface-section" aria-labelledby="accepted-surface">
      <header className="section-heading">
        <p className="eyebrow">Canonical inventory</p>
        <h2 id="accepted-surface">Complete accepted surface</h2>
        <p>
          Every active spelling is owned by one accepted, implemented RFC.
          This table is generated from <code>design/surface.tsv</code>.
        </p>
        <Link className="machine-link" href="/reference/surface.json">
          Download the machine-readable surface
        </Link>
      </header>
      {categories.map((category) => (
        <section className="surface-group" key={category}>
          <h3>{category}</h3>
          <div className="table-scroll">
            <table>
              <thead>
                <tr><th>Spelling</th><th>Semantic role</th><th>Owner</th></tr>
              </thead>
              <tbody>
                {entries
                  .filter((entry) => entry.category === category)
                  .map((entry) => (
                    <tr key={`${entry.category}:${entry.name}`}>
                      <td><code>{entry.name}</code></td>
                      <td>{entry.semanticRole}</td>
                      <td><Link href={entry.rfcRoute}>{entry.rfc}</Link></td>
                    </tr>
                  ))}
              </tbody>
            </table>
          </div>
        </section>
      ))}
    </section>
  );
}
