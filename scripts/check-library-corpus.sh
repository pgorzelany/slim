#!/bin/sh
set -eu

slim_corpus_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
slim_corpus_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-library-corpus.XXXXXX")
trap 'rm -rf "$slim_corpus_work"' EXIT HUP INT TERM
slim_corpus_compiler="$slim_corpus_root/slimc"
slim_corpus_tab=$(printf '\t')
slim_corpus_count=0

while IFS="$slim_corpus_tab" read -r slim_corpus_name slim_corpus_project; do
    case "$slim_corpus_name" in
        ''|'#'*) continue ;;
    esac
    if test -z "$slim_corpus_project"; then
        echo "library corpus: malformed row for $slim_corpus_name" >&2
        exit 1
    fi
    slim_corpus_count=$((slim_corpus_count + 1))
    "$slim_corpus_compiler" fmt "$slim_corpus_root/$slim_corpus_project" --check
    "$slim_corpus_compiler" check "$slim_corpus_root/$slim_corpus_project"
    "$slim_corpus_compiler" build "$slim_corpus_root/$slim_corpus_project" -o "$slim_corpus_work/$slim_corpus_name"
done < "$slim_corpus_root/library/corpus.tsv"

"$slim_corpus_work/standard-library" > "$slim_corpus_work/standard-library.first"
"$slim_corpus_work/standard-library" > "$slim_corpus_work/standard-library.second"
cmp "$slim_corpus_work/standard-library.first" "$slim_corpus_work/standard-library.second"

"$slim_corpus_work/development-operation-cost" \
    "$slim_corpus_root/library/applications/development_operation_cost/fixtures/observations.ns" \
    > "$slim_corpus_work/development-operation-cost.out"
cmp "$slim_corpus_work/development-operation-cost.out" \
    "$slim_corpus_root/library/applications/development_operation_cost/fixtures/observations.expected"

"$slim_corpus_work/project-impact" \
    "$slim_corpus_root/library/applications/project_impact/fixtures/before-catalog.ns" \
    "$slim_corpus_root/library/applications/project_impact/fixtures/before-graph.ns" \
    "$slim_corpus_root/library/applications/project_impact/fixtures/after-catalog.ns" \
    "$slim_corpus_root/library/applications/project_impact/fixtures/after-graph.ns" \
    > "$slim_corpus_work/project-impact.out"
cmp "$slim_corpus_work/project-impact.out" \
    "$slim_corpus_root/library/applications/project_impact/fixtures/impact.expected"

"$slim_corpus_work/development-summary" \
    "$slim_corpus_root/library/applications/development_summary/fixtures/observations.ns" \
    > "$slim_corpus_work/development-summary.out"
cmp "$slim_corpus_work/development-summary.out" \
    "$slim_corpus_root/library/applications/development_summary/fixtures/observations.expected"

if "$slim_corpus_work/api-diff" \
    "$slim_corpus_root/library/tools/fixtures/api-previous.sli" \
    "$slim_corpus_root/library/tools/fixtures/api-current.sli" \
    > "$slim_corpus_work/api-diff.out"; then
    echo "library corpus: breaking interface change was accepted" >&2
    exit 1
fi
if ! grep -q '^compatible no$' "$slim_corpus_work/api-diff.out"; then
    echo "library corpus: API comparison did not report incompatibility" >&2
    exit 1
fi

"$slim_corpus_work/ndjson" \
    "$slim_corpus_root/library/applications/ndjson/fixtures/telemetry.ndjson" \
    > "$slim_corpus_work/ndjson.first"
"$slim_corpus_work/ndjson" \
    "$slim_corpus_root/library/applications/ndjson/fixtures/telemetry.ndjson" \
    > "$slim_corpus_work/ndjson.second"
cmp "$slim_corpus_work/ndjson.first" "$slim_corpus_work/ndjson.second"

"$slim_corpus_work/ledger" \
    "$slim_corpus_root/library/applications/ledger/fixtures/ledger.log" \
    > "$slim_corpus_work/ledger.first"
"$slim_corpus_work/ledger" \
    "$slim_corpus_root/library/applications/ledger/fixtures/ledger.log" \
    > "$slim_corpus_work/ledger.second"
