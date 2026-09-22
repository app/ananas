#!/usr/bin/env bash
# Build with CMake and run the designer checks under Xvfb:
#   1. the form-designer wrapper open/save round-trip on a form from the
#      inventory scheme;
#   2. the designer application starts (MainForm construction) via --help.
#
# Container engine: podman (preferred) or docker; override with CONTAINER=...
#
# Usage: smoke-designer.sh [path-to-ananas]
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${1:-$SCRIPT_DIR/../..}"
IMAGE="${ANANAS_IMAGE:-ananas-qt6-builder}"
FIXTURE_FORM="${DESIGNER_FIXTURE_FORM:-406}"

if [[ ! -d "$REPO" ]]; then
    echo "Repository not found: $REPO" >&2
    exit 1
fi
REPO="$(cd "$REPO" && pwd)"
FIXTURE_DIR="$REPO/tmp/designer-fixtures"
FIXTURE_UI="$FIXTURE_DIR/inventory-form-$FIXTURE_FORM.ui"

CONTAINER="${CONTAINER:-$(command -v podman || command -v docker || true)}"
if [[ -z "$CONTAINER" ]]; then
    echo "ERROR: podman or docker is required" >&2
    exit 1
fi

echo "===> Extracting fixture form $FIXTURE_FORM..."
mkdir -p "$FIXTURE_DIR"
python3 "$SCRIPT_DIR/extract-cfg-form.py" \
    "$REPO/applications/inventory/inventory.cfg" \
    "$FIXTURE_FORM" "$FIXTURE_UI"

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

        export QT_PLUGIN_PATH=/repo/lib
        export LD_LIBRARY_PATH=/repo/lib:/repo/lib/designer:/repo/src/editor

        echo "===> Running the wrapper smoke (Xvfb)..."
        timeout 120 xvfb-run -a ./bin/designer-smoke \
            /repo/tmp/designer-fixtures/inventory-form-'"$FIXTURE_FORM"'.ui

        echo "===> Running ananas-designer --help (Xvfb)..."
        out=$(timeout 120 xvfb-run -a ./bin/ananas-designer --help 2>&1 || true)
        if echo "$out" | grep -q "Usage: ananas-designer"; then
            echo "     designer start ok"
        else
            echo "DESIGNER START FAILED"; echo "$out" | tail -30; exit 1
        fi

        echo "===> ccache stats:"
        ccache -s | head -8
    '
