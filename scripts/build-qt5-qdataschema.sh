#!/usr/bin/env bash
# Build the libqdataschema Debian package for Qt5 in the ananas-qt5-builder
# image (Ubuntu 24.04), the same way the main ananas package is built.
#
# The resulting package satisfies the `libqdataschema (>= 1.0.0)` dependency of
# the ananas .deb, so it can be installed on the host with apt.
#
# Usage: build-qt5-qdataschema.sh
#
# Environment:
#   QDS_BRANCH    qdataschema branch to build (default: qt5)
#   ANANAS_IMAGE  build image to use (default: ananas-qt5-builder)
set -e

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
REPO="$WORKSPACE_DIR/ananas-legacy-qdataschema"
BRANCH="${QDS_BRANCH:-qt5}"
IMAGE="${ANANAS_IMAGE:-ananas-qt5-builder}"

# git archive only exports tracked files: fail fast if a source file exists on
# disk but is ignored by .gitignore (it would silently be missing from the build).
MISSING="$(git -C "$REPO" ls-files --others --ignored --exclude-standard \
    | grep -E '\.(h|hpp|cpp|cc|cxx|pro|pri|ui|qrc)$' \
    | grep -vE '/(\.moc|\.obj|\.ui)/' \
    | grep -vE '(^|/)moc_predefs\.h$' \
    | grep -vE '(^|/)(moc_|qrc_)[^/]*\.cpp$' || true)"
if [[ -n "$MISSING" ]]; then
    echo "ERROR: ignored source files would be missing from the build:" >&2
    echo "$MISSING" >&2
    echo "Fix .gitignore or 'git add' them before packaging." >&2
    exit 1
fi

echo "===> 1. Сборка Podman-образа (Ubuntu 24.04 + Qt5)..."
podman build -t "$IMAGE" -f "$WORKSPACE_DIR/tools/docker/Containerfile.qt5" "$WORKSPACE_DIR"

echo "===> 2. Сборка libqdataschema (ветка ${BRANCH})..."
podman run --rm \
  -v "$WORKSPACE_DIR":/workspace:z \
  -e BRANCH="$BRANCH" \
  "$IMAGE" \
  bash -c '
    set -e
    export CCACHE_DIR=/workspace/tmp/ccache
    mkdir -p "$CCACHE_DIR"
    rm -rf /tmp/qdataschema
    mkdir -p /tmp/qdataschema
    git -C /workspace/ananas-legacy-qdataschema archive "${BRANCH}" | tar -x -C /tmp/qdataschema
    cd /tmp/qdataschema

    echo "Запуск компиляции..."
    # -d: build-deps are provided by the image and are not registered in dpkg
    dpkg-buildpackage -b -uc -us -d

    echo "Копирование пакетов в /workspace/dist..."
    mkdir -p /workspace/dist
    cp -v /tmp/libqdataschema_*.deb /workspace/dist/

    echo "===> ccache stats:"
    ccache -s
  '

echo "===> Сборка завершена успешно! Пакет: $WORKSPACE_DIR/dist/libqdataschema_*.deb"
