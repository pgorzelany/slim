import type { Metadata } from "next";
import { notFound } from "next/navigation";
import { DocumentLayout } from "../../../_components/DocumentLayout";
import { content } from "../../../_lib/content";

export function generateStaticParams() {
  return content.appendices.map((document) => ({ slug: document.slug }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ slug: string }>;
}): Promise<Metadata> {
  const { slug } = await params;
  const document = content.appendices.find((item) => item.slug === slug);
  return document ? { title: document.title, description: document.summary } : {};
}

export default async function AppendixPage({
  params,
}: {
  params: Promise<{ slug: string }>;
}) {
  const { slug } = await params;
  const document = content.appendices.find((item) => item.slug === slug);
  if (!document) notFound();
  return (
    <DocumentLayout
      document={document}
      eyebrow="Handbook appendix"
      indexRoute="/handbook"
      indexLabel="Handbook"
    />
  );
}
