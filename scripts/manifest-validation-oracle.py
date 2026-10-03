#!/usr/bin/env python3
"""RFC167 finite fixture and byte-trie geometry oracle, never a SLIM checker.

All plans below are fixed test data. No supplied source is parsed or accepted.
Generated diagnostic spans come from literal construction positions. Native
old/candidate checking remains the only acceptance authority.
"""

from dataclasses import dataclass, replace
import argparse
import hashlib
import json
from pathlib import Path


COUNTER_CAP = 1_000_000_000
SIZES = (16, 32, 64, 128, 256)
COUNTERS = (
    "builder_module_headers", "unknown_module_headers", "cycle_module_headers",
    "unknown_import_headers", "cycle_import_headers", "insert_calls",
    "insertion_character_headers", "query_calls", "query_character_headers",
    "edge_headers", "nodes_appended", "edges_appended", "terminal_value_reads",
)
ELIGIBILITY = (
    "preflight_headers", "preflight_token_reads", "preflight_span_checks",
    "query_span_checks",
)
LEGACY = (
    "manifest_tokens", "entry_headers", "entry_comparisons",
    "module_order_headers", "duplicate_module_headers", "prior_module_headers",
    "module_name_pairs", "path_module_headers", "prior_path_headers", "path_pairs",
    "unknown_module_headers", "unknown_import_headers", "unknown_lookup_headers",
    "unknown_lookup_comparisons", "cycle_module_headers", "cycle_import_headers",
    "cycle_unused_lookup_headers", "cycle_unused_lookup_comparisons",
    "cycle_actual_lookup_headers", "cycle_actual_lookup_comparisons",
    "cycle_unused_list_headers", "cycle_actual_list_headers",
)


def add(counts, name, amount=1):
    if type(amount) is not int or amount < 0 or amount > COUNTER_CAP:
        raise ValueError("counter operand outside finite domain")
    before = counts[name]
    if before > COUNTER_CAP - amount:
        raise ValueError("counter cap exceeded")
    counts[name] = before + amount


@dataclass(frozen=True)
class Module:
    name: bytes
    path: bytes                 # Exact quoted path, including both quotes.
    imports: tuple = ()
    exports: tuple = ()


@dataclass(frozen=True)
class Case:
    identity: str
    family: str
    modules: tuple = ()
    entry: bytes = b""
    gate: str = "pass"           # Fixed observation plan; no gate inference.
    error: tuple = ()            # (code, field, module, atom) records.
    path_stop: int | None = None
    order_stop: int | None = None
    duplicate_module_stop: int | None = None
    unknown_stop: int | None = None
    cycle_stop: int | None = None
    raw: bytes | None = None


def name(index):
    return f"m{index:04d}".encode("ascii")


def modules(size, edge_kind="empty", quoted_length=None):
    result = []
    for index in range(size):
        identity = name(index)
        if quoted_length is None:
            path = b'"lib/' + identity + b'.slim"'
        else:
            path = b'"' + b"a" * (quoted_length - 13) + b"/" + identity + b'.slim"'
            if len(path) != quoted_length:
                raise ValueError("declared raw path length mismatch")
        targets = () if not index or edge_kind == "empty" else (
            (name(0),) if edge_kind == "front" else (name(index - 1),)
        )
        result.append(Module(identity, path, targets))
    return tuple(result)


def geometric_cases():
    result = []
    for size in SIZES:
        rows = modules(size)
        result.append(Case(f"paths-{size}", "paths", rows, name(size - 1)))
        duplicated = rows[:-1] + (replace(rows[-1], path=rows[0].path),)
        result.append(Case(
            f"duplicate-{size}", "duplicate", duplicated, name(size - 1),
            "path", (("E0408", "path", size - 1, 0),), path_stop=size - 1,
        ))
    for kind in ("front", "back"):
        for size in SIZES:
            result.append(Case(f"{kind}-{size}", kind, modules(size, kind), name(size - 1)))
    for count in SIZES:
        rows, remaining = list(modules(256)), count
        for index in range(256):
            take = min(index, remaining)
            rows[index] = replace(rows[index], imports=tuple(name(i) for i in range(take)))
            remaining -= take
        if remaining:
            raise ValueError("fixed edge campaign construction mismatch")
        result.append(Case(f"edges-{count}", "edges", tuple(rows), name(255)))
    for length in (32, 64, 128, 256):
        result.append(Case(f"bytes-{length}", "bytes", modules(16, "back", length), name(15)))
    if len(result) != 29:
        raise ValueError("fixed geometric matrix mismatch")
    return tuple(result)


