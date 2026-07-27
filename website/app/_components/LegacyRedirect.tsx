import Link from "next/link";
import { content } from "../_lib/content";

export function LegacyRedirect({ target }: { target: string }) {
  const href = `${content.meta.basePath}${target}/`;
  return (
    <main className="document-page shell" id="main">
      <meta httpEquiv="refresh" content={`0; url=${href}`} />
      <p className="eyebrow">Documentation moved</p>
      <h1>This page has a new canonical location.</h1>
      <p><Link href={target}>Continue to the current documentation</Link>.</p>
    </main>
  );
}
