import type { Metadata } from "next";
import Link from "next/link";
import { notFound } from "next/navigation";
import { RenderedMarkdown } from "../../../_components/RenderedMarkdown";
import { content } from "../../../_lib/content";

export function generateStaticParams() {
  return content.algorithms.map((algorithm) => ({ slug: algorithm.slug }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ slug: string }>;
}): Promise<Metadata> {
  const { slug } = await params;
  const algorithm = content.algorithms.find((item) => item.slug === slug);
  return algorithm ? { title: algorithm.title, description: algorithm.summary } : {};
}

export default async function AlgorithmPage({
  params,
}: {
  params: Promise<{ slug: string }>;
}) {
  const { slug } = await params;
  const algorithm = content.algorithms.find((item) => item.slug === slug);
  if (!algorithm) notFound();
  const chapterCandidates = [
    algorithm.features.some((feature) => ["vector", "arena", "bytes", "typed-id"].includes(feature))
      ? ["/handbook/storage-and-allocation", "Storage and allocation"]
      : null,
    algorithm.features.includes("mutation")
      ? ["/handbook/bindings-evaluation-and-mutation", "Bindings and mutation"]
      : null,
    algorithm.features.some((feature) => ["recursion", "backtracking", "search"].includes(feature))
      ? ["/handbook/functions-effects-and-control-flow", "Functions and recurrence"]
      : null,
    algorithm.features.some((feature) => ["record", "variant"].includes(feature))
      ? ["/handbook/structs-enums-and-patterns", "Structs and enums"]
      : null,
    algorithm.features.includes("checked-arithmetic")
      ? ["/handbook/values-types-and-operators", "Values and checked operators"]
      : null,
    algorithm.features.includes("parallel-candidate")
      ? ["/handbook/deterministic-parallelism", "Deterministic parallelism"]
      : null,
  ].filter((item): item is string[] => item !== null);
  return (
    <main className="document-page shell" id="main">
      <nav className="breadcrumbs" aria-label="Breadcrumb">
        <ol><li><Link href="/handbook">Handbook</Link></li><li><Link href="/handbook/examples">Examples</Link></li><li>{algorithm.title}</li></ol>
      </nav>
      <header className="page-intro">
        <p className="eyebrow">{algorithm.category} · {algorithm.shape}-shaped</p>
        <h1>{algorithm.title}</h1>
        <p className="page-intro__description">{algorithm.summary}</p>
      </header>
      <dl className="algorithm-facts">
        <div><dt>Time</dt><dd>{algorithm.time}</dd></div>
        <div><dt>Space</dt><dd>{algorithm.space}</dd></div>
        <div><dt>Concepts</dt><dd>{algorithm.features.join(", ")}</dd></div>
      </dl>
      {algorithm.walkthrough && <RenderedMarkdown html={algorithm.walkthrough.html} />}
      {!algorithm.walkthrough && (
        <p className="document-notice">This gallery entry is concise. Its source remains a fully verified comparative workload.</p>
      )}
      <section className="algorithm-links" aria-labelledby="learn-and-verify">
        <h2 id="learn-and-verify">Learn and verify</h2>
        <ul>
          {chapterCandidates.map(([route, label]) => (
            <li key={route}><Link href={route}>{label}</Link></li>
          ))}
          <li>
            <Link href="/development/evidence/2026-07-24-safe-c-parity-typed-set">
              Comparative benchmark evidence
            </Link>
          </li>
        </ul>
      </section>
      <figure className="code-example algorithm-source" id="canonical-source">
        <figcaption><span>{algorithm.sourcePath}</span><span>canonical tested source</span></figcaption>
        <pre><code className="language-slim">{algorithm.source}</code></pre>
      </figure>
    </main>
  );
}
