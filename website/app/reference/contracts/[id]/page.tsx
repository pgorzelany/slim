import type { Metadata } from "next";
import { notFound } from "next/navigation";
import { LegacyRedirect } from "../../../_components/LegacyRedirect";
import { content } from "../../../_lib/content";

export const metadata: Metadata = {
  title: "Documentation moved",
  robots: { index: false, follow: true },
};

export function generateStaticParams() {
  return Object.keys(content.redirects.contracts).map((id) => ({ id }));
}

export default async function LegacyContractPage({
  params,
}: {
  params: Promise<{ id: string }>;
}) {
  const { id } = await params;
  const target = content.redirects.contracts[id as keyof typeof content.redirects.contracts];
  if (!target) notFound();
  return <LegacyRedirect target={target} />;
}
