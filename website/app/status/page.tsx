import type { Metadata } from "next";
import { PageIntro } from "../_components/SiteShell";
import { RenderedMarkdown } from "../_components/RenderedMarkdown";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Status",
  description: "The exact implemented, bounded, and missing SLIM capabilities.",
};

export default function StatusPage() {
  return (
    <main className="document-page shell" id="main">
      <PageIntro
        eyebrow={`Compiler ${content.meta.compilerVersion}`}
        title={content.status.title}
        description={`${content.meta.milestone}. Updated ${content.meta.updated}.`}
      />
      <RenderedMarkdown html={content.status.html} />
    </main>
  );
}
