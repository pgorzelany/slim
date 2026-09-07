#!/usr/bin/env python3
"""Pack, verify, or restore dated evidence without changing its contents."""

import argparse
import csv
import gzip
import hashlib
import io
from pathlib import Path


RESULTS = Path(__file__).resolve().parents[1] / "benchmarks/results"
ARCHIVE = RESULTS / "archive"
MANIFEST = ARCHIVE / "manifest.tsv"
FIELDS = ["file", "sha256", "bytes", "lines", "gzip_bytes"]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def require(condition, message):
    if not condition:
        raise ValueError(message)


def filename(value):
    if Path(value).name != value or not value.startswith("20"):
        raise ValueError(f"invalid evidence filename: {value}")
    return value


def verified(row):
    compressed = (ARCHIVE / (filename(row["file"]) + ".gz")).read_bytes()
    data = gzip.decompress(compressed)
    require(digest(data) == row["sha256"], f"hash mismatch: {row['file']}")
    require(len(data) == int(row["bytes"]), f"size mismatch: {row['file']}")
    require(len(data.splitlines()) == int(row["lines"]), f"line count mismatch: {row['file']}")
    require(len(compressed) == int(row["gzip_bytes"]), f"gzip size mismatch: {row['file']}")
    return data


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    pack = commands.add_parser("pack", help="archive named files in benchmarks/results")
    pack.add_argument("files", nargs="+")
    commands.add_parser("verify", help="verify every archived byte against the manifest")
    extract = commands.add_parser("extract", help="restore files without overwriting different data")
    extract.add_argument("directory", type=Path)
    args = parser.parse_args()
    rows = []
    if MANIFEST.exists():
        with MANIFEST.open(newline="") as stream:
            rows = list(csv.DictReader(stream, delimiter="\t"))
    require(len({row["file"] for row in rows}) == len(rows), "duplicate manifest entry")
    for row in rows:
        verified(row)
    if args.command == "pack":
        names = [filename(name) for name in args.files]
        require(len(set(names)) == len(names), "duplicate input")
        require(not set(names) & {row["file"] for row in rows}, "archive names are immutable")
        pending = [(name, (RESULTS / name).read_bytes()) for name in names]
        ARCHIVE.mkdir(parents=True, exist_ok=True)
        for name, data in pending:
            compressed = gzip.compress(data, compresslevel=9, mtime=0)
            require(gzip.decompress(compressed) == data, f"compression mismatch: {name}")
            destination = ARCHIVE / (name + ".gz")
            with destination.open("xb") as stream:
                stream.write(compressed)
            row = dict(zip(FIELDS, [name, digest(data), len(data), len(data.splitlines()), len(compressed)]))
            verified(row)
            rows.append(row)
        output = io.StringIO(newline="")
        writer = csv.DictWriter(output, fieldnames=FIELDS, delimiter="\t", lineterminator="\n")
        writer.writeheader()
        writer.writerows(sorted(rows, key=lambda row: row["file"]))
        temporary = MANIFEST.with_suffix(".tmp")
        temporary.write_text(output.getvalue())
        temporary.replace(MANIFEST)
        for name, data in pending:
            require((RESULTS / name).read_bytes() == data, "source changed during packing")
            (RESULTS / name).unlink()
    elif args.command == "extract":
        args.directory.mkdir(parents=True, exist_ok=True)
        for row in rows:
            destination = args.directory / row["file"]
            data = verified(row)
            if destination.exists():
                require(destination.read_bytes() == data, f"refusing to overwrite {destination}")
            else:
                with destination.open("xb") as stream:
                    stream.write(data)
    print(f"{args.command}: {len(rows)} archives verified; {sum(int(row['bytes']) for row in rows)} original bytes")


if __name__ == "__main__":
    main()
