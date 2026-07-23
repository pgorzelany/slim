import type { Metadata } from "next";
import { PageIntro } from "../_components/SiteShell";
import { RenderedMarkdown } from "../_components/RenderedMarkdown";
import { content } from "../_lib/content";

export const metadata: Metadata = {
  title: "Learn",
  description: "A compact, tested introduction to the implemented SLIM language.",
};

export default function LearnPage() {
  return (
    <main className="document-page shell" id="main">
      <PageIntro
        eyebrow="Learn"
        title={content.learn.title}
        description="One tested route from Hello through ownership, projects, analysis, and guarded parallel execution."
      />
      <RenderedMarkdown html={content.learn.html} />
    </main>
  );
}