def control_cases():
    rows = modules(3)
    duplicate_before_invalid = (
        rows[0], replace(rows[1], path=rows[0].path),
        replace(rows[2], path=b'"../bad.slim"'),
    )
    duplicate_after_invalid = (
        replace(rows[0], path=b'"../bad.slim"'), rows[1],
        replace(rows[2], path=rows[1].path),
    )
    result = (
        Case("prefix", "control", (
            Module(b"m", b'"lib/m.slim"'), Module(b"ma", b'"lib/ma.slim"'),
        ), b"ma"),
        Case("nonascii-path", "control", (
            Module(name(0), '"lib/ą.slim"'.encode()), Module(name(1), rows[1].path),
        ), name(1)),
        Case("unknown-entry", "control", rows, b"missing", "entry",
             (("E0403", "entry", 0, 0),)),
        Case("unknown-first", "control", (
            replace(rows[0], imports=(b"missing",)), rows[1], rows[2],
        ), name(2), "unknown", (("E0411", "import", 0, 0),), unknown_stop=0),
        Case("unknown-last", "control", (
            rows[0], replace(rows[1], imports=(name(0),)),
            replace(rows[2], imports=(b"missing",)),
        ), name(2), "unknown", (("E0411", "import", 2, 0),), unknown_stop=1),
        Case("unsorted-duplicate", "control", (
            rows[0], replace(rows[1], name=name(0)),
        ), name(0), "order", (
            ("E0406", "name", 1, 0), ("E0408", "name", 1, 0),
        ), order_stop=1, duplicate_module_stop=1),
        Case("unsorted-later-duplicate", "control", (
            rows[1], rows[0], replace(rows[2], name=name(0)),
        ), name(0), "order", (("E0406", "name", 1, 0),),
             order_stop=1, duplicate_module_stop=2),
        Case("duplicate-before-invalid", "control", duplicate_before_invalid,
             name(2), "path", (("E0408", "path", 1, 0),), path_stop=1),
        Case("duplicate-after-invalid", "control", duplicate_after_invalid,
             name(2), "path", (("E0408", "path", 2, 0),), path_stop=2),
        Case("duplicate-before-list", "control", (
            rows[0], replace(rows[1], path=rows[0].path),
            replace(rows[2], imports=(name(1), name(0))),
        ), name(2), "path", (("E0408", "path", 1, 0),), path_stop=1),
        Case("self-import", "control", (
            replace(rows[0], imports=(name(0),)), rows[1],
        ), name(1), "self", (("E0412", "import", 0, 0),)),
        Case("reciprocal", "control", (
            replace(rows[0], imports=(name(1),)),
            replace(rows[1], imports=(name(0),)),
        ), name(1), "cycle", (("E0413", "name", 0, 0),), cycle_stop=0),
    )
    if len(result) != 12:
        raise ValueError("fixed control matrix mismatch")
    return result


def render(case):
    if case.raw is not None:
        _, _, files = render(replace(case, raw=None))
        return case.raw, {}, files
    output, fields, files = bytearray(), {}, {}

    def token(value, field):
        start = len(output)
        output.extend(value)
        fields[field] = (start, len(output))

    output.extend(b"(project 1\n  (entry ")
    token(case.entry, ("entry", 0, 0))
    output.extend(b")\n")
    for index, row in enumerate(case.modules):
        output.extend(b"  (module ")
        token(row.name, ("name", index, 0))
        output.extend(b" ")
        token(row.path, ("path", index, 0))
        output.extend(b" (imports")
        for ordinal, imported in enumerate(row.imports):
            output.extend(b" ")
            token(imported, ("import", index, ordinal))
        output.extend(b") (exports")
        for ordinal, exported in enumerate(row.exports):
            output.extend(b" ")
            token(exported, ("export", index, ordinal))
        output.extend(b"))\n")
        path = row.path[1:-1]
        # Only known positive fixture file paths are materialized. Invalid
        # manifest spellings are diagnostic controls, never filesystem authority.
        if path and all(part not in (b"", b".", b"..") for part in path.split(b"/")) and b"\\" not in path:
            source = b"module " + row.name + b"\n\n"
            if row.name == case.entry:
                source += b"fn main(args: Vec[Bytes]) -> I64:\n  0\n"
            else:
                source += b"fn value() -> I64:\n  0\n"
            files.setdefault(path.decode("utf-8"), source)
    output.extend(b")\n")
    return bytes(output), fields, files


