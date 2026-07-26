import type { MetadataRoute } from "next";

export const dynamic = "force-static";

export default function robots(): MetadataRoute.Robots {
  const configuredSiteUrl =
    process.env.NEXT_PUBLIC_SITE_URL ?? "https://pgorzelany.github.io/slim/";
  const siteUrl = new URL(
    configuredSiteUrl.endsWith("/") ? configuredSiteUrl : `${configuredSiteUrl}/`,
  );
  return {
    rules: {
      userAgent: "*",
      allow: "/",
    },
    sitemap: new URL("sitemap.xml", siteUrl).toString(),
  };
}
