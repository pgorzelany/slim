import contentData from "../../generated/content.json";

export type Heading = {
  level: number;
  id: string;
  title: string;
};

export type FixtureReference = {
  id: string;
  mode: string;
  path: string;
  expectation: string;
  manifestPath: string;
};

export type Document = {
  kind: string;
  order: number;
  slug: string;
  path: string;
  route: string;
  title: string;
  summary: string;
  html: string;
  headings: Heading[];
  wordCount: number;
  fixtures: FixtureReference[];
};

export type RfcDocument = Document & {
  id: string;
  number: number;
  status: string;
  implementation: string;
  process: string;
  audience: string;
  kind: string;
  primitive: string;
  score: number;
  ratings: Record<string, number>;
};

export type Algorithm = {
  challenge: string;
  title: string;
  category: string;
  summary: string;
  time: string;
  space: string;
  features: string[];
  featured: boolean;
  shape: string;
  order: number;
  slug: string;
  route: string;
  sourcePath: string;
  source: string;
  walkthroughPath: string | null;
  walkthrough: {
    title: string;
    summary: string;
    html: string;
    headings: Heading[];
    wordCount: number;
    fixtures: FixtureReference[];
  } | null;
};

export type SearchEntry = {
  route: string;
  title: string;
  summary: string;
  headings: Array<{ id: string; title: string }>;
  text: string;
  scope: "current" | "development" | "rfc" | "evidence";
  audience: string;
  lifecycle: string;
  tags: string[];
};

export type SurfaceEntry = {
  category: string;
  name: string;
  semanticRole: string;
  rfc: string;
  rfcRoute: string;
};

export const content = contentData as typeof contentData & {
  handbook: Document[];
  appendices: Document[];
  algorithms: Algorithm[];
  development: {
    overview: Document[];
    contracts: Document[];
    subsystems: Document[];
    evidence: Document[];
  };
  rfcs: RfcDocument[];
  surface: {
    entries: SurfaceEntry[];
  };
};