def malformed_cases():
    suffixes = (
        ("head", b"  (module"),
        ("name", b"  (module z"),
        ("path", b'  (module z "z.slim"'),
        ("imports", b'  (module z "z.slim" (imports'),
        ("exports", b'  (module z "z.slim" (imports) (exports'),
        ("atom", b"  z"),
        ("extra-close", b") z"),
        ("escape", b'  "tail' + bytes((92,))),
    )
    rows = modules(2)
    prefixes = (
        ("unsorted", Case("basis", "malformed", (rows[1], rows[0]), name(0))),
        ("sorted-distinct", Case("basis", "malformed", rows, name(1))),
        ("sorted-duplicate", Case("basis", "malformed", (
            rows[0], replace(rows[1], path=rows[0].path),
        ), name(1))),
    )
    result = []
    for kind, base in prefixes:
        data, _, _ = render(base)
        for label, suffix in suffixes:
            result.append(Case(
                f"malformed-{kind}-{label}", "malformed", base.modules, base.entry,
                "malformed", raw=data[:-2] + suffix,
            ))
    result.append(Case(
        "malformed-short-escape", "malformed", gate="malformed",
        raw=b'(project 1 (entry app) x ) app "foo' + bytes((92,)),
    ))
    if len(result) != 25:
        raise ValueError("fixed malformed matrix mismatch")
    return tuple(result)


class TrieGeometry:
    def __init__(self):
        self.counts = dict.fromkeys(COUNTERS, 0)
        self.nodes = []
        self.append_node()
        self.append_node()

    def append_node(self):
        add(self.counts, "nodes_appended")
        self.nodes.append({"value": -1, "edges": []})
        return len(self.nodes) - 1

    def child(self, parent, code):
        for candidate, child in self.nodes[parent]["edges"]:
            add(self.counts, "edge_headers")
            if candidate == code:
                return child
        add(self.counts, "edge_headers")
        return -1

    def insert(self, key, root, value):
        add(self.counts, "insert_calls")
        node = root
        for code in key:
            add(self.counts, "insertion_character_headers")
            child = self.child(node, code)
            if child < 0:
                child = self.append_node()
                add(self.counts, "edges_appended")
                self.nodes[node]["edges"].insert(0, (code, child))
            node = child
        add(self.counts, "insertion_character_headers")
        add(self.counts, "terminal_value_reads")
        previous = self.nodes[node]["value"]
        if previous < 0:
            self.nodes[node]["value"] = value
        return previous

    def query(self, key):
        add(self.counts, "query_calls")
        node = 0
        for code in key:
            add(self.counts, "query_character_headers")
            node = self.child(node, code)
            if node < 0:
                return -1
        add(self.counts, "query_character_headers")
        add(self.counts, "terminal_value_reads")
        return self.nodes[node]["value"]


def phase_queries(case, stop):
    """Expand only a predeclared finite query-prefix plan, not a source checker."""
    visits, module_headers, import_headers = [], 0, 0
    for owner, row in enumerate(case.modules):
        module_headers += 1
        for target in row.imports:
            import_headers += 1
            visits.append((owner, target))
            if stop is not None and len(visits) == stop + 1:
                return visits, module_headers, import_headers
        import_headers += 1
    return visits, module_headers + 1, import_headers


def extents(case):
    n = len(case.modules)
    e = sum(len(row.imports) for row in case.modules)
    bm = sum(len(row.name) for row in case.modules)
    bp = sum(len(row.path) for row in case.modules)
    be = sum(len(key) for row in case.modules for key in row.imports)
    exports = sum(len(row.exports) for row in case.modules)
    return {"N": n, "E": e, "Bm": bm, "Bp": bp, "Be": be,
            "B": bm + bp + be, "P": 8 + 11 * n + e + exports}


