import type { Metadata } from "next";
import { notFound } from "next/navigation";
import { DocumentLayout } from "../../../_components/DocumentLayout";
import { content } from "../../../_lib/content";

const documents = [
  ...content.development.overview,
  ...content.development.contracts,
  ...content.development.subsystems,
  ...content.development.evidence,
];

export function generateStaticParams() {
  return documents.map((document) => ({
    group: document.route.split("/")[2],
    slug: document.slug,
  }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ group: string; slug: string }>;
}): Promise<Metadata> {
  const { group, slug } = await params;
  const document = documents.find((item) => item.route === `/development/${group}/${slug}`);
  return document ? { title: document.title, description: document.summary } : {};
}

export default async function DevelopmentDocumentPage({
  params,
}: {
  params: Promise<{ group: string; slug: string }>;
}) {
  const { group, slug } = await params;
  const document = documents.find((item) => item.route === `/development/${group}/${slug}`);
  if (!document) notFound();
  const historical = group === "evidence";
  return (
    <DocumentLayout
      document={document}
      eyebrow={historical ? "Dated evidence" : "Development"}
      indexRoute="/development"
      indexLabel="Development"
      notice={historical ? "Historical evidence: this report records a dated measurement and is not the current language contract." : undefined}
    />
  );
}
