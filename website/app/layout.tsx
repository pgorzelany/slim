import type { Metadata } from "next";
import { Geist, Geist_Mono } from "next/font/google";
import { headers } from "next/headers";
import { SiteFooter, SiteHeader } from "./_components/SiteShell";
import { content } from "./_lib/content";
import "./globals.css";

const geistSans = Geist({
  variable: "--font-geist-sans",
  subsets: ["latin"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

export async function generateMetadata(): Promise<Metadata> {
  const requestHeaders = await headers();
  const host = requestHeaders.get("host") ?? "localhost:3000";
  const protocol =
    requestHeaders.get("x-forwarded-proto") ??
    (host.startsWith("localhost") ? "http" : "https");
  const metadataBase = new URL(`${protocol}://${host}`);

  return {
    metadataBase,
    title: {
      default: "SLIM — Small Language for Intelligent Machines",
      template: "%s · SLIM",
    },
    description:
      "A small systems language designed for AI-generated programs.",
    robots: {
      index: false,
      follow: false,
    },
    icons: {
      icon: new URL("/og-indented.png", metadataBase).toString(),
    },
    openGraph: {
      type: "website",
      title: "SLIM — Small Language for Intelligent Machines",
      description:
        "Canonical syntax, static safety, deterministic memory, bounded analysis.",
      siteName: "SLIM",
      images: [
        {
          url: new URL("/og-indented.png", metadataBase).toString(),
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
      images: [new URL("/og-indented.png", metadataBase).toString()],
    },
  };
}

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body
        className={`${geistSans.variable} ${geistMono.variable} antialiased`}
      >
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