def indexed_geometry(case):
    if case.gate in ("entry", "order", "malformed"):
        return {"scope": "declined-or-not-reached", "work": None}
    extent = extents(case)
    n, e = extent["N"], extent["E"]
    trie = TrieGeometry()
    eligibility = dict(zip(ELIGIBILITY, (2 * n + e + 1, 12 * n + e + 5, 2 * n + e, 0)))
    ordinal = 7
    for index, row in enumerate(case.modules):
        add(trie.counts, "builder_module_headers")
        if trie.insert(row.name, 0, ordinal) >= 0:
            raise ValueError("fixed eligible name plan is not distinct")
        previous = trie.insert(row.path, 1, ordinal + 3)
        if index == case.path_stop:
            if previous < 0:
                raise ValueError("fixed duplicate path plan mismatch")
            break
        if previous >= 0:
            raise ValueError("unplanned duplicate path")
        ordinal += 11 + len(row.imports) + len(row.exports)
    else:
        add(trie.counts, "builder_module_headers")
    if case.gate in ("pass", "unknown", "cycle"):
        visits, mh, ih = phase_queries(case, case.unknown_stop)
        add(trie.counts, "unknown_module_headers", mh)
        add(trie.counts, "unknown_import_headers", ih)
        for _, target in visits:
            trie.query(target)
            add(eligibility, "query_span_checks")
    if case.gate in ("pass", "cycle"):
        visits, mh, ih = phase_queries(case, case.cycle_stop)
        add(trie.counts, "cycle_module_headers", mh)
        add(trie.counts, "cycle_import_headers", ih)
        for _, target in visits:
            trie.query(target)       # Retained unused reciprocal target lookup.
            trie.query(target)       # Explicit target lookup, in that order.
            add(eligibility, "query_span_checks", 2)
    w = sum(trie.counts.values())
    local = w + sum(eligibility.values())
    ceiling = 800 * (extent["B"] + n + e + 1)
    if local > ceiling or local > COUNTER_CAP:
        raise ValueError("fixed local work admission failed")
    return {"scope": "exact-finite-data-geometry", "work": trie.counts,
            "eligibility": eligibility, "W": w, "local_work": local, "ceiling": ceiling}


def legacy_geometry(case):
    if case.gate == "malformed":
        return {"scope": "unknown-until-held-native-observation", "work": None, "trace": None}
    counts, trace = dict.fromkeys(LEGACY, 0), []
    n = len(case.modules)
    counts["manifest_tokens"] = extents(case)["P"]
    names = [row.name for row in case.modules]

    def search(key, phase, code):
        trace.append(code)
        for candidate in names:
            add(counts, phase + "_headers")
            add(counts, phase + "_comparisons")
            if candidate == key:
                return
        add(counts, phase + "_headers")

    search(case.entry, "entry", "1")
    if case.gate == "entry":
        return {"scope": "exact-finite-data-geometry", "work": counts, "trace": "".join(trace)}
    add(counts, "module_order_headers", n + 1 if case.order_stop is None else case.order_stop + 1)
    if case.gate == "order":
        for index, current in enumerate(names):
            add(counts, "duplicate_module_headers")
            for prior in names[:index]:
                add(counts, "prior_module_headers")
                add(counts, "module_name_pairs")
                if prior == current:
                    if index != case.duplicate_module_stop:
                        raise ValueError("fixed duplicate module plan mismatch")
                    return {"scope": "exact-finite-data-geometry", "work": counts, "trace": "".join(trace)}
            add(counts, "prior_module_headers")
        raise ValueError("fixed duplicate-module stop not reached")
    for index, row in enumerate(case.modules):
        add(counts, "path_module_headers")
        for prior in case.modules[:index]:
            add(counts, "prior_path_headers")
            add(counts, "path_pairs")
            if prior.path == row.path:
                if index != case.path_stop:
                    raise ValueError("fixed duplicate path stop mismatch")
                return {"scope": "exact-finite-data-geometry", "work": counts, "trace": "".join(trace)}
        add(counts, "prior_path_headers")
    add(counts, "path_module_headers")
    if case.gate in ("pass", "unknown", "cycle"):
        visits, mh, ih = phase_queries(case, case.unknown_stop)
        add(counts, "unknown_module_headers", mh)
        add(counts, "unknown_import_headers", ih)
        for _, target in visits:
            search(target, "unknown_lookup", "2")
    if case.gate in ("pass", "cycle"):
        visits, mh, ih = phase_queries(case, case.cycle_stop)
        add(counts, "cycle_module_headers", mh)
        add(counts, "cycle_import_headers", ih)
        by_name = {row.name: row for row in case.modules}
        for owner, target in visits:
            target_imports = by_name[target].imports
            search(target, "cycle_unused_lookup", "3")
            trace.append("5")
            add(counts, "cycle_unused_list_headers", len(target_imports) + 1)
            search(target, "cycle_actual_lookup", "4")
            trace.append("6")
            source = case.modules[owner].name
            amount = target_imports.index(source) + 1 if source in target_imports else len(target_imports) + 1
            add(counts, "cycle_actual_list_headers", amount)
    return {"scope": "exact-finite-data-geometry", "work": counts, "trace": "".join(trace)}


