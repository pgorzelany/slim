import type { Metadata } from "next";
import { notFound } from "next/navigation";
import { DocumentLayout } from "../../_components/DocumentLayout";
import { content } from "../../_lib/content";

export function generateStaticParams() {
  return content.rfcs.map((rfc) => ({ slug: rfc.slug }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ slug: string }>;
}): Promise<Metadata> {
  const { slug } = await params;
  const rfc = content.rfcs.find((item) => item.slug === slug);
  return rfc ? { title: rfc.title, description: rfc.summary } : {};
}

export default async function RfcPage({
  params,
}: {
  params: Promise<{ slug: string }>;
}) {
  const { slug } = await params;
  const rfc = content.rfcs.find((item) => item.slug === slug);
  if (!rfc) notFound();
  return (
    <DocumentLayout
      document={rfc}
      eyebrow={`${rfc.status} · ${rfc.kind} · score ${rfc.score}`}
      indexRoute="/rfcs"
      indexLabel="RFCs"
      notice={`RFC record: ${rfc.status}, implementation ${rfc.implementation}. Current behavior is owned by the Handbook and maintained contracts.`}
      metadata={(
        <dl className="rfc-ratings" aria-label="RFC weighted ratings">
          {Object.entries(rfc.ratings).map(([name, rating]) => (
            <div key={name}><dt>{name}</dt><dd>{rating > 0 ? `+${rating}` : rating}</dd></div>
          ))}
          <div><dt>weighted score</dt><dd>{rfc.score}</dd></div>
        </dl>
      )}
    />
  );
}
