#!/usr/bin/env bash
# Build the ananas .deb packages (ananas + libqdataschema) from the committed
# tree in the Qt6 build image.
#
# Usage: build-deb.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
BRANCH="${ANANAS_BRANCH:-master}"
IMAGE="${ANANAS_IMAGE:-ananas-qt6-builder}"

# git archive only exports tracked files: fail fast if a source file exists on
# disk but is ignored by .gitignore (it would silently be missing from the build).
MISSING="$(git -C "$REPO" ls-files --others --ignored --exclude-standard \
    | grep -E '\.(h|hpp|cpp|cc|cxx|ui|qrc)$' \
    | grep -vE '^(cmake-build|bin|lib|dist|tmp)/' \
    | grep -vE '/(\.moc|\.obj|\.ui)/' \
    | grep -vE '(^|/)moc_predefs\.h$' \
    | grep -vE '(^|/)(moc_|qrc_)[^/]*\.cpp$' || true)"
if [[ -n "$MISSING" ]]; then
    echo "ERROR: ignored source files would be missing from the build:" >&2
    echo "$MISSING" >&2
    echo "Fix .gitignore or 'git add' them before packaging." >&2
    exit 1
fi

echo "===> 1. Building the Qt6 image (Ubuntu 24.04 + Qt6 + CMake)..."
podman build -t "$IMAGE" -f "$REPO/tools/docker/Containerfile" "$REPO"

echo "===> 2. Building the .deb packages (branch ${BRANCH})..."
podman run --rm \
    -v "$REPO":/repo:z \
    -e BRANCH="$BRANCH" \
    "$IMAGE" \
    bash -c '
        set -e
        export CCACHE_DIR=/repo/tmp/ccache
        mkdir -p "$CCACHE_DIR"
        rm -rf /tmp/ananas && mkdir -p /tmp/ananas
        git -C /repo archive "${BRANCH}" | tar -x -C /tmp/ananas
        cd /tmp/ananas
        dpkg-buildpackage -b -uc -us -d
        mkdir -p /repo/dist
        cp -v /tmp/ananas_*.deb /tmp/libqdataschema_*.deb /repo/dist/
        echo "===> ccache stats:"
        ccache -s
    '

echo "===> done: $REPO/dist"