def record(case):
    data, fields, files = render(case)
    diagnostics = b"".join(
        f"{code}@-@{fields[(field, index, atom)][0]}:{fields[(field, index, atom)][1]}\n".encode()
        for code, field, index, atom in case.error
    )
    extent = None if case.raw is not None else extents(case)
    if len(data) > (4096 if case.family == "malformed" else 1048576):
        raise ValueError("manifest admission exceeded")
    aggregate = len(data) + sum(len(source) for source in files.values())
    if aggregate > 4194304 or any(len(source) > 1048576 for source in files.values()):
        raise ValueError("fixture source admission exceeded")
    if extent and (extent["N"] > 256 or extent["E"] > 32768 or extent["P"] > 1000000):
        raise ValueError("finite node/count admission exceeded")
    # Tiny fixed source bodies have <100 canonical entries each. This is an
    # explicit conservative fixture upper bound, not a parser result.
    source_node_upper = 100 * len(case.modules) + 1024
    if source_node_upper > 1000000:
        raise ValueError("fixed source-node upper bound exceeded")
    return {
        "id": case.identity, "family": case.family, "gate": case.gate,
        "manifest_bytes": len(data), "manifest_sha256": hashlib.sha256(data).hexdigest(),
        "aggregate_bytes": aggregate, "source_node_upper_bound": source_node_upper,
        "extents": extent, "files": {path: {"bytes": len(value), "sha256": hashlib.sha256(value).hexdigest()} for path, value in sorted(files.items())},
        "status": None if case.gate == "malformed" else (0 if case.gate == "pass" else 1),
        "diagnostics_hex": None if case.gate == "malformed" else diagnostics.hex(),
        "indexed": indexed_geometry(case), "legacy": legacy_geometry(case),
    }


def cases():
    result = geometric_cases() + control_cases() + malformed_cases()
    if len(result) != 66 or len({case.identity for case in result}) != 66:
        raise ValueError("finite fixture count/identity mismatch")
    return result


def emit(directory):
    directory = Path(directory)
    directory.mkdir(parents=True, exist_ok=False)
    rows = []
    for case in cases():
        case_dir = directory / case.identity
        case_dir.mkdir()
        data, _, files = render(case)
        (case_dir / "slim.project").write_bytes(data)
        for relative, source in files.items():
            path = case_dir / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(source)
        rows.append(record(case))
    payload = {"schema": 1, "authority": "finite-measurement-data-only",
               "counter_cap": COUNTER_CAP, "counts": {"geometric": 29, "control": 12, "malformed": 25},
               "cases": rows}
    (directory / "oracle.json").write_text(json.dumps(payload, sort_keys=True, indent=2) + "\n")
    return payload


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--emit", type=Path, required=True, help="fresh fixed-fixture directory")
    arguments = parser.parse_args()
    payload = emit(arguments.emit)
    print(json.dumps({"cases": len(payload["cases"]), "oracle_sha256": hashlib.sha256((arguments.emit / "oracle.json").read_bytes()).hexdigest()}, sort_keys=True))


if __name__ == "__main__":
    main()
