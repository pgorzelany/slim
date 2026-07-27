import type { Metadata } from "next";
import { PageIntro } from "../_components/SiteShell";
import { RfcExplorer } from "../_components/RfcExplorer";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "RFCs",
  description: "Numbered SLIM proposals with ratings, dispositions, implementation states, and rationale.",
};

export default function RfcsPage() {
  return (
    <main className="index-page shell" id="main">
      <PageIntro
        eyebrow="Request for Comments"
        title="SLIM RFCs"
        description={`${content.rfcCounts.accepted} accepted and ${content.rfcCounts.rejected} rejected proposals preserve both the current design basis and alternatives that did not ship.`}
      />
      <RfcExplorer rfcs={content.rfcs.map((rfc) => ({
        id: rfc.id,
        route: rfc.route,
        title: rfc.title,
        summary: rfc.summary,
        status: rfc.status,
        implementation: rfc.implementation,
        kind: rfc.kind,
        audience: rfc.audience,
        score: rfc.score,
      }))} />
    </main>
  );
}
