import contentData from "../../generated/content.json";

export type SurfaceEntry = {
  category: string;
  name: string;
  semanticRole: string;
  decision: string;
};

export type ReferenceDocument = {
  id: string;
  order: number;
  path: string;
  route: string;
  title: string;
  summary: string;
  html: string;
  headings: Heading[];
  fixtures: FixtureReference[];
};

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

export type BookChapter = {
  kind: string;
  order: number;
  slug: string;
  path: string;
  route: string;
  title: string;
  summary: string;
  html: string;
  headings: Heading[];
  fixtures: FixtureReference[];
};

export type SearchEntry = {
  route: string;
  title: string;
  summary: string;
  headings: Array<{ id: string; title: string }>;
  text: string;
};

export const content = contentData as typeof contentData & {
  guide: BookChapter[];
  languageReference: BookChapter[];
  reference: ReferenceDocument[];
  search: SearchEntry[];
  routes: string[];
  surface: {
    entries: SurfaceEntry[];
  };
};
