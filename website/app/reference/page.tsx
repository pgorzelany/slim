import type { Metadata } from "next";
import { LegacyRedirect } from "../_components/LegacyRedirect";

export const metadata: Metadata = {
  title: "Documentation moved",
  robots: { index: false, follow: true },
};

export default function ReferencePage() {
  return <LegacyRedirect target="/handbook/appendix/core" />;
}
