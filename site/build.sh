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
        python3 "$site/stripTranslations.py" "$stage/$1"
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

# Each file describes itself in its header comment, which Doxygen reads too:
#     /**
#      * @file
#      * @ingroup <group>
#      * @brief <one sentence, possibly wrapped>
#      */
# The assembly modules carry the same lines as ";" comments. Print each
# file's group and sentence.
describe_awk='
{
    line = $0
    if (FILENAME ~ /\.asm$/) {
        if (line !~ /^;/)
            exit
        sub(/^;/, "", line)
    } else {
        if (!open && line !~ /^[ \t]*\/\*\*/)
            next
        open = 1
        closing = line ~ /\*\//
        sub(/^[ \t]*\/\*\*/, "", line)
        sub(/\*\/.*/, "", line)
        sub(/^[ \t]*\*/, "", line)
    }
    gsub(/^[ \t]+|[ \t]+$/, "", line)
    if (collecting && (line == "" || line ~ /^[@\\]/))
        collecting = 0
    if (collecting)
        brief = brief " " line
    if (line ~ /^[@\\]ingroup[ \t]/) {
        group = line
        sub(/^[@\\]ingroup[ \t]+/, "", group)
    }
    if (line ~ /^[@\\]brief[ \t]/) {
        brief = line
        sub(/^[@\\]brief[ \t]+/, "", brief)
        collecting = 1
    }
    if (closing)
        exit
}
END { printf "%s\t%s\n", group, brief }'

# Sources in link order, then the headers alphabetically.
for file in "$@" $(printf '%s\n' $headers | sort); do
    printf '%s\t' "$file"
    awk "$describe_awk" "$file"
done > "$out/files.tsv"

# The groups, in site/groups.dox order.
sed -n 's/^[^@]*@defgroup[[:space:]]\{1,\}\([^[:space:]]\{1,\}\)[[:space:]]\{1,\}\(.*[^[:space:]]\)[[:space:]]*\*\/.*$/\1	\2/p' \
    "$site/groups.dox" > "$out/groups.tsv"

# The main page: site/intro.md, then one table per group.
cp "$site/intro.md" "$stage/mainpage.md"
awk -F '\t' "$listing_awk"'
NR == FNR { title[$1] = $2; order[++groups] = $1; next }
!($2 in title) || $3 == "" {
    printf "error: %s needs @ingroup with a group of site/groups.dox and a @brief\n", $1 > "/dev/stderr"
    failed = 1
}
{ path[++files] = $1; group[files] = $2; brief[files] = $3 }
END {
    if (failed)
        exit 1
    for (g = 1; g <= groups; g++) {
        shown = 0
        for (f = 1; f <= files; f++) {
            if (group[f] != order[g])
                continue
            if (!shown)
                printf "\n### %s\n\n| File | Contents |\n| --- | --- |\n", title[order[g]]
            shown = 1
            count = split(path[f], parts, "/")
            printf "| [%s](%s) | %s |\n", parts[count], listing(path[f]), brief[f]
        }
    }
}' "$out/groups.tsv" "$out/files.tsv" >> "$stage/mainpage.md"

# Link previews need absolute URLs; the Pages workflow passes the address.
site_url=${SITE_URL:-https://neuromancer.github.io/wc1-re}
sed "s|@SITE_URL@|${site_url%/}|g" "$site/header.html" > "$out/header.html"

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
            grep -e 'name="description"' -e 'name="theme-color"' \
                -e 'property="og:' -e 'name="twitter:' "$out/header.html" |
                sed "s|[\$]title|$file|"
            printf '<link rel="stylesheet" href="../wc1.css">\n'
            printf '<link rel="stylesheet" href="asm.css">\n'
            printf '<script src="../doxygen-awesome-darkmode-toggle.js"></script>\n'
            printf '</head>\n<body>\n<main class="asm">\n<div class="topbar">\n'
            printf '<p class="breadcrumb"><a href="../index.html">Wing Commander</a> / %s</p>\n' \
                "$(dirname "$file")"
            printf '<doxygen-awesome-dark-mode-toggle title="Toggle Light/Dark Mode">'
            printf '</doxygen-awesome-dark-mode-toggle>\n</div>\n'
            printf '<script>document.querySelector("doxygen-awesome-dark-mode-toggle").updateIcon()</script>\n'
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
for page in $(awk -F '\t' "$listing_awk"'{ print listing($1) }' "$out/files.tsv"); do
    if [ ! -f "$out/html/$page" ]; then
        echo "error: the file list links to missing page $page" >&2
        missing=1
    fi
done
[ "$missing" = 0 ]
echo "Open $out/html/index.html"
