import type { Metadata } from "next";
import { notFound } from "next/navigation";
import { BookLayout } from "../../../_components/BookLayout";
import { content } from "../../../_lib/content";

export function generateStaticParams() {
  return content.languageReference.map((chapter) => ({ slug: chapter.slug }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ slug: string }>;
}): Promise<Metadata> {
  const { slug } = await params;
  const chapter = content.languageReference.find((item) => item.slug === slug);
  if (!chapter) return {};
  return { title: chapter.title, description: chapter.summary };
}

export default async function LanguageReferencePage({
  params,
}: {
  params: Promise<{ slug: string }>;
}) {
  const { slug } = await params;
  const chapter = content.languageReference.find((item) => item.slug === slug);
  if (!chapter) notFound();
  return (
    <BookLayout
      document={chapter}
      documents={content.languageReference}
      sectionTitle="Language Reference"
      indexRoute="/reference"
    />
  );
}
