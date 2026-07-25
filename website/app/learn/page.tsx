import type { Metadata } from "next";
import { PageIntro } from "../_components/SiteShell";
import { RenderedMarkdown } from "../_components/RenderedMarkdown";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Learn",
  description: "A complete, tested tour of the accepted SLIM 1.0 language surface.",
};

export default function LearnPage() {
  return (
    <main className="document-page shell" id="main">
      <PageIntro
        eyebrow="Learn"
        title={content.learn.title}
        description="One tested route through the complete accepted language: syntax, types, memory, effects, host services, projects, tools, and parallel execution."
      />
      <RenderedMarkdown html={content.learn.html} />
    </main>
  );
}
