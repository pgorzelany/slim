import Link from "next/link";

const navigation = [
  { href: "/learn", label: "Learn" },
  { href: "/reference", label: "Reference" },
  { href: "/status", label: "Status" },
] as const;

export function SiteHeader() {
  return (
    <header className="site-header">
      <div className="site-header__inner">
        <Link className="wordmark" href="/" aria-label="SLIM home">
          <span aria-hidden="true">(</span>
          SLIM
          <span aria-hidden="true">)</span>
        </Link>
        <nav aria-label="Primary navigation">
          {navigation.map((item) => (
            <Link href={item.href} key={item.href}>
              {item.label}
            </Link>
          ))}
        </nav>
      </div>
    </header>
  );
}

export function SiteFooter({
  milestone,
  version,
}: {
  milestone: string;
  version: string;
}) {
  return (
    <footer className="site-footer">
      <div className="site-footer__inner">
        <p>SLIM — Small Language for Intelligent Machines</p>
        <p>
          Compiler {version} · {milestone}
        </p>
      </div>
    </footer>
  );
}

export function PageIntro({
  eyebrow,
  title,
  description,
}: {
  eyebrow: string;
  title: string;
  description: string;
}) {
  return (
    <header className="page-intro">
      <p className="eyebrow">{eyebrow}</p>
      <h1>{title}</h1>
      <p className="page-intro__description">{description}</p>
    </header>
  );
}
