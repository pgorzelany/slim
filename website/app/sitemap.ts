import type { MetadataRoute } from "next";
import { content } from "./_lib/content";

export const dynamic = "force-static";

export default function sitemap(): MetadataRoute.Sitemap {
  const configuredSiteUrl =
    process.env.NEXT_PUBLIC_SITE_URL ?? "https://pgorzelany.github.io/slim/";
  const siteUrl = new URL(
    configuredSiteUrl.endsWith("/") ? configuredSiteUrl : `${configuredSiteUrl}/`,
  );
  return content.routes.map((route) => ({
    url: new URL(route === "/" ? "." : `${route.slice(1)}/`, siteUrl).toString(),
    lastModified: content.meta.updated,
  }));
}
