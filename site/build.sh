#!/bin/sh
# Static, cross-referenced browser for the reconstructed Win32 source, as
# published on GitHub Pages. Run through `make site`, which passes the Win32
# build's sources in link order. unifdef strips the SDL2 port's code, Doxygen
# renders the C and C++ files and Pygments the hand-written assembly; open
# out/site/html/index.html directly, no server is needed.
set -eu

if [ $# -eq 0 ]; then
    echo "usage: make site" >&2
    exit 2
fi

site=$(cd "$(dirname "$0")" && pwd)
repo=$(cd "$site/.." && pwd)
out=$repo/out/site
stage=$out/source

rm -rf "$stage" "$out/html"
mkdir -p "$stage"
cd "$repo"

# The Win32 build's view: SDL_PORT is undefined, and so is ANALYSIS_BUILD
# (declarations for the comparison tools only).
strip_file() {
    mkdir -p "$stage/$(dirname "$1")"
    case $1 in
    *.asm)
        # MASM has no SDL conditionals, and unifdef only tokenises C.
        cp "$1" "$stage/$1"
        ;;
    *)
        unifdef -x2 -B -USDL_PORT -USDL_PORT_LEGACY_DEBUG_OUTPUT \
            -UANALYSIS_BUILD -o "$stage/$1" "$1"
        ;;
    esac
}

for file in "$@"; do
    strip_file "$file"
done

# Headers: whatever the stripped files still include, transitively. The
# SDL-only headers are included only from SDL_PORT blocks and stay out.
headers=
added=1
while [ "$added" = 1 ]; do
    added=0
    for name in $(find "$stage" -name '*.c' -o -name '*.cpp' -o -name '*.h' |
                  xargs sed -n 's/^[[:space:]]*#[[:space:]]*include[[:space:]]*"\([^"]*\)".*/\1/p' |
                  sort -u); do
        for dir in include src/ix src; do
            if [ -f "$dir/$name" ] && [ ! -f "$stage/$dir/$name" ]; then
                strip_file "$dir/$name"
                headers="$headers $dir/$name"
                added=1
            fi
        done
    done
done

# Doxygen names a file's listing after it: "_" becomes "__" and "." "_8".
listing_awk='
function listing(path,    count, parts, name) {
    count = split(path, parts, "/")
    name = parts[count]
    if (name ~ /\.asm$/)
        return "asm/" name ".html"
    gsub(/_/, "__", name)
    gsub(/\./, "_8", name)
    return name "_source.html"
}'

# The main page: site/intro.md, then one table per group of modules.tsv.
cp "$site/intro.md" "$stage/mainpage.md"
awk -F '\t' "$listing_awk"'
{
    if ($1 != group) {
        group = $1
        printf "\n### %s\n\n| File | Contents |\n| --- | --- |\n", group
    }
    count = split($2, parts, "/")
    printf "| [%s](%s) | %s |\n", parts[count], listing($2), $3
}' "$site/modules.tsv" >> "$stage/mainpage.md"

for file in "$@" $headers; do
    grep -q "	$file	" "$site/modules.tsv" ||
        echo "warning: $file has no description in site/modules.tsv" >&2
done

doxygen "$site/Doxyfile"

# Doxygen cannot parse MASM; Pygments highlights the assembly modules,
# framed by a page that shares the Doxygen pages' stylesheet.
mkdir -p "$out/html/asm"
cp "$site/theme/asm.css" "$out/html/asm/asm.css"
for file in "$@"; do
    case $file in
    *.asm)
        {
            printf '<!DOCTYPE html>\n<html lang="en">\n<head>\n<meta charset="utf-8">\n'
            printf '<meta name="viewport" content="width=device-width, initial-scale=1">\n'
            printf '<title>%s - Wing Commander</title>\n' "$file"
            printf '<link rel="stylesheet" href="../wc1.css">\n'
            printf '<link rel="stylesheet" href="asm.css">\n</head>\n<body>\n<main class="asm">\n'
            printf '<p class="breadcrumb"><a href="../index.html">Wing Commander</a> / %s</p>\n' \
                "$(dirname "$file")"
            printf '<h1>%s</h1>\n' "$(basename "$file")"
            pygmentize -l tasm -f html -O linenos=table "$stage/$file"
            printf '</main>\n</body>\n</html>\n'
        } > "$out/html/asm/$(basename "$file").html"
        ;;
    esac
done
touch "$out/html/.nojekyll"

# Every link in the file list must reach a generated page.
missing=0
for page in $(awk -F '\t' "$listing_awk"'{ print listing($2) }' "$site/modules.tsv"); do
    if [ ! -f "$out/html/$page" ]; then
        echo "error: the file list links to missing page $page" >&2
        missing=1
    fi
done
[ "$missing" = 0 ]
echo "Open $out/html/index.html"
