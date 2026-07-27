import type { Metadata } from "next";
import { AlgorithmExplorer } from "../../_components/AlgorithmExplorer";
import { PageIntro } from "../../_components/SiteShell";
import { content } from "../../_lib/content";

export const metadata: Metadata = {
  title: "Algorithm gallery",
  description: "Twenty well-known algorithms and workloads written in tested canonical SLIM.",
};

export default function AlgorithmGalleryPage() {
  return (
    <main className="index-page shell" id="main">
      <PageIntro
        eyebrow="Handbook examples"
        title="Algorithm gallery"
        description="Twenty canonical benchmark programs show how search, sorting, graphs, dynamic programming, backtracking, grids, and state machines are written in SLIM."
      />
      <AlgorithmExplorer algorithms={content.algorithms.map((algorithm) => ({
        slug: algorithm.slug,
        route: algorithm.route,
        title: algorithm.title,
        category: algorithm.category,
        summary: algorithm.summary,
        time: algorithm.time,
        space: algorithm.space,
        features: algorithm.features,
        featured: algorithm.featured,
      }))} />
    </main>
  );
}
