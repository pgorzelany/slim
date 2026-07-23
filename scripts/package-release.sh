#!/bin/sh
set -eu

slim_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
slim_manifest="$slim_root/release/manifest.txt"
slim_version=$(sed -n '1p' "$slim_root/VERSION")

if test "$#" -gt 1; then
    echo "usage: scripts/package-release.sh [OUTPUT_DIRECTORY]" >&2
    exit 64
fi

case "$slim_version" in
    ''|*[!0-9.]*|.*|*.)
        echo "release: invalid VERSION" >&2
        exit 1
        ;;
esac

slim_output_input=${1:-build/release}
case "$slim_output_input" in
    /*) slim_output=$slim_output_input ;;
    *) slim_output="$slim_root/$slim_output_input" ;;
esac

command -v git >/dev/null 2>&1 || {
    echo "release: git is required to select committed source files" >&2
    exit 1
}
command -v bsdtar >/dev/null 2>&1 || {
    echo "release: bsdtar is required for reproducible ustar output" >&2
    exit 1
}
command -v gzip >/dev/null 2>&1 || {
    echo "release: gzip is required" >&2
    exit 1
}

slim_work=$(mktemp -d "${TMPDIR:-/tmp}/slim-release.XXXXXX")
trap 'rm -rf "$slim_work"' EXIT HUP INT TERM
slim_paths="$slim_work/paths"
slim_tree="$slim_work/tree"
slim_prefix="slim-$slim_version"
mkdir -p "$slim_tree/$slim_prefix" "$slim_output"

if ! test -f "$slim_manifest"; then
    echo "release: missing release/manifest.txt" >&2
    exit 1
fi

slim_dirty=0
while IFS= read -r slim_path; do
    case "$slim_path" in
        ''|'#'*) continue ;;
        /*|*'..'*|*'	'*|*' '*)
            echo "release: unsafe manifest path: $slim_path" >&2
            exit 1
            ;;
    esac
    slim_status=$(git -C "$slim_root" status --porcelain --untracked-files=all -- "$slim_path")
    if test -n "$slim_status"; then
        printf '%s\n' "$slim_status" >&2
        slim_dirty=1
    fi
    slim_selected=$(git -C "$slim_root" ls-files -- "$slim_path")
    if test -z "$slim_selected"; then
        echo "release: manifest path selects no committed files: $slim_path" >&2
        exit 1
    fi
    printf '%s\n' "$slim_selected" >> "$slim_paths"
done < "$slim_manifest"

if test "$slim_dirty" -ne 0; then
    echo "release: included source must be committed and clean" >&2
    exit 1
fi

LC_ALL=C sort -u "$slim_paths" -o "$slim_paths"
while IFS= read -r slim_path; do
    test -n "$slim_path" || continue
    if test -L "$slim_root/$slim_path" || ! test -f "$slim_root/$slim_path"; then
        echo "release: only regular tracked files are supported: $slim_path" >&2
        exit 1
    fi
    mkdir -p "$slim_tree/$slim_prefix/$(dirname -- "$slim_path")"
    cp "$slim_root/$slim_path" "$slim_tree/$slim_prefix/$slim_path"
done < "$slim_paths"

find "$slim_tree/$slim_prefix" -type d -exec chmod 755 {} +
find "$slim_tree/$slim_prefix" -type f -exec chmod 644 {} +
for slim_executable in bootstrap.sh slimc scripts/package-release.sh \
    scripts/refresh-bootstrap-seed.sh scripts/verify-release.sh scripts/verify.sh
do
    chmod 755 "$slim_tree/$slim_prefix/$slim_executable"
done

slim_source_manifest="$slim_tree/$slim_prefix/SOURCE-MANIFEST.sha256"
(
    cd "$slim_tree/$slim_prefix"
    while IFS= read -r slim_path; do
        if command -v sha256sum >/dev/null 2>&1; then
            slim_digest=$(sha256sum "$slim_path" | awk '{ print $1 }')
        else
            slim_digest=$(shasum -a 256 "$slim_path" | awk '{ print $1 }')
        fi
        printf '%s  %s\n' "$slim_digest" "$slim_path"
    done < "$slim_paths"
) > "$slim_source_manifest"

TZ=UTC find "$slim_tree/$slim_prefix" -exec touch -t 200001010000.00 {} +
(
    cd "$slim_tree"
    find "$slim_prefix" -print | LC_ALL=C sort > "$slim_work/archive-paths"
    bsdtar -c --format ustar --uid 0 --gid 0 --uname root --gname root \
        -T "$slim_work/archive-paths" -f "$slim_work/release.tar"
)

slim_archive="$slim_output/$slim_prefix.tar.gz"
gzip -n -9 < "$slim_work/release.tar" > "$slim_work/release.tar.gz"
mv -f "$slim_work/release.tar.gz" "$slim_archive"

if command -v sha256sum >/dev/null 2>&1; then
    slim_digest=$(sha256sum "$slim_archive" | awk '{ print $1 }')
else
    slim_digest=$(shasum -a 256 "$slim_archive" | awk '{ print $1 }')
fi
printf '%s  %s\n' "$slim_digest" "$(basename -- "$slim_archive")" \
    > "$slim_archive.sha256"

echo "release: wrote $slim_archive"
echo "release: SHA-256 $slim_digest"
