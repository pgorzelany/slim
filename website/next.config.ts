import type { NextConfig } from "next";

const basePath = process.env.PAGES_BASE_PATH ?? "";

if (
  basePath !== "" &&
  (!basePath.startsWith("/") || basePath.endsWith("/"))
) {
  throw new Error("PAGES_BASE_PATH must be empty or a leading, non-trailing path");
}

const nextConfig: NextConfig = {
  output: "export",
  trailingSlash: true,
  basePath,
  images: {
    unoptimized: true,
  },
};

export default nextConfig;