cmp "$slim_corpus_work/ledger.first" "$slim_corpus_work/ledger.second"

"$slim_corpus_work/sat" \
    "$slim_corpus_root/library/applications/sat/fixtures/sat.cnf" \
    > "$slim_corpus_work/sat.first"
"$slim_corpus_work/sat" \
    "$slim_corpus_root/library/applications/sat/fixtures/sat.cnf" \
    > "$slim_corpus_work/sat.second"
cmp "$slim_corpus_work/sat.first" "$slim_corpus_work/sat.second"
"$slim_corpus_work/sat" \
    "$slim_corpus_root/library/applications/sat/fixtures/unsat.cnf" \
    > "$slim_corpus_work/unsat.out"

"$slim_corpus_work/raster" \
    "$slim_corpus_root/library/applications/raster/fixtures/scene.txt" \
    > "$slim_corpus_work/raster.first.pgm"
"$slim_corpus_work/raster" \
    "$slim_corpus_root/library/applications/raster/fixtures/scene.txt" \
    > "$slim_corpus_work/raster.second.pgm"
cmp "$slim_corpus_work/raster.first.pgm" "$slim_corpus_work/raster.second.pgm"

"$slim_corpus_work/lz4" compress \
    "$slim_corpus_root/library/applications/lz4/fixtures/repetitive.txt" \
    > "$slim_corpus_work/lz4.first"
"$slim_corpus_work/lz4" compress \
    "$slim_corpus_root/library/applications/lz4/fixtures/repetitive.txt" \
    > "$slim_corpus_work/lz4.second"
cmp "$slim_corpus_work/lz4.first" "$slim_corpus_work/lz4.second"
"$slim_corpus_work/lz4" decompress "$slim_corpus_work/lz4.first" \
    > "$slim_corpus_work/lz4.roundtrip"
cmp "$slim_corpus_work/lz4.roundtrip" \
    "$slim_corpus_root/library/applications/lz4/fixtures/repetitive.txt"

slim_corpus_seed=0
while test "$slim_corpus_seed" -lt 12; do
    "$slim_corpus_work/source-gen" valid "$slim_corpus_seed" \
        > "$slim_corpus_work/generated-$slim_corpus_seed.slim"
    "$slim_corpus_work/source-gen" valid "$slim_corpus_seed" \
        > "$slim_corpus_work/generated-$slim_corpus_seed.second.slim"
    cmp "$slim_corpus_work/generated-$slim_corpus_seed.slim" \
        "$slim_corpus_work/generated-$slim_corpus_seed.second.slim"
    "$slim_corpus_compiler" fmt \
        "$slim_corpus_work/generated-$slim_corpus_seed.slim" --check
    "$slim_corpus_compiler" check \
        "$slim_corpus_work/generated-$slim_corpus_seed.slim"
    "$slim_corpus_compiler" run \
        "$slim_corpus_work/generated-$slim_corpus_seed.slim" \
        > "$slim_corpus_work/generated-$slim_corpus_seed.out"

    "$slim_corpus_work/source-gen" mutant "$slim_corpus_seed" \
        > "$slim_corpus_work/mutant-$slim_corpus_seed.slim"
    if "$slim_corpus_compiler" check \
        "$slim_corpus_work/mutant-$slim_corpus_seed.slim" \
        > "$slim_corpus_work/mutant-$slim_corpus_seed.out" 2>&1; then
        echo "library corpus: mutant $slim_corpus_seed was accepted" >&2
        exit 1
    fi
    slim_corpus_seed=$((slim_corpus_seed + 1))
done

"$slim_corpus_root/scripts/generate-library-docs.sh" --check
python3 "$slim_corpus_root/scripts/verify-source-components.py" \
    --binaries-directory "$slim_corpus_work" --output "$slim_corpus_work/source-components" --full --faults --work
python3 "$slim_corpus_root/scripts/verify-catalog-diff.py" \
    --catalog-binary "$slim_corpus_work/catalog" --output "$slim_corpus_work/catalog-diff" --full --faults --work
echo "library corpus: $slim_corpus_count projects, 12 valid generated programs, and 12 rejected mutants passed"
