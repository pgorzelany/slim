import type { Metadata } from "next";
import { notFound } from "next/navigation";
import { LegacyRedirect } from "../../../_components/LegacyRedirect";
import { content } from "../../../_lib/content";

export const metadata: Metadata = {
  title: "Documentation moved",
  robots: { index: false, follow: true },
};

export function generateStaticParams() {
  return Object.keys(content.redirects.language).map((slug) => ({ slug }));
}

export default async function LegacyLanguagePage({
  params,
}: {
  params: Promise<{ slug: string }>;
}) {
  const { slug } = await params;
  const target = content.redirects.language[slug as keyof typeof content.redirects.language];
  if (!target) notFound();
  return <LegacyRedirect target={target} />;
}
