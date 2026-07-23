import contentData from "../../generated/content.json";

export type SurfaceEntry = {
  category: string;
  name: string;
  semanticRole: string;
  decision: string;
};

export type ReferenceDocument = {
  id: string;
  path: string;
  title: string;
  summary: string;
  html: string;
};

export const content = contentData as typeof contentData & {
  reference: ReferenceDocument[];
  surface: {
    entries: SurfaceEntry[];
  };
};
