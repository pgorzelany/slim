import type { Metadata } from "next";
import { notFound } from "next/navigation";
import { BookLayout } from "../../../_components/BookLayout";
import { content } from "../../../_lib/content";

export function generateStaticParams() {
  return content.reference.map((document) => ({ id: document.id }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ id: string }>;
}): Promise<Metadata> {
  const { id } = await params;
  const document = content.reference.find((item) => item.id === id);
  if (!document) return {};
  return { title: document.title, description: document.summary };
}

export default async function ContractPage({
  params,
}: {
  params: Promise<{ id: string }>;
}) {
  const { id } = await params;
  const document = content.reference.find((item) => item.id === id);
  if (!document) notFound();
  return (
    <BookLayout
      document={document}
      documents={content.reference}
      sectionTitle="Canonical Contracts"
      indexRoute="/reference"
    />
  );
}
