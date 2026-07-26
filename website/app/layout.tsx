import type { Metadata } from "next";
import { SiteFooter, SiteHeader } from "./_components/SiteShell";
import { content } from "./_lib/content";
import "./globals.css";

const configuredSiteUrl =
  process.env.NEXT_PUBLIC_SITE_URL ?? "https://pgorzelany.github.io/slim/";
const metadataBase = new URL(
  configuredSiteUrl.endsWith("/") ? configuredSiteUrl : `${configuredSiteUrl}/`,
);
const socialImageUrl = new URL("og-indented.png", metadataBase).toString();

export const metadata: Metadata = {
  metadataBase,
  title: {
    default: "SLIM — Small Language for Intelligent Machines",
    template: "%s · SLIM",
  },
  description:
    "A small systems language designed for AI-generated programs.",
  robots: {
    index: true,
    follow: true,
  },
  icons: {
    icon: socialImageUrl,
  },
  openGraph: {
    type: "website",
    title: "SLIM — Small Language for Intelligent Machines",
    description:
      "Canonical syntax, static safety, deterministic memory, bounded analysis.",
    siteName: "SLIM",
    images: [
      {
        url: socialImageUrl,
        width: 1536,
        height: 1024,
        alt: "SLIM — Small Language for Intelligent Machines",
      },
    ],
  },
  twitter: {
    card: "summary_large_image",
    title: "SLIM — Small Language for Intelligent Machines",
    description:
      "Canonical syntax, static safety, deterministic memory, bounded analysis.",
    images: [socialImageUrl],
  },
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body>
        <a className="skip-link" href="#main">
          Skip to content
        </a>
        <SiteHeader />
        {children}
        <SiteFooter
          milestone={content.meta.milestone}
          version={content.meta.compilerVersion}
        />
      </body>
    </html>
  );
}
