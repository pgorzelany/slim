import type { Metadata } from "next";
import Link from "next/link";
import { content } from "./_lib/content";

export const metadata: Metadata = {
  title: {
    absolute: "SLIM — Small Language for Intelligent Machines",
  },
  description:
    "A small systems language designed for AI-generated programs.",
};

export default function Home() {
  return (
    <main id="main">
      <section className="hero shell">
        <div className="hero__copy">
          <p className="eyebrow">
            {content.meta.milestone} · compiler {content.meta.compilerVersion}
          </p>
          <h1>
            Small language.
            <br />
            Explicit guarantees.
          </h1>
          <p className="hero__lede">
            <strong>SLIM</strong> is the Small Language for Intelligent
            Machines: canonical syntax, strong static types, explicit effects,
            deterministic memory, and bounded evidence.
          </p>
          <div className="hero__actions">
            <Link className="button button--primary" href="/handbook">
              Read the Handbook
            </Link>
            <Link className="button" href="/status">
              Current status
            </Link>
          </div>
        </div>
        <figure className="hero-code">
          <figcaption>
            <span>{content.hello.sourcePath}</span>
            <span>{content.hello.output}</span>
          </figcaption>
          <pre>
            <code>{content.hello.source}</code>
          </pre>
        </figure>
      </section>

      <section className="section shell" aria-labelledby="current-boundary">
        <div className="section-heading">
          <p className="eyebrow">Current boundary</p>
          <h2 id="current-boundary">{content.meta.milestone}</h2>
        </div>
        <div className="route-grid">
          <Link className="route-card" href="/handbook">
            <span>01</span>
            <h3>Handbook</h3>
            <p>One progressive, fixture-backed path through the programming model.</p>
          </Link>
          <Link className="route-card" href="/development">
            <span>02</span>
            <h3>Development</h3>
            <p>Architecture, contracts, compiler subsystems, and dated evidence.</p>
          </Link>
          <Link className="route-card" href="/rfcs">
            <span>03</span>
            <h3>RFCs</h3>
            <p>Accepted, rejected, and proposed changes with ratings and rationale.</p>
          </Link>
          <Link className="route-card" href="/status">
            <span>04</span>
            <h3>Status</h3>
            <p>What works, what is bounded, and what does not exist yet.</p>
          </Link>
        </div>
      </section>
    </main>
  );
}
