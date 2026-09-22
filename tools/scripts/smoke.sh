#!/usr/bin/env bash
# Build the whole tree with CMake in the Qt6 image and run the QtTest suite
# headlessly under Xvfb.
#
# Container engine: podman (preferred) or docker; override with CONTAINER=...
#
# Usage: smoke.sh [path-to-ananas]
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${1:-$SCRIPT_DIR/../..}"
IMAGE="${ANANAS_IMAGE:-ananas-qt6-builder}"

if [[ ! -d "$REPO" ]]; then
    echo "Repository not found: $REPO" >&2
    exit 1
fi
REPO="$(cd "$REPO" && pwd)"

CONTAINER="${CONTAINER:-$(command -v podman || command -v docker || true)}"
if [[ -z "$CONTAINER" ]]; then
    echo "ERROR: podman or docker is required" >&2
    exit 1
fi

if ! "$CONTAINER" image inspect "$IMAGE" >/dev/null 2>&1; then
    echo "===> Building the Qt6 image..."
    "$CONTAINER" build -t "$IMAGE" -f "$REPO/tools/docker/Containerfile" "$REPO"
fi

"$CONTAINER" run --rm \
    -v "$REPO":/repo:z \
    -w /repo \
    "$IMAGE" \
    bash -c '
        set -e
        export CCACHE_DIR=/repo/tmp/ccache
        mkdir -p "$CCACHE_DIR"

        echo "===> Building the project (CMake)..."
        rm -rf cmake-build
        cmake -S . -B cmake-build -DCMAKE_BUILD_TYPE=Release
        cmake --build cmake-build -j"$(nproc)"

        echo "===> Running ananas-test (Xvfb)..."
        export LD_LIBRARY_PATH=/repo/lib:/repo/lib/designer:/repo/src/editor
        export QT_PLUGIN_PATH=/repo/lib
        xvfb-run -a ./bin/ananas-test

        echo "===> ccache stats:"
        ccache -s
    '
