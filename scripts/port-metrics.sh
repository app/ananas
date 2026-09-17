#!/usr/bin/env bash
# Read-only burndown metrics for the Ananas Qt4 -> Qt5/Qt6 port.
# Usage: port-metrics.sh [path-to-ananas-legacy-qt4]
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${1:-$SCRIPT_DIR/../../ananas-legacy-qt4}"

if [[ ! -d "$REPO" ]]; then
    echo "Repository not found: $REPO" >&2
    exit 1
fi

cd "$REPO"

# The form designer (src/designer) is excluded from the first porting
# milestone, so it is not counted in the burndown.
src_files() {
    find . -path ./.git -prune -o -path ./src/designer -prune -o \
        -type d \( -name .moc -o -name .obj -o -name .ui \) -prune -o -type f \
        \( -name '*.cpp' -o -name '*.h' -o -name '*.ui' -o -name '*.pro' -o -name '*.pri' \) -print
}

count_occurrences() {
    local pattern="$1"
    src_files | xargs -r grep -Eho "$pattern" 2>/dev/null | wc -l
}

count_files() {
    local pattern="$1"
    src_files | xargs -r grep -El "$pattern" 2>/dev/null | wc -l
}

echo "Ananas port metrics (target: 0 for all Qt3Support/QtScript items)"
echo "Repository: $REPO"
echo "Date:       $(date -u +%Y-%m-%dT%H:%M:%SZ)"
echo
printf '%-42s %8s %8s\n' "metric" "files" "hits"
printf '%-42s %8s %8s\n' "------------------------------------------" "--------" "--------"
printf '%-42s %8s %8s\n' "Q3* identifiers" \
    "$(count_files '\bQ3[A-Za-z0-9_]+')" "$(count_occurrences '\bQ3[A-Za-z0-9_]+')"
printf '%-42s %8s %8s\n' "#include <q3*.h>" \
    "$(count_files '#include[[:space:]]*[<"]q3[A-Za-z0-9_.]+')" "$(count_occurrences '#include[[:space:]]*[<"]q3[A-Za-z0-9_.]+')"
printf '%-42s %8s %8s\n' "Qt3Support / QT3_SUPPORT" \
    "$(count_files 'Qt3Support|QT3_SUPPORT')" "$(count_occurrences 'Qt3Support|QT3_SUPPORT')"
printf '%-42s %8s %8s\n' "QT += qt3support" \
    "$(count_files 'qt3support')" "$(count_occurrences 'qt3support')"
printf '%-42s %8s %8s\n' ".ui files with class=\"Q3...\"" \
    "$(src_files | grep '\.ui$' | xargs -r grep -El 'class="Q3' 2>/dev/null | wc -l)" "-"
printf '%-42s %8s %8s\n' "QDesigner* identifiers" \
    "$(count_files '\bQDesigner[A-Za-z0-9_]+')" "$(count_occurrences '\bQDesigner[A-Za-z0-9_]+')"
printf '%-42s %8s %8s\n' "QScript* identifiers" \
    "$(count_files '\bQScript[A-Za-z0-9_]+')" "$(count_occurrences '\bQScript[A-Za-z0-9_]+')"
echo
echo "Top 15 files by Q3* usage:"
src_files | xargs -r grep -El '\bQ3[A-Za-z0-9_]+' 2>/dev/null \
    | while read -r f; do printf '%6d %s\n' "$(grep -Eho '\bQ3[A-Za-z0-9_]+' "$f" | wc -l)" "$f"; done \
    | sort -rn | head -15
